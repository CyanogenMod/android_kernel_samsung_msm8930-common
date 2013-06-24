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
#include "mipi_magna_oled.h"

static struct msm_panel_info pinfo;
#define MAX_GAMMA_VALUE 25
static struct mipi_panel_data mipi_pd;
static int g_gamma_lux = 180;

static char all_pixel_off[] = { 0x23, /* no param */ };
static char normal_mode_on[] = { 0x13, /* no parm */ };
static char sleep_in[2] = { 0x10, /* no param */ };
static char sleep_out[2] = { 0x11, /* no param */ };
static char display_on[2] = { 0x29, /* no param */ };
static char display_off[2] = { 0x28, /* no param */ };

static char acl_on[] = {
	0xC0,
	0x01
};

static char acl_off[] = {
	0xC0,
	0x00
};

static char elvss_cond_set[] = {
	0xB2,
	0x06, 0x06, 0x06, 0x06
};

static char elvss_on[] = {
	0xB1,
	0x0B, 0x16
};

enum {
	GAMMA_30CD,
	GAMMA_40CD,
	GAMMA_50CD,
	GAMMA_60CD,
	GAMMA_70CD,
	GAMMA_80CD,
	GAMMA_90CD,
	GAMMA_100CD,
	GAMMA_110CD,
	GAMMA_120CD,
	GAMMA_130CD,
	GAMMA_140CD,
	GAMMA_150CD,
	GAMMA_160CD,
	GAMMA_170CD,
	GAMMA_180CD,
	GAMMA_190CD,
	GAMMA_200CD,
	GAMMA_210CD,
	GAMMA_220CD,
	GAMMA_230CD,
	GAMMA_240CD,
	GAMMA_250CD,
	GAMMA_300CD,
};

static int lux_tbl_acl[] = {
	  30, 40, 50, 60, 70, 80,
	  90, 100, 110, 120, 130, 140,
	  150, 160, 170, 180, 190, 200,
	  210, 220, 230, 240, 250, 300
};


static char GAMMA_SmartDimming_COND_SET[] = {
	0xF9,
	0x00, 0xa6, 0xbf, 0xab, 0xd0,
	0xc1, 0xc1, 0x55, 0x00, 0xc1,
	0xbe, 0xa9, 0xce, 0xbe, 0xc7,
	0x55, 0x00, 0xc7, 0xbe, 0xa8,
	0xcf, 0xbf, 0xd3, 0x55
};

static char level2_comma_set[] = {
	0xF0,
	0x5A, 0x5A,
};

static char nvm_refresh_off[] = {
	0xB3, 0x00,
};

static char gloal_parameter_access[] = {
	0xB0, 0x06,
};

static char eot_check_disable[] = {
	0xE0, 0x41,
};

static char display_control_set[] = {
	0xF2,
	0x02, 0x06, 0xA, 0x20, 0x50,
};
static char gtcon_control[] = {
	0xf7,
	0x09,
};
static char ltps_timing_set[] = {
	0xf8,
	0x05, 0x71, 0xac, 0x80, 0x8f,
	0x0f, 0x48, 0x00, 0x00, 0x3a,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x07, 0x06, 0x24, 0x24,
	0x24, 0x00, 0x00
};

static char level3_comma_set[] = {
	0xFC,
	0x5A, 0x5A,
};

static char key_command_set[] = {
	0xF1,
	0x5A, 0x5A
};

static char power_ctrl_set[] = {
	0xF4,
	0xAB, 0x6a, 0x00, 0x55, 0x03,
};

static char gamma_flag_set[] = {
	0xFB,
	0x00, 0x5A
};
/* GAMMA SET FROM SMD */
static char gamma_set_cmd1[] = {
	0xF9,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char gamma_flag_set_2[] = {
	0xFB,
	0x00, 0xa5,
};

static struct dsi_cmd_desc samsung_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(level2_comma_set), level2_comma_set},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 10,
		sizeof(sleep_out), sleep_out},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(key_command_set), key_command_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(level2_comma_set), level2_comma_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(level3_comma_set), level3_comma_set},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 10,
		sizeof(display_control_set), display_control_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gtcon_control), gtcon_control},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(ltps_timing_set), ltps_timing_set},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_flag_set), gamma_flag_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_set_cmd1), gamma_set_cmd1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gamma_flag_set_2), gamma_flag_set_2},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(elvss_on), elvss_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		 sizeof(elvss_cond_set), elvss_cond_set},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(power_ctrl_set), power_ctrl_set},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(nvm_refresh_off), nvm_refresh_off},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(gloal_parameter_access), gloal_parameter_access},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(eot_check_disable), eot_check_disable},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};
static struct dsi_cmd_desc samsung_panel_ready_to_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 5,
		sizeof(display_off), display_off},
};

static struct dsi_cmd_desc samsung_panel_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc samsung_panel_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10,
		sizeof(sleep_in), sleep_in},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(all_pixel_off), all_pixel_off},
};

static struct dsi_cmd_desc samsung_panel_late_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(normal_mode_on), normal_mode_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 5,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc samsung_panel_early_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10,
		sizeof(sleep_in), sleep_in},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(all_pixel_off), all_pixel_off},
};
static struct dsi_cmd_desc samsung_mtp_read_cmds[] = {

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(key_command_set), key_command_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(level2_comma_set), level2_comma_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(level3_comma_set), level3_comma_set},
};
/********************* ACL *******************/
static char ACL_COND_SET_40[] = {
	0xC1,
	0x4D, 0x96, 0x1D, 0x00, 0x00,
	0x01, 0xDF, 0x00, 0x00, 0x03,
	0x1F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x06, 0x11, 0x1A,
	0x20, 0x25, 0x29, 0x2D, 0x30,
	0x33, 0x35
};

static struct dsi_cmd_desc DSI_CMD_ACL_40 = {
	DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(ACL_COND_SET_40), ACL_COND_SET_40 };

static struct dsi_cmd_desc_LCD lcd_acl_table[] = {
	{0, "30", NULL},
	{0, "40", NULL},
	{0, "50", &DSI_CMD_ACL_40},
	{0, "60", &DSI_CMD_ACL_40},
	{40, "70", &DSI_CMD_ACL_40},
	{40, "80", &DSI_CMD_ACL_40},
	{40, "90", &DSI_CMD_ACL_40},
	{40, "100", &DSI_CMD_ACL_40},
	{40, "110", &DSI_CMD_ACL_40},
	{40, "120", &DSI_CMD_ACL_40},
	{40, "130", &DSI_CMD_ACL_40},
	{40, "140", &DSI_CMD_ACL_40},
	{40, "150", &DSI_CMD_ACL_40},
	{40, "160", &DSI_CMD_ACL_40},
	{40, "170", &DSI_CMD_ACL_40},
	{40, "180", &DSI_CMD_ACL_40},
	{40, "190", &DSI_CMD_ACL_40},
	{40, "200", &DSI_CMD_ACL_40},
	{40, "210", &DSI_CMD_ACL_40},
	{40, "220", &DSI_CMD_ACL_40},
	{40, "230", &DSI_CMD_ACL_40},
	{40, "240", &DSI_CMD_ACL_40},
	{40, "250", &DSI_CMD_ACL_40},
	{40, "300", &DSI_CMD_ACL_40},
};

static struct dsi_cmd_desc samsung_panel_acl_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_on), acl_on},
};

static struct dsi_cmd_desc samsung_panel_acl_off_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(acl_off), acl_off},
};
static struct dsi_cmd_desc samsung_panel_acl_update_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	 sizeof(ACL_COND_SET_40), ACL_COND_SET_40},
};

static struct dsi_cmd_desc samsung_panel_gamma_update_cmds[3] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(gamma_flag_set), gamma_flag_set},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(gamma_set_cmd1), gamma_set_cmd1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	sizeof(gamma_flag_set_2), gamma_flag_set_2},

};

static struct dsi_cmd_desc samsung_panel_elvss_update_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(elvss_on), elvss_on},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
	 sizeof(elvss_cond_set), elvss_cond_set},
};

/********************* ELVSS *******************/
#define LCD_ELVSS_DELTA_300CD (0x00)
#define LCD_ELVSS_DELTA_200CD (0x06)
#define LCD_ELVSS_DELTA_160CD (0x0A)
#define LCD_ELVSS_DELTA_100CD (0x0F)

#define LCD_ELVSS_RESULT_LIMIT (0x1F)

static int GET_ELVSS_ID[] = {
	LCD_ELVSS_DELTA_100CD,/* 0 = 30_dimming,*/
	LCD_ELVSS_DELTA_100CD,/* 1 = 40*/
	LCD_ELVSS_DELTA_100CD,/* 2 = 50*/
	LCD_ELVSS_DELTA_100CD,/* 3 = 60,*/
	LCD_ELVSS_DELTA_100CD,/* 4 = 70,*/
	LCD_ELVSS_DELTA_100CD,/* 5 = 80,*/
	LCD_ELVSS_DELTA_100CD,/* 6 = 90,*/
	LCD_ELVSS_DELTA_100CD,/* 7 = 100,*/
	LCD_ELVSS_DELTA_160CD,/* 8 = 110,*/
	LCD_ELVSS_DELTA_160CD,/* 9 = 120,*/
	LCD_ELVSS_DELTA_160CD,/* 10 = 130,*/
	LCD_ELVSS_DELTA_160CD,/* 11 = 140,*/
	LCD_ELVSS_DELTA_160CD,/* 12 = 150,*/
	LCD_ELVSS_DELTA_160CD,/* 13 = 160,*/
	LCD_ELVSS_DELTA_200CD,/* 14 = 170,*/
	LCD_ELVSS_DELTA_200CD,/* 15 = 180,*/
	LCD_ELVSS_DELTA_200CD,/* 16 = 190,*/
	LCD_ELVSS_DELTA_200CD,/* 17 = 200,*/
	LCD_ELVSS_DELTA_300CD,/* 18 = 210,*/
	LCD_ELVSS_DELTA_300CD,/* 19 = 220,*/
	LCD_ELVSS_DELTA_300CD,/* 20 = 230,*/
	LCD_ELVSS_DELTA_300CD,/* 21 = 240,*/
	LCD_ELVSS_DELTA_300CD,/* 22 = 250,*/
	LCD_ELVSS_DELTA_300CD/* 23 = 300,*/
};

#define LCD_ELVSS_DEFAULT_100CD (0x15)
#define LCD_ELVSS_DEFAULT_160CD (0x0F)
#define LCD_ELVSS_DEFAULT_200CD (0x0B)
#define LCD_ELVSS_DEFAULT_300CD (0x05)
static int GET_DEFAULT_ELVSS_ID[] = {
	LCD_ELVSS_DEFAULT_100CD,/* 0 = 30_dimming,*/
	LCD_ELVSS_DEFAULT_100CD,/* 1 = 40*/
	LCD_ELVSS_DEFAULT_100CD,/* 2 = 50*/
	LCD_ELVSS_DEFAULT_100CD,/* 3 = 60,*/
	LCD_ELVSS_DEFAULT_100CD,/* 4 = 70,*/
	LCD_ELVSS_DEFAULT_100CD,/* 5 = 80,*/
	LCD_ELVSS_DEFAULT_100CD,/* 6 = 90,*/
	LCD_ELVSS_DEFAULT_100CD,/* 7 = 100,*/
	LCD_ELVSS_DEFAULT_160CD,/* 8 = 110,*/
	LCD_ELVSS_DEFAULT_160CD,/* 9 = 120,*/
	LCD_ELVSS_DEFAULT_160CD,/* 10 = 130,*/
	LCD_ELVSS_DEFAULT_160CD,/* 11 = 140,*/
	LCD_ELVSS_DEFAULT_160CD,/* 12 = 150,*/
	LCD_ELVSS_DEFAULT_160CD,/* 13 = 160,*/
	LCD_ELVSS_DEFAULT_200CD,/* 14 = 170,*/
	LCD_ELVSS_DEFAULT_200CD,/* 15 = 180,*/
	LCD_ELVSS_DEFAULT_200CD,/* 16 = 190,*/
	LCD_ELVSS_DEFAULT_200CD,/* 17 = 200,*/
	LCD_ELVSS_DEFAULT_300CD,/* 18 = 210,*/
	LCD_ELVSS_DEFAULT_300CD,/* 19 = 220,*/
	LCD_ELVSS_DEFAULT_300CD,/* 20 = 230,*/
	LCD_ELVSS_DEFAULT_300CD,/* 21 = 240,*/
	LCD_ELVSS_DEFAULT_300CD,/* 22 = 250,*/
	LCD_ELVSS_DEFAULT_300CD/* 23 = 300,*/
};

static int get_candela_index(int bl_level)
{
	int backlightlevel;

	/* brightness setting from platform is from 0 to 255
	 * But in this driver, brightness is only supported from 0 to 24 */

	switch (bl_level) {
	case 0 ... 39:
		backlightlevel = GAMMA_30CD; /* 0*/
		break;
	case 40 ... 49:
		backlightlevel = GAMMA_40CD; /* 1 */
		break;
	case 50 ... 59:
		if (is_acl_on)
			backlightlevel = GAMMA_40CD; /* 4 */
		else
			backlightlevel = GAMMA_50CD; /* 2 */
		break;
	case 60 ... 69:
		if (is_acl_on)
			backlightlevel = GAMMA_40CD; /* 4 */
		else
			backlightlevel = GAMMA_60CD; /* 3 */
		break;
	case 70 ... 79:
		backlightlevel = GAMMA_70CD; /* 4 */
		break;
	case 80 ... 89:
		backlightlevel = GAMMA_80CD; /* 5 */
		break;
	case 90 ... 99:
		backlightlevel = GAMMA_90CD; /* 6 */
		break;
	case 100 ... 109:
		backlightlevel = GAMMA_100CD; /* 7 */
		break;
	case 110 ... 119:
		backlightlevel = GAMMA_110CD; /* 8 */
		break;
	case 120 ... 129:
		backlightlevel = GAMMA_120CD; /* 9 */
		break;
	case 130 ... 139:
		backlightlevel = GAMMA_130CD; /* 10 */
		break;
	case 140 ... 149:
		backlightlevel = GAMMA_140CD; /* 11 */
		break;
	case 150 ... 159:
		backlightlevel = GAMMA_150CD; /* 12 */
		break;
	case 160 ... 169:
		backlightlevel = GAMMA_160CD; /* 13 */
		break;
	case 170 ... 179:
		backlightlevel = GAMMA_170CD; /* 14 */
		break;
	case 180 ... 189:
		backlightlevel = GAMMA_180CD; /* 15 */
		break;
	case 190 ... 199:
		backlightlevel = GAMMA_190CD; /* 16 */
		break;
	case 200 ... 209:
		backlightlevel = GAMMA_200CD; /* 17 */
		break;
	case 210 ... 219:
		backlightlevel = GAMMA_210CD; /* 18 */
		break;
	case 220 ... 229:
		backlightlevel = GAMMA_220CD; /* 10 */
		break;
	case 230 ... 239:
		backlightlevel = GAMMA_230CD; /* 20 */
		break;
	case 240 ... 249:
		backlightlevel = GAMMA_240CD; /* 21 */
		break;
	case 250 ... 254:
		backlightlevel = GAMMA_250CD; /* 22 */
		break;
	case 255:
		if (mipi_pd.msd->dstat.auto_brightness == 0)
			backlightlevel = GAMMA_250CD; /* 22 */
		else
			backlightlevel = GAMMA_300CD; /* 23 */

		break;
	default:
		backlightlevel = GAMMA_40CD; /* 1 */
		break;
	}
	return backlightlevel;
}



static int set_acl_on_level(int bl_level)
{
	int cd;
	cd = get_candela_index(bl_level);
	if (!lcd_acl_table[cd].lux)
		return 1;

	if (lcd_acl_table[cd].lux) {
		samsung_panel_acl_update_cmds[0].dlen =
		    lcd_acl_table[cd].cmd->dlen;
		samsung_panel_acl_update_cmds[0].payload =
		    lcd_acl_table[cd].cmd->payload;
		pr_info("%s:%d\n", __func__, lcd_acl_table[cd].lux);
	}
	return 0;
}

static int set_elvss_level(int bl_level)
{
	unsigned char calc_elvss;
	int cd;
	int id3 = mipi_pd.manufacture_id & 0xFF;
	int id2 = (mipi_pd.manufacture_id>>8) & 0xFF;

	cd = get_candela_index(bl_level);

	if (id2 == 0x4A)
		calc_elvss = id3 + GET_ELVSS_ID[cd];
	else
		calc_elvss = GET_DEFAULT_ELVSS_ID[cd];

	pr_debug("%s: ID2=%x, ID3=%x, calc_elvss = %x\n", __func__, id2, id3,
		calc_elvss);

	if (calc_elvss > LCD_ELVSS_RESULT_LIMIT)
		calc_elvss = LCD_ELVSS_RESULT_LIMIT;

	elvss_cond_set[1] = calc_elvss;
	elvss_cond_set[2] = calc_elvss;
	elvss_cond_set[3] = calc_elvss;
	elvss_cond_set[4] = calc_elvss;

	return 0;
}


void reset_gamma_level(void)
{
	pr_info("reset_gamma_level\n");
	mipi_pd.lcd_current_cd_idx = -1;
}

int get_gamma_lux(void)
{
	return g_gamma_lux;
}

static int set_gamma_level(int bl_level, enum gamma_mode_list gamma_mode)
{
	int cd;
	int *lux_tbl = lux_tbl_acl;

	cd = get_candela_index(bl_level);
	if (mipi_pd.lcd_current_cd_idx == cd)
		return -EPERM;
	else
	    mipi_pd.lcd_current_cd_idx = cd;

	if (gamma_mode == GAMMA_SMART) {

		/*  SMART Dimming gamma_lux;  */
		char pBuffer[256];
		int i;
		int gamma_lux;

		gamma_lux = lux_tbl[cd];

		if (gamma_lux > SmartDimming_CANDELA_UPPER_LIMIT)
			gamma_lux = SmartDimming_CANDELA_UPPER_LIMIT;

		mipi_pd.smart_ea8868.brightness_level = gamma_lux;

		if (gamma_lux != 30)
			g_gamma_lux = gamma_lux;

		pr_info("lcd :current lux(%d) g_gamma_lux(%d)\n",
					gamma_lux, g_gamma_lux);

		for (i = SmartDimming_GammaUpdate_Pos;
		     i < sizeof(GAMMA_SmartDimming_COND_SET); i++)
			GAMMA_SmartDimming_COND_SET[i] = 0;

		generate_gamma(&(mipi_pd.smart_ea8868),
						GAMMA_SmartDimming_COND_SET +
						SmartDimming_GammaUpdate_Pos,
								GEN_GAMMA_MAX);

		samsung_panel_gamma_update_cmds[1].dlen =
		    sizeof(GAMMA_SmartDimming_COND_SET);
		samsung_panel_gamma_update_cmds[1].payload =
		    GAMMA_SmartDimming_COND_SET;
		pBuffer[0] = 0;
		for (i = 0; i < sizeof(GAMMA_SmartDimming_COND_SET); i++) {
			snprintf(pBuffer + strnlen(pBuffer, 256), 256, " %02x",
				 GAMMA_SmartDimming_COND_SET[i]);
		}
		pr_debug("SD: %03d %s\n", gamma_lux, pBuffer);
		pr_info("bl_level:%d,cd:%d:Candela:%d\n", bl_level, cd,
			gamma_lux);
		}

	return 0;
}

static struct mipi_panel_data mipi_pd = {
	.panel_name = "SMD_AMS452GP32\n",
	.ready_to_on = {samsung_display_on_cmds
				, ARRAY_SIZE(samsung_display_on_cmds)},
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
	.acl_on = {samsung_panel_acl_on_cmds
			, ARRAY_SIZE(samsung_panel_acl_on_cmds)},
	.acl_off = {samsung_panel_acl_off_cmds
			, ARRAY_SIZE(samsung_panel_acl_off_cmds)},
	.acl_update = {samsung_panel_acl_update_cmds,
	     ARRAY_SIZE(samsung_panel_acl_update_cmds)},
	.set_acl = set_acl_on_level,
	.elvss_update = {samsung_panel_elvss_update_cmds,
	     ARRAY_SIZE(samsung_panel_elvss_update_cmds)},
	.set_elvss = set_elvss_level,
	.gamma_update = {samsung_panel_gamma_update_cmds,
			 ARRAY_SIZE(samsung_panel_gamma_update_cmds)},
	.set_gamma = set_gamma_level,
	.lcd_current_cd_idx = -1,
	.mtp_read_enable = {samsung_mtp_read_cmds
				, ARRAY_SIZE(samsung_mtp_read_cmds)},
	.gamma_initial = gamma_set_cmd1,
};

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x5B, 0x28, 0x0C, 0x00, 0x33, 0x3A, 0x10, 0x2C, 0x14, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xee, 0x02, 0x86, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static int __init mipi_video_magna_oled_wvga_pt_init(void)
{
	int ret;
#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
	if (msm_fb_detect_client("mipi_video_magna_oled_wvga"))
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
	pinfo.lcdc.h_pulse_width = 4;
	pinfo.lcdc.h_back_porch = 16;
	pinfo.lcdc.h_front_porch = 80;
	pinfo.lcdc.v_pulse_width = 2;
	pinfo.lcdc.v_back_porch = 4;
	pinfo.lcdc.v_front_porch = 10;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 343500000;
	pinfo.mipi.mode = DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = FALSE;
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

	pinfo.mipi.tx_eot_append = FALSE;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2D;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.force_clk_lane_hs = 1;
	pinfo.mipi.esc_byte_ratio = 3;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	ret = mipi_samsung_device_register(&pinfo, MIPI_DSI_PRIM,
				MIPI_DSI_PANEL_WVGA_PT,
				&mipi_pd);
	if (ret)
		pr_info("%s: failed to register device!\n", __func__);

		pr_info("%s:\n", __func__);
	return ret;
}
module_init(mipi_video_magna_oled_wvga_pt_init);
