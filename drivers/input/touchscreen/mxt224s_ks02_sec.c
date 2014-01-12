/*
 *  drivers/input/touchscreen/mxt1664s_sec.c
 *
 * Copyright (C) 2012 Samsung Electronics, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/firmware.h>
#include <linux/fs.h>
#include <linux/i2c/mxt224s_ks02.h>
#include "mxt224s_ks02_dev.h"

#define mxt224s_ref_offset 16384

#if TSP_SEC_SYSFS
static void set_default_result(struct mxt_data_sysfs *data)
{
	char delim = ':';

	memset(data->cmd_result, 0x00, ARRAY_SIZE(data->cmd_result));
	memcpy(data->cmd_result, data->cmd, strlen(data->cmd));
	strncat(data->cmd_result, &delim, 1);
}

static void set_cmd_result(struct mxt_data_sysfs *data, char *buff, int len)
{
	strncat(data->cmd_result, buff, len);
}

static void not_support_cmd(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};

	set_default_result(sysfs_data);
	sprintf(buff, "%s", "NA");
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_NOT_APPLICABLE;
	dev_info(&client->dev, "%s: \"%s(%d)\"\n", __func__,
				buff, strnlen(buff, sizeof(buff)));
}
/*
static bool check_xy_range(struct mxt_data *data, u16 node)
{
	u8 x_line = node / MATRIX_X;
	u8 y_line = node % MATRIX_Y;
	return (y_line < MATRIX_Y) ?
		(x_line < MATRIX_X) : false;
}*/

/* +  Vendor specific helper functions */
static int mxt_xy_to_node(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};
	int node;

	/* cmd_param[0][1] : [x][y] */
	if (sysfs_data->cmd_param[0] < 0 ||
		sysfs_data->cmd_param[0] >= MATRIX_X ||
		sysfs_data->cmd_param[1] < 0 ||
		sysfs_data->cmd_param[1] >= MATRIX_Y) {
		snprintf(buff, sizeof(buff) , "%s", "NG");
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
		sysfs_data->cmd_state = CMD_STATUS_FAIL;

		dev_info(&client->dev, "%s: parameter error: %u,%u\n",
				__func__, sysfs_data->cmd_param[0],
				sysfs_data->cmd_param[1]);
		return -EINVAL;
	}

	/*
	* maybe need to consider orient.
	*   --> y number
	*  |(0,0) (0,1)
	*  |(1,0) (1,1)
	*  v
	*  x number
	*/
	node = sysfs_data->cmd_param[0] * MATRIX_Y
			+ sysfs_data->cmd_param[1];

	dev_info(&client->dev, "%s: node = %d\n", __func__, node);
	return node;
}

static void mxt_node_to_xy(struct mxt_data *data, u16 *x, u16 *y)
{
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	*x = sysfs_data->delta_max_node / MATRIX_Y;
	*y = sysfs_data->delta_max_node % MATRIX_Y;

	dev_info(&client->dev, "%s: node[%d] is X,Y=%d,%d\n", __func__,
		sysfs_data->delta_max_node, *x, *y);
}


static int mxt_read_all_diagnostic_data(struct mxt_data *data, u8 dbg_mode)
{
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	int ret = 1;

	/* calculate end page of IC */
	sysfs_data->ref_min_data = MAX_VALUE;
	sysfs_data->ref_max_data = MIN_VALUE;
	sysfs_data->delta_max_data = 0;
	sysfs_data->delta_max_node = 0;

	if (dbg_mode == MXT_DIAG_REFERENCE_MODE) {
		ret = read_all_data(dbg_mode);

	} else if (dbg_mode == MXT_DIAG_DELTA_MODE) {
		ret = read_all_delta_data(dbg_mode);
	}


	return ret;
}

/*
* find the x,y position to use maximum delta.
* it is diffult to map the orientation and caculate the node number
* because layout is always different according to device
*/
static void find_delta_node(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	char buff[16] = {0};
	u16 x, y;
	int ret;

	set_default_result(sysfs_data);

	/* read all delta to get the maximum delta value */
	ret = mxt_read_all_diagnostic_data(data,
			MXT_DIAG_DELTA_MODE);
	if (ret) {
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
	} else {
		mxt_node_to_xy(data, &x, &y);
		snprintf(buff, sizeof(buff), "%d,%d", x, y);
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));

		sysfs_data->cmd_state = CMD_STATUS_OK;
	}
}

/* -  Vendor specific helper functions */

/* + function realted samsung factory test */
static void fw_update(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	struct file *filp = NULL;
	const struct firmware *fw = NULL;
	const u8 *fw_data = NULL;
	long fw_size = 0;
	mm_segment_t old_fs = {0};
	char *fw_path;
	int ret;

	set_default_result(sysfs_data);

	fw_path = kzalloc(MXT_MAX_FW_PATH, GFP_KERNEL);
	if (fw_path == NULL) {
		dev_err(&client->dev, "failed to allocate firmware path.\n");
		goto out;
	}

	switch (sysfs_data->cmd_param[0]) {
	case MXT_FW_FROM_UMS:
		snprintf(fw_path, MXT_MAX_FW_PATH, "%s", MXT_FW_NAME);

		old_fs = get_fs();
		set_fs(get_ds());

		filp = filp_open(fw_path, O_RDONLY, 0);
		if (IS_ERR(filp)) {
			dev_err(&client->dev, "could not open firmware: %s,%d\n",
				fw_path, (s32)filp);
			set_fs(old_fs);
			goto err_open;
		}

		fw_size = filp->f_path.dentry->d_inode->i_size;

		fw_data = kzalloc((size_t)fw_size, GFP_KERNEL);
		if (!fw_data) {
			dev_err(&client->dev, "fail to alloc buffer for fw\n");
			filp_close(filp, current->files);
			set_fs(old_fs);
			goto err_alloc;
		}
		ret = vfs_read(filp, (char __user *)fw_data,
				fw_size, &filp->f_pos);
		if (ret != fw_size) {
			dev_err(&client->dev, "fail to read file %s (ret = %d)\n",
					fw_path, ret);
			goto err_read;
		}
		filp_close(filp, current->files);
		set_fs(old_fs);
	break;

	case MXT_FW_FROM_BUILT_IN:
	case MXT_FW_FROM_REQ_FW:
		snprintf(fw_path, MXT_MAX_FW_PATH, "%s", MXT_FW_NAME);
		ret = request_firmware(&fw, fw_path, &client->dev);
		if (ret) {
			dev_err(&client->dev,
				"could not request firmware %s\n", fw_path);
			goto err_open;
		}

		fw_size = fw->size;
		fw_data = kzalloc(fw_size, GFP_KERNEL);
		if (!fw_data) {
			dev_err(&client->dev, "fail to alloc buffer for fw\n");
			goto err_alloc;
		}
		memcpy((void *)fw_data, fw->data, fw_size);
		release_firmware(fw);
	break;

	default:
		dev_err(&client->dev, "invalid fw file type!!\n");
		goto not_support;
	}

	kfree(fw_path);
	disable_irq(data->client->irq);

	ret = set_mxt_firm_update_store(data, fw_data, fw_size);
	kfree(fw_data);
	enable_irq(data->client->irq);

	if (ret)
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
	else
		sysfs_data->cmd_state = CMD_STATUS_OK;

	return;

err_read:
	kfree(fw_data);
err_alloc:
	release_firmware(fw);
err_open:
not_support:
	kfree(fw_path);
out:
	sysfs_data->cmd_state = CMD_STATUS_FAIL;
	return;
}

static void get_fw_ver_bin(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[40] = {0};
	int ver = firmware_latest[0];
	set_default_result(sysfs_data);
	snprintf(buff, sizeof(buff), "AT224S%02x", ver);

	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_info(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_fw_ver_ic(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[40] = {0};
	int ver = data->tsp_version;

	set_default_result(sysfs_data);
	snprintf(buff, sizeof(buff), "AT224S%02x", ver);

	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_info(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_config_ver(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	char buff[40] = {0};

	set_default_result(sysfs_data);
	snprintf(buff, sizeof(buff), "%s", data->pdata->config_fw_version);

	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_info(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_threshold(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};
	u8 threshold;

	set_default_result(sysfs_data);

	mxt_read_object(data,
		PROCG_NOISESUPPRESSION_T62, 35, &threshold);
	if (threshold < 0) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
		return;
	}
	snprintf(buff, sizeof(buff), "%d", threshold);

	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_info(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void module_off_master(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[3] = {0};
	pr_info("module_off_master %d!!\n", mxt_enabled);
	mutex_lock(&data->lock);
	if (mxt_enabled) {
		disable_irq(client->irq);

		if (!data->power_off())
			snprintf(buff, sizeof(buff), "%s", "NG");
		else
			snprintf(buff, sizeof(buff), "%s", "OK");

		mxt_enabled = false;
	} else {
		snprintf(buff, sizeof(buff), "%s", "OK");
	}
	mutex_unlock(&data->lock);

	set_default_result(sysfs_data);
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));

	if (strncmp(buff, "OK", 2) == 0)
		sysfs_data->cmd_state = CMD_STATUS_OK;
	else
		sysfs_data->cmd_state = CMD_STATUS_FAIL;

	dev_info(&client->dev, "%s: %s\n", __func__, buff);
}

static void module_on_master(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[3] = {0};

	mutex_lock(&data->lock);

	if (!mxt_enabled) {
		if (!data->power_on())
			snprintf(buff, sizeof(buff), "%s", "NG");
		else
			snprintf(buff, sizeof(buff), "%s", "OK");

		enable_irq(client->irq);
		mxt_enabled = true;
	} else {
		snprintf(buff, sizeof(buff), "%s", "OK");
	}
	mutex_unlock(&data->lock);

	set_default_result(sysfs_data);
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));

	if (strncmp(buff, "OK", 2) == 0)
		sysfs_data->cmd_state = CMD_STATUS_OK;
	else
		sysfs_data->cmd_state = CMD_STATUS_FAIL;

	dev_info(&client->dev, "%s: %s\n", __func__, buff);

}

static void get_chip_vendor(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};

	set_default_result(sysfs_data);

	snprintf(buff, sizeof(buff), "%s", "ATMEL");
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_dbg(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_chip_name(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};

	set_default_result(sysfs_data);

	snprintf(buff, sizeof(buff), "%s", "MXT224S");
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;
	dev_dbg(&client->dev, "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_x_num(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};
	u8 val = 0;

	set_default_result(sysfs_data);
	val = MATRIX_REAL_X;  /* 19 * 14, but really, 18 * 11 */
	if (val < 0) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
		sysfs_data->cmd_state = CMD_STATUS_FAIL;

		dev_err(&client->dev,
			 "%s: fail to read num of x (%d).\n", __func__, val);

		return;
	}
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;

	dev_dbg(&client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

static void get_y_num(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};
	u8 val;

	set_default_result(sysfs_data);
	/* val = MATRIX_Y;  */
	val = MATRIX_REAL_Y;  /* 19 * 14, but really, 18 * 11 */
	if (val < 0) {
		snprintf(buff, sizeof(buff), "%s", "NG");
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
		sysfs_data->cmd_state = CMD_STATUS_FAIL;

		dev_err(&client->dev,
			 "%s: fail to read num of y (%d).\n", __func__, val);

		return;
	}
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));
	sysfs_data->cmd_state = CMD_STATUS_OK;

	dev_dbg(&client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

static void run_reference_read(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	int ret;
	char buff[16] = {0};

	set_default_result(sysfs_data);

	ret = mxt_read_all_diagnostic_data(data,
			MXT_DIAG_REFERENCE_MODE);
	if (ret)
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
	else {
		snprintf(buff, sizeof(buff), "%d,%d",
			sysfs_data->ref_min_data - mxt224s_ref_offset, sysfs_data->ref_max_data - mxt224s_ref_offset);
		set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));

		sysfs_data->cmd_state = CMD_STATUS_OK;
	}
}

static void get_reference(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	char buff[16] = {0};
	int node;

	set_default_result(sysfs_data);
	/* add read function */

	node = mxt_xy_to_node(data);
	if (node < 0) {
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
		return;
	} else {
		snprintf(buff, sizeof(buff), "%u",
			sysfs_data->reference[node] - mxt224s_ref_offset);
		set_cmd_result(sysfs_data,
			buff, strnlen(buff, sizeof(buff)));

		sysfs_data->cmd_state = CMD_STATUS_OK;
	}
	dev_info(&client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

static void run_delta_read(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	int ret;

	set_default_result(sysfs_data);

	ret = mxt_read_all_diagnostic_data(data,
			MXT_DIAG_DELTA_MODE);
	if (ret)
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
	else
		sysfs_data->cmd_state = CMD_STATUS_OK;
}

static void get_delta(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	char buff[16] = {0};
	int node;

	set_default_result(sysfs_data);
	/* add read function */

	node = mxt_xy_to_node(data);
	if (node < 0) {
		sysfs_data->cmd_state = CMD_STATUS_FAIL;
		return;
	} else {
		snprintf(buff, sizeof(buff), "%d",
			sysfs_data->delta[node]);
		set_cmd_result(sysfs_data,
			buff, strnlen(buff, sizeof(buff)));

		sysfs_data->cmd_state = CMD_STATUS_OK;
	}
	dev_info(&client->dev, "%s: %s(%d)\n", __func__, buff,
			strnlen(buff, sizeof(buff)));
}

#if TSP_BOOSTER
static void boost_level(void *device_data)
{
	struct mxt_data *data = (struct mxt_data *)device_data;
	struct i2c_client *client = data->client;	
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	char buff[3] = {0};

	int retval;

	dev_info(&client->dev, "%s\n", __func__);

	set_default_result(sysfs_data);

	data->booster.dvfs_boost_mode = sysfs_data->cmd_param[0];

	dev_info(&client->dev,
			"%s: dvfs_boost_mode = %d\n",
			__func__, data->booster.dvfs_boost_mode);

	snprintf(buff, sizeof(buff), "OK");
	sysfs_data->cmd_state = CMD_STATUS_OK;

	if (data->booster.dvfs_boost_mode == DVFS_STAGE_NONE) {
			retval = set_freq_limit(DVFS_TOUCH_ID, -1);
			if (retval < 0) {
				dev_err(&client->dev,
					"%s: booster stop failed(%d).\n",
					__func__, retval);
				snprintf(buff, sizeof(buff), "NG");
				sysfs_data->cmd_state = CMD_STATUS_FAIL;

				data->booster.dvfs_lock_status = false;
			}
	}

	set_cmd_result(sysfs_data, buff, strnlen(buff, sizeof(buff)));

	mutex_lock(&sysfs_data->cmd_lock);
	sysfs_data->cmd_is_running = false;
	mutex_unlock(&sysfs_data->cmd_lock);

	sysfs_data->cmd_state = CMD_STATUS_WAITING;

	return;
}
#endif

/* - function realted samsung factory test */

#define TSP_CMD(name, func) .cmd_name = name, .cmd_func = func
#define TOSTRING (x) #x

struct tsp_cmd {
	struct list_head	list;
	const char		*cmd_name;
	void			(*cmd_func)(void *device_data);
};

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
	{TSP_CMD("run_reference_read", run_reference_read),},
	{TSP_CMD("get_reference", get_reference),},
	{TSP_CMD("run_delta_read", run_delta_read),},
	{TSP_CMD("get_delta", get_delta),},
	{TSP_CMD("find_delta", find_delta_node),},
	{TSP_CMD("not_support_cmd", not_support_cmd),},
#if TSP_BOOSTER
	{TSP_CMD("boost_level", boost_level),},
#endif	
};

/* Functions related to basic interface */
static ssize_t store_cmd(struct device *dev, struct device_attribute
				  *devattr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char *cur, *start, *end;
	char buff[TSP_CMD_STR_LEN] = {0};
	int len, i;
	struct tsp_cmd *tsp_cmd_ptr = NULL;
	char delim = ',';
	bool cmd_found = false;
	int param_cnt = 0;
	int ret;

	if (sysfs_data->cmd_is_running == true) {
		dev_err(&client->dev, "tsp_cmd: other cmd is running.\n");
		goto err_out;
	}

	/* check lock  */
	mutex_lock(&sysfs_data->cmd_lock);
	sysfs_data->cmd_is_running = true;
	mutex_unlock(&sysfs_data->cmd_lock);

	sysfs_data->cmd_state = CMD_STATUS_RUNNING;

	for (i = 0; i < ARRAY_SIZE(sysfs_data->cmd_param); i++)
		sysfs_data->cmd_param[i] = 0;

	len = (int)count;
	if (*(buf + len - 1) == '\n')
		len--;
	memset(sysfs_data->cmd, 0x00, ARRAY_SIZE(sysfs_data->cmd));
	memcpy(sysfs_data->cmd, buf, len);

	cur = strchr(buf, (int)delim);
	if (cur)
		memcpy(buff, buf, cur - buf);
	else
		memcpy(buff, buf, len);

	/* find command */
	list_for_each_entry(tsp_cmd_ptr, &sysfs_data->cmd_list_head, list) {
		if (!strcmp(buff, tsp_cmd_ptr->cmd_name)) {
			cmd_found = true;
			break;
		}
	}

	/* set not_support_cmd */
	if (!cmd_found) {
		list_for_each_entry(tsp_cmd_ptr,
			&sysfs_data->cmd_list_head, list) {
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
				ret = kstrtoint(buff, 10,\
					sysfs_data->cmd_param + param_cnt);
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
						sysfs_data->cmd_param[i]);

	tsp_cmd_ptr->cmd_func(data);
err_out:
	return count;
}

static ssize_t show_cmd_status(struct device *dev,
		struct device_attribute *devattr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	char buff[16] = {0};

	dev_dbg(&data->client->dev, "tsp cmd: status:%d\n",
			sysfs_data->cmd_state);

	if (sysfs_data->cmd_state == CMD_STATUS_WAITING)
		snprintf(buff, sizeof(buff), "WAITING");

	else if (sysfs_data->cmd_state == CMD_STATUS_RUNNING)
		snprintf(buff, sizeof(buff), "RUNNING");

	else if (sysfs_data->cmd_state == CMD_STATUS_OK)
		snprintf(buff, sizeof(buff), "OK");

	else if (sysfs_data->cmd_state == CMD_STATUS_FAIL)
		snprintf(buff, sizeof(buff), "FAIL");

	else if (sysfs_data->cmd_state == CMD_STATUS_NOT_APPLICABLE)
		snprintf(buff, sizeof(buff), "NOT_APPLICABLE");

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", buff);
}

static ssize_t show_cmd_result(struct device *dev, struct device_attribute
				    *devattr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;

	dev_info(&data->client->dev,
		"tsp cmd: result: %s\n", sysfs_data->cmd_result);

	mutex_lock(&sysfs_data->cmd_lock);
	sysfs_data->cmd_is_running = false;
	mutex_unlock(&sysfs_data->cmd_lock);

	sysfs_data->cmd_state = CMD_STATUS_WAITING;

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", sysfs_data->cmd_result);
}

static ssize_t show_cmd_list(struct device *dev, struct device_attribute
				    *devattr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct mxt_data_sysfs *sysfs_data = data->sysfs_data;
	struct tsp_cmd *tsp_cmd_ptr = NULL;
	int cnt = 0;

	list_for_each_entry(tsp_cmd_ptr, &sysfs_data->cmd_list_head, list)
		cnt += sprintf(buf + cnt,
			"%s\n", tsp_cmd_ptr->cmd_name);

	return cnt;
}

static DEVICE_ATTR(cmd, S_IWUSR | S_IWGRP, NULL, store_cmd);
static DEVICE_ATTR(cmd_status, S_IRUGO, show_cmd_status, NULL);
static DEVICE_ATTR(cmd_result, S_IRUGO, show_cmd_result, NULL);
static DEVICE_ATTR(cmd_list, S_IRUGO, show_cmd_list, NULL);

static struct attribute *touchscreen_attributes[] = {
	&dev_attr_cmd.attr,
	&dev_attr_cmd_status.attr,
	&dev_attr_cmd_result.attr,
	&dev_attr_cmd_list.attr,
	NULL,
};

static struct attribute_group touchscreen_attr_group = {
	.attrs = touchscreen_attributes,
};

#endif	/* TSP_SEC_SYSFS*/

#if ITDEV
static int mxt_read_block(struct i2c_client *client,
		   u16 addr,
		   u16 length,
		   u8 *value)
{
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg[2];
	__le16	le_addr;
	struct mxt_data *data = i2c_get_clientdata(client);

	if (data != NULL) {
		if ((data->last_read_addr == addr) &&
			(addr == data->msg_proc)) {
			if  (i2c_master_recv(client, value, length) == length) {
				if (debug_enabled)
					print_hex_dump(KERN_INFO, "MXT RX:",
						DUMP_PREFIX_NONE, 16, 1,
						value, length, false);
				return 0;
			} else
				return -EIO;
		} else {
			data->last_read_addr = addr;
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
		if (debug_enabled) {
			print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, msg[0].buf, msg[0].len, false);
			print_hex_dump(KERN_INFO, "MXT RX:", DUMP_PREFIX_NONE,
				16, 1, msg[1].buf, msg[1].len, false);
		}
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

	struct mxt_data *data = i2c_get_clientdata(client);

	if (length > 256)
		return -EINVAL;

	if (data != NULL)
		data->last_read_addr = -1;

	for (i = 0; i < length; i++)
		i2c_block_transfer.data[i] = *value++;

	i2c_block_transfer.le_addr = cpu_to_le16(addr);

	i = i2c_master_send(client, (u8 *) &i2c_block_transfer, length + 2);

	if (i == (length + 2)) {
		if (debug_enabled)
			print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, &i2c_block_transfer, length+2, false);
		return length;
	} else
		return -EIO;
}

static ssize_t mem_access_read(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client =
		to_i2c_client(container_of(kobj, struct device, kobj));

	dev_info(&client->dev, "mem_access_read p=%p off=%lli c=%zi\n",
			buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0)
		ret = mxt_read_block(client, off, count, buf);

	return ret >= 0 ? count : ret;
}

static ssize_t mem_access_write(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client =
		to_i2c_client(container_of(kobj, struct device, kobj));

	dev_info(&client->dev, "mem_access_write p=%p off=%lli c=%zi\n",
			buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0)
		ret = mxt_write_block(client, off, count, buf);

	return ret >= 0 ? count : 0;
}

static ssize_t pause_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
//	struct mxt_data *data = dev_get_drvdata(dev);
	int count = 0;

	count += sprintf(buf + count, "%d", driver_paused);
	count += sprintf(buf + count, "\n");

	return count;
}

static ssize_t pause_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	int i;

	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		driver_paused = i;

		dev_info(&client->dev, "%s\n", i ? "paused" : "unpaused");
	} else {
		dev_info(&client->dev, "pause_driver write error\n");
	}

	return count;
}

static ssize_t debug_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
//	struct mxt_data *data = dev_get_drvdata(dev);
	int count = 0;

	count += sprintf(buf + count, "%d", debug_enabled);
	count += sprintf(buf + count, "\n");

	return count;
}

static ssize_t debug_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	int i;
	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		debug_enabled = i;

		dev_info(&client->dev, "%s\n",
			i ? "debug enabled" : "debug disabled");
	} else {
		dev_info(&client->dev, "debug_enabled write error\n");
	}

	return count;
}

static ssize_t command_calibrate_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret;

	/* send calibration command to the chip */
	ret = mxt_write_object(data, GEN_COMMANDPROCESSOR_T6,
		CMD_CALIBRATE_OFFSET, 1);

	return (ret < 0) ? ret : count;
}

static ssize_t command_reset_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret;

	/* send reset command to the chip */
	ret = mxt_write_object(data, GEN_COMMANDPROCESSOR_T6,
		CMD_RESET_OFFSET, 1);

	return (ret < 0) ? ret : count;
}

static ssize_t command_backup_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret;

	/* send backup command to the chip */
	ret = mxt_write_object(data, GEN_COMMANDPROCESSOR_T6,
		CMD_BACKUP_OFFSET, 0x55);

	return (ret < 0) ? ret : count;
}
#endif	/* TSP_ITDEV */

static ssize_t mxt_debug_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
//	struct mxt_data *data = dev_get_drvdata(dev);
#if 0

	u32 mode = 0;

	if (sscanf(buf, "%u", &mode) == 1)
		data->debug_log = !!mode;
#endif
	return count;
}

static ssize_t mxt_object_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	unsigned int object_type;
	unsigned int object_register;
	unsigned int register_value;
	u8 val;
	int ret;
	sscanf(buf, "%u%u%u", &object_type, &object_register, &register_value);
	dev_info(&client->dev, "object type T%d", object_type);
	dev_info(&client->dev, "object register ->Byte%d\n", object_register);
	dev_info(&client->dev, "register value %d\n", register_value);

	ret = mxt_write_object(data,
		(u8)object_type, (u8)object_register, (u8)register_value);

	if (ret) {
		dev_err(&client->dev, "fail to write T%d index:%d, value:%d\n",
			object_type, object_register, register_value);
		goto out;
	} else {
		ret = mxt_read_object(data,
				(u8)object_type, (u8)object_register, &val);

		if (ret) {
			dev_err(&client->dev, "fail to read T%d\n",
				object_type);
			goto out;
		} else
			dev_info(&client->dev, "T%d Byte%d is %d\n",
				object_type, object_register, val);
	}
out:
	return count;
}

static ssize_t mxt_object_show(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	struct object_t *object;
	unsigned int object_type;
	u8 val;
	u16 i;

	sscanf(buf, "%u", &object_type);
	dev_info(&client->dev, "object type T%d\n", object_type);

	object = mxt_get_object(data, object_type);
	if (!object) {
		dev_err(&client->dev, "fail to get object_info\n");
		return -EINVAL;
	} else {
		for (i = 0; i < object->size; i++) {
			read_mem(data, object->i2c_address + i,
				1, &val);
			dev_info(&client->dev, "Byte %u --> %u\n", i, val);
		}
	}

	return count;
}

#if ITDEV
/* Functions for mem_access interface */
struct bin_attribute mem_access_attr;

/* Sysfs files for libmaxtouch interface */
static DEVICE_ATTR(pause_driver, S_IRUGO | S_IWUGO,
	pause_show, pause_store);
static DEVICE_ATTR(debug_enable, S_IRUGO | S_IWUGO,
	debug_enable_show, debug_enable_store);
static DEVICE_ATTR(command_calibrate, S_IRUGO | S_IWUGO,
	NULL, command_calibrate_store);
static DEVICE_ATTR(command_reset, S_IRUGO | S_IWUGO,
	NULL, command_reset_store);
static DEVICE_ATTR(command_backup, S_IRUGO | S_IWUGO,
	NULL, command_backup_store);
#endif
static DEVICE_ATTR(object_show, S_IWUSR | S_IWGRP, NULL,
	mxt_object_show);
static DEVICE_ATTR(object_write, S_IWUSR | S_IWGRP, NULL,
	mxt_object_setting);
static DEVICE_ATTR(dbg_switch, S_IWUSR | S_IWGRP, NULL,
	mxt_debug_setting);

static struct attribute *libmaxtouch_attributes[] = {
#if ITDEV
	&dev_attr_pause_driver.attr,
	&dev_attr_debug_enable.attr,
	&dev_attr_command_calibrate.attr,
	&dev_attr_command_reset.attr,
	&dev_attr_command_backup.attr,
#endif
	&dev_attr_object_show.attr,
	&dev_attr_object_write.attr,
	&dev_attr_dbg_switch.attr,
	NULL,
};

static struct attribute_group libmaxtouch_attr_group = {
	.attrs = libmaxtouch_attributes,
};

int  __devinit mxt_sysfs_init(struct i2c_client *client)
{
	struct mxt_data *data = i2c_get_clientdata(client);
	int i;
	int ret;
#if TSP_SEC_SYSFS
	struct mxt_data_sysfs *sysfs_data = NULL;
	struct device *fac_dev_ts;

	sysfs_data = kzalloc(sizeof(struct mxt_data_sysfs), GFP_KERNEL);
	if (sysfs_data == NULL) {
		dev_err(&client->dev, "failed to allocate sysfs data.\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&sysfs_data->cmd_list_head);
	for (i = 0; i < ARRAY_SIZE(tsp_cmds); i++)
		list_add_tail(&tsp_cmds[i].list, &sysfs_data->cmd_list_head);

	mutex_init(&sysfs_data->cmd_lock);
	sysfs_data->cmd_is_running = false;

	data->sysfs_data = sysfs_data;

	fac_dev_ts = device_create(sec_class,
			NULL, 0, data, "tsp");
	if (IS_ERR(fac_dev_ts)) {
		dev_err(&client->dev, "Failed to create device for the sysfs\n");
		ret = IS_ERR(fac_dev_ts);
		goto free_mem;
	}
	ret = sysfs_create_group(&fac_dev_ts->kobj, &touchscreen_attr_group);
	if (ret) {
		dev_err(&client->dev, "Failed to create touchscreen sysfs group\n");
		goto free_mem;
	}
#endif

	ret = sysfs_create_group(&client->dev.kobj, &libmaxtouch_attr_group);
	if (ret) {
		dev_err(&client->dev, "Failed to create libmaxtouch sysfs group\n");
		goto free_mem;
	}

#if ITDEV
	sysfs_bin_attr_init(&mem_access_attr);
	mem_access_attr.attr.name = "mem_access";
	mem_access_attr.attr.mode = S_IRUGO | S_IWUGO;
	mem_access_attr.read = mem_access_read;
	mem_access_attr.write = mem_access_write;
	mem_access_attr.size = 65535;

	if (sysfs_create_bin_file(&client->dev.kobj, &mem_access_attr) < 0) {
		dev_err(&client->dev, "Failed to create device file(%s)!\n",
				mem_access_attr.attr.name);
		goto free_mem;
	}
#endif

	return 0;

free_mem:
#if TSP_SEC_SYSFS
	kfree(sysfs_data);
#endif
	return ret;
}

#if TSP_BOOSTER
void change_dvfs_lock(struct work_struct *work)
{
	struct mxt_data *data = container_of(work,
				struct mxt_data, booster.work_dvfs_chg.work);
	int retval = 0;

	mutex_lock(&data->booster.dvfs_lock);
	if (data->booster.dvfs_boost_mode == DVFS_STAGE_DUAL) {
		retval = set_freq_limit(DVFS_TOUCH_ID,
				MIN_TOUCH_LIMIT_SECOND);
		data->booster.dvfs_freq = MIN_TOUCH_LIMIT_SECOND;
	} else if (data->booster.dvfs_boost_mode == DVFS_STAGE_SINGLE) {
		retval = set_freq_limit(DVFS_TOUCH_ID, -1);
		data->booster.dvfs_freq = -1;
	}

	if (retval < 0)
		dev_err(&data->client->dev,
			"%s: booster change failed(%d).\n",
			__func__, retval);
	mutex_unlock(&data->booster.dvfs_lock);
}

void set_dvfs_off(struct work_struct *work)
{
	struct mxt_data *data = container_of(work,
				struct mxt_data, booster.work_dvfs_off.work);
	int retval;

	mutex_lock(&data->booster.dvfs_lock);
	retval = set_freq_limit(DVFS_TOUCH_ID, -1);
	data->booster.dvfs_freq = -1;
	if (retval < 0)
		dev_err(&data->client->dev,
			"%s: booster stop failed(%d).\n",
			__func__, retval);	
	data->booster.dvfs_lock_status = false;
	mutex_unlock(&data->booster.dvfs_lock);
}

void set_dvfs_lock(struct mxt_data *data, int32_t on)
{
	int ret = 0;

	if (data->booster.dvfs_boost_mode == DVFS_STAGE_NONE) {
		dev_info(&data->client->dev,
				"%s: DVFS stage is none(%d)\n",
				__func__, data->booster.dvfs_boost_mode);
		return;
	}

	mutex_lock(&data->booster.dvfs_lock);
	if (on == 0) {
		if (data->booster.dvfs_lock_status) {
			cancel_delayed_work(&data->booster.work_dvfs_chg);
			schedule_delayed_work(&data->booster.work_dvfs_off,
				msecs_to_jiffies(TOUCH_BOOSTER_OFF_TIME));
		}
	} else if (on > 0) {
		cancel_delayed_work(&data->booster.work_dvfs_off);
		if (data->booster.dvfs_old_stauts != on) {
			cancel_delayed_work(&data->booster.work_dvfs_chg);
			if (data->booster.dvfs_freq != MIN_TOUCH_LIMIT) {
				ret = set_freq_limit(DVFS_TOUCH_ID,
				MIN_TOUCH_LIMIT);
				data->booster.dvfs_freq = MIN_TOUCH_LIMIT;

				if (ret < 0)
				dev_err(&data->client->dev,
				"%s: cpu first lock failed(%d)\n",
				__func__, ret);
			}
			schedule_delayed_work(&data->booster.work_dvfs_chg,
				msecs_to_jiffies(TOUCH_BOOSTER_CHG_TIME));
			data->booster.dvfs_lock_status = true;
		}
	} else if (on < 0) {
		if (data->booster.dvfs_lock_status) {
			cancel_delayed_work(&data->booster.work_dvfs_off);
			cancel_delayed_work(&data->booster.work_dvfs_chg);
			schedule_work(&data->booster.work_dvfs_off.work);
		}
	}
	data->booster.dvfs_old_stauts = on;
	mutex_unlock(&data->booster.dvfs_lock);
}

void mxt_init_dvfs(struct mxt_data *data)
{
	mutex_init(&data->booster.dvfs_lock);
	data->booster.dvfs_boost_mode = DVFS_STAGE_DUAL;
	INIT_DELAYED_WORK(&data->booster.work_dvfs_off, set_dvfs_off);
	INIT_DELAYED_WORK(&data->booster.work_dvfs_chg, change_dvfs_lock);
	data->booster.dvfs_lock_status = false;
}
#endif /* - TOUCH_BOOSTER */


MODULE_DESCRIPTION("sec sysfs for the Atmel");
MODULE_LICENSE("GPL");
