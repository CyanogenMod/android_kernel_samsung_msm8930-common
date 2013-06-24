/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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

#ifndef MIPI_V5D3BX_DSI2LVDS_H
#define MIPI_V5D3BX_DSI2LVDS_H


#include "mipi_dsi.h"
#include "mdp4.h"

#define DISP_BL_CONT_GPIO 10
#define V5D3BX_VEESTRENGHT		0x00001f07
#define V5D3BX_VEEDEFAULTVAL		0
#define V5D3BX_DEFAULT_STRENGHT		5
#define V5D3BX_DEFAULT_LOW_STRENGHT	8
#define V5D3BX_DEFAULT_HIGH_STRENGHT	10
#define V5D3BX_MAX_STRENGHT		15

#define V5D3BX_CABCBRIGHTNESSRATIO	815
#define V5D3BX_10KHZ_DEFAULT_RATIO	4707
#define AUTOBRIGHTNESS_LIMIT_VALUE	207

#define MIN_BRIGHTNESS			0
#define MAX_BRIGHTNESS_LEVEL		255
#define MID_BRIGHTNESS_LEVEL		160
#define LOW_BRIGHTNESS_LEVEL		20
#define DIM_BRIGHTNESS_LEVEL		19
#define DEFAULT_BRIGHTNESS		MID_BRIGHTNESS_LEVEL


struct mipi_dsi2lvds_driver_data {
	struct dsi_buf dsi2lvds_tx_buf;
	struct dsi_buf dsi2lvds_rx_buf;
	struct msm_panel_common_pdata *mipi_dsi2lvds_disp_pdata;
	struct mipi_panel_data *mpd;

#if defined(CONFIG_LCD_CLASS_DEVICE)
		struct platform_device *msm_pdev;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif

};


struct mipi_panel_data {
	const char panel_name[20];
	unsigned int manufacture_id;
	struct mipi_dsi2lvds_driver_data *msd;
};


enum {
	MIPI_RESUME_STATE,
	MIPI_SUSPEND_STATE,
};
int mipi2lvds_vx5b3d_disp_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel,
					struct mipi_panel_data *mpd);

#endif  /* MIPI_V5D3BX_DSI2LVDS_H */
