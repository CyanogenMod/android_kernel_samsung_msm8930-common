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
#include "mipi_himax_tft.h"

static struct msm_panel_info pinfo;
static struct mipi_panel_data mipi_pd;


static char SLPIN[2] = {
	0x10,
};

/*needed 120ms*/
static char SLPOUT[2] = {
	0x11,
};
/*needed 500ms*/
static char DISPON[2] = {
	0x29,
};
static char DISPOFF[2] = {
	0x28,
};
#define HX8369B_CMD_SETTLE 0


static char hx8369b_boe1[] = {
	0xB9,
	0xFF, 0x83, 0x69
};
static char hx8369b_boe2[] = {
	0xBA,
	0x31, 0x00, 0x16, 0xCA, 0xB1, 0x0A, 0x00, 0x10, 0x28, 0x02, 0x21, 0x21, 0x9A, 0x1A, 0x8F
};
static char hx8369b_boe3[] = {
	0x3A,
	0x70
};
static char hx8369b_boe4[] = {
	0xD5,
	0x00, 0x00, 0x08, 0x00, 0x0A, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x01, 0x49, 0x37,
	0x00, 0x00, 0x0A, 0x0A, 0x0B, 0x47, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x00, 0x00, 0x26, 0x00, 0x00, 0x91, 0x13, 0x35, 0x57, 0x75, 0x18, 0x00, 
	0x00, 0x00, 0x86, 0x64, 0x42, 0x20, 0x00, 0x49, 0x00, 0x00, 0x00, 0x90, 0x02, 0x24, 0x46, 
	0x64, 0x08, 0x00, 0x00, 0x00, 0x87, 0x75, 0x53, 0x31, 0x11, 0x59, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0xFF, 0x00, 
	0x80, 0x5A
};
static char hx8369b_boe5[] = {
	0xB1,
	0x0C, 0x83, 0x77, 0x00, 0x0F, 0x0F, 0x18, 0x18, 0x0C, 0x02
};
static char hx8369b_boe6[] = {
	0xB2,
	0x00, 0x70
};
static char hx8369b_boe7[] = {
	0xB3,
	0x83, 0x00, 0x31, 0x03
};
static char hx8369b_boe8[] = {
	0xB4,
	0x02
};
static char hx8369b_boe9[] = {
	0xB6,
	0xA0, 0xA0
};

static char hx8369b_boe9_1[] = {
	0xCB,
	0x6D
};

static char hx8369b_boe10[] = {
	0xCC,
	0x0E
};
static char hx8369b_boe11[] = {
	0xC6,
	0x41, 0xFF, 0x7A
};
static char hx8369b_boe12[] = {
	0xEA,
	0x72
};
static char hx8369b_boe13[] = {
	0xE3,
	0x07, 0x0F, 0x07, 0x0F
};
static char hx8369b_boe14[] = {
	0xC0,
	0x73, 0x50, 0x00, 0x34, 0xC4, 0x09
};
static char hx8369b_boe14_1[] = {
	0xC1,
	0x00
};

static char hx8369b_boe15[] = {
	0xE0,
	0x00, 0x07, 0x0C, 0x30, 0x32, 0x3F, 0x1C, 0x3A, 0x08, 0x0D, 0x10, 0x14, 0x16, 0x14, 0x15, 
	0x0E, 0x12, 0x00, 0x07, 0x0C, 0x30, 0x32, 0x3F, 0x1C, 0x3A, 0x08, 0x0D, 0x10, 0x14, 0x16, 
	0x14, 0x15, 0x0E, 0x12, 0x01
};

static struct dsi_cmd_desc hx8369b_video_display_init_boe_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe1), hx8369b_boe1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe2), hx8369b_boe2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe3), hx8369b_boe3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe4), hx8369b_boe4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe5), hx8369b_boe5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe6), hx8369b_boe6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe7), hx8369b_boe7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe8), hx8369b_boe8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe9), hx8369b_boe9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe9_1), hx8369b_boe9_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe10), hx8369b_boe10},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe11), hx8369b_boe11},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe12), hx8369b_boe12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe13), hx8369b_boe13},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe14), hx8369b_boe14},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe14_1), hx8369b_boe14_1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_boe15), hx8369b_boe15},
		{DTYPE_GEN_LWRITE, 0, 0, 0, 120,
				sizeof(SLPOUT), SLPOUT},
		{DTYPE_GEN_LWRITE, 0, 0, 0, 500,
			sizeof(DISPON), DISPON},

};
#if 0 /*not yet used*/
static char SETEXTENSION[] = {
	0xB9,
	0xff, 0x83, 0x69,
};

static char SETMIPI[] = {
	0xBA,
	0x31, 0x00,	0x16, 0xCA, 0xB1,
	0x0A, 0x00, 0x10, 0x28, 0x02, 
	0x21, 0x21, 0x9A, 0x1A, 0x8F,
};


static char SETGIP[] = {
	0xD5,
	0x00, 0x00, 0x09, 0x00, 0x0B,
	0x00, 0x00, 0x10, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x49, 0x37,
	0x00, 0x00, 0x0B, 0x01, 0x49,
	0x37, 0x00, 0x00, 0x00, 0x60,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x91, 0x13,
	0x35, 0x57, 0x75, 0x18, 0x00,
	0x00, 0x00, 0x86, 0x64, 0x42,
	0x20, 0x00, 0x49, 0x00, 0x00,
	0x00, 0x90, 0x02, 0x24, 0x46,
	0x64, 0x08, 0x00, 0x00, 0x00, 
	0x87, 0x75, 0x53, 0x31, 0x11,
	0x59, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x0F,
	0x00, 0x0F, 0xFF, 0xFF, 0x0F,
	0x00, 0x0F, 0xFF, 0xFF, 0x00,
	0x05, 0x5A,
};

static char SETPWD[] = {
	0xB1,
	0x0C, 0x83, 0x77, 0x00, 0x0F,
	0x0F, 0x18, 0x18, 0x0C, 0x02,
};

static char SETPWD2[] = {
	0xB2,
	0x00, 0x70,
};

static char SETPWD3[] = {
	0xB3,
	0x83, 0x00, 0x31, 0x03
};

/*display inversion setting*/
static char SETDIS[] = {
	0xB4,
	0x02,
};

static char SETVCOMV[] = {
	0xB6,
	0xA0, 0xA0,
};	

static char SETVCOMV2[] = {
	0xCB,
	0x6D,
};	

static char SETDISPD[] = {
	0xCC,
	0x02,
};

static char SETIUSED[] = {
	0xC6,
	0x41, 0xFF, 0x7A,
};

static char SETCABC[] = {
	0xEA,
	0x72,
};

static char SETSEQ[] = {
	0xE3,
	0x07, 0x0F, 0x07, 0x0F,
};

static char SETSOUROPT[] = {
	0xC0,
	0x73, 0x50, 0x00, 0x1C, 0xC4,
	0x09,
	
};

static char SETDGC[] = {
	0xC1,
	0x00,
};

static char SETGAMMA[] = {
	0xE0,
	0x00, 0x07, 0x0C, 0x30, 0x32,
	0x3F, 0x1C, 0x3A, 0x08, 0x0D,
	0x10, 0x14, 0x16, 0x14, 0x15,
	0x0E, 0x12, 0x00, 0x07, 0x0C,
	0x30, 0x32, 0x3F, 0x1C, 0x3A,
	0x08, 0x0D, 0x10, 0x14, 0x16,
	0x14, 0x15, 0x0E, 0x12, 0x01,
};

static char hx8369b_auo1[] = {
	0xB9,
	0xFF, 0x83, 0x69
};
static char hx8369b_auo2[] = {
	0x3A,
	0x77
};
static char hx8369b_auo3[] = {
	0xD5,
	0x00, 0x00, 0x13, 0x03, 0x35, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x01, 0x7A, 0x16,
	0x04, 0x04, 0x13, 0x07, 0x40, 0x13, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x40, 0x00, 0x88, 0x88, 0x54, 0x20, 0x00, 
	0x00, 0x00, 0x10, 0x00, 0x88, 0x88, 0x67, 0x13, 0x50, 0x00, 0x00, 0x50, 0x00, 0x88, 0x88, 
	0x76, 0x31, 0x10, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x45, 0x02, 0x40, 0x00, 0x00, 0x00, 
	0x51, 0x00, 0x00, 0x00, 0x0A, 0X00, 0xEF, 0x00, 0xEF, 0x0A, 0x00, 0xEF, 0x00, 0xEF, 0x00, 
	0x01, 0x5A
};
static char hx8369b_auo3_1[] = {
	0xD5,	
	0x00, 0x00, 0x13, 0x03, 0x35, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x01, 0x7A, 0x16, 
	0x04, 0x04, 0x13, 0x07, 0x40, 0x13, 0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x48, 0x88, 0x85, 0x42, 0x00, 0x99, 0x99, 
	0x00, 0x00, 0x18, 0x88, 0x86, 0x71, 0x35, 0x99, 0x99, 0x00, 0x00, 0x58, 0x88, 0x87, 0x63, 
	0x11, 0x99, 0x99, 0x00, 0x00, 0x08, 0x88, 0x84, 0x50, 0x24, 0x99, 0x99, 0x00, 0x00, 0x00, 
	0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00, 
	0x01, 0x5A
};
static char hx8369b_auo4[] = {
	0xBA,
	0x31, 0x00, 0x16, 0xCA, 0xB1, 0x0A, 0x00, 0x10, 0x28, 0x02, 0x21, 0x21, 0x9A, 0x1A, 0x8F
};
static char hx8369b_auo5[] = {
	0xB1,
	0x09, 0x83, 0x67, 0x00, 0x92, 0x12, 0x16, 0x16, 0x0C, 0x02
};
static char hx8369b_auo6[] = {
	0xB2,
	0x00, 0x70
};
static char hx8369b_auo7[] = {
	0xE0,
	0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3E, 0x07, 0x0D, 0x0E, 0x12, 0x13, 0x12, 0x14,
	0x13, 0x1A, 0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3E, 0x07, 0x0D, 0x0E, 0x12, 0x13,
	0x12, 0x14, 0x13, 0x1A, 0x01
};
static char hx8369b_auo7_1[] = {
	0xE0,
	0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3D, 0x07, 0x0D, 0x0E, 0x12, 0x13, 0x12, 0x13, 
	0x11, 0x1A, 0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3D, 0x07, 0x0D, 0x0E, 0x12, 0x13, 
	0x12, 0x13, 0x11, 0x1A, 0x01
};
static char hx8369b_auo8[] = {
	0xC1,
	0x03, 0x00, 0x09, 0x11, 0x18, 0x1E, 0x27, 0x2F, 0x36, 0x3E, 0x45, 0x4C, 0x54, 0x5C, 0x64, 
	0x6B, 0x73, 0x7C, 0x83, 0x8B, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC, 0xC4, 0xCD, 0xD5, 0xDD, 
	0xE6, 0xEE, 0xF7, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 
	0x11, 0x18, 0x1E, 0x27, 0x2F, 0x36, 0x3E, 0x45, 0x4C, 0x54, 0x5C, 0x64, 0x6B, 0x73, 0x7C, 
	0x83, 0x8B, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC, 0xC4, 0xCD, 0xD5, 0xDD, 0xE6, 0xEE, 0xF7, 
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x18, 0x1E, 
	0x27, 0x2F, 0x36, 0x3E, 0x45, 0x4C, 0x54, 0x5C, 0x64, 0x6B, 0x73, 0x7C, 0x83, 0x8B, 0x94, 
	0x9C, 0xA4, 0xAC, 0xB4, 0xBC, 0xC4, 0xCD, 0xD5, 0xDD, 0xE6, 0xEE, 0xF7, 0xFF, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char hx8369b_auo9[] = {
	0xB3,
	0x83, 0x00, 0x31, 0x03, 0x01, 0x15, 0x14
};
static char hx8369b_auo10[] = {
	0xB4,
	0x02
};
static char hx8369b_auo11[] = {
	0xB5,
	0x0B, 0x0B, 0x24
};
static char hx8369b_auo12[] = {
	0xCC,
	0x02
};
static char hx8369b_auo13[] = {
	0xC6,
	0x41, 0xFF, 0x7A
};
static char hx8369b_auo14[] = {
	0xC0,
	0x73, 0x50, 0x00, 0x34, 0xC4, 0x02
};
static char hx8369b_auo15[] = {
	0xE3,
	0x00, 0x00, 0x13, 0x1B
};
static char hx8369b_auo16[] = {
	0xCB,
	0x6D
};
static char hx8369b_auo17[] = {
	0xEA,
	0x62
};

static char COLMOD[] = {
	0x3a,
	0x77,
};


static struct dsi_cmd_desc hx8369b_video_display_init_auo1_cmds[] = {
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo1), hx8369b_auo1},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo2), hx8369b_auo2},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo3), hx8369b_auo3},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo4), hx8369b_auo4},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo5), hx8369b_auo5},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo6), hx8369b_auo6},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo7), hx8369b_auo7},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo8), hx8369b_auo8},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo9), hx8369b_auo9},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo10), hx8369b_auo10},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo11), hx8369b_auo11},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo12), hx8369b_auo12},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo13), hx8369b_auo13},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo14), hx8369b_auo14},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo15), hx8369b_auo15},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo16), hx8369b_auo16},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo17), hx8369b_auo17},

	{DTYPE_GEN_LWRITE, 0, 0, 0, 120,
			sizeof(SLPOUT), SLPOUT},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 500,
		sizeof(DISPON), DISPON},

};

static struct dsi_cmd_desc hx8369b_video_display_init_auo2_cmds[] = {
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo1), hx8369b_auo1},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo2), hx8369b_auo2},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo3_1), hx8369b_auo3_1},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo4), hx8369b_auo4},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo5), hx8369b_auo5},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo6), hx8369b_auo6},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo7_1), hx8369b_auo7_1},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo8), hx8369b_auo8},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo9), hx8369b_auo9},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo10), hx8369b_auo10},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo11), hx8369b_auo11},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo12), hx8369b_auo12},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo13), hx8369b_auo13},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo14), hx8369b_auo14},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo15), hx8369b_auo15},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo16), hx8369b_auo16},
	{DTYPE_GEN_LWRITE, 0, 0, 0, HX8369B_CMD_SETTLE, sizeof(hx8369b_auo17), hx8369b_auo17},

	{DTYPE_GEN_LWRITE, 0, 0, 0, 20,
		sizeof(DISPON), DISPON},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 120,
			sizeof(SLPOUT), SLPOUT},

};

static struct dsi_cmd_desc samsung_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETEXTENSION), SETEXTENSION},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETMIPI), SETMIPI},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(COLMOD), COLMOD},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETGIP), SETGIP},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETPWD), SETPWD},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETPWD2), SETPWD2},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETPWD3), SETPWD3},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETDIS), SETDIS},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETVCOMV), SETVCOMV},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETVCOMV2), SETVCOMV2},	
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETDISPD), SETDISPD},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETIUSED), SETIUSED},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETCABC), SETCABC},	
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETSEQ), SETSEQ},	
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETSOUROPT), SETSOUROPT},	
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETDGC), SETDGC},		
	{DTYPE_GEN_LWRITE, 0, 0, 0, 0,
		sizeof(SETGAMMA), SETGAMMA},	
	{DTYPE_GEN_LWRITE, 0, 0, 0, 120,
			sizeof(SLPOUT), SLPOUT},
	{DTYPE_GEN_LWRITE, 0, 0, 0, 500,
		sizeof(DISPON), DISPON},	

};
#endif
static struct dsi_cmd_desc samsung_panel_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(DISPON), DISPON},
};
static struct dsi_cmd_desc samsung_panel_ready_to_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 5,
		sizeof(DISPOFF), DISPOFF},
};

static struct dsi_cmd_desc samsung_panel_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(SLPIN), SLPIN},
};

static struct dsi_cmd_desc samsung_panel_late_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(DISPON), DISPON},
};

static struct dsi_cmd_desc samsung_panel_early_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(DISPOFF), DISPOFF},
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
	2,
	5, 7, 9, 10, 11, 13, 14, 15, 16,17,
	18, 19, 20, 22,	24, 25, 26, 27, 28,	29,
	30, 31, 32, 0,
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
	.panel_name = "JDI_L5F31188T07\n",
#if 0
	.ready_to_on = {hx8369b_video_display_init_auo2_cmds
				, ARRAY_SIZE(hx8369b_video_display_init_auo2_cmds)},
#else
	.ready_to_on = {hx8369b_video_display_init_boe_cmds
				, ARRAY_SIZE(hx8369b_video_display_init_boe_cmds)},
#endif
	.ready_to_off	= {samsung_panel_ready_to_off_cmds
				, ARRAY_SIZE(samsung_panel_ready_to_off_cmds)},
	.on		= {samsung_panel_on_cmds
				, ARRAY_SIZE(samsung_panel_on_cmds)},
	.off		= {samsung_panel_off_cmds
				, ARRAY_SIZE(samsung_panel_off_cmds)},
	.late_on	= {samsung_panel_late_on_cmds
				, ARRAY_SIZE(samsung_panel_late_on_cmds)},
	.early_off	= {samsung_panel_early_off_cmds
				, ARRAY_SIZE(samsung_panel_early_off_cmds)},
	.set_brightness_level = get_candela_index,
};

#ifdef CONFIG_CAMERA_PICTURE_QUALIRY_TEST /*not used!!!!!*/
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI_BIT_CLK at 450MHz, 4 lane, RGB888 */
	{0x09, 0x08, 0x05, 0x00, 0x20},    /* regulator */
	/* timing */
	{0xb7, 0x2b, 0x1d, 0x00, 0x57, 0x62, 0x22, 0x2f,
	0x32, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x30, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};



static int __init mipi_video_himax_tft_hd_pt_init(void)
{
	int ret;
#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_himax_tft_hd"))
		return 0;
#endif

	pinfo.xres = 720;
	pinfo.yres = 1280;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_pulse_width = 36;
	pinfo.lcdc.h_back_porch = 108;
	pinfo.lcdc.h_front_porch = 108;
	pinfo.lcdc.v_pulse_width = 2;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 11;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 450000000;

/*
	mipi clk =[(720+108+108+36)*(1280+11+4+2)*24*60]/4(lanes)
		=453846240Hz

	pixel clk=(mipi clk *4)/24
		=75641040Hz
*/
	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = TRUE;
	pinfo.mipi.hsa_power_stop = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_PULSE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;

	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x00;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.data_lane2 = TRUE;
	pinfo.mipi.data_lane3 = TRUE;

	pinfo.mipi.force_clk_lane_hs = 1;
	pinfo.mipi.no_max_pkt_size = 1;

	pinfo.mipi.tx_eot_append = FALSE;
	pinfo.mipi.t_clk_post = 0x20;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;

	pinfo.mipi.esc_byte_ratio = 4;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	ret = mipi_samsung_device_register(&pinfo, MIPI_DSI_PRIM,
				MIPI_DSI_PANEL_720P_PT,
				&mipi_pd);
	if (ret)
		pr_info("%s: failed to register device!\n", __func__);

		pr_info("%s:\n", __func__);
	return ret;
}

#else
/*WVGA*/
#if 1
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x8A, 0x33, 0x14, 0x00, 0x45, 0x4A, 0x19, 0x37, 0x23, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};
#else
#if 1
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI Bit Clock at 492 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x03, 0x01, 0x01, 0x00},		
	/* timing */
	{0xb8, 0x8d, 0x1f, 0x00, 0x97, 0x94, 0x22, 0x8f,
	0x18, 0x03, 0x04},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xbb, 0x02, 0x06, 0x00},	
	/* pll control */
	{0x00, 0xe5, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};*/
#endif
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xb8, 0x8d, 0x1f, 0x00, 0x97, 0x94, 0x22, 0x8f, 0x18, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

#endif
static int __init mipi_video_himax_tft_hd_pt_init(void)
{
	int ret;
#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_himax_tft_hd"))
		return 0;
#endif

	pinfo.xres = 480;
	pinfo.yres = 800;
//	pinfo.height = 101;
//	pinfo.width = 57;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_pulse_width = 32;
	pinfo.lcdc.h_back_porch = 135;
	pinfo.lcdc.h_front_porch = 150;
	pinfo.lcdc.v_pulse_width = 2;
	pinfo.lcdc.v_back_porch = 22;
	pinfo.lcdc.v_front_porch = 20;
	
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 492000000;

/*
	mipi clk =[(480+h_back_porch+h_front_porch+h_pulse_width)*(800+v_front_porch+v_back_porch+v_pulse_width)*24*60]/4(lanes)
	mipi clk =[(480+135+150+32)*(800+20+22+2)*24*60]/4(lanes)

		=797*844*24*60/2 =484

	pixel clk=(mipi clk *4)/24
		=484*4/24= 80 
*/
	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = FALSE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;

	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;

	/* BOARD_JASPER2_VZW/BOARD_HIGGS_SPR */
	if (system_rev == 0)
		pinfo.mipi.dlane_swap = 0x00;
	else
		pinfo.mipi.dlane_swap = 0x01;
	
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;

//	pinfo.mipi.no_max_pkt_size = 0;

	pinfo.mipi.tx_eot_append = 0x01;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;//DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.force_clk_lane_hs = 1;
	pinfo.mipi.esc_byte_ratio = 2;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	ret = mipi_samsung_device_register(&pinfo, MIPI_DSI_PRIM,
				MIPI_DSI_PANEL_WVGA_PT,
				&mipi_pd);
	if (ret)
		pr_info("%s: failed to register device!\n", __func__);

		pr_info("%s:\n", __func__);
	return ret;
}
#endif
module_init(mipi_video_himax_tft_hd_pt_init);
