/*
 * Copyright (c) 2010 SAMSUNG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <mach/hardware.h>
#include <linux/wakelock.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/gp2ap030.h>
#include <linux/slab.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#include <linux/module.h>

/* for debugging */
#undef DEBUG

#define GP2A_VENDOR	"SHARP"
#define GP2A_CHIP_ID		"GP2AP030A00F"

#define PROX_READ_NUM	10

#define SENSOR_NAME "light_sensor"

#define SENSOR_DEFAULT_DELAY            (200)	/* 200 ms */
#define SENSOR_MAX_DELAY                (2000)	/* 2000 ms */

#define OFFSET_ARRAY_LENGTH		10
#define OFFSET_FILE_PATH	"/efs/prox_cal"

#define DEFAULT_LO_THRESHOLD    0x07 /* sharp recommanded Loff */
#define DEFAULT_HI_THRESHOLD    0x08 /* sharp recommanded Lon */

static struct i2c_client *opt_i2c_client = NULL;
static int lightval_logcount = 250;

struct opt_state {
	struct i2c_client *client;
};

struct gp2a_data {
	struct input_dev *proximity_input_dev;
	struct input_dev *light_input_dev;
	struct work_struct proximity_work;	/* for proximity sensor */
	struct mutex light_mutex;
	struct mutex data_mutex;
	struct delayed_work light_work;
	struct device *proximity_dev;
	struct device *light_dev;
	struct gp2a_platform_data *pdata;
	struct wake_lock prx_wake_lock;

	int proximity_enabled;
	int light_enabled;
	u8 lightsensor_mode;		/* 0 = low, 1 = high */
	bool light_data_first;
	int prox_data;
	int irq;
	int average[PROX_READ_NUM];	/*for proximity adc average */
	int light_delay;
	int testmode;
	int light_buffer;
	int light_count;
	int light_level_state;
	bool light_first_level;
	char proximity_detection;
	/* Auto Calibration */
	int offset_value;
	int cal_result;
	int threshold_high;
	int proximity_value;
	bool offset_cal_high;
};


/* initial value for sensor register */
#define COL 8
static u8 gp2a_original_image[COL][2] = {
	/*  {Regster, Value} */
	/*PRST :01(4 cycle at Detection/Non-detection),
	   ALSresolution :16bit, range *128   //0x1F -> 5F by sharp */
	{0x01, 0x63},
	/*ALC : 0, INTTYPE : 1, PS mode resolution : 12bit, range*1 */
	{0x02, 0x1A},
	/*LED drive current 110mA, Detection/Non-detection judgment output */
	{0x03, 0x3C},
	/*	{0x04 , 0x00}, */
	/*	{0x05 , 0x00}, */
	/*	{0x06 , 0xFF}, */
	/*	{0x07 , 0xFF}, */
#if defined(CONFIG_MACH_CRATER_CHN_CTC)
	{0x08, 0x06},		/*PS mode LTH(Loff):  (??mm) */
#else
	{0x08, 0x09},		/*PS mode LTH(Loff):  (??mm) */
#endif
	{0x09, 0x00},		/*PS mode LTH(Loff) : */
#if defined(CONFIG_MACH_CRATER_CHN_CTC)
	{0x0A, 0x08},		/*PS mode HTH(Lon) : (??mm) */
#else
	{0x0A, 0x0C},		/*PS mode HTH(Lon) : (??mm) */
#endif
	{0x0B, 0x00},		/* PS mode HTH(Lon) : */
	/* {0x13 , 0x08}, by sharp for internal calculation (type:0) */
	/*alternating mode (PS+ALS), TYPE=1
	   (0:externel 1:auto calculated mode) //umfa.cal */
	{0x00, 0xC0}
};

static int proximity_onoff(u8 onoff, struct gp2a_data *data);
static int lightsensor_get_adc(struct gp2a_data *data);
static int lightsensor_onoff(u8 onoff, struct gp2a_data *data);
static int lightsensor_get_adcvalue(struct gp2a_data *data);
static int opt_i2c_init(void);

static struct gp2a_data *gp2a_opt_data;

/* offset calibration interface */
static int proximity_open_offset(struct gp2a_data *data)
{
	struct file *offset_filp = NULL;
	int err = 0;
	mm_segment_t old_fs;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	offset_filp = filp_open(OFFSET_FILE_PATH, O_RDONLY, 0666);
	if (IS_ERR(offset_filp)) {
		printk(KERN_INFO "[GP2A] %s: no offset file\n", __func__);
		err = PTR_ERR(offset_filp);
		if (err != -ENOENT)
			pr_err("[GP2A] %s: Can't open cancelation file\n", __func__);
		set_fs(old_fs);
		return err;
	}

	err = offset_filp->f_op->read(offset_filp,
			(char *)&data->offset_value, sizeof(u8), &offset_filp->f_pos);
	if (err != sizeof(u8)) {
		pr_err("[GP2A] %s: Can't read the cancel data from file\n", __func__);
		err = -EIO;
	}

	printk(KERN_INFO "[GP2A]%s: offset_value = %d\n", __func__, data->offset_value);
	filp_close(offset_filp, current->files);
	set_fs(old_fs);

	return err;
}

static int proximity_adc_read(struct gp2a_data *gp2a)
{
	int sum[OFFSET_ARRAY_LENGTH];
	int i = 0;
	int avg = 0;
	int min = 0;
	int max = 0;
	int total = 0;
	int D2_data = 0;
	unsigned char get_D2_data[2]={0,};//test

	mutex_lock(&gp2a->data_mutex);
	for (i = 0; i < OFFSET_ARRAY_LENGTH; i++) {
		mdelay(50);
		opt_i2c_read(0x10, get_D2_data, sizeof(get_D2_data));
		D2_data =(get_D2_data[1] << 8) | get_D2_data[0];
		sum[i] = D2_data;
		if (i == 0) {
			min = sum[i];
			max = sum[i];
		} else {
			if (sum[i] < min)
				min = sum[i];
			else if (sum[i] > max)
				max = sum[i];
		}
		total += sum[i];
	}
	mutex_unlock(&gp2a->data_mutex);

	total -= (min + max);
	avg = (int)(total / (OFFSET_ARRAY_LENGTH - 2));
	printk(KERN_INFO "[GP2A] %s: avg = %d\n", __func__, avg);

	return avg;
}

static int proximity_store_offset(struct device *dev, bool do_calib)
{
	struct gp2a_data *gp2a = dev_get_drvdata(dev);
	struct file *offset_filp = NULL;
	mm_segment_t old_fs;
	int err = 0;
	int xtalk_avg = 0;
	int offset_change = 0;
	u8 thrd = 0;

	if (do_calib) {
		/* tap offset button */
		/* get offset value */
		xtalk_avg = proximity_adc_read(gp2a);
		offset_change = gp2a_original_image[5][1] - DEFAULT_HI_THRESHOLD;
		if(xtalk_avg < offset_change){ //don't need to calibrate
			/* calibration result */
			gp2a->cal_result = 0;
			return err;
		}
		gp2a->offset_value = xtalk_avg - offset_change;
		/* update threshold */
		thrd = gp2a_original_image[3][1]+(gp2a->offset_value);
		opt_i2c_write(gp2a_original_image[3][0], &thrd);
		thrd = gp2a_original_image[5][1]+(gp2a->offset_value);
		opt_i2c_write(gp2a_original_image[5][0], &thrd);
		/* calibration result */
		gp2a->cal_result = 1;
	}
	else{
		/* tap reset button */
		gp2a->offset_value = 0;
		 /* update threshold */
		opt_i2c_write(gp2a_original_image[3][0], &gp2a_original_image[3][1]);
		opt_i2c_write(gp2a_original_image[5][0], &gp2a_original_image[5][1]);
		/* calibration result */
		gp2a->cal_result = 2;
	}

        printk(KERN_INFO "[GP2A] %s : offset_value=%d\n",__func__, gp2a->offset_value);

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	offset_filp = filp_open(OFFSET_FILE_PATH, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (IS_ERR(offset_filp)) {
		pr_err("%s: Can't open prox_offset file\n", __func__);
		set_fs(old_fs);
		err = PTR_ERR(offset_filp);
		return err;
	}

	err = offset_filp->f_op->write(offset_filp,
			(char *)&gp2a->offset_value, sizeof(u8), &offset_filp->f_pos);
	if (err != sizeof(u8)) {
		pr_err("%s: Can't write the offset data to file\n", __func__);
		err = -EIO;
	}

	filp_close(offset_filp, current->files);
	set_fs(old_fs);
	return err;
}

static ssize_t proximity_cal_store(struct device *dev,
							struct device_attribute *attr,
							const char *buf, size_t size)
{
	bool do_calib;
	int err;

	if (sysfs_streq(buf, "1")) { /* calibrate cancelation value */
		do_calib = true;
	} else if (sysfs_streq(buf, "0")) { /* reset cancelation value */
		do_calib = false;
	} else {
		pr_err("%s: invalid value %d\n", __func__, *buf);
		return -EINVAL;
	}

        printk(KERN_INFO "[GP2A] %s : do_calib=%d\n",__func__, do_calib);

	err = proximity_store_offset(dev, do_calib);
	if (err < 0) {
		pr_err("%s: proximity_store_offset() failed\n", __func__);
		return err;
	}

	return size;
}

static ssize_t proximity_cal_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct gp2a_data *gp2a = dev_get_drvdata(dev);
	int thresh_hi = 0;
	unsigned char get_D2_data[2]={0,};

	msleep(20);
	opt_i2c_read(PS_HT_LSB, get_D2_data, sizeof(get_D2_data));
	thresh_hi =(get_D2_data[1] << 8) | get_D2_data[0];
	gp2a->threshold_high = thresh_hi;

        printk(KERN_INFO "[GP2A] %s : %d, %d\n",__func__, gp2a->offset_value, gp2a->threshold_high);
	return sprintf(buf, "%d,%d\n", gp2a->offset_value, gp2a->threshold_high);
}

static ssize_t prox_offset_pass_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct gp2a_data *gp2a = dev_get_drvdata(dev);

        printk(KERN_INFO "[GP2A] %s : cal_result=%d\n",__func__, gp2a->cal_result);
	return sprintf(buf, "%d\n", gp2a->cal_result);
}

static int gp2a_update_threshold(unsigned long new_threshold, bool update_reg)
{
	int i, err = 0;
	u8 set_value;

	for (i = 0; i < COL; i++) {
		switch (gp2a_original_image[i][0]) {
		case PS_LT_LSB:
			/*PS mode LTH(Loff) for low 8bit*/
			set_value = new_threshold & 0x00FF;
			break;

		case PS_LT_MSB:
			/*PS mode LTH(Loff) for high 8bit*/
			set_value = (new_threshold & 0xFF00) >> 8;
			break;

		case PS_HT_LSB:
			/*PS mode HTH(Lon) for low 8bit*/
			set_value = (new_threshold+1) & 0x00FF;
			break;

		case PS_HT_MSB:
			/* PS mode HTH(Lon) for high 8bit*/
			set_value = ((new_threshold+1) & 0xFF00) >> 8;
			break;

		default:
			continue;
		}

		if (update_reg)
			err = opt_i2c_write(gp2a_original_image[i][0], &set_value);

		if (err) {
			pr_err("%s : setting error i = %d, err=%d\n", __func__, i, err);
			return err;
		} else {
			gp2a_original_image[i][1] = set_value;
		}
	}

	return err;
}

static ssize_t proximity_thresh_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	int i;
	int threshold = 0;

	for (i = 0; i < COL; i++) {
		if (gp2a_original_image[i][0] == 0x08)
			/*PS mode LTH(Loff) */
			threshold = gp2a_original_image[i][1];
		else if (gp2a_original_image[i][0] == 0x09)
			/*PS mode LTH(Loff) */
			threshold |= gp2a_original_image[i][1]<<8;
	}

        printk(KERN_INFO "[GP2A] %s : threshold=%d\n",__func__, threshold);
	return sprintf(buf, "prox_threshold = %d\n", threshold);
}

static ssize_t proximity_thresh_store(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t size)
{
	unsigned long threshold;
	int err = 0;

	err = strict_strtoul(buf, 10, &threshold);

	if (err) {
		pr_err("%s, conversion %s to number.\n", __func__, buf);
		return err;
	}

        printk(KERN_INFO "[GP2A] %s : threshold=%ld\n",__func__, threshold);

	err = gp2a_update_threshold(threshold, true);

	if (err) {
		pr_err("gp2a threshold(with register) update fail.\n");
		return err;
	}

	return size;
}

static ssize_t
proximity_enable_show(struct device *dev,
		      struct device_attribute *attr, char *buf)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->proximity_enabled);
}

static ssize_t
proximity_enable_store(struct device *dev,
		       struct device_attribute *attr,
		       const char *buf, size_t count)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	int value = 0;
	int err = 0;

	err = kstrtoint(buf, 10, &value);

	if (err)
		pr_err("%s, kstrtoint failed.", __func__);

	if (value != 0 && value != 1)
		return count;

        printk(KERN_INFO "[GP2A] proximity_enable_store : value=%d, offset=%d\n", value, data->offset_value);

	if (data->proximity_enabled && !value) {	/* Prox power off */
		disable_irq_wake(data->irq);
		disable_irq(data->irq);

		proximity_onoff(0, data);
		#if defined (CONFIG_MACH_CRATER_CHN_CTC)
		if (data->pdata->gp2a_led_on && data->light_enabled!=1)
		#else
		if (data->pdata->gp2a_led_on)
		#endif
			data->pdata->gp2a_led_on(0);
	}
	if (!data->proximity_enabled && value) {	/* prox power on */
		if (data->pdata->gp2a_led_on) {
			data->pdata->gp2a_led_on(1);
                        msleep(5);
		}
		proximity_onoff(1, data);

		input_report_abs(data->proximity_input_dev, ABS_DISTANCE, 1);
		input_sync(data->proximity_input_dev);

		enable_irq_wake(data->irq);
		enable_irq(data->irq);
	}
	data->proximity_enabled = value;

	return count;
}


static ssize_t proximity_state_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{

	struct gp2a_data *data = dev_get_drvdata(dev);
	static int count;		/*count for proximity average */

	int D2_data = 0;
	unsigned char get_D2_data[2] = { 0, };

	mutex_lock(&data->data_mutex);
	opt_i2c_read(0x10, get_D2_data, sizeof(get_D2_data));
	mutex_unlock(&data->data_mutex);
	D2_data = (get_D2_data[1] << 8) | get_D2_data[0];

	data->average[count] = D2_data;
	count++;
	if (count == PROX_READ_NUM)
		count = 0;

	D2_data = D2_data - (data->offset_value); // for ADC compensation

	return snprintf(buf, PAGE_SIZE, "%d\n", D2_data);
}

static ssize_t proximity_avg_show(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	int min = 0, max = 0, avg = 0;
	int i;
	int proximity_value = 0;

	for (i = 0; i < PROX_READ_NUM; i++) {
		proximity_value = data->average[i];
		if (proximity_value > 0) {

			avg += proximity_value;

			if (!i)
				min = proximity_value;
			else if (proximity_value < min)
				min = proximity_value;

			if (proximity_value > max)
				max = proximity_value;
		}
	}
	avg /= i;

	return snprintf(buf, PAGE_SIZE, "%d, %d, %d\n", min, avg, max);
}

static ssize_t proximity_avg_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t size)
{
	return proximity_enable_store(dev, attr, buf, size);
}


/* Light Sysfs interface */
static ssize_t lightsensor_file_state_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	int adc = 0;
	struct gp2a_data *data = dev_get_drvdata(dev);

	adc = lightsensor_get_adcvalue(data);

	return snprintf(buf, PAGE_SIZE, "%d\n", adc);
}

static ssize_t lightsensor_raw_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	unsigned char get_data[4] = { 0, };
	int D0_raw_data;
	int D1_raw_data;
	int ret = 0;

	mutex_lock(&data->data_mutex);
	ret = opt_i2c_read(DATA0_LSB, get_data, sizeof(get_data));
	mutex_unlock(&data->data_mutex);
	if (ret < 0)
		pr_err("%s i2c err: %d\n", __func__, ret) ;
	D0_raw_data = (get_data[1] << 8) | get_data[0];	/* clear */
	D1_raw_data = (get_data[3] << 8) | get_data[2];	/* IR */

	return snprintf(buf, PAGE_SIZE, "%d,%d\n", D0_raw_data, D1_raw_data);
}

static ssize_t
light_delay_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->light_delay);
}

static ssize_t
light_delay_store(struct device *dev, struct device_attribute *attr,
		  const char *buf, size_t count)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	int delay;
	int err = 0;

	err = kstrtoint(buf, 10, &delay);

	if (err)
		pr_err("%s, kstrtoint failed.", __func__);

	if (delay < 0)
		return count;

	delay = delay / 1000000;	/* ns to msec */

        printk(KERN_INFO "[GP2A] poll_delay_store : new_delay=%d\n", delay);

	if (SENSOR_MAX_DELAY < delay)
		delay = SENSOR_MAX_DELAY;

	data->light_delay = delay;

	mutex_lock(&data->light_mutex);

	if (data->light_enabled) {
		cancel_delayed_work_sync(&data->light_work);
		schedule_delayed_work(&data->light_work,
			msecs_to_jiffies(delay));
	}

	mutex_unlock(&data->light_mutex);

	return count;
}

static ssize_t
light_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", data->light_enabled);
}

static ssize_t
light_enable_store(struct device *dev, struct device_attribute *attr,
		   const char *buf, size_t count)
{
	struct gp2a_data *data = dev_get_drvdata(dev);

	int value;
	int err = 0;

	err = kstrtoint(buf, 10, &value);

	if (err)
		pr_err("%s, kstrtoint failed.", __func__);

        printk(KERN_INFO "[GP2A] light_enable_store : value=%d\n", value);

	if (value != 0 && value != 1)
		return count;

	mutex_lock(&data->light_mutex);

	if (data->light_enabled && !value) {
		cancel_delayed_work_sync(&data->light_work);
		lightsensor_onoff(0, data);

#if defined (CONFIG_MACH_CRATER_CHN_CTC)
		if(data->proximity_enabled != 1)
#endif
		data->pdata->power_on(0);

	}
	if (!data->light_enabled && value) {

                data->pdata->power_on(1);
                msleep(5);

		data->light_data_first = true;
		lightsensor_onoff(1, data);
		schedule_delayed_work(&data->light_work, 300);
	}

	data->light_enabled = value;


	mutex_unlock(&data->light_mutex);

	return count;
}
static ssize_t gp2a_vendor_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", GP2A_VENDOR);
}
static ssize_t gp2a_name_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", GP2A_CHIP_ID);
}


static struct device_attribute dev_attr_proximity_enable =
	__ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
	       proximity_enable_show, proximity_enable_store);
static DEVICE_ATTR(vendor, S_IRUGO|S_IWUSR, gp2a_vendor_show, NULL);
static DEVICE_ATTR(name, S_IRUGO|S_IWUSR, gp2a_name_show, NULL);
static DEVICE_ATTR(prox_avg, S_IRUGO|S_IWUSR, proximity_avg_show, proximity_avg_store);
static DEVICE_ATTR(state, S_IRUGO|S_IWUSR, proximity_state_show, NULL);
static DEVICE_ATTR(prox_cal, S_IRUGO | S_IWUSR, proximity_cal_show, proximity_cal_store);
static DEVICE_ATTR(prox_offset_pass, S_IRUGO|S_IWUSR,prox_offset_pass_show, NULL);
static DEVICE_ATTR(prox_thresh, S_IRUGO | S_IWUSR, proximity_thresh_show, proximity_thresh_store);

static struct device_attribute dev_attr_light_enable =
	__ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
	       light_enable_show, light_enable_store);
static DEVICE_ATTR(lux, S_IRUGO|S_IWUSR, lightsensor_file_state_show, NULL);
static DEVICE_ATTR(raw_data, S_IRUGO, lightsensor_raw_show, NULL);
static DEVICE_ATTR(poll_delay, S_IRUGO|S_IWUSR|S_IWGRP, light_delay_show, light_delay_store);

static struct attribute *proximity_attributes[] = {
	&dev_attr_proximity_enable.attr,
	&dev_attr_state.attr,
	&dev_attr_prox_avg.attr,
	&dev_attr_prox_cal.attr,
	&dev_attr_prox_offset_pass.attr,
	&dev_attr_prox_thresh.attr,
	NULL
};

static struct attribute *lightsensor_attributes[] = {
	&dev_attr_poll_delay.attr,
	&dev_attr_light_enable.attr,
	NULL
};


static struct attribute_group proximity_attribute_group = {
	.attrs = proximity_attributes
};

static struct attribute_group lightsensor_attribute_group = {
	.attrs = lightsensor_attributes
};


irqreturn_t gp2a_irq_handler(int irq, void *dev_id)
{
	struct gp2a_data *gp2a = dev_id;

	printk(KERN_INFO "[GP2A] gp2a_irq_handler called\n");

	if (gp2a->irq != -1) {
	schedule_work(&gp2a->proximity_work);
	wake_lock_timeout(&gp2a->prx_wake_lock, 3 * HZ);
	}
	return IRQ_HANDLED;
}


static int gp2a_setup_irq(struct gp2a_data *gp2a)
{
	int rc = -EIO;
	struct gp2a_platform_data *pdata = gp2a->pdata;
	int irq;

	printk(KERN_INFO "[GP2A] %s, start\n", __func__);

	rc = gpio_request(pdata->p_out, "gpio_proximity_out");
	if (rc < 0) {
		pr_err("%s: gpio %d request failed (%d)\n",
		       __func__, pdata->p_out, rc);
		return rc;
	}

	rc = gpio_direction_input(pdata->p_out);
	if (rc < 0) {
		pr_err("%s: failed to set gpio %d as input (%d)\n",
		       __func__, pdata->p_out, rc);
		goto err_gpio_direction_input;
	}

	irq = gpio_to_irq(pdata->p_out);
	rc = request_threaded_irq(irq, NULL,
				  gp2a_irq_handler,
				  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND,
				  "proximity_int", gp2a);
	if (rc < 0) {
		pr_err("%s: request_irq(%d) failed for gpio %d (%d)\n",
		       __func__, irq, pdata->p_out, rc);
		goto err_request_irq;
	}

	printk(KERN_INFO "[GP2A] request_irq success IRQ_NO:%d, GPIO:%d", irq, pdata->p_out);

	/* start with interrupts disabled */
	disable_irq(irq);
	gp2a->irq = irq;

	goto done;

err_request_irq:
err_gpio_direction_input:
	gpio_free(pdata->p_out);
done:
	return rc;
}



static void gp2a_work_func_prox(struct work_struct *work)
{
	struct gp2a_data *gp2a = container_of((struct work_struct *)work,
					      struct gp2a_data, proximity_work);

	unsigned char value;
	char result;
	int ret;

	if (gp2a->irq != 0) {
		disable_irq_wake(gp2a->irq);
		disable_irq(gp2a->irq);
	}
	/* 0 : proximity, 1 : away */
        result = gpio_get_value(gp2a->pdata->p_out);
	gp2a->proximity_detection = !result;

	input_report_abs(gp2a->proximity_input_dev, ABS_DISTANCE, result);
	input_sync(gp2a->proximity_input_dev);

        printk(KERN_INFO "[GP2A] proximity value = %d \n",result);

	value = 0x0C;
	ret = opt_i2c_write(COMMAND1, &value);	/*Software reset */

	if (result == 0) {	/* detection = Falling Edge */
		if (gp2a->lightsensor_mode == 0)	/* Low mode */
			value = 0x23;
		else		/* High mode */
			value = 0x27;
		ret = opt_i2c_write(COMMAND2, &value);
	} else {		/* none Detection */
		if (gp2a->lightsensor_mode == 0)	/* Low mode */
			value = 0x63;
		else		/* High mode */
			value = 0x67;
		ret = opt_i2c_write(COMMAND2, &value);
	}

	if (gp2a->irq != 0) {
	enable_irq(gp2a->irq);
		enable_irq_wake(gp2a->irq);
	}
	value = 0xCC;
	ret = opt_i2c_write(COMMAND1, &value);

	gp2a->prox_data = result;

}


int opt_i2c_read(u8 reg, unsigned char *rbuf, int len)
{
	int ret = -1;
	struct i2c_msg msg;

	msg.addr = opt_i2c_client->addr;
	msg.flags = 0;
	msg.len = 1;
	msg.buf = &reg;

	ret = i2c_transfer(opt_i2c_client->adapter, &msg, 1);

	if (ret >= 0) {
		msg.flags = I2C_M_RD;
		msg.len = len;
		msg.buf = rbuf;
		ret = i2c_transfer(opt_i2c_client->adapter, &msg, 1);
	}

	if (ret < 0)
		pr_err("i2c transfer error ret=%d\n", ret);

	return ret;
}

int opt_i2c_write(u8 reg, u8 *val)
{
	int err = 0;
	struct i2c_msg msg[1];
	unsigned char data[2];
	int retry = 10;

	if( (opt_i2c_client == NULL) || (!opt_i2c_client->adapter) ){
	    return -ENODEV;
	}

	while (retry--) {
		data[0] = reg;
		data[1] = *val;

		msg->addr = opt_i2c_client->addr;
		msg->flags = 0;
		msg->len = 2;
		msg->buf = data;

		err = i2c_transfer(opt_i2c_client->adapter, msg, 1);

		if (err >= 0)
			return 0;
	}
	pr_err(" i2c transfer error(%d)\n", err);
	return err;
}

static int proximity_input_init(struct gp2a_data *data)
{
	struct input_dev *dev;
	int err = 0;

	pr_info("%s, %d start\n", __func__, __LINE__);

	dev = input_allocate_device();
	if (!dev) {
		pr_err("%s, error\n", __func__);
		return -ENOMEM;
	}

	input_set_capability(dev, EV_ABS, ABS_DISTANCE);
	input_set_abs_params(dev, ABS_DISTANCE, 0, 1, 0, 0);

	dev->name = "proximity_sensor";
	input_set_drvdata(dev, data);

	err = input_register_device(dev);
	if (err < 0) {
		input_free_device(dev);
		return err;
	}
	data->proximity_input_dev = dev;

	printk(KERN_INFO "[GP2A] %s, success\n", __func__);
	return 0;
}

static int light_input_init(struct gp2a_data *data)
{
	struct input_dev *dev;
	int err = 0;

	pr_info("%s, %d start\n", __func__, __LINE__);

	dev = input_allocate_device();
	if (!dev) {
		pr_err("%s, error\n", __func__);
		return -ENOMEM;
	}

	input_set_capability(dev, EV_ABS, ABS_MISC);
	input_set_abs_params(dev, ABS_MISC, 0, 1, 0, 0);

	dev->name = "light_sensor";
	input_set_drvdata(dev, data);

	err = input_register_device(dev);
	if (err < 0) {
		input_free_device(dev);
		return err;
	}
	data->light_input_dev = dev;

	printk(KERN_INFO "[GP2A] %s, success\n", __func__);
	return 0;
}

int lightsensor_get_adc(struct gp2a_data *data)
{
	unsigned char get_data[4] = { 0, };
	int D0_raw_data;
	int D1_raw_data;
	int D0_data;
	int D1_data;
	int lx = 0;
	u8 value;
	int light_alpha;
	int light_beta;
	static int lx_prev;
	int ret = 0;
	int d0_boundary = 91;
	mutex_lock(&data->data_mutex);
	ret = opt_i2c_read(DATA0_LSB, get_data, sizeof(get_data));
	mutex_unlock(&data->data_mutex);
	if (ret < 0)
		return lx_prev;
	D0_raw_data = (get_data[1] << 8) | get_data[0];	/* clear */
	D1_raw_data = (get_data[3] << 8) | get_data[2];	/* IR */

	// Result of baffin tunning
	if (100 * D1_raw_data <= 41 * D0_raw_data) {
		light_alpha = 830;
		light_beta = 0;
	} else if (100 * D1_raw_data <= 62 * D0_raw_data) {
		light_alpha = 2039;
		light_beta = 2949;
	} else if (100 * D1_raw_data <= d0_boundary * D0_raw_data) {
		light_alpha = 649;
		light_beta = 708;
	} else {
		light_alpha = 0;
		light_beta = 0;
	}


	if (data->lightsensor_mode) {	/* HIGH_MODE */
		D0_data = D0_raw_data * 16;
		D1_data = D1_raw_data * 16;
	} else {		/* LOW_MODE */
		D0_data = D0_raw_data;
		D1_data = D1_raw_data;
	}

	if (D0_data < 3) {
		lx = 0;
	} else if (data->lightsensor_mode == 0
		   && (D0_raw_data >= 16000 || D1_raw_data >= 16000)
		   && (D0_raw_data <= 16383 && D1_raw_data <= 16383)) {
		lx = lx_prev;
	} else if (100 * D1_data > d0_boundary * D0_data) {

		lx = lx_prev;
		return lx;
	} else {
		lx = (int)((light_alpha / 10 * D0_data * 33)
			- (light_beta / 10 * D1_data * 33)) / 1000;
	}

	lx_prev = lx;

	if (data->lightsensor_mode) {	/* HIGH MODE */
		if (D0_raw_data < 1000) {
			pr_info("%s: change to LOW_MODE detection=%d\n",
			       __func__, data->proximity_detection);
			data->lightsensor_mode = 0;	/* change to LOW MODE */

			value = 0x0C;
			opt_i2c_write(COMMAND1, &value);

			if (data->proximity_detection)
				value = 0x23;
			else
				value = 0x63;

			opt_i2c_write(COMMAND2, &value);

			if (data->proximity_enabled)
				value = 0xCC;
			else
				value = 0xDC;

			opt_i2c_write(COMMAND1, &value);
		}
	} else {		/* LOW MODE */
		if (D0_raw_data > 16000 || D1_raw_data > 16000) {
			pr_info("%s: change to HIGH_MODE detection=%d\n",
			       __func__, data->proximity_detection);
			/* change to HIGH MODE */
			data->lightsensor_mode = 1;

			value = 0x0C;
			opt_i2c_write(COMMAND1, &value);

			if (data->proximity_detection)
				value = 0x27;
			else
				value = 0x67;
			opt_i2c_write(COMMAND2, &value);

			if (data->proximity_enabled)
				value = 0xCC;
			else
				value = 0xDC;
			opt_i2c_write(COMMAND1, &value);
		}
	}

	return lx;
}

static int lightsensor_get_adcvalue(struct gp2a_data *data)
{
#if 0 //disable average
	int i = 0, j = 0;
	unsigned int adc_total = 0;
	static int adc_avr_value;
	unsigned int adc_index = 0;
	static unsigned int adc_index_count;
	unsigned int adc_max = 0;
	unsigned int adc_min = 0;
	int value = 0;
	static int adc_value_buf[ADC_BUFFER_NUM] = { 0 };
#else
	int value = 0;
#endif
	value = lightsensor_get_adc(data);

#if 0 //disable average
	/*cur_adc_value = value; */

	adc_index = (adc_index_count++) % ADC_BUFFER_NUM;

	/*ADC buffer initialize (light sensor off ---> light sensor on) */
	if (data->light_data_first) {
		for (j = 0; j < ADC_BUFFER_NUM; j++)
			adc_value_buf[j] = value;
		data->light_data_first = false;
	} else {
		adc_value_buf[adc_index] = value;
	}

	adc_max = adc_value_buf[0];
	adc_min = adc_value_buf[0];

	for (i = 0; i < ADC_BUFFER_NUM; i++) {
		adc_total += adc_value_buf[i];

		if (adc_max < adc_value_buf[i])
			adc_max = adc_value_buf[i];

		if (adc_min > adc_value_buf[i])
			adc_min = adc_value_buf[i];
	}
	adc_avr_value =
	    (adc_total - (adc_max + adc_min)) / (ADC_BUFFER_NUM - 2);

	if (adc_index_count == ADC_BUFFER_NUM - 1)
		adc_index_count = 0;

	return adc_avr_value;
#else
	return value;
#endif
}

static int lightsensor_onoff(u8 onoff, struct gp2a_data *data)
{
	u8 value;

	if (onoff) {
		/*in calling, must turn on proximity sensor */
		if (data->proximity_enabled == 0) {
			value = 0x01;
			opt_i2c_write(COMMAND4, &value);
			value = 0x63;
			opt_i2c_write(COMMAND2, &value);
			/*OP3 : 1(operating mode) OP2 :1
			   (coutinuous operating mode)
			   OP1 : 01(ALS mode) TYPE=0(auto) */
			value = 0xD0;
			opt_i2c_write(COMMAND1, &value);
			/* other setting have defualt value. */
		}
	} else {
		/*in calling, must turn on proximity sensor */
		if (data->proximity_enabled == 0) {
			value = 0x00;	/*shutdown mode */
			opt_i2c_write((u8) (COMMAND1), &value);
		}
	}

	return 0;
}



static void gp2a_work_func_light(struct work_struct *work)
{
	struct gp2a_data *data = container_of((struct delayed_work *)work,
						struct gp2a_data, light_work);

	int adc = 0;
#if defined(CONFIG_MACH_BAFFIN_DUOS_CTC)
	volatile static int count = 0;
#endif
	adc = lightsensor_get_adcvalue(data);

#if defined(CONFIG_MACH_BAFFIN_DUOS_CTC)
	if(adc == 0)
	{
		count++;
		if(count == 18) //detecting 0 after 3.6sec,  set the register again.
		{
			printk(KERN_INFO" [GP2A]: add for ESD \n");
			lightsensor_onoff(1,data);
			count = 0;
		}
	}
#endif
	input_report_abs(data->light_input_dev, ABS_MISC, adc);
	input_sync(data->light_input_dev);


	if (lightval_logcount++ > 250) {
		printk(KERN_INFO "[GP2A] light value = %d \n", adc);
		lightval_logcount = 0;
	}

	if (data->light_enabled)
		schedule_delayed_work(&data->light_work,
			msecs_to_jiffies(data->light_delay));
}


static int gp2a_opt_open(struct inode *ip, struct file *fp)
{
	return nonseekable_open(ip, fp);
}

static int gp2a_opt_release(struct inode *ip, struct file *fp)
{
	return 0;
}

static long gp2a_opt_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{
    int ret = 0;
    short data = 0;
    u8 thrd = 0;

    switch (cmd)
    {
	case PROX_IOC_SET_CALIBRATION:
        {
		printk(KERN_INFO "[GP2A] PROX_IOC_SET_CALIBRATION\n");
		if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
			return -EFAULT;

		ret = proximity_open_offset(gp2a_opt_data);
		if (ret < 0 && ret != -ENOENT)
		{
			printk(KERN_INFO "[GP2A] proximity_open_offset() failed\n");
		}else {
			thrd = gp2a_original_image[3][1]+(gp2a_opt_data->offset_value);
			opt_i2c_write(gp2a_original_image[3][0], &thrd);
			thrd = gp2a_original_image[5][1]+(gp2a_opt_data->offset_value);
			opt_i2c_write(gp2a_original_image[5][0], &thrd);
		}
		break;
	}
	case PROX_IOC_GET_CALIBRATION:
        {
		printk(KERN_INFO "[GP2A] PROX_IOC_GET_CALIBRATION\n");
                data = gp2a_opt_data->offset_value;
		if (copy_to_user((void __user *)arg, &data, sizeof(data)))
			return -EFAULT;
		break;
	}
	default:
            printk(KERN_ERR "Unknown IOCTL command");
            ret = -ENOTTY;
            break;
    }
    return ret;
}


static struct file_operations gp2a_opt_fops = {
	.owner = THIS_MODULE,
	.open = gp2a_opt_open,
	.release = gp2a_opt_release,
	.unlocked_ioctl = gp2a_opt_ioctl,
};

static struct miscdevice gp2a_opt_misc_device = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "proximity",
    .fops   = &gp2a_opt_fops,
};

static int gp2a_opt_probe(struct platform_device *pdev)
{
	struct gp2a_data *gp2a;
	struct gp2a_platform_data *pdata = pdev->dev.platform_data;
	u8 value = 0;
	int err = 0;

	printk(KERN_INFO"[GP2A] %s : probe start!\n", __func__);

	if (!pdata) {
		pr_err("%s: missing pdata!\n", __func__);
		return err;
	}

        /*PROXY_EN*/
	if (gpio_request(pdata ->power_gpio, "PROXY_EN")) {
		printk(KERN_ERR "Request GPIO_%d failed!\n", pdata ->power_gpio);
	}

	if (pdata->gp2a_led_on)
		pdata->gp2a_led_on(1);

#ifndef CONFIG_MACH_CRATER_CHN_CTC
	if (pdata->gp2a_get_threshold) {
		gp2a_update_threshold(pdata->gp2a_get_threshold(), false);
	}
#endif

	/* allocate driver_data */
	gp2a = kzalloc(sizeof(struct gp2a_data), GFP_KERNEL);
	if (!gp2a) {
		pr_err("kzalloc error\n");
		return -ENOMEM;
	}

	gp2a->proximity_enabled = 0;
	gp2a->pdata = pdata;

	gp2a->light_enabled = 0;
	gp2a->light_delay = SENSOR_DEFAULT_DELAY;
	gp2a->testmode = 0;
	gp2a->light_level_state = 0;

	if (pdata->power_on) {
		pdata->power_on(1);
		msleep(5);
	}

	INIT_DELAYED_WORK(&gp2a->light_work, gp2a_work_func_light);
	INIT_WORK(&gp2a->proximity_work, gp2a_work_func_prox);

	/*misc device registration*/
        err = misc_register(&gp2a_opt_misc_device);
	if( err < 0 )
		goto error_setup_reg_misc;

	err = proximity_input_init(gp2a);
	if (err < 0)
		goto error_setup_reg_prox;

	err = light_input_init(gp2a);
	if (err < 0)
		goto error_setup_reg_light;

	err = sysfs_create_group(&gp2a->proximity_input_dev->dev.kobj,
				 &proximity_attribute_group);
	if (err < 0)
		goto err_sysfs_create_group_proximity;

	err = sysfs_create_group(&gp2a->light_input_dev->dev.kobj,
				&lightsensor_attribute_group);
	if (err)
		goto err_sysfs_create_group_light;

	mutex_init(&gp2a->light_mutex);
	mutex_init(&gp2a->data_mutex);

	/* set platdata */
	platform_set_drvdata(pdev, gp2a);

	/* wake lock init */
	wake_lock_init(&gp2a->prx_wake_lock, WAKE_LOCK_SUSPEND,
		"prx_wake_lock");

	/* init i2c */
	err = opt_i2c_init();
	if(err < 0)
	{
		pr_err("opt_probe failed : i2c_client is NULL\n");
		goto err_no_device;
	}
	else
		printk(KERN_INFO "[GP2A] opt_i2c_client : (0x%p)\n",opt_i2c_client);

	/* GP2A Regs INIT SETTINGS  and Check I2C communication */

	value = 0x00;
	err = opt_i2c_write((u8) (COMMAND1), &value);	/* shutdown mode op[3]=0 */
	if (err < 0) {
		pr_err("%s failed : threre is no such device.\n", __func__);
		goto err_no_device;
	}

	/* Setup irq */
	err = gp2a_setup_irq(gp2a);
	if (err) {
		pr_err("%s: could not setup irq\n", __func__);
		goto err_setup_irq;
	}

	/* set sysfs for proximity sensor */
	gp2a->proximity_dev = device_create(sensors_class,
					    NULL, 0, NULL, "proximity_sensor");
	if (IS_ERR(gp2a->proximity_dev)) {
		pr_err("%s: could not create proximity_dev\n", __func__);
		goto err_proximity_device_create;
	}

	gp2a->light_dev = device_create(sensors_class,
					NULL, 0, NULL, "light_sensor");
	if (IS_ERR(gp2a->light_dev)) {
		pr_err("%s: could not create light_dev\n", __func__);
		goto err_light_device_create;
	}


	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_state) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_state.attr.name);
		goto err_proximity_device_create_file1;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_prox_avg) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_prox_avg.attr.name);
		goto err_proximity_device_create_file2;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_proximity_enable) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_proximity_enable.attr.name);
		goto err_proximity_device_create_file3;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_vendor) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_vendor.attr.name);
		goto err_proximity_device_create_file4;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_name) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_name.attr.name);
		goto err_proximity_device_create_file5;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_prox_cal) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
			dev_attr_prox_cal.attr.name);
		goto err_proximity_device_create_file6;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_prox_offset_pass) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
			dev_attr_prox_offset_pass.attr.name);
		goto err_proximity_device_create_file7;
	}

	if (device_create_file(gp2a->proximity_dev,
		&dev_attr_prox_thresh) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
			dev_attr_prox_thresh.attr.name);
		goto err_proximity_device_create_file8;
	}

	if (device_create_file(gp2a->light_dev,
		&dev_attr_lux) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_lux.attr.name);
		goto err_light_device_create_file1;
	}

	if (device_create_file(gp2a->light_dev,
		&dev_attr_light_enable) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_light_enable.attr.name);
		goto err_light_device_create_file2;
	}

	if (device_create_file(gp2a->light_dev,
		&dev_attr_vendor) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_vendor.attr.name);
		goto err_light_device_create_file3;
	}

	if (device_create_file(gp2a->light_dev,
		&dev_attr_name) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_name.attr.name);
		goto err_light_device_create_file4;
	}

	if (device_create_file(gp2a->light_dev,
		&dev_attr_raw_data) < 0) {
		pr_err("%s: could not create device file(%s)!\n", __func__,
		       dev_attr_raw_data.attr.name);
		goto err_light_device_create_file5;
	}

	dev_set_drvdata(gp2a->proximity_dev, gp2a);
	dev_set_drvdata(gp2a->light_dev, gp2a);

	device_init_wakeup(&pdev->dev, 1);

	if (pdata->gp2a_led_on) {

			pdata->gp2a_led_on(0);
            printk(KERN_INFO "[GP2A] gpio_get_value of GPIO(%d) is %d\n",pdata ->power_gpio,
                gpio_get_value(pdata ->power_gpio));
	}

	/* set initial proximity value as 1 */
	input_report_abs(gp2a->proximity_input_dev, ABS_DISTANCE, 1);
	input_sync(gp2a->proximity_input_dev);

        gp2a_opt_data = gp2a;

	printk(KERN_INFO"[GP2A] %s : probe success!\n", __func__);

	return 0;

err_light_device_create_file5:
	device_remove_file(gp2a->light_dev, &dev_attr_name);
err_light_device_create_file4:
	device_remove_file(gp2a->light_dev, &dev_attr_vendor);
err_light_device_create_file3:
	device_remove_file(gp2a->light_dev, &dev_attr_light_enable);
err_light_device_create_file2:
	device_remove_file(gp2a->light_dev, &dev_attr_lux);
err_light_device_create_file1:
	device_remove_file(gp2a->proximity_dev, &dev_attr_prox_thresh);
err_proximity_device_create_file8:
	device_remove_file(gp2a->proximity_dev, &dev_attr_prox_offset_pass);
err_proximity_device_create_file7:
	device_remove_file(gp2a->proximity_dev, &dev_attr_prox_cal);
err_proximity_device_create_file6:
	device_remove_file(gp2a->proximity_dev, &dev_attr_name);
err_proximity_device_create_file5:
	device_remove_file(gp2a->proximity_dev, &dev_attr_vendor);
err_proximity_device_create_file4:
	device_remove_file(gp2a->proximity_dev, &dev_attr_proximity_enable);
err_proximity_device_create_file3:
	device_remove_file(gp2a->proximity_dev, &dev_attr_prox_avg);
err_proximity_device_create_file2:
	device_remove_file(gp2a->proximity_dev, &dev_attr_state);
err_proximity_device_create_file1:
err_light_device_create:
	device_destroy(sensors_class, 0);
err_proximity_device_create:
	gpio_free(pdata->p_out);
err_setup_irq:
err_no_device:
	wake_lock_destroy(&gp2a->prx_wake_lock);
	mutex_destroy(&gp2a->light_mutex);
	mutex_destroy(&gp2a->data_mutex);
	sysfs_remove_group(&gp2a->light_input_dev->dev.kobj,
			   &lightsensor_attribute_group);
err_sysfs_create_group_light:
	sysfs_remove_group(&gp2a->proximity_input_dev->dev.kobj,
			   &proximity_attribute_group);
err_sysfs_create_group_proximity:
	input_unregister_device(gp2a->light_input_dev);
error_setup_reg_light:
	input_unregister_device(gp2a->proximity_input_dev);
error_setup_reg_prox:
	misc_deregister(&gp2a_opt_misc_device);
error_setup_reg_misc:
	if (pdata->power_on)
		pdata->power_on(0);
	kfree(gp2a);
	return err;
}

static int gp2a_opt_remove(struct platform_device *pdev)
{
	struct gp2a_data *gp2a = platform_get_drvdata(pdev);

	if (gp2a == NULL) {
		pr_err("%s, gp2a_data is NULL!!!!!\n", __func__);
		return 0;
	}

	if (sensors_class != NULL) {
		device_remove_file(gp2a->proximity_dev, &dev_attr_prox_avg);
		device_remove_file(gp2a->proximity_dev, &dev_attr_state);
		device_remove_file(gp2a->proximity_dev,
			&dev_attr_proximity_enable);
		device_remove_file(gp2a->proximity_dev, &dev_attr_name);
		device_remove_file(gp2a->proximity_dev, &dev_attr_vendor);
		device_remove_file(gp2a->light_dev, &dev_attr_lux);
		device_remove_file(gp2a->light_dev, &dev_attr_light_enable);
		device_remove_file(gp2a->light_dev, &dev_attr_raw_data);
		device_remove_file(gp2a->light_dev, &dev_attr_name);
		device_remove_file(gp2a->light_dev, &dev_attr_vendor);
		device_destroy(sensors_class, 0);
	}

	if (gp2a->proximity_input_dev != NULL) {
		sysfs_remove_group(&gp2a->proximity_input_dev->dev.kobj,
				   &proximity_attribute_group);
		input_unregister_device(gp2a->proximity_input_dev);

		if (gp2a->proximity_input_dev != NULL)
			kfree(gp2a->proximity_input_dev);
	}

	cancel_delayed_work(&gp2a->light_work);
	flush_scheduled_work();
	mutex_destroy(&gp2a->light_mutex);

	if (gp2a->light_input_dev != NULL) {
		sysfs_remove_group(&gp2a->light_input_dev->dev.kobj,
				   &lightsensor_attribute_group);
		input_unregister_device(gp2a->light_input_dev);

		if (gp2a->light_input_dev != NULL)
			kfree(gp2a->light_input_dev);
	}

	mutex_destroy(&gp2a->data_mutex);
	wake_lock_destroy(&gp2a->prx_wake_lock);
	device_init_wakeup(&pdev->dev, 0);

	kfree(gp2a);

	return 0;
}

static void gp2a_opt_shutdown(struct platform_device *pdev)
{
	struct gp2a_data *gp2a = platform_get_drvdata(pdev);

	if (gp2a == NULL) {
		pr_err("%s, gp2a_data is NULL!!!!!\n", __func__);
		return;
	}

	if (sensors_class != NULL) {
		device_remove_file(gp2a->proximity_dev, &dev_attr_prox_avg);
		device_remove_file(gp2a->proximity_dev, &dev_attr_state);
		device_remove_file(gp2a->proximity_dev,
			&dev_attr_proximity_enable);
		device_remove_file(gp2a->proximity_dev, &dev_attr_name);
		device_remove_file(gp2a->proximity_dev, &dev_attr_vendor);
		device_remove_file(gp2a->light_dev, &dev_attr_lux);
		device_remove_file(gp2a->light_dev, &dev_attr_light_enable);
		device_remove_file(gp2a->light_dev, &dev_attr_raw_data);
		device_remove_file(gp2a->light_dev, &dev_attr_name);
		device_remove_file(gp2a->light_dev, &dev_attr_vendor);
		device_destroy(sensors_class, 0);
	}

	if (gp2a->proximity_input_dev != NULL) {
		sysfs_remove_group(&gp2a->proximity_input_dev->dev.kobj,
				   &proximity_attribute_group);
		input_unregister_device(gp2a->proximity_input_dev);

/*
		if (gp2a->proximity_input_dev != NULL)
			kfree(gp2a->proximity_input_dev);
*/
	}

	cancel_delayed_work(&gp2a->light_work);
	//flush_scheduled_work();
	mutex_destroy(&gp2a->light_mutex);

	if (gp2a->light_input_dev != NULL) {
		sysfs_remove_group(&gp2a->light_input_dev->dev.kobj,
				   &lightsensor_attribute_group);
		input_unregister_device(gp2a->light_input_dev);

/*
		if (gp2a->light_input_dev != NULL)
			kfree(gp2a->light_input_dev);
*/
	}
	misc_deregister(&gp2a_opt_misc_device);
	mutex_destroy(&gp2a->data_mutex);
	wake_lock_destroy(&gp2a->prx_wake_lock);
	device_init_wakeup(&pdev->dev, 0);

	kfree(gp2a);

}

static int gp2a_opt_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct gp2a_data *gp2a = platform_get_drvdata(pdev);

	mutex_lock(&gp2a->light_mutex);
	if (gp2a->light_enabled)
		cancel_delayed_work_sync(&gp2a->light_work);

	mutex_unlock(&gp2a->light_mutex);

	return 0;
}

static int gp2a_opt_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct gp2a_data *gp2a = platform_get_drvdata(pdev);

	gp2a->light_count = 0;
	gp2a->light_buffer = 0;
	gp2a->light_first_level = true;

	mutex_lock(&gp2a->light_mutex);

	if (gp2a->light_enabled)
		schedule_delayed_work(&gp2a->light_work, 0);

	mutex_unlock(&gp2a->light_mutex);

	return 0;
}

static int proximity_onoff(u8 onoff, struct gp2a_data  *data)
{
	u8 value;
	int i;
	/* unsigned char get_data[1]; */
	int err = 0;

	/* already on light sensor, so must simultaneously
	   turn on light sensor and proximity sensor */
	if (onoff) {
		/*opt_i2c_read(COMMAND1, get_data, sizeof(get_data)); */
		/*if (get_data == 0xC1)
		   return 0; */
		for (i = 0; i < COL; i++) {
			err = opt_i2c_write(gp2a_original_image[i][0],
				&gp2a_original_image[i][1]);
			if (err < 0)
				pr_err("%s : turnning on error i = %d, err=%d\n",
				       __func__, i, err);
			data->lightsensor_mode = 0;
		}
	} else { /* light sensor turn on and proximity turn off */
		/*opt_i2c_read(COMMAND1, get_data, sizeof(get_data)); */
		/*if (get_data == 0xD1)
		   return 0; */

		if (data->lightsensor_mode)
			value = 0x67; /*resolution :16bit, range: *8(HIGH) */
		else
			value = 0x63; /* resolution :16bit, range: *128(LOW) */
		opt_i2c_write(COMMAND2, &value);
		/* OP3 : 1(operating mode)
		   OP2 :1(coutinuous operating mode) OP1 : 01(ALS mode) */
		value = 0xD0;
		opt_i2c_write(COMMAND1, &value);
	}

	return 0;
}

static int opt_i2c_remove(struct i2c_client *client)
{
	struct opt_state *data = i2c_get_clientdata(client);
	kfree(data);
	opt_i2c_client = NULL;
	return 0;
}

static int opt_i2c_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct opt_state *opt;

	printk(KERN_INFO "[GP2A] %s : start!!!\n", __func__);

	opt = kzalloc(sizeof(struct opt_state), GFP_KERNEL);
	if (opt == NULL) {
		pr_err("%s, %d : error!!!\n", __func__, __LINE__);
		return -ENOMEM;
	}

	if (client == NULL)
		pr_err("GP2A i2c client is NULL !!!\n");
	opt->client = client;
	i2c_set_clientdata(client, opt);

	/* rest of the initialisation goes here. */

	printk(KERN_INFO"[GP2A] opt i2c attach success!!!\n");

	opt_i2c_client = client;

	return 0;
}

static const struct i2c_device_id opt_device_id[] = {
	{"gp2a", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, opt_device_id);

static struct i2c_driver opt_i2c_driver = {
	.driver = {
		   .name = "gp2a",
		   .owner = THIS_MODULE,
		   },
	.probe = opt_i2c_probe,
	.remove = opt_i2c_remove,
	.id_table = opt_device_id,
};

static const struct dev_pm_ops gp2a_dev_pm_ops = {
	.suspend = gp2a_opt_suspend,
	.resume = gp2a_opt_resume,
};

static struct platform_driver gp2a_opt_driver = {
	.probe = gp2a_opt_probe,
	.remove = gp2a_opt_remove,
	.shutdown = gp2a_opt_shutdown,
	.driver = {
		   .name = "gp2a-opt",
		   .owner = THIS_MODULE,
			.pm = &gp2a_dev_pm_ops,
		   },
};

static int opt_i2c_init(void)
{
	if (i2c_add_driver(&opt_i2c_driver)) {
		pr_err("i2c_add_driver failed\n");
		return -ENODEV;
	}
	return 0;
}


static int __init gp2a_opt_init(void)
{
	int ret;

	ret = platform_driver_register(&gp2a_opt_driver);
	return ret;
}

static void __exit gp2a_opt_exit(void)
{
	platform_driver_unregister(&gp2a_opt_driver);
}

module_init(gp2a_opt_init);
module_exit(gp2a_opt_exit);

MODULE_AUTHOR("SAMSUNG");
MODULE_DESCRIPTION("Optical Sensor driver for GP2AP020A00F");
MODULE_LICENSE("GPL");
