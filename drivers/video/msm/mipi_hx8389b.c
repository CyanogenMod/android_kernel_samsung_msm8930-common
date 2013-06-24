/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
#define DEBUG

#include <mach/socinfo.h>
#include <linux/lcd.h>
#include "msm_fb.h"
#include <linux/gpio.h>
#include "mipi_dsi.h"
#include "mipi_hx8389b.h"

#if defined(CONFIG_FB_MDP4_ENHANCE)
#include "mdp4_video_enhance.h"
#elif defined(CONFIG_MDNIE_LITE_TUNING)
#include "mdnie_lite_tuning.h"
#endif
#define HX8389B_PANEL_LMS579NF02	0x538890

//static struct msm_panel_common_pdata *mipi_hx8389b_pdata;
static struct dsi_buf hx8389b_tx_buf;
static struct dsi_buf hx8389b_rx_buf;

//static struct mipi_samsung_driver_data msd;

static int mipi_hx8389b_bl_ctrl;

#if defined(CONFIG_BACKLIGHT_IC_KTD3102)
spinlock_t bl_ctrl_lock;
static int lcd_brightness = -1;
#endif

/* common setting */
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};

static char cmd1[] = {
	0xB9,
	0xFF, 0x83, 0x89
};
static char cmd2[] = {
	0xDE,
	0x05, 0x58, 0x02
};
static char cmd3[] = {
	0xB1,
	0x00, 0x00, 0x07, 0xF2, 0x97, 0x10, 0x11, 0x74, 0xF4, 0x36, 0x3A, 0x22, 0x1B, 0x41, 0x00,
	0x3A, 0xF7, 0x20, 0x48
};
static char cmd4[] = {
	0xB2,
	0x00, 0x00, 0x78, 0x0D, 0x06, 0x3F, 0x40
};
static char cmd5[] = {
	0xB4,
	0x09, 0x18, 0x00, 0x32, 0x10, 0x09, 0x32, 0x13, 0xCD, 0x00, 0x00, 0x00, 0x37, 0x0A, 0x40,
	0x0D, 0x37, 0x0A, 0x40, 0x16, 0x50, 0x58, 0x0A
};
static char cmd6[] = {
	0xD5,
	0x00, 0x00, 0x4C, 0x02, 0x03, 0x00, 0x00, 0x00, 0x06, 0x98, 0x88, 0x88, 0x45, 0x67, 0x88,
	0x99, 0xDD, 0x88, 0x01, 0x23, 0x67, 0x45, 0x23, 0x01, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x32, 0x10, 0x99, 0x88, 0xDD, 0x88, 0x76, 0x54, 0x10, 0x32, 0x54, 0x76, 0x88, 0x88,
	0x88, 0x88, 0x88, 
};
static char cmd7[] = {
	0xE0,
	0x00, 0x15, 0x13, 0x1D, 0x23, 0x3D, 0x2A, 0x39, 0x0E, 0x10, 0x11, 0x17, 0x1A, 0x17, 0x17,
	0x08, 0x12, 0x00, 0x15, 0x13, 0x1D, 0x23, 0x3B, 0x2A, 0x39, 0x0E, 0x10, 0x11, 0x17, 0x1A,
	0x17, 0x17, 0x08, 0x12
};
static char cmd8[] = {
	0xC1,
	0x01, 0x02, 0x09, 0x11, 0x1A, 0x1D, 0x28, 0x30, 0x37, 0x40, 0x49, 0x51, 0x59, 0x61, 0x69,
	0x71, 0x7B, 0x81, 0x88, 0x8F, 0x98, 0x9F, 0xA6, 0xAE, 0xB7, 0xBF, 0xC7, 0xCC, 0xD5, 0xDF,
	0xE4, 0xED, 0xF4, 0xF9, 0xF9, 0x42, 0x19, 0xAA, 0x80, 0x2D, 0x7C, 0x33, 0x40, 0x02, 0x09,
	0x11, 0x1A, 0x1D, 0x28, 0x30, 0x37, 0x40, 0x49, 0x51, 0x59, 0x61, 0x69, 0x71, 0x7B, 0x81,
	0x88, 0x8F, 0x98, 0x9F, 0xA6, 0xAE, 0xB7, 0xBF, 0xC7, 0xCC, 0xD5, 0xDF, 0xE4, 0xEF, 0xF4,
	0xFC, 0xFA, 0x41, 0x19, 0xAA, 0x80, 0x2D, 0x7C, 0x33, 0x40, 0x01, 0x07, 0x0C, 0x17, 0x1B,
	0x1E, 0x26, 0x2D, 0x34, 0x3C, 0x43, 0x4A, 0x4F, 0x55, 0x5B, 0x62, 0x67, 0x6F, 0x77, 0x7F,
	0x85, 0x8C, 0x92, 0x9B, 0xA3, 0xA8, 0xB3, 0xBE, 0xC6, 0xCD, 0xD9, 0xEA, 0xF3, 0xF8, 0x95,
	0x98, 0xAA, 0x80, 0x2D, 0x74, 0x33, 0x00
};
static char cmd9[] = {
	0xCC,
	0x0E,
};
static char cmd10[] = {
	0xC0,
	0x43, 0x17
};
static char cmd11[] = {
	0xC6,
	0x14, 0x00
};
static char cmd12[] = {
	0xC9,
	0x0F, 0x02
};
static char cmd13[] = {
	0xC6,
	0x14, 0x00
};
static char cmd14[] = {
	0xC9,
	0x0F, 0x02
};

static char cmd15[] = {
	0x51,
	0xFF
};
static char cmd16[] = {
	0x53,
	0x24
};
static char cmd17[] = {
	0x55,
#if defined(CONFIG_MACH_CRATER_CHN_CTC)
	0x00	/*default CABC OFF*/
#else
	0x01
#endif
};

static struct dsi_cmd_desc hx8389b_video_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd1), cmd1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd2), cmd2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd3), cmd3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd4), cmd4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd5), cmd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd6), cmd6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd7), cmd7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd8), cmd8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd9), cmd9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd10), cmd10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd11), cmd11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd12), cmd12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd13), cmd13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(cmd14), cmd14},
	{DTYPE_DCS_WRITE, 1, 0, 0, 300, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(display_on), display_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd15), cmd15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd16), cmd16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmd17), cmd17},
};

static struct dsi_cmd_desc hx8389b_video_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};
#if defined(CONFIG_FB_MDP4_ENHANCE)
extern int is_negativeMode_on(void);
#endif
static int mipi_hx8389b_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;

	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi  = &mfd->panel_info.mipi;

	if (!mfd->cont_splash_done) {
		mfd->cont_splash_done = 1;
		return 0;
	}
#if defined(CONFIG_FB_MDP4_ENHANCE)
	is_negativeMode_on();
#endif	
	printk(KERN_ERR "[LCD] mipi_hx8389b_lcd_on\n");
	if (mipi->mode == DSI_VIDEO_MODE)
		mipi_dsi_cmds_tx(&hx8389b_tx_buf,
			hx8389b_video_display_on_cmds,
			ARRAY_SIZE(hx8389b_video_display_on_cmds));
#if defined(CONFIG_BACKLIGHT_IC_KTD3102)
	pr_info("%s: DISP_BL_CONT_GPIO High\n", __func__);
	gpio_set_value(DISP_BL_CONT_GPIO, 1);
#endif
	return 0;
}

static int mipi_hx8389b_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi  = &mfd->panel_info.mipi;

	printk(KERN_ERR "[LCD] mipi_hx8389b_lcd_off\n");
	if (mipi->mode == DSI_VIDEO_MODE)
		mipi_dsi_cmds_tx(&hx8389b_tx_buf,
			hx8389b_video_display_off_cmds,
			ARRAY_SIZE(hx8389b_video_display_off_cmds));
#if defined(CONFIG_BACKLIGHT_IC_KTD3102)	
	pr_info("%s: DISP_BL_CONT_GPIO low-block\n", __func__);
	gpio_set_value(DISP_BL_CONT_GPIO, 0);
#endif
	return 0;
}

#if defined(CONFIG_LCD_CLASS_DEVICE)
static struct lcd_ops mipi_lcd_props;
static ssize_t mipi_hx8389b_lcdtype_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char temp[30];
	sprintf(temp, "INH_%x\n", HX8389B_PANEL_LMS579NF02);
	strcat(buf, temp);
	return strlen(buf);
}

static DEVICE_ATTR(lcd_type, S_IRUGO, mipi_hx8389b_lcdtype_show, NULL);
#endif

static ssize_t mipi_hx8389b_wta_bl_ctrl(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = strnlen(buf, PAGE_SIZE);
	int err;

	err =  kstrtoint(buf, 0, &mipi_hx8389b_bl_ctrl);
	if (err)
		return ret;

	pr_info("%s: bl ctrl set to %d\n", __func__, mipi_hx8389b_bl_ctrl);

	return ret;
}

static DEVICE_ATTR(bl_ctrl, S_IWUSR, NULL, mipi_hx8389b_wta_bl_ctrl);

static struct attribute *mipi_hx8389b_fs_attrs[] = {
	&dev_attr_bl_ctrl.attr,
	NULL,
};

static struct attribute_group mipi_hx8389b_fs_attr_group = {
	.attrs = mipi_hx8389b_fs_attrs,
};

static int mipi_hx8389b_create_sysfs(struct platform_device *pdev)
{
	int rc;
	struct msm_fb_data_type *mfd = platform_get_drvdata(pdev);

	if (!mfd) {
		pr_err("%s: mfd not found\n", __func__);
		return -ENODEV;
	}
	if (!mfd->fbi) {
		pr_err("%s: mfd->fbi not found\n", __func__);
		return -ENODEV;
	}
	if (!mfd->fbi->dev) {
		pr_err("%s: mfd->fbi->dev not found\n", __func__);
		return -ENODEV;
	}
	rc = sysfs_create_group(&mfd->fbi->dev->kobj,
		&mipi_hx8389b_fs_attr_group);
	if (rc) {
		pr_err("%s: sysfs group creation failed, rc=%d\n",
			__func__, rc);
		return rc;
	}

	return 0;
}

static int __devinit mipi_hx8389b_lcd_probe(struct platform_device *pdev)
{
	struct platform_device *pthisdev = NULL;
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct lcd_device *lcd_device;
	int ret;
#endif
	pr_debug("%s\n", __func__);

	if (pdev->id == 0) {
#if 0
		mipi_hx8389b_pdata = pdev->dev.platform_data;
		if (mipi_hx8389b_pdata->bl_lock)
			spin_lock_init(&mipi_hx8389b_pdata->bl_spinlock);
#endif
		mipi_hx8389b_bl_ctrl = 1;

		return 0;
	}

	pthisdev = msm_fb_add_device(pdev);
	mipi_hx8389b_create_sysfs(pthisdev);

#if defined(CONFIG_MDNIE_LITE_TUNING) || defined(CONFIG_FB_MDP4_ENHANCE)
	/*	mdnie sysfs create */
	init_mdnie_class();
#endif

#if defined(CONFIG_LCD_CLASS_DEVICE)
	lcd_device = lcd_device_register("panel", &pdev->dev, NULL,
					&mipi_lcd_props);

	if (IS_ERR(lcd_device)) {
		int ret = PTR_ERR(lcd_device);
		printk(KERN_ERR "lcd : failed to register device\n");
		return ret;
	}

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_type.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_type.attr.name);
	}

#endif

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_hx8389b_lcd_probe,
	.driver = {
		.name   = "mipi_hx8389b",
	},
};

#if defined(CONFIG_BACKLIGHT_IC_KTD3102)
#if defined(CONFIG_BL_CTRL_MODE_2)
struct brt_value brt_table_aat[] = {
	{	255 ,	7	},	/*Max 255*/
	{	239 ,	8	},	
	{	223 ,	9	},	
	{	207 ,	10	},	
	{	191 ,	11	},	
	{	175 ,	12	},	
	{	159 ,	13	},	
	{	143 ,	14	},	
	{	127 ,	15	},	/*Default 127*/
	{	114 ,	17	},	
	{	102 ,	19	},	
	{	90	,	21	},	
	{	78	,	23	},	
	{	66	,	25	},	
	{	54	,	27	},	
	{	42	,	29	},	
	{	30	,	31	},	/*Dim 30 Min 10*/
	{	0	,	32	},	/*Off*/
};
#define MAX_BRT_STAGE (int)(sizeof(brt_table_aat)/sizeof(struct brt_value))
static int mipi_hx8389b_set_brightness_level(int level)
{
	int tune_level = 0;

	if (level > 0) {
		if (level < MIN_BRIGHTNESS_VALUE) {
			tune_level = AAT_DIMMING_VALUE; /* DIMMING */
		} else {
			int i;

			for (i = 0; i < MAX_BRT_STAGE; i++) {
				if (level <= brt_table_aat[i].level
					&& level > brt_table_aat[i+1].level) {
					tune_level = brt_table_aat[i].tune_level;
					break;
				}
			}
		}
	} 
	return tune_level;
}
#else

static int lux_tbl[] = {
	2,
	5, 7, 9, 10, 11, 13, 14, 15, 16,17,
	18, 19, 20, 22,	24, 25, 26, 27, 28,	29,
	30, 31, 32, 0,
};

static int mipi_hx8389b_set_brightness_level(int bl_level)
{
	int backlightlevel;
	int cd;
		switch (bl_level) {
 		case 0: 
			backlightlevel = 24; /*0*/
			break;
		case 1 ... 19:
			backlightlevel = 23; /* 32 */
			break;
		case 20 ... 29:
			backlightlevel = 22; /* 31 */
			break;
		case 30 ... 39:
			backlightlevel = 22; /* 31 */
			break;
		case 40 ... 49:
			backlightlevel = 21; /* 30 */
			break;
		case 50 ... 59:
			backlightlevel = 20; /* 29 */
			break;
		case 60 ... 69:
			backlightlevel = 19;  /* 28 */
			break;
		case 70 ... 79:
			backlightlevel = 18;  /* 27 */
			break;
		case 80 ... 89:
			backlightlevel = 17;  /* 26 */
			break;
		case 90 ... 99:
			backlightlevel = 15;  /* 24 */
			break;
		case 100 ... 109:
			backlightlevel = 15;  /* 24 */
			break;
		case 110 ... 119:
			backlightlevel = 14;  /* 20 */
			break;
		case 120 ... 129:
			backlightlevel = 13;  /* 21 */
			break;
		case 130 ... 139:
			backlightlevel = 13;  /* 20 */
			break;
		case 140 ... 149:
			backlightlevel = 11;  /* 18 */
			break;
		case 150 ... 159:
			backlightlevel = 11;  /* 18 */
			break;
		case 160 ... 169:
			backlightlevel = 10;  /* 17 */
			break;
		case 170 ... 179:
			backlightlevel = 9;  /* 16 */
			break;
		case 180 ... 189:
			backlightlevel = 8;  /* 15 */
			break;
		case 190 ... 199:
			backlightlevel = 8;  /* 15 */
			break;
		case 200 ... 209:
			backlightlevel = 6 ;  /* 13 */
			break;
		case 210 ... 214:
			backlightlevel = 5;  /* 11 */
			break;
		case 215 ... 255:
			backlightlevel = 3;  /* 9 */
			break;
#if 0
		case 230 ... 239:
			backlightlevel = 2;  /* 7 */
			break;
		case 240 ... 249:
			backlightlevel = 1;  /* 5 */
			break;
		case 250 ... 255:
			backlightlevel = 0;  /* 2 */
			break;
#endif
		default:
			backlightlevel = 23; /*32*/
			break;
		}

	cd = lux_tbl[backlightlevel];
	return cd;
}
#endif

void mipi_hx8389b_set_backlight(int level)
{

	
	int tune_level = 0;

	spin_lock(&bl_ctrl_lock);	
	tune_level = level;

	//pr_debug("[KTD3102] tune_level : %d, lcd_brightness : %d \n",tune_level,lcd_brightness);	
	if(tune_level != lcd_brightness)
	{
		if (!tune_level) {
			gpio_set_value(DISP_BL_CONT_GPIO, 0);
			mdelay(3);
			lcd_brightness = tune_level;
		} else {
		       int pulse;
			if (unlikely(lcd_brightness < 0)) {
				int val = gpio_get_value(DISP_BL_CONT_GPIO);
				if (val) {
					lcd_brightness = 0;
					gpio_set_value(DISP_BL_CONT_GPIO, 0);
					mdelay(3);
					pr_info("LCD Baklight init in boot time on kernel\n");
				}
			}
			if (!lcd_brightness) {
				gpio_set_value(DISP_BL_CONT_GPIO, 1);
				udelay(3);
				
#if defined(CONFIG_BL_CTRL_MODE_2)
				lcd_brightness = MAX_BRIGHTNESS_IN_BLU;
#else
				lcd_brightness = 1;
#endif
			}

			pulse = (tune_level - lcd_brightness + MAX_BRIGHTNESS_IN_BLU)
							% MAX_BRIGHTNESS_IN_BLU;

			pr_debug("[KTD3102] tune_level: %d, pulse : %d, lcd_brightness : %d \n",tune_level,pulse,lcd_brightness); 

			for (; pulse > 0; pulse--) {
				gpio_set_value(DISP_BL_CONT_GPIO, 0);
				udelay(3);
				gpio_set_value(DISP_BL_CONT_GPIO, 1);
				udelay(3);
			}

			lcd_brightness = tune_level;
		}
	}
	mdelay(1);
	spin_unlock(&bl_ctrl_lock);
}


static void mipi_hx8389b_disp_backlight(struct msm_fb_data_type *mfd)
{
	struct mipi_panel_info *mipi;
#ifndef CONFIG_BACKLIGHT_IC_KTD3102
	struct dcs_cmd_req cmdreq;
#endif	
	static int bl_level_old;

	pr_info("%s Back light level:%d\n", __func__, mfd->bl_level);

	mipi  = &mfd->panel_info.mipi;
	if (bl_level_old == mfd->bl_level)
		goto end;
	if (!mfd->panel_power_on)
		goto end;

#if defined(CONFIG_BACKLIGHT_IC_KTD3102)

	mipi_hx8389b_set_backlight(mipi_hx8389b_set_brightness_level(mfd->bl_level));
	bl_level_old = mfd->bl_level;

#else /*for himax ldi HX8394-A*/
	mutex_lock(&mfd->dma->ov_mutex);
	/* mdp4_dsi_cmd_busy_wait: will turn on dsi clock also */
	mipi_dsi_mdp_busy_wait();

	WRDISBV[1] = (unsigned char)
		(msd.mpd->set_brightness_level(mfd->bl_level));

	cmdreq.cmds = himax_video_backlight_cmds;
	cmdreq.cmds_cnt = ARRAY_SIZE(himax_video_backlight_cmds);
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

	bl_level_old = mfd->bl_level;
	mutex_unlock(&mfd->dma->ov_mutex);
#endif

end:
	return;
	
}
#else


//static int old_bl_level;
 
static void mipi_hx8389b_set_backlight(struct msm_fb_data_type *mfd)
{
#if 0
	int bl_level;
	unsigned long flags;
	bl_level = mfd->bl_level;

	if (mipi_hx8389b_pdata->bl_lock) {
		if (!mipi_hx8389b_bl_ctrl) {
			/* Level received is of range 1 to bl_max,
			   We need to convert the levels to 1
			   to 31 */
			bl_level = (2 * bl_level * 31 + mfd->panel_info.bl_max)
					/(2 * mfd->panel_info.bl_max);
			if (bl_level == old_bl_level)
				return;

			if (bl_level == 0)
				mipi_hx8389b_pdata->backlight(0, 1);

			if (old_bl_level == 0)
				mipi_hx8389b_pdata->backlight(50, 1);

			spin_lock_irqsave(&mipi_hx8389b_pdata->bl_spinlock,
						flags);
			mipi_hx8389b_pdata->backlight(bl_level, 0);
			spin_unlock_irqrestore(&mipi_hx8389b_pdata->bl_spinlock,
						flags);
			old_bl_level = bl_level;
		} else {
			mipi_hx8389b_pdata->backlight(bl_level, 1);
		}
	} else {
		mipi_hx8389b_pdata->backlight(bl_level, mipi_hx8389b_bl_ctrl);
	}
#endif
	printk(KERN_INFO "mipi_hx8389b_set_backlight\n");
}
#endif
static struct msm_fb_panel_data hx8389b_panel_data = {
	.on	= mipi_hx8389b_lcd_on,
	.off = mipi_hx8389b_lcd_off,
	#if defined(CONFIG_BACKLIGHT_IC_KTD3102)
	.set_backlight = mipi_hx8389b_disp_backlight,
	#else
	.set_backlight = mipi_hx8389b_set_backlight,
	#endif
};  

static int ch_used[3];

static int mipi_hx8389b_lcd_init(void)
{
	mipi_dsi_buf_alloc(&hx8389b_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&hx8389b_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
int mipi_hx8389b_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	printk(KERN_ERR "[LCD] mipi_hx8389b_device_register\n");
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_hx8389b_lcd_init();
	if (ret) {
		pr_err("mipi_hx8389b_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_hx8389b", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	hx8389b_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &hx8389b_panel_data,
				sizeof(hx8389b_panel_data));
	if (ret) {
		pr_debug("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_debug("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}
