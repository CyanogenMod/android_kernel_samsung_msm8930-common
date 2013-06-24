/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mipi_dsi.h"
#include <linux/wakelock.h>

#ifndef MIPI_AMS367AV_OLED_H
#define MIPI_AMS367AV_OLED_H

#define USE_ACL
#define USE_ELVSS
#include "smart_dimming.h"

#define SmartDimming_useSampleValue
#define SmartDimming_CANDELA_UPPER_LIMIT (300)
#define MTP_DATA_SIZE (24)
#define ELVSS_DATA_SIZE (24)
#define MTP_REGISTER	(0xD3)
#define ELVSS_REGISTER	 (0xD4)
#define SmartDimming_GammaUpdate_Pos (2)

#include "smart_mtp_s6e88a.h"

#define HBM_MODE_ID (6)

#define MTP_RETRY_MAX	(3)
#define DIMMING_BL (20)
#define DEFAULT_BL (160)

#define USE_READ_ID

#ifdef CONFIG_DUAL_LCD
#define LCD_LOTS	2
#else /* #ifdef CONFIG_DUAL_LCD */
#define LCD_LOTS	1
#endif /* #ifdef CONFIG_DUAL_LCD */

enum mipi_samsung_cmd_list {
	PANEL_READY_TO_ON,
	PANEL_READY_TO_OFF,
	PANEL_ON,
	PANEL_OFF,
	PANEL_LATE_ON,
	PANEL_EARLY_OFF,
	PANEL_BRIGHT_CTRL,
	PANEL_MTP_ENABLE,
	PANEL_MTP_DISABLE,
	PANEL_NEED_FLIP,
	PANEL_ACL_CONTROL,
	PANLE_TEMPERATURE,
	PANLE_TOUCH_KEY,
};

enum gamma_mode_list {
	GAMMA_2_2 = 0,
	GAMMA_1_9 = 1,
	GAMMA_SMART = 2,
};

enum {
	MIPI_RESUME_STATE,
	MIPI_SUSPEND_STATE,
};

struct cmd_set {
	struct dsi_cmd_desc *cmd;
	int size;
};

struct gamma_table {
	char *table;
	int table_cnt;
	int data_size;
};

struct mipi_panel_data {
	const char panel_name[20];
	struct cmd_set on;
	struct cmd_set off;
	struct cmd_set late_on;
	struct cmd_set early_off;
	struct cmd_set mtp_enable;
	struct cmd_set mtp_disable;
	struct cmd_set need_flip;
	struct cmd_set brightness;
	struct cmd_set acl_cmds;
	struct cmd_set temperature;
	struct cmd_set touch_key;
	unsigned int manufacture_id;

	struct mipi_samsung_driver_data *msd;

	int (*backlight_control)(int bl_level);
	int (*acl_control)(int bl_level);
	int (*cmd_set_change)(int cmd_set, int panel_id);

	int *lux_table;
	int lux_table_max_cnt;
	struct SMART_DIM smart_s6e88a[LCD_LOTS];

	void(*reset_bl_level)(void);
	int brightness_level;
	int rst_brightness;

	int acl_status;

	int siop_pre_acl_status;
	int siop_status;

	int first_bl_hbm_psre;
	char ldi_rev;

	unsigned char lcd_mtp_data[LCD_LOTS][MTP_DATA_SIZE + 16];
	unsigned char lcd_elvss_data[LCD_LOTS][ELVSS_DATA_SIZE + 16];
	unsigned char *gamma_smartdim[LCD_LOTS];

	int lcd_no; /* access lcd NO : 0 ~ (LCD_LOTS-1) */
};
struct display_status {
	unsigned char acl_on;
	unsigned char gamma_mode;	/* 1: 1.9 gamma, 0: 2.2 gamma */
	unsigned char is_smart_dim_loaded[LCD_LOTS];
	unsigned char is_elvss_loaded[LCD_LOTS];
	unsigned char auto_brightness;
};
struct mipi_samsung_driver_data {
	struct dsi_buf samsung_tx_buf;
	struct dsi_buf samsung_rx_buf;
	struct msm_panel_common_pdata *mipi_samsung_disp_pdata;
	struct mipi_panel_data *mpd;
	struct display_status dstat;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_LCD_CLASS_DEVICE)
	struct platform_device *msm_pdev;
#endif
};

struct dsi_cmd_desc_LCD {
	int lux;
	char strID[8];
	struct dsi_cmd_desc *cmd;
};
int mipi_ams367av_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd);

extern struct mutex dsi_tx_mutex;

unsigned char get_auto_brightness(void);
char* get_b5_reg(void);
char get_b5_reg_19(void);
char* get_b6_reg(void);

#endif /* MIPI_SAMSUNG_OLED_H */
