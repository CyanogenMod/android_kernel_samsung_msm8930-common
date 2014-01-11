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
 *
 */

#include "mipi_dsi.h"
#include <linux/wakelock.h>

#ifndef MIPI_NT35510_H
#define MIPI_NT35510_H

#define MAX_BL_LEVEL 225
#define MAX_GAMMA_VALUE 25
#define MIN_BL_LEVEL 30
#define INDEX_OFFSET 1

#define DIMMING_BL (20)
#define DISP_BL_CONT_GPIO 7

/*#define USE_READ_ID*/
enum mipi_samsung_cmd_list {
	PANEL_READY_TO_ON,
	PANEL_READY_TO_OFF,
	PANEL_ON,
	PANEL_OFF,
	PANEL_LATE_ON,
	PANEL_EARLY_OFF,
	PANEL_GAMMA_UPDATE,
	MTP_READ_ENABLE,
	PANEL_BRIGHT_CTRL,
	PANEL_TUNE_CTRL,
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
	struct cmd_set ready_to_on;
	struct cmd_set ready_to_on_t05;
	struct cmd_set ready_to_off;
	struct cmd_set on;
	struct cmd_set off;
	struct cmd_set late_on;
	struct cmd_set early_off;
	struct cmd_set mtp_read_enable;
	struct cmd_set tune;

	/*struct str_smart_dim smart;*/
	signed char lcd_current_cd_idx;
	unsigned int manufacture_id;

	int (*set_brightness_level)(int bl_level);
	struct mipi_samsung_driver_data *msd;
	struct workqueue_struct *esd_workqueue;
	struct delayed_work esd_work;
	struct wake_lock esd_wake_lock;
};
struct display_status {
	unsigned char acl_on;
	unsigned char gamma_mode; /* 1: 1.9 gamma, 0: 2.2 gamma */
	unsigned char is_smart_dim_loaded;
	unsigned char is_elvss_loaded;
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
int mipi_samsung_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd);

void reset_gamma_level(void);
unsigned char bypass_LCD_Id(void);

#if defined(CONFIG_BACKLIGHT_IC_KTD253)
void ktd253_set_brightness(int level);
#define MAX_BRIGHTNESS_IN_BLU	32
#endif

extern struct mutex dsi_tx_mutex;

#endif  /* MIPI_NT35510_H */
