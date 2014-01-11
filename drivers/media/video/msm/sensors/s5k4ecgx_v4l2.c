/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <media/msm_camera.h>
#include <media/v4l2-subdev.h>
#include <mach/gpio.h>
#include <mach/camera.h>

#include <asm/mach-types.h>
#include <mach/vreg.h>
#include <linux/io.h>

#include "msm.h"
#include "msm_ispif.h"
#include "msm_sensor.h"
#include <mach/msm8930-gpio.h>

#include "s5k4ecgx.h"
#if defined(CONFIG_MACH_WILCOX_EUR_LTE)
#include "s5k4ecgx_wilcox_regs.h"
#elif defined(CONFIG_MACH_LOGANRE)
#include "s5k4ecgx_loganre_regs.h"
#else
#include "s5k4ecgx_regs.h"
#endif
#ifdef CONFIG_LEDS_RT8547
#include <linux/leds-rt8547.h>
#endif /*CONFIG_LEDS_RT8547*/

/*#define CONFIG_LOAD_FILE */

#ifdef CONFIG_LOAD_FILE
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static char *s5k4ecgx_regs_table;
static int s5k4ecgx_regs_table_size;
static int s5k4ecgx_write_regs_from_sd(char *name);
static int s5k4ecgx_sensor_write(unsigned short addr, unsigned int w_data);
#endif

static int s5k4ecgx_set_whitebalance(int wb);
static int s5k4ecgx_set_effect(int effect);
static void s5k4ecgx_set_ev(int ev);
static void s5k4ecgx_set_camcorder_ev(int ev);
static int s5k4ecgx_set_af_mode(int mode);
static int s5k4ecgx_set_ae_awb(int lock);
static int s5k4ecgx_reset_AF_region(void);
static void s5k4ecgx_set_scene_mode(int mode);
static void s5k4ecgx_set_metering(int mode);
static int s5k4ecgx_sensor_config(struct msm_sensor_ctrl_t *s_ctrl, void __user *argp);
static void s5k4ecgx_exif_shutter_speed(void);
static void s5k4ecgx_exif_iso(void);
static int s5k4ecgx_get_flash_status(void);
static void s5k4ecgx_set_flash(int mode);

struct s5k4ecgx_exif_data
{
	unsigned short iso;
	unsigned short shutterspeed;
};

static struct s5k4ecgx_exif_data *s5k4ecgx_exif;
DEFINE_MUTEX(s5k4ecgx_mut);

#ifndef CONFIG_LOAD_FILE
#define S5K4ECGX_USE_BURSTMODE
#endif

#ifdef S5K4ECGX_USE_BURSTMODE
#define S5K4ECGX_WRITE_LIST(A) \
			s5k4ecgx_sensor_burst_write(A, (sizeof(A) / sizeof(A[0])), #A)
#else
#define S5K4ECGX_WRITE_LIST(A) \
			s5k4ecgx_sensor_write_list(A, (sizeof(A) / sizeof(A[0])), #A)
#endif

static struct  i2c_client *s5k4ecgx_client;
static struct msm_sensor_ctrl_t s5k4ecgx_s_ctrl;
static struct device s5k4ecgx_dev;

struct s5k4ecgx_ctrl {
	const struct msm_camera_sensor_info *sensordata;
	struct s5k4ecgx_userset settings;
	struct msm_camera_i2c_client *sensor_i2c_client;
	struct msm_sensor_ctrl_t *s_ctrl;
	struct v4l2_subdev *sensor_dev;
	struct v4l2_subdev sensor_v4l2_subdev;
	struct v4l2_subdev_info *sensor_v4l2_subdev_info;
	uint8_t sensor_v4l2_subdev_info_size;
	struct v4l2_subdev_ops *sensor_v4l2_subdev_ops;

	int op_mode;
	int dtp_mode;
	int cam_mode;
	int pre_cam_mode;
	int vtcall_mode;
	int started;
	int dtpTest;
	int isCapture;
	unsigned int ae_awb_lock;
	int touchaf_enable;
	int af_mode;
	int lowLight;
	int lightsensing_mode;
};

static unsigned int config_csi2;
static struct s5k4ecgx_ctrl *s5k4ecgx_ctrl;
int af_low_lux;
static bool mode_enable;

struct s5k4ecgx_format {
	enum v4l2_mbus_pixelcode code;
	enum v4l2_colorspace colorspace;
	u16 fmt;
	u16 order;
};


#ifdef CONFIG_LOAD_FILE

void s5k4ecgx_regs_table_init(void)
{
	struct file *filp;
	char *dp;
	long lsize;
	loff_t pos;
	int ret;

	/*Get the current address space */
	mm_segment_t fs = get_fs();

	CAM_DEBUG("%s %d", __func__, __LINE__);

	/*Set the current segment to kernel data segment */
	set_fs(get_ds());

	filp = filp_open("/mnt/sdcard/s5k4ecgx_regs.h", O_RDONLY, 0);

	if (IS_ERR_OR_NULL(filp)) {
		cam_err("file open error\n");
		return ;
	}

	lsize = filp->f_path.dentry->d_inode->i_size;
	CAM_DEBUG("size : %ld", lsize);
	dp = vmalloc(lsize);
	if (dp == NULL) {
		cam_err("Out of Memory");
		filp_close(filp, current->files);
	}

	pos = 0;
	memset(dp, 0, lsize);
	ret = vfs_read(filp, (char __user *)dp, lsize, &pos);
	if (ret != lsize) {
		cam_err("Failed to read file ret = %d\n", ret);
		vfree(dp);
		filp_close(filp, current->files);
	}
	/*close the file*/
	filp_close(filp, current->files);

	/*restore the previous address space*/
	set_fs(fs);

	s5k4ecgx_regs_table = dp;

	s5k4ecgx_regs_table_size = lsize;

	*((s5k4ecgx_regs_table + s5k4ecgx_regs_table_size) - 1) = '\0';

	CAM_DEBUG("s5k4ecgx_reg_table_init");

	return;
}

void s5k4ecgx_regs_table_exit(void)
{
	CAM_DEBUG("%s %d", __func__, __LINE__);
	if (s5k4ecgx_regs_table) {
		vfree(s5k4ecgx_regs_table);
		s5k4ecgx_regs_table = NULL;
	}
}

static int s5k4ecgx_write_regs_from_sd(char *name)
{
	char *start, *end, *reg;
	unsigned short addr, value;
	unsigned long data;
	char data_buf[11];

	addr = value = 0;
	CAM_DEBUG(" : E");
	//*(reg_buf + 6) = '\0';
	*(data_buf + 10) = '\0';

	if (s5k4ecgx_regs_table == NULL){
		CAM_DEBUG("s5k4ecgx_regs_table == NULL ::: s5k4ecgx_regs_table_write \n");
		return 0;
	}
	CAM_DEBUG("write table = s5k4ecgx_regs_table,find string = %s\n", name);
	start = strstr(s5k4ecgx_regs_table, name);
	if (start == NULL){
		CAM_DEBUG("start == NULL ::: start \n");
		return 0;
	}
	end = strstr(start, "};");
	if(end == NULL){
		CAM_DEBUG("end == NULL ::: end \n");
		return 0;	
	}

	while (1) {
		/* Find Address */
		if(start >= end)
		break;
		reg = strstr(start, "0x");
		if (reg)
			start = (reg + 10);

		if (reg == NULL){
			if(reg > end){
			CAM_DEBUG("write end of %s \n",name);
			break;
			}
			else if(reg < end){
				CAM_DEBUG(	"EXCEPTION! reg value : %c  addr : 0x%x,  value : 0x%x\n",
				*reg, addr, value);
			}
		}
		/* Write Value to Address */
		//memcpy(reg_buf, reg, 6);
		memcpy(data_buf, reg, 10);

		//addr = (unsigned short)simple_strtoul
		//	(reg_buf, NULL, 16);
		data = (unsigned long)simple_strtoul
			(data_buf, NULL, 16);

		addr = (data>>16);
		value = (data&0xffff);

		//printk("[S5K4ECGX]addr 0x%04x, value 0x%04x\n",addr, value);

		if (addr == 0xffff) {
			msleep(value);
			CAM_DEBUG(
				"delay 0x%04x, value 0x%04x\n",
				addr, value);
		} else {
		if (s5k4ecgx_sensor_write(addr, value) < 0)	{
				CAM_DEBUG(
					"%s fail on sensor_write\n",
					__func__);
				return -EIO;
			}
		}
	}
	CAM_DEBUG(" : X");
	return 0;
}

#endif

static DECLARE_WAIT_QUEUE_HEAD(s5k4ecgx_wait_queue);

/*
 * s5k4ecgx_sensor_read: Read (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @data: data to be read
 *
 * Returns 0 on success, <0 on error
 */
static int s5k4ecgx_sensor_read(unsigned short subaddr, unsigned short *data)
{
	unsigned char buf[2];
	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 2, buf};

	int err = 0;

	if (!s5k4ecgx_client->adapter)
		return -EIO;

	buf[0] = subaddr >> 8;
	buf[1] = subaddr & 0xFF;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	msg.flags = I2C_M_RD;

	err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
	if (unlikely(err < 0))
		return -EIO;

	*data = ((buf[0] << 8) | buf[1]);

	/* [Arun c]Data should be written in Little Endian in parallel mode;
	   So there is no need for byte swapping here */
	/*data = *(unsigned long *)(&buf); */

	return err;
}

/**
 * s5k4ecgx_sensor_write: Write (I2C) multiple bytes to the camera sensor
 * @client: pointer to i2c_client
 * @cmd: command register
 * @w_data: data to be written
 * @w_len: length of data to be written
 *
 * Returns 0 on success, <0 on error
 */
static int s5k4ecgx_sensor_write(unsigned short addr, unsigned int w_data)
{
	unsigned char buf[4];
	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 4, buf};

	int retry_count = 3;
	int err = 0;

	if (!s5k4ecgx_client->adapter)
		return -EIO;

	buf[0] = addr >> 8;
	buf[1] = addr & 0xFF;
	buf[2] = w_data >> 8;
	buf[3] = w_data & 0xFF;
	/*
	 * Data should be written in Little Endian in parallel mode; So there
	 * is no need for byte swapping here
	 */

	while (retry_count--) {
		err  = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
		if (likely(err == 1))
			break;
	}
	return (err == 1) ? 0 : -EIO;
}
#ifdef S5K4ECGX_USE_BURSTMODE
#define BURST_MODE_BUFFER_MAX_SIZE 3000
unsigned char s5k4ecgx_buf[BURST_MODE_BUFFER_MAX_SIZE];
static int s5k4ecgx_sensor_burst_write(const unsigned int *list, int size,
				       char *name)
{
	int i = 0;
	int idx;
	int err = -EINVAL;
	int retry = 0;
	unsigned short subaddr = 0, next_subaddr = 0;
	unsigned short value = 0;

	struct i2c_msg msg = {s5k4ecgx_client->addr, 0, 0, s5k4ecgx_buf};

	CAM_DEBUG("%s", name);

	if (!s5k4ecgx_client->adapter) {
		CAM_DEBUG("%s: %d can't search i2c client adapter\n",
			__func__, __LINE__);
		return -EIO;
	}

I2C_RETRY:
	idx = 0;
	for (i = 0; i < size; i++) {

		if (idx > (BURST_MODE_BUFFER_MAX_SIZE - 10)) {
			/*pr_info("[S5K4ECGX]s5k4ecgx_buf_for_burstmode
			    overflow will occur!!!\n");*/
			return err;
		}

		/*address */
		subaddr = (list[i] >> 16);
		if (subaddr == 0x0F12)	/*address */
			next_subaddr = list[i + 1] >> 16;
		value = (list[i] & 0xFFFF);	/*value */

		switch (subaddr) {
		case 0x0F12:
			/* make and fill buffer for burst mode write */
			if (idx == 0) {
				s5k4ecgx_buf[idx++] = 0x0F;
				s5k4ecgx_buf[idx++] = 0x12;
			}
			s5k4ecgx_buf[idx++] = value >> 8;
			s5k4ecgx_buf[idx++] = value & 0xFF;
			/*write in burstmode */
			if (next_subaddr != 0x0F12) {
				msg.len = idx;
				err = i2c_transfer(s5k4ecgx_client->adapter, &msg, 1);
				/*pr_info("s5k4ecgx_sensor_burst_write, idx = %d\n", idx); */
				idx = 0;
				if (unlikely(err < 0)) {
					printk("[S5K4ECGX]%s: register set failed.\n",
						__func__);
					if ((retry++) < 3) {
						printk("[S5K4ECGX]%s: retry (%d)\n", __func__, retry);
						goto I2C_RETRY;
					}
					return err;
				}
			}
			break;
		case 0xFFFF:
			break;
		default:
			/* Set Address */
			idx = 0;
			err = s5k4ecgx_sensor_write(subaddr, value);
			if (unlikely(err < 0)) {
				printk("[S5K4ECGX]%s: register set failed.\n",
					__func__);
				return err;
			}
			break;
		}
	}

	CAM_DEBUG(" end!\n");
	return 0;
	
}
#else
static int s5k4ecgx_sensor_write_list(const unsigned int *list, int size,
				      char *name)
{
#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_write_regs_from_sd(name);
#else

	int ret = 0;
	int i;
	unsigned short subaddr;
	unsigned short value;

	CAM_DEBUG("%s", name);

	if (!s5k4ecgx_client->adapter) {
		cam_err("Can't search i2c client adapter");
		return -EIO;
	}

	for (i = 0; i < size; i++) {
		/*CAM_DEBUG("[PGH] %x
		   %x\n", list[i].subaddr, list[i].value); */
		subaddr = (list[i] >> 16);	/*address */
		value = (list[i] & 0xFFFF);	/*value */
		if (subaddr == 0xffff) {
			CAM_DEBUG("SETFILE DELAY : %dms\n", value);
			msleep(value);
		} else {
			if (s5k4ecgx_sensor_write(subaddr, value) < 0) {
				pr_info("[S5K4ECGX]sensor_write_list fail\n");
				return -EIO;
			}
		}
	}
#endif
	return 0;
}
#endif

int s5k4ecgx_get_light_level(void)
{
	unsigned short msb = 0;
	unsigned short lsb = 0;
	unsigned short cur_lux = 0;

	s5k4ecgx_sensor_write(0x002C, 0x7000);
	s5k4ecgx_sensor_write(0x002E, 0x2C18);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&lsb);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&msb);

	cur_lux = (msb << 16) | lsb;

	CAM_DEBUG("cur_lux = %d", cur_lux);

	return cur_lux;
}

#ifdef FACTORY_TEST
struct class *sec_class;
struct device *s5k4ecgx_dev;

static ssize_t cameratype_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
	char sensor_info[30] = "s5k4ecgx";
	return snprintf(buf, "%s\n", sensor_info);
}

static ssize_t cameratype_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
		/*Reserved*/
	return size;
}

static struct device_attribute s5k4ecgx_camtype_attr = {
	.attr = {
		.name = "camtype",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameratype_file_cmd_show,
		.store = cameratype_file_cmd_store
};

static ssize_t cameraflash_file_cmd_show(struct device *dev,
				struct device_attribute *attr, char *buf) {
		/*Reserved*/
	return 0;
}

static ssize_t cameraflash_file_cmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size) {
	int value;
	sscanf(buf, "%d", &value);

	if (value == 0) {
		CAM_DEBUG("[Factory flash]OFF");
		s5k4ecgx_set_flash(MOVIE_FLASH, 0);
	} else {
		CAM_DEBUG("[Factory flash]ON");
		s5k4ecgx_set_flash(MOVIE_FLASH, 1);
	}
	return size;
}

static struct device_attribute s5k4ecgx_cameraflash_attr = {
	.attr = {
		.name = "cameraflash",
		.mode = (S_IRUGO | S_IWUGO)},
		.show = cameraflash_file_cmd_show,
		.store = cameraflash_file_cmd_store
};
#endif

static int s5k4ecgx_get_exif(int exif_cmd)
{
	unsigned short val=0;
	CAM_DEBUG("E");

	switch (exif_cmd) {
	case EXIF_SHUTTERSPEED:
		val = s5k4ecgx_exif->shutterspeed;
		break;

	case EXIF_ISO:
		val = s5k4ecgx_exif->iso;
		CAM_DEBUG("exif iso value read : %d\n", val);
		break;

	default:
		CAM_DEBUG("invalid cmd: %d", exif_cmd);
		break;
	}

	CAM_DEBUG("X");
	return val;
}

static void s5k4ecgx_exif_shutter_speed()
{
	unsigned short msb = 0;
	unsigned short lsb = 0;

	s5k4ecgx_sensor_write(0x002C, 0x7000);
	s5k4ecgx_sensor_write(0x002E, 0x2BC0);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&lsb);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&msb);

	s5k4ecgx_exif->shutterspeed = 400000 / ((msb << 16) + lsb);
	CAM_DEBUG("Exposure time = %d\n", s5k4ecgx_exif->shutterspeed);
	return;
}

void s5k4ecgx_set_frame_rate(int32_t fps)
{
	CAM_DEBUG(" %d", fps);

	switch (fps) {
	case 15:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_15);
		break;

	case 20:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_20);
		break;

	case 24:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_24);
		break;

	case 25:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_25);
		break;

	case 30:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_30);
		break;

	default:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FPS_Auto);
		break;
	}
	s5k4ecgx_ctrl->settings.fps = fps;
}

static void s5k4ecgx_exif_iso()
{

	unsigned short analog_gain;
	unsigned short digital_gain;
	unsigned int iso_value;

	s5k4ecgx_sensor_write(0x002C, 0x7000);
	s5k4ecgx_sensor_write(0x002E, 0x2BC4);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&analog_gain);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&digital_gain);

	iso_value =(unsigned int) ( analog_gain * digital_gain ) / 256 / 2;
	if (iso_value < 0xD0)
		s5k4ecgx_exif->iso = 50;
	else if(iso_value < 0x1A0)
		s5k4ecgx_exif->iso = 100;
	else if(iso_value < 0x374)
		s5k4ecgx_exif->iso = 200;
	else
		s5k4ecgx_exif->iso = 400;

	CAM_DEBUG(" : %d , iso_value = 0x%x\n", s5k4ecgx_exif->iso, iso_value);

	return;
}

void s5k4ecgx_set_preview_size(int32_t index)
{
	CAM_DEBUG("index %d", index);

	switch (index) {
	case PREVIEW_SIZE_VGA:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_640_Preview);
		break;
		
	case PREVIEW_SIZE_D1:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_720_Preview);
		break;

	case PREVIEW_SIZE_WVGA:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_800_Preview);
		break;

	case PREVIEW_SIZE_HD:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_1280_Preview);
		break;

	default:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_640_Preview);
		break;
	}

	s5k4ecgx_ctrl->settings.preview_size_idx = index;
}


void s5k4ecgx_set_preview(void)
{
	CAM_DEBUG("cam_mode = %d", s5k4ecgx_ctrl->cam_mode);
	mode_enable = true;

	if (s5k4ecgx_ctrl->cam_mode == MOVIE_MODE) {
		CAM_DEBUG("Camcorder_Mode_ON");
		if (s5k4ecgx_ctrl->settings.preview_size_idx ==
				PREVIEW_SIZE_HD) {
			CAM_DEBUG("720P recording");
			S5K4ECGX_WRITE_LIST(s5k4ecgx_1280_Camcorder);
		} else if (s5k4ecgx_ctrl->settings.preview_size_idx ==
				PREVIEW_SIZE_D1) {
			CAM_DEBUG("D1 recording");
			S5K4ECGX_WRITE_LIST(s5k4ecgx_720_Camcorder);
		} else {
			CAM_DEBUG("VGA recording");
			S5K4ECGX_WRITE_LIST(s5k4ecgx_640_Camcorder);
			if (s5k4ecgx_ctrl->settings.fps == 15) {
				CAM_DEBUG("MMS recording");
				s5k4ecgx_set_frame_rate(s5k4ecgx_ctrl->settings.fps);
			}
		}

		if (s5k4ecgx_ctrl->settings.scene ==
				CAMERA_SCENE_AUTO) {
			s5k4ecgx_set_whitebalance(s5k4ecgx_ctrl->settings.wb);
			s5k4ecgx_set_effect(s5k4ecgx_ctrl->settings.effect);
			s5k4ecgx_set_camcorder_ev(s5k4ecgx_ctrl->settings.brightness);
			s5k4ecgx_set_af_mode(s5k4ecgx_ctrl->settings.focus_mode);
		} else {
			if (s5k4ecgx_ctrl->settings.scene
					== CAMERA_SCENE_NIGHT) {
				msleep(500);
				CAM_DEBUG("500ms (NIGHTSHOT)");
			} else if (s5k4ecgx_ctrl->settings.scene
					== CAMERA_SCENE_FIRE) {
				msleep(800);
				CAM_DEBUG("800ms (FIREWORK)");
			} else {
				msleep(50);
			}
		}
	} else {
		CAM_DEBUG("Preview_Mode");
		if (s5k4ecgx_ctrl->op_mode == CAMERA_MODE_INIT) {
			s5k4ecgx_set_ae_awb(0);
			s5k4ecgx_set_preview_size\
				(s5k4ecgx_ctrl->settings.preview_size_idx);
			if ( s5k4ecgx_ctrl->lightsensing_mode == 1 ) { // Lightsensing mode enabled
				CAM_DEBUG("lightsensing_mode");
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_lock);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_awb_lock);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_100);
			} else {
				if (s5k4ecgx_ctrl->settings.scene ==
					CAMERA_SCENE_AUTO) {
					s5k4ecgx_set_whitebalance\
						(s5k4ecgx_ctrl->settings.wb);
					s5k4ecgx_set_effect\
						(s5k4ecgx_ctrl->settings.effect);
					s5k4ecgx_set_ev\
						(s5k4ecgx_ctrl->settings.brightness);
					s5k4ecgx_set_metering\
						(s5k4ecgx_ctrl->settings.metering);
				} else {
					s5k4ecgx_set_scene_mode\
						(s5k4ecgx_ctrl->settings.scene);
					if (s5k4ecgx_ctrl->settings.scene
						== CAMERA_SCENE_NIGHT) {
						msleep(500);
						CAM_DEBUG("500ms (NIGHTSHOT)");
					} else if (s5k4ecgx_ctrl->settings.scene
						== CAMERA_SCENE_FIRE) {
						msleep(800);
						CAM_DEBUG("800ms (FIREWORK)");
					} else {
						msleep(50);
					}
				}
			}
		} else {
			if (s5k4ecgx_ctrl->pre_cam_mode == MOVIE_MODE ) {
				CAM_DEBUG("Return_preview_Mode from Camcorder mode");
				S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_Disable);
				s5k4ecgx_set_preview_size\
						(s5k4ecgx_ctrl->settings.preview_size_idx);
			 } else {
				CAM_DEBUG("Return_preview_Mode from Capture mode");
				s5k4ecgx_set_ae_awb(0);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_Preview_Return);
				s5k4ecgx_set_preview_size\
						(s5k4ecgx_ctrl->settings.preview_size_idx);
			 }
		if (s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_NIGHT)
			msleep(500);
		else if (s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_FIRE)
			msleep(1200);
		else
			msleep(120);
		}
		s5k4ecgx_ctrl->op_mode = CAMERA_MODE_PREVIEW;
	}
}

void s5k4ecgx_set_capture(void)
{

	s5k4ecgx_ctrl->op_mode = CAMERA_MODE_CAPTURE;

	/** Capture Sequence **/
	if (s5k4ecgx_get_flash_status()) {
		s5k4ecgx_set_flash(CAPTURE_FLASH);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Main_Flash_On);
	}

	S5K4ECGX_WRITE_LIST(s5k4ecgx_Capture_Start);
	msleep(100);
	if ((s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_NIGHT)
			|| (s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_FIRE)) {
		CAM_DEBUG("Delay for Night or Firework  Snapshot!");
		msleep(100);
	}

	/*Exif data*/
	s5k4ecgx_exif_shutter_speed();
	s5k4ecgx_exif_iso();
	mode_enable = false;
}

static int32_t s5k4ecgx_sensor_setting(int update_type, int rt)
{
	int32_t rc = 0;

	CAM_DEBUG("Start");
	switch (update_type) {
	case REG_INIT:
		if (rt == RES_PREVIEW || rt == RES_CAPTURE)
			/* Add some condition statements */
		break;
	case UPDATE_PERIODIC:
	msm_sensor_enable_debugfs(s5k4ecgx_ctrl->s_ctrl);
	if (rt == RES_PREVIEW || rt == RES_CAPTURE) {
			CAM_DEBUG("UPDATE_PERIODIC");
	msleep(20);

	CAM_DEBUG(" start AP MIPI setting");
	if(s5k4ecgx_ctrl->s_ctrl->sensordata->
		sensor_platform_info->
		csi_lane_params != NULL) {
		CAM_DEBUG(" lane_assign ="\
			" 0x%x",
			s5k4ecgx_ctrl->s_ctrl->
			sensordata->
			sensor_platform_info->
			csi_lane_params->
			csi_lane_assign);

		CAM_DEBUG(" lane_mask ="\
			" 0x%x",
			s5k4ecgx_ctrl->s_ctrl->
			sensordata->
			sensor_platform_info->
			csi_lane_params->
			csi_lane_mask);
		}

		mb();
	
		mb();
		config_csi2 = 1;
		msleep(20);
		
	}
	break;
	default:
		rc = -EINVAL;
		break;
	}

	return rc;
}

static int32_t s5k4ecgx_video_config(int mode)
{
	int32_t	rc = 0;
	CAM_DEBUG(" E ");
	if (s5k4ecgx_sensor_setting(UPDATE_PERIODIC, RES_PREVIEW) < 0)
		rc = -1;

	return rc;
}

static long s5k4ecgx_set_sensor_mode(int mode)
{
	CAM_DEBUG("%d", mode);

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
	case SENSOR_VIDEO_MODE:
		s5k4ecgx_set_preview();
		if (config_csi2 == 0)
			s5k4ecgx_video_config(mode);
		break;
	case SENSOR_SNAPSHOT_MODE:
	case SENSOR_RAW_SNAPSHOT_MODE:
		s5k4ecgx_set_capture();
		s5k4ecgx_reset_AF_region();
		break;
	default:
		return 0;
	}
	return 0;
}

static struct msm_cam_clk_info cam_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

static int s5k4ecgx_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;

	cam_info(" Nothing");

	return rc;
}

#if defined(CONFIG_S5K4ECGX) && defined(CONFIG_SR030PC50) /* CANE */
static struct regulator *l11, *l29, *l32, *l34;
static int s5k4ecgx_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int ret = 0;
	int temp = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG(" : E");
#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_regs_table_init();
#endif

	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0)
		pr_err("%s: request gpio failed\n", __func__);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_3M_RST : %d", temp);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_3M_ISP_INIT : %d", temp);

	/*Power on the LDOs */
	//data->sensor_platform_info->sensor_power_on(0);
	//usleep(20);

	/*Sensor AVDD 2.8V - CAM_SENSOR_A2P8 */
	l32 = regulator_get(NULL, "8917_l32");
	ret = regulator_set_voltage(l32, 2800000, 2800000);
	if (ret)
		cam_err("error setting voltage\n");
	ret = regulator_enable(l32);
	if (ret)
		cam_err("error enabling regulator\n");

	udelay(150);

	/*Sensor vt core 1.8 - VT_CORE_1P8 */
	l29 = regulator_get(NULL, "8921_l29");
	ret = regulator_set_voltage(l29, 1800000, 1800000);
	if (ret)
		cam_err("error setting voltage\n");
	ret = regulator_enable(l29);
	if (ret)
		cam_err("error enabling regulator\n");
	udelay(250);

	/*Sensor IO 1.8V -CAM_SENSOR_IO_1P8  */
	l34 = regulator_get(NULL, "8917_l34");
	ret = regulator_set_voltage(l34, 1800000, 1800000);
	if (ret)
		cam_err("error setting voltage\n");
	ret = regulator_enable(l34);
	if (ret)
		cam_err("error enabling regulator\n");
	udelay(250);

	/*Sensor AF 2.8V -CAM_AF_2P8  */
	l11 = regulator_get(NULL, "8917_l11");
	ret = regulator_set_voltage(l11, 2800000, 2850000);
	if (ret)
		cam_err("error setting voltage\n");
	ret = regulator_enable(l11);
	if (ret)
		cam_err("error enabling regulator\n");
	udelay(10);

	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
	usleep(2 * 1000); /*msleep(4);*/

	/*Set Main clock */
	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 1,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_4MA),
		GPIO_CFG_ENABLE);

	if (s_ctrl->clk_rate != 0)
		cam_clk_info->clk_rate = s_ctrl->clk_rate;

	rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 1);
	if (rc < 0)
		pr_err("%s: clk enable failed\n", __func__);

	usleep(2 * 1000); /*msleep(2);*/

	/*reset VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(1000); /*msleep(1);*/

	/*standy VT */
	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);
	usleep(10);

	/*5M Core 1.2V - CAM_ISP_CORE_1P2*/
	gpio_tlmm_config(GPIO_CFG(GPIO_CAM_CORE_EN, 0, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	gpio_set_value_cansleep(GPIO_CAM_CORE_EN, 1);
	ret = gpio_get_value(GPIO_CAM_CORE_EN);
	if (ret)
		CAM_DEBUG("check CAM_CORE_EN : %d\n", ret);

	usleep(1000);
	
	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_3M_ISP_INIT : %d", temp);
	usleep(1000);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 1);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_3M_RST : %d", temp);
	usleep(50 * 1000);

	/* sensor validation test */
	CAM_DEBUG(" Camera Sensor Validation Test");
	rc = S5K4ECGX_WRITE_LIST(s5k4ecgx_init_reg1);
	if (rc < 0) {
		pr_info("Error in Camera Sensor Validation Test");
		return rc;
	}
	rc = S5K4ECGX_WRITE_LIST(s5k4ecgx_init_reg2);
	if (rc < 0) {
		pr_info("Error in Camera Sensor init setting");
		return rc;
	}
	config_csi2 = 0;
	s5k4ecgx_ctrl->op_mode = CAMERA_MODE_INIT;

	return rc;
}
#else
static int s5k4ecgx_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	printk(KERN_ERR "s5k4ecgx_sensor_power_up");
}
#endif

#if defined(CONFIG_S5K4ECGX) && defined(CONFIG_SR030PC50) /* CANE */
static int s5k4ecgx_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int temp = 0;

	struct msm_camera_sensor_info *data = s_ctrl->sensordata;

	CAM_DEBUG(" : E");
	if ( s5k4ecgx_ctrl->lightsensing_mode == 1 ) { // Lightsensing mode enabled
		s5k4ecgx_ctrl->lightsensing_mode = 0;
	}

#ifdef CONFIG_LOAD_FILE
	s5k4ecgx_regs_table_exit();
#endif

	CAM_DEBUG("set AF default mode at power-off");
	S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_1);
	msleep(100);
	S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_2);
	msleep(100);
	S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_3);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_stby);
	CAM_DEBUG("check VT standby : %d", temp);


	gpio_set_value_cansleep(data->sensor_platform_info->sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_reset);
	CAM_DEBUG("CAM_3M_RST : %d", temp);
	usleep(50);

	/*CAM_MCLK0*/
	msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
		cam_clk_info, s_ctrl->cam_clk, ARRAY_SIZE(cam_clk_info), 0);

	gpio_tlmm_config(GPIO_CFG(data->sensor_platform_info->mclk, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

	gpio_set_value_cansleep(data->sensor_platform_info->sensor_stby, 0);
	temp = gpio_get_value(data->sensor_platform_info->sensor_stby);
	CAM_DEBUG("CAM_3M_ISP_INIT : %d", temp);
	usleep(10);

	gpio_set_value_cansleep(data->sensor_platform_info->vt_sensor_reset, 0);
	temp = gpio_get_value(data->sensor_platform_info->vt_sensor_reset);
	CAM_DEBUG("check VT reset : %d", temp);
	usleep(10); /* 20clk = 0.833us */

	//data->sensor_platform_info->sensor_power_off(0);
	/*Sensor AF 2.8V -CAM_AF_2P8*/
	if (l11) {
		rc = regulator_disable(l11);
		if (rc)
			cam_err("error disabling regulator\n");
	}

	usleep(10);

	/*VT core 1.8 - CAM_DVDD_1P8*/
	if (l29) {
		rc = regulator_disable(l29);
		if (rc)
			cam_err("error disabling regulator\n");
	}

	usleep(10);

	/*Sensor AVDD 2.8V - CAM_SENSOR_A2P8 */
	if (l32) {
		rc = regulator_disable(l32);
		if (rc)
			cam_err("error disabling regulator\n");
	}
	usleep(10);

	/*Sensor IO 1.8V -CAM_SENSOR_IO_1P8  */
	if (l34) {
			rc = regulator_disable(l34);
			if (rc)
				cam_err("error disabling regulator\n");
	}
	usleep(10);

	/*5M Core 1.2V - CAM_ISP_CORE_1P2*/
	gpio_set_value_cansleep(GPIO_CAM_CORE_EN, 0);

	msm_camera_request_gpio_table(data, 0);

	return rc;
}
#else
static int s5k4ecgx_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	printk(KERN_ERR "s5k4ecgx_sensor_power_down");
}
#endif

static void s5k4ecgx_check_dataline(unsigned short val)
{
	if (val) {
		CAM_DEBUG("DTP ON");
		s5k4ecgx_ctrl->dtpTest = 1;

	} else {
		CAM_DEBUG("DTP OFF");
		s5k4ecgx_ctrl->dtpTest = 0;
	}
}

static int s5k4ecgx_set_ae_awb(int lock)
{
	if (s5k4ecgx_ctrl->touchaf_enable == 1){
		if (!lock) {
			if (s5k4ecgx_ctrl->settings.ae_awb_lock == 1) {
				CAM_DEBUG("AWB_AE_UNLOCK");
				if (s5k4ecgx_ctrl->settings.wb ==
					CAMERA_WHITE_BALANCE_AUTO) {
					S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_unlock);
					S5K4ECGX_WRITE_LIST(s5k4ecgx_awb_unlock);
				} else {
					S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_unlock);
				}
				s5k4ecgx_ctrl->settings.ae_awb_lock = 0;
			}
		}
		return 0;
	}

	if (lock) {
		if (s5k4ecgx_ctrl->settings.ae_awb_lock == 0) {
			CAM_DEBUG("AWB_AE_LOCK");
			if (s5k4ecgx_ctrl->settings.wb ==
				CAMERA_WHITE_BALANCE_AUTO) {
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_lock);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_awb_lock);
			} else {
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_lock);
			}
			s5k4ecgx_ctrl->settings.ae_awb_lock = 1;
		}
	} else {
		if (s5k4ecgx_ctrl->settings.ae_awb_lock == 1) {
			CAM_DEBUG("AWB_AE_UNLOCK");
			if (s5k4ecgx_ctrl->settings.wb ==
				CAMERA_WHITE_BALANCE_AUTO) {
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_unlock);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_awb_unlock);
			} else {
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ae_unlock);
			}
			s5k4ecgx_ctrl->settings.ae_awb_lock = 0;
		}
	}

	return 0;
}


static int s5k4ecgx_set_af_mode(int mode)
{
	CAM_DEBUG(" %d", mode);

	if (!mode_enable)
		goto focus_end;

	switch (mode) {
	case CAMERA_AF_AUTO: /*2*/
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_1);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_2);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_3);
		msleep(100);
		s5k4ecgx_ctrl->settings.focus_status = IN_AUTO_MODE;
		break;

	case CAMERA_AF_MACRO: /*1*/
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_1);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_2);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_3);
		msleep(100);
		s5k4ecgx_ctrl->settings.focus_status = IN_MACRO_MODE;
		break;

	default:
		CAM_DEBUG("default mode is auto");
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_1);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_2);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Normal_mode_3);
		msleep(100);
		break;
	}

focus_end:
	s5k4ecgx_ctrl->settings.focus_mode = mode;
	return 0;
}

static int s5k4ecgx_set_af_stop(int af_check)
{
	CAM_DEBUG(" %d", af_check);

	if (s5k4ecgx_get_flash_status()) {
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_Off);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_Off);
		s5k4ecgx_set_flash(FLASH_OFF);
	}

	s5k4ecgx_set_af_mode(s5k4ecgx_ctrl->settings.focus_mode);
	s5k4ecgx_ctrl->af_mode = SHUTTER_AF_MODE;

	return 0;
}

void s5k4ecgx_check_ae_stable(void)
{
	unsigned short ae_stable = 0;
	int ae_count = 0;
	
	msleep(200);
	while (ae_count++ < 5) {
		s5k4ecgx_sensor_write(0x002C, 0x7000);
		s5k4ecgx_sensor_write(0x002E, 0x2C74);
		s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&ae_stable);
		if (ae_stable == 0x01)
			break;
		msleep(100);
	}
	if (ae_count > 4) {
		CAM_DEBUG("AE NOT STABLE");
	}
}

int s5k4ecgx_set_af_status(int status, int initial_pos)
{
	int rc = 0;
	unsigned int cur_lux = 0;

	if (status == 1) {
		CAM_DEBUG("S5K4ECGX_AF_START");

		cur_lux = s5k4ecgx_get_light_level();
		CAM_DEBUG("AF light level = %d", cur_lux);
		if (cur_lux <= LOW_LIGHT_LEVEL) {
			CAM_DEBUG("LOW LUX AF ");
			af_low_lux = 1;
			s5k4ecgx_ctrl->lowLight = 1;
		} else {
			CAM_DEBUG("NORMAL LUX AF ");
			af_low_lux = 0;
			s5k4ecgx_ctrl->lowLight = 0;
		}
		if (s5k4ecgx_get_flash_status()) {
			S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_On);
			S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_On);
			s5k4ecgx_set_flash(MOVIE_FLASH);
			s5k4ecgx_check_ae_stable();
		}

		/*AE/AWB lock*/
		s5k4ecgx_set_ae_awb(1);
//	}else if (status == 2) {
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Single_AF_Start);
		if (af_low_lux) {
			CAM_DEBUG("200ms delay for Low Lux AF");
			if (s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_NIGHT
			|| s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_FIRE)
				msleep(700);
			else
				msleep(200);
		} else {
			msleep(200);
		}

	} else {
		CAM_DEBUG("S5K4ECGX_AF_ABORT\n");
/*
		if (initial_pos == 2) {
			S5K4ECGX_WRITE_LIST(s5k4ecgx_af_abort);
		} else if (initial_pos == 1) {
			s5k4ecgx_set_af_mode\
				(s5k4ecgx_ctrl->settings.focus_mode);
		} else {
			S5K4ECGX_WRITE_LIST(s5k4ecgx_af_abort);
			s5k4ecgx_set_af_mode\
				(s5k4ecgx_ctrl->settings.focus_mode);
		}
*/
		s5k4ecgx_set_af_mode(s5k4ecgx_ctrl->settings.focus_mode);
		/*AE/AWB unlock*/
		s5k4ecgx_set_ae_awb(0);

		af_low_lux = 0;

		s5k4ecgx_ctrl->touchaf_enable = false;
	}

	s5k4ecgx_ctrl->settings.focus_status = status;

	return rc;
}

int s5k4ecgx_reset_AF_region(void)
{
	u16 mapped_x = 320;
	u16 mapped_y = 240;
	u16 inner_window_start_x = 0;
	u16 inner_window_start_y = 0;
	u16 outer_window_start_x = 0;
	u16 outer_window_start_y = 0;

	s5k4ecgx_ctrl->touchaf_enable = false;

	mapped_x = (mapped_x * 640) / 666;
	mapped_y = (mapped_y * 480) / 500;

	inner_window_start_x    = mapped_x - (INNER_WINDOW_WIDTH_640_480 / 2);
	outer_window_start_x    = mapped_x - (OUTER_WINDOW_WIDTH_640_480 / 2);

	inner_window_start_y    = mapped_y - (INNER_WINDOW_HEIGHT_640_480 / 2);
	outer_window_start_y    = mapped_y - (OUTER_WINDOW_HEIGHT_640_480 / 2);

	/*calculate the start position value*/
	inner_window_start_x = inner_window_start_x * 640 / 640;
	outer_window_start_x = outer_window_start_x * 640 / 640;
	inner_window_start_y = inner_window_start_y * 640 / 480;
	outer_window_start_y = outer_window_start_y * 640 / 480;

	/*Write register*/
	s5k4ecgx_sensor_write(0x0028, 0x7000);

	/* inner_window_start_x */
	s5k4ecgx_sensor_write(0x002A, 0x029C);
	s5k4ecgx_sensor_write(0x0F12, inner_window_start_x);

	/* outer_window_start_x*/
	s5k4ecgx_sensor_write(0x002A, 0x0294);
	s5k4ecgx_sensor_write(0x0F12, outer_window_start_x);

	/* inner_window_start_y */
	s5k4ecgx_sensor_write(0x002A, 0x029E);
	s5k4ecgx_sensor_write(0x0F12, inner_window_start_y);

	/* outer_window_start_y */
	s5k4ecgx_sensor_write(0x002A, 0x0296);
	s5k4ecgx_sensor_write(0x0F12, outer_window_start_y);

	/* Update AF window*/
	s5k4ecgx_sensor_write(0x002A, 0x02A4);
	s5k4ecgx_sensor_write(0x0F12, 0x0001);

	return 0;
}

static void s5k4ecgx_touchaf_set_resolution(unsigned int addr, unsigned int value)
{
	s5k4ecgx_sensor_write(0xFCFC, 0xD000);
	s5k4ecgx_sensor_write(0x0028, 0x7000);
	s5k4ecgx_sensor_write(0x002A, addr);
	s5k4ecgx_sensor_write(0x0F12, value);
}

static int s5k4ecgx_set_touchaf_pos(int x, int y)
{

	static unsigned int inWindowWidth = 143;
	static unsigned int inWindowHeight = 143;
	static unsigned int outWindowWidth = 320;
	static unsigned int outWindowHeight = 266;

	unsigned int previewWidth;
	unsigned int previewHeight;

	s5k4ecgx_ctrl->touchaf_enable = true;

	if (s5k4ecgx_ctrl->settings.preview_size_idx == PREVIEW_SIZE_WVGA) {
		previewWidth = 800;
		previewHeight = 480;
	} else {
		previewWidth = 640;
		previewHeight = 480;
	}

	x = previewWidth - x;
	y = previewHeight - y;

	s5k4ecgx_sensor_write(0x002C, 0x7000);
	s5k4ecgx_sensor_write(0x002E, 0x02A0);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&inWindowWidth);
//	s5k4ecgx_sensor_write(0x002C, 0x7000);
//	s5k4ecgx_sensor_write(0x002E, 0x02A2);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&inWindowHeight);
	inWindowWidth = inWindowWidth * previewWidth / 1024;
	inWindowHeight = inWindowHeight * previewHeight / 1024;

	s5k4ecgx_sensor_write(0x002C, 0x7000);
	s5k4ecgx_sensor_write(0x002E, 0x0298);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&outWindowWidth);
//	s5k4ecgx_sensor_write(0x002C, 0x7000);
//	s5k4ecgx_sensor_write(0x002E, 0x029A);
	s5k4ecgx_sensor_read(0x0F12, (unsigned short *)&outWindowHeight);
	outWindowWidth = outWindowWidth * previewWidth / 1024;
	outWindowHeight = outWindowHeight * previewHeight / 1024;

	if (x < inWindowWidth/2)	
		x = inWindowWidth/2+1;
	else if (x > previewWidth - inWindowWidth/2)
		x = previewWidth - inWindowWidth/2 -1;
	if (y < inWindowHeight/2)	
		y = inWindowHeight/2+1;
	else if (y > previewHeight - inWindowHeight/2)
		y = previewHeight - inWindowHeight/2 -1;

	s5k4ecgx_touchaf_set_resolution(0x029C, (x - inWindowWidth/2) * 1024 / previewWidth);		
	s5k4ecgx_touchaf_set_resolution(0x029E, (y - inWindowHeight/2) * 1024 / previewHeight);		

	if (x < outWindowWidth/2)	
		x = outWindowWidth/2+1;
	else if (x > previewWidth - outWindowWidth/2)
		x = previewWidth - outWindowWidth/2 -1;
	if (y < outWindowHeight/2)	
		y = outWindowHeight/2+1;
	else if (y > previewHeight - outWindowHeight/2)
		y = previewHeight - outWindowHeight/2 -1;

	s5k4ecgx_touchaf_set_resolution(0x0294, (x - outWindowWidth/2) * 1024 / previewWidth);
	s5k4ecgx_touchaf_set_resolution(0x0296, (y - outWindowHeight/2) * 1024 / previewHeight);

	s5k4ecgx_touchaf_set_resolution(0x02A4, 0x0001);

	s5k4ecgx_ctrl->touchaf_enable = true;
	msleep(100);		// 1frame delay

	return 0;

}


int s5k4ecgx_get_af_status(int is_search_status)
{
	unsigned short af_status = 0;
	unsigned short return_af_status = 0;
	int cnt = 0;

	switch (is_search_status) {
	case 0:
		do {
			s5k4ecgx_sensor_write(0x002C, 0x7000);
			s5k4ecgx_sensor_write(0x002E, 0x2EEE);
			s5k4ecgx_sensor_read(0x0F12, &af_status);
			CAM_DEBUG("1st AF status : %x", af_status);
			if( cnt ++ > 100 )
				break;
			mdelay(50);
		} while (af_status == 1);
		if (af_status == 2) {
			do {
				s5k4ecgx_sensor_write(0x002C, 0x7000);
				s5k4ecgx_sensor_write(0x002E, 0x2207);
				s5k4ecgx_sensor_read(0x0F12, &af_status);
				CAM_DEBUG("2nd AF status : %x", af_status);
				if( cnt ++ > 100 )
					break;
				mdelay(50);
			} while (af_status != 0);
			if (af_status == 0) {	//AF success
				CAM_DEBUG("AF finished successfully");
				return_af_status = 1;
				s5k4ecgx_ctrl->af_mode = SHUTTER_AF_MODE;
				if(s5k4ecgx_ctrl->touchaf_enable == 1)
					s5k4ecgx_set_ae_awb(0);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_Off);
				if (s5k4ecgx_get_flash_status()) {
					S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_Off);
					s5k4ecgx_set_flash(FLASH_OFF);
				}
			} else {
				CAM_DEBUG("AF failed in problem");
				return_af_status = 2;
				if(s5k4ecgx_ctrl->touchaf_enable == 1)
					s5k4ecgx_set_ae_awb(0);
				S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_Off);
				if (s5k4ecgx_get_flash_status()) {
					S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_Off);
					s5k4ecgx_set_flash(FLASH_OFF);
				}
			}
		} else {
			CAM_DEBUG("1st AF failed");
			return_af_status = 2;
			s5k4ecgx_ctrl->af_mode = SHUTTER_AF_MODE;
			if(s5k4ecgx_ctrl->touchaf_enable == 1)
				s5k4ecgx_set_ae_awb(0);
			S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_Off);
			if (s5k4ecgx_get_flash_status()) {
				S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_Off);
				s5k4ecgx_set_flash(FLASH_OFF);
			}
		}
		break;
	default:
		CAM_DEBUG("unexpected mode is comming from hal\n");
		s5k4ecgx_set_ae_awb(0);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_FAST_AE_Off);
		if (s5k4ecgx_get_flash_status()) {
			S5K4ECGX_WRITE_LIST(s5k4ecgx_Pre_Flash_Off);
			s5k4ecgx_set_flash(FLASH_OFF);
			mdelay(100);
		}
		break;
	}

	if (s5k4ecgx_ctrl->settings.scene == CAMERA_SCENE_NIGHT)
		msleep(20);

	return  return_af_status;
}

static int s5k4ecgx_get_flash_status(void)
{
	int flash_status = 0;

	if (((s5k4ecgx_ctrl->settings.flash_mode == CAMERA_FLASH_AUTO) && (s5k4ecgx_ctrl->lowLight))
		|| (s5k4ecgx_ctrl->settings.flash_mode == CAMERA_FLASH_ON)) {
		flash_status = 1;
	}

	CAM_DEBUG(" %d", flash_status);

	return flash_status;
}

static void s5k4ecgx_set_flash(int mode)
{
	//int i = 0;

	cam_info(" %d", mode);

//	if (torchonoff > 0) {
//		printk(" [TorchOnOFF = %d] Do not control flash!\n",
//			torchonoff);
//		return;
//	}
	switch(mode) {
		case MOVIE_FLASH:
			CAM_DEBUG("MOVIE FLASH ON");
#if defined (CONFIG_LEDS_RT8547)
			rt8547_set_led_low();
#endif
			break;
		case CAPTURE_FLASH:
			CAM_DEBUG("CAPTURE FLASH ON");
#if defined (CONFIG_LEDS_RT8547)
			rt8547_set_led_high();
#endif
			break;
		case FLASH_OFF:
			CAM_DEBUG("FLASH OFF");
			if (s5k4ecgx_ctrl->settings.flash_state == CAPTURE_FLASH)
				S5K4ECGX_WRITE_LIST(s5k4ecgx_Main_Flash_Off);
#if defined (CONFIG_LEDS_RT8547)
			rt8547_set_led_off();
#endif
			break;
		default:
			CAM_DEBUG("UNKNOWN FLASH MODE");
			break;
	}
	s5k4ecgx_ctrl->settings.flash_state = mode;
}

static int s5k4ecgx_set_effect(int effect)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d", __func__, effect);
	if (!mode_enable)
		goto effect_end;

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Effect_Normal);
		break;

	case CAMERA_EFFECT_MONO:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Effect_Mono);
		break;

	case CAMERA_EFFECT_NEGATIVE:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Effect_Negative);
		break;

	case CAMERA_EFFECT_SEPIA:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Effect_Sepia);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] default effect");
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Effect_Normal);
		return 0;
	}

effect_end:
	s5k4ecgx_ctrl->settings.effect = effect;

	return 0;
}

static int s5k4ecgx_set_whitebalance(int wb)
{
	if (!mode_enable)
		goto whitebalance_end;

	switch (wb) {
	case CAMERA_WHITE_BALANCE_AUTO:
			S5K4ECGX_WRITE_LIST(s5k4ecgx_WB_Auto);
		break;

	case CAMERA_WHITE_BALANCE_INCANDESCENT:
			S5K4ECGX_WRITE_LIST(s5k4ecgx_WB_Tungsten);
		break;

	case CAMERA_WHITE_BALANCE_FLUORESCENT:
			S5K4ECGX_WRITE_LIST(s5k4ecgx_WB_Fluorescent);
		break;

	case CAMERA_WHITE_BALANCE_DAYLIGHT:
			S5K4ECGX_WRITE_LIST(s5k4ecgx_WB_Sunny);
		break;

	case CAMERA_WHITE_BALANCE_CLOUDY_DAYLIGHT:
			S5K4ECGX_WRITE_LIST(s5k4ecgx_WB_Cloudy);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected WB mode %s/%d",
			__func__, __LINE__);
		return 0;
	}

whitebalance_end:
	s5k4ecgx_ctrl->settings.wb = wb;

	return 0;
}

static void s5k4ecgx_set_ev(int ev)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d", __func__, ev);
	if (!mode_enable)
		goto exposure_end;

	switch (ev) {
	case CAMERA_EV_M4:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Minus_4);
		break;

	case CAMERA_EV_M3:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Minus_3);
		break;

	case CAMERA_EV_M2:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Minus_2);
		break;

	case CAMERA_EV_M1:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Minus_1);
		break;

	case CAMERA_EV_DEFAULT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Default);
		break;

	case CAMERA_EV_P1:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Plus_1);
		break;

	case CAMERA_EV_P2:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Plus_2);
		break;

	case CAMERA_EV_P3:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Plus_3);
		break;

	case CAMERA_EV_P4:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_EV_Plus_4);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected ev mode %s/%d",
			__func__, __LINE__);
		break;
	}

exposure_end:
	s5k4ecgx_ctrl->settings.brightness = ev;
}

static void s5k4ecgx_set_camcorder_ev(int ev)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d", __func__, ev);
	if (!mode_enable)
		goto exposure_end;

	switch (ev) {
	case CAMERA_EV_M4:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Minus_4);
		break;

	case CAMERA_EV_M3:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Minus_3);
		break;

	case CAMERA_EV_M2:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Minus_2);
		break;

	case CAMERA_EV_M1:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Minus_1);
		break;

	case CAMERA_EV_DEFAULT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Default);
		break;

	case CAMERA_EV_P1:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Plus_1);
		break;

	case CAMERA_EV_P2:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Plus_2);
		break;

	case CAMERA_EV_P3:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Plus_3);
		break;

	case CAMERA_EV_P4:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Camcorder_EV_Plus_4);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected ev mode %s/%d",
			__func__, __LINE__);
		break;
	}

exposure_end:
	s5k4ecgx_ctrl->settings.brightness = ev;
}

static void s5k4ecgx_set_scene_mode(int mode)
{
	CAM_DEBUG("mode = %d", mode);
	/* Fix for the gallery entrance the camera thinks the mode
	is same but the sensor is actually after restart so it needs
	to be set again
	if (s5k4ecgx_ctrl->settings.scene == mode) {
		CAM_DEBUG("Scene mode is Same the Before");
		return ;
	}
	*/
	if (s5k4ecgx_ctrl->cam_mode == MOVIE_MODE) {
		CAM_DEBUG("Not change to scene mode because of just chaging cam_mode");
		return ;
	}

	/* if the ae awb lock is aquired we need to take it off when the scene is changing */
	s5k4ecgx_set_ae_awb(0);

	switch (mode) {
	case CAMERA_SCENE_OFF:
	case CAMERA_SCENE_AUTO:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Default);
		break;

	case CAMERA_SCENE_LANDSCAPE:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Landscape);
		break;

	case CAMERA_SCENE_DAWN:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Duskdawn);
		break;

	case CAMERA_SCENE_BEACH:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Beach_Snow);
		break;

	case CAMERA_SCENE_SUNSET:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Sunset);
		break;

	case CAMERA_SCENE_NIGHT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Nightshot);
		break;

	case CAMERA_SCENE_PORTRAIT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Portrait);
		break;

	case CAMERA_SCENE_AGAINST_LIGHT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Backlight);
		break;

	case CAMERA_SCENE_SPORT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Sports);
		break;

	case CAMERA_SCENE_FALL:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Fall_Color);
		break;

	case CAMERA_SCENE_TEXT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Text);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_1);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_2);
		msleep(100);
		S5K4ECGX_WRITE_LIST(s5k4ecgx_AF_Macro_mode_3);
		msleep(100);
		break;

	case CAMERA_SCENE_CANDLE:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Candle_Light);
		break;

	case CAMERA_SCENE_FIRE:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Fireworks);
		break;

	case CAMERA_SCENE_PARTY:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Scene_Party_Indoor);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected scene mode %s/%d\n",
			  __func__, __LINE__);
		break;
	}

	s5k4ecgx_ctrl->settings.scene = mode;

}

static void s5k4ecgx_set_iso(int iso)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d\n", __func__, iso);

	if (s5k4ecgx_ctrl->settings.wb == CAMERA_WHITE_BALANCE_AUTO) {
		switch (iso) {
			case CAMERA_ISO_MODE_AUTO:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_Auto);
				break;

			case CAMERA_ISO_MODE_50:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_50);
				break;

			case CAMERA_ISO_MODE_100:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_100);
				break;

			case CAMERA_ISO_MODE_200:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_200);
				break;

			case CAMERA_ISO_MODE_400:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_400);
				break;

			default:
				cam_info(" iso : default");
				break;
		}
	} else {
		switch (iso) {
			case CAMERA_ISO_MODE_AUTO:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_Auto_MWB_on);
				break;

			case CAMERA_ISO_MODE_50:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_50_MWB_on);
				break;

			case CAMERA_ISO_MODE_100:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_100_MWB_on);
				break;

			case CAMERA_ISO_MODE_200:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_200_MWB_on);
				break;

			case CAMERA_ISO_MODE_400:
				S5K4ECGX_WRITE_LIST(s5k4ecgx_ISO_400_MWB_on);
				break;

			default:
				cam_info(" iso : default");
				break;
		}
	}

	s5k4ecgx_ctrl->settings.iso = iso;
}

static void s5k4ecgx_set_metering(int mode)
{
	CAM_DEBUG("[s5k4ecgx] %s : %d\n", __func__, mode);

	if (!mode_enable)
		goto metering_end;

	switch (mode) {
	case CAMERA_CENTER_WEIGHT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Metering_Center);
		break;

	case CAMERA_AVERAGE:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Metering_Matrix);
		break;

	case CAMERA_SPOT:
		S5K4ECGX_WRITE_LIST(s5k4ecgx_Metering_Spot);
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] unexpected metering mode %s/%d\n",
			  __func__, __LINE__);
		break;
	}

metering_end:
	s5k4ecgx_ctrl->settings.metering = mode;

}

void sensor_native_control(void __user *arg)
{
	struct ioctl_native_cmd ctrl_info;
	struct msm_camera_v4l2_ioctl_t *ioctl_ptr = arg;
	int rc = 0;

	/*if (copy_from_user((void *)&ctrl_info,
		(const void *)arg, sizeof(ctrl_info)))
		CAM_DEBUG(
			"[s5k4ecgx] %s fail copy_from_user!", __func__);*/
	if (copy_from_user(&ctrl_info,
		(void __user *)ioctl_ptr->ioctl_ptr,
		sizeof(ctrl_info))) {
		cam_err("fail copy_from_user!");
		goto FAIL_END;
	}

	CAM_DEBUG("[s5k4ecgx]  %s, ctrl_info.mode = %d\n", __func__, ctrl_info.mode);
	switch (ctrl_info.mode) {
	case EXT_CAM_FLASH_STATUS:  // 2
		CAM_DEBUG("EXT_CAM_FLASH_STATUS: %d", ctrl_info.value_1);
		s5k4ecgx_set_flash(ctrl_info.value_1);
		break;
		
	case EXT_CAM_FLASH_MODE:  // 3
		CAM_DEBUG("EXT_CAM_FLASH_MODE: %d", ctrl_info.value_1);
		s5k4ecgx_ctrl->settings.flash_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_EV: // 4
		CAM_DEBUG("EXT_CAM_EV: %d", ctrl_info.value_1);
		if (s5k4ecgx_ctrl->cam_mode == MOVIE_MODE)
			s5k4ecgx_set_camcorder_ev(ctrl_info.value_1);
		else
			s5k4ecgx_set_ev(ctrl_info.value_1);
		break;

	case EXT_CAM_SCENE_MODE: // 5
		CAM_DEBUG("EXT_CAM_SCENE_MODE: %d", ctrl_info.value_1);
		s5k4ecgx_set_scene_mode(ctrl_info.value_1);
		break;

	case EXT_CAM_ISO: // 6
		CAM_DEBUG("EXT_CAM_ISO: %d", ctrl_info.value_1);
		s5k4ecgx_set_iso(ctrl_info.value_1);
		break;

	case EXT_CAM_METERING: // 7
		CAM_DEBUG("EXT_CAM_METERING: %d", ctrl_info.value_1);
		s5k4ecgx_set_metering(ctrl_info.value_1);
		break;

	case EXT_CAM_WB: // 8
		CAM_DEBUG("EXT_CAM_WB: %d", ctrl_info.value_1);
		s5k4ecgx_set_whitebalance(ctrl_info.value_1);
		break;

	case EXT_CAM_EFFECT: // 9
		CAM_DEBUG("EXT_CAM_EFFECT: %d", ctrl_info.value_1);
		s5k4ecgx_set_effect(ctrl_info.value_1);
		break;

	case EXT_CAM_FOCUS: // 10
		CAM_DEBUG("EXT_CAM_FOCUS: %d", ctrl_info.value_1);
		s5k4ecgx_set_af_mode(ctrl_info.value_1);
		break;

	case EXT_CAM_PREVIEW_SIZE: // 11
		CAM_DEBUG("EXT_CAM_PREVIEW_SIZE: %d", ctrl_info.value_1);
		s5k4ecgx_set_preview_size(ctrl_info.value_1);
		break;

	case  EXT_CAM_MOVIE_MODE: // 12
		CAM_DEBUG("EXT_CAM_MOVIE_MODE: %d", ctrl_info.value_1);
		CAM_DEBUG("MOVIE mode : %d", ctrl_info.value_1);
		s5k4ecgx_ctrl->pre_cam_mode = s5k4ecgx_ctrl->cam_mode;
		s5k4ecgx_ctrl->cam_mode = ctrl_info.value_1;
		break;

	case EXT_CAM_DTP_TEST: // 13
		CAM_DEBUG("EXT_CAM_DTP_TEST: %d", ctrl_info.value_1);
		s5k4ecgx_check_dataline(ctrl_info.value_1);
		break;

	case EXT_CAM_SET_AF_STATUS: // 14
		CAM_DEBUG("EXT_CAM_SET_AF_STATUS: %d : %d", ctrl_info.value_1, ctrl_info.value_2);
		rc = s5k4ecgx_set_af_status(ctrl_info.value_1, ctrl_info.value_2);
		break;

	case EXT_CAM_GET_AF_STATUS: // 15
		CAM_DEBUG("EXT_CAM_GET_AF_STATUS: %d", ctrl_info.value_1);
		ctrl_info.value_1 = s5k4ecgx_get_af_status(ctrl_info.value_1);
		break;

	case EXT_CAM_SET_TOUCHAF_POS: // 17
		CAM_DEBUG("EXT_CAM_SET_TOUCHAF_POS: %d : %d", ctrl_info.value_1, ctrl_info.value_2);
		rc = s5k4ecgx_set_touchaf_pos(ctrl_info.value_1, ctrl_info.value_2);
		break;

	case EXT_CAM_SET_AE_AWB: // 18
		CAM_DEBUG("EXT_CAM_SET_AE_AWB: %d", ctrl_info.value_1);
		rc = s5k4ecgx_set_ae_awb(ctrl_info.value_1);
		break;

	case EXT_CAM_SET_AF_STOP: // 23
		CAM_DEBUG("EXT_CAM_SET_AF_STOP: %d", ctrl_info.value_1);
		s5k4ecgx_set_af_stop(ctrl_info.value_1);
		break;

	case EXT_CAM_EXIF:  // 27
		CAM_DEBUG("EXT_CAM_EXIF: %d", ctrl_info.value_1);
		ctrl_info.value_1 = s5k4ecgx_get_exif(ctrl_info.address);
		break;

	case EXT_CAM_SET_FPS:  // 31
		CAM_DEBUG("EXT_CAM_SET_FPS: %d", ctrl_info.value_1);
		if (s5k4ecgx_ctrl->cam_mode == MOVIE_MODE) {
			s5k4ecgx_set_frame_rate(ctrl_info.value_1);
			CAM_DEBUG("Camcorder mode FPS is %d", ctrl_info.value_1);
		} else {
			s5k4ecgx_set_frame_rate(0);
			CAM_DEBUG("Camera mode FPS is Auto");
		}
		break;

	case EXT_CAM_GET_FLASH_STATUS:  // 32
		CAM_DEBUG("EXT_GET_CAM_FLASH_STATUS: %d", ctrl_info.value_1);
		ctrl_info.value_1 = s5k4ecgx_get_flash_status();
		break;

	case EXT_CAM_LIGHTSENSING_MODE: // 47
		CAM_DEBUG("EXT_CAM_LIGHTSENSING_MODE: %d", ctrl_info.value_1);
		s5k4ecgx_ctrl->lightsensing_mode = ctrl_info.value_1;
		break;

	default:
		CAM_DEBUG("[s5k4ecgx] default mode");
		break;
	}

	/*if (copy_to_user((void *)arg,
		(const void *)&ctrl_info, sizeof(ctrl_info)))
		CAM_DEBUG("[s5k4ecgx] %s fail copy_to_user!", __func__);*/
	CAM_DEBUG("Before copy_to_user, ctrl_info.mode = %d\n", ctrl_info.mode);
	if (copy_to_user((void __user *)ioctl_ptr->ioctl_ptr,
		  (const void *)&ctrl_info,
			sizeof(ctrl_info))) {
		cam_err("fail copy_to_user!");
		goto FAIL_END;
	}	
	return ;
FAIL_END:
	cam_err("can't handle native control");
//	return rc;
}

long s5k4ecgx_sensor_subdev_ioctl(struct v4l2_subdev *sd,
			unsigned int cmd, void *arg)
{
	void __user *argp = (void __user *)arg;
	struct msm_sensor_ctrl_t *s5k4ecgx_s_ctrl = get_sctrl(sd);
	CAM_DEBUG("s5k4ecgx_sensor_subdev_ioctl\n");
	switch (cmd) {
	case VIDIOC_MSM_SENSOR_CFG:
		return s5k4ecgx_sensor_config(s5k4ecgx_s_ctrl, argp);
	case VIDIOC_MSM_SENSOR_CSID_INFO:
		{
		struct msm_sensor_csi_info *csi_info =
			(struct msm_sensor_csi_info *)arg;
		s5k4ecgx_s_ctrl->is_csic = csi_info->is_csic;
		return 0;
	}	
	default:
		return -ENOIOCTLCMD;
	}
}

int s5k4ecgx_sensor_config(struct msm_sensor_ctrl_t *s_ctrl, void __user *argp)
{
	struct sensor_cfg_data cfg_data;
	long   rc = 0;

	if (copy_from_user(&cfg_data, (void *)argp,
						sizeof(struct sensor_cfg_data)))
		return -EFAULT;

	CAM_DEBUG(" cfgtype = %d, mode = %d",
			cfg_data.cfgtype, cfg_data.mode);
		switch (cfg_data.cfgtype) {
		case CFG_SET_MODE:
			rc = s5k4ecgx_set_sensor_mode(cfg_data.mode);
			break;
		case CFG_SENSOR_INIT:
		if (config_csi2 == 0)
			rc = s5k4ecgx_sensor_setting(UPDATE_PERIODIC,
				RES_PREVIEW);
		break;	
		case CFG_GET_CSI_PARAMS:
		CAM_DEBUG("RInside CFG_GET_CSI_PARAMS");
			if (s_ctrl->func_tbl->sensor_get_csi_params == NULL) {
				CAM_DEBUG("cfgtype = %d, mode = %d\n",
			cfg_data.cfgtype, cfg_data.mode);
				rc = -EFAULT;
				break;
			}

			rc = s_ctrl->func_tbl->sensor_get_csi_params(
				s_ctrl,
				&cfg_data.cfg.csi_lane_params);
			CAM_DEBUG("Inside CFG_GET_CSI_PARAMS");
		 if (copy_to_user((void *)argp,
				 (const void *)&cfg_data,
				sizeof(cfg_data)))
				rc = -EFAULT;

			break;
		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = 0;
			cam_err(" Invalid cfgtype = %d", cfg_data.cfgtype);
			break;
		}
	return rc;
}


struct v4l2_subdev_info s5k4ecgx_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_YUYV8_2X8,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static int s5k4ecgx_enum_fmt(struct v4l2_subdev *sd, unsigned int index,
			   enum v4l2_mbus_pixelcode *code) {
	CAM_DEBUG("Index is %d", index);
	if ((unsigned int)index >= ARRAY_SIZE(s5k4ecgx_subdev_info))
		return -EINVAL;

	*code = s5k4ecgx_subdev_info[index].code;
	return 0;
}

static struct v4l2_subdev_core_ops s5k4ecgx_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = s5k4ecgx_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k4ecgx_subdev_video_ops = {
	.enum_mbus_fmt = s5k4ecgx_enum_fmt,
};

static struct v4l2_subdev_ops s5k4ecgx_subdev_ops = {
	.core = &s5k4ecgx_subdev_core_ops,
	.video  = &s5k4ecgx_subdev_video_ops,
};

static int s5k4ecgx_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;

	cam_err("%s_i2c_probe called", client->name);

	s5k4ecgx_ctrl = kzalloc(sizeof(struct s5k4ecgx_ctrl), GFP_KERNEL);
	if (!s5k4ecgx_ctrl) {
		CAM_DEBUG("s5k4ecgx_ctrl alloc failed!\n");
		rc  = -ENOMEM;
		goto probe_failure;
	}

	s5k4ecgx_exif = kzalloc(sizeof(struct s5k4ecgx_exif_data), GFP_KERNEL);
	if (!s5k4ecgx_exif) {
		CAM_DEBUG("Failed to allocate memory to EXIF structure!\n");
		rc = -ENOMEM;
		goto probe_failure;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		cam_err("i2c_check_functionality failed\n");
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		if (s_ctrl->sensor_i2c_addr != 0)
			s_ctrl->sensor_i2c_client->client->addr =
				s_ctrl->sensor_i2c_addr;
	} else {
		cam_err("s_ctrl->sensor_i2c_client is NULL\n");
		rc = -EFAULT;
		goto probe_failure;
	}

	s_ctrl->sensordata = client->dev.platform_data;
	if (s_ctrl->sensordata == NULL) {
		pr_err("%s: NULL sensor data\n", __func__);
		rc = -EFAULT;
		goto probe_failure;
	}

	s5k4ecgx_client = client;
	s5k4ecgx_dev = s_ctrl->sensor_i2c_client->client->dev;

	snprintf(s_ctrl->sensor_v4l2_subdev.name,
		sizeof(s_ctrl->sensor_v4l2_subdev.name), "%s", id->name);

	v4l2_i2c_subdev_init(&s_ctrl->sensor_v4l2_subdev, client,
		&s5k4ecgx_subdev_ops);

	s5k4ecgx_ctrl->s_ctrl = s_ctrl;
	s5k4ecgx_ctrl->sensor_dev = &s_ctrl->sensor_v4l2_subdev;
	s5k4ecgx_ctrl->sensordata = client->dev.platform_data;

	rc = msm_sensor_register(&s_ctrl->sensor_v4l2_subdev);
	if (rc) {
		CAM_DEBUG("msm_sensor_register failed\n");
		goto probe_failure;
		}

	cam_err("s5k4ecgx_probe succeeded!");
	return 0;

probe_failure:
	cam_err("s5k4ecgx_probe failed!");
	kfree(s5k4ecgx_ctrl);
	kfree(s5k4ecgx_exif);
	return rc;
}

static const struct i2c_device_id s5k4ecgx_i2c_id[] = {
	{"s5k4ecgx", (kernel_ulong_t)&s5k4ecgx_s_ctrl},
	{},
};

static struct msm_camera_i2c_client s5k4ecgx_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static struct i2c_driver s5k4ecgx_i2c_driver = {
	.id_table = s5k4ecgx_i2c_id,
	.probe  = s5k4ecgx_i2c_probe,
	.driver = {
		.name = "s5k4ecgx",
	},
};

static int __init s5k4ecgx_init(void)
{
	return i2c_add_driver(&s5k4ecgx_i2c_driver);
}

static struct msm_sensor_fn_t s5k4ecgx_func_tbl = {
	.sensor_config = s5k4ecgx_sensor_config,
	.sensor_power_up = s5k4ecgx_sensor_power_up,
	.sensor_power_down = s5k4ecgx_sensor_power_down,
	.sensor_match_id = s5k4ecgx_sensor_match_id,
	.sensor_adjust_frame_lines = msm_sensor_adjust_frame_lines1,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};


static struct msm_sensor_reg_t s5k4ecgx_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
};

static struct msm_sensor_ctrl_t s5k4ecgx_s_ctrl = {
	.msm_sensor_reg = &s5k4ecgx_regs,
	.sensor_i2c_client = &s5k4ecgx_sensor_i2c_client,
	.sensor_i2c_addr = 0xAC >> 1,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &s5k4ecgx_mut,
	.sensor_i2c_driver = &s5k4ecgx_i2c_driver,
	.sensor_v4l2_subdev_info = s5k4ecgx_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k4ecgx_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k4ecgx_subdev_ops,
	.func_tbl = &s5k4ecgx_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(s5k4ecgx_init);
