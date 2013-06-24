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
#include "mipi_tc358762_dsi2rgb.h"

static struct msm_panel_info pinfo;
static struct mipi_panel_data mipi_pd;

static char power_setting_seq_1[] = {
	0x00, 0x05,
	0xC0, 0x26,
};

static char power_setting_seq_2[] = {
	0x00, 0x05,
	0xC1, 0x10,
};

static char power_setting_seq_3[] = {
	0x00, 0x05,
	0xC5, 0x2D, 0x3F,
};

static char display_parameter_setting_1[] = {
	0x00, 0x05,
	0xCF, 0x00, 0xF9, 0x30,
};

static char display_parameter_setting_2[] = {
	0x00, 0x05,
	0xED, 0x64, 0x03, 0x12, 0x81,
};

static char display_parameter_setting_3[] = {
	0x00, 0x05,
	0xE8, 0x85, 0x00, 0x60,
};

static char display_parameter_setting_4[] = {
	0x00, 0x05,
	0xCB, 0x39, 0x2C, 0x00, 0x34,
	0x02,
};

static char display_parameter_setting_5[] = {
	0x00, 0x05,
	0xF7, 0x20,
};

static char display_parameter_setting_6[] = {
	0x00, 0x05,
	0xEA, 0x00, 0x00,
};

static char display_parameter_setting_7[] = {
	0x00, 0x05,
	0xB7, 0x06,
};

static char display_parameter_setting_8[] = {
	0x00, 0x05,
	0xB1, 0x00, 0x15,
};

static char display_parameter_setting_9[] = {
	0x00, 0x05,
	0xB6, 0x0A, 0xA2, 0x27,
};

static char display_parameter_setting_10[] = {
	0x00, 0x05,
	0x2A, 0x00, 0x00, 0x00, 0xEF,
};

static char display_parameter_setting_11[] = {
	0x00, 0x05,
	0x2B,0x00, 0x00, 0x01, 0x3F,
};

static char display_parameter_setting_12[] = {
	0x00, 0x05,
	0x35, 0x00,
};

static char display_parameter_setting_13[] = {
	0x00, 0x05,
	0x36, 0xD8,
};

static char display_parameter_setting_14[] = {
	0x00, 0x05,
	0x3A, 0x55,
};

static char display_parameter_setting_15[] = {
	0x00, 0x05,
	0xB4, 0x02,
};

static char display_parameter_setting_16[] = {
	0x00, 0x05,
	0xF2, 0x03,
};

static char display_parameter_setting_17[] = {
	0x00, 0x05,
	0x26, 0x01,
};

static char display_parameter_setting_18[] = {
	0x00, 0x05,
	0xF6, 0x01, 0x30, 0x00,
};

static char display_parameter_setting_19[] = {
	0x00, 0x05,
	0xC7, 0x00,
};

static char gamma_setting_seq_1[] = {
	0x00, 0x05,
	0xE0, 0x0F, 0x1E, 0x1B, 0x09,
	0x0C, 0x06, 0x4C, 0x76, 0x3D,
	0x0A, 0x16, 0x07, 0x0B, 0x07,
	0x00,
};

static char gamma_setting_seq_2[] = {
	0x00, 0x05,
	0xE1, 0x00, 0x21, 0x23, 0x01,
	0x0F, 0x04, 0x35, 0x14, 0x45,
	0x02, 0x0A, 0x0A, 0x37, 0x3B,
	0x0F,
};

static char gamma_setting_seq_3[] = {
	0x00, 0x05,
	0xE2, 0x00, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09,
	0x09,
};

static char gamma_setting_seq_4[] = {
	0x00, 0x05,
	0xE2, 0x09,
};

static char gamma_setting_seq_5[] = {
	0x00, 0x05,
	0xE3, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x05, 0x04, 0x04, 0x03,
	0x03, 0x02, 0x01, 0x01, 0x00,
	0x00,
};

static char gamma_setting_seq_6[] = {
	0x00, 0x05,
	0xE3, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00
};

static char gamma_setting_seq_7[] = {
	0x00, 0x05,
	0xE3, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
};

static char gamma_setting_seq_8[] = {
	0x00, 0x05,
	0xE3, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
};

static char gamma_setting_seq_9[] = {
	0x00, 0x05,
	0xE3,0x00, 0x00, 0x00, 0x00,
};


static char SLPIN[] = {
	0x00, 0x05,
	0x10,
};

static char SLPOUT[] = {
	0x00, 0x05,
	0x11,
};
static char DISPON[] = {
	0x00, 0x05,
	0x29,
};

static char DISPOFF[] = {
	0x00, 0x05,
	0x28,
};

static char memory_write[] = {
	0x00, 0x05,
	0x2C,
};


static struct dsi_cmd_desc ili9341_panel_on_cmds[] = {

	{DTYPE_GEN_LWRITE, 1, 0, 0, 200,
		sizeof(SLPOUT), SLPOUT},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_1), display_parameter_setting_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_2), display_parameter_setting_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_3), display_parameter_setting_3},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_4), display_parameter_setting_4},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_5), display_parameter_setting_5},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_6), display_parameter_setting_6},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_7), display_parameter_setting_7},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_8), display_parameter_setting_8},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_9), display_parameter_setting_9},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_10), display_parameter_setting_10},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_11), display_parameter_setting_11},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_12), display_parameter_setting_12},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_13), display_parameter_setting_13},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_14), display_parameter_setting_14},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_15), display_parameter_setting_15},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_16), display_parameter_setting_16},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_17), display_parameter_setting_17},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_18), display_parameter_setting_18},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_parameter_setting_19), display_parameter_setting_19},


	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_setting_seq_1), power_setting_seq_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_setting_seq_2), power_setting_seq_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_setting_seq_3), power_setting_seq_3},


	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_1), gamma_setting_seq_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_2), gamma_setting_seq_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_3), gamma_setting_seq_3},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_4), gamma_setting_seq_4},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_5), gamma_setting_seq_5},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_6), gamma_setting_seq_6},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_7), gamma_setting_seq_7},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_8), gamma_setting_seq_8},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_setting_seq_9), gamma_setting_seq_9},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(DISPON), DISPON},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(memory_write), memory_write},

};

static struct dsi_cmd_desc ili9341_panel_off_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(DISPOFF), DISPOFF},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(SLPIN), SLPIN},
};
static struct mipi_panel_data mipi_pd = {
	.panel_name = "BOE_GH96-06385A\n",
	.on = {ili9341_panel_on_cmds
			, ARRAY_SIZE(ili9341_panel_on_cmds)},
	.off = {ili9341_panel_off_cmds
			, ARRAY_SIZE(ili9341_panel_off_cmds)},
};

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x30, 0x1E, 0x04, 0x00, 0x22, 0x28, 0x08, 0x22, 0x06, 0x03, 0x04, 0x0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_ili9341_boe_video_qqvga_pt_init(void)
{
	int ret;
	if (msm_fb_detect_client("mipi_ili9341_boe_vide_qvga"))
		return 0;

	/* Landscape */
	pinfo.xres = 240;
	pinfo.yres = 320;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 16; /* RGB888 = 24 bits-per-pixel */
	pinfo.fb_num = 2; /* using two frame buffers */

	/* bitclk */

	pinfo.clk_rate = 106000000;

	pinfo.lcdc.h_front_porch = 300;/* thfp */
	pinfo.lcdc.h_back_porch = 50;	/* thb */
	pinfo.lcdc.h_pulse_width = 50;	/* thpw */

	pinfo.lcdc.v_front_porch = 5;	/* tvfp */
	pinfo.lcdc.v_back_porch = 5;	/* tvb */	
	pinfo.lcdc.v_pulse_width = 10;	/* tvpw */

	pinfo.lcdc.border_clr = 0;		/* black */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */

	/* mipi - general */
	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode =  DSI_NON_BURST_SYNCH_PULSE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB565;
	pinfo.mipi.vc = 0;

	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x01;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = FALSE;
	pinfo.mipi.data_lane3 = FALSE;

	pinfo.mipi.force_clk_lane_hs = 1;
	//pinfo.mipi.no_max_pkt_size = 0;

	pinfo.mipi.tx_eot_append = FALSE;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;

	pinfo.mipi.esc_byte_ratio = 2;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;

	ret = mipi_tc358762_disp_device_register(&pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_VGA, &mipi_pd);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	pr_info("%s: end!\n", __func__);

	return ret;
}

module_init( mipi_ili9341_boe_video_qqvga_pt_init);

