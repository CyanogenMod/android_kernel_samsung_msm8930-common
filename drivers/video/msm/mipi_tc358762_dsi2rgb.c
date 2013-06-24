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
#include <linux/lcd.h>
#include <linux/leds.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include <linux/gpio.h>
#include "mipi_tc358762_dsi2rgb.h"

static struct  mipi_tc358762_driver_data msd;
static unsigned int recovery_boot_mode;

#define WA_FOR_FACTORY_MODE
struct mutex dsi_tx_mutex;

/* DSI PPI Layer Registers */
#define PPI_STARTPPI	0x0104	/* START control bit of PPI-TX function. */
#define PPI_BUSYPPI	0x0108	/* Bit for PPI Busy. */
#define PPI_LINEINITCNT	0x0110	/* Line Initialization Wait Counter  */
#define PPI_LPTXTIMCNT	0x0114	/* Line Initialization Wait Counter*/
#define PPI_LANEENABLE	0x0134	/* Enables each lane at the PPI layer. */
#define PPI_TX_RX_TA	0x013C	/* DSI Bus Turn Around timing parameters */

/* Analog timer function enable */
#define PPI_CLS_ATMR	0x0140	/* Delay for Clock Lane in LPRX  */
#define PPI_D0S_ATMR	0x0144	/* Delay for Data Lane 0 in LPRX */
#define PPI_D1S_ATMR	0x0148	/* Delay for Data Lane 1 in LPRX */
#define PPI_D0S_CLRSIPOCOUNT	0x0164	/* For lane 0 */
#define PPI_D1S_CLRSIPOCOUNT	0x0168	/* For lane 1 */

#define DSI_STARTDSI	0x0204	/* START control bit of DSI-TX function */
#define DSI_LANEENABLE	0x0210	/* Enables each lane at the Protocol layer. */
#define DSI_LANESTATUS0	0x0214	/* Displays lane is in HS RX mode. */
#define DSI_LANESTATUS1	0x0218	/* Displays lane is in ULPS or STOP state */

#define APLCTRL	0x0400	/* Application Layer Control */
#define RDPKTLN	0x0410	/* Command Read Packet Length */
#define PXLFMT	0x0414	/* RGB Pixel Format in Video Data */
#define MEMWRCMD	0x0418	/* Memory Write Command*/
#define DBICTRL	0x0440	/* Controls various features of DBI-B.*/
#define LCDCTRL	0x0420	/* Controls various features of LCDC.*/
#define HTIM1	0x0424	/* Horizontal Timing Control 1 */
#define HTIM2	0x0428	/* Horizontal Timing Control 2 */
#define VTIM1	0x042C	/* Vertical Timing Control 1 */
#define VTIM2	0x0430	/* Vertical Timing Control 2 */
#define VFUEN	0x0434	/* Video Frame Timing Update Enable */
#define SPICTRL	0x0450	/* SPI control register */
#define SPITCR1	0x0454	/* SPI timing control register */
#define SYSPMCTRL	0x047C	/* System Power Management Control Register*/
#define SYSCTRLL		0x0464	/* System Control Register */
#define APLCNTL	0x0400	/* Application Layer Control Register */
#define SYSPLL1	0x0468	/* System PLL Control Register 1*/
#define SYSPLL3	0x0470	/* System PLL Control Register 3*/
#define  WCMDQ	0x0500	/* Write Command Queue */


/* Chip ID and Revision ID Register */
#define IDREG		0x04A0	/* TC358762 Chip ID Register */

#define TC358762XBG_ID	0x00006200	/* TC358762 Chip ID and Revision ID*/
#define BTASTA	0x0278	/* BTA set*/
#define BTACLR	0x027C	/* BTA clr */

/**
 * Command payload for DTYPE_GEN_LWRITE (0x29) / DTYPE_GEN_READ2 (0x24).
 */
struct wr_cmd_payload {
	u16 addr;
	u32 data;
} __packed;

 /**
 * Write a bridge register
 *
 * @param mfd
 *
 * @return int
 */
static int mipi_d2r_write_reg(struct msm_fb_data_type *mfd, u16 reg, u32 data)
{
	struct wr_cmd_payload payload;
	int rc=0;
	struct dsi_cmd_desc cmd_write_reg = {
		DTYPE_GEN_LWRITE, 1, 0, 0, 0,
			sizeof(payload), (char *)&payload};

	payload.addr = reg;
	payload.data = data;

	mipi_dsi_buf_init(&msd.tc358762_tx_buf);
	/* mutex had been acquired at mipi_dsi_on */
	rc=mipi_dsi_cmds_tx(&msd.tc358762_tx_buf, &cmd_write_reg, 1);
	if (rc<1){
		pr_info("Command Sending failed");
		pr_info("%s:fail reg=0x%x. data=0x%x.\n", __func__, reg, data);
		}

	pr_debug("%s: reg=0x%x. data=0x%x.\n", __func__, reg, data);

	return 0;
}
/**
 * Init the D2R bridge via the DSI interface for Video.
 *
 * @param mfd
 *
 * @return int
 */
static int mipi_d2r_dsi_init_sequence(struct msm_fb_data_type *mfd)
{
	mipi_d2r_write_reg(mfd, DSI_LANEENABLE, 0x00000007);
	mipi_d2r_write_reg(mfd, PPI_D0S_CLRSIPOCOUNT, 0x00000001);
	mipi_d2r_write_reg(mfd, PPI_D1S_CLRSIPOCOUNT, 0x00000001);
	mipi_d2r_write_reg(mfd, PPI_D0S_ATMR, 0x00000000);
	mipi_d2r_write_reg(mfd, PPI_D1S_ATMR, 0x00000000);
	mipi_d2r_write_reg(mfd, PPI_LPTXTIMCNT, 0x00000001);

	mipi_d2r_write_reg(mfd, SPICTRL, 0x00000003);
	mipi_d2r_write_reg(mfd, SPITCR1, 0x00000122);

	mipi_d2r_write_reg(mfd, LCDCTRL, 0x00000120);
	mipi_d2r_write_reg(mfd, SYSCTRLL, 0x0000010F);
	mipi_d2r_write_reg(mfd, MEMWRCMD, 0x0000003C);
	mipi_d2r_write_reg(mfd, PXLFMT, 0x00000055);
	mipi_d2r_write_reg(mfd, DBICTRL, 0x00000200);

	mipi_d2r_write_reg(mfd, PPI_STARTPPI, 0x00000001);
	mipi_d2r_write_reg(mfd, DSI_STARTDSI, 0x00000001);

	return 0;
}
static int mipi_tc358762_disp_send_cmd(struct msm_fb_data_type *mfd,
				       enum mipi_tc358762_cmd_list cmd,
				       unsigned char lock)
{
	struct dsi_cmd_desc *cmd_desc;
	struct dcs_cmd_req cmdreq;
	int cmd_size = 0;

	pr_info("%s : %s\n", __func__, msd.mpd->panel_name);
	pr_info("%s cmd = 0x%x\n", __func__, cmd);


		if (lock)
			mutex_lock(&mfd->dma->ov_mutex);
		switch (cmd) {

		case PANEL_ON:
			cmd_desc = msd.mpd->on.cmd;
			cmd_size = msd.mpd->on.size;
			break;

		case PANEL_OFF:
			cmd_desc = msd.mpd->off.cmd;
			cmd_size = msd.mpd->off.size;
			break;

		default:
			pr_info("%s UNKNOW CMD", __func__);
			goto unknown_command;
			;
	}


	if (!cmd_size)
		goto unknown_command;

	cmdreq.cmds = cmd_desc;
	cmdreq.cmds_cnt = cmd_size;

	cmdreq.flags = CMD_REQ_COMMIT;

	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);

		if (lock)
			mutex_unlock(&mfd->dma->ov_mutex);
		return 0;

unknown_command:
		if (lock)
			mutex_unlock(&mfd->dma->ov_mutex);
	return 0;
}

/**
 * LCD ON.
 **/
static int mipi_tc358762_disp_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	int ret = 0;

	int disp_bl_cont_gpio;
	disp_bl_cont_gpio = DISP_BL_EN_GPIO;

	pr_info("****** %s *******\n", __func__);

	mfd = platform_get_drvdata(pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	mipi = &mfd->panel_info.mipi;

	ret = mipi_d2r_dsi_init_sequence(mfd);
	if (ret) {
		pr_err("mipi_d2r_panel_initialization failed");
		return ret;
	}

	mipi_tc358762_disp_send_cmd(mfd, PANEL_ON, false);
	pr_debug("%s: DISP_BL_CONT_GPIO High\n", __func__);
	gpio_set_value(disp_bl_cont_gpio , 1);

	pr_info("%s:Display on completed\n", __func__);
	return 0;
}
static int mipi_tc358762_disp_off(struct platform_device *pdev)
{
	int disp_bl_cont_gpio;
	struct msm_fb_data_type *mfd;

	disp_bl_cont_gpio =  DISP_BL_EN_GPIO;
	mfd = platform_get_drvdata(pdev);

	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;
	pr_debug("%s: DISP_BL_CONT_GPIO low\n", __func__);
	gpio_set_value(disp_bl_cont_gpio, 0);

	pr_info("%s:Display off completed\n", __func__);

	return 0;
}

static void __devinit mipi_tc358762_disp_shutdown(struct platform_device *pdev)
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

static void mipi_tc358762_disp_set_backlight(struct msm_fb_data_type *mfd)
{
	struct mipi_panel_info *mipi;
	static int bl_level_old;

	pr_info("%s : level (%d)\n", __func__, mfd->bl_level);

	mipi  = &mfd->panel_info.mipi;
	if (bl_level_old == mfd->bl_level)
		goto end;
	if (!mfd->panel_power_on)
		goto end;

	bl_level_old = mfd->bl_level;

end:
	return;
}

#if defined(CONFIG_HAS_EARLYSUSPEND)
static void mipi_tc358762_disp_early_suspend(struct early_suspend *h)
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

static void mipi_tc358762_disp_late_resume(struct early_suspend *h)
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

#if defined(CONFIG_LCD_CLASS_DEVICE)
#ifdef WA_FOR_FACTORY_MODE
static ssize_t mipi_tc358762_disp_get_power(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct msm_fb_data_type *mfd;
	int rc;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n", mfd->panel_power_on);
	pr_info("mipi_tc358762_disp_get_power(%d)\n", mfd->panel_power_on);

	return rc;
}

static ssize_t mipi_tc358762_disp_set_power(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;
	unsigned int power;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sscanf(buf, "%u", &power) != 1)
		return -EINVAL;

	if (power == mfd->panel_power_on)
		return 0;

	if (power) {
		mfd->fbi->fbops->fb_blank(FB_BLANK_UNBLANK, mfd->fbi);
		mfd->fbi->fbops->fb_pan_display(&mfd->fbi->var, mfd->fbi);
		/*mipi_tc358762_disp_send_cmd(mfd, PANEL_LATE_ON, true);*/
		mipi_tc358762_disp_set_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("mipi_tc358762_disp_set_power\n");

	return size;
}
#else
static int mipi_tc358762_disp_get_power(struct lcd_device *dev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);
	if (unlikely(!mfd))
		return -ENODEV;
	if (unlikely(mfd->key != MFD_KEY))
		return -EINVAL;

	pr_info("mipi_tc358762_disp_get_power(%d)\n", mfd->panel_power_on);

	return mfd->panel_power_on;
}

static int mipi_tc358762_disp_set_power(struct lcd_device *dev, int power)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(msd.msm_pdev);

	if (power == mfd->panel_power_on)
		return 0;

	if (power) {
		mfd->fbi->fbops->fb_blank(FB_BLANK_UNBLANK, mfd->fbi);
		mfd->fbi->fbops->fb_pan_display(&mfd->fbi->var, mfd->fbi);
		/*mipi_tc358762_disp_send_cmd(mfd, PANEL_LATE_ON, true);*/
		mipi_tc358762_disp_set_backlight(mfd);
	} else {
		mfd->fbi->fbops->fb_blank(FB_BLANK_POWERDOWN, mfd->fbi);
	}

	pr_info("mipi_tc358762_disp_set_power\n");
	return 0;
}
#endif

static ssize_t mipi_tc358762_lcdtype_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char temp[20];

	snprintf(temp, strnlen(msd.mpd->panel_name, 20) + 1,
						msd.mpd->panel_name);
	strlcat(buf, temp, 20);
	return strnlen(buf, 20);
}

static ssize_t mipi_tc358762_auto_brightness_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int rc;

	rc = snprintf((char *)buf, sizeof(buf), "%d\n",
					msd.dstat.auto_brightness);
	pr_info("auot_brightness: %d\n", *buf);

	return rc;
}

static ssize_t mipi_tc358762_auto_brightness_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(msd.msm_pdev);

	if (sysfs_streq(buf, "0"))
		msd.dstat.auto_brightness = 0;
	else if (sysfs_streq(buf, "1"))
		msd.dstat.auto_brightness = 1;
	else if (sysfs_streq(buf, "2"))
		msd.dstat.auto_brightness = 2;
	else if (sysfs_streq(buf, "3"))
		msd.dstat.auto_brightness = 3;
	else if (sysfs_streq(buf, "4"))
		msd.dstat.auto_brightness = 4;
	else if (sysfs_streq(buf, "5"))
		msd.dstat.auto_brightness = 5;
	else if (sysfs_streq(buf, "6"))
		msd.dstat.auto_brightness = 6;
	else
		pr_info("%s: Invalid argument!!", __func__);

	pr_info("%s : level (%d), bl_level (%d) \n",
		__func__, msd.dstat.auto_brightness, mfd->bl_level);
	return size;
}


static struct lcd_ops mipi_tc358762_disp_props = {
#ifdef WA_FOR_FACTORY_MODE
	.get_power = NULL,
	.set_power = NULL,
#else
	.get_power = mipi_tc358762_disp_get_power,
	.set_power = mipi_tc358762_disp_set_power,
#endif
};

#ifdef WA_FOR_FACTORY_MODE
static DEVICE_ATTR(lcd_power, S_IRUGO | S_IWUSR,
			mipi_tc358762_disp_get_power,
			mipi_tc358762_disp_set_power);
#endif
static DEVICE_ATTR(lcd_type, S_IRUGO, mipi_tc358762_lcdtype_show, NULL);
static DEVICE_ATTR(auto_brightness, S_IRUGO | S_IWUSR | S_IWGRP,
			mipi_tc358762_auto_brightness_show,
			mipi_tc358762_auto_brightness_store);
#endif


static int __devinit mipi_tc358762_disp_probe(struct platform_device *pdev)
{
	struct platform_device *msm_fb_added_dev;
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct lcd_device *lcd_device;
	int ret;
#endif
#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	struct backlight_device *bd;
#endif


	if (pdev->id == 0) {
		msd.mipi_tc358762_disp_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_added_dev = msm_fb_add_device(pdev);

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	msd.msm_pdev = msm_fb_added_dev;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	msd.early_suspend.suspend = mipi_tc358762_disp_early_suspend;
	msd.early_suspend.resume = mipi_tc358762_disp_late_resume;
	msd.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	register_early_suspend(&msd.early_suspend);

#endif

#if defined(CONFIG_LCD_CLASS_DEVICE)
	lcd_device = lcd_device_register("panel", &pdev->dev, NULL,
					&mipi_tc358762_disp_props);

	if (IS_ERR(lcd_device)) {
		ret = PTR_ERR(lcd_device);
		printk(KERN_ERR "lcd : failed to register device\n");
		return ret;
	}

#ifdef WA_FOR_FACTORY_MODE
	sysfs_remove_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_power.attr);

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_power.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_power.attr.name);
	}
#endif

	ret = sysfs_create_file(&lcd_device->dev.kobj,
					&dev_attr_lcd_type.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_lcd_type.attr.name);
	}

#if defined(CONFIG_BACKLIGHT_CLASS_DEVICE)
	bd = backlight_device_register("panel", &lcd_device->dev,
						NULL, NULL, NULL);
	if (IS_ERR(bd)) {
		ret = PTR_ERR(bd);
		pr_info("backlight : failed to register device\n");
		return ret;
	}

	ret = sysfs_create_file(&bd->dev.kobj,
					&dev_attr_auto_brightness.attr);
	if (ret) {
		pr_info("sysfs create fail-%s\n",
				dev_attr_auto_brightness.attr.name);
	}
#endif
#endif


	pr_info("%s:Display probe completed\n", __func__);
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_tc358762_disp_probe,
	.driver = {
		.name   = "mipi_toshiba_tc358762",
	},
	.shutdown = mipi_tc358762_disp_shutdown
};

static struct msm_fb_panel_data tc358762_panel_data = {
	.on		= mipi_tc358762_disp_on,
	.off		= mipi_tc358762_disp_off,
	.set_backlight	= mipi_tc358762_disp_set_backlight,
};

static int ch_used[3];

int mipi_tc358762_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd)
{
	struct platform_device *pdev = NULL;
	int ret = 0;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_toshiba_tc358762",
					   (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	tc358762_panel_data.panel_info = *pinfo;
	msd.mpd = mpd;
	if (!msd.mpd) {
		printk(KERN_ERR
		  "%s: get mipi_panel_data failed!\n", __func__);
		goto err_device_put;
	}
	mpd->msd = &msd;
	ret = platform_device_add_data(pdev, &tc358762_panel_data,
		sizeof(tc358762_panel_data));
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

static int __init mipi_tc358762_disp_init(void)
{
	mipi_dsi_buf_alloc(&msd.tc358762_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&msd.tc358762_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
module_init(mipi_tc358762_disp_init);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Toshiba tc358762 LCD driver");
MODULE_AUTHOR("Ashish Kumar Singh <singh.ashish@samsung.com>");
