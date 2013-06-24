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

#ifndef __MXT_224S_KS02_DEV_H
#define __MXT_224S_KS02_DEV_H

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define OBJECT_TABLE_ELEMENT_SIZE	6
#define OBJECT_TABLE_START_ADDRESS	7

#define CMD_RESET_OFFSET		0
#define CMD_BACKUP_OFFSET		1
#define CMD_CALIBRATE_OFFSET		2
#define CMD_REPORTATLL_OFFSET		3
#define CMD_DEBUG_CTRL_OFFSET		4
#define CMD_DIAGNOSTIC_OFFSET		5

#define DETECT_MSG_MASK			0x80
#define PRESS_MSG_MASK			0x40
#define RELEASE_MSG_MASK		0x20
#define MOVE_MSG_MASK			0x10
#define VECTOR_MSG_MASK		0x08
#define AMPLITUDE_MSG_MASK		0x04
#define SUPPRESS_MSG_MASK		0x02

/* Slave addresses */
#define MXT_APP_LOW			0x4a
#define MXT_APP_HIGH		0x4b
#define MXT_BOOT_LOW		0x24
#define MXT_BOOT_HIGH		0x25

#define MXT_BOOT_VALUE		0xa5
#define MXT_BACKUP_VALUE	0x55

/* Bootloader mode status */
#define MXT_WAITING_BOOTLOAD_CMD	0xc0	/* valid 7 6 bit only */
#define MXT_WAITING_FRAME_DATA		0x80	/* valid 7 6 bit only */
#define MXT_FRAME_CRC_CHECK		0x02
#define MXT_FRAME_CRC_FAIL		0x03
#define MXT_FRAME_CRC_PASS		0x04
#define MXT_APP_CRC_FAIL		0x40	/* valid 7 8 bit only */
#define MXT_BOOT_STATUS_MASK	0x3f

/* Bootloader ID */
#define MXT_BOOT_EXTENDED_ID		0x20
#define MXT_BOOT_ID_MASK		0x1f


/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB		0xaa
#define MXT_UNLOCK_CMD_LSB		0xdc

#define ID_BLOCK_SIZE			7
#define MXT_STATE_INACTIVE		-1
#define MXT_STATE_RELEASE		0
#define MXT_STATE_PRESS			1
#define MXT_STATE_MOVE			2


/* Diagnostic cmds  */
#define MXT_DIAG_PAGE_UP		0x01
#define MXT_DIAG_PAGE_DOWN		0x02
#define MXT_DIAG_DELTA_MODE		0x10
#define MXT_DIAG_REFERENCE_MODE		0x11
#define MXT_DIAG_CTE_MODE		0x31
#define MXT_DIAG_IDENTIFICATION_MODE	0x80
#define MXT_DIAG_TOCH_THRESHOLD_MODE	0xF4

#define MXT_DIAG_MODE_MASK	0xFC
#define MXT_DIAGNOSTIC_MODE	0
#define MXT_DIAGNOSTIC_PAGE	1

/* FIRMWARE NAME */
#define MXT_FW_NAME			"tsp_atmel/mxt224s.fw"
#define MXT_MAX_FW_PATH		255

/* Firmware version */
#define MXT_FIRM_VERSION	0x10
#define MXT_FIRM_BUILD		0xAA
#define MAX_USING_FINGER_NUM 10
#define MXT224S_MAX_MT_FINGERS		0x0A


#define MXT_SW_RESET_TIME		300

#define MATRIX_X 19
#define MATRIX_Y 14
#define MATRIX_REAL_X	18
#define MATRIX_REAL_Y	11

/* Feature */
/*#######################################*/
#define TOUCH_BOOSTER				0
#define SYSFS	1
#define FOR_BRINGUP  1
#define UPDATE_ON_PROBE   1
#define READ_FW_FROM_HEADER	1
#define FOR_DEBUGGING_TEST_DOWNLOADFW_BIN 0
#define TSP_SEC_SYSFS			1
#define ITDEV	0
#define DEBUG_INFO	1
#define HIGH_RESOLUTION							0
#define TREAT_ERR				0
#define FORCE_RELEASE				0
#define DOWNLOAD_CONFIG				0
#ifdef CONFIG_DUAL_LCD
#define DUAL_TSP		1
#else
#define DUAL_TSP		0
#endif
#define TSP_INFORM_CHARGER	0
/*#######################################*/

/* TSP_ITDEV feature just for atmel tunning app
* so it should be disabled after finishing tunning
* because it use other write permission. it will be cause
* failure of CTS
*/

#define MXT_T7_IDLE_ACQ_INT	0
#define MXT_T7_ACT_ACQ_INT	1

#if CHECK_ANTITOUCH
#define MXT_T61_TIMER_ONESHOT			0
#define MXT_T61_TIMER_REPEAT			1
#define MXT_T61_TIMER_CMD_START		1
#define MXT_T61_TIMER_CMD_STOP		2
#endif



#if TSP_SEC_SYSFS
#define TSP_BUF_SIZE	 1024

#define NODE_NUM	384

#define NODE_PER_PAGE	64
#define DATA_PER_NODE	2

#define MIN_VALUE		19744
#define MAX_VALUE		28884

#define TSP_CMD_STR_LEN		32
#define TSP_CMD_RESULT_STR_LEN	512
#define TSP_CMD_PARAM_NUM	8

enum CMD_STATUS {
	CMD_STATUS_WAITING = 0,
	CMD_STATUS_RUNNING,
	CMD_STATUS_OK,
	CMD_STATUS_FAIL,
	CMD_STATUS_NOT_APPLICABLE,
};

enum {
	MXT_FW_FROM_BUILT_IN = 0,
	MXT_FW_FROM_UMS,
	MXT_FW_FROM_REQ_FW,
};
#endif

/* touch booster */
#if TOUCH_BOOSTER
#include <mach/cpufreq.h>
#define TOUCH_BOOSTER_TIME	          3000
#define TOUCH_BOOSTER_LIMIT_CLK	        500000

static bool tsp_press_status;
static bool touch_cpu_lock_status;
static int cpu_lv = -1;
#endif

/* Firmware */
#if READ_FW_FROM_HEADER
static u8 firmware_mXT[] = {
	#include "mxt224s_V1.1.AA_.h"
};
#endif


struct object_t {
	u8 object_type;
	u16 i2c_address;
	u8 size;
	u8 instances;
	u8 num_report_ids;
} __packed;

/*#define CONFIG_READ_FROM_FILE*/
#ifdef CONFIG_READ_FROM_FILE
#define CONFIG_READ_FROM_SDCARD
#define MXT_CFG_MAGIC		"OBP_RAW V1"
#define MXT_BATT_CFG_NAME	"/sdcard/mxt224s_batt_config.raw"
#define MXT_TA_CFG_NAME		"/sdcard/mxt224s_ta_config.raw"
struct mxt_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
};
#endif

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

#if TOUCH_BOOSTER
struct touch_booster {
	bool touch_cpu_lock_status;
	int cpu_lv;
	struct delayed_work dvfs_dwork;
	struct device *bus_dev;
	struct device *dev;
};
#endif

#if TSP_SEC_SYSFS
struct mxt_data_sysfs {
	struct list_head			cmd_list_head;
	u8			cmd_state;
	char			cmd[TSP_CMD_STR_LEN];
	int			cmd_param[TSP_CMD_PARAM_NUM];
	char			cmd_result[TSP_CMD_RESULT_STR_LEN];
	struct mutex			cmd_lock;
	bool			cmd_is_running;

	u16 reference[NODE_NUM];
	s16 delta[NODE_NUM];

	u32 ref_max_data;
	u32 ref_min_data;
	s16 delta_max_data;
	u16 delta_max_node;
};
#endif

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
#if DUAL_TSP
	const u8 *power_cfg;
#endif
#if TSP_SEC_SYSFS
	struct mxt_data_sysfs *sysfs_data;
#endif
	u8 finger_type;
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
	unsigned char Report_touch_number;
	u8 max_id;
	u8 old_id;
	u16 distance[10];
#if TOUCH_BOOSTER
	struct delayed_work dvfs_dwork;
#endif
	int (*power_on)(void);
	int (*power_off)(void);
	struct tsp_callbacks callbacks;
	bool ta_status;
	void (*register_cb)(struct tsp_callbacks *);
	int num_fingers;
#if ITDEV
	u16 last_read_addr;
	u16 msg_proc_addr;
#endif
#ifdef CONFIG_READ_FROM_FILE
	struct mxt_info info;
#endif
	u8 max_report_id;
	struct report_id_map_t *rid_map;
	bool rid_map_alloc;

	u16 nSumCnt;
	int tcount[10];
	int touchbx[MAX_USING_FINGER_NUM];
	int touchby[MAX_USING_FINGER_NUM];
	struct mutex lock;
	struct finger_info fingers[MXT224S_MAX_MT_FINGERS];

};

static int mxt_enabled;

#if ITDEV
static int driver_paused;
static int debug_enabled;
#endif

#if TSP_SEC_SYSFS
extern struct class *sec_class;
#endif

static u8 firmware_latest[] = {0x11, 0xaa};	/* version, build_version */

extern int  __devinit mxt_sysfs_init(struct i2c_client *client);

extern int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf);
extern int write_mem(struct mxt_data *data, u16 reg, u8 len, const u8 *buf);
extern struct object_t *
		mxt_get_object(struct mxt_data *data, u8 type);
extern int mxt_read_object(struct mxt_data *data,
				u8 type, u8 offset, u8 *val);
extern int mxt_write_object(struct mxt_data *data,
				 u8 type, u8 offset, u8 val);
extern int read_all_data(uint16_t dbg_mode);
extern int read_all_delta_data(uint16_t dbg_mode);

#if TSP_SEC_SYSFS
extern  int set_mxt_firm_update_store(struct mxt_data *data, const char *buf, size_t size);
#endif

#if TOUCH_BOOSTER
extern void mxt_set_dvfs_on(struct mxt_data *data);
extern void mxt_set_dvfs_off(struct work_struct *work);
extern int mxt_init_dvfs(struct mxt_data *data);
#endif	/* TSP_BOOSTER */

#endif /* __MXT_224S_KS02_DEV_H */
