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

static char frame_rate_control_1[] = {
	0x00, 0x5,
	0xB1, 0x04, 0x04, 0x03,
};

static char display_inversion_control[] = {
	0x00, 0x5,
	0xB4, 0x00,
};

static char  frame_rate_control_2[] = {
	0x00, 0x5,
	0xB6, 0x17, 0x00,
};

static char power_control_1[] = {
	0x00, 0x5,
	0xC0, 0x02, 0x70,
};

static char power_control_2[] = {
	0x00, 0x5,
	0xC1, 0x05,
};

static char power_control_3[] = {
	0x00, 0x5,
	0xC2, 0x01, 0x01,
};

static char power_control_4[] = {
	0x00, 0x5,
	0xC3, 0x02, 0x07,
};

static char power_control_5[] = {
	0x00, 0x5,
	0xC4, 0x02, 0x04,
};

static char VCOM_control_1[] = {
	0x00, 0x5,
	0xC5, 0x3B, 0x57,
};

static char extension_command_control_1[] = {
	0x00, 0x5,
	0xF0, 0x01,
};

static char extension_command_control_2[] = {
	0x00, 0x5,
	0xF2, 0xC0,
};

static char power_control_5_1[] = {
	0x00, 0x5,
	0xFC, 0x11, 0x17,
};

static char VCOM_4level_control[] = {
	0x00, 0x5,
	0xFF, 0x40, 0x03, 0x1A,
};

static char interface_pixel_format[] = {
	0x00, 0x5,
	0x3A, 0x05,
};

static char gamma_positivepolar_format[] = {
	0x00, 0x5,
	0xE0, 0x03, 0x20, 0x05, 0x19,
	0x1F, 0x1D, 0x16, 0x1B, 0x16,
	0x12, 0x1E, 0x30, 0x00, 0x01,
	0x01, 0x0B,
};

static char gamma_negativepolar_format[] = {
	0x00, 0x5,
	0xE1, 0x04, 0x20, 0x04, 0x23,
	0x2D, 0x2B, 0x27, 0x2E, 0x32,
	0x31, 0x3A, 0x3A, 0x00, 0x01,
	0x02, 0x0C,
};

static char column_address_set[] = {
	0x00, 0x5,
	0x2A, 0x00, 0x00, 0x00, 0x9F,
};

static char row_address_set[] = {
	0x00, 0x5,
	0x2B, 0x00, 0x00, 0x00, 0x7F,
};

static char tearing_effect_line_on[] = {
	0x00, 0x5,
	0x35, 0x00,
};

static char memory_data_access_control[] = {
	0x00, 0x5,
	0x36, 0x68,
};

static char memory_write[] = {
	0x00, 0x5,
	0x2C,
};

static char SLPIN[] = {
	0x00, 0x5,
	0x10,
};

static char SLPOUT[] = {
	0x00, 0x5,
	0x11,
};
static char DISPON[] = {
	0x00, 0x5,
	0x29,
};

static char DISPOFF[] = {
	0x00, 0x5,
	0x28,
};

static struct dsi_cmd_desc toshiba_panel_on_cmds[] = {

	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(SLPOUT), SLPOUT},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(frame_rate_control_1), frame_rate_control_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(display_inversion_control), display_inversion_control},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(frame_rate_control_2), frame_rate_control_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_1), power_control_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_2), power_control_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_3), power_control_3},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_4), power_control_4},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_5), power_control_5},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(VCOM_control_1), VCOM_control_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(extension_command_control_1),
			extension_command_control_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(extension_command_control_2),
			extension_command_control_2},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(power_control_5_1), power_control_5_1},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(VCOM_4level_control), VCOM_4level_control},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(interface_pixel_format), interface_pixel_format},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_positivepolar_format), gamma_positivepolar_format},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_negativepolar_format), gamma_negativepolar_format},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(column_address_set), column_address_set},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(row_address_set), row_address_set},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(tearing_effect_line_on), tearing_effect_line_on},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(memory_data_access_control), memory_data_access_control},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(DISPON), DISPON},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(memory_write), memory_write},
};

static struct dsi_cmd_desc toshiba_panel_off_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(DISPOFF), DISPOFF},

	{DTYPE_GEN_LWRITE, 1, 0, 0, 120,
		sizeof(SLPIN), SLPIN},
};
static struct mipi_panel_data mipi_pd = {
	.panel_name = "DA_177TC-08\n",
	.on = {toshiba_panel_on_cmds
			, ARRAY_SIZE(toshiba_panel_on_cmds)},
	.off = {toshiba_panel_off_cmds
			, ARRAY_SIZE(toshiba_panel_off_cmds)},
};

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x3A, 0x20, 0x05, 0x00, 0x26, 0x2C, 0x09, 0x24, 0x09, 0x03, 0x04, 0x0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_toshiba_tft_video_qqvga_pt_init(void)
{
	int ret;

	/* Landscape */
	pinfo.xres = 128;
	pinfo.yres = 160;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 16; /* RGB888 = 24 bits-per-pixel */
	pinfo.fb_num = 2; /* using two frame buffers */

	/* bitclk */

	pinfo.clk_rate = 200456000;

	pinfo.lcdc.h_front_porch = 166;/* thfp */
	pinfo.lcdc.h_back_porch = 166;	/* thb */
	pinfo.lcdc.h_pulse_width = 180;	/* thpw */

	pinfo.lcdc.v_front_porch = 60;	/* tvfp */
	pinfo.lcdc.v_back_porch = 60;	/* tvb */	
	pinfo.lcdc.v_pulse_width = 200;	/* tvpw */

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

module_init( mipi_toshiba_tft_video_qqvga_pt_init);

