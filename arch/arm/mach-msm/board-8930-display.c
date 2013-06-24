/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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

#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_memtypes.h>
#include <mach/board.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include <linux/msm_ion.h>
#include <mach/ion.h>

#include "devices.h"
#include "board-8930.h"

#ifdef CONFIG_SAMSUNG_CMC624
#include <linux/i2c/samsung_cmc624.h>
#include <mach/msm8930-gpio.h>
#endif

#if defined(CONFIG_MACH_KS02) || (defined(CONFIG_MACH_LT02) || defined(CONFIG_MACH_LT02_CHN_CTC))
#include <mach/msm8930-gpio.h>
#endif
#if defined(CONFIG_MACH_CRATER_CHN_CTC)
#include <mach/msm8930-gpio.h>
#endif

#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
static struct sec_esd_platform_data esd_pdata;
static struct platform_device samsung_mipi_esd_refresh_device = {
	.name = "samsung_mipi_esd_refresh",
	.id = -1,
	.dev = {
		.platform_data  = &esd_pdata,
	},
};
#endif

#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#if defined(CONFIG_MACH_LT02_CHN_CTC) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (1024 * 600 * 4 * 3)
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (1280 * 736 * 4 * 3)
#elif defined(CONFIG_FB_MSM_MIPI_MAGNA_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (800 * 480 * 4 * 3)
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_CMD_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (544 * 960 * 4 * 3)
#elif  defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (480 * 640* 4 * 3)
#else
#define MSM_FB_PRIM_BUF_SIZE \
		(roundup((1920 * 1088 * 4), 4096) * 3) /* 4 bpp x 3 pages */
#endif
#else
/* prim = 540 x 960 x 4(bpp) x 2(pages) */
#if defined(CONFIG_MACH_LT02_CHN_CTC) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (1024 * 600 * 4 * 2)
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (1280 * 736 * 4 * 2)
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (800 * 480 * 4 * 3)
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_CMD_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (544 * 960 * 4 * 2)
#elif  defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
#define MSM_FB_PRIM_BUF_SIZE (480 * 640 * 4 * 2)
#else
#define MSM_FB_PRIM_BUF_SIZE \
		(roundup((1920 * 1088 * 4), 4096) * 2) /* 4 bpp x 2 pages */
#endif
#endif

/* Note: must be multiple of 4096 */
#define MSM_FB_SIZE roundup(MSM_FB_PRIM_BUF_SIZE, 4096)

#ifdef CONFIG_FB_MSM_OVERLAY0_WRITEBACK
#if defined(CONFIG_MACH_LT02_CHN_CTC) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((1024 * 600 * 3 * 2), 4096)
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((1280 * 736 * 3 * 2), 4096)
#elif defined(CONFIG_FB_MSM_MIPI_MAGNA_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((800 * 480 * 3 * 2), 4096)
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_CMD_QHD_PT_PANEL)\
 || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_QHD_PT_PANEL)\
 || defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL) \
 || defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL)
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((544 * 960 * 3 * 2), 4096)
#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((480 * 640* 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((1376 * 768 * 3 * 2), 4096)
#endif
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY0_WRITEBACK */

#ifdef CONFIG_FB_MSM_OVERLAY1_WRITEBACK
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE roundup((1920 * 1088 * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY1_WRITEBACK_SIZE (0)
#endif  /* CONFIG_FB_MSM_OVERLAY1_WRITEBACK */

#define MDP_VSYNC_GPIO 0

#define MIPI_ILI9341_BOE_VIDEO_QVGA_PANEL_NAME  "mipi_ili9341_boe_vide_qvga"
#define MIPI_VIDEO_NOVATEK_HD_PANEL_NAME "mipi_novatek_tft_video_hd"
#define MIPI_CMD_NOVATEK_QHD_PANEL_NAME	"mipi_cmd_novatek_qhd"
#define MIPI_VIDEO_NOVATEK_QHD_PANEL_NAME	"mipi_video_novatek_qhd"
#define MIPI_VIDEO_TOSHIBA_WSVGA_PANEL_NAME	"mipi_video_toshiba_wsvga"
#define MIPI_VIDEO_SAMSUNG_WSVGA_PANEL_NAME	"mipi_video_samsung_tft_wsvga"
#define MIPI_VIDEO_CHIMEI_WXGA_PANEL_NAME	"mipi_video_chimei_wxga"
#define MIPI_VIDEO_SIMULATOR_VGA_PANEL_NAME	"mipi_video_simulator_vga"
#define MIPI_CMD_RENESAS_FWVGA_PANEL_NAME	"mipi_cmd_renesas_fwvga"
#define MIPI_VIDEO_NT_HD_PANEL_NAME		"mipi_video_nt35590_720p"
#define MIPI2LVDS_VIDEO_VX5B3D_WSVGA_PANEL_NAME	"mipi2lvds_vx5b3d_wsvga"
#define HDMI_PANEL_NAME	"hdmi_msm"
#define TVOUT_PANEL_NAME	"tvout_msm"

static struct resource msm_fb_resources[] = {
	{
		.flags = IORESOURCE_DMA,
	}
};

static int msm_fb_detect_panel(const char *name)
{
	if (machine_is_msm8930_evt()) {
		if (!strncmp(name, MIPI_VIDEO_NT_HD_PANEL_NAME,
			strnlen(MIPI_VIDEO_NT_HD_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
			return 0;
	} else {
		if (!strncmp(name, MIPI_CMD_NOVATEK_QHD_PANEL_NAME,
			strnlen(MIPI_CMD_NOVATEK_QHD_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;
	}
        if (!strncmp(name, MIPI2LVDS_VIDEO_VX5B3D_WSVGA_PANEL_NAME,
			strnlen(MIPI2LVDS_VIDEO_VX5B3D_WSVGA_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;
	if (!strncmp(name, MIPI_VIDEO_NOVATEK_HD_PANEL_NAME,
			strnlen(MIPI_VIDEO_NOVATEK_HD_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;
	if (!strncmp(name, MIPI_VIDEO_SAMSUNG_WSVGA_PANEL_NAME,
			strnlen(MIPI_VIDEO_SAMSUNG_WSVGA_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;
	if (!strncmp(name, MIPI_ILI9341_BOE_VIDEO_QVGA_PANEL_NAME,
			strnlen(MIPI_ILI9341_BOE_VIDEO_QVGA_PANEL_NAME ,
				PANEL_NAME_MAX_LEN)))
		return 0;
#if !defined(CONFIG_FB_MSM_LVDS_MIPI_PANEL_DETECT) && \
	!defined(CONFIG_FB_MSM_MIPI_PANEL_DETECT)
	if (!strncmp(name, MIPI_VIDEO_NOVATEK_QHD_PANEL_NAME,
			strnlen(MIPI_VIDEO_NOVATEK_QHD_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;

	if (!strncmp(name, MIPI_VIDEO_TOSHIBA_WSVGA_PANEL_NAME,
			strnlen(MIPI_VIDEO_TOSHIBA_WSVGA_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;

	if (!strncmp(name, MIPI_VIDEO_SIMULATOR_VGA_PANEL_NAME,
			strnlen(MIPI_VIDEO_SIMULATOR_VGA_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;

	if (!strncmp(name, MIPI_CMD_RENESAS_FWVGA_PANEL_NAME,
			strnlen(MIPI_CMD_RENESAS_FWVGA_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;
#endif

	if (!strncmp(name, HDMI_PANEL_NAME,
			strnlen(HDMI_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;

	if (!strncmp(name, TVOUT_PANEL_NAME,
			strnlen(TVOUT_PANEL_NAME,
				PANEL_NAME_MAX_LEN)))
		return 0;

	pr_warning("%s: not supported '%s'", __func__, name);
	return -ENODEV;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
	.dev.platform_data = &msm_fb_pdata,
};

static bool dsi_power_on;

/*
 * TODO: When physical 8930/PM8038 hardware becomes
 * available, replace mipi_dsi_cdp_panel_power with
 * appropriate function.
 */
#if defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
#define DISP_RST_GPIO GPIO_MLCD_RST
#define DISP_3D_2D_MODE GPIO_MHL_RST
#elif defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
/* see ks02-gpio.h */
#define DISP_3D_2D_MODE GPIO_MHL_RST
#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
#define MIPI_RGB_RST_GPIO 11
#define DISP_RST_GPIO 58
#else
#define DISP_RST_GPIO 58
#define DISP_3D_2D_MODE 1
#endif
#if defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
#if defined(CONFIG_MACH_CANE)
#define DISP_BL_CONT_GPIO 7
#else
#define DISP_BL_CONT_GPIO 10
#endif
#endif
void pull_ldi_reset_down(void)
{
	int rc;
	rc = gpio_request(DISP_RST_GPIO, "disp_rst_n");
	if (rc) {
		pr_err("request gpio DISP_RST_GPIO failed, rc=%d\n",
			rc);
		gpio_free(DISP_RST_GPIO);
	}
	gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
			GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
	msleep(50);
	gpio_set_value(DISP_RST_GPIO, 0);
}
void active_reset(int on)
{
	if (on)
	{
#if defined(CONFIG_FB_MSM_MIPI_MAGNA_OLED_VIDEO_WVGA_PT_PANEL)\
		|| defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)\
		|| defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
		msleep(50);
		gpio_set_value(DISP_RST_GPIO, 0);
		usleep(5000);
		gpio_set_value(DISP_RST_GPIO, 1);
		msleep(50);
#elif defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
		mdelay(1);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(1);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		mdelay(5);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(150);
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_QHD_PT_PANEL)\
		|| defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_CMD_QHD_PT_PANEL)
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(5);
#elif defined(CONFIG_FB_MSM_MIPI_SAMSUNG_OLED_VIDEO_WVGA_PT_PANEL)
		udelay(5);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(120);
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
		msleep(15);
		gpio_set_value(GPIO_MLCD_RST, 0);
		pr_debug("%s: GPIO_MLCD_RST low\n", __func__);
		msleep(5);
		gpio_set_value(GPIO_MLCD_RST, 1);
		pr_debug("%s: GPIO_MLCD_RST high\n", __func__);
		msleep(15);
#ifdef CONFIG_SAMSUNG_CMC624
		/* Enable CMC Chip */
		if (samsung_has_cmc624()) {
			int retry=0;
			for(retry=0; retry<3; retry++)
			{
				cmc_power(on);
				if(samsung_cmc624_on(1))
					break;
				pr_err("%s: [CMC624] Init Failed!!! (Retry cnt=%d)\n", __func__,retry+1);
				cmc_power(0);
				msleep(500);
			}
		}
#endif
#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
		udelay(5);
		gpio_set_value(MIPI_RGB_RST_GPIO, 0);
		pr_info("%s: MIPI_RGB_RST_GPIO low\n", __func__);
		mdelay(5);
		gpio_set_value(MIPI_RGB_RST_GPIO, 1);
		pr_info("%s: MIPI_RGB_RST_GPIO high\n", __func__);
		mdelay(5);
		gpio_set_value(MIPI_RGB_RST_GPIO, 0);
		pr_info("%s: MIPI_RGB_RST_GPIO low\n", __func__);
		mdelay(5);
		gpio_set_value(MIPI_RGB_RST_GPIO, 1);
		pr_info("%s: MIPI_RGB_RST_GPIO high\n", __func__);
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		mdelay(5);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(5);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		mdelay(5);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(25);
#elif defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) || defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL) \
		|| defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		mdelay(25);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(5);
		gpio_set_value(DISP_RST_GPIO, 0);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(20);
		gpio_set_value(DISP_RST_GPIO, 1);
		pr_info("%s: DISP_RST_GPIO high\n", __func__);
		mdelay(5);
#endif
	} else {
#if defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
		msleep(10);
#endif

#if defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
#ifdef CONFIG_SAMSUNG_CMC624
		if (samsung_has_cmc624()) {
			samsung_cmc624_on(0);
			cmc_power(0);
		}
#endif
		gpio_set_value(GPIO_MLCD_RST, 0);
		pr_debug("%s: DISP_RST_GPIO low\n", __func__);
		msleep(5);
#endif
#if defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
		gpio_set_value(MIPI_RGB_RST_GPIO, 0);
		pr_info("%s: MIPI_RGB_RST_GPIO low\n", __func__);
		mdelay(1);
#endif
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		gpio_set_value(DISP_RST_GPIO, 0);
	}
}

#if defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
extern unsigned int system_rev;
#endif

#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
void set_esd_gpio_config(void)
{
	int rc;
	rc = gpio_request(GPIO_LCD_ESD_DET, "ESD_DET");
	if (rc) {
		pr_err("request gpio GPIO_LCD_ESD_DET failed, rc=%d\n",rc);
		gpio_free(GPIO_LCD_ESD_DET);
		return;
	}
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
	if(system_rev >= 11) {
	gpio_tlmm_config(GPIO_CFG(GPIO_LCD_ESD_DET,  0, GPIO_CFG_INPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	} else {
	gpio_tlmm_config(GPIO_CFG(GPIO_LCD_ESD_DET,  0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	}
#else
	gpio_tlmm_config(GPIO_CFG(GPIO_LCD_ESD_DET,  0, GPIO_CFG_INPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
#endif

}
#endif

#if defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
#if defined(CONFIG_MACH_LT02_CHN_CTC)
static int mipi_dsi2lvds_cdp_panel_power(int on)
{
        static struct regulator *reg_l2;

	int rc;
	
	if(!dsi_power_on){
	        reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,"dsi_vdda");
			if (IS_ERR(reg_l2)) {
				pr_err("could not get 8038_l2, rc = %ld\n",
					PTR_ERR(reg_l2));
				return -ENODEV;
			}

		dsi_power_on = true;
	}

	gpio_tlmm_config(GPIO_CFG(80, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	GPIO_CFG_ENABLE); //LCD Power enable
	gpio_tlmm_config(GPIO_CFG(47, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	GPIO_CFG_ENABLE); //LVDS Power enable
	gpio_tlmm_config(GPIO_CFG(2, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	GPIO_CFG_ENABLE);  //LCD Enable

	
	rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
	if (rc) {
		pr_err("set_voltage l2 failed, rc=%d\n", rc);
		return -EINVAL;
	}

	if(on){

		gpio_set_value(80, 1);
		gpio_set_value(47, 0);
		gpio_set_value(2, 0);
		mdelay(10);
		
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		msleep(5);

			
		gpio_set_value(47, 1);
		gpio_set_value(2, 1);
		
		printk("mipi2lvds_panel_power on \n");

		
		
	}else{

		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		
		gpio_set_value(47, 0);
		gpio_set_value(2, 0);

		printk("mipi2lvds_panel_power off \n");
	}
	return 0;
}
#else
static int mipi_dsi2lvds_cdp_panel_power(int on)
{

	static struct regulator *reg_l23, *reg_l2, *reg_l12, *reg_l35, *vreg_s1p8;

	int rc;

	pr_info("%s: state : %d\n", __func__, on);

	if (!dsi_power_on) {

		reg_l23 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vddio");
		if (IS_ERR(reg_l23)) {
			pr_err("could not get 8038_l23, rc = %ld\n",
				PTR_ERR(reg_l23));
			return -ENODEV;
		}

		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8038_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			return -ENODEV;
		}


		rc = regulator_set_voltage(reg_l23, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		reg_l12 = regulator_get(&msm_mipi_dsi1_device.dev, "dsi_vdc4");
		rc = regulator_set_voltage(reg_l12, 1200000, 1200000);
		if (rc)
			pr_err("error setting voltage for reg_l12\n");

		reg_l35 = regulator_get(&msm_mipi_dsi1_device.dev, "dsi_vdc3");
		rc = regulator_set_voltage(reg_l35, 3300000, 3300000);
		if (rc)
			pr_err("error setting voltage for reg_l35\n");

		vreg_s1p8 = regulator_get(&msm_mipi_dsi1_device.dev, "dsi_vdc5");
		if(!system_rev) {

			gpio_tlmm_config(GPIO_CFG(LVDS_I2C_CLK, 0, GPIO_CFG_INPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
					GPIO_CFG_DISABLE);

			gpio_tlmm_config(GPIO_CFG(LVDS_I2C_SDA, 0, GPIO_CFG_INPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
					GPIO_CFG_DISABLE);
			gpio_set_value(LVDS_I2C_CLK, 0);
			gpio_set_value(LVDS_I2C_SDA, 0);
		}
		gpio_tlmm_config(GPIO_CFG(LCD_BLIC_ON, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
				GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(LCD_BL_PWM, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
				GPIO_CFG_ENABLE);

		rc = gpio_request(DISP_RST_GPIO, "disp_rst_n");
		if (rc) {
			pr_err("request gpio DISP_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(DISP_RST_GPIO);
			return -ENODEV;
		}

		gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

		gpio_set_value(LCD_BLIC_ON, 0);

		dsi_power_on = true;
	}
	if (on) {

		rc = regulator_set_optimum_mode(reg_l23, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}

		rc = regulator_enable(reg_l23); /*1.8V*/
		if (rc) {
			pr_err("enable l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}

		rc = regulator_enable(reg_l12);
		if (rc)
			pr_err("error enabling regulator reg_l12\n");

		rc = regulator_enable(reg_l35);
		if (rc)
			pr_err("error enabling regulator reg_l35\n");

		rc = regulator_enable(vreg_s1p8);
		if (rc)
			pr_err("%s: error enabling regulator\n", __func__);

		gpio_set_value(LCD_BLIC_ON, 1);
		gpio_set_value(LCD_BL_PWM, 1);
	} else {

		gpio_set_value(DISP_RST_GPIO, 0);
		gpio_set_value(LCD_BLIC_ON, 0);
		gpio_set_value(LCD_BL_PWM, 0);
		rc = regulator_disable(vreg_s1p8); /*1.8V*/
		if (rc) {
			pr_err("disable vreg_s1p8 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l35); /*1.8V*/
		if (rc) {
			pr_err("disable reg_l35 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l12); /*1.8V*/
		if (rc) {
			pr_err("disable reg_l12 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l23); /*1.8V*/
		if (rc) {
			pr_err("disable reg_l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l2); /*1.8V*/
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
	}
	return 0;
}
#endif
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
int mipi_dsi_melius_panel_power(int on)
{
	static struct regulator *reg_l23, *reg_l2;
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_USC)\
	|| defined(CONFIG_MACH_MELIUS_SPR) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
	static struct regulator *reg_l30;
#endif
	int rc;

	int disp_bl_cont_gpio;
	disp_bl_cont_gpio = (system_rev>1)?GPIO_LCD_BOOSTER_EN:GPIO_LCD_BOOSTER_EN_EMUL;

	pr_debug("%s: state : %d (%d)\n", __func__, on, dsi_power_on);

	if (!dsi_power_on) {

		reg_l23 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vddio");
		if (IS_ERR(reg_l23)) {
			pr_err("could not get 8038_l23, rc = %ld\n",
				PTR_ERR(reg_l23));
			return -ENODEV;
		}
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
		if(system_rev >= 10) {
			reg_l30 = regulator_get(&msm_mipi_dsi1_device.dev,
					"8917_l30");
			if (IS_ERR(reg_l30)) {
				pr_err("could not get 8038_l30, rc = %ld\n",
					PTR_ERR(reg_l30));
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_USC)
		if(system_rev >= 2) {
			reg_l30 = regulator_get(&msm_mipi_dsi1_device.dev,
					"8917_l30");
			if (IS_ERR(reg_l30)) {
				pr_err("could not get 8038_l30, rc = %ld\n",
					PTR_ERR(reg_l30));
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_SPR)
				if(system_rev >= 3) {
					reg_l30 = regulator_get(&msm_mipi_dsi1_device.dev,
							"8917_l30");
					if (IS_ERR(reg_l30)) {
						pr_err("could not get 8038_l30, rc = %ld\n",
							PTR_ERR(reg_l30));
						return -ENODEV;
					}
				}

#endif
		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8038_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			return -ENODEV;
		}

		rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
		if(system_rev >= 10) {
			rc = regulator_set_voltage(reg_l30, 1800000, 1800000);
			if (rc) {
				pr_err("set_voltage l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_USC)
		if(system_rev >= 2) {
			rc = regulator_set_voltage(reg_l30, 1800000, 1800000);
			if (rc) {
				pr_err("set_voltage l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_SPR)
		if(system_rev >= 3) {
			rc = regulator_set_voltage(reg_l30, 1800000, 1800000);
			if (rc) {
				pr_err("set_voltage l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#endif

		rc = regulator_set_voltage(reg_l23, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = gpio_request(GPIO_LCD_EN, "LCD_EN");
		if (rc) {
			pr_err("request gpio GPIO_LCD_EN failed, rc=%d\n",
				rc);
			gpio_free(GPIO_LCD_EN);
		}
		gpio_tlmm_config(GPIO_CFG(GPIO_LCD_EN,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);



		dsi_power_on = true;
	}
	if (on) {
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
		if(system_rev >= 10) {
			rc = regulator_set_optimum_mode(reg_l30, 100000);
			if (rc < 0) {
				pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_USC)
		if(system_rev >= 2) {
			rc = regulator_set_optimum_mode(reg_l30, 100000);
			if (rc < 0) {
				pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_SPR)
		if(system_rev >= 3) {
			rc = regulator_set_optimum_mode(reg_l30, 100000);
			if (rc < 0) {
				pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
				return -EINVAL;
			}
		}
#endif
		rc = regulator_set_optimum_mode(reg_l23, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
		if(system_rev >= 10) {
			rc = regulator_enable(reg_l30);
			if (rc) {
				pr_err("enable l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_USC)
		if(system_rev >= 2) {
			rc = regulator_enable(reg_l30);
			if (rc) {
				pr_err("enable l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_SPR)
		if(system_rev >= 3) {
			rc = regulator_enable(reg_l30);
			if (rc) {
				pr_err("enable l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#endif
		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		msleep(5);
		rc = regulator_enable(reg_l23); /*1.8V*/
		if (rc) {
			pr_err("enable l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		msleep(5);

		gpio_tlmm_config(GPIO_CFG(disp_bl_cont_gpio,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(GPIO_LCD_EN,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

		gpio_set_value(disp_bl_cont_gpio, 1); /*backlight signal*/
		pr_debug("%s: GPIO_LCD_BOOSTER_EN high\n", __func__);

		msleep(5);
		gpio_set_value(GPIO_LCD_EN, 1);
		pr_debug("%s: DISP_EN high\n", __func__);
	}
	else {
		gpio_set_value(GPIO_LCD_EN, 0);
		pr_debug("%s: DISP_EN low\n", __func__);
		msleep(5);


		gpio_tlmm_config(GPIO_CFG(disp_bl_cont_gpio,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_DISABLE);
		gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_DISABLE);
		gpio_tlmm_config(GPIO_CFG(GPIO_LCD_EN,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_DISABLE);

		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}

#if defined(CONFIG_MACH_MELIUS_EUR_OPEN) || defined(CONFIG_MACH_MELIUS_EUR_LTE)\
	|| defined(CONFIG_MACH_MELIUS_ATT) || defined(CONFIG_MACH_MELIUS_TMO)\
	|| defined(CONFIG_MACH_MELIUS_SKT) || defined(CONFIG_MACH_MELIUS_KTT)\
	|| defined(CONFIG_MACH_MELIUS_LGT) || defined(CONFIG_MACH_MELIUS_CHN_CTC)
		if(system_rev >= 10) {
			rc = regulator_disable(reg_l30);
			if (rc) {
				pr_err("disable reg_l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_USC)
		if(system_rev >= 2) {
			rc = regulator_disable(reg_l30);
			if (rc) {
				pr_err("disable reg_l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#elif defined(CONFIG_MACH_MELIUS_SPR)
		if(system_rev >= 3) {
			rc = regulator_disable(reg_l30);
			if (rc) {
				pr_err("disable reg_l30 failed, rc=%d\n", rc);
				return -ENODEV;
			}
		}
#endif

#if 0	// common power supply.
		rc = regulator_disable(reg_l23);
		if (rc) {
			pr_err("disable reg_l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}

		rc = regulator_set_optimum_mode(reg_l23, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
	}
	return 0;
}
#elif  defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
#define LCD_BL_EN_GPIO 51
#define LCD_EN_N_GPIO 2
#define MIPI_RGB_REFCLK_GPIO 3
#define TE_GPIO 1
#define VGH_GPIO 19
#define MIPI_RGB_SDA_GPIO 8
#define MIPI_RGB_SCL_GPIO 9
static int mipi_dsi_biscotto_panel_power(int on)
{
	static struct regulator *reg_l12, *reg_l8, *reg_l2, *reg_l30;
	int rc;

	if (!dsi_power_on) {
	reg_l12 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc4");
		if (IS_ERR(reg_l12)) {
			pr_err("could not get 8917_l12, rc = %ld\n",
				PTR_ERR(reg_l12));
			return -ENODEV;
		}
	reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8917_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			return -ENODEV;
		}
	reg_l8 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc");
		if (IS_ERR(reg_l8)) {
			pr_err("could not get 8917_l8, rc = %ld\n",
				PTR_ERR(reg_l8));
			return -ENODEV;
		}
	reg_l30 =  regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc2");
		if (IS_ERR(reg_l30)) {
			pr_err("could not get 8917_l30, rc = %ld\n",
				PTR_ERR(reg_l30));
			return -ENODEV;
		}
	rc = regulator_set_voltage(reg_l12, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l12 failed, rc=%d\n", rc);
			return -EINVAL;
		}
	rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
	rc = regulator_set_voltage(reg_l8, 2800000, 2800000);
		if (rc) {
			pr_err("set_voltage l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
	udelay(200);
	rc = regulator_set_voltage(reg_l30, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage l30 failed, rc=%d\n", rc);
			return -EINVAL;
		}
	udelay(50);
	rc = gpio_request(DISP_RST_GPIO, "disp_rst_n");
		if (rc) {
			pr_err("request gpio DISP_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(DISP_RST_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(MIPI_RGB_RST_GPIO, "mipi_rgb_rst");
		if (rc) {
			pr_err("request gpio MIPI_RGB_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(MIPI_RGB_RST_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(MIPI_RGB_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(LCD_BL_EN_GPIO, "lcd_bl_en");
		if (rc) {
			pr_err("request gpio LCD_BL_EN_GPIO failed, rc=%d\n",
				rc);
			gpio_free(LCD_BL_EN_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(LCD_BL_EN_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(LCD_EN_N_GPIO, "lcd_en_n");
		if (rc) {
			pr_err("request gpio LCD_EN_N_GPIO failed, rc=%d\n",
				rc);
			gpio_free(LCD_EN_N_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(LCD_EN_N_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(MIPI_RGB_REFCLK_GPIO, "mipi_rgb_refclk");
		if (rc) {
			pr_err("request gpio MIPI_RGB_REFCLK_GPIO failed, rc=%d\n",
				rc);
			gpio_free(MIPI_RGB_REFCLK_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(MIPI_RGB_REFCLK_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(MIPI_RGB_SDA_GPIO, "mipi_rgb_sda");
		if (rc) {
			pr_err("request gpio LCD_EN_N_GPIO failed, rc=%d\n",
				rc);
			gpio_free(MIPI_RGB_SDA_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(MIPI_RGB_SDA_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	rc = gpio_request(MIPI_RGB_SCL_GPIO, "mipi_rgb_scl");
		if (rc) {
			pr_err("request gpio MIPI_RGB_SCL_GPIO failed, rc=%d\n",
				rc);
			gpio_free(MIPI_RGB_SCL_GPIO);
			return -ENODEV;
		}
	gpio_tlmm_config(GPIO_CFG(MIPI_RGB_SCL_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

	dsi_power_on = true;
	}
	if (on) {

		rc = regulator_set_optimum_mode(reg_l12, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l12 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l8, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l30, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_enable(reg_l12); /*1.2V*/
		if (rc) {
			pr_err("enable l12 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_enable(reg_l2); /*1.2V*/
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_enable(reg_l8); /*2.8V*/
		if (rc) {
			pr_err("enable l8 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		usleep(200);
		rc = regulator_enable(reg_l30); /*1.8V*/
		if (rc) {
			pr_err("enable l30 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		usleep(100);
		gpio_set_value(LCD_BL_EN_GPIO, 1);
		pr_info("%s: LCD_BL_EN_GPIO high\n", __func__);
		mdelay(5);
		gpio_set_value(LCD_EN_N_GPIO, 1);
		pr_info("%s: LCD_EN_N_GPIO high\n", __func__);
		mdelay(5);
		gpio_set_value(MIPI_RGB_SDA_GPIO, 0);
		gpio_set_value(MIPI_RGB_SCL_GPIO, 0);
		gpio_set_value(MIPI_RGB_REFCLK_GPIO, 0);

	} else {
		rc = regulator_disable(reg_l30);
		if (rc) {
			pr_err("disable reg_l30 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		udelay(100);
		rc = regulator_disable(reg_l8);
		if (rc) {
			pr_err("disable reg_8 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_disable(reg_l12);
		if (rc) {
			pr_err("disable reg_l12 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_set_optimum_mode(reg_l30, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		usleep(50);
		rc = regulator_set_optimum_mode(reg_l8, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l2, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_optimum_mode(reg_l12, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		gpio_set_value(LCD_EN_N_GPIO, 0);
		pr_info("%s: LCD_EN_N_GPIO low\n", __func__);
		mdelay(1);
		gpio_set_value(LCD_BL_EN_GPIO, 0);
		pr_info("%s: DISP_EN_N_GPIO low\n", __func__);
		mdelay(1);
	}
	return 0;
}
#elif defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)

#ifdef CONFIG_DUAL_LCD
int flip_value;
int get_lcd_flip_status(void);

void set_lcd_flip_status( int value)
{
	flip_value = value;
}

int get_lcd_flip_status()
{
	return flip_value;
}

int get_hall_sensor(void)
{
	return gpio_get_value(GPIO_HALL_SENSOR_INT);
}

int get_disp_switch(void)
{
	return gpio_get_value(DISP_SWITCH_GPIO);
}

static int updated_disp_switch  = false;
int is_already_updated_disp_switch( void )
{
	return updated_disp_switch;
}
#endif /* #ifdef DUAL_LCD */

static int mipi_dsi_cdp_panel_power(int on)
{
	static struct regulator *reg_l2;
	static struct regulator *sub_buck1, *sub_ldo5;
	int rc;
	int ret = 0;

	pr_debug("%s: state : %d\n", __func__, on);

	if (!dsi_power_on) {
		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8038_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			ret = -ENODEV;
			goto fail_mipi_dsi_cdp_panel_power;
		}
		sub_buck1 = regulator_get(NULL, "vlcd_1.8v");
		if (IS_ERR(sub_buck1)) {
			pr_err("lp8720 : could not get sub_buck1, rc = %ld\n",
				PTR_ERR(sub_buck1));
			sub_buck1 = NULL;
		}
		sub_ldo5 = regulator_get(NULL, "vlcd_2.8v");
		if (IS_ERR(sub_ldo5)) {
			pr_err("lp8720 : could not get sub_ldo5, rc = %ld\n",
				PTR_ERR(sub_ldo5));
			sub_ldo5 = NULL;
		}
		if( sub_buck1 != NULL && sub_ldo5 != NULL )
		{
			rc = regulator_set_voltage(sub_buck1, 1800000, 1800000);
			if (rc) {
				pr_err("set_voltage sub_buck1 failed, rc=%d\n", rc);
				ret = -EINVAL;
				goto fail_mipi_dsi_cdp_panel_power;
			}
			rc = regulator_set_voltage(sub_ldo5, 3000000, 3000000);
			if (rc) {
				pr_err("set_voltage sub_ldo5 failed, rc=%d\n", rc);
				ret = -EINVAL;
				goto fail_mipi_dsi_cdp_panel_power;
			}
			pr_debug( "lp8720(LCD) : regulator_set_voltage success" );
		}
		rc = gpio_request(DISP_RST_GPIO, "disp_rst_n");
		if (rc) {
			pr_err("request gpio DISP_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(DISP_RST_GPIO);
			ret = -ENODEV;
			goto fail_mipi_dsi_cdp_panel_power;
		}

		gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);

		dsi_power_on = true;
	}
	if (on) {
#ifdef CONFIG_DUAL_LCD
		updated_disp_switch = true;
		set_lcd_flip_status( get_hall_sensor() );
		pr_info( "%s : switch = %s", __func__, (get_lcd_flip_status() ? "1(open)" : "0(close)")  );
		gpio_set_value(DISP_SWITCH_GPIO, !get_lcd_flip_status());
#endif /* #ifdef CONFIG_DUAL_LCD */
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			ret = -EINVAL;
			goto fail_mipi_dsi_cdp_panel_power;
		}
		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			ret = -ENODEV;
			goto fail_mipi_dsi_cdp_panel_power;
		}
		usleep(5000);
		if( sub_buck1 != NULL && sub_ldo5 != NULL )
		{
			rc = regulator_enable(sub_buck1); /*1.8V*/
			if (rc) {
				pr_err("enable sub_buck1 failed, rc=%d\n", rc);
				ret = -ENODEV;
				goto fail_mipi_dsi_cdp_panel_power;
			}
			usleep(5000);
			rc = regulator_enable(sub_ldo5); /*2.8V*/
			if (rc) {
				pr_err("enable sub_ldo5 failed, rc=%d\n", rc);
				ret = -ENODEV;
				goto fail_mipi_dsi_cdp_panel_power;
			}
		}
		else
		{
			pr_err( "lp8720(mipi_lcd) : PASS(regulator_enable)" );
		}

	} else {

#if 0 /* defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL) */
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		gpio_set_value(DISP_RST_GPIO, 0);
#endif
		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			ret = -ENODEV;
			goto fail_mipi_dsi_cdp_panel_power;
		}
		if( sub_buck1 != NULL && sub_ldo5 != NULL )
		{
			rc = regulator_disable(sub_buck1); /*1.8V*/
			if (rc) {
				pr_err("disable sub_buck1 failed, rc=%d\n", rc);
				ret = -ENODEV;
				goto fail_mipi_dsi_cdp_panel_power;
			}
			usleep(5000);
			rc = regulator_disable(sub_ldo5); /*2.8V*/
			if (rc) {
				pr_err("disable sub_ldo5 failed, rc=%d\n", rc);
				ret = -ENODEV;
				goto fail_mipi_dsi_cdp_panel_power;
			}
		}
		else	pr_err( "lp8720(mipi_lcd) : PASS(regulator_disable)" );
	}

fail_mipi_dsi_cdp_panel_power:
	updated_disp_switch = false;
	return ret;
}
#else /* CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL */
static int mipi_dsi_cdp_panel_power(int on)
{
#if defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
	|| defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
	static struct regulator *reg_l8, *reg_l2;
#elif defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
	static struct regulator *reg_l30, *reg_l23, *reg_l2;
#else
	static struct regulator *reg_l8, *reg_l23, *reg_l2;
#endif
	int rc;

	pr_debug("%s: state : %d\n", __func__, on);

	if (!dsi_power_on) {

#if defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		reg_l30 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc2");
		if (IS_ERR(reg_l30)) {
			pr_err("could not get 8038_l30, rc = %ld\n",
				PTR_ERR(reg_l30));
			return -ENODEV;
		}
#else
		reg_l8 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdc");
		if (IS_ERR(reg_l8)) {
			pr_err("could not get 8038_l8, rc = %ld\n",
				PTR_ERR(reg_l8));
			return -ENODEV;
		}
#endif
#if !defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
		&& !defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		reg_l23 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vddio");
		if (IS_ERR(reg_l23)) {
			pr_err("could not get 8038_l23, rc = %ld\n",
				PTR_ERR(reg_l23));
			return -ENODEV;
		}
#endif
		reg_l2 = regulator_get(&msm_mipi_dsi1_device.dev,
				"dsi_vdda");
		if (IS_ERR(reg_l2)) {
			pr_err("could not get 8038_l2, rc = %ld\n",
				PTR_ERR(reg_l2));
			return -ENODEV;
		}
#if defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
		||  defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		rc = regulator_set_voltage(reg_l8, 3300000, 3300000);
		if (rc) {
			pr_err("set_voltage l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#elif defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		rc = regulator_set_voltage(reg_l30, 3300000, 3300000);
		if (rc) {
			pr_err("set_voltage l30 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_voltage(reg_l23, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#else
		rc = regulator_set_voltage(reg_l8, 3000000, 3000000);
		if (rc) {
			pr_err("set_voltage l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_set_voltage(reg_l23, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
		rc = regulator_set_voltage(reg_l2, 1200000, 1200000);
		if (rc) {
			pr_err("set_voltage l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}

		rc = gpio_request(DISP_RST_GPIO, "disp_rst_n");
		if (rc) {
			pr_err("request gpio DISP_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(DISP_RST_GPIO);
			return -ENODEV;
		}

		gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
#if defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
		rc = gpio_request(DISP_BL_CONT_GPIO, "disp_BL_n");
		if (rc) {
			pr_info("request gpio DISP_RST_GPIO failed, rc=%d\n",
				rc);
			gpio_free(DISP_BL_CONT_GPIO);
			return -ENODEV;
		}
		usleep(5000);

		gpio_tlmm_config(GPIO_CFG(DISP_BL_CONT_GPIO, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
#endif
		dsi_power_on = true;
	}
	if (on) {
#if defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		rc = regulator_set_optimum_mode(reg_l30, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#else
		rc = regulator_set_optimum_mode(reg_l8, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
#if !defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
		&& !defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		rc = regulator_set_optimum_mode(reg_l23, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
		rc = regulator_set_optimum_mode(reg_l2, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_enable(reg_l2);
		if (rc) {
			pr_err("enable l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}

#if defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL)\
		|| defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		gpio_tlmm_config(GPIO_CFG(43, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 	GPIO_CFG_ENABLE);
#ifdef CONFIG_MACH_CRATERTD_CHN_3G
		gpio_set_value(43, 0);
#else
		gpio_set_value(43, 1);
#endif
#else
		rc = regulator_enable(reg_l23); /*1.8V*/
		if (rc) {
			pr_err("enable l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#endif
		usleep(5000);
#if defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		gpio_tlmm_config(GPIO_CFG(GPIO_LCD_EN,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
		gpio_set_value(GPIO_LCD_EN, 1);
		pr_debug("%s: LCD_EN high\n", __func__);

		rc = regulator_enable(reg_l30);
		if (rc) {
			pr_err("enable l30 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#else
		rc = regulator_enable(reg_l8); /*3V*/
		if (rc) {
			pr_err("enable l8 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#endif
	} else {

#if 0
		pr_info("%s: DISP_RST_GPIO low\n", __func__);
		gpio_set_value(DISP_RST_GPIO, 0);
#endif
#if defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		rc = regulator_disable(reg_l30);
		if (rc) {
			pr_err("disable reg_l30 failed, rc=%d\n", rc);
			return -ENODEV;
		}

		gpio_set_value(GPIO_LCD_EN, 0);
		pr_debug("%s: DISP_EN low\n", __func__);
		msleep(5);
		gpio_tlmm_config(GPIO_CFG(GPIO_LCD_EN,  0, GPIO_CFG_OUTPUT,
					GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
				GPIO_CFG_DISABLE);
#else
		rc = regulator_disable(reg_l8);
		if (rc) {
			pr_err("disable reg_l8 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#endif
#if defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
		|| defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
#ifdef CONFIG_MACH_CRATERTD_CHN_3G
		gpio_set_value(43, 1);
#else
		gpio_set_value(43, 0);
#endif
#else
		rc = regulator_disable(reg_l23);
		if (rc) {
			pr_err("disable reg_l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#endif
		rc = regulator_disable(reg_l2);
		if (rc) {
			pr_err("disable reg_l2 failed, rc=%d\n", rc);
			return -ENODEV;
		}
#if defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		rc = regulator_set_optimum_mode(reg_l30, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l30 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#else
		rc = regulator_set_optimum_mode(reg_l8, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l8 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
#if !defined(CONFIG_FB_MSM_MIPI_HX8389B_VIDEO_QHD_PT_PANEL) \
		&& !defined(CONFIG_FB_MSM_MIPI_HX8369B_VIDEO_WVGA_PT_PANEL)
		rc = regulator_set_optimum_mode(reg_l23, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
#if !defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
		rc = regulator_set_optimum_mode(reg_l2, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l2 failed, rc=%d\n", rc);
			return -EINVAL;
		}
#endif
	}
	return 0;
}
#endif /* CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL */

static char mipi_dsi_splash_is_enabled(void);
static int mipi_dsi_panel_power(int on)
{
	pr_debug("%s: on=%d\n", __func__, on);
#if defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
	return mipi_dsi2lvds_cdp_panel_power(on);
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
	return mipi_dsi_melius_panel_power(on);
#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
	return mipi_dsi_biscotto_panel_power(on);
#else
	return mipi_dsi_cdp_panel_power(on);
#endif
}

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio = MDP_VSYNC_GPIO,
	.dsi_power_save = mipi_dsi_panel_power,
	.splash_is_enabled = mipi_dsi_splash_is_enabled,
	.lcd_rst_down = pull_ldi_reset_down,
#if defined (CONFIG_MIPI_DSI_RESET_LP11)
	.active_reset = active_reset,
#endif
};

#ifdef CONFIG_MSM_BUS_SCALING

static struct msm_bus_vectors mdp_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
static struct msm_bus_vectors hdmi_as_primary_vectors[] = {
	/* If HDMI is used as primary */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 2000000000,
		.ib = 2000000000,
	},
};
static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
	{
		ARRAY_SIZE(hdmi_as_primary_vectors),
		hdmi_as_primary_vectors,
	},
};
#else
static struct msm_bus_vectors mdp_ui_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_vga_vectors[] = {
	/* VGA and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 216000000 * 2,
		.ib = 270000000 * 2,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	/* 720p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 304128000 * 2,
		.ib = 380160000 * 2,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	/* 1080p and less video */
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 407808000 * 4,
		.ib = 509760000 * 4,
	},
};

static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(mdp_ui_vectors),
		mdp_ui_vectors,
	},
	{
		ARRAY_SIZE(mdp_ui_vectors),
		mdp_ui_vectors,
	},
	{
		ARRAY_SIZE(mdp_vga_vectors),
		mdp_vga_vectors,
	},
	{
		ARRAY_SIZE(mdp_720p_vectors),
		mdp_720p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
};
#endif

static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};

#endif

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = MDP_VSYNC_GPIO,
	.mdp_max_clk = 200000000,
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
	.mdp_rev = MDP_REV_43,
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
	.mem_hid = BIT(ION_CP_MM_HEAP_ID),
#else
	.mem_hid = MEMTYPE_EBI1,
#endif
	.mdp_iommu_split_domain = 0,
	.cont_splash_enabled = 0x01,
	.splash_screen_addr = 0x00,
	.splash_screen_size = 0x00,
};

void __init msm8930_mdp_writeback(struct memtype_reserve* reserve_table)
{
	mdp_pdata.ov0_wb_size = MSM_FB_OVERLAY0_WRITEBACK_SIZE;
	mdp_pdata.ov1_wb_size = MSM_FB_OVERLAY1_WRITEBACK_SIZE;
#if defined(CONFIG_ANDROID_PMEM) && !defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	reserve_table[mdp_pdata.mem_hid].size +=
		mdp_pdata.ov0_wb_size;
	reserve_table[mdp_pdata.mem_hid].size +=
		mdp_pdata.ov1_wb_size;
#endif
}

#if defined(CONFIG_FB_MSM_MIPI_MAGNA_OLED_VIDEO_WVGA_PT_PANEL)
static struct platform_device mipi_dsi_magna_panel_device = {
	.name = "mipi_magna",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};
#elif defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
static struct platform_device mipi_dsi_ams367_panel_device = {
	.name = "mipi_ams367",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};
#elif defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
static struct platform_device mipi_dsi_himax_panel_device = {
	.name = "mipi_himax",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};
#elif defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
static struct platform_device mipi_dsi_himax8389b_panel_device = {
	.name = "mipi_hx8389b",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};
#elif defined(CONFIG_MACH_LT02_CHN_CTC) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
static struct platform_device mipi2dsi_vx5b3d_panel_device = {
	.name = "mipi2lvds_vx5b3d",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
static struct platform_device mipi_dsi_novatek_nt35596_panel_device = {
	.name = "mipi_novatek_nt35596",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};

#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
static struct platform_device mipi_dsi_ili9341_boe_panel_device = {
		.name = "mipi_toshiba_tc358762",
		.id = 0,
		.dev.platform_data = &mipi_dsi_pdata,
};

#else
static struct platform_device mipi_dsi_samsung_panel_device = {
	.name = "mipi_samsung",
	.id = 0,
	.dev.platform_data = &mipi_dsi_pdata,
};

#endif

#define LPM_CHANNEL0 0
#define FPGA_3D_GPIO_CONFIG_ADDR	0xB5

static char mipi_dsi_splash_is_enabled(void)
{
	return mdp_pdata.cont_splash_enabled;
}

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct resource hdmi_msm_resources[] = {
	{
		.name  = "hdmi_msm_qfprom_addr",
		.start = 0x00700000,
		.end   = 0x007060FF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_hdmi_addr",
		.start = 0x04A00000,
		.end   = 0x04A00FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_irq",
		.start = HDMI_IRQ,
		.end   = HDMI_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static int hdmi_enable_5v(int on);
static int hdmi_core_power(int on, int show);
static int hdmi_cec_power(int on);
static int hdmi_gpio_config(int on);
static int hdmi_panel_power(int on);

static struct msm_hdmi_platform_data hdmi_msm_data = {
	.irq = HDMI_IRQ,
	.enable_5v = hdmi_enable_5v,
	.core_power = hdmi_core_power,
	.cec_power = hdmi_cec_power,
	.panel_power = hdmi_panel_power,
	.gpio_config = hdmi_gpio_config,
};

static struct platform_device hdmi_msm_device = {
	.name = "hdmi_msm",
	.id = 0,
	.num_resources = ARRAY_SIZE(hdmi_msm_resources),
	.resource = hdmi_msm_resources,
	.dev.platform_data = &hdmi_msm_data,
};
#else
static int hdmi_panel_power(int on) { return 0; }
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */

#ifdef CONFIG_FB_MSM_WRITEBACK_MSM_PANEL
static struct platform_device wfd_panel_device = {
	.name = "wfd_panel",
	.id = 0,
	.dev.platform_data = NULL,
};

static struct platform_device wfd_device = {
	.name          = "msm_wfd",
	.id            = -1,
};
#endif

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors dtv_bus_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
static struct msm_bus_vectors dtv_bus_def_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 2000000000,
		.ib = 2000000000,
	},
};


static struct msm_bus_vectors dtv_bus_cam_override_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 2000000000,
	},
};

#else
static struct msm_bus_vectors dtv_bus_def_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 566092800 * 2,
		.ib = 707616000 * 2,
	},
};

static struct msm_bus_vectors dtv_bus_cam_override_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 707616000 * 2,
	},
};

#endif

static struct msm_bus_paths dtv_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(dtv_bus_init_vectors),
		dtv_bus_init_vectors,
	},
	{
		ARRAY_SIZE(dtv_bus_def_vectors),
		dtv_bus_def_vectors,
	},
	{
		ARRAY_SIZE(dtv_bus_cam_override_vectors),
		dtv_bus_cam_override_vectors,
	},
};
static struct msm_bus_scale_pdata dtv_bus_scale_pdata = {
	dtv_bus_scale_usecases,
	ARRAY_SIZE(dtv_bus_scale_usecases),
	.name = "dtv",
};

static struct lcdc_platform_data dtv_pdata = {
	.bus_scale_table = &dtv_bus_scale_pdata,
	.lcdc_power_save = hdmi_panel_power,
};
#endif

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static int hdmi_enable_5v(int on)
{
	static struct regulator *reg_ext_5v;	/* HDMI_5V */
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_ext_5v) {
		reg_ext_5v = regulator_get(&hdmi_msm_device.dev, "hdmi_mvs");
		if (IS_ERR(reg_ext_5v)) {
			pr_err("'%s' regulator not found, rc=%ld\n",
				"hdmi_mvs", IS_ERR(reg_ext_5v));
			reg_ext_5v = NULL;
			return -ENODEV;
		}
	}

	if (on) {
		rc = regulator_enable(reg_ext_5v);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_ext_5v", rc);
			return rc;
		}
		pr_debug("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_ext_5v);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_ext_5v", rc);
		pr_debug("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}

static int hdmi_core_power(int on, int show)
{
	/* Both HDMI "avdd" and "vcc" are powered by 8038_l23 regulator */
	static struct regulator *reg_8038_l23;
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_8038_l23) {
		reg_8038_l23 = regulator_get(&hdmi_msm_device.dev, "hdmi_avdd");
		if (IS_ERR(reg_8038_l23)) {
			pr_err("could not get reg_8038_l23, rc = %ld\n",
				PTR_ERR(reg_8038_l23));
			return -ENODEV;
		}
		rc = regulator_set_voltage(reg_8038_l23, 1800000, 1800000);
		if (rc) {
			pr_err("set_voltage failed for 8921_l23, rc=%d\n", rc);
			return -EINVAL;
		}
	}

	if (on) {
		rc = regulator_set_optimum_mode(reg_8038_l23, 100000);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		rc = regulator_enable(reg_8038_l23);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"hdmi_avdd", rc);
			return rc;
		}
		pr_debug("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8038_l23);
		if (rc) {
			pr_err("disable reg_8038_l23 failed, rc=%d\n", rc);
			return -ENODEV;
		}
		rc = regulator_set_optimum_mode(reg_8038_l23, 100);
		if (rc < 0) {
			pr_err("set_optimum_mode l23 failed, rc=%d\n", rc);
			return -EINVAL;
		}
		pr_debug("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
}

static int hdmi_gpio_config(int on)
{
	int rc = 0;
	static int prev_on;

	if (on == prev_on)
		return 0;

	if (on) {
		rc = gpio_request(100, "HDMI_DDC_CLK");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_DDC_CLK", 100, rc);
			return rc;
		}
		rc = gpio_request(101, "HDMI_DDC_DATA");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_DDC_DATA", 101, rc);
			goto error1;
		}
		rc = gpio_request(102, "HDMI_HPD");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_HPD", 102, rc);
			goto error2;
		}
		pr_debug("%s(on): success\n", __func__);
	} else {
		gpio_free(100);
		gpio_free(101);
		gpio_free(102);
		pr_debug("%s(off): success\n", __func__);
	}

	prev_on = on;
	return 0;

error2:
	gpio_free(101);
error1:
	gpio_free(100);
	return rc;
}

static int hdmi_cec_power(int on)
{
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (on) {
		rc = gpio_request(99, "HDMI_CEC_VAR");
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"HDMI_CEC_VAR", 99, rc);
			goto error;
		}
		pr_debug("%s(on): success\n", __func__);
	} else {
		gpio_free(99);
		pr_debug("%s(off): success\n", __func__);
	}

	prev_on = on;

	return 0;
error:
	return rc;
}

static int hdmi_panel_power(int on)
{
	int rc;

	pr_debug("%s: HDMI Core: %s\n", __func__, (on ? "ON" : "OFF"));
	rc = hdmi_core_power(on, 1);
	if (rc)
		rc = hdmi_cec_power(on);

	pr_debug("%s: HDMI Core: %s Success\n", __func__, (on ? "ON" : "OFF"));
	return rc;
}
#endif /* CONFIG_FB_MSM_HDMI_MSM_PANEL */

#ifdef CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL
int is_ams367_s6e88a = false;
static int __init get_lcd_id_cmdline(char *mode)
{
	char *pt;
	int lcd_id;

	lcd_id = 0;
	if( mode == NULL ) return 1;
	for( pt = mode; *pt != 0; pt++ )
	{
		lcd_id <<= 4;
		switch(*pt)
		{
			case '0' ... '9' :
				lcd_id += *pt -'0';
			break;
			case 'a' ... 'f' :
				lcd_id += 10 + *pt -'a';
			break;
			case 'A' ... 'F' :
				lcd_id += 10 + *pt -'A';
			break;
		}
	}

	/* check AMS367 S6E88A0 */
	is_ams367_s6e88a = ((lcd_id&0xFF0000)!=0xFE0000);

	printk(KERN_DEBUG "%s: LCD_ID = 0x%X, S6E88A =%d", __func__,lcd_id, is_ams367_s6e88a);

	return 0;
}

int is_S6E88A(void)
{
	return is_ams367_s6e88a;
}
__setup( "lcd_id=0x", get_lcd_id_cmdline );
#endif /* #ifdef CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL */

void __init msm8930_init_fb(void)
{
	platform_device_register(&msm_fb_device);

#ifdef CONFIG_FB_MSM_WRITEBACK_MSM_PANEL
	platform_device_register(&wfd_panel_device);
	platform_device_register(&wfd_device);
#endif

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	platform_device_register(&hdmi_msm_device);
#endif

#if defined(CONFIG_FB_MSM_MIPI_MAGNA_OLED_VIDEO_WVGA_PT_PANEL)
	platform_device_register(&mipi_dsi_magna_panel_device);
#elif defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
	platform_device_register(&mipi_dsi_ams367_panel_device);
#elif defined(CONFIG_FB_MSM_MIPI_HIMAX_TFT_VIDEO_WVGA_PT_PANEL)
	platform_device_register(&mipi_dsi_himax_panel_device);
#elif defined(CONFIG_FB_MSM_MIPI_LMS_HX8389B_VIDEO_QHD_PT_PANEL)
	platform_device_register(&mipi_dsi_himax8389b_panel_device);
#elif defined(CONFIG_MACH_LT02_CHN_CTC) || defined(CONFIG_FB_MSM_MIPI_SAMSUNG_TFT_VIDEO_WSVGA_PT_PANEL)
	platform_device_register(&mipi2dsi_vx5b3d_panel_device);
#elif defined(CONFIG_FB_MSM_MIPI_NOVATEK_VIDEO_HD_PT_PANEL)
	platform_device_register(&mipi_dsi_novatek_nt35596_panel_device);
#elif defined(CONFIG_FB_MSM_MIPI_ILI9341_BOE_VIDEO_QVGA_PT_PANEL)
	platform_device_register(&mipi_dsi_ili9341_boe_panel_device);
#else
	platform_device_register(&mipi_dsi_samsung_panel_device);
#endif

#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
	esd_pdata.esd_gpio_irq =  MSM_GPIO_TO_INT(GPIO_LCD_ESD_DET);
	platform_device_register(&samsung_mipi_esd_refresh_device);
#endif
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
#ifdef CONFIG_MSM_BUS_SCALING
	msm_fb_register_device("dtv", &dtv_pdata);
#endif
}

void __init msm8930_allocate_fb_region(void)
{
	void *addr;
	unsigned long size;

	if (use_frame_buffer) {
		size = MSM_FB_SIZE;
		addr = alloc_bootmem_align(size, 0x1000);
		msm_fb_resources[0].start = __pa(addr);
		msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
		pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
			size, addr, __pa(addr));
	} else {
		size = 0;
		addr = NULL;
		msm_fb_resources[0].start = 0UL;
		msm_fb_resources[0].end = 0UL;
		pr_info("FB is not used. No allocation for FB");
	}
}
