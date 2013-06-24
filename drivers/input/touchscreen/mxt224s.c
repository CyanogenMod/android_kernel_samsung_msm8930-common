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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c/mxt224s.h>
#include <asm/unaligned.h>
#include <linux/firmware.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/cpufreq.h>

#define OBJECT_TABLE_START_ADDRESS	7
#define OBJECT_TABLE_ELEMENT_SIZE	6

#define CMD_RESET_OFFSET		0
#define CMD_BACKUP_OFFSET		1
#define CMD_CALIBRATE_OFFSET    2
#define CMD_REPORTATLL_OFFSET   3
#define CMD_DEBUG_CTRL_OFFSET   4
#define CMD_DIAGNOSTIC_OFFSET   5


#define DETECT_MSG_MASK			0x80
#define PRESS_MSG_MASK			0x40
#define RELEASE_MSG_MASK		0x20
#define MOVE_MSG_MASK			0x10
#define AMPLITUDE_MSG_MASK		0x04
#define SUPPRESS_MSG_MASK		0x02

#if CHECK_ANTITOUCH
#define MXT_T61_TIMER_ONESHOT			0
#define MXT_T61_TIMER_REPEAT			1
#define MXT_T61_TIMER_CMD_START		1
#define MXT_T61_TIMER_CMD_STOP		2
#endif

/* mxt224s firmware update ~ */
/* Slave addresses */
#define MXT_APP_LOW			0x4a
#define MXT_APP_HIGH		0x4b
#define MXT_BOOT_LOW		0x24
#define MXT_BOOT_HIGH		0x25

/* FIRMWARE NAME */
#define MXT_FW_NAME			"touchscreen/mxt224s.fw"
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

/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB		0xaa
#define MXT_UNLOCK_CMD_LSB		0xdc
#define MXT_SW_RESET_TIME		300
#define MXT_STATE_INACTIVE		-1
#define MXT_STATE_RELEASE		0
#define MXT_STATE_PRESS			1
#define MXT_STATE_MOVE			2
#define ID_BLOCK_SIZE			7

/* touch booster */
#ifdef CONFIG_SEC_DVFS
#define TOUCH_BOOSTER			0
#define TOUCH_BOOSTER_OFF_TIME	100
#define TOUCH_BOOSTER_CHG_TIME	200
#endif

#if !defined(CONFIG_TOUCHSCREEN_MXT224)
int touch_is_pressed;
EXPORT_SYMBOL(touch_is_pressed);
#endif

static int mxt_enabled;
static bool g_debug_switch;
static int tsp_probe_num;
static u8 firmware_latest[] = {0x11, 0xaa};	/* version, build_version */

#define MIN_VALUE 19744
#define MAX_VALUE 28884
static u16 max_ref, min_ref;

static unsigned char test_node[5] = {15, 23, 131, 239, 247};
static int mxt_delta_node[384] = { 0 };
static uint16_t mxt_refer_node[384] = { 0 };
static int index_reference;
static int index_delta;

#if DEBUG_INFO
static u8	*object_type_name[63] = {
	[5]	= "GEN_MESSAGEPROCESSOR_T5",
	[6]	= "GEN_COMMANDPROCESSOR_T6",
	[7]	= "GEN_POWERCONFIG_T7",
	[8]	= "GEN_ACQUIRECONFIG_T8",
	[9]	= "TOUCH_MULTITOUCHSCREEN_T9",
	[15]	= "TOUCH_KEYARRAY_T15",
	[18]	= "SPT_COMCONFIG_T18",
	[23]	= "TOUCH_PROXIMITY_T23",
	[25]	= "SPT_SELFTEST_T25",
	[37]	= "DEBUG_DIAGNOSTICS_T37",
	[38]	= "USER_DATA_T38",
	[40]	= "PROCI_GRIPSUPPRESSION_T40",
	[42]	= "PROCI_TOUCHSUPPRESSION_T42",
	[44]	= "MESSAGECOUNT_T44",
	[46]	= "SPT_CTECONFIG_T46",
	[47]	= "PROCI_STYLUS_T47",
	[55]	= "PROCI_ADAPTIVETHRESHOLD_T55",
	[56]	= "PROCI_SHIELDLESS_T56",
	[57]	= "SPT_GENERICDATA_T57",
	[61]	= "SPT_TIMER_T61",
	[62]	= "PROCG_NOISESUPPRESSION_T62",
};
#endif

#if defined(SEC_TSP_FACTORY_TEST)

#define TSP_CMD(name, func) .cmd_name = name, .cmd_func = func

struct tsp_cmd {
	struct list_head	list;
	const char	*cmd_name;
	void	(*cmd_func)(void *device_data);
};

static void fw_update(void *device_data);
static void get_fw_ver_bin(void *device_data);
static void get_fw_ver_ic(void *device_data);
static void get_config_ver(void *device_data);
static void get_threshold(void *device_data);
static void module_off_master(void *device_data);
static void module_on_master(void *device_data);
static void get_chip_vendor(void *device_data);
static void get_chip_name(void *device_data);
static void get_x_num(void *device_data);
static void get_y_num(void *device_data);
static void get_reference(void *device_data);
static void get_delta(void *device_data);
static void run_reference_read(void *device_data);
static void run_delta_read(void *device_data);
static void not_support_cmd(void *device_data);

struct tsp_cmd tsp_cmds[] = {
	{TSP_CMD("fw_update", fw_update),},
	{TSP_CMD("get_fw_ver_bin", get_fw_ver_bin),},
	{TSP_CMD("get_fw_ver_ic", get_fw_ver_ic),},
	{TSP_CMD("get_config_ver", get_config_ver),},
	{TSP_CMD("get_threshold", get_threshold),},
	{TSP_CMD("module_off_master", module_off_master),},
	{TSP_CMD("module_on_master", module_on_master),},
	{TSP_CMD("module_off_slave", not_support_cmd),},
	{TSP_CMD("module_on_slave", not_support_cmd),},
	{TSP_CMD("get_chip_vendor", get_chip_vendor),},
	{TSP_CMD("get_chip_name", get_chip_name),},
	{TSP_CMD("get_x_num", get_x_num),},
	{TSP_CMD("get_y_num", get_y_num),},
	{TSP_CMD("get_reference", get_reference),},
	{TSP_CMD("get_delta", get_delta),},
	{TSP_CMD("run_reference_read", run_reference_read),},
	{TSP_CMD("run_delta_read", run_delta_read),},
	{TSP_CMD("not_support_cmd", not_support_cmd),},
};

static int read_all_data(struct mxt_data *data, uint16_t dbg_mode);
static int read_all_delta_data(struct mxt_data *data, uint16_t dbg_mode);
static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf);
static int mxt_internal_suspend(struct mxt_data *data);
static int mxt_internal_resume(struct mxt_data *data);
static uint8_t calibrate_chip(struct mxt_data *data);
static int write_config(struct mxt_data *data, u8 type, const u8 *cfg);

static void set_default_result(struct mxt_data *data)
{
	char delim = ':';

	memset(data->cmd_result, 0x00, ARRAY_SIZE(data->cmd_result));
	memcpy(data->cmd_result, data->cmd, strlen(data->cmd));
	strncat(data->cmd_result, &delim, 1);
}

static void set_cmd_result(struct mxt_data *data, char *buff, int len)
{
	strncat(data->cmd_result, buff, len);
}

static void not_support_cmd(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	char buff[16] = {0};

	set_default_result(data);
	sprintf(buff, "%s", "NA");
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 4;
	dev_info(&data->client->dev, "%s: \"%s(%d)\"\n", __func__,
				buff, strnlen(buff, sizeof(buff)));
	return;
}

static void fw_update(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%#02x", firmware_latest[0]);

	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 4;

	dev_info(&data->client->dev, "%s: do not supply firmware update\n"
			, __func__);
	return;
}

static void get_fw_ver_bin(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "AT00%02x%02x",
			firmware_latest[0], firmware_latest[1]);

	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_fw_ver_ic(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};
	u8 id[ID_BLOCK_SIZE];
	u8 value;
	int ret;
	u8 i;

	set_default_result(data);

	if (mxt_enabled == 1) {
		disable_irq(data->client->irq);
		for (i = 0; i < 4; i++) {
			ret = read_mem(data, 0, sizeof(id), id);
			if (!ret)
				break;
		}
		enable_irq(data->client->irq);
		if (ret < 0)
			pr_err("TSP read fail : %s", __func__);
		else
			pr_info("%s : %#02x\n", __func__, id[2]);

	} else {
		pr_err("TSP power off : %s", __func__);
		value = 0;
	}

	snprintf(buff, sizeof(buff), "AT00%02x%02x", id[2], id[3]);

	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_config_ver(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[20] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%s", data->config_fw_version);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_threshold(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%d", data->tchthr_batt);

	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void module_off_master(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[3] = {0};

	if (mxt_enabled == 1) {
		mxt_enabled = 0;
		touch_is_pressed = 0;

		disable_irq(data->client->irq);
		mxt_internal_suspend(data);
	}

	snprintf(buff, sizeof(buff), "%s", "OK");

	set_default_result(data);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	if (strncmp(buff, "OK", 2) == 0)
		data->cmd_state = 2;
	else
		data->cmd_state = 3;

	dev_info(&data->client->dev, "%s: %s\n", __func__, buff);
}

static void module_on_master(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[3] = {0};

	if (mxt_enabled == 0) {
		mxt_internal_resume(data);
		enable_irq(data->client->irq);

		mxt_enabled = 1;

		if (data->ta_status)
			write_config(data,
				data->t62_config_chrg_s[0],
				data->t62_config_chrg_s + 1);
		else
			write_config(data,
				data->t62_config_batt_s[0],
				data->t62_config_batt_s + 1);

		calibrate_chip(data);
	}

	snprintf(buff, sizeof(buff), "%s", "OK");

	set_default_result(data);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	if (strncmp(buff, "OK", 2) == 0)
		data->cmd_state = 2;
	else
		data->cmd_state = 3;

	dev_info(&data->client->dev, "%s: %s\n", __func__, buff);

}

static void get_chip_vendor(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%s", "ATMEL");
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_chip_name(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%s", "MXT224S");
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;
	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_reference(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};
	unsigned int val;
	int node;

	set_default_result(data);
	node = (data->cmd_param[0] * 14) + data->cmd_param[1];
	val = mxt_refer_node[node];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

}

static void get_delta(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};
	int val;
	int node;

	set_default_result(data);
	node = (data->cmd_param[0] * 14) + data->cmd_param[1];
	val = mxt_delta_node[node];

	snprintf(buff, sizeof(buff), "%d", val);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

}

static void get_x_num(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%u", data->x_line);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

static void get_y_num(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;

	char buff[16] = {0};

	set_default_result(data);

	snprintf(buff, sizeof(buff), "%u", data->y_line);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));
	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

static void run_reference_read(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	int status = 0;
	char buff[16] = {0};

	set_default_result(data);

	status = read_all_data(data, MXT_REFERENCE_MODE);
	snprintf(buff, sizeof(buff), "%d,%d", min_ref, max_ref);
	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s\n", __func__);
}

static void run_delta_read(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	int status = 0;
	char buff[16] = {0};

	set_default_result(data);

	status = read_all_delta_data(data, MXT_DELTA_MODE);

	set_cmd_result(data, buff, strnlen(buff, sizeof(buff)));

	data->cmd_state = 2;

	dev_info(&data->client->dev, "%s\n", __func__);
}

static ssize_t store_cmd(struct device *dev, struct device_attribute
		*devattr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	char *cur, *start, *end;
	char buff[TSP_CMD_STR_LEN] = {0};
	int len, i;
	struct tsp_cmd *tsp_cmd_ptr = NULL;
	char delim = ',';
	bool cmd_found = false;
	int param_cnt = 0;


	if (data->cmd_is_running == true) {
		dev_err(&data->client->dev, "tsp_cmd: other cmd is running.\n");
		goto err_out;
	}


	/* check lock  */
	mutex_lock(&data->cmd_lock);
	data->cmd_is_running = true;
	mutex_unlock(&data->cmd_lock);

	data->cmd_state = 1;

	for (i = 0; i < ARRAY_SIZE(data->cmd_param); i++)
		data->cmd_param[i] = 0;

	len = (int)count;
	if (*(buf + len - 1) == '\n')
		len--;
	memset(data->cmd, 0x00, ARRAY_SIZE(data->cmd));
	memcpy(data->cmd, buf, len);

	cur = strchr(buf, (int)delim);
	if (cur)
		memcpy(buff, buf, cur - buf);
	else
		memcpy(buff, buf, len);

	/* find command */
	list_for_each_entry(tsp_cmd_ptr, &data->cmd_list_head, list) {
		if (!strcmp(buff, tsp_cmd_ptr->cmd_name)) {
			cmd_found = true;
			break;
		}
	}

	/* set not_support_cmd */
	if (!cmd_found) {
		list_for_each_entry(tsp_cmd_ptr, &data->cmd_list_head, list) {
			if (!strcmp("not_support_cmd", tsp_cmd_ptr->cmd_name))
				break;
		}
	}

	/* parsing parameters */
	if (cur && cmd_found) {
		cur++;
		start = cur;
		memset(buff, 0x00, ARRAY_SIZE(buff));
		do {
			if (*cur == delim || cur - buf == len) {
				end = cur;
				memcpy(buff, start, end - start);
				*(buff + strlen(buff)) = '\0';
				if (kstrtoint(buff, 10,
					data->cmd_param + param_cnt) < 0)
					goto err_out;
				start = cur + 1;
				memset(buff, 0x00, ARRAY_SIZE(buff));
				param_cnt++;
			}
			cur++;
		} while (cur - buf <= len);
	}

	dev_info(&client->dev, "cmd = %s\n", tsp_cmd_ptr->cmd_name);
	for (i = 0; i < param_cnt; i++)
		dev_info(&client->dev, "cmd param %d= %d\n", i,
							data->cmd_param[i]);

	tsp_cmd_ptr->cmd_func(data);


err_out:
	return count;
}

static ssize_t show_cmd_status(struct device *dev,
		struct device_attribute *devattr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	char buff[16] = {0};

	dev_info(&data->client->dev, "tsp cmd: status:%d\n",
			data->cmd_state);

	if (data->cmd_state == 0)
		snprintf(buff, sizeof(buff), "WAITING");

	else if (data->cmd_state == 1)
		snprintf(buff, sizeof(buff), "RUNNING");

	else if (data->cmd_state == 2)
		snprintf(buff, sizeof(buff), "OK");

	else if (data->cmd_state == 3)
		snprintf(buff, sizeof(buff), "FAIL");

	else if (data->cmd_state == 4)
		snprintf(buff, sizeof(buff), "NOT_APPLICABLE");

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", buff);
}

static ssize_t show_cmd_result(struct device *dev, struct device_attribute
		*devattr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	dev_info(&data->client->dev, "tsp cmd: result: %s\n", data->cmd_result);

	mutex_lock(&data->cmd_lock);
	data->cmd_is_running = false;
	mutex_unlock(&data->cmd_lock);

	data->cmd_state = 0;

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", data->cmd_result);
}

static ssize_t show_close_tsp_test(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return snprintf(buf, TSP_BUF_SIZE, "%u\n", 0);
}

static DEVICE_ATTR(close_tsp_test, S_IRUGO, show_close_tsp_test, NULL);
static DEVICE_ATTR(cmd, S_IWUSR | S_IWGRP, NULL, store_cmd);
static DEVICE_ATTR(cmd_status, S_IRUGO, show_cmd_status, NULL);
static DEVICE_ATTR(cmd_result, S_IRUGO, show_cmd_result, NULL);

static struct attribute *sec_touch_facotry_attributes[] = {
		&dev_attr_close_tsp_test.attr,
		&dev_attr_cmd.attr,
		&dev_attr_cmd_status.attr,
		&dev_attr_cmd_result.attr,
		NULL,
};

static struct attribute_group sec_touch_factory_attr_group = {
	.attrs = sec_touch_facotry_attributes,
};
#endif /* SEC_TSP_FACTORY_TEST */

#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
static void change_dvfs_lock(struct work_struct *work)
{
	struct mxt_data *data = container_of(work,
				struct mxt_data, work_dvfs_chg.work);
	int ret;

	mutex_lock(&data->dvfs_lock);
	ret = cpufreq_set_limit(TOUCH_BOOSTER_FIRST_STOP, 0);
	mutex_unlock(&data->dvfs_lock);

	if (ret < 0)
		pr_err("%s: 1booster stop failed(%d)\n",\
					__func__, __LINE__);
	else
		pr_info("[TSP] %s", __func__);
}

static void set_dvfs_off(struct work_struct *work)
{
	struct mxt_data *data = container_of(work,
				struct mxt_data, work_dvfs_off.work);

	mutex_lock(&data->dvfs_lock);
	cpufreq_set_limit(TOUCH_BOOSTER_FIRST_STOP, 0);
	cpufreq_set_limit(TOUCH_BOOSTER_SECOND_STOP, 0);
	data->dvfs_lock_status = false;
	mutex_unlock(&data->dvfs_lock);

	pr_info("[TSP] DVFS Off!\n");
}

static void set_dvfs_lock(struct mxt_data *data, uint32_t on)
{
	int ret = 0, ret2 = 0;

	mutex_lock(&data->dvfs_lock);
	if (on == 0) {
		if (data->dvfs_lock_status) {
			cancel_delayed_work(&data->work_dvfs_chg);
			schedule_delayed_work(&data->work_dvfs_off,
				msecs_to_jiffies(TOUCH_BOOSTER_OFF_TIME));
		}
	} else if (on == 1) {
		cancel_delayed_work(&data->work_dvfs_off);
		if (!data->dvfs_lock_status) {
			ret = cpufreq_set_limit(TOUCH_BOOSTER_SECOND_START, 0);
			ret2 = cpufreq_set_limit(TOUCH_BOOSTER_FIRST_START, 0);
			if (ret < 0 || ret2 < 0)
				pr_err("%s: cpu lock failed(%d, %d)\n",\
							__func__, ret, ret2);

			schedule_delayed_work(&data->work_dvfs_chg,
				msecs_to_jiffies(TOUCH_BOOSTER_CHG_TIME));
			data->dvfs_lock_status = true;
			pr_info("[TSP] DVFS On!\n");
		}
	} else if (on == 2) {
		cancel_delayed_work(&data->work_dvfs_off);
		cancel_delayed_work(&data->work_dvfs_chg);
		schedule_work(&data->work_dvfs_off.work);
	}
	mutex_unlock(&data->dvfs_lock);
}
#endif
#endif
/* declare function proto type */
static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf)
{
	int ret;
	u16 le_reg = cpu_to_le16(reg);
	struct i2c_msg msg[2] = {
		{
			.addr = data->client->addr,
			.flags = 0,
			.len = 2,
			.buf = (u8 *)&le_reg,
		},
		{
			.addr = data->client->addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = buf,
		},
	};

	ret = i2c_transfer(data->client->adapter, msg, 2);

	if (ret < 0) {
		pr_err("[TSP] i2c failed ret = %d\n", ret);
		return ret;
	}
	return ret == 2 ? 0 : -EIO;
}

static int write_mem(struct mxt_data *data, u16 reg, u8 len, const u8 *buf)
{
	int ret;
	u8 tmp[len + 2];

	put_unaligned_le16(cpu_to_le16(reg), tmp);
	memcpy(tmp + 2, buf, len);

	ret = i2c_master_send(data->client, tmp, sizeof(tmp));

	if (ret < 0) {
		data->power_onoff(0);
		data->power_onoff(1);
		printk(KERN_ERR "[TSP] %s failed. reset\n", __func__);
		ret = i2c_master_send(data->client, tmp, sizeof(tmp));
		if (ret < 0)
			return ret;
	}
	return ret == sizeof(tmp) ? 0 : -EIO;
}

static int mxt_reset(struct mxt_data *data)
{
	u8 buf = 1u;
#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
	set_dvfs_lock(data, 2);
	pr_info("[TSP] dvfs_lock free.\n");
#endif
#endif
	return write_mem(data, data->cmd_proc + CMD_RESET_OFFSET, 1, &buf);
}

static int mxt_backup(struct mxt_data *data)
{
	u8 buf = 0x55u;
	return write_mem(data, data->cmd_proc + CMD_BACKUP_OFFSET, 1, &buf);
}

static int get_object_info(struct mxt_data *data, u8 object_type, u16 *size,
				u16 *address)
{
	int i;

	for (i = 0; i < data->objects_len; i++) {
		if (data->objects[i].object_type == object_type) {
			*size = data->objects[i].size + 1;
			*address = data->objects[i].i2c_address;
			return 0;
		}
	}

	return -ENODEV;
}

#if CHECK_ANTITOUCH
void mxt_t61_timer_set(struct mxt_data *data, u8 mode, u8 cmd, u16 msPeriod)
{
	int ret = 0;
	u8 buf[5] = {3, 0, 0, 0, 0};
	u16 size = 0;
	u16 obj_address = 0;

	get_object_info(data, SPT_TIMER_T61, &size, &obj_address);
	buf[1] = cmd;
	buf[2] = mode;
	buf[3] = msPeriod & 0xFF;
	buf[4] = (msPeriod >> 8) & 0xFF;

	ret = write_mem(data, obj_address, 5, buf);
	if (ret)
		dev_err(&data->client->dev,
			"%s write error T%d address[0x%x]\n",
			__func__, SPT_TIMER_T61, obj_address);

	dev_info(&data->client->dev,
		"[TSP] T61 Timer Enabled %d\n", msPeriod);
}

void mxt_t8_cal_set(struct mxt_data *data, u8 mstime)
{
	int ret = 0;
	u16 size = 0;
	u16 obj_address = 0;

	if (mstime)
		data->pdata->check_autocal = 1;
	else
		data->pdata->check_autocal = 0;

	get_object_info(data,
			GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);

	ret = write_mem(data, obj_address+4, 1, &mstime);

	if (ret)
		dev_err(&data->client->dev,
			"%s write error T%d address[0x%x]\n",
			__func__, SPT_TIMER_T61, obj_address+4);
}

static int diff_two_point(s16 x, s16 y, s16 oldx, s16 oldy)
{
	s16 diffx, diffy;
	s16 distance;

	diffx = x-oldx;
	diffy = y-oldy;
	distance = abs(diffx) + abs(diffy);

	return distance;
}

static void mxt_check_coordinate(struct mxt_data *data,
				u8 detect, u8 id,
				s16 x, s16 y)
{
	if (detect) {
		data->tcount[id] = 0;
		data->distance[id] = 0;
	}
	if (data->tcount[id] >= 1) {
		data->distance[id] = diff_two_point(x, y,
					data->touchbx[id], data->touchby[id]);
	}
	if (data->tcount[id] > 10000)
		data->tcount[id] = 1;
	else
		data->tcount[id]++;

	data->touchbx[id] = x;
	data->touchby[id] = y;

	if (id >= data->old_id)
		data->max_id = id;
	else
		data->max_id = data->old_id;

	data->old_id = id;
}

#endif	/* CHECK_ANTITOUCH */

static int write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
	int ret;
	u16 address = 0;
	u16 size = 0;

	ret = get_object_info(data, type, &size, &address);

	if (size == 0 && address == 0)
		return 0;
	else
		return write_mem(data, address, size, cfg);
}

static int check_instance(struct mxt_data *data, u8 object_type)
{
	int i;

	for (i = 0; i < data->objects_len; i++) {
		if (data->objects[i].object_type == object_type)
			return data->objects[i].instances;
	}
	return 0;
}

static int init_write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
	int ret;
	u16 address = 0;
	u16 size = 0;
	u8 *temp;

	ret = get_object_info(data, type, &size, &address);

	if ((size == 0) || (address == 0)) {
		pr_err("%s error object_type(%d)\n", __func__, type);
		return -ENODEV;
	}

	ret = write_mem(data, address, size, cfg);

	if (check_instance(data, type)) {
#if DEBUG_INFO
		pr_info("exist instance1 object (%d)\n", type);
#endif
		temp = kmalloc(size * sizeof(u8), GFP_KERNEL);
		memset(temp, 0, size);
		ret |= write_mem(data, address+size, size, temp);
		kfree(temp);
	}

	return ret;
}


static u32 __devinit crc24(u32 crc, u8 byte1, u8 byte2)
{
	static const u32 crcpoly = 0x80001B;
	u32 res;
	u16 data_word;

	data_word = (((u16)byte2) << 8) | byte1;
	res = (crc << 1) ^ (u32)data_word;

	if (res & 0x1000000)
		res ^= crcpoly;

	return res;
}

static int __devinit calculate_infoblock_crc(struct mxt_data *data,
							u32 *crc_pointer)
{
	u32 crc = 0;
	u8 mem[7 + data->objects_len * 6];
	int status;
	int i;

	status = read_mem(data, 0, sizeof(mem), mem);

	if (status)
		return status;

	for (i = 0; i < sizeof(mem) - 1; i += 2)
		crc = crc24(crc, mem[i], mem[i + 1]);

	*crc_pointer = crc24(crc, mem[i], 0) & 0x00FFFFFF;

	return 0;
}

static uint8_t calibrate_chip(struct mxt_data *data)
{
	u8 cal_data = 1;
	int ret = 0;
	/* send calibration command to the chip */
	if (data->cal_busy)
		return ret;
	ret = write_mem(data,
		data->cmd_proc + CMD_CALIBRATE_OFFSET,
		1, &cal_data);
	/* set flag for calibration lockup
	recovery if cal command was successful */
	data->cal_busy = 1;
	if (!ret)
		pr_info("[TSP] calibration success!!!\n");
	return ret;
}

static void mxt_ta_cb(struct tsp_callbacks *cb, bool ta_status)
{
	struct mxt_data *data =
			container_of(cb, struct mxt_data, callbacks);

	data->ta_status = ta_status;

	if (mxt_enabled) {
		if (ta_status) {
			write_config(data,
					data->t62_config_chrg_s[0],
					data->t62_config_chrg_s + 1);

			   write_config(data,
			   data->t46_config_chrg_s[0],
			   data->t46_config_chrg_s + 1);

		pr_info("[TSP]%s:threshold_chrg: %d, probe_num: %d\n",
			__func__, data->tchthr_charging,
					tsp_probe_num);
		} else {
			write_config(data,
					data->t62_config_batt_s[0],
					data->t62_config_batt_s + 1);

			write_config(data,
			   data->t46_config_batt_s[0],
			   data->t46_config_batt_s + 1);

			pr_info("[TSP]%s:threshold_batt: %d, probe_num: %d\n",
					__func__, data->tchthr_batt,
					tsp_probe_num);
		}
	} else {
		pr_info("[TSP]%s : mxt in suspend\n", __func__);
	}
	calibrate_chip(data);
}

static uint8_t reportid_to_type(struct mxt_data *data,
						u8 report_id, u8 *instance)
{
	struct report_id_map_t *report_id_map;
	report_id_map = data->rid_map;

	if (report_id <= data->max_report_id) {
		*instance = report_id_map[report_id].instance;
		return report_id_map[report_id].object_type;
	} else
		return 0;
}

static int __devinit mxt_init_touch_driver(struct mxt_data *data)
{
	struct object_t *object_table;
	struct report_id_map_t *report_id_map_t;
	u32 read_crc = 0;
	u32 calc_crc;
	u16 crc_address;
	u16 dummy;
	int i, j;
	u8 id[ID_BLOCK_SIZE];
	int ret;
	u8 type_count = 0;
	u8 tmp;
	int cur_rep_id, start_report_id;

	ret = read_mem(data, 0, sizeof(id), id);
	if (ret)
		return ret;

	pr_info("[TSP] family = %#02x, variant = %#02x, version = %#02x\n"
		, id[0], id[1], id[2]);
	pr_info("[TSP] build = %#02x, matrix X,Y size = %d,%d\n"
		, id[3], id[4], id[5]);
	data->family_id = id[0];
	data->tsp_variant = id[1];
	data->tsp_version = id[2];
	data->tsp_build = id[3];
	data->objects_len = id[6];
	data->x_line = 19;
	data->y_line = 11;
/*
	data->x_line = id[4];
	data->y_line = id[5];
*/
/*
 * mxt224 return real channel value.
 * factory application need actual using channel value.
 */

	object_table = kmalloc(data->objects_len * sizeof(*object_table),
				GFP_KERNEL);
	if (!object_table)
		return -ENOMEM;

	ret = read_mem(data, OBJECT_TABLE_START_ADDRESS,
			data->objects_len * sizeof(*object_table),
			(u8 *)object_table);
	if (ret)
		goto err;

	data->max_report_id = 0;

	for (i = 0; i < data->objects_len; i++) {
		object_table[i].i2c_address =
			le16_to_cpu(object_table[i].i2c_address);
		data->max_report_id += object_table[i].num_report_ids *
						(object_table[i].instances + 1);
		tmp = 0;
		if (object_table[i].num_report_ids) {
			tmp = type_count + 1;
			type_count += object_table[i].num_report_ids *
						(object_table[i].instances + 1);
		}
		switch (object_table[i].object_type) {
		case TOUCH_MULTITOUCHSCREEN_T9:
			data->finger_type = tmp;
			pr_info("Finger type = %d\n",
						data->finger_type);
			break;
		case GEN_MESSAGEPROCESSOR_T5:
#if ITDEV
			data->msg_proc_addr = object_table[i].i2c_address;
#endif
			data->msg_object_size = object_table[i].size + 1;
			break;
		}
	}
	if (data->rid_map_alloc) {
		data->rid_map_alloc = false;
		kfree(data->rid_map);
	}
	data->rid_map = \
		kmalloc((sizeof(report_id_map_t) * data->max_report_id + 1),
				GFP_KERNEL);

	if (!data->rid_map) {
		kfree(object_table);
		return -ENOMEM;
	}
	data->rid_map_alloc = true;
	data->rid_map[0].instance = 0;
	data->rid_map[0].object_type = 0;
	cur_rep_id = 1;

	for (i = 0; i < data->objects_len; i++) {
		if (object_table[i].num_report_ids != 0) {
			for (j = 0; j <= object_table[i].instances; j++) {
				for (start_report_id = cur_rep_id;
				     cur_rep_id <
				     (start_report_id +
				      object_table[i].num_report_ids);
				     cur_rep_id++) {
					data->rid_map[cur_rep_id].instance = j;
					data->rid_map[cur_rep_id].object_type =
					    object_table[i].object_type;
				}
			}
		}
	}
	data->objects = object_table;

	/* Verify CRC */
	crc_address = OBJECT_TABLE_START_ADDRESS +
			data->objects_len * OBJECT_TABLE_ELEMENT_SIZE;

#ifdef __BIG_ENDIAN
#error The following code will likely break on a big endian machine
#endif
	ret = read_mem(data, crc_address, 3, (u8 *)&read_crc);
	if (ret)
		goto err;

	read_crc = le32_to_cpu(read_crc);

	ret = calculate_infoblock_crc(data, &calc_crc);
	if (ret)
		goto err;

	if (read_crc != calc_crc) {
		pr_err("CRC error\n");
		ret = -EFAULT;
		goto err;
	}

	ret = get_object_info(data, GEN_MESSAGEPROCESSOR_T5, &dummy,
					&data->msg_proc);
	if (ret)
		goto err;

	ret = get_object_info(data, GEN_COMMANDPROCESSOR_T6, &dummy,
					&data->cmd_proc);
	if (ret)
		goto err;

#if DEBUG_INFO
	pr_info("maXTouch: %d Objects\n",
			data->objects_len);

	for (i = 0; i < data->objects_len; i++) {
		pr_info("Type:\t\t\t[%d]: %s\n",
			 object_table[i].object_type,
			 object_type_name[object_table[i].object_type]);
		pr_info("\tAddress:\t0x%04X\n",
			 object_table[i].i2c_address);
		pr_info("\tSize:\t\t%d Bytes\n",
			 object_table[i].size);
		pr_info("\tInstances:\t%d\n",
			 object_table[i].instances);
		pr_info("\tReport Id's:\t%d\n",
			 object_table[i].num_report_ids);
	}
#endif

	return 0;

err:
	kfree(object_table);
	return ret;
}


static void report_input_data(struct mxt_data *data)
{
	int i;
	int count = 0;
	int report_count = 0;

	for (i = 0; i < data->num_fingers; i++) {
		if (data->fingers[i].state == MXT_STATE_INACTIVE)
			continue;

		if (data->fingers[i].state == MXT_STATE_RELEASE) {
			input_mt_slot(data->input_dev, i);
			input_mt_report_slot_state(data->input_dev,
					MT_TOOL_FINGER, false);
		} else {
			input_mt_slot(data->input_dev, i);
			input_mt_report_slot_state(data->input_dev,
					MT_TOOL_FINGER, true);
			input_report_abs(data->input_dev, ABS_MT_POSITION_X,
					data->fingers[i].x);
			input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
					data->fingers[i].y);
			input_report_abs(data->input_dev, ABS_MT_PRESSURE,
					data->fingers[i].z);
			input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
					 data->fingers[i].w);
		}
		#ifdef _SUPPORT_SHAPE_TOUCH_
		input_report_abs(data->input_dev, ABS_MT_COMPONENT,
			data->fingers[i].component);
		#endif

		report_count++;

#if SHOW_COORDINATE
		switch (data->fingers[i].state) {
		case MXT_STATE_PRESS:
			if (data->Press_Release_check) {
				data->Press_cnt++;
				data->finger_area = data->fingers[i].z;
			} else
				data->Press_cnt = 0;

			pr_info("[TSP] Press: id[%d],x=%d,y=%d,w=%d, z=%d\n"
				, i, data->fingers[i].x, data->fingers[i].y
				, data->fingers[i].w, data->fingers[i].z);
			pr_info("[TSP] Press cnt =%d\n", data->Press_cnt);
			break;
		case MXT_STATE_RELEASE:
			if (data->Press_Release_check)
				data->Release_cnt++;
			else
				data->Release_cnt = 0;
			pr_info("[TSP] Release: id[%d],mc=%d\n",
				i, data->fingers[i].mcount);
			pr_info("[TSP] Release_cnt =%d\n", data->Release_cnt);
				break;
		default:
			break;
		}
#else
		if (data->fingers[i].state == MXT_STATE_PRESS) {
			if (data->Press_Release_check)
				data->Press_cnt++;
			else
				data->Press_cnt = 0;
			pr_info("[TSP] Press: id[%d],w=%d\n", \
				i, data->fingers[i].w);
			pr_info("[TSP] Press cnt =%d\n", data->Press_cnt);

		} else if (data->fingers[i].state == MXT_STATE_RELEASE) {
			if (data->Press_Release_check)
				data->Release_cnt++;
			else
				data->Release_cnt = 0;
			pr_info("[TSP] Release: id[%d],mc=%d\n", \
					i, data->fingers[i].mcount);
			pr_info("[TSP] Release_cnt =%d\n", data->Release_cnt);
		}
#endif
		if (data->fingers[i].state == MXT_STATE_RELEASE) {
			data->fingers[i].state = MXT_STATE_INACTIVE;
			data->fingers[i].mcount = 0;
		} else {
			data->fingers[i].state = MXT_STATE_MOVE;
			count++;
		}
	}
	if (report_count > 0) {
#if ITDEV
		if (!data->driver_paused)
#endif
			input_sync(data->input_dev);
	}

	if (count)
		touch_is_pressed = 1;
	else
		touch_is_pressed = 0;
#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
	set_dvfs_lock(data, !!touch_is_pressed);
#endif
#endif
	data->finger_mask = 0;
}

static u16 mxt_dist_check(struct mxt_data *data)
{
	int i;
	u16 dist_sum = 0;

	for (i = 0; i <= data->max_id; i++) {
		if (data->distance[i] < 3)
			dist_sum++;
		else
			dist_sum = 0;
	}

	for (i = data->max_id + 1; i < MAX_USING_FINGER_NUM; i++)
		data->distance[i] = 0;

	return dist_sum;
}

static void mxt_tch_atch_area_check(struct mxt_data *data,
		int tch_area, int atch_area, int touch_area)
{
	u16 dist_sum = 0;
	unsigned char touch_num;

	touch_num = data->Report_touch_number;
	if (tch_area) {
		/* First Touch After Calibration */
		if (data->pdata->check_timer == 0) {
			data->coin_check = 0;
			mxt_t61_timer_set(data,
					MXT_T61_TIMER_ONESHOT,
					MXT_T61_TIMER_CMD_START,
					1000);
			data->pdata->check_timer = 1;
		}
	}

	if ((tch_area == 0) && (atch_area > 0)) {
		pr_info("[TSP] T57_Abnormal Status\n");
		calibrate_chip(data);
		return;
	}
	dist_sum = mxt_dist_check(data);
	if (touch_num > 1 && tch_area <= 45) {
		if (touch_num == 2) {
			if (tch_area < atch_area-3) {
				pr_info("[TSP] T57_Two touch  Cal_Bad : tch area < atch_area-3 !!!\n");
				calibrate_chip(data);
			} else if (tch_area <= atch_area-2) {
				data->twotouch_check_1st++;
				if (data->twotouch_check_1st > 2) {
					pr_info("[TSP] T57_Two touch Cal_Bad : twotouch_check tch area <= atch_area-2 !!!\n");
					calibrate_chip(data);
					data->twotouch_check_1st = 0;
				}
			} else if (tch_area <= atch_area-1) {
				data->twotouch_check_2nd++;
				if (data->twotouch_check_2nd > 4) {
					pr_info("[TSP] T57_Two touch Cal_Bad : twotouch_check tch area <= atch_area-1 !!!\n");
					calibrate_chip(data);
					data->twotouch_check_2nd = 0;
				}

			}
}
		else if (tch_area <= (touch_num * 5 + 2)) {
			if (!data->coin_check) {
				if (dist_sum == (data->max_id + 1)) {
					if (touch_area < T_AREA_LOW_MT) {
						if (data->t_area_l_cnt >= 7) {
								pr_info("[TSP] T57_Multi touch Cal maybe bad contion : Set autocal = 5\n");
							mxt_t8_cal_set(data, 5);
							data->coin_check = 1;
							data->t_area_l_cnt = 0;
						} else {
							data->t_area_l_cnt++;
						}

						data->t_area_cnt = 0;
					} else {
						data->t_area_cnt = 0;
						data->t_area_l_cnt = 0;
					}
				} else {
						pr_info("[TSP] T57_Multi touch Cal maybe bad, but distance failed\n");
				}
			}
		} else {
			if (tch_area < atch_area-2) {
				pr_info("[TSP] T57_Multi touch  Cal_Bad : tch area < atch_area-2 !!!\n");
				calibrate_chip(data);
			}
		}
	} else if (touch_num  > 1 && tch_area > 48) {
			if (tch_area > atch_area) {
				pr_info("[TSP] T57_Multi touch  Cal_Bad : tch area > atch_area !!!\n");
				calibrate_chip(data);
			} else {
				pr_info("[TSP] T57_Multi touch Cal maybe good contion : tch area <= atch_area\n");
			}

	} else if (touch_num == 1) {
		/* single Touch */
		dist_sum = data->distance[0];
		if ((tch_area < 9) &&
			(atch_area <= 1)) {
			if (!data->coin_check) {
				if (data->distance[0] < 3) {
					if (touch_area < T_AREA_LOW_ST) {
						if (data->t_area_l_cnt >= 7) {
								pr_info("[TSP] T57_Single Floating metal Wakeup suspection :Set autocal = 5\n");
							mxt_t8_cal_set(data, 5);
							data->coin_check = 1;
							data->t_area_l_cnt = 0;
						} else {
							data->t_area_l_cnt++;
						}
						data->t_area_cnt = 0;

					} else if (touch_area < \
							T_AREA_HIGH_ST) {
						if (data->t_area_cnt >= 7) {
								pr_info("[TSP] T57_Single Floating metal Wakeup suspection :Set autocal = 5\n");
							mxt_t8_cal_set(data, 5);
							data->coin_check = 1;
							data->t_area_cnt = 0;
						} else {
							data->t_area_cnt++;
						}
						data->t_area_l_cnt = 0;
					} else {
						data->t_area_cnt = 0;
						data->t_area_l_cnt = 0;
					}
				}
			}
		} else if (tch_area > 25) {
			pr_info("[TSP] tch_area > 25\n");
			calibrate_chip(data);
		}
	}
}

static irqreturn_t mxt_irq_thread(int irq, void *ptr)
{
	struct mxt_data *data = ptr;
	int id;
	u8 msg[data->msg_object_size];
	u8 touch_message_flag = 0;
	int i;
	u8 object_type, instance;
	u16 touch_area_T57 = 0;
	u8 distsum_buff = 0;

#if CHECK_ANTITOUCH
	u16 tch_area = 0, atch_area = 0;
 #endif
	do {
		touch_message_flag = 0;
		if (read_mem(data, data->msg_proc, sizeof(msg), msg))
			return IRQ_HANDLED;
#if ITDEV

		if (data->debug_enabled)
			print_hex_dump(KERN_INFO, "MXT MSG:",
					DUMP_PREFIX_NONE,
					16, 1, msg,
					sizeof(msg), false);
#endif
		object_type = reportid_to_type(data, msg[0] , &instance);

		if (object_type == GEN_COMMANDPROCESSOR_T6) {
			if (msg[1] == 0x00) /* normal mode */
				pr_info("[TSP] normal mode\n");
				if (data->cal_busy)
					data->cal_busy = 0;
			if ((msg[1]&0x04) == 0x04) /* I2C checksum error */
				pr_info("I2C checksum error\n");
			if ((msg[1]&0x08) == 0x08) /* config error */
				pr_info("config error\n");
			if ((msg[1]&0x10) == 0x10) {
				/* calibration */
				pr_info("[TSP] calibration is on going !!\n");
#if CHECK_ANTITOUCH
				/* After Calibration */
				data->coin_check = 0;
				mxt_t8_cal_set(data, 0);
				data->pdata->check_antitouch = 1;
				mxt_t61_timer_set(data,
						MXT_T61_TIMER_ONESHOT,
						MXT_T61_TIMER_CMD_STOP, 0);
				data->pdata->check_timer = 0;
				data->pdata->check_calgood = 0;
				data->cal_busy = 1;

				data->twotouch_check_1st = 0;
				data->twotouch_check_2nd = 0;
				data->finger_area = 0;
#endif
			}
			if ((msg[1]&0x20) == 0x20) /* signal error */
				pr_info("signal error\n");
			if ((msg[1]&0x40) == 0x40) /* overflow */
				pr_info("overflow detected\n");
			if ((msg[1]&0x80) == 0x80) { /* reset */
				pr_info("[TSP] reset is ongoing\n");
				data->Press_Release_check = 1;
				data->Press_cnt = 0;
				data->Release_cnt = 0;
			}
		}

		if (object_type == PROCI_TOUCHSUPPRESSION_T42) {
			if ((msg[1] & 0x01) == 0x00) {
				/* Palm release */
				pr_info("palm touch released\n");
				touch_is_pressed = 0;

			} else if ((msg[1] & 0x01) == 0x01) {
				/* Palm Press */
				pr_info("palm touch detected\n");
				touch_is_pressed = 1;
				touch_message_flag = 1;
			}
		}

		if (object_type == PROCI_EXTRATOUCHSCREENDATA_T57) {
			touch_area_T57 = msg[1] | (msg[2] << 8);
#if CHECK_ANTITOUCH
			tch_area = msg[3] | (msg[4] << 8);
			atch_area = msg[5] | (msg[6] << 8);

			data->Report_touch_number = 0;
			for (i = 0; i < data->num_fingers; i++) {
				if ((data->fingers[i].state != \
					MXT_STATE_INACTIVE) &&
					(data->fingers[i].state != \
					MXT_STATE_RELEASE))
					data->Report_touch_number++;
			}

			if (data->pdata->check_antitouch)
				mxt_tch_atch_area_check(data,
					tch_area, atch_area, touch_area_T57);
			if ((data->Report_touch_number >= 5) && \
				(touch_area_T57 <\
					(data->Report_touch_number * 2) + 2)) {
				if (data->palm_cnt >= 5) {
					data->palm_cnt = 0;
					pr_info("[TSP] Palm Calibration_T57\n");
					calibrate_chip(data);
				} else {
					data->palm_cnt++;
				}
			} else {
				data->palm_cnt = 0;
			}

			if (data->pdata->check_calgood == 1) {
				distsum_buff = mxt_dist_check(data);
				if (data->Report_touch_number >= 2) {
					if ((tch_area-touch_area_T57 > 10) \
						&& (atch_area > 5)) {
						pr_info("[TSP] Cal Not Good1\n");
						calibrate_chip(data);
					}
				}
				if ((atch_area - tch_area) > 15) {
					if (tch_area < 25) {
						pr_info("[TSP] Cal Not Good atch:%d  tch:%d\n",
							atch_area, tch_area);
						calibrate_chip(data);
					}
				}
				if ((tch_area - atch_area) > 48) {
					pr_info("[TSP] Cal Not Good 4 - atch:%d tch:%d\n",
							atch_area, tch_area);
					calibrate_chip(data);
				}
			}
#endif	/* CHECK_ANTITOUCH */
		}

#if CHECK_ANTITOUCH
		if (object_type == SPT_TIMER_T61) {
			if ((msg[1] & 0xa0) == 0xa0) {
				if (data->pdata->check_calgood == 1) {
					if (data->Press_cnt == \
							data->Release_cnt) {
						pr_info("[TSP] SPT_TIMER_T61 Stop 3sec\n");
						data->pdata->check_calgood = 0;
						data->Press_Release_check = 0;
						write_config(data,
						data->t8_config_normal_s[0],
						data->t8_config_normal_s + 1);
					} else {
						if (data->finger_area < 35) {
							calibrate_chip(data);
							pr_info("[TSP] Press_cnt Fail cal\n");
						} else
							pr_info("[TSP] Press_cnt Fail but finger_area < 35 not cal\n");
					}

				}
				if (data->pdata->check_antitouch) {
					if (data->pdata->check_autocal == 1) {
						pr_info("[TSP] Auto cal is on going - 1sec time restart\n");
						data->pdata->check_timer = 0;
						data->coin_check = 0;
						data->palm_cnt = 0;
						mxt_t8_cal_set(data, 0);
						mxt_t61_timer_set(data,
							MXT_T61_TIMER_ONESHOT,
							MXT_T61_TIMER_CMD_START,
							1000);
					} else {
					data->pdata->check_antitouch = 0;
						data->pdata->check_timer = 0;
						data->palm_cnt = 0;
						mxt_t8_cal_set(data, 0);
						data->pdata->check_calgood = 1;
						data->coin_check = 0;
						mxt_t61_timer_set(data,
							MXT_T61_TIMER_ONESHOT,
							MXT_T61_TIMER_CMD_START,
							3000);
					}
				}
			}
		}
#endif	/* CHECK_ANTITOUCH */

		if (object_type == TOUCH_MULTITOUCHSCREEN_T9) {
			id = msg[0] - data->finger_type;

			/* If not a touch event, then keep going */
			if (id < 0 || id >= data->num_fingers)
				continue;

			if (data->finger_mask & (1U << id))
				report_input_data(data);

			if (msg[1] & RELEASE_MSG_MASK) {
				data->fingers[id].z = 0;
				data->fingers[id].w = msg[5];
				data->finger_mask |= 1U << id;
				data->fingers[id].state = \
							  MXT_STATE_RELEASE;
				data->tcount[id] = 0;
				data->distance[id] = 0;
			} else if ((msg[1] & DETECT_MSG_MASK) &&
					(msg[1] &
					 (PRESS_MSG_MASK | MOVE_MSG_MASK))) {

				touch_message_flag = 1;
				data->fingers[id].z = msg[6];
				data->fingers[id].w = msg[5];
				if (data->fingers[id].w == 0)
					data->fingers[id].w = 1;

				data->fingers[id].x =
					(((msg[2] << 4) |
					  (msg[4] >> 4))
					 >> data->x_dropbits);

				data->fingers[id].y =
					(((msg[3] << 4)	|
					  (msg[4] & 0xF))
					 >> data->y_dropbits);
#if HIGH_RESOLUTION
				/* high X resolution version*/
				data->fingers[id].x = \
				      (u16)((data->fingers[id].x * 480)\
				      / 4096);
				/* 800 -> 480 */
				data->fingers[id].y = \
				      (u16)((data->fingers[id].y * 800)\
				      / 4096);
				/* 480 -> 800 */
#endif

				data->finger_mask |= 1U << id;

				if (msg[1] & PRESS_MSG_MASK) {
					data->fingers[id].state = \
							  MXT_STATE_PRESS;
					data->fingers[id].mcount = 0;
					mxt_check_coordinate(data, 1, id,
							data->fingers[id].x,
							data->fingers[id].y);

				} else if (msg[1] & MOVE_MSG_MASK) {
					data->fingers[id].mcount += 1;
					mxt_check_coordinate(data, 0, id,
							data->fingers[id].x,
							data->fingers[id].y);
				}

#ifdef _SUPPORT_SHAPE_TOUCH_
				data->fingers[id].component = msg[7];
#endif

			} else if ((msg[1] & SUPPRESS_MSG_MASK)
					&& (data->fingers[id].state != \
					 MXT_STATE_INACTIVE)) {
				data->fingers[id].z = 0;
				data->fingers[id].w = msg[5];
				data->fingers[id].state = MXT_STATE_RELEASE;
				data->finger_mask |= 1U << id;
			} else {
				/* ignore changed amplitude message */
			if (!((msg[1] & DETECT_MSG_MASK) \
				&& (msg[1] & AMPLITUDE_MSG_MASK)))
				continue;
			}
		}
	} while (!gpio_get_value(data->gpio_read_done));

	if (data->finger_mask)
		report_input_data(data);

	return IRQ_HANDLED;
}

static int mxt_internal_suspend(struct mxt_data *data)
{
	int i;
	int count = 0;

	for (i = 0; i < data->num_fingers; i++) {
		if (data->fingers[i].state == MXT_STATE_INACTIVE)
			continue;
		data->fingers[i].z = 0;
		data->fingers[i].state = MXT_STATE_RELEASE;
		count++;
	}
	if (count)
		report_input_data(data);
#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
	set_dvfs_lock(data, 2);
	pr_info("[TSP] dvfs_lock free.\n");
#endif
#endif
	data->power_onoff(0);

	return 0;
}

static int mxt_internal_resume(struct mxt_data *data)
{
	data->power_onoff(1);
	msleep(70);
	mxt_reset(data);
	msleep(30);

	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
#define mxt_suspend	NULL
#define mxt_resume	NULL

static void mxt_early_suspend(struct early_suspend *h)
{
	struct mxt_data *data = container_of(h, struct mxt_data,
								early_suspend);

	data->twotouch_check_1st = 0;
	data->twotouch_check_2nd = 0;

	if (mxt_enabled == 1) {
		pr_info("%s\n", __func__);
		mxt_enabled = 0;
		touch_is_pressed = 0;
		disable_irq(data->client->irq);
		mxt_internal_suspend(data);
	} else
		pr_err("%s. but touch already off\n", __func__);
}

static void mxt_late_resume(struct early_suspend *h)
{
	struct mxt_data *data = container_of(h, struct mxt_data,
								early_suspend);

	data->twotouch_check_1st = 0;
	data->twotouch_check_2nd = 0;

	if (mxt_enabled == 0) {
		pr_info("[TSP] %s\n", __func__);
		mxt_internal_resume(data);
		mxt_enabled = 1;

		if (data->ta_status)
			write_config(data,
				data->t62_config_chrg_s[0],
				data->t62_config_chrg_s + 1);
		else
			write_config(data,
				data->t62_config_batt_s[0],
				data->t62_config_batt_s + 1);

		enable_irq(data->client->irq);
	} else
		pr_err("%s. but touch already on\n", __func__);
}
#else
static int mxt_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mxt_data *data = i2c_get_clientdata(client);

	mxt_enabled = 0;
	touch_is_pressed = 0;

	return mxt_internal_suspend(data);
}

static int mxt_resume(struct device *dev)
{
	int ret = 0;
	bool ta_status = 0;
	struct i2c_client *client = to_i2c_client(dev);
	struct mxt_data *data = i2c_get_clientdata(client);

	ret = mxt_internal_resume(data);

	mxt_enabled = 1;

	if (data->ta_status)
		write_config(data,
			data->t62_config_chrg_s[0],
			data->t62_config_chrg_s + 1);
	else
		write_config(data,
			data->t62_config_batt_s[0],
			data->t62_config_batt_s + 1);
	return ret;
}
#endif

#if SYSFS
static ssize_t mxt_debug_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	g_debug_switch = !g_debug_switch;
	return 0;
}

static ssize_t mxt_object_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	unsigned int object_type;
	unsigned int object_register;
	unsigned int register_value;
	u8 value;
	u8 val;
	int ret;
	u16 address;
	u16 size;
	sscanf(buf, "%u%u%u", &object_type, &object_register, &register_value);
	pr_info("object type T%d", object_type);
	pr_info("object register ->Byte%d\n", object_register);
	pr_info("register value %d\n", register_value);
	ret = get_object_info(data, (u8)object_type, &size, &address);
	if (ret) {
		pr_err("fail to get object_info\n");
		return count;
	}

	size = 1;
	value = (u8)register_value;
	write_mem(data, address+(u16)object_register, size, &value);
	read_mem(data, address+(u16)object_register, (u8)size, &val);

	pr_info("T%d Byte%d is %d\n",
		object_type, object_register, val);
	return count;
}

static ssize_t mxt_object_show(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	unsigned int object_type;
	u8 val;
	int ret;
	u16 address;
	u16 size;
	u16 i;
	sscanf(buf, "%u", &object_type);
	pr_info("object type T%d\n", object_type);
	ret = get_object_info(data, (u8)object_type, &size, &address);
	if (ret) {
		pr_err("fail to get object_info\n");
		return count;
	}
	for (i = 0; i < size; i++) {
		read_mem(data, address+i, 1, &val);
		pr_info("Byte %u --> %u\n", i, val);
	}
	return count;
}

static struct device *sec_touchscreen;
static struct device *mxt_noise_test;
/*
	top_left, top_right, center, bottom_left, bottom_right
*/
static void diagnostic_chip(struct mxt_data *data, u8 mode)
{
	int error;
	u16 t6_address = 0;
	u16 size_one;
	int ret;
	u8 value;
	u16 t37_address = 0;

	ret = get_object_info(data,
		GEN_COMMANDPROCESSOR_T6, &size_one, &t6_address);

	size_one = 1;
	error = write_mem(data, t6_address+5, (u8)size_one, &mode);
	/* QT602240_COMMAND_DIAGNOSTIC, mode); */
	if (error < 0) {
		pr_err("error %s: write_object\n", __func__);
	} else {
		get_object_info(data,
			DEBUG_DIAGNOSTIC_T37, &size_one, &t37_address);
		size_one = 1;
		/* pr_info("diagnostic_chip setting success\n"); */
		read_mem(data, t37_address, (u8)size_one, &value);
		/* pr_info("dianostic_chip mode is %d\n",value); */
	}
}

static uint8_t read_uint16_t(struct mxt_data *data,
					uint16_t address, uint16_t *buf)
{
	uint8_t status;
	uint8_t temp[2];

	status = read_mem(data, address, 2, temp);
	*buf = ((uint16_t)temp[1]<<8) + (uint16_t)temp[0];

	return status;
}

static void read_dbg_data(struct mxt_data *data, uint8_t dbg_mode ,
					uint16_t node, uint16_t *dbg_data)
{
	u8 read_page, read_point;
	uint8_t mode, page;
	u16 size;
	u16 diagnostic_addr = 0;

	if (!mxt_enabled) {
		pr_err("read_dbg_data. mxt_enabled is 0\n");
		return;
	}

	get_object_info(data,
		DEBUG_DIAGNOSTIC_T37, &size, &diagnostic_addr);

	read_page = node / 64;
	node %= 64;
	read_point = (node * 2) + 2;

	/* Page Num Clear */
	diagnostic_chip(data, MXT_CTE_MODE);
	msleep(20);

	do {
		if (read_mem(data, diagnostic_addr, 1, &mode)) {
			pr_info("READ_MEM_FAILED\n");
			return;
		}
	} while (mode != MXT_CTE_MODE);

	diagnostic_chip(data, dbg_mode);
	msleep(20);

	do {
		if (read_mem(data, diagnostic_addr, 1, &mode)) {
			pr_info("READ_MEM_FAILED\n");
			return;
		}
	} while (mode != dbg_mode);

	for (page = 1; page <= read_page; page++) {
		diagnostic_chip(data, MXT_PAGE_UP);
		msleep(20);
		do {
			if (read_mem(data,
				diagnostic_addr + 1, 1, &mode)) {
				pr_info("READ_MEM_FAILED\n");
				return;
			}
		} while (mode != page);
	}

	if (read_uint16_t(data, diagnostic_addr + read_point, dbg_data)) {
		pr_info("READ_MEM_FAILED\n");
		return;
	}
}

static int read_all_data(struct mxt_data *data, uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 max_value = MIN_VALUE, min_value = MAX_VALUE;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	u16 size;
	u8 val;
	bool dual_x_mode = 0;

	ret = get_object_info(data,
			PROCG_NOISESUPPRESSION_T62, &size, &object_address);
	read_mem(data, object_address + 3, 1, &val);
	if (val & 0x10)
		dual_x_mode = 1;

	/* Page Num Clear */
	diagnostic_chip(data, MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(data, dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(data,
			DEBUG_DIAGNOSTIC_T37, &size, &object_address);
	/*jerry no need to leave it */
	msleep(50); /* msleep(20);  */

	for (read_page = 0 ; read_page < 6; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(data,
				object_address + (u16)read_point,
				2, data_buffer);
			mxt_refer_node[num] =
				((u16)data_buffer[1] << 8)
				+ (u16)data_buffer[0];

			if ((mxt_refer_node[num] > MIN_VALUE)
				|| (mxt_refer_node[num] < MAX_VALUE)) {
				state = 1;
				printk(KERN_ERR
					"[TSP] mxt_refer_node[%3d] = %5d\n",
					num, mxt_refer_node[num]);
			}

			if (data_buffer[0] != 0) {
				if (mxt_refer_node[num] != 0) {
					if (mxt_refer_node[num] > max_value)
						max_value = mxt_refer_node[num];
					if (mxt_refer_node[num] < min_value)
						min_value = mxt_refer_node[num];
				}
			}
			if (num > 250)
				goto out;
			num++;
		}
		diagnostic_chip(data, MXT_PAGE_UP);
		msleep(20);
	}

out:
	if ((max_value - min_value) > 4000) {
		printk(KERN_ERR
			"[TSP] diff = %d, max_value = %d, min_value = %d\n",
			(max_value - min_value), max_value, min_value);
		state = 1;
	}

	max_ref = max_value;
	min_ref = min_value;

	return state;
}

static int read_all_delta_data(struct mxt_data *data, uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	int temp;
	u16 size;

	if (!mxt_enabled) {
		pr_err("%s : mxt_enabled is 0\n", __func__);
		return 1;
	}

	/* Page Num Clear */
	diagnostic_chip(data, MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(data, dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(data,
		DEBUG_DIAGNOSTIC_T37, &size, &object_address);
	msleep(50); /* msleep(20);  */


	/* 768/64 */
	for (read_page = 0 ; read_page < 6; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(data,
				object_address+(u16)read_point, 2, data_buffer);
			temp = (int)((uint16_t)data_buffer[1]<<8)
					+ (int)data_buffer[0];
			if (temp & (1 << 15))
				mxt_delta_node[num] = temp - 65535;
			else
				mxt_delta_node[num] = temp;
/*
			mxt_delta_node[num] =
					(int)((uint16_t)data_buffer[1]<<8)
					+ (int)data_buffer[0];
*/
			printk(KERN_ERR
					"[TSP] mxt_delta_node[%3d] = %5d\n",
					num, mxt_delta_node[num]);
			num++;
		}
		diagnostic_chip(data, MXT_PAGE_UP);
		msleep(35);
	}

	return state;
}

int find_channel(struct mxt_data *data, uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	u16 size;
	u16 delta_val = 0;
	u16 max_val = 0;

	if (!mxt_enabled) {
		pr_err("%s : mxt_enabled is 0\n", __func__);
		return 1;
	}

	/* Page Num Clear */
	diagnostic_chip(data, MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(data, dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(data,
		DEBUG_DIAGNOSTIC_T37, &size, &object_address);
	msleep(50); /* msleep(20);  */


	/* 768/64 */
	for (read_page = 0 ; read_page < 12; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(data,
				object_address+(u16)read_point, 2, data_buffer);
				delta_val = ((uint16_t)data_buffer[1]<<8)
					+ (uint16_t)data_buffer[0];

			if (delta_val > 32767)
				delta_val = 65535 - delta_val;
			if (delta_val > max_val) {
				max_val = delta_val;
				state = (read_point - 2)/2 +
					(read_page * 64);
				}
			num++;
		}
		diagnostic_chip(data, MXT_PAGE_UP);
		msleep(35);
	}

	return state;
}

static ssize_t find_channel_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	int status = 0;

	status = find_channel(data, MXT_DELTA_MODE);

	return snprintf(buf, 10, "%u\n", status);
}
#endif

static int mxt_check_bootloader(struct i2c_client *client,
					unsigned int state)
{
	u8 val;
	u8 temp;

recheck:
	if (i2c_master_recv(client, &val, 1) != 1)
		return -EIO;

	if (val & 0x20)	{
		if (i2c_master_recv(client, &temp, 1) != 1)
			return -EIO;

		if (i2c_master_recv(client, &temp, 1) != 1)
			return -EIO;
		val &= ~0x20;
	}

	if ((val & 0xF0) == MXT_APP_CRC_FAIL) {
		pr_info("MXT_APP_CRC_FAIL\n");
		if (i2c_master_recv(client, &val, 1) != 1)
			return -EIO;

		if (val & 0x20) {
			if (i2c_master_recv(client, &temp, 1) != 1)
				return -EIO;

			if (i2c_master_recv(client, &temp, 1) != 1)
				return -EIO;
			val &= ~0x20;
		}
	}

	switch (state) {
	case MXT_WAITING_BOOTLOAD_CMD:
	case MXT_WAITING_FRAME_DATA:
		val &= ~MXT_BOOT_STATUS_MASK;
		break;
	case MXT_FRAME_CRC_PASS:
		if (val == MXT_FRAME_CRC_CHECK)
			goto recheck;
		break;
	default:
		return -EINVAL;
	}

	if (val != state) {
		pr_err("Unvalid bootloader mode state\n");
		return -EINVAL;
	}

	return 0;
}

static int mxt_unlock_bootloader(struct i2c_client *client)
{
	u8 buf[2];

	buf[0] = MXT_UNLOCK_CMD_LSB;
	buf[1] = MXT_UNLOCK_CMD_MSB;

	if (i2c_master_send(client, buf, 2) != 2) {
		pr_err("%s: i2c send failed\n",
			__func__);

		return -EIO;
	}

	return 0;
}

static int mxt_fw_write(struct i2c_client *client,
				const u8 *data, unsigned int frame_size)
{
	if (i2c_master_send(client, data, frame_size) != frame_size) {
		pr_err("%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int mxt_load_fw(struct mxt_data *data, const char *fn)
{
	struct i2c_client *client = data->client;
	unsigned int frame_size;
	unsigned int pos = 0;
	int ret;
	u16 obj_address = 0;
	u16 size_one;
	u8 value;
	unsigned int object_register;
	int check_frame_crc_error = 0;
	int check_wating_frame_data_error = 0;
	const struct firmware *fw = NULL;

	pr_info("mxt_load_fw startl!!!\n");
	ret = request_firmware(&fw, fn, &client->dev);
	if (ret) {
		pr_err("Unable to open firmware %s\n", fn);
		return ret;
	}

	/* Change to the bootloader mode */
	object_register = 0;
	value = (u8)MXT_BOOT_VALUE;
	ret = get_object_info(data,
		GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
	if (ret) {
		pr_err("fail to get object_info\n");
		release_firmware(fw);
		return ret;
	}
	size_one = 1;
	write_mem(data, obj_address+(u16)object_register, (u8)size_one, &value);
	msleep(MXT_SW_RESET_TIME);

	/* Change to slave address of bootloader */
	if (client->addr == MXT_APP_LOW)
		client->addr = MXT_BOOT_LOW;
	else
		client->addr = MXT_BOOT_HIGH;

	ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
	if (ret)
		goto out;

	/* Unlock bootloader */
	mxt_unlock_bootloader(client);

	while (pos < fw->size) {
		ret = mxt_check_bootloader(client,
						MXT_WAITING_FRAME_DATA);
		if (ret) {
			check_wating_frame_data_error++;
			if (check_wating_frame_data_error > 10) {
				pr_err("firm update fail. wating_frame_data err\n");
				goto out;
			} else {
				pr_err("check_wating_frame_data_error = %d, retry\n",
					check_wating_frame_data_error);
				continue;
			}
		}

		frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

		/* We should add 2 at frame size as the the firmware data is not
		* included the CRC bytes.
		*/
		frame_size += 2;

		/* Write one frame to device */
		mxt_fw_write(client, fw->data + pos, frame_size);

		ret = mxt_check_bootloader(client,
						MXT_FRAME_CRC_PASS);
		if (ret) {
			check_frame_crc_error++;
			if (check_frame_crc_error > 10) {
				pr_err("firm update fail. frame_crc err\n");
				goto out;
			} else {
				pr_err("check_frame_crc_error = %d, retry\n",
					check_frame_crc_error);
				continue;
			}
		}

		pos += frame_size;

		pr_info("Updated %d bytes / %zd bytes\n",
			pos, fw->size);

		msleep(20);
	}

out:
	release_firmware(fw);

	/* Change to slave address of application */
	if (client->addr == MXT_BOOT_LOW)
		client->addr = MXT_APP_LOW;
	else
		client->addr = MXT_APP_HIGH;
	return ret;
}

static int mxt_load_fw_bootmode(struct i2c_client *client, const char *fn)
{
	unsigned int frame_size;
	unsigned int pos = 0;
	int ret;

	int check_frame_crc_error = 0;
	int check_wating_frame_data_error = 0;
	const struct firmware *fw = NULL;

	pr_info("mxt_load_fw start!!!\n");

	ret = request_firmware(&fw, fn, &client->dev);
	if (ret) {
		pr_err("Unable to open firmware %s\n", fn);
		return ret;
	}

	/* Unlock bootloader */
	mxt_unlock_bootloader(client);

	while (pos < fw->size) {
		ret = mxt_check_bootloader(client,
						MXT_WAITING_FRAME_DATA);
		if (ret) {
			check_wating_frame_data_error++;
			if (check_wating_frame_data_error > 10) {
				pr_err("firm update fail. wating_frame_data err\n");
				goto out;
			} else {
				pr_err("check_wating_frame_data_error = %d, retry\n",
					check_wating_frame_data_error);
				continue;
			}
		}

		frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

		/* We should add 2 at frame size as the the firmware data is not
		* included the CRC bytes.
		*/
		frame_size += 2;

		/* Write one frame to device */
		mxt_fw_write(client, fw->data + pos, frame_size);

		ret = mxt_check_bootloader(client,
						MXT_FRAME_CRC_PASS);
		if (ret) {
			check_frame_crc_error++;
			if (check_frame_crc_error > 10) {
				pr_err("firm update fail. frame_crc err\n");
				goto out;
			} else {
				pr_err("check_frame_crc_error = %d, retry\n",
					check_frame_crc_error);
				continue;
			}
		}

		pos += frame_size;

		pr_info("Updated %d bytes / %zd bytes\n",
			pos, fw->size);

		msleep(20);
	}

out:
	release_firmware(fw);

	/* Change to slave address of application */
	if (client->addr == MXT_BOOT_LOW)
		client->addr = MXT_APP_LOW;
	else
		client->addr = MXT_APP_HIGH;
	return ret;
}

#if SYSFS
static ssize_t set_refer0_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_reference = 0;
	read_dbg_data(data, MXT_REFERENCE_MODE, test_node[0], &mxt_reference);
	return snprintf(buf, 10, "%u\n", mxt_reference);
}

static ssize_t set_refer1_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_reference = 0;
	read_dbg_data(data, MXT_REFERENCE_MODE, test_node[1], &mxt_reference);
	return snprintf(buf, 10, "%u\n", mxt_reference);
}

static ssize_t set_refer2_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_reference = 0;
	read_dbg_data(data, MXT_REFERENCE_MODE, test_node[2], &mxt_reference);
	return snprintf(buf, 10, "%u\n", mxt_reference);
}

static ssize_t set_refer3_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_reference = 0;
	read_dbg_data(data, MXT_REFERENCE_MODE, test_node[3], &mxt_reference);
	return snprintf(buf, 10, "%u\n", mxt_reference);
}

static ssize_t set_refer4_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_reference = 0;
	read_dbg_data(data, MXT_REFERENCE_MODE, test_node[4], &mxt_reference);
	return snprintf(buf, 10, "%u\n", mxt_reference);
}

static ssize_t set_delta0_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_delta = 0;
	read_dbg_data(data, MXT_DELTA_MODE, test_node[0], &mxt_delta);
	if (mxt_delta < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return snprintf(buf, 10, "-%u\n", mxt_delta);
	else
		return snprintf(buf, 10, "%u\n", mxt_delta);
}

static ssize_t set_delta1_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_delta = 0;
	read_dbg_data(data, MXT_DELTA_MODE, test_node[1], &mxt_delta);
	if (mxt_delta < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return snprintf(buf, 10, "-%u\n", mxt_delta);
	else
		return snprintf(buf, 10, "%u\n", mxt_delta);
}

static ssize_t set_delta2_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_delta = 0;
	read_dbg_data(data, MXT_DELTA_MODE, test_node[2], &mxt_delta);
	if (mxt_delta < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return snprintf(buf, 10, "-%u\n", mxt_delta);
	else
		return snprintf(buf, 10, "%u\n", mxt_delta);
}

static ssize_t set_delta3_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_delta = 0;
	read_dbg_data(data, MXT_DELTA_MODE, test_node[3], &mxt_delta);
	if (mxt_delta < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return snprintf(buf, 10, "-%u\n", mxt_delta);
	else
		return snprintf(buf, 10, "%u\n", mxt_delta);
}

static ssize_t set_delta4_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	uint16_t mxt_delta = 0;
	read_dbg_data(data, MXT_DELTA_MODE, test_node[4], &mxt_delta);
	if (mxt_delta < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return snprintf(buf, 10, "-%u\n", mxt_delta);
	else
		return snprintf(buf, 10, "%u\n", mxt_delta);
}

static ssize_t set_threshold_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	return snprintf(buf, 10, "%u\n", data->tchthr_batt);
}

static ssize_t set_all_refer_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	int status = 0;

	status = read_all_data(data, MXT_REFERENCE_MODE);

	return snprintf(buf, 20, "%u, %u, %u\n", status, max_ref, min_ref);

}

static int atoi(const char *str)
{
	int result = 0;
	int count = 0;
	if (str == NULL)
		return 0;
	while (str[count] && str[count] >= '0' && str[count] <= '9') {
		result = result * 10 + str[count] - '0';
		++count;
	}
	return result;
}

static ssize_t disp_all_refdata_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 10, "%u\n",  mxt_refer_node[index_reference]);
}

static ssize_t disp_all_refdata_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	index_reference = atoi(buf);
	return size;
}

static ssize_t set_all_delta_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	int status = 0;

	status = read_all_delta_data(data, MXT_DELTA_MODE);

	return snprintf(buf, 10, "%u\n", status);
}

static ssize_t disp_all_deltadata_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (mxt_delta_node[index_delta] < 32767)
		return snprintf(buf, 10, "%u\n", mxt_delta_node[index_delta]);
	else
		mxt_delta_node[index_delta] = \
					65535 - mxt_delta_node[index_delta];

	return snprintf(buf, 10, "-%u\n", mxt_delta_node[index_delta]);
}

static ssize_t disp_all_deltadata_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	index_delta = atoi(buf);
	return size;
}

static ssize_t set_firm_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	u8 id[ID_BLOCK_SIZE];
	u8 value;
	int ret;
	u8 i;

	if (mxt_enabled == 1) {
		disable_irq(data->client->irq);
		for (i = 0; i < 4; i++) {
			ret = read_mem(data, 0, sizeof(id), id);
			if (!ret)
				break;
		}
		enable_irq(data->client->irq);
		if (ret < 0) {
			pr_err("TSP read fail : %s", __func__);
			value = 0;
			return snprintf(buf, 5, "%d\n", value);
		} else {
			pr_info("%s : %#02x\n",
				__func__, id[2]);
			return snprintf(buf, 5, "%#02x\n", id[2]);
		}
	} else {
		pr_err("TSP power off : %s", __func__);
		value = 0;
		return snprintf(buf, 5, "%d\n", value);
	}
}

static ssize_t set_module_off_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	pr_info("%s!!\n", __func__);

	if (*buf != 'S' && *buf != 'F') {
		pr_err("Invalid values\n");
		return -EINVAL;
	}
	if (mxt_enabled == 1) {
		mxt_enabled = 0;
		touch_is_pressed = 0;

		disable_irq(data->client->irq);
		mxt_internal_suspend(data);
	}
	return size;
}

static ssize_t set_module_on_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	bool ta_status = 0;

	pr_info("%s!!\n", __func__);

	if (*buf != 'S' && *buf != 'F') {
		pr_err("Invalid values\n");
		return -EINVAL;
	}

	if (mxt_enabled == 0) {
		mxt_internal_resume(data);
		enable_irq(data->client->irq);

		mxt_enabled = 1;

		if (ta_status)
			write_config(data,
					data->t62_config_chrg_s[0],
					data->t62_config_chrg_s + 1);
		else
			write_config(data,
					data->t62_config_batt_s[0],
					data->t62_config_batt_s + 1);
		calibrate_chip(data);
	}

	return size;
}

static ssize_t key_threshold_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	return snprintf(buf, 5, "%u\n", data->tchthr_batt);
}

static ssize_t key_threshold_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	/*TO DO IT*/
	struct mxt_data *data = dev_get_drvdata(dev);
	unsigned int object_register = 7;
	u8 value;
	u8 val;
	int ret;
	u16 address = 0;
	u16 size_one;
	int num;
	if (sscanf(buf, "%d", &num) == 1) {
		data->tchthr_batt = num;
		pr_info("threshold value %d\n", data->tchthr_batt);
		ret = get_object_info(data,
			TOUCH_MULTITOUCHSCREEN_T9, &size_one, &address);
		size_one = 1;
		value = (u8)data->tchthr_batt;
		write_mem(data,
			address+(u16)object_register, size_one, &value);
		read_mem(data,
			address+(u16)object_register, (u8)size_one, &val);
		pr_err("T9 Byte%d is %d\n", object_register, val);
	}
	return size;
}

static ssize_t set_mxt_firm_status_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 20, "PASS\n");
}

static ssize_t set_mxt_firm_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("phone's version : %#02x,%#02x\n"
		, firmware_latest[0], firmware_latest[1]);
	return snprintf(buf, 10, "%#02x\n", firmware_latest[0]);
}

static ssize_t set_mxt_firm_version_read_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	pr_info("phone's version : %#02x,%#02x\n"
		, data->tsp_version, data->tsp_build);
	return snprintf(buf, 10, "%#02x\n", data->tsp_version);
}

static ssize_t set_mxt_config_version_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	return snprintf(buf, 20, "%s\n", data->config_fw_version);
}

static ssize_t mxt_touchtype_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	char temp[15];

	snprintf(temp, 15, "ATMEL,MXT224S\n");
	strlcat(buf, temp, 15);

	return strnlen(buf, 5);
}

static ssize_t x_line_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u8 data = 24;
	return snprintf(buf, 6, "%d\n", data);
}

static ssize_t y_line_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u8 data = 32;
	return snprintf(buf, 6, "%d\n", data);
}

#if ITDEV
/* Functions for mem_access interface */
struct bin_attribute mem_access_attr;

static int mxt_read_block(struct i2c_client *client,
		   u16 addr,
		   u16 length,
		   u8 *value)
{
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg[2];
	__le16	le_addr;
	struct mxt_data *mxt;

	mxt = i2c_get_clientdata(client);

	if (mxt != NULL) {
		if ((mxt->last_read_addr == addr) &&
			(addr == mxt->msg_proc_addr)) {
			if  (i2c_master_recv(client, value, length) == length) {
#if ITDEV
					print_hex_dump(KERN_INFO, "MXT RX:",
						DUMP_PREFIX_NONE, 16, 1,
						value, length, false);
#endif
				return 0;
			} else
				return -EIO;
		} else {
			mxt->last_read_addr = addr;
		}
	}

	le_addr = cpu_to_le16(addr);
	msg[0].addr  = client->addr;
	msg[0].flags = 0x00;
	msg[0].len   = 2;
	msg[0].buf   = (u8 *) &le_addr;

	msg[1].addr  = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len   = length;
	msg[1].buf   = (u8 *) value;
	if (i2c_transfer(adapter, msg, 2) == 2) {
#if ITDEV
		print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, msg[0].buf, msg[0].len, false);
		print_hex_dump(KERN_INFO, "MXT RX:", DUMP_PREFIX_NONE,
				16, 1, msg[1].buf, msg[1].len, false);
#endif
		return 0;
	} else
		return -EIO;
}

/* Writes a block of bytes (max 256) to given address in mXT chip. */

int mxt_write_block(struct i2c_client *client,
		    u16 addr,
		    u16 length,
		    u8 *value)
{
	int i;
	struct {
		__le16	le_addr;
		u8	data[256];

	} i2c_block_transfer;

	struct mxt_data *mxt;

	if (length > 256)
		return -EINVAL;

	mxt = i2c_get_clientdata(client);
	if (mxt != NULL)
		mxt->last_read_addr = -1;

	for (i = 0; i < length; i++)
		i2c_block_transfer.data[i] = *value++;

	i2c_block_transfer.le_addr = cpu_to_le16(addr);

	i = i2c_master_send(client, (u8 *) &i2c_block_transfer, length + 2);

	if (i == (length + 2)) {
#if ITDEV
		print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, &i2c_block_transfer, length+2, false);
#endif
		return length;
	} else
		return -EIO;
}

static ssize_t mem_access_read(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client;

	pr_info("mem_access_read p=%p off=%lli c=%zi\n", buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0)	{
		client = to_i2c_client(container_of(kobj, struct device, kobj));
		ret = mxt_read_block(client, off, count, buf);
	}

	return ret >= 0 ? count : ret;
}

static ssize_t mem_access_write(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client;

	pr_info("mem_access_write p=%p off=%lli c=%zi\n", buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0) {
		client = to_i2c_client(container_of(kobj, struct device, kobj));
		ret = mxt_write_block(client, off, count, buf);
	}

	return ret >= 0 ? count : 0;
}

static ssize_t pause_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	int count = 0;

	count += snprintf(buf + count, 3, "%d", data->driver_paused);
	count += snprintf(buf + count, 3, "\n");

	return count;
}

static ssize_t pause_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int i;

	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		data->driver_paused = i;

		pr_info("%s\n", i ? "paused" : "unpaused");
	} else {
		pr_info("pause_driver write error\n");
	}

	return count;
}

static ssize_t debug_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int count = 0;

	count += snprintf(buf + count, 3, "%d", data->debug_enabled);
	count += snprintf(buf + count, 3, "\n");

	return count;
}

static ssize_t debug_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int i;

	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		data->debug_enabled = i;

		pr_info("%s\n",
			i ? "debug enabled" : "debug disabled");
	} else {
		pr_info("debug_enabled write error\n");
	}

	return count;
}

static ssize_t command_calibrate_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret;

	ret = calibrate_chip(data);

	return (ret < 0) ? ret : count;
}

static ssize_t command_reset_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client;
	struct mxt_data   *mxt;
	int ret;

	client = to_i2c_client(dev);
	mxt = i2c_get_clientdata(client);

	ret = mxt_reset(mxt);

	return (ret < 0) ? ret : count;
}

static ssize_t command_backup_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client;
	struct mxt_data   *mxt;
	int ret;

	client = to_i2c_client(dev);
	mxt = i2c_get_clientdata(client);

	ret = mxt_backup(mxt);

	return (ret < 0) ? ret : count;
}
#endif

static DEVICE_ATTR(set_refer0, S_IRUGO,
	set_refer0_mode_show, NULL);
static DEVICE_ATTR(set_delta0, S_IRUGO,
	set_delta0_mode_show, NULL);
static DEVICE_ATTR(set_refer1, S_IRUGO,
	set_refer1_mode_show, NULL);
static DEVICE_ATTR(set_delta1, S_IRUGO,
	set_delta1_mode_show, NULL);
static DEVICE_ATTR(set_refer2, S_IRUGO,
	set_refer2_mode_show, NULL);
static DEVICE_ATTR(set_delta2, S_IRUGO,
	set_delta2_mode_show, NULL);
static DEVICE_ATTR(set_refer3, S_IRUGO,
	set_refer3_mode_show, NULL);
static DEVICE_ATTR(set_delta3, S_IRUGO,
	set_delta3_mode_show, NULL);
static DEVICE_ATTR(set_refer4, S_IRUGO,
	set_refer4_mode_show, NULL);
static DEVICE_ATTR(set_delta4, S_IRUGO,
	set_delta4_mode_show, NULL);
static DEVICE_ATTR(set_all_refer, S_IRUGO,
	set_all_refer_mode_show, NULL);
static DEVICE_ATTR(disp_all_refdata, S_IRUGO | S_IWUSR | S_IWGRP,
	disp_all_refdata_show, disp_all_refdata_store);
static DEVICE_ATTR(set_all_delta, S_IRUGO,
	set_all_delta_mode_show, NULL);
static DEVICE_ATTR(disp_all_deltadata, S_IRUGO | S_IWUSR | S_IWGRP,
	disp_all_deltadata_show, disp_all_deltadata_store);
static DEVICE_ATTR(set_firm_version, S_IRUGO | S_IWUSR | S_IWGRP,
	set_firm_version_show, NULL);
static DEVICE_ATTR(set_module_off, S_IRUGO | S_IWUSR | S_IWGRP,
	NULL, set_module_off_store);
static DEVICE_ATTR(set_module_on, S_IRUGO | S_IWUSR | S_IWGRP,
	NULL, set_module_on_store);
static DEVICE_ATTR(mxt_touchtype, S_IRUGO | S_IWUSR | S_IWGRP,
	mxt_touchtype_show, NULL);
static DEVICE_ATTR(set_threshold, S_IRUGO,
	set_threshold_mode_show, NULL);
/* touch threshold return, store */
static DEVICE_ATTR(tsp_threshold, S_IRUGO | S_IWUSR | S_IWGRP,
	key_threshold_show, key_threshold_store);
static DEVICE_ATTR(tsp_firm_update_status, S_IRUGO,
		set_mxt_firm_status_show, NULL);
/* PHONE*/	/* firmware version resturn in phone driver version */
static DEVICE_ATTR(tsp_firm_version_phone, S_IRUGO,
	set_mxt_firm_version_show, NULL);
/*PART*/	/* firmware version resturn in TSP panel version */
static DEVICE_ATTR(tsp_firm_version_panel, S_IRUGO,
	set_mxt_firm_version_read_show, NULL);
static DEVICE_ATTR(tsp_firm_version_config, S_IRUGO,
			set_mxt_config_version_show, NULL);
static DEVICE_ATTR(object_show, S_IWUSR | S_IWGRP, NULL,
	mxt_object_show);
static DEVICE_ATTR(object_write, S_IWUSR | S_IWGRP, NULL,
	mxt_object_setting);
static DEVICE_ATTR(dbg_switch, S_IWUSR | S_IWGRP, NULL,
	mxt_debug_setting);
static DEVICE_ATTR(find_delta_channel, S_IRUGO,
	find_channel_show, NULL);
static DEVICE_ATTR(x_line, S_IRUGO,
	x_line_show, NULL);
static DEVICE_ATTR(y_line, S_IRUGO,
	y_line_show, NULL);
#if ITDEV
/* Sysfs files for libmaxtouch interface */
static DEVICE_ATTR(pause_driver, 0664,
	pause_show, pause_store);
static DEVICE_ATTR(debug_enable, 0664,
	debug_enable_show, debug_enable_store);
static DEVICE_ATTR(command_calibrate, 0664,
	NULL, command_calibrate_store);
static DEVICE_ATTR(command_reset, 0664,
	NULL, command_reset_store);
static DEVICE_ATTR(command_backup, 0664,
	NULL, command_backup_store);

static struct attribute *libmaxtouch_attributes[] = {
	&dev_attr_pause_driver.attr,
	&dev_attr_debug_enable.attr,
	&dev_attr_command_calibrate.attr,
	&dev_attr_command_reset.attr,
	&dev_attr_command_backup.attr,
	NULL,
};

static struct attribute_group libmaxtouch_attr_group = {
	.attrs = libmaxtouch_attributes,
};
#endif

static struct attribute *mxt_attrs[] = {
	&dev_attr_object_show.attr,
	&dev_attr_object_write.attr,
	&dev_attr_dbg_switch.attr,
	NULL
};

static const struct attribute_group mxt_attr_group = {
	.attrs = mxt_attrs,
};

#endif
static int __devinit mxt_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct mxt224s_platform_data *pdata = client->dev.platform_data;
	struct mxt_data *data;
	struct input_dev *input_dev;
	int ret;
	int i;
#ifdef SEC_TSP_FACTORY_TEST
	struct device *fac_dev_ts;
#endif

	u16 size;
	u16 obj_address = 0;
	u8 **tsp_config;

	pr_info("%s +++\n", __func__);

	touch_is_pressed = 0;
	tsp_probe_num = 0;
	if (!pdata) {
		pr_err("missing platform data\n");
		return -ENODEV;
	}

	if (pdata->max_finger_touches <= 0)
		return -EINVAL;

	data = kzalloc(sizeof(*data) + pdata->max_finger_touches *
					sizeof(*data->fingers), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->pdata = pdata;
	data->num_fingers = pdata->max_finger_touches;
	data->power_onoff = pdata->power_onoff;
	data->pdata->request_gpio();
	/* TSP IC power on at board file */

	data->register_cb = pdata->register_cb;
	data->callbacks.inform_charger = mxt_ta_cb;
	if (data->register_cb)
		data->register_cb(&data->callbacks);

/*(FOR_BRINGUP) */
	data->config_fw_version = pdata->config_fw_version;

	data->client = client;
	i2c_set_clientdata(client, data);

	input_dev = input_allocate_device();
	if (!input_dev) {
		ret = -ENOMEM;
		pr_err("input device allocation failed\n");
		goto err_alloc_dev;
	}
	data->input_dev = input_dev;
	input_set_drvdata(input_dev, data);
	input_dev->name = "sec_touchscreen";

	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(MT_TOOL_FINGER, input_dev->keybit);
	set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

	input_mt_init_slots(input_dev, MAX_USING_FINGER_NUM);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X, pdata->min_x,
			     pdata->max_x, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, pdata->min_y,
			     pdata->max_y, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, pdata->min_z,
			     pdata->max_z, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, pdata->min_w,
			     pdata->max_w, 0, 0);

#ifdef _SUPPORT_SHAPE_TOUCH_
	input_set_abs_params(input_dev, ABS_MT_COMPONENT, 0, 255, 0, 0);
#endif
	ret = input_register_device(input_dev);
	if (ret) {
		input_free_device(input_dev);
		goto err_reg_dev;
	}

	data->gpio_read_done = pdata->gpio_read_done;
#ifdef CONFIG_SEC_DVFS
#if TOUCH_BOOSTER
	mutex_init(&data->dvfs_lock);
	INIT_DELAYED_WORK(&data->work_dvfs_off, set_dvfs_off);
	INIT_DELAYED_WORK(&data->work_dvfs_chg, change_dvfs_lock);
	data->dvfs_lock_status = false;
#endif
#endif

	if (client->addr == MXT_APP_LOW)
		client->addr = MXT_BOOT_LOW;
	else
		client->addr = MXT_BOOT_HIGH;

	ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
	if (ret >= 0) {
		pr_info("boot mode. firm update excute\n");
		mxt_load_fw_bootmode(client, MXT_FW_NAME);
		msleep(MXT_SW_RESET_TIME);
	} else {
		if (client->addr == MXT_BOOT_LOW)
			client->addr = MXT_APP_LOW;
		else
			client->addr = MXT_APP_HIGH;
	}

	ret = mxt_init_touch_driver(data);

/*!(FOR_BRINGUP)*/
	if (ret) {
		pr_err("[TSP] chip initialization failed\n");
		goto err_init_drv;
	}

	/* tsp_family_id - 0x82 : MXT336S series */
	if (data->family_id == 0x82) {
		tsp_config = (u8 **)data->pdata->config;

	data->t8_config_normal_s = pdata->t8_config_normal_s;
	data->t62_config_batt_s = pdata->t62_config_batt_s;
	data->t62_config_chrg_s = pdata->t62_config_chrg_s;
	data->t46_config_batt_s = pdata->t46_config_batt_s;
	data->t46_config_chrg_s = pdata->t46_config_chrg_s;
	data->tchthr_batt = pdata->tchthr_batt;
	data->tchthr_charging = pdata->tchthr_charging;

#if !(FOR_BRINGUP)
		data->t62_config_batt = pdata->t62_config_batt;
		data->t62_config_chrg = pdata->t62_config_chrg;
		data->tchthr_batt = pdata->tchthr_batt;
		data->tchthr_charging = pdata->tchthr_charging;
		data->calcfg_batt = pdata->calcfg_batt;
		data->calcfg_charging = pdata->calcfg_charging;
#endif
#if UPDATE_ON_PROBE
	if (data->tsp_version < firmware_latest[0]
			|| (data->tsp_version == firmware_latest[0]
			&& data->tsp_build != firmware_latest[1])) {
		pr_info("force firmware update\n");
		ret = mxt_load_fw(data, MXT_FW_NAME);
		if (ret != 1) {
			msleep(MXT_SW_RESET_TIME);
			mxt_init_touch_driver(data);
		}
	}
#endif

	} else {
		pr_err("ERROR : There is no valid TSP ID\n");
		goto err_config;
	}

	get_object_info(data, SPT_USERDATA_T38, &size, &obj_address);
	read_mem(data, obj_address + 0, 1, &data->disable_config_write);


	for (i = 0; tsp_config[i][0] != RESERVED_T255; i++) {
		ret = init_write_config(data, tsp_config[i][0],
							tsp_config[i] + 1);

		if (tsp_config[i][0] == TOUCH_MULTITOUCHSCREEN_T9) {
			/* Are x and y inverted? */
			if (tsp_config[i][10] & 0x1) {
				data->x_dropbits =
					(!(tsp_config[i][22] & 0xC)) << 1;
				data->y_dropbits =
					(!(tsp_config[i][20] & 0xC)) << 1;
			} else {
				data->x_dropbits =
					(!(tsp_config[i][20] & 0xC)) << 1;
				data->y_dropbits =
					(!(tsp_config[i][22] & 0xC)) << 1;
			}
		}
	}
	ret = mxt_backup(data);
	if (ret) {
		tsp_probe_num = 8;
		printk(KERN_ERR "%s: probe num = %d\n",
				__func__, tsp_probe_num);
	}

	/* reset the touch IC. */
	ret = mxt_reset(data);
	if (ret) {
		tsp_probe_num = 9;
		printk(KERN_ERR "%s: probe num = %d\n",
				__func__, tsp_probe_num);
	}

	msleep(MXT_SW_RESET_TIME);

	for (i = 0; i < data->num_fingers; i++)
		data->fingers[i].state = MXT_STATE_INACTIVE;

	ret = request_threaded_irq(client->irq, NULL, mxt_irq_thread,
		IRQF_TRIGGER_LOW | IRQF_ONESHOT, "mxt_ts", data);

	if (ret < 0)
		goto err_irq;

#if SYSFS
	ret = sysfs_create_group(&client->dev.kobj, &mxt_attr_group);
	if (ret)
		pr_err("sysfs_create_group()is falled\n");


#if ITDEV
	ret = sysfs_create_group(&client->dev.kobj, &libmaxtouch_attr_group);
	if (ret) {
		pr_err("Failed to create libmaxtouch sysfs group\n");
		goto err_irq;
	}

	sysfs_bin_attr_init(&mem_access_attr);
	mem_access_attr.attr.name = "mem_access";
	mem_access_attr.attr.mode = 0664;
	mem_access_attr.read = mem_access_read;
	mem_access_attr.write = mem_access_write;
	mem_access_attr.size = 65535;

	if (sysfs_create_bin_file(&client->dev.kobj, &mem_access_attr) < 0) {
		pr_err("Failed to create device file(%s)!\n",
			mem_access_attr.attr.name);
		goto err_irq;
	}
#endif

	sec_touchscreen = device_create(sec_class,
		NULL, 0, NULL, "sec_touchscreen");
	dev_set_drvdata(sec_touchscreen, data);

	if (IS_ERR(sec_touchscreen))
		pr_err("Failed to create device(sec_touchscreen)!\n");

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_threshold) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_threshold.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_update_status) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_update_status.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_phone) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_phone.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_panel) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_panel.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_config) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_config.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_mxt_touchtype) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_mxt_touchtype.attr.name);

	mxt_noise_test = device_create(sec_class,
		NULL, 0, NULL, "tsp_noise_test");
	dev_set_drvdata(mxt_noise_test, data);

	if (IS_ERR(mxt_noise_test))
		pr_err("Failed to create device(mxt_noise_test)!\n");

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer0) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer0.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta0) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta0.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer1) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer1.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta1) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta1.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer2) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer2.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta2) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta2.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer3) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer3.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta3) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta3.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer4) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer4.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta4) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta4.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_all_refer) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_all_refer.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_disp_all_refdata) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_disp_all_refdata.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_all_delta) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_all_delta.attr.name);

	if (device_create_file(mxt_noise_test,
				&dev_attr_disp_all_deltadata) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_disp_all_deltadata.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_threshold) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_threshold.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_firm_version) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_firm_version.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_module_off) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_module_off.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_module_on) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_module_on.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_x_line) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_x_line.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_y_line) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_y_line.attr.name);

	if (device_create_file(mxt_noise_test,
				&dev_attr_find_delta_channel) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_find_delta_channel.attr.name);
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 1;
	data->early_suspend.suspend = mxt_early_suspend;
	data->early_suspend.resume = mxt_late_resume;
	register_early_suspend(&data->early_suspend);
#endif
	mxt_enabled = 1;

#if ITDEV
	data->driver_paused = 0;
	data->debug_enabled = 0;
#endif

#ifdef SEC_TSP_FACTORY_TEST
	INIT_LIST_HEAD(&data->cmd_list_head);
	for (i = 0; i < ARRAY_SIZE(tsp_cmds); i++)
		list_add_tail(&tsp_cmds[i].list, &data->cmd_list_head);

	mutex_init(&data->cmd_lock);
	data->cmd_is_running = false;

	fac_dev_ts = device_create(sec_class,
			NULL, 0, data, "tsp");
	if (IS_ERR(fac_dev_ts))
		dev_err(&client->dev, "Failed to create device for the sysfs\n");

	ret = sysfs_create_group(&fac_dev_ts->kobj,
			       &sec_touch_factory_attr_group);
	if (ret)
		dev_err(&client->dev, "Failed to create sysfs group\n");
#endif


	printk(KERN_ERR "%s : TSP probe done.\n", __func__);
	return 0;

err_irq:
err_config:
	kfree(data->objects);
err_init_drv:
	gpio_free(data->gpio_read_done);
err_reg_dev:
err_alloc_dev:
	kfree(data);
	printk(KERN_ERR "%s : TSP probe failed.\n", __func__);
	return ret;
}

static int __devexit mxt_remove(struct i2c_client *client)
{
	struct mxt_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&data->early_suspend);
#endif
	free_irq(client->irq, data);
	kfree(data->objects);
	gpio_free(data->gpio_read_done);
	data->power_onoff(0);
	input_unregister_device(data->input_dev);
	kfree(data);

	return 0;
}

static struct i2c_device_id mxt_idtable[] = {
	{MXT224S_DEV_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, mxt_idtable);

static const struct dev_pm_ops mxt_pm_ops = {
	.suspend = mxt_suspend,
	.resume = mxt_resume,
};

static struct i2c_driver mxt_i2c_driver = {
	.id_table = mxt_idtable,
	.probe = mxt_probe,
	.remove = __devexit_p(mxt_remove),
	.driver = {
		.owner	= THIS_MODULE,
		.name	= MXT224S_DEV_NAME,
		.pm	= &mxt_pm_ops,
	},
};

static int __init mxt_init(void)
{
	return i2c_add_driver(&mxt_i2c_driver);
}

static void __exit mxt_exit(void)
{
	i2c_del_driver(&mxt_i2c_driver);
}
module_init(mxt_init);
module_exit(mxt_exit);

MODULE_DESCRIPTION("Atmel MaXTouch driver");
MODULE_AUTHOR("ki_won.kim<ki_won.kim@samsung.com>");
MODULE_LICENSE("GPL");
