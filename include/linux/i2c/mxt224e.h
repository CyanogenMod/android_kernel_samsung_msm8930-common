/*
 *  Copyright (C) 2010, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifndef __MXT224_H__
#define __MXT224_H__

#define MXT224_DEV_NAME "Atmel MXT224"

extern struct class *sec_class;
enum {
	RESERVED_T0 = 0,
	RESERVED_T1,
	DEBUG_DELTAS_T2,
	DEBUG_REFERENCES_T3,
	DEBUG_SIGNALS_T4,
	GEN_MESSAGEPROCESSOR_T5,
	GEN_COMMANDPROCESSOR_T6,
	GEN_POWERCONFIG_T7,
	GEN_ACQUISITIONCONFIG_T8,
	TOUCH_MULTITOUCHSCREEN_T9,
	TOUCH_SINGLETOUCHSCREEN_T10,
	TOUCH_XSLIDER_T11,
	TOUCH_YSLIDER_T12,
	TOUCH_XWHEEL_T13,
	TOUCH_YWHEEL_T14,
	TOUCH_KEYARRAY_T15,
	PROCG_SIGNALFILTER_T16,
	PROCI_LINEARIZATIONTABLE_T17,
	SPT_COMCONFIG_T18,
	SPT_GPIOPWM_T19,
	PROCI_GRIPFACESUPPRESSION_T20,
	RESERVED_T21,
	PROCG_NOISESUPPRESSION_T22,
	TOUCH_PROXIMITY_T23,
	PROCI_ONETOUCHGESTUREPROCESSOR_T24,
	SPT_SELFTEST_T25,
	DEBUG_CTERANGE_T26,
	PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
	SPT_CTECONFIG_T28,
	SPT_GPI_T29,
	SPT_GATE_T30,
	TOUCH_KEYSET_T31,
	TOUCH_XSLIDERSET_T32,
	RESERVED_T33,
	GEN_MESSAGEBLOCK_T34,
	SPT_GENERICDATA_T35,
	RESERVED_T36,
	DEBUG_DIAGNOSTIC_T37,
	SPARE_T38,
	SPARE_T39,
	PROCI_GRIPSUPPRESSION_T40,
	SPARE_T41,
	PROCI_TOUCHSUPPRESSION_T42,
	SPARE_T43,
	SPARE_T44,
	SPARE_T45,
	SPT_CTECONFIG_T46,
	/*SPARE_T46,*/
	PROCI_STYLUS_T47,
	PROCG_NOISESUPPRESSION_T48,
	/*SPARE_T48,*/
	SPARE_T49,
	SPARE_T50,
	RESERVED_T255 = 255,
};

struct mxt224_platform_data {
	int max_finger_touches;
	const u8 **config;
	const u8 **config_e;
	int gpio_read_done;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int min_z;
	int max_z;
	int min_w;
	int max_w;
	bool inverse_x;
	bool inverse_y;
	u8 atchcalst;
	u8 atchcalsthr;
	u8 tchthr_batt;
	u8 tchthr_charging;
	u8 tchthr_batt_init;
	u8 noisethr_batt;
	u8 noisethr_charging;
	u8 movfilter_batt;
	u8 movfilter_charging;
	u8 tchthr_batt_e;
	u8 tchthr_charging_e;
	u8 calcfg_batt_e;
	u8 calcfg_charging_e;
	u8 atchfrccalthr_e;
	u8 atchfrccalratio_e;
	const u8 *t48_config_batt_e;
	const u8 *t48_config_chrg_e;
	void (*power_onoff)(int);
	void (*register_cb)(void *);
	void (*read_ta_status)(void *);
	const u8 *config_fw_version;
};

#define	MXT_PAGE_UP      0x01
#define	MXT_PAGE_DOWN    0x02
#define	MXT_DELTA_MODE   0x10
#define	MXT_REFERENCE_MODE  0x11
#define	MXT_CTE_MODE        0x31

#if defined(CONFIG_MACH_COMANCHE)
#define THRESHOLD_E_TA		32
#define THRESHOLD_E_BAT		32
#define ACTIVE_DEPTH_TA		42
#define ACTIVE_DEPTH_BAT	27

#define MEDIAN_ERR_T48_ADDR_3_BAT	2
#define MEDIAN_ERR_T48_ADDR_3_TA	2
#define MEDIAN_ERR_T48_ADDR_8		0
#define MEDIAN_ERR_T48_ADDR_9		0
#define MEDIAN_ERR_T48_ADDR_35		40
#define MEDIAN_ERR_T48_ADDR_39_BAT	81
#define MEDIAN_ERR_T48_ADDR_39_TA	65
#define MEDIAN_ERR_T46_ADDR_3_BAT	42
#define MEDIAN_ERR_T46_ADDR_3_TA	63
#define MEDIAN_ERR_T48_ADDR_38_TA	3

#elif defined(CONFIG_MACH_APEXQ)
#define THRESHOLD_E_TA		40
#define THRESHOLD_E_BAT		33
#define ACTIVE_DEPTH_TA		35
#define ACTIVE_DEPTH_BAT	25

#define MEDIAN_ERR_T48_ADDR_3_BAT	20
#define MEDIAN_ERR_T48_ADDR_3_TA	10
#define MEDIAN_ERR_T48_ADDR_8		1
#define MEDIAN_ERR_T48_ADDR_9		2
#define MEDIAN_ERR_T48_ADDR_35		45
#define MEDIAN_ERR_T48_ADDR_39_BAT	65
#define MEDIAN_ERR_T48_ADDR_39_TA	65
#define MEDIAN_ERR_T46_ADDR_3_BAT	30
#define MEDIAN_ERR_T46_ADDR_3_TA	52
#define MEDIAN_ERR_T48_ADDR_38_TA	0

#elif defined(CONFIG_MACH_AEGIS2)
#define THRESHOLD_E_TA		40
#define THRESHOLD_E_BAT		33
#define ACTIVE_DEPTH_TA		35
#define ACTIVE_DEPTH_BAT	25

#define MEDIAN_ERR_T48_ADDR_3_BAT	20
#define MEDIAN_ERR_T48_ADDR_3_TA	10
#define MEDIAN_ERR_T48_ADDR_8		1
#define MEDIAN_ERR_T48_ADDR_9		2
#define MEDIAN_ERR_T48_ADDR_35		45
#define MEDIAN_ERR_T48_ADDR_39_BAT	65
#define MEDIAN_ERR_T48_ADDR_39_TA	65
#define MEDIAN_ERR_T46_ADDR_3_BAT	30
#define MEDIAN_ERR_T46_ADDR_3_TA	52
#define MEDIAN_ERR_T48_ADDR_38_TA	0

#elif defined(CONFIG_MACH_EXPRESS)
#define THRESHOLD_E_TA		32
#define THRESHOLD_E_BAT		32
#define ACTIVE_DEPTH_TA		38
#define ACTIVE_DEPTH_BAT	28

#define MEDIAN_ERR_T48_ADDR_3_BAT	2
#define MEDIAN_ERR_T48_ADDR_3_TA	10
#define MEDIAN_ERR_T48_ADDR_8		0
#define MEDIAN_ERR_T48_ADDR_9		0
#define MEDIAN_ERR_T48_ADDR_35		40
#define MEDIAN_ERR_T48_ADDR_39_BAT	47
#define MEDIAN_ERR_T48_ADDR_39_TA	47
#define MEDIAN_ERR_T46_ADDR_3_BAT	32
#define MEDIAN_ERR_T46_ADDR_3_TA	48
#define MEDIAN_ERR_T48_ADDR_38_TA	0

#else
#define THRESHOLD_E_TA		32
#define THRESHOLD_E_BAT		32
#define ACTIVE_DEPTH_TA		38
#define ACTIVE_DEPTH_BAT	28

#define MEDIAN_ERR_T48_ADDR_3_BAT	2
#define MEDIAN_ERR_T48_ADDR_3_TA	10
#define MEDIAN_ERR_T48_ADDR_8		0
#define MEDIAN_ERR_T48_ADDR_9		0
#define MEDIAN_ERR_T48_ADDR_35		40
#define MEDIAN_ERR_T48_ADDR_39_BAT	81
#define MEDIAN_ERR_T48_ADDR_39_TA	65
#define MEDIAN_ERR_T46_ADDR_3_BAT	32
#define MEDIAN_ERR_T46_ADDR_3_TA	63
#define MEDIAN_ERR_T48_ADDR_38_TA	0
#endif

#if defined(CONFIG_MACH_JAGUAR) || defined(CONFIG_MACH_APEXQ)
#define VIRTUAL_KEYPAD_ISSUE
#endif
#if defined(CONFIG_MIPI_SAMSUNG_ESD_REFRESH)
extern void set_esd_enable(void);
extern void set_esd_disable(void);
#endif
#endif
