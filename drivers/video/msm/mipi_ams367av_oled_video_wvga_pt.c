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
#include "mipi_ams367av_oled.h"

static struct msm_panel_info pinfo;
static struct mipi_panel_data mipi_pd;

#if 0
#define MIPI_CLK (450000000)
#endif
#define MIPI_CLK (365000000) /* 130412 HW request */
#define USE_AOR (1)
#define HBM_OVERWRITING	(1)

static int lux_tbl[] = {
	10, 11, 12, 13, 14,
	15, 16, 17, 19, 20,
	21, 22, 24, 25, 27,
	29, 30, 32, 34, 37,
	39, 41, 44, 47, 50,
	53, 56, 60, 64, 68,
	72, 77, 82, 87, 93,
	98, 105, 111, 119, 126,
	134, 143, 152, 162, 172,
	183, 195, 207, 220, 234,
	249, 265, 282, 300,
};

static char samsung_nop[] = {
	0x0, 0x0, 0x0, 0x0,
};

static char samsung_test_key_on1[] = {
	0xF0,
	0x5A, 0x5A,
};

static char samsung_test_key_on2[] = {
	0xF1,
	0x5A, 0x5A,
};

static char samsung_test_key_on3[] = {
	0xFC,
	0x5A, 0x5A,
};

static char samsung_test_key_off1[] = {
	0xF0,
	0xA5, 0xA5,
};

static char samsung_test_key_off3[] = {
	0xFC,
	0xA5, 0xA5,
};

/*
static char samsung_touchkey_off[] = {
	0xFF,
	0x1F,
};
*/
static char samsung_touchkey_on[] = {
	0xFF,
	0x07,
};

static char samsung_brightness_gamma[] = {
	0xCA,
	0x01, 0x00, 0x01, 0x00,
	0x01, 0x00, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x02, 0x03,
	0x02,
};

static char samsung_gamma_update[] = {
	0xF7,
	0x03,
};

#ifdef USE_AOR
static char samsung_brightness_aor_condition[] = {
	0xB2,
	0x40, 0x07, 0x1C, 0x00,
	0x08,
};
#endif /* USE_AOR */

static char samsung_elvss_condition1[] = {
	0xB6,
	0x28,
};

static char samsung_elvss_condition2[] = {
	0xB0,
	0x01,
};

static char samsung_elvss_condition3[] = {
	0xB6,
	0x0B,
};

static char samsung_acl_condition[] = {
	0x55,
	0x00,
};

static char samsung_avc_set_global[] = {
	0xB0,
	0x03,
};

static char samsung_avc_set_power_control[] = {
	0xF4,
	0x00,
};

static char samsung_src_latch_set_global_1[] = {
	0xB0,
	0x11,
};

static char samsung_src_latch_set_1[] = {
	0xFD,
	0x11,
};

static char samsung_src_latch_set_global_2[] = {
	0xB0,
	0x13,
};

static char samsung_src_latch_set_2[] = {
	0xFD,
	0x18,
};

#if 0 /* build warning */
static char samsung_aid_condition[] = {
	0xB2,
	0x40, 0x06, 0x18, 0x00,
	0x06,
};

static char samsung_aid_default_R01[] = {
	0xB2,
	0x00, 0x00, 0x00, 0x00,
	0x00,
};
#endif

#if (MIPI_CLK==450000000)
static char samsung_panel_condition[] = {
	0xCB,
	0x06, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x02, 0x00, 0x00,
	0x30, 0x67, 0x89, 0x00, 0x59,
	0xC6, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x60, 0xB1,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0E,
	0x71, 0x0E, 0x71, 0x05, 0x05,
	0x20, 0x20, 0x20, 0x00, 0x00,
	0x00, 0x05, 0x80, 0x80, 0x0C,
	0x01,
};
#elif (MIPI_CLK==365000000)	/* 130509 updated */
static char samsung_panel_condition[] = {
	0xCB,
	0x06, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x02, 0x00, 0x00,
	0x30, 0x67, 0x89, 0x00, 0x48,
	0xA1, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x4E, 0x90,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0B,
	0x5B, 0x0B, 0x5B, 0x04, 0x04,
	0x1A, 0x1A, 0x1A, 0x00, 0x00,
	0x00, 0x04, 0x80, 0x80, 0x0C,
	0x01
};
#else
#error MIPI_CLK unknown case
#endif


#ifdef USE_AOR
static char samsung_brightness_aor_default[] = {
	0xB2,
	0x40, 0x07, 0x1C, 0x00,
	0x00,
};

static char samsung_brightness_aor_ref[] = {
	0xB2,
	0x40, 0x07, 0x1C, 0x00,
	0x08,
};

static char samsung_brightness_aor_pre[] = {
	0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00,
};
#endif /* USE_AOR */

static char samsung_brightness_elvss1_default[] = {
	0xB6,
	0x28,
};

/* Register B0 is used in many case */
static char samsung_brightness_elvss1_global_para[] = {
	0xB0,
	0x01,
};

static char samsung_brightness_elvss3_ref[] = {
	0xB6,
	0x0B,
};

static char samsung_brightness_elvss3_default[] = {
	0xB6,
	0x0B,
};

static char samsung_brightness_acl_default[] = {
	0x55,
	0x00,
};

static char samsung_brightness_acl_ref[] = {
	0x55,
	0x00,
};

static char samsung_brightness_acl_pre[] = {
	0x00,
	0x00,
};

#if 0 /* build warning */
static char samsung_display_control_vporch[] = {
	0xF2,
	0x03, 0x0C,
};
#endif

static char samsung_temperature[] = {
	0xB6,
	0x2C,
};

static char samsung_display_on[] = { 0x29, /* no param */ };
static char samsung_display_off[] = { 0x28, /* no param */ };
static char samsung_sleep_in[] = { 0x10, /* no param */ };
static char samsung_sleep_out[] = { 0x11, /* no param */ };

static struct dsi_cmd_desc samsung_on_cmds [] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_on1), samsung_test_key_on1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_on2), samsung_test_key_on2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_on3), samsung_test_key_on3},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_src_latch_set_global_1), samsung_src_latch_set_global_1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_src_latch_set_1), samsung_src_latch_set_1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_src_latch_set_global_2), samsung_src_latch_set_global_2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_src_latch_set_2), samsung_src_latch_set_2},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 20,
		sizeof(samsung_sleep_out), samsung_sleep_out},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_brightness_gamma), /* ar first disp_on, 300cd */
			samsung_brightness_gamma},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_brightness_aor_ref),
			samsung_brightness_aor_ref},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_gamma_update),
			samsung_gamma_update},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_elvss_condition1), samsung_elvss_condition1},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_elvss_condition2), samsung_elvss_condition2},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_elvss_condition3), samsung_elvss_condition3},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_acl_condition), samsung_acl_condition},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 120,
		sizeof(samsung_panel_condition), samsung_panel_condition},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_avc_set_global), samsung_avc_set_global},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(samsung_avc_set_power_control), samsung_avc_set_power_control},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_display_on), samsung_display_on},
};

static struct dsi_cmd_desc panel_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 40,
		sizeof(samsung_display_off), samsung_display_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 120,
		sizeof(samsung_sleep_in), samsung_sleep_in},
};

static struct dsi_cmd_desc panel_late_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 5,
		sizeof(samsung_display_on), samsung_display_on},
};

static struct dsi_cmd_desc panel_early_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_sleep_in), samsung_sleep_in},
};

static struct dsi_cmd_desc panel_mtp_enable_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_on1), samsung_test_key_on1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_on3), samsung_test_key_on3},
};

static struct dsi_cmd_desc panel_mtp_disable_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_off3), samsung_test_key_off3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_test_key_off1), samsung_test_key_off1},
};

static struct dsi_cmd_desc panel_need_flip_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_panel_condition), samsung_panel_condition},
};

static struct dsi_cmd_desc panel_temperature[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_temperature), samsung_temperature},
};

static struct dsi_cmd_desc panel_touchkey_on[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(samsung_touchkey_on), samsung_touchkey_on},
};

static struct dsi_cmd_desc brightness_packet[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*elvss*/
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*elvss*/
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*elvss*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*aor*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*acl*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*gamma*/
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(samsung_nop), samsung_nop},/*gamma update*/
};

static struct dsi_cmd_desc panel_acl_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(samsung_brightness_acl_ref), samsung_brightness_acl_ref},
};

enum {
	GAMMA_10CD,
	GAMMA_11CD,
	GAMMA_12CD,
	GAMMA_13CD,
	GAMMA_14CD,
	GAMMA_15CD,
	GAMMA_16CD,
	GAMMA_17CD,
	GAMMA_19CD,
	GAMMA_20CD,
	GAMMA_21CD,
	GAMMA_22CD,
	GAMMA_24CD,
	GAMMA_25CD,
	GAMMA_27CD,
	GAMMA_29CD,
	GAMMA_30CD,
	GAMMA_32CD,
	GAMMA_34CD,
	GAMMA_37CD,
	GAMMA_39CD,
	GAMMA_41CD,
	GAMMA_44CD,
	GAMMA_47CD,
	GAMMA_50CD,
	GAMMA_53CD,
	GAMMA_56CD,
	GAMMA_60CD,
	GAMMA_64CD,
	GAMMA_68CD,
	GAMMA_72CD,
	GAMMA_77CD,
	GAMMA_82CD,
	GAMMA_87CD,
	GAMMA_93CD,
	GAMMA_98CD,
	GAMMA_105CD,
	GAMMA_111CD,
	GAMMA_119CD,
	GAMMA_126CD,
	GAMMA_134CD,
	GAMMA_143CD,
	GAMMA_152CD,
	GAMMA_162CD,
	GAMMA_172CD,
	GAMMA_183CD,
	GAMMA_195CD,
	GAMMA_207CD,
	GAMMA_220CD,
	GAMMA_234CD,
	GAMMA_249CD,
	GAMMA_265CD,
	GAMMA_282CD,
	GAMMA_300CD,
};

static int get_candela_index(int bl_level)
{
	int backlightlevel;

	/* brightness setting from platform is from 0 to 255
	 * But in this driver, brightness is only supported from 0 to 24 */

	backlightlevel = 0;

	switch (bl_level) {
	case 0 ... 10:
		backlightlevel = GAMMA_10CD;
		break;
	case 11:
		backlightlevel = GAMMA_11CD;
		break;
	case 12:
		backlightlevel = GAMMA_12CD;
		break;
	case 13:
		backlightlevel = GAMMA_13CD;
		break;
	case 14:
		backlightlevel = GAMMA_14CD;
		break;
	case 15:
		backlightlevel = GAMMA_15CD;
		break;
	case 16:
		backlightlevel = GAMMA_16CD;
		break;
	case 17 ... 18:
		backlightlevel = GAMMA_17CD;
		break;
	case 19:
		backlightlevel = GAMMA_19CD;
		break;
	case 20:
		backlightlevel = GAMMA_20CD;
		break;
	case 21:
		backlightlevel = GAMMA_21CD;
		break;
	case 22 ... 23:
		backlightlevel = GAMMA_22CD;
		break;
	case 24:
		backlightlevel = GAMMA_24CD;
		break;
	case 25 ... 26:
		backlightlevel = GAMMA_25CD;
		break;
	case 27 ... 28:
		backlightlevel = GAMMA_27CD;
		break;
	case 29:
		backlightlevel = GAMMA_29CD;
		break;
	case 30 ... 31:
		backlightlevel = GAMMA_30CD;
		break;
	case 32:
		backlightlevel = GAMMA_32CD;
		break;
	case 33 ... 36:
		backlightlevel = GAMMA_34CD;
		break;
	case 37 ... 38:
		backlightlevel = GAMMA_37CD;
		break;
	case 39 ... 40:
		backlightlevel = GAMMA_39CD;
		break;
	case 41 ... 43:
		backlightlevel = GAMMA_41CD;
		break;
	case 44 ... 46:
		backlightlevel = GAMMA_44CD;
		break;
	case 47 ... 49:
		backlightlevel = GAMMA_47CD;
		break;
	case 50 ... 52:
		backlightlevel = GAMMA_50CD;
		break;
	case 53 ... 55:
		backlightlevel = GAMMA_53CD;
		break;
	case 56 ... 59:
		backlightlevel = GAMMA_56CD;
		break;
	case 60 ... 63:
		backlightlevel = GAMMA_60CD;
		break;
	case 64 ... 67:
		backlightlevel = GAMMA_64CD;
		break;
	case 68 ... 71:
		backlightlevel = GAMMA_68CD;
		break;
	case 72 ... 76:
		backlightlevel = GAMMA_72CD;
		break;
	case 77 ... 81:
		backlightlevel = GAMMA_77CD;
		break;
	case 82 ... 86:
		backlightlevel = GAMMA_82CD;
		break;
	case 87 ... 92:
		backlightlevel = GAMMA_87CD;
		break;
	case 93 ... 97:
		backlightlevel = GAMMA_93CD;
		break;
	case 98 ... 104:
		backlightlevel = GAMMA_98CD;
		break;
	case 105 ... 110:
		backlightlevel = GAMMA_105CD;
		break;
	case 111 ... 118:
		backlightlevel = GAMMA_111CD;
		break;
	case 119 ... 125:
		backlightlevel = GAMMA_119CD;
		break;
	case 126 ... 133:
		backlightlevel = GAMMA_126CD;
		break;
	case 134 ... 142:
		backlightlevel = GAMMA_134CD;
		break;
	case 143 ... 149:
		backlightlevel = GAMMA_143CD;
		break;
	case 150 ... 159:
		backlightlevel = GAMMA_152CD;
		break;
	case 160 ... 170:
		backlightlevel = GAMMA_162CD;
		break;
	case 171 ... 182:
		backlightlevel = GAMMA_172CD;
		break;
	case 183 ... 194:
		backlightlevel = GAMMA_183CD;
		break;
	case 195 ... 206:
		backlightlevel = GAMMA_195CD;
		break;
	case 207 ... 219:
		backlightlevel = GAMMA_207CD;
		break;
	case 220 ... 233:
		backlightlevel = GAMMA_220CD;
		break;
	case 234 ... 254:
		backlightlevel = GAMMA_234CD;
		break;
	case 255:
		if(get_auto_brightness() > 4)
			backlightlevel = GAMMA_300CD;
		else
			backlightlevel = GAMMA_249CD;
		break;
	default:
		pr_info("%s lcd error bl_level : %d", __func__, bl_level);
		backlightlevel = GAMMA_162CD;
		break;
	}

	return backlightlevel;
}


#define DEFAULT_ELVSS 0x0B
#define ELVSS_MAX 18
static int elvss_array[ELVSS_MAX][2] = {
	{105, 0x14},
	{111, 0x14},
	{119, 0x13},
	{126, 0x13},
	{134, 0x13},
	{143, 0x12},
	{152, 0x12},
	{162, 0x11},
	{172, 0x11},
	{183, 0x13},
	{195, 0x12},
	{207, 0x12},
	{220, 0x11},
	{234, 0x10},
	{249, 0x0F},
	{265, 0x0D},
	{282, 0x0C},
	{300, 0x0B}
};

static int get_elvss_value(int candela)
{
	int elvss_value;
	int loop;

	elvss_value = DEFAULT_ELVSS;

	for (loop = 0; loop < ELVSS_MAX; loop++) {
		if (candela <= elvss_array[loop][0]) {
			elvss_value = elvss_array[loop][1];
			break;
		}
		}

	if (elvss_value >= 0x29) {
		pr_err("%s : candela=%d, elvss=0x%x : OVER RANGE -> 0x29\n", __func__, candela, elvss_value);
		elvss_value = 0x29;
	}
/*      pr_info( "%s : candela = %d, ELVSS = 0x%x\n", __func__, candela, elvss_value); */

	return elvss_value;
}

#ifdef USE_AOR

static void aor_copy_AMS367AV( int candela, char* dest )
{
	char aor_value[2];

	switch( candela )
	{
	case 0 ... 10 :	aor_value[0] = 0x02; aor_value[1] = 0xEA;	break; /* 91.4 */
	case 11 :	aor_value[0] = 0x02; aor_value[1] = 0xE3;	break; /* 90.6 */
	case 12 :	aor_value[0] = 0x02; aor_value[1] = 0xDC;	break; /* 89.7 */
	case 13 :	aor_value[0] = 0x02; aor_value[1] = 0xD5;	break; /* 88.8 */
	case 14 :	aor_value[0] = 0x02; aor_value[1] = 0xCE;	break; /* 88.0 */
	case 15 :	aor_value[0] = 0x02; aor_value[1] = 0xC7;	break; /* 87.1 */
	case 16 :	aor_value[0] = 0x02; aor_value[1] = 0xB9;	break; /* 85.4 */
	case 17 ... 18 :	aor_value[0] = 0x02; aor_value[1] = 0xB2;	break; /* 84.6 */
	case 19 :	aor_value[0] = 0x02; aor_value[1] = 0xAB;	break; /* 83.7 */
	case 20 :	aor_value[0] = 0x02; aor_value[1] = 0xA4;	break; /* 82.8 */
	case 21 :	aor_value[0] = 0x02; aor_value[1] = 0x96;	break; /* 81.1 */
	case 22 ... 23 :	aor_value[0] = 0x02; aor_value[1] = 0x8F;	break; /* 80.3 */
	case 24 :	aor_value[0] = 0x02; aor_value[1] = 0x81;	break; /* 78.6 */
	case 25 ... 26 :	aor_value[0] = 0x02; aor_value[1] = 0x7A;	break; /* 77.7 */
	case 27 ... 28 :	aor_value[0] = 0x02; aor_value[1] = 0x6C;	break; /* 76.0 */
	case 29 :	aor_value[0] = 0x02; aor_value[1] = 0x65;	break; /* 75.1 */
	case 30 ... 31 :	aor_value[0] = 0x02; aor_value[1] = 0x57;	break; /* 73.4 */
	case 32 ... 33 :	aor_value[0] = 0x02; aor_value[1] = 0x49;	break; /* 71.7 */
	case 34 ... 36 :	aor_value[0] = 0x02; aor_value[1] = 0x3B;	break; /* 70.0 */
	case 37 ... 38 :	aor_value[0] = 0x02; aor_value[1] = 0x26;	break; /* 67.4 */
	case 39 ... 40 :	aor_value[0] = 0x02; aor_value[1] = 0x18;	break; /* 65.7 */
	case 41 ... 43 :	aor_value[0] = 0x02; aor_value[1] = 0x03;	break; /* 63.1 */
	case 44 ... 46 :	aor_value[0] = 0x01; aor_value[1] = 0xEE;	break; /* 60.5 */
	case 47 ... 49 :	aor_value[0] = 0x01; aor_value[1] = 0xD9;	break; /* 58.0 */
	case 50 ... 52 :	aor_value[0] = 0x01; aor_value[1] = 0xC4;	break; /* 55.4 */
	case 53 ... 55 :	aor_value[0] = 0x01; aor_value[1] = 0xA8;	break; /* 52.0 */
	case 56 ... 59 :	aor_value[0] = 0x01; aor_value[1] = 0x93;	break; /* 49.4 */
	case 60 ... 63 :	aor_value[0] = 0x01; aor_value[1] = 0x77;	break; /* 46.0 */
	case 64 ... 67 :	aor_value[0] = 0x01; aor_value[1] = 0x54;	break; /* 41.7 */
	case 68 ... 71 :	aor_value[0] = 0x01; aor_value[1] = 0x38;	break; /* 38.2 */
	case 72 ... 76 :	aor_value[0] = 0x01; aor_value[1] = 0x15;	break; /* 33.9 */
	case 77 ... 81 :	aor_value[0] = 0x00; aor_value[1] = 0xF2;	break; /* 29.7 */
	case 82 ... 86 :	aor_value[0] = 0x00; aor_value[1] = 0xC8;	break; /* 24.5 */
	case 87 ... 92 :	aor_value[0] = 0x00; aor_value[1] = 0xA5;	break; /* 20.2 */
	case 93 ... 97 :	aor_value[0] = 0x00; aor_value[1] = 0x74;	break; /* 14.2 */
	case 98 ... 104 :	aor_value[0] = 0x00; aor_value[1] = 0x51;	break; /* 09.9 */
	case 105 ... 109 :	aor_value[0] = 0x00; aor_value[1] = 0x19;	break; /* 03.1 */
	case 110 ... 180 :	aor_value[0] = 0x01; aor_value[1] = 0x46;	break; /* 40.0 */
	case 183 ... 300 :	aor_value[0] = 0x00; aor_value[1] = 0x07;	break; /* 0.9 */
	default :
		pr_err( "%s(AOR) : OVER RANGE, brightness=%d\n", __func__, candela );
		aor_value[0] = 0x00; aor_value[1] = 0x07;	/* 0.9 */
		break;
	}

	dest[4] = aor_value[0];
	dest[5] = aor_value[1];

	return;
}
#endif /* USE_AOR */

static int brightness_control(int bl_level)
{
	int id3;
	int cmd_size;
	char elvss_value;
	int candela;
	int is_HBM;
	int i;
#ifdef HBM_OVERWRITING
	int use_hbm_temp;
#endif

	char* b5_reg = get_b5_reg();
	char* b6_reg = get_b6_reg();
	char b5_reg_19 = get_b5_reg_19();

	id3 = mipi_pd.manufacture_id & 0xFF;

	if (bl_level < 30)
		bl_level = 30;

	is_HBM = (get_auto_brightness() == HBM_MODE_ID);
#ifdef HBM_OVERWRITING
	use_hbm_temp = (is_HBM && b5_reg[0]==0 && b5_reg[1]==0);
#endif
	candela = lux_tbl[get_candela_index(bl_level)];

	if (!mipi_pd.rst_brightness && mipi_pd.brightness_level == candela)	return 0;

	pr_info("%s brightness_level : %d, candela : %d, auto : %d, rst : %d\n",
		__func__, mipi_pd.brightness_level, candela, get_auto_brightness(), mipi_pd.rst_brightness);

	cmd_size = 0;

	/* elvss ****************************************************************************/
	/* 0xB6 setting */

	/* Reset ELVSS Value : 0xB6 */
	brightness_packet[cmd_size].dtype = DTYPE_DCS_WRITE1;
	brightness_packet[cmd_size].payload =
				samsung_brightness_elvss1_default; /* ELVSS contition set */
	brightness_packet[cmd_size].dlen =
				sizeof(samsung_brightness_elvss1_default);
	brightness_packet[cmd_size].last = 0;
	cmd_size++;

	/* 0xB0 */
	if (is_HBM)
		samsung_brightness_elvss1_global_para[1] = 0x10; /*17th*/
	else
		samsung_brightness_elvss1_global_para[1] = 0x01;

	brightness_packet[cmd_size].dtype = DTYPE_DCS_WRITE1;
	brightness_packet[cmd_size].payload =
				samsung_brightness_elvss1_global_para; /* register B0 */
	brightness_packet[cmd_size].dlen =
				sizeof(samsung_brightness_elvss1_global_para);
	brightness_packet[cmd_size].last = 0;
	cmd_size++;

	/* 0xB6 */
	if (is_HBM)
		samsung_brightness_elvss3_ref[1] = b5_reg_19;
	else {
		/* in manual, elvss_value += 17th para(ELVSS_CAL_OFFSET) */
		/* it is not emplemented */
		elvss_value = get_elvss_value(candela);
		samsung_brightness_elvss3_ref[1] = elvss_value;
	}

#ifdef HBM_OVERWRITING
	if( use_hbm_temp ) samsung_brightness_elvss3_ref[1] = 0x12;
#endif

	brightness_packet[cmd_size].dtype = DTYPE_DCS_WRITE1;
	brightness_packet[cmd_size].payload =
				samsung_brightness_elvss3_ref; /* ELVSS contition set */
	brightness_packet[cmd_size].dlen =
				sizeof(samsung_brightness_elvss3_ref);
	brightness_packet[cmd_size].last = 0;
	cmd_size++;

#ifdef USE_AOR
	/* aor ****************************************************************************/
	/* 0xB2 setting */
	memcpy(samsung_brightness_aor_pre, samsung_brightness_aor_ref,
					sizeof(samsung_brightness_aor_ref));

	aor_copy_AMS367AV(candela, samsung_brightness_aor_ref);

	if ( mipi_pd.rst_brightness ||
			memcmp(samsung_brightness_aor_pre, samsung_brightness_aor_ref,
			sizeof(samsung_brightness_aor_ref))) {
		brightness_packet[cmd_size].dtype = DTYPE_DCS_LWRITE;
		brightness_packet[cmd_size].payload =
					samsung_brightness_aor_ref;
		brightness_packet[cmd_size].dlen =
					sizeof(samsung_brightness_aor_ref);
		brightness_packet[cmd_size].last = 0;
		cmd_size++;
	}
#endif /* USE_AOR */
	/* acl control *************************************************************************/
	/* 0xB5 setting */
	memcpy(samsung_brightness_acl_pre, samsung_brightness_acl_ref,
					sizeof(samsung_brightness_acl_ref));

	if (mipi_pd.acl_status || mipi_pd.siop_status || is_HBM)
		samsung_brightness_acl_ref[1] = 0x02; /* 40% */
	else
		samsung_brightness_acl_ref[1] = 0x00;

	if (memcmp(samsung_brightness_acl_pre, samsung_brightness_acl_ref,
				sizeof(samsung_brightness_acl_ref))) {
		brightness_packet[cmd_size].dtype = DTYPE_DCS_WRITE1;
		brightness_packet[cmd_size].payload =
				samsung_brightness_acl_ref;
		brightness_packet[cmd_size].dlen =
				sizeof(samsung_brightness_acl_ref);
		brightness_packet[cmd_size].last = 0;
		cmd_size++;
	}

	/* gamma ******************************************************************************/
	/* 0xCA setting */
	mipi_pd.smart_s6e88a[mipi_pd.lcd_no].brightness_level = candela;

	generate_gamma(&(mipi_pd.smart_s6e88a[mipi_pd.lcd_no]),
			&(samsung_brightness_gamma[1]), GAMMA_SET_MAX);

	/* LSI HBM */
	if (is_HBM) {
		for (i=1; i<=6 ; i++)
			samsung_brightness_gamma[i] = *(b5_reg+(i-1));
		for (i=7; i<=9 ; i++)
			samsung_brightness_gamma[i] = *(b5_reg+(i+6));
		for (i=10; i<=21 ; i++)
			samsung_brightness_gamma[i] = *(b6_reg+(i-10));
		for (i=22; i<=30 ; i++)
			samsung_brightness_gamma[i] = 0x80;
		samsung_brightness_gamma[31] = 0x02;
		samsung_brightness_gamma[32] = 0x03;
		samsung_brightness_gamma[33] = 0x02;

#ifdef HBM_OVERWRITING
		if( use_hbm_temp ) {
			i = 1;
			samsung_brightness_gamma[i++] = 0x01;
			samsung_brightness_gamma[i++] = 0x1C;
			samsung_brightness_gamma[i++] = 0x01;
			samsung_brightness_gamma[i++] = 0x1C;
			samsung_brightness_gamma[i++] = 0x01;
			samsung_brightness_gamma[i++] = 0x1C;
			for(; i<=30; i++ ) samsung_brightness_gamma[i] = 0x80;
			samsung_brightness_gamma[i++] = 0x02;
			samsung_brightness_gamma[i++] = 0x03;
			samsung_brightness_gamma[i++] = 0x02;

			if( i != 34 ) pr_err( "%s : ASSERT : HBM_OVERWRITING parameter", __func__ );
			else pr_info( "%s : HBM_OVERWRITING", __func__ );
		}
#endif /* HBM_OVERWRITING */

#ifdef CONFIG_HBM_PSRE_DEBUG
		/*for debug*/
		printk("%s [HBM] CA[1~33] : ",__func__);
		for (i=1; i<=33; i++)
			printk("(%x)",samsung_brightness_gamma[i]);
		printk("\n");
#endif
	}

	brightness_packet[cmd_size].dtype = DTYPE_DCS_LWRITE;
	brightness_packet[cmd_size].payload = samsung_brightness_gamma;
	brightness_packet[cmd_size].dlen = sizeof(samsung_brightness_gamma);
	brightness_packet[cmd_size].last = 0;
	cmd_size++;

	/* gamma update ***********************************************************************/
	/* 0xF7 setting */
	brightness_packet[cmd_size].dtype = DTYPE_DCS_WRITE1;
	brightness_packet[cmd_size].payload = samsung_gamma_update;
	brightness_packet[cmd_size].dlen = sizeof(samsung_gamma_update);
	brightness_packet[cmd_size].last = 1;
	cmd_size++;

	mipi_pd.brightness.size = cmd_size;
	mipi_pd.brightness_level = candela;

	mipi_pd.rst_brightness = false;
	return 1;
}

static int acl_control(int bl_level)
{
	if (mipi_pd.acl_status || mipi_pd.siop_status ||
					(get_auto_brightness() == HBM_MODE_ID))
		samsung_brightness_acl_ref[1] = 0x02;/*40p*/
	else
		samsung_brightness_acl_ref[1] = 0x00;

	return 1;
}

static int cmd_set_change(int cmd_set, int panel_id)
{
	switch (cmd_set) {
	case PANEL_ON:
		mipi_pd.on.cmd = samsung_on_cmds ;
		mipi_pd.on.size =
			ARRAY_SIZE(samsung_on_cmds);
		break;
	default:
		break;
	}

	return 0;
}

void reset_bl_level(void)
{
#ifdef USE_AOR
	/* copy current brightness level */
	if (memcmp(samsung_brightness_aor_ref, samsung_brightness_aor_default,
				sizeof(samsung_brightness_aor_ref))) {
		memcpy(samsung_brightness_aor_condition,
				samsung_brightness_aor_ref,
				sizeof(samsung_brightness_aor_condition));
	}
#endif /* USE_AOR */

	if (memcmp(samsung_brightness_elvss3_ref, samsung_brightness_elvss3_default,
				sizeof(samsung_brightness_elvss3_default))) {
		memcpy(samsung_elvss_condition3,
				samsung_brightness_elvss3_ref,
				sizeof(samsung_elvss_condition3));
	}

	/* reset brightness change value */
	memcpy(samsung_brightness_acl_ref, samsung_brightness_acl_default,
					sizeof(samsung_brightness_acl_ref));

	memcpy(samsung_brightness_elvss3_ref, samsung_brightness_elvss3_default,
					sizeof(samsung_brightness_elvss3_default));

#ifdef USE_AOR
	memcpy(samsung_brightness_aor_ref, samsung_brightness_aor_default,
					sizeof(samsung_brightness_aor_ref));
#endif /* USE_AOR */
}

static struct mipi_panel_data mipi_pd = {
	.panel_name	= "SDC_AMS367AV\n",
	.on		= {samsung_on_cmds
				, ARRAY_SIZE(samsung_on_cmds)},
	.off		= {panel_off_cmds
				, ARRAY_SIZE(panel_off_cmds)},
	.late_on		= {panel_late_on_cmds
				, ARRAY_SIZE(panel_late_on_cmds)},
	.early_off	= {panel_early_off_cmds
				, ARRAY_SIZE(panel_early_off_cmds)},

	.brightness	= {brightness_packet
				, ARRAY_SIZE(brightness_packet)},
	.backlight_control = brightness_control,

	.reset_bl_level = reset_bl_level,

	.mtp_enable	= {panel_mtp_enable_cmds
				, ARRAY_SIZE(panel_mtp_enable_cmds)},
	.mtp_disable	= {panel_mtp_disable_cmds
				, ARRAY_SIZE(panel_mtp_disable_cmds)},
	.need_flip	= {panel_need_flip_cmds
				, ARRAY_SIZE(panel_mtp_disable_cmds)},
	.temperature	= {panel_temperature
				,ARRAY_SIZE(panel_temperature)},
	.touch_key	= {panel_touchkey_on
				,ARRAY_SIZE(panel_touchkey_on)},

	.cmd_set_change = cmd_set_change,

	.lux_table = lux_tbl,
	.lux_table_max_cnt = ARRAY_SIZE(lux_tbl),

	.acl_control = acl_control,
	.acl_cmds = {panel_acl_cmds
				, ARRAY_SIZE(panel_acl_cmds)},
};

#if (MIPI_CLK==450000000)
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI_BIT_CLK at 450MHz, 2 lane, RGB888 */
	{0x03, 0x0a, 0x04, 0x00, 0x20}, /* regulator */
	/* timing   */
	{0x7f, 0x30, 0x13, 0x00, 0x41, 0x47, 0x17, 0x34,
	0x20, 0x03, 0x04, 0x00},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0xc1, 0x1, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01},
};
#elif (MIPI_CLK==365000000)	/* 130509 updated */
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI_BIT_CLK at 450MHz, 2 lane, RGB888 */
	{0x03, 0x0a, 0x04, 0x00, 0x20},	/* regulator */
	/* timing   */
	{0x6c, 0x2c, 0x0f, 0x00, 0x39, 0x42, 0x13, 0x30,
	0x19, 0x03, 0x04, 0x00},	/* 365Mhz */
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x6c, 0x01, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01},
};
#endif

#if defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
int is_S6E88A(void);
#endif

static int __init mipi_video_ams367av_oled_wvga_pt_init(void)
{
	int ret;

#if defined(CONFIG_FB_MSM_MIPI_AMS367_OLED_VIDEO_WVGA_PT_PANEL)
	if( !is_S6E88A() )
	{
		pr_info("%s: IGNORE\n", __func__);
		return -ENODEV;
	}
#endif

printk(KERN_DEBUG "[lcd] mipi_video_ams367av_oled_wvga_pt_init start\n");

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_ams367av_oled_wvga"))
		return 0;
#endif

	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.mode2_xres = 0;
	pinfo.mode2_yres = 0;
	pinfo.mode2_bpp = 0;

	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;

#if (MIPI_CLK==450000000)
	pinfo.lcdc.h_back_porch = 83;
	pinfo.lcdc.h_front_porch = 200;
	pinfo.lcdc.h_pulse_width = 2;
#elif (MIPI_CLK==365000000)	/* 130509 updated */
	pinfo.lcdc.h_back_porch = 50;
	pinfo.lcdc.h_front_porch = 90;
	pinfo.lcdc.h_pulse_width = 2;
#endif

	pinfo.lcdc.v_back_porch = 1;
	pinfo.lcdc.v_front_porch = 13;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;

	pinfo.clk_rate = MIPI_CLK;

	pinfo.mipi.pulse_mode_hsa_he = FALSE;
	pinfo.mipi.hfp_power_stop = TRUE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x01;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;

	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.force_clk_lane_hs = 1;
	pinfo.mipi.esc_byte_ratio = 2;
	pinfo.mipi.mode = DSI_VIDEO_MODE;

	ret = mipi_ams367av_device_register(&pinfo, MIPI_DSI_PRIM, MIPI_DSI_PANEL_WVGA_PT, &mipi_pd);

	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}
module_init(mipi_video_ams367av_oled_wvga_pt_init);

