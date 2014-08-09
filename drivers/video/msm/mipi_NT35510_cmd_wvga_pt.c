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
 */

#include "msm_fb.h"
#include "mipi_NT35510.h"

static struct msm_panel_info pinfo;
static struct mipi_panel_data mipi_pd;

static char SLPIN[2] = { 0x10, };
static char SLPOUT[2] = { 0x11, };
static char DISPON[2] = { 0x29, };
static char DISPOFF[2] = { 0x28, };

#define NT35510_CMD_SETTLE 0

static char nt35510_boe1[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x01
};
static char nt35510_boe2[] = {
	0xB0,
	0x09, 0x09, 0x09
};
static char nt35510_boe3[] = {
	0xB6,
	0x34, 0x34, 0x34
};
static char nt35510_boe4[] = {
	0xB1,
	0x09, 0x09, 0x09
};
static char nt35510_boe5[] = {
	0xB7,
	0x24, 0x24, 0x24
};
static char nt35510_boe6[] = {
	0xB3,
	0x05, 0x05, 0x05
};
static char nt35510_boe7[] = {
	0xB9,
	0x25, 0x25, 0x25
};
static char nt35510_boe8[] = {
	0xBF,
	0x01
};
static char nt35510_boe9[] = {
	0xB5,
	0x0B, 0x0B, 0x0B
};
static char nt35510_boe10[] = {
	0xBA,
	0x24, 0x24, 0x24
};
static char nt35510_boe11[] = {
	0xC2,
	0x01
};
static char nt35510_boe12[] = {
	0xBC,
	0x00, 0xA3, 0x00
};
static char nt35510_boe13[] = {
	0xBD,
	0x00, 0xA3, 0x00
};

static char nt35510_boe14[] = {
	0xD1,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x83, 0x02, 0x78, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};
static char nt35510_boe15[] = {
	0xD2,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x83, 0x02, 0x78, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};
static char nt35510_boe16[] = {
	0xD3,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x83, 0x02, 0x78, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};
static char nt35510_boe17[] = {
	0xD4,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x43, 0x02, 0x50, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};
static char nt35510_boe18[] = {
	0xD5,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x43, 0x02, 0x50, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};
static char nt35510_boe19[] = {
	0xD6,
	0x00, 0x01, 0x00, 0x43, 0x00, 0x6B, 0x00, 0x87,
	0x00, 0xA3, 0x00, 0xCE, 0x00, 0xF1, 0x01, 0x27,
	0x01, 0x53, 0x01, 0x98, 0x01, 0xCE, 0x02, 0x22,
	0x02, 0x43, 0x02, 0x50, 0x02, 0x9E, 0x02, 0xDD,
	0x03, 0x00, 0x03, 0x2E, 0x03, 0x54, 0x03, 0x7F,
	0x03, 0x95, 0x03, 0xB3, 0x03, 0xC2, 0x03, 0xE1,
	0x03, 0xF1, 0x03, 0xFE
};

static char nt35510_boe20[] = {
	0xF0,
	0x55, 0xAA, 0x52, 0x08, 0x00
};
static char nt35510_boe21[] = {
	0xB1,
	0x4C, 0x04
};
static char nt35510_boe22[] = {	//x-flip register, jh0223
	0x36,
	0x02
};
static char nt35510_boe23[] = {
	0xB6,
	0x0A
};
static char nt35510_boe24[] = {
	0xB7,
	0x00, 0x00
};
static char nt35510_boe25[] = {
	0xB8,
	0x01, 0x05, 0x05, 0x05
};
static char nt35510_boe26[] = {
	0xBA,
	0x01
};
static char nt35510_boe27[] = {
	0xBD,
	0x01, 0x84, 0x07, 0x32, 0x00
};
static char nt35510_boe28[] = {
	0xBE,
	0x01, 0x84, 0x07, 0x31, 0x00
};
static char nt35510_boe29[] = {
	0xBF,
	0x01, 0x84, 0x07, 0x31, 0x00
};
static char nt35510_boe30[] = {
	0x35,
	0x00
};
static char nt35510_boe31[] = {
	0x2A,
	0x00, 0x00, 0x01, 0xDF
};
static char nt35510_boe32[] = {
	0x2B,
	0x00, 0x00, 0x03, 0x1F
};
static char nt35510_boe33[] = {
	0x3A,
	0x77
};
static char nt35510_boe34[] = {
	0xCC,
	0x03, 0x00, 0x00
};

static char nt35510_deepsleep[] = {
	0x4F,
	0x01
};
static struct dsi_cmd_desc nt35510_cmd_display_init_cmds[] = {
	{DTYPE_DCS_LWRITE, 0, 0, 0, 120,                sizeof(SLPOUT),          SLPOUT},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe1),    nt35510_boe1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe2),    nt35510_boe2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe3),    nt35510_boe3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe4),    nt35510_boe4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe5),    nt35510_boe5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe6),    nt35510_boe6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe7),    nt35510_boe7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe8),    nt35510_boe8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe9),    nt35510_boe9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe10),   nt35510_boe10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe11),   nt35510_boe11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe12),   nt35510_boe12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 120,                sizeof(nt35510_boe13),   nt35510_boe13},

	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe14),   nt35510_boe14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe15),   nt35510_boe15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe16),	 nt35510_boe16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe17),	 nt35510_boe17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe18),	 nt35510_boe18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe19),	 nt35510_boe19},

	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe20),   nt35510_boe20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe21),   nt35510_boe21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe22),	 nt35510_boe22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE,	sizeof(nt35510_boe23),	 nt35510_boe23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe24),	 nt35510_boe24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe25),	 nt35510_boe25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE,	sizeof(nt35510_boe26),	 nt35510_boe26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe27),	 nt35510_boe27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe28),	 nt35510_boe28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE,	sizeof(nt35510_boe29),	 nt35510_boe29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe30),	 nt35510_boe30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe31),	 nt35510_boe31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE,	sizeof(nt35510_boe32),	 nt35510_boe32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe33),	 nt35510_boe33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, NT35510_CMD_SETTLE, sizeof(nt35510_boe34),	 nt35510_boe34},
	{DTYPE_DCS_LWRITE, 0, 0, 0, 10,                 sizeof(DISPON),          DISPON},
};


static struct dsi_cmd_desc nt35510_panel_on_cmds[] = {
{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(DISPON), DISPON},
};
static struct dsi_cmd_desc nt35510_panel_ready_to_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(DISPOFF), DISPOFF},
};

static struct dsi_cmd_desc nt35510_panel_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 80, sizeof(SLPIN), SLPIN},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt35510_deepsleep), nt35510_deepsleep},		
};

static struct dsi_cmd_desc nt35510_panel_late_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,sizeof(DISPON), DISPON},
};

static struct dsi_cmd_desc nt35510_panel_early_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(DISPOFF), DISPOFF},
};


enum {
	GAMMA_15CD,
	GAMMA_21CD,
	GAMMA_37CD,
	GAMMA_56CD,
	GAMMA_61CD,
	GAMMA_100CD,
	GAMMA_110CD,
	GAMMA_134CD,
	GAMMA_146CD,
	GAMMA_159CD,
	GAMMA_172CD,
	GAMMA_190CD,
	GAMMA_200CD,
	GAMMA_212CD,
	GAMMA_224CD,
	GAMMA_252CD,
	GAMMA_260CD,
	GAMMA_270CD,
	GAMMA_280CD,
	GAMMA_290CD,
	GAMMA_300CD,
	GAMMA_312CD,
	GAMMA_320CD,
	GAMMA_360CD,
};

static int lux_tbl[] = {
	7,
	8,  9,  10,  11, 12, 13, 14, 15, 16, 17,
	19, 20, 21, 22,	23, 24, 25, 26, 27,	28,
	29, 30, 32, 0,
};

static int get_candela_index(int bl_level)
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
		case 210 ... 219:
			backlightlevel = 5;  /* 11 */
			break;
		case 220 ... 229:
			backlightlevel = 3;  /* 9 */
			break;
		case 230 ... 239:
			backlightlevel = 2;  /* 7 */
			break;
		case 240 ... 249:
			backlightlevel = 1;  /* 5 */
			break;
		case 250 ... 255:
			backlightlevel = 0;  /* 2 */
			break;
		default:
			backlightlevel = 23; /*32*/
			break;
		}

	cd = lux_tbl[backlightlevel];
	return cd;
}

static struct mipi_panel_data mipi_pd = {
	.panel_name = "INH_55BCC0\n",
	.ready_to_on = {nt35510_cmd_display_init_cmds
				, ARRAY_SIZE(nt35510_cmd_display_init_cmds)},
	.ready_to_off	= {nt35510_panel_ready_to_off_cmds
				, ARRAY_SIZE(nt35510_panel_ready_to_off_cmds)},
	.on		= {nt35510_panel_on_cmds
				, ARRAY_SIZE(nt35510_panel_on_cmds)},
	.off		= {nt35510_panel_off_cmds
				, ARRAY_SIZE(nt35510_panel_off_cmds)},
	.late_on	= {nt35510_panel_late_on_cmds
				, ARRAY_SIZE(nt35510_panel_late_on_cmds)},
	.early_off	= {nt35510_panel_early_off_cmds
				, ARRAY_SIZE(nt35510_panel_early_off_cmds)},
	.set_brightness_level = get_candela_index,
};

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xb9, 0x8e, 0x13, 0x00, 0x98, 0x9c, 0x13, 0x90,
	0x18, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_cmd_nt35510_wvga_pt_init(void)
{
	int ret;
#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_cmd_nt35510_wvga"))
		return 0;
#endif

	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.height = 101;
	pinfo.width = 60;	
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_pulse_width = 4;
	pinfo.lcdc.h_back_porch  = 11;
	pinfo.lcdc.h_front_porch = 5;
	pinfo.lcdc.v_pulse_width = 4;
	pinfo.lcdc.v_back_porch  = 8;
	pinfo.lcdc.v_front_porch = 4;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 320000000;

	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.primary_start_pos = 400;
	pinfo.lcd.refx100 = 6150; /* adjust refx100 to prevent tearing */
	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x20;
	pinfo.mipi.t_clk_pre = 0x3F;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60; /* FIXME */
	pinfo.mipi.te_sel = 1; /* TE from vsync gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.esc_byte_ratio = 2;	
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
	pinfo.mipi.tx_eot_append = 0x01;
	pinfo.mipi.rx_eot_ignore = 0x0;
	pinfo.mipi.dlane_swap = 0x01;

	ret = mipi_samsung_device_register(&pinfo, MIPI_DSI_PRIM,
				MIPI_DSI_PANEL_WVGA_PT,
				&mipi_pd);
	if (ret)
		pr_info("%s: failed to register device!\n", __func__);

		pr_info("%s:\n", __func__);

	return ret;
}

module_init(mipi_cmd_nt35510_wvga_pt_init);
