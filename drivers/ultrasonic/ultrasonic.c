/*
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/gpio.h>

#define SENDING_START_COMMAND				0x00
#define SENSING_STATUS						0x01
#define PEAK1_DETECT_TIMEMSB				0x02
#define PEAK1_DETECT_TIMELSB				0x03
#define PEAK1_DETECT_LEVEL					0x04
#define PEAK2_DETECT_TIMEMSB				0x05
#define PEAK2_DETECT_TIMELSB				0x06
#define PEAK2_DETECT_LEVEL					0x07
#define PEAK3_DETECT_TIMEMSB				0x08
#define PEAK3_DETECT_TIMELSB				0x09
#define PEAK3_DETEC_TLEVEL					0x0A
#define ERROE_CODE							0x0B
#define BURST_FREQUENCY						0x0C
#define BURST_NUMBER						0x0D
#define COMPANY_CODE						0x0E
#define PRODUCT_CODE						0x0F
#define FW_VERSION							0x10

/* power control */
#define ON				1
#define OFF				0

static struct device *ultrasonic_dev;
struct class *ultrasonic_class;

static ssize_t ultrasonic_enable_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);
static ssize_t ultrasonic_poll_delay_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_poll_delay_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);
static ssize_t ultrasonic_name_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_read_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_ver_check_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_data_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t ultrasonic_data_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);

static unsigned char Read_i2cBuf[16] = { 0, };

struct ultrasonic {
	struct i2c_client *client;
	struct mutex lock;
	struct input_dev *input;
	struct work_struct work_ultrasonic;
	struct workqueue_struct *wq;
	struct hrtimer timer;
	ktime_t ultrasonic_poll_delay;
	int state;
	struct device *ultrasonic_sensor_device;
	struct ultrasonic_platform_data *pdata;
};

static int ultrasonic_i2c_write(struct i2c_client *i2c_client,
					unsigned int len, unsigned char *data)
{
	struct i2c_msg msgs[1];
	int res;

	if (unlikely(NULL == data || NULL == i2c_client))
		return -EINVAL;

	msgs[0].addr = i2c_client->addr;
	msgs[0].flags = 0;	/* write */
	msgs[0].buf = (unsigned char *)data;
	msgs[0].len = len;
	res = i2c_transfer(i2c_client->adapter, msgs, 1);

	if (unlikely(res < 1))
	{
		pr_info("%s:(error = %d)\n", __func__,res);
		return res;
	}
	else
		return 0;
}

int ultrasonic_i2c_write_single_reg(struct i2c_client *i2c_client,
					unsigned char reg, unsigned char value)
{
	unsigned char data[2];
	data[0] = reg;
	data[1] = value;
	return ultrasonic_i2c_write(i2c_client, 2, data);
}

static int ultrasonic_i2c_read_reg(struct i2c_client *i2c_client,
			 unsigned char reg, unsigned int len,
			 unsigned char *data)
{
	struct i2c_msg msgs[2];
	int res;

	if (unlikely(NULL == data || NULL == i2c_client))
	{
		return -EINVAL;
	}
	msgs[0].addr = i2c_client->addr;
	msgs[0].flags = 0;	/* write */
	msgs[0].buf = &reg;
	msgs[0].len = 1;

	msgs[1].addr = i2c_client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = data;
	msgs[1].len = len;
	res = i2c_transfer(i2c_client->adapter, msgs, 2);
	if (unlikely(res < 1))
	{
		pr_info("%s:(error = %d)\n", __func__,res);
		return res;
	}
	else
		return 0;
}


/* power & timer enable */
static int ultrasonic_enable(struct ultrasonic *data)
{
	int i=0;
	gpio_set_value(54,1);
	gpio_set_value(47,1);
	for(i=0; i<16; i++)
	{
		Read_i2cBuf[i]= 0;
	}

	hrtimer_start(&data->timer, data->ultrasonic_poll_delay, HRTIMER_MODE_REL);

	return 0;
}

/* power & timer disable */
static int ultrasonic_disable(struct ultrasonic *data)
{
	hrtimer_cancel(&data->timer);
	cancel_work_sync(&data->work_ultrasonic);
	gpio_set_value(54,0);
	gpio_set_value(47,0);

	return 0;
}

static int ultrasonic_get_data(struct i2c_client *client)
{
	unsigned char reg[16] = { 0, };
	int i =0;
	ultrasonic_i2c_write_single_reg(client, SENDING_START_COMMAND, 0x01);
	msleep(30);
	ultrasonic_i2c_read_reg(client, SENSING_STATUS, 16, reg);

	for(i=0; i<16; i++)
	{
		Read_i2cBuf[i]= reg[i];
	}
	return 0;
}

static void ultrasonic_work_func_ultrasonic(struct work_struct *work)
{
	//int result;
	struct ultrasonic *data =
	    container_of(work, struct ultrasonic, work_ultrasonic);
	ultrasonic_get_data(data->client);
}

static enum hrtimer_restart ultrasonic_timer_func(struct hrtimer *timer)
{
	struct ultrasonic *data =
	    container_of(timer, struct ultrasonic, timer);

	queue_work(data->wq, &data->work_ultrasonic);
	hrtimer_forward_now(&data->timer, data->ultrasonic_poll_delay);
	return HRTIMER_RESTART;
}

static ssize_t ultrasonic_poll_delay_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct ultrasonic *data = dev_get_drvdata(dev);
	return sprintf(buf, "%lld\n", ktime_to_ns(data->ultrasonic_poll_delay));
}

static ssize_t ultrasonic_poll_delay_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t size)
{
	int err;
	int64_t new_delay;
	struct ultrasonic *data = dev_get_drvdata(dev);
	err = strict_strtoll(buf, 10, &new_delay);
	if (err < 0)
		return err;

	if(new_delay <= 45000000)
		new_delay = 45000000;

	pr_info("%s: new delay = %lldns \n", __func__,new_delay);

	if (new_delay != ktime_to_ns(data->ultrasonic_poll_delay)) {
		data->ultrasonic_poll_delay = ns_to_ktime(new_delay);

		if(data->state)
				pr_info("%s: true!\n", __func__);
		else
				pr_info(" %s: false!\n", __func__);
	}

	return size;
}

static ssize_t ultrasonic_enable_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct ultrasonic *data = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", data->state);
}

static ssize_t ultrasonic_enable_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t size)
{
	bool new_value = false;
	int err = 0;
	struct ultrasonic *data = dev_get_drvdata(dev);
	if (sysfs_streq(buf, "1")) {
		new_value = true;
	} else if (sysfs_streq(buf, "0")) {
		new_value = false;
	} else {
		pr_err("%s: invalid value %d\n", __func__, *buf);
		return -EINVAL;
	}

	pr_debug("new_value = %d, old state = %d\n", new_value, data->state);

	if (new_value && (!data->state)) {
		err = ultrasonic_enable(data);
		if (!err) {
			data->state = ON;
		} else {
			pr_err("%s: couldn't enable", __func__);
			data->state = OFF;
		}
	} else if (!new_value && (data->state)) {
		err = ultrasonic_disable(data);
		if (!err)
			data->state = OFF;
		else
			pr_err("%s: couldn't disable", __func__);
	}

	return size;
}

static ssize_t ultrasonic_name_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return sprintf(buf, "%s\n", "ultrasonic");
}

static ssize_t ultrasonic_read_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	u16 raw[3];
	int data[3] ;
	int read_data[3];
	int i=0;

	raw[0] = (((u16) ((u16) Read_i2cBuf[1] << 8)) | Read_i2cBuf[2]);
	raw[1] = (((u16) ((u16) Read_i2cBuf[4] << 8)) | Read_i2cBuf[5]);
	raw[2] = (((u16) ((u16) Read_i2cBuf[7] << 8)) | Read_i2cBuf[8]);

	for(i=0; i<3; i++)
	{
		data[i] = raw[i];
		read_data[i] = data[i] * 340 / 20000;
	}
	pr_info(" %s %d %d %d\n", __func__,read_data[0],read_data[1],read_data[2]);
	return sprintf(buf, "%x,%d,%d,%d \n", Read_i2cBuf[0],read_data[0],read_data[1],read_data[2]);
}

static ssize_t ultrasonic_ver_check_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct ultrasonic *data = dev_get_drvdata(dev);
	unsigned char reg=0;
	unsigned int ret=0;
	int Read_VerCheck=0;
	int Read_ProductCode=0;
	gpio_set_value(54, 1);
	gpio_set_value(47, 1);
	mdelay(50);
	ultrasonic_i2c_write_single_reg(data->client, SENDING_START_COMMAND, 0x01);
	mdelay(50);

	ret = ultrasonic_i2c_read_reg(data->client, COMPANY_CODE, 1, &reg);
	if(!ret)
		Read_ProductCode= reg;
	else
		Read_ProductCode=0;

	ret = ultrasonic_i2c_read_reg(data->client, FW_VERSION, 1, &reg);
	if(!ret)
		Read_VerCheck= reg;
	else
		Read_VerCheck=0;

	gpio_set_value(54, 0);
	gpio_set_value(47, 0);
	return sprintf(buf, "%d,%d \n", Read_ProductCode,Read_VerCheck);

}

static ssize_t ultrasonic_data_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct ultrasonic *data = dev_get_drvdata(dev);
	unsigned char reg=0;
	unsigned int ret=0;
	unsigned char read_data[2]={0,};
	u16 raw=0;
	u16 raw_data=0;

	ultrasonic_i2c_write_single_reg(data->client, SENDING_START_COMMAND, 0x01);
	mdelay(30);
	ret = ultrasonic_i2c_read_reg(data->client, PEAK1_DETECT_TIMEMSB, 1, &reg);
	if(!ret)
		read_data[0]= reg;
	else
		read_data[0]=0;
	ret = ultrasonic_i2c_read_reg(data->client, PEAK1_DETECT_TIMELSB, 1, &reg);
	if(!ret)
		read_data[1]= reg;
	else
		read_data[1]=0;

	raw = (((u16) ((u16) read_data[0] << 8)) | read_data[1]);
	raw_data = raw * 340 / 20000;
	return sprintf(buf, "%d,%d \n", raw,raw_data);
}

static ssize_t ultrasonic_data_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t size)
{
	bool new_value = false;
	pr_info("%s: true!\n", __func__);
	if (sysfs_streq(buf, "1")) {
		new_value = true;
	} else if (sysfs_streq(buf, "0")) {
		new_value = false;
	} else {
		pr_err("%s: invalid value %d\n", __func__, *buf);
		return -EINVAL;
	}
	if (new_value) {
		gpio_set_value(54, 1);
		gpio_set_value(47, 1);
	}else{
		gpio_set_value(54, 0);
		gpio_set_value(47, 0);
	}
	return size;
}

static DEVICE_ATTR(poll_delay ,S_IRWXUGO, ultrasonic_poll_delay_show, ultrasonic_poll_delay_store);
static DEVICE_ATTR(enable ,S_IRWXUGO, ultrasonic_enable_show, ultrasonic_enable_store);
static DEVICE_ATTR(name,S_IRWXUGO, ultrasonic_name_show, NULL);
static DEVICE_ATTR(read,S_IRWXUGO, ultrasonic_read_show, NULL);
static DEVICE_ATTR(ver_check,S_IRWXUGO, ultrasonic_ver_check_show, NULL);
static DEVICE_ATTR(data,S_IRWXUGO, ultrasonic_data_show, ultrasonic_data_store);

static int ultrasonic_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;
	struct ultrasonic *data;
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	pr_info("%s: success!\n", __func__);
	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
		return -EIO;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		pr_err("%s, failed to alloc memory for module data\n",
		       __func__);
		return -ENOMEM;
	}
	data->client = client;
	i2c_set_clientdata(client, data);

	mutex_init(&data->lock);

	hrtimer_init(&data->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	data->ultrasonic_poll_delay = ns_to_ktime(180 * NSEC_PER_MSEC);
	data->timer.function = ultrasonic_timer_func;
	data->state = OFF;

	data->wq = alloc_workqueue("ultrasonic_wq", WQ_UNBOUND | WQ_RESCUER, 1);
	if (!data->wq) {
		err = -ENOMEM;
		pr_err("%s: could not create workqueue\n", __func__);
		goto err_create_workqueue;
	}

	INIT_WORK(&data->work_ultrasonic, ultrasonic_work_func_ultrasonic);

	 ultrasonic_class = class_create(THIS_MODULE, "ultrasonic");

        /* sys fs */
        if(IS_ERR(ultrasonic_class))
            pr_err("Failed to create class(sec)!\n");

        ultrasonic_dev = device_create(ultrasonic_class, NULL, 0, data, "ultrasonic");
        if(IS_ERR(ultrasonic_dev))
            pr_err("Failed to create device(lcd)!\n");
	if(device_create_file(ultrasonic_dev, &dev_attr_enable) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_enable.attr.name);
	if(device_create_file(ultrasonic_dev, &dev_attr_poll_delay) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_poll_delay.attr.name);
	if(device_create_file(ultrasonic_dev, &dev_attr_read) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_read.attr.name);
	if(device_create_file(ultrasonic_dev, &dev_attr_name) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_name.attr.name);
	if(device_create_file(ultrasonic_dev, &dev_attr_ver_check) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_ver_check.attr.name);
	if(device_create_file(ultrasonic_dev, &dev_attr_data) < 0)
            pr_err("Failed to create device file(%s)!\n", dev_attr_data.attr.name);


	pr_info("%s: success!\n", __func__);
	goto done;


err_create_workqueue:
	mutex_destroy(&data->lock);
	kfree(data);
done:
	return err;
}

static int ultrasonic_remove(struct i2c_client *client)
{
	struct ultrasonic *data = i2c_get_clientdata(client);
	input_unregister_device(data->input);

	if (data->state)
		ultrasonic_disable(data);

	destroy_workqueue(data->wq);
	mutex_destroy(&data->lock);
	kfree(data);

	return 0;
}

static void ultrasonic_shutdown(struct i2c_client *client)
{
	struct ultrasonic *data = i2c_get_clientdata(client);
	if (data->state) {
		int err;
		err = ultrasonic_disable(data);
		if (err)
			pr_err("%s: could not disable\n", __func__);
	}
}
static const struct i2c_device_id ultrasonic_id[] = {
	{"STMA530", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, ultrasonic_id);

static struct i2c_driver ultrasonic_driver = {
	.probe = ultrasonic_probe,
	.remove = ultrasonic_remove,
	.shutdown = ultrasonic_shutdown,
	.id_table = ultrasonic_id,
	.driver = {
		   .name = "STMA530",
		   .owner = THIS_MODULE,
		//   .pm = &ultrasonic_pm_ops,
	},
};

static int __init ultrasonic_init(void)
{
	int result=0;

	result = i2c_add_driver(&ultrasonic_driver);

	return result;
}

static void __exit ultrasonic_exit(void)
{
	i2c_del_driver(&ultrasonic_driver);
}

MODULE_DESCRIPTION("ultrasonic Ambient ultrasonic sensor driver");
MODULE_LICENSE("GPL v2");

module_init(ultrasonic_init);
module_exit(ultrasonic_exit);

