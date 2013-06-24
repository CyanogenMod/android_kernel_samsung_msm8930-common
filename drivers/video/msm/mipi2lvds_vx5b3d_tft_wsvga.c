/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#define DEBUG	1
#include <linux/lcd.h>
#include <linux/leds.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include "mipi2lvds_vx5b3d_tft_wsvga.h"
#include <linux/delay.h>
#include <linux/pwm.h>
#include <linux/gpio.h>
#include <mach/msm8930-gpio.h>


static struct mipi_dsi2lvds_driver_data msd;
static unsigned int recovery_boot_mode;
extern unsigned int system_rev;
static int is_lcd_on;
static struct class *mipi2lvds_dnie_class;
static struct device *mipi2lvds_tune_dev;


enum {
    LCD_STATUS_OFF = 0,
    LCD_STATUS_ON,
};

#define WA_FOR_FACTORY_MODE
#define READ_MTP_ONCE

#define DEFAULT_USLEEP 5

struct i2c_gpio {
	unsigned int scl;
	unsigned int sda;
};

static struct i2c_gpio gpio_i2c = {
		.scl	= 7,
		.sda	= 6,
};

#define GEN_QL_CSR_WRITE  {\
	START_BYTE, \
	CONTROL_BYTE_GEN, \
	0x29,  /* Data ID */\
	0x05,  /* Vendor Id 1 */\
	0x01,  /* Vendor Id 2 */\
	0x40,  /* Vendor Unique Command */\
        0x00,  /* Address LS */\
        0x00,  /* Address MS */\
        0x00,  /* data LS */\
	0x00, \
	0x00, \
        0x00,  /* data MS */\
}

#define GEN_QL_CSR_OFFSET_LENGTH  {\
	START_BYTE, \
	CONTROL_BYTE_GEN, \
        0x29,  /* Data ID */\
        0x05,  /* Vendor Id 1 */\
        0x01,  /* Vendor Id 2 */\
        0x41,  /* Vendor Unique Command */\
        0x00,  /* Address LS */\
        0x00,  /* Address MS */\
        0x00,  /* Length LS */\
        0x00,  /* Length MS */\
    }

#define GPIO_I2C_SCL_HIGH		gpio_set_value(gpio_i2c.scl, 1);
#define GPIO_I2C_SCL_LOW		gpio_set_value(gpio_i2c.scl, 0);
#define GPIO_I2C_SDA_HIGH		gpio_set_value(gpio_i2c.sda, 1);
#define GPIO_I2C_SDA_LOW		gpio_set_value(gpio_i2c.sda, 0);


#define GPIO_I2C_DELAY 1

#define START_BYTE       (0xc8)
#define START_READ_BYTE       (0xc9)
#define CONTROL_BYTE_GEN       (0x09u)

static void SCLH_SDAH(uint32_t delay)
{
	GPIO_I2C_SCL_HIGH;
	GPIO_I2C_SDA_HIGH;
	udelay(delay);
}

static void SCLH_SDAL(uint32_t delay)
{
	GPIO_I2C_SCL_HIGH;
	GPIO_I2C_SDA_LOW;
	udelay(delay);
}

static void SCLL_SDAH(uint32_t delay)
{
	GPIO_I2C_SCL_LOW;
	GPIO_I2C_SDA_HIGH;
	udelay(delay);
}

static void SCLL_SDAL(uint32_t delay)
{
	GPIO_I2C_SCL_LOW;
	GPIO_I2C_SDA_LOW;
	udelay(delay);
}

static void GPIO_I2C_LOW(uint32_t delay)
{
	SCLL_SDAL(delay);
	SCLH_SDAL(delay);
	SCLH_SDAL(delay);
	SCLL_SDAL(delay);
	}
	
static void GPIO_I2C_HIGH(uint32_t delay)
{
	SCLL_SDAH(delay);
	SCLH_SDAH(delay);
	SCLH_SDAH(delay);
	SCLL_SDAH(delay);
}

static void GPIO_I2C_START(uint32_t delay)
{
	SCLH_SDAH(5);
	SCLH_SDAL(5);
	udelay(5);
	SCLL_SDAL(30);
}


static void GPIO_I2C_END(uint32_t delay)
{
	SCLL_SDAH(50);
	SCLL_SDAL(5);
	SCLH_SDAL(5);
	SCLH_SDAH(5);
	udelay(30);
}

static void GPIO_I2C_ACK(uint32_t delay)
{
	uint32_t ack;

	GPIO_I2C_SCL_LOW;
	udelay(delay);

	/* SDA -> Input */
	gpio_tlmm_config(GPIO_CFG(gpio_i2c.sda, 0, GPIO_CFG_INPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_DISABLE); 
	gpio_direction_input(gpio_i2c.sda);

	GPIO_I2C_SCL_HIGH;
	udelay(delay);
	ack = gpio_get_value(gpio_i2c.sda); 
	GPIO_I2C_SCL_HIGH;
	udelay(delay);

	/* SDA -> Onput Low */
	gpio_tlmm_config(GPIO_CFG(gpio_i2c.sda, 0, GPIO_CFG_OUTPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_DISABLE); 
	gpio_set_value(gpio_i2c.sda, 0);
	udelay(delay);

	GPIO_I2C_SCL_LOW;
	udelay(delay);

	if (ack)
		printk("FUEL I2C  -> No ACK\n");

	}

void lvds_i2c_init(void)
{
	gpio_tlmm_config(GPIO_CFG(gpio_i2c.scl, 0, GPIO_CFG_OUTPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_ENABLE); 
	gpio_tlmm_config(GPIO_CFG(gpio_i2c.sda, 0, GPIO_CFG_OUTPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_ENABLE); 

	gpio_set_value(gpio_i2c.scl, 1);
	gpio_set_value(gpio_i2c.sda, 1);

}

void lvds_i2c_burst_write(uint8_t *Data, uint8_t length)
{
	uint32_t i, j;

	GPIO_I2C_START(GPIO_I2C_DELAY);

	for(j=0; j<length; j++)
	{
		for (i = 8; i > 0; i--) {
			if ((*Data >> (i - 1)) & 0x1){
				GPIO_I2C_HIGH(GPIO_I2C_DELAY);
				}
			else{
				GPIO_I2C_LOW(GPIO_I2C_DELAY);
				}
		}
		Data++;

		GPIO_I2C_ACK(GPIO_I2C_DELAY);

	}
	
	GPIO_I2C_END(GPIO_I2C_DELAY);

	}

void WriteRegister(u16 addr, u32 w_data)
{
	char buf[] = GEN_QL_CSR_WRITE;

	buf[6] = (uint8_t)addr & 0xff;  /* Address LS */
	buf[7] = (uint8_t)(addr >> 8) & 0xff;	/* Address MS */

	buf[8] = w_data & 0xff;
	buf[9] = (w_data >> 8) & 0xff;
	buf[10] = (w_data >> 16) & 0xff;
	buf[11] =(w_data >> 24) & 0xff;

	lvds_i2c_burst_write((char *)(&buf[0]), (uint8_t)12);

	}
	

static int first_boot = 1;

static void send_i2c_lvds_data(void)
{

	lvds_i2c_init();

	WriteRegister(0x700, 0x6C900040);

	
	WriteRegister(0x704, 0x30438);

	WriteRegister(0x70C, 0x00004604);
	WriteRegister(0x710, 0x54D004B);
	WriteRegister(0x714, 0x20);
	WriteRegister(0x718, 0x00000102);
	WriteRegister(0x71C, 0xA8002F);
	WriteRegister(0x720, 0x0);
	
	WriteRegister(0x154, 0x00000000);
	WriteRegister(0x154, 0x80000000);
	msleep(1);
	WriteRegister(0x158, 0x0);
	WriteRegister(0x158, 0x1);
	msleep(1);
	WriteRegister(0x700, 0x6C900840);
	WriteRegister(0x70C, 0x5E56/*0x5646*/);
	WriteRegister(0x718, 0x00000202);
	
	
	WriteRegister(0x154, 0x00000000);	
	WriteRegister(0x154, 0x80000000);
	WriteRegister(0x158, 0x0);
	WriteRegister(0x158, 0x1);
	msleep(1); /* For pll locking */
	WriteRegister(0x37C, 0x00001063);
	WriteRegister(0x380, 0x82A86030);
	WriteRegister(0x384, 0x2861408B);
	WriteRegister(0x388, 0x00130285);
	WriteRegister(0x38C, 0x10630009);
	WriteRegister(0x394, 0x400B82A8);
	WriteRegister(0x600, 0x16CC78C);
	WriteRegister(0x604, 0x3FFFFFE0);
	WriteRegister(0x608, 0xD8C);

	WriteRegister(0x154, 0x00000000);
	WriteRegister(0x154, 0x80000000);
	WriteRegister(0x158, 0x0);
	WriteRegister(0x158, 0x1);
	msleep(1);

	/* ...move for system reset command (0x158)*/
	WriteRegister(0x120, 0x5);
	WriteRegister(0x124, 0x4D2C400);

	WriteRegister(0x128, 0x104010);
	WriteRegister(0x12C, 0x8D);

	WriteRegister(0x130, 0x3C18);
	WriteRegister(0x134, 0x15);
	WriteRegister(0x138, 0xFF8000);
	WriteRegister(0x13C, 0x0);


	/*PWM  100 % duty ration*/

	WriteRegister(0x114, 0xc6302);
	/*backlight duty ration control when device is first bring up.*/
	WriteRegister(0x160, 0xff);

	/*n nnnedietd to ddnnedfixnnnnednneded*/
	if(first_boot == 1)
	{
		WriteRegister(0x164, 0x4c);
		first_boot=0;
	}

	WriteRegister(0x138, 0x3fff0000);
	
	WriteRegister(0x15c, 0x5);
	/* END...*/
	WriteRegister(0x140, 0x10000);
	/*Add for power consumtion*/
	WriteRegister(0x174, 0xff);
	
	/*end*/

	/*
	slope = 2 / variance = 0x55550022
	slope register [15,10]
	*/
	WriteRegister( 0x404, 0x55550822);

	/*
	To minimize the text effect 
	this value from 0xa to 0xf
	*/
	WriteRegister(0x418, 0x555502ff);

	/* 
	Disable brightnes issue Caused by IBC
	read 4 bytes from address 0x410 to 0x413
	0x15E50300 is read value for 0x410 register
	0x5E50300= 0x15E50300 & 0xefffffff
	 */
	WriteRegister(0x410, 0x5E50300);
	/*...end*/
	WriteRegister(0x20C, 0x124);
	WriteRegister(0x21C, 0x780);

	WriteRegister(0x224, 0x7);

	WriteRegister(0x228, 0x50001);
	WriteRegister(0x22C, 0xFF03);
	WriteRegister(0x230, 0x1);
	WriteRegister(0x234, 0xCA033E10);
	WriteRegister(0x238, 0x00000060);
	WriteRegister(0x23C, 0x82E86030);
	WriteRegister(0x244, 0x001E0285);
	WriteRegister(0x258, 0x30013);

	/*vee strenght initialization*/
	WriteRegister(0x400, 0x0);

	WriteRegister(0x154, 0x00000000);
	WriteRegister(0x154, 0x80000000);
	msleep(1);
	WriteRegister(0x158, 0x0);
	WriteRegister(0x158, 0x1);
	msleep(1); /* For pll locking */	

	printk("sent i2c data\n");
}





/**
 * LCD ON.
 *
 * Set LCD On via MIPI interface .
 * Set Backlight on.*/


static int mipi2lvds_disp_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	pr_info("****** %s *******\n", __func__);

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	mipi = &mfd->panel_info.mipi;

	if (mipi->mode == DSI_VIDEO_MODE){
			send_i2c_lvds_data();
        }

	is_lcd_on = 1;

	pr_info("%s:Display on completed\n", __func__);
return 0;
}

static int mipi2lvds_disp_off(struct platform_device *pdev)
{

	gpio_tlmm_config(GPIO_CFG(47, 0, GPIO_CFG_OUTPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_ENABLE); //LVDS Power enable
	gpio_tlmm_config(GPIO_CFG(2, 0, GPIO_CFG_OUTPUT,
						GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	GPIO_CFG_ENABLE);  //LCD Enable
	gpio_set_value(47, 0);
	gpio_set_value(2, 0);

	pr_info("%s:Display off completed\n", __func__);
	is_lcd_on = 0;
	
	return 0;
}

static void __devinit mipi2lvds_disp_shutdown(struct platform_device *pdev)
{
	static struct mipi_dsi_platform_data *mipi_dsi_pdata;

	if (pdev->id != 0)
		return;

	 mipi_dsi_pdata = pdev->dev.platform_data;
	if (mipi_dsi_pdata == NULL) {
		pr_err("LCD Power off failure: No Platform Data\n");
		return;
	}
}

static void mipi2lvds_disp_set_pwm_duty(int level)
{

	int vx5b3d_level = 0;
	u32 vee_strenght = 0;


	/* brightness tuning*/
	if (level > MAX_BRIGHTNESS_LEVEL)
		level = MAX_BRIGHTNESS_LEVEL;

	if (level >= MID_BRIGHTNESS_LEVEL) {
		vx5b3d_level  = (level - MID_BRIGHTNESS_LEVEL) *
		(MAX_BRIGHTNESS_LEVEL - MID_BRIGHTNESS_LEVEL) / (MAX_BRIGHTNESS_LEVEL-MID_BRIGHTNESS_LEVEL) + MID_BRIGHTNESS_LEVEL;
	} else if (level >= LOW_BRIGHTNESS_LEVEL) {
		vx5b3d_level  = (level - LOW_BRIGHTNESS_LEVEL) *
		(MID_BRIGHTNESS_LEVEL - LOW_BRIGHTNESS_LEVEL) / (MID_BRIGHTNESS_LEVEL-LOW_BRIGHTNESS_LEVEL) + LOW_BRIGHTNESS_LEVEL;
	} else if (level >= DIM_BRIGHTNESS_LEVEL) {
		vx5b3d_level  = (level - DIM_BRIGHTNESS_LEVEL) *
		(LOW_BRIGHTNESS_LEVEL - DIM_BRIGHTNESS_LEVEL) / (LOW_BRIGHTNESS_LEVEL-DIM_BRIGHTNESS_LEVEL) + DIM_BRIGHTNESS_LEVEL;
	} else if (level > 0)
		vx5b3d_level  = DIM_BRIGHTNESS_LEVEL;
	else {
		vx5b3d_level = 0;
		pr_info("level = [%d]: vx5b3d_level = [%d]\n",\
			level,vx5b3d_level);	
	}

	vee_strenght = V5D3BX_VEESTRENGHT | (V5D3BX_VEEDEFAULTVAL << 27);
	WriteRegister(0x400,vee_strenght);
	if (vx5b3d_level != 0) {
		WriteRegister(0x164,((vx5b3d_level * V5D3BX_CABCBRIGHTNESSRATIO)/1000));
	}


}

static void mipi2lvds_disp_set_backlight(struct msm_fb_data_type *mfd)
{
	static int bl_level_old;
	pr_info("%s : level (%d)\n", __func__, mfd->bl_level);

	if (bl_level_old == mfd->bl_level)
		return;
	if (!mfd->panel_power_on)
		return;
	
	mipi2lvds_disp_set_pwm_duty(mfd->bl_level);

	return;
}

#if defined(CONFIG_HAS_EARLYSUSPEND)
static void mipi2lvds_disp_early_suspend(struct early_suspend *h)
{
	struct msm_fb_data_type *mfd;
	pr_info("%s", __func__);

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}
	if (unlikely(mfd->key != MFD_KEY)) {
		pr_info("%s MFD_KEY is not matched.\n", __func__);
		return;
	}

	mfd->resume_state = MIPI_SUSPEND_STATE;
	
}

static void mipi2lvds_disp_late_resume(struct early_suspend *h)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd)) {
		pr_info("%s NO PDEV.\n", __func__);
		return;
	}
	if (unlikely(mfd->key != MFD_KEY)) {
		pr_info("%s MFD_KEY is not matched.\n", __func__);
		return;
	}

	mfd->resume_state = MIPI_RESUME_STATE;
	pr_info("%s", __func__);
}
#endif


/* ##########################################################
 * #
 * # Scenario change Sysfs node
 * #
 * ##########################################################*/
static ssize_t scenario_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;
}


static ssize_t scenario_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

}
static DEVICE_ATTR(scenario, 0664, scenario_show, scenario_store);


/* ##########################################################
 * #
 * # MDNIE OVE Sysfs node
 * #
 * ##########################################################*/
static ssize_t outdoor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[MIPI2LVDS]:%s called\n", __func__);
	return 0;

}


static ssize_t outdoor_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

}

static DEVICE_ATTR(outdoor, 0664, outdoor_show, outdoor_store);

/* ##########################################################
 * #
 * # MDNIE CABC Sysfs node
 * #
 * ##########################################################*/
static ssize_t cabc_show(struct device *dev,
		struct device_attribute *attr, char *buf)	
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

	}

static ssize_t cabc_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

}

static DEVICE_ATTR(cabc, 0664, cabc_show, cabc_store);


/* ##########################################################
 * #
 * # MDNIE Accessibility Sysfs node
 * #
 * ##########################################################*/

static ssize_t accessibility_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

}

static ssize_t accessibility_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t size)
{

	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

	}
static DEVICE_ATTR(accessibility, 0664, accessibility_show, accessibility_store);

static ssize_t mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
		return 0;

	}
static ssize_t mode_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[MIPI2LVDS]: %s called\n", __func__);
	return 0;

}


static DEVICE_ATTR(mode, 0664, mode_show, mode_store);



int mipi2lvds_sysfs_init(void)
{

	/*  1. CLASS Create
	 *  2. Device Create
	 *  3. node create
	 *   - bypass on/off node
	 *   - cabc on/off node
	 *   - lcd_power node
	 *   - scenario node
	 *   - mdnie_outdoor node
	 *   - mdnie_bg node*/
	 pr_info("%s:Sysfs init start\n", __func__);
	mipi2lvds_dnie_class = class_create(THIS_MODULE, "mdnie");
	if (IS_ERR(mipi2lvds_dnie_class)) {
		pr_info("Failed to create class(mipi2lvds_dnie_class)!!\n");
	}
	mipi2lvds_tune_dev = device_create(mipi2lvds_dnie_class, NULL, 0, NULL, "mdnie");
		if (IS_ERR(mipi2lvds_tune_dev)) {
			pr_info("Failed to create device(mipi2lvds_tune_dev)!!");
		}

	if (device_create_file(mipi2lvds_tune_dev, &dev_attr_scenario) < 0) {
		pr_info("Failed to create device file!(%s)!\n",\
			dev_attr_scenario.attr.name);
	}
	if (device_create_file(mipi2lvds_tune_dev, &dev_attr_outdoor) < 0) {
		pr_info("[mipi2lvds:ERROR] device_crate_filed(%s)\n",\
			dev_attr_outdoor.attr.name);
	}

	if (device_create_file(mipi2lvds_tune_dev, &dev_attr_mode) < 0) {
		pr_info("[mipi2lvds:ERROR] device_crate_filed(%s)\n",\
			dev_attr_mode.attr.name);
	}

	if (device_create_file(mipi2lvds_tune_dev, &dev_attr_cabc) < 0) {
		pr_info("[mipi2lvds:ERROR] device_create_file(%s)\n",\
			dev_attr_cabc.attr.name);
	}

	if (device_create_file(mipi2lvds_tune_dev, &dev_attr_accessibility) < 0) {
		pr_info("[mipi2lvds:ERROR] device_create_file(%s)\n",\
			dev_attr_accessibility.attr.name);
	}
	pr_info("%s:Sysfs init completed\n", __func__);

	return 0;
	}


static int __devinit mipi2lvds_vx5b3d_disp_probe(struct platform_device *pdev)
{
	struct platform_device *msm_fb_added_dev;
        is_lcd_on = LCD_STATUS_ON;

	if (pdev->id == 0) {
		msd.mipi_dsi2lvds_disp_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_added_dev = msm_fb_add_device(pdev);

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	msd.msm_pdev = msm_fb_added_dev;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	msd.early_suspend.suspend = mipi2lvds_disp_early_suspend;
	msd.early_suspend.resume = mipi2lvds_disp_late_resume;
	msd.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&msd.early_suspend);

#endif
	mipi2lvds_sysfs_init();
	return 0;
	
}

static struct platform_driver this_driver = {
	.probe  = mipi2lvds_vx5b3d_disp_probe,
	.driver = {
		.name   = "mipi2lvds_vx5b3d",
	},
	.shutdown = mipi2lvds_disp_shutdown
};

static struct msm_fb_panel_data vx5b3d_panel_data = {
	.on		= mipi2lvds_disp_on,
	.off		= mipi2lvds_disp_off,
	.set_backlight	= mipi2lvds_disp_set_backlight,
};

static int ch_used[3];

int mipi2lvds_vx5b3d_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd)
{
	struct platform_device *pdev = NULL;
	int ret = 0;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi2lvds_vx5b3d",
					   (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	vx5b3d_panel_data.panel_info = *pinfo;
	msd.mpd = mpd;
	if (!msd.mpd) {
		printk(KERN_ERR
		  "%s: get mipi_panel_data failed!\n", __func__);
		goto err_device_put;
	}
	mpd->msd = &msd;
	ret = platform_device_add_data(pdev, &vx5b3d_panel_data,
		sizeof(vx5b3d_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return ret;

err_device_put:
	platform_device_put(pdev);
	return ret;
}


static int __init current_boot_mode(char *mode)
{
	/*
	*	1 is recovery booting
	*	0 is normal booting
	*/

	if (strncmp(mode, "1", 1) == 0)
		recovery_boot_mode = 1;
	else
		recovery_boot_mode = 0;

	pr_debug("%s %s", __func__, recovery_boot_mode == 1 ?
						"recovery" : "normal");
	return 1;
}
__setup("androidboot.boot_recovery=", current_boot_mode);

static int __init mipi2lvds_disp_init(void)
{
	return platform_driver_register(&this_driver);
}
module_init(mipi2lvds_disp_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Quick Logic VX5B3D LCD driver");
