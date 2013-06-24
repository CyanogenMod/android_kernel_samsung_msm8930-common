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

#ifndef __MXT224S_H__
#define __MXT224S_H__

#include <linux/earlysuspend.h>

#define MXT224S_MAX_MT_FINGERS		0x0A
#define MXT224S_DEV_NAME "Atmel MXT224S"
#define SPT_USERDATA_T38	38
#define SPT_DIGITIZER_T43	43
#define SPARE_T51			51
#define TOUCH_PROXIMITY_KEY_T52	52
#define GEN_DATASOURCE_T53		53
#define SPARE_T54				54
#define PROCI_ADAPTIVETHRESHOLD_T55	55
#define PROCI_SHIELDLESS_T56	56
#define PROCI_EXTRATOUCHSCREENDATA_T57	57
#define SPARE_T58	58
#define SPARE_T59	59
#define SPARE_T60	60
#define SPT_TIMER_T61	61
#define PROCG_NOISESUPPRESSION_T62	62
#define MAX_USING_FINGER_NUM	10
#define MXT_SW_RESET_TIME		300

/* Tuning Value*/
#define T_AREA_LOW_ST		2
#define T_AREA_HIGH_ST		3
#define T_AREA_LOW_MT		4
#define T_AREA_HIGH_MT		5
/* ST: Single Touch, MT : Multi Touch*/

/* Feature */
#define CHECK_ANTITOUCH			1
#define SYSFS					1
#define FOR_BRINGUP				1
#define ITDEV					1
#define SHOW_COORDINATE				1
#define DEBUG_INFO				0
#define HIGH_RESOLUTION				0
#define FORCE_RELEASE				0
#define UPDATE_ON_PROBE				1
#define SEC_TSP_FACTORY_TEST
#define TOUCH_BOOSTER				0
#define PALM_CAL				1

#if defined(SEC_TSP_FACTORY_TEST)
#define TSP_BUF_SIZE				1024
#define TSP_CMD_STR_LEN			32
#define TSP_CMD_RESULT_STR_LEN	512
#define TSP_CMD_PARAM_NUM			8
#define NODE_NUM					768
#endif

extern struct class *sec_class;
extern int system_rev;
extern int touch_is_pressed;
struct tsp_callbacks {
	void (*inform_charger)(struct tsp_callbacks *tsp_cb, bool mode);
};
extern struct tsp_callbacks *charger_callbacks;

struct mxt224s_platform_data {
	int max_finger_touches;
	const u8 **config;
	const u8 **config_s;
	int gpio_read_done;
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int min_z;
	int max_z;
	int min_w;
	int max_w;
	u8 tchthr_batt;
	u8 tchthr_charging;
#if CHECK_ANTITOUCH
	u8 check_antitouch;
	u8 check_timer;
	u8 check_autocal;
	u8 check_calgood;
#endif
	const u8 *t62_config_batt_s;
	const u8 *t62_config_chrg_s;
	const u8 *t46_config_batt_s;
	const u8 *t46_config_chrg_s;
	const u8 *t8_config_normal_s;
	void (*power_onoff) (int);
	void (*register_cb)(struct tsp_callbacks *);
	const u8 *config_fw_version;
	void (*request_gpio)(void);
};

struct object_t {
	u8 object_type;
	u16 i2c_address;
	u8 size;
	u8 instances;
	u8 num_report_ids;
} __packed;

struct finger_info {
	s16 x;
	s16 y;
	s16 z;
	u16 w;
	s8 state;
	int16_t component;
	u16 mcount;	/*add for debug*/
};

struct report_id_map_t {
	u8 object_type;     /*!< Object type. */
	u8 instance;        /*!< Instance number. */
};


struct mxt_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct mxt224s_platform_data *pdata;
	struct early_suspend early_suspend;
	u8 family_id;
	u32 finger_mask;
	int gpio_read_done;
	struct object_t *objects;
	u8 objects_len;
	u8 tsp_version;
	u8 tsp_build;
	u8 tsp_variant;
	u8 finger_type;
	u8 x_line;
	u8 y_line;
	u16 msg_proc;
	u16 cmd_proc;
	u16 msg_object_size;
	u32 x_dropbits:2;
	u32 y_dropbits:2;
	u8 tchthr_batt;
	u8 tchthr_charging;
	u8 calcfg_batt;
	u8 calcfg_charging;
	u8 disable_config_write;
	const u8 *t8_config_normal_s;
	const u8 *t46_config_batt_s;
	const u8 *t46_config_chrg_s;
	const u8 *t62_config_batt_s;
	const u8 *t62_config_chrg_s;
	unsigned char Report_touch_number;
	unsigned char Press_Release_check;
	u16 palm_cnt;
	u16 Press_cnt;
	u16 Release_cnt;
	u16 twotouch_check_1st;
	u16 twotouch_check_2nd;
	u16 finger_area;

	u8 max_id;
	u8 old_id;
	u16 distance[10];
#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
	struct delayed_work work_dvfs_off;
	struct delayed_work	work_dvfs_chg;
	bool	dvfs_lock_status;
	struct mutex dvfs_lock;
#endif
#endif
	void (*power_onoff)(int);
	struct tsp_callbacks callbacks;
	bool ta_status;
	void (*register_cb)(struct tsp_callbacks *);
	int num_fingers;
	const u8 *config_fw_version;
#if ITDEV
	u16 last_read_addr;
	u16 msg_proc_addr;
	int driver_paused;
	int debug_enabled;
#endif
	u8 max_report_id;
	struct report_id_map_t *rid_map;
	bool rid_map_alloc;
	unsigned char coin_check;
	unsigned int t_area_l_cnt;
	unsigned int t_area_cnt;
	unsigned char cal_busy;
/*
 * t_area_cnt : touch area count
 * t_area_l_cnt : touch area low count
 */
	int tcount[10];
	int touchbx[MAX_USING_FINGER_NUM];
	int touchby[MAX_USING_FINGER_NUM];
	u16 pre_x[MXT224S_MAX_MT_FINGERS][4];
	u16 pre_y[MXT224S_MAX_MT_FINGERS][4];
	struct finger_info fingers[MXT224S_MAX_MT_FINGERS];
#if defined(SEC_TSP_FACTORY_TEST)
	struct list_head			cmd_list_head;
	u8			cmd_state;
	char			cmd[TSP_CMD_STR_LEN];
	int			cmd_param[TSP_CMD_PARAM_NUM];
	char			cmd_result[TSP_CMD_RESULT_STR_LEN];
	struct mutex			cmd_lock;
	bool			cmd_is_running;

#endif /* SEC_TSP_FACTORY_TEST */
};

/* mxt224e.h */
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

#define	MXT_PAGE_UP		0x01
#define	MXT_PAGE_DOWN		0x02
#define	MXT_DELTA_MODE		0x10
#define	MXT_REFERENCE_MODE	0x11
#define	MXT_CTE_MODE		0x31

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

#endif	/* __MXT224S_H__ */

