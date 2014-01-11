/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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

#include "msm_fb.h"
#include "msm_fb_panel.h"
#include "mipi_dsi.h"
#include "mipi_samsung_tft.h"

static struct msm_panel_info pinfo;
static struct mipi_panel_data mipi_pd;

#define S6D78A0X_SLEEP_OUT_DELAY (120)
#define S6D78A0X_DISP_ON_DELAY (100)
#define S6D78A0X_DISP_OFF_DELAY (40)
#define S6D78A0X_SLEEP_IN_DELAY (120)

static char s6d78a0x_passwd1_enable[] = {
	0xF0,
	0x5A, 0X5A,
};

static char s6d78a0x_passwd1_disable[] = {
	0xF0,
	0xA5, 0XA5,
};

static char s6d78a0x_passwd2_enable[] = {
	0xF1,
	0x5A, 0X5A,
};

static char s6d78a0x_passwd2_disable[] = {
	0xF1,
	0xA5, 0XA5,
};

static char s6d78a0x_batctl[] = {
	0xBC,
	0x01
};

static char s6d78a0x_reg_B1h[] = {
	0xB1,
	0x82
};

static char s6d78a0x_reg_D0h[] = {
	0xD0, 0xC8
};

static char s6d78a0x_reg_F4h[] = {
	0xF4,
	0x01, 0x10, 0x32, 0x00, 0x24, 0x26, 0x28, 0x27,
	0x27, 0x27, 0x27, 0x2B, 0x2C, 0x65, 0x6A, 0x34,
	0x60,
};

static char s6d78a0x_reg_EFh[] = {
	0xEF,
	0x12, 0x12, 0x80, 0x21, 0x00, 0x21, 0x0B, 0x00,
	0x22, 0x22, 0x48, 0x01, 0x10, 0x21, 0x29, 0x03,
	0x03, 0x40, 0x00, 0x10,
};

static char s6d78a0x_reg_F2h[] = {
	0xF2,
	0x11, 0x04, 0x00, 0x10, 0x10, 0x1A, 0x10,
};

static char s6d78a0x_reg_E1h[] = {
	0xE1,
	0x01, 0x00, 0x01, 0x1B, 0x20, 0x17,
};

static char s6d78a0x_reg_E2h[] = {
	0xE2,
	0xED, 0xC7, 0x23,
};

static char s6d78a0x_reg_F6h[] = {
	0xF6,
	0x63, 0x23, 0x11, 0x1A, 0x1A, 0x10,
};

static char s6d78a0x_reg_35h[] = {0x35, 0x00};

static char s6d78a0x_reg_36h[] = {0x36, 0x10};

static char s6d78a0x_reg_F7h[] = {
	0xF7,
	0x1F, 0x1F, 0x05, 0x1A, 0x1B,
	0x1B, 0x1A, 0x0B, 0x0A, 0x11,
	0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x1F,
	0x1F, 0x04, 0x1A, 0x1B, 0x1B,
	0x1A, 0x09, 0x08, 0x10, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02,
};

static char s6d78a0x_reg_FAh[] = {
	0xFA,
	0x30, 0x3B, 0x33, 0x37, 0x34,
	0x3C, 0x3C, 0x33, 0x33, 0x2E,
	0x2A, 0x2B, 0x2A, 0x30, 0x3B,
	0x33, 0x37, 0x2C, 0x37, 0x3A,
	0x33, 0x33, 0x2E, 0x2A, 0x2B,
	0x2A, 0x19, 0x3B, 0x31, 0x38,
	0x34, 0x3C, 0x3B, 0x34, 0x33,
	0x2D, 0x26, 0x23, 0x17,
};

static char s6d78a0x_reg_FBh[] = {
	0xFB,
	0x1C, 0x3F, 0x33, 0x37, 0x34,
	0x3C, 0x3C, 0x33, 0x33, 0x2E,
	0x2A, 0x2B, 0x2A, 0x1C, 0x3F,
	0x33, 0x37, 0x2C, 0x37, 0x3A,
	0x33, 0x33, 0x2E, 0x2A, 0x2B,
	0x2A, 0x05, 0x3F, 0x31, 0x38,
	0x34, 0x3C, 0x3B, 0x34, 0x33,
	0x2D, 0x26, 0x23, 0x17,
};

static char exit_sleep[] = {0x11, 0x00};
static char display_on[] = {0x29, 0x00};
static char display_off[] = {0x28, 0x00};
static char enter_sleep[] = {0x10, 0x00};

static struct dsi_cmd_desc s6d78a0x_video_display_enable_access_register[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_passwd1_enable),s6d78a0x_passwd1_enable},
	{DTYPE_DCS_LWRITE,1, 0, 0,0,sizeof(s6d78a0x_passwd2_enable),s6d78a0x_passwd2_enable},
};

static struct dsi_cmd_desc s6d78a0x_video_display_disable_access_register[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_passwd1_disable), s6d78a0x_passwd1_disable},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_passwd2_disable), s6d78a0x_passwd2_disable},
};

static struct dsi_cmd_desc s6d78a0x_video_display_init_cmds[] = {
	/* Power Setting Sequence */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(s6d78a0x_batctl), s6d78a0x_batctl},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(s6d78a0x_reg_B1h), s6d78a0x_reg_B1h},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(s6d78a0x_reg_D0h), s6d78a0x_reg_D0h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_F4h), s6d78a0x_reg_F4h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_EFh), s6d78a0x_reg_EFh},
	/* Initializing Sequence */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_F2h), s6d78a0x_reg_F2h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_E1h), s6d78a0x_reg_E1h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_E2h), s6d78a0x_reg_E2h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_F6h), s6d78a0x_reg_F6h},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(s6d78a0x_reg_35h), s6d78a0x_reg_35h},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(s6d78a0x_reg_36h), s6d78a0x_reg_36h},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_F7h), s6d78a0x_reg_F7h},
	/* Gamma Setting Sequence */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_FAh), s6d78a0x_reg_FAh},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(s6d78a0x_reg_FBh), s6d78a0x_reg_FBh},
};

static struct dsi_cmd_desc s6d78a0x_video_display_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, S6D78A0X_SLEEP_OUT_DELAY, sizeof(exit_sleep),exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, S6D78A0X_DISP_ON_DELAY, sizeof(display_on),display_on},
};

static struct dsi_cmd_desc s6d78a0x_video_display_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, S6D78A0X_DISP_OFF_DELAY, sizeof(display_off),display_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, S6D78A0X_SLEEP_IN_DELAY, sizeof(enter_sleep),enter_sleep},
};

static int get_candela_index(int bl_level)
{
	int backlightlevel;

	/* brightness setting from platform is from 0 to 255
	 * But in this driver, brightness is only supported from 0 to 24 */

	switch (bl_level) {
	case 0:
		backlightlevel = 0;
		break;
	case 1 ... 19:
		backlightlevel = 32;
		break;
	case 20 ... 29:
		backlightlevel = 31;
		break;
	case 30 ... 37:
		backlightlevel = 31;
		break;
	case 38 ... 45:
		backlightlevel = 30;
		break;
	case 46 ... 53:
		backlightlevel = 29;
		break;
	case 54 ... 61:
		backlightlevel = 28;
		break;
	case 62 ... 70:
		backlightlevel = 27;
		break;
	case 71 ... 78:
		backlightlevel = 26;
		break;
	case 79 ... 86:
		backlightlevel = 25;
		break;
	case 87 ... 94:
		backlightlevel = 24;
		break;
	case 95 ... 102:
		backlightlevel = 23;
		break;
	case 103 ... 111:
		backlightlevel = 22;
		break;
	case 112 ... 119:
		backlightlevel = 21;
		break;
	case 120 ... 127:
		backlightlevel = 20;
		break;
	case 128 ... 135:
		backlightlevel = 19;
		break;
	case 136 ... 144:
		backlightlevel = 18;
		break;
	case 145 ... 157:
		backlightlevel = 17;
		break;
	case 158 ... 171:
		backlightlevel = 16;
		break;
	case 172 ... 185:
		backlightlevel = 15;
		break;
	case 186 ... 199:
		backlightlevel = 14;
		break;
	case 200 ... 212:
		backlightlevel = 13;
		break;
	case 213 ... 226:
		backlightlevel = 12;
		break;
	case 227 ... 240:
		backlightlevel = 11;
		break;
	case 241 ... 254:
		backlightlevel = 10;
		break;
	case 255:
		backlightlevel = 9;
		break;
	default:
		backlightlevel = 32;
		break;
	}
	return backlightlevel;
}

static struct mipi_panel_data mipi_pd = {
	.panel_name	= "SEC_S6D78A0X\n",
	.on		= {s6d78a0x_video_display_on_cmds
				, ARRAY_SIZE(s6d78a0x_video_display_on_cmds)},
	.off		= {s6d78a0x_video_display_off_cmds
				, ARRAY_SIZE(s6d78a0x_video_display_off_cmds)},
	.init		= {s6d78a0x_video_display_init_cmds
				, ARRAY_SIZE(s6d78a0x_video_display_init_cmds)},
	.en_access		= {s6d78a0x_video_display_enable_access_register
				, ARRAY_SIZE(s6d78a0x_video_display_enable_access_register)},
	.dis_access		= {s6d78a0x_video_display_disable_access_register
				, ARRAY_SIZE(s6d78a0x_video_display_disable_access_register)},
	.set_brightness_level = get_candela_index,
};

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI_BIT_CLK at 277.13MHz, 3 lane, RGB888 */
	/* regulator */
	.regulator = {0x0F, 0x0a, 0x04, 0x00, 0x20}, /* regulator */
	/* timing */
	.timing = {0x97, 0x23, 0x18, 0x00, 0x4B, 0x53, 0x1C, 0x27,
	0x27, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	.ctrl = {0x5f, 0x00, 0x00, 0x10},
	/* strength */
	.strength = {0xff, 0x00, 0x06, 0x00},
	/* pll control */
	.pll = {0x0, 0x14, 0x02, 0x19, 0x00, 0x50, 0x48, 0x63,
	0x31, 0x0F, 0x03,/* 4 lane */
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01},
};

static int __init mipi_cmd_samsung_tft_full_hd_pt_init(void)
{
	int ret;

	printk(KERN_DEBUG "[lcd] mipi_cmd_samsung_tft_full_hd_pt_init start\n");

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.width = 56;
	pinfo.height = 100;

	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

	pinfo.lcdc.h_back_porch = 10;
	pinfo.lcdc.h_front_porch = 90;
	pinfo.lcdc.h_pulse_width = 16;

	pinfo.lcdc.v_back_porch = 8;
	pinfo.lcdc.v_front_porch = 16;
	pinfo.lcdc.v_pulse_width = 8;

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0x000000;	/* blue */
	pinfo.lcdc.hsync_skew = 0;

	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;

	pinfo.clk_rate = 277130000;

	pinfo.mipi.mode = DSI_VIDEO_MODE;

	/*DSI1_VIDEO_MODE_CTRL */
	pinfo.mipi.pulse_mode_hsa_he = FALSE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = TRUE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = FALSE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_BURST_MODE;

	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.dlane_swap = 0x01;

	pinfo.mipi.t_clk_post = 0x04;
	pinfo.mipi.t_clk_pre = 0x1c;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.force_clk_lane_hs = 1;
	pinfo.mipi.esc_byte_ratio = 1;

	ret = mipi_samsung_tft_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_FULL_HD_PT,
						&mipi_pd);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	printk(KERN_DEBUG "[lcd] mipi_cmd_samsung_tft_full_hd_pt_init end\n");

	return ret;
}
module_init(mipi_cmd_samsung_tft_full_hd_pt_init);

