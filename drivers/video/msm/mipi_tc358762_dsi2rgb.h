/* Copyright (c) 2011, The Linux Foundation. All rights reserved.
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

#ifndef MIPI_TC358762_DSI2RGB_H
#define MIPI_TC358762_DSI2RGB_H

#include "mipi_dsi.h"
#include "mdp4.h"
#include <linux/wakelock.h>

#define DISP_BL_EN_GPIO 51

enum mipi_tc358762_cmd_list {
	PANEL_READY_TO_ON,
	PANEL_READY_TO_OFF,
	PANEL_ON,
	PANEL_OFF,
	PANEL_LATE_ON,
	PANEL_EARLY_OFF,
	PANEL_BRIGHT_CTRL,
};

struct cmd_set {
	struct dsi_cmd_desc *cmd;
	int size;
};

enum {
	MIPI_RESUME_STATE,
	MIPI_SUSPEND_STATE,
};

struct display_status {
	unsigned char auto_brightness;
};

struct mipi_panel_data {
	const char panel_name[20];
	struct cmd_set ready_to_on;
	struct cmd_set ready_to_off;
	struct cmd_set on;
	struct cmd_set off;
	struct cmd_set late_on;
	struct cmd_set early_off;
	struct mipi_tc358762_driver_data *msd;
};

struct mipi_tc358762_driver_data {
	struct dsi_buf tc358762_tx_buf;
	struct dsi_buf tc358762_rx_buf;
	struct msm_panel_common_pdata *mipi_tc358762_disp_pdata;
	struct mipi_panel_data *mpd;
	struct display_status dstat;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
#if defined(CONFIG_LCD_CLASS_DEVICE)
	struct platform_device *msm_pdev;
#endif
};

int mipi_tc358762_disp_device_register(struct msm_panel_info *pinfo,
	u32 channel_id, u32 panel_id, struct mipi_panel_data *mpd);
#endif  /* MIPI_TC358762_DSI2RGB_H */
