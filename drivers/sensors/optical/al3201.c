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
#include <linux/sensors_core.h>
#include <linux/al3201.h>

struct al3201_data {
	struct i2c_client *client;
	struct mutex lock;
	struct input_dev *input;
	struct work_struct work_light;
	struct workqueue_struct *wq;
	struct hrtimer timer;
	ktime_t light_poll_delay;
	u8 reg_cache[AL3201_NUM_CACHABLE_REGS];
	int state;
	struct device *light_sensor_device;
	struct al3201_platform_data *pdata;
};

/*
 * register access helpers
 */
static int al3201_read_reg(struct i2c_client *client,
			   u32 reg, u8 mask, u8 shift)
{
	struct al3201_data *data = i2c_get_clientdata(client);

	return (data->reg_cache[reg] & mask) >> shift;
}

static int al3201_write_reg(struct i2c_client *client,
			    u32 reg, u8 mask, u8 shift, u8 val)
{
	u8 tmp;
	int ret;
	struct al3201_data *data = i2c_get_clientdata(client);

	if (reg >= AL3201_NUM_CACHABLE_REGS)
		return -EINVAL;

	mutex_lock(&data->lock);

	tmp = data->reg_cache[reg];
	tmp &= ~mask;
	tmp |= val << shift;

	ret = i2c_smbus_write_byte_data(client, reg, tmp);
	if (!ret)
		data->reg_cache[reg] = tmp;
	else
		pr_err("%s: I2C read failed!\n", __func__);

	mutex_unlock(&data->lock);
	return ret;
}

/*
 * internally used functions
 */

/* range */
static int al3201_get_range(struct i2c_client *client)
{
	int tmp;
	tmp = al3201_read_reg(client, AL3201_RAN_COMMAND,
			      AL3201_RAN_MASK, AL3201_RAN_SHIFT);
	return tmp;
}

static int al3201_set_range(struct i2c_client *client, int range)
{
	return al3201_write_reg(client, AL3201_RAN_COMMAND,
				AL3201_RAN_MASK, AL3201_RAN_SHIFT, range);
}

/* Response time */
static int al3201_set_response_time(struct i2c_client *client, int time)
{
	return al3201_write_reg(client, AL3201_RT_COMMAND,
				AL3201_RT_MASK, AL3201_RT_SHIFT, time);
}

/* power_state */
static int al3201_set_power_state(struct i2c_client *client, int state)
{
	return al3201_write_reg(client, AL3201_POW_COMMAND,
				AL3201_POW_MASK, AL3201_POW_SHIFT,
				state ? AL3201_POW_UP : AL3201_POW_DOWN);
}

/* power & timer enable */
static int al3201_enable(struct al3201_data *data)
{
	int err;

	err = al3201_set_power_state(data->client, ON);
	if (unlikely(err)) {
		pr_err("%s: Failed to write byte (POWER_UP)\n", __func__);
		return err;
	}

	hrtimer_start(&data->timer, data->light_poll_delay, HRTIMER_MODE_REL);

	return err;
}

/* power & timer disable */
static int al3201_disable(struct al3201_data *data)
{
	int err;

	hrtimer_cancel(&data->timer);
	cancel_work_sync(&data->work_light);

	err = al3201_set_power_state(data->client, OFF);
	if (unlikely(err))
		pr_err("%s: Failed to write byte (POWER_DOWN)\n", __func__);

	return err;
}

static int al3201_get_adc_value(struct i2c_client *client)
{
	int lsb, msb, range;
	u32 val;
	struct al3201_data *data = i2c_get_clientdata(client);

	range = al3201_get_range(client);
	if (!range) {
		pr_err("%s: reset! need to re-init...\n", __func__);
		/* 1 : High resolution range, 0 to 8192 lux*/
		al3201_set_range(client, 1);
		/* 0x02 : Response time 200ms low pass fillter */
		al3201_set_response_time(client, 0x02);
		usleep_range(10000, 11000);
	}

	mutex_lock(&data->lock);
	lsb = i2c_smbus_read_byte_data(client, AL3201_ADC_LSB);
	if (lsb < 0) {
		mutex_unlock(&data->lock);
		return lsb;
	}
	msb = i2c_smbus_read_byte_data(client, AL3201_ADC_MSB);
	mutex_unlock(&data->lock);
	if (msb < 0)
		return msb;

	val = (u32) (msb << 8 | lsb);
	if (val < 7)
		val = 0;
	return val;
}

static void al3201_work_func_light(struct work_struct *work)
{
	u32 result;
	struct al3201_data *data =
	    container_of(work, struct al3201_data, work_light);

	result = al3201_get_adc_value(data->client);
	pr_debug("%s : adc value = %d\n", __func__, result);
	if (result > 60000)
		result = 60000;
	if (!result)
		result = 1;
	input_report_rel(data->input, REL_MISC, result);
	input_sync(data->input);
}

static enum hrtimer_restart al3201_timer_func(struct hrtimer *timer)
{
	struct al3201_data *data =
	    container_of(timer, struct al3201_data, timer);

	queue_work(data->wq, &data->work_light);
	hrtimer_forward_now(&data->timer, data->light_poll_delay);

	return HRTIMER_RESTART;
}

static ssize_t al3201_poll_delay_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct al3201_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%lld\n", ktime_to_ns(data->light_poll_delay));
}

static ssize_t al3201_poll_delay_store(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t size)
{
	int err;
	int64_t new_delay;
	struct al3201_data *data = dev_get_drvdata(dev);

	err = strict_strtoll(buf, 10, &new_delay);
	if (err < 0)
		return err;

	pr_debug("new delay = %lldns, old delay = %lldns\n",
	       new_delay, ktime_to_ns(data->light_poll_delay));

	if (new_delay != ktime_to_ns(data->light_poll_delay)) {
		data->light_poll_delay = ns_to_ktime(new_delay);
		if (data->state) {
			al3201_disable(data);
			al3201_enable(data);
		}
	}

	return size;
}

static DEVICE_ATTR(poll_delay, S_IRUGO | S_IWUSR | S_IWGRP,
		   al3201_poll_delay_show, al3201_poll_delay_store);

static ssize_t al3201_light_enable_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct al3201_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", data->state);
}

static ssize_t al3201_light_enable_store(struct device *dev,
					 struct device_attribute *attr,
					 const char *buf, size_t size)
{
	bool new_value = false;
	int err = 0;
	struct al3201_data *data = dev_get_drvdata(dev);

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
		err = al3201_enable(data);
		if (!err) {
			data->state = ON;
		} else {
			pr_err("%s: couldn't enable", __func__);
			data->state = OFF;
		}
	} else if (!new_value && (data->state)) {
		err = al3201_disable(data);
		if (!err)
			data->state = OFF;
		else
			pr_err("%s: couldn't disable", __func__);
	}

	return size;
}

static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR | S_IWGRP,
		   al3201_light_enable_show, al3201_light_enable_store);

static struct attribute *al3201_attributes[] = {
	&dev_attr_enable.attr,
	&dev_attr_poll_delay.attr,
	NULL
};

static const struct attribute_group al3201_attribute_group = {
	.attrs = al3201_attributes,
};

static ssize_t al3201_light_sensor_lux_show(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	struct al3201_data *data = dev_get_drvdata(dev);
	int result;
	int err;
	int i;

	if (!data->state) {
		err = al3201_set_power_state(data->client, ON);
		if (err) {
			pr_err("%s: Failed (POWER_UP)\n", __func__);
			return err;
		}
		usleep_range(5000, 6000);

		for (i = 0; i < 10; i++) {
			result = al3201_get_adc_value(data->client);
			usleep_range(10000, 11000);
		}

		err = al3201_set_power_state(data->client, OFF);
		if (unlikely(err))
			pr_err("%s: Failed (POWER_DOWN)\n", __func__);

		usleep_range(5000, 6000);
	} else {
		result = al3201_get_adc_value(data->client);
	}

	return sprintf(buf, "%d\n", result);
}

static struct device_attribute dev_attr_light_sensor_lux =
__ATTR(lux, S_IRUSR | S_IRGRP, al3201_light_sensor_lux_show, NULL);

static struct device_attribute dev_attr_light_sensor_raw_data =
	__ATTR(raw_data, S_IRUSR | S_IRGRP,
		al3201_light_sensor_lux_show, NULL);

static ssize_t al3201_light_vendor_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return sprintf(buf, "%s\n", "LITEON");
}
static struct device_attribute dev_attr_light_sensor_vendor =
	__ATTR(vendor, S_IRUSR | S_IRGRP, al3201_light_vendor_show, NULL);

static ssize_t al3201_light_name_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return sprintf(buf, "%s\n", "AL3201");
}
static struct device_attribute dev_attr_light_sensor_name =
	__ATTR(name, S_IRUSR | S_IRGRP, al3201_light_name_show, NULL);

static struct device_attribute *additional_light_attrs[] = {
	&dev_attr_light_sensor_lux,
	&dev_attr_light_sensor_raw_data,
	&dev_attr_light_sensor_vendor,
	&dev_attr_light_sensor_name,
	NULL,
};

static int al3201_init_client(struct i2c_client *client)
{
	int i;
	struct al3201_data *data = i2c_get_clientdata(client);

	/* read all the registers once to fill the cache.
	 * if one of the reads fails, we consider the init failed */
	for (i = 0; i < ARRAY_SIZE(data->reg_cache); i++) {
		int v = i2c_smbus_read_byte_data(client, al3201_reg[i]);
		if (v < 0)
			return -ENODEV;
		data->reg_cache[i] = v;
	}

	/* set defaults */
	/*
	 *      0 : Low resolution range, 0 to 32768 lux
	 *  1 : High resolution range, 0 to 8192 lux
	 */
	al3201_set_range(client, 1);
	/* 0x02 : Response time 200ms low pass fillter */
	al3201_set_response_time(client, 0x02);
	/* chip power off */
	al3201_set_power_state(client, OFF);

	return 0;
}

static int __devinit al3201_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	int err = 0;
	struct al3201_data *data;
	struct input_dev *input_dev;
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct al3201_platform_data *pdata = client->dev.platform_data;

	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
		return -EIO;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		pr_err("%s, failed to alloc memory for module data\n",
		       __func__);
		return -ENOMEM;
	}
	data->pdata = pdata;
	data->client = client;
	i2c_set_clientdata(client, data);

	if (likely(pdata)) {
		if (pdata->power_on)
			pdata->power_on(true);
	}

	mutex_init(&data->lock);

	/* initialize the AL3201 chip */
	err = al3201_init_client(client);
	if (err) {
		pr_err("%s: No search al3201 lightsensor!\n", __func__);
		goto err_initializ_chip;
	}

	hrtimer_init(&data->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	data->light_poll_delay = ns_to_ktime(200 * NSEC_PER_MSEC);
	data->timer.function = al3201_timer_func;
	data->state = OFF;

	data->wq = alloc_workqueue("al3201_wq", WQ_UNBOUND | WQ_RESCUER, 1);
	if (!data->wq) {
		err = -ENOMEM;
		pr_err("%s: could not create workqueue\n", __func__);
		goto err_create_workqueue;
	}

	INIT_WORK(&data->work_light, al3201_work_func_light);

	input_dev = input_allocate_device();
	if (!input_dev) {
		pr_err("%s: could not allocate input device\n", __func__);
		err = -ENOMEM;
		goto err_input_allocate_device_light;
	}

	input_set_drvdata(input_dev, data);
	input_dev->name = "light_sensor";
	input_set_capability(input_dev, EV_REL, REL_MISC);

	err = input_register_device(input_dev);
	if (err < 0) {
		pr_err("%s: could not register input device\n", __func__);
		input_free_device(input_dev);
		goto err_input_register_device_light;
	}

	data->input = input_dev;

	err = sysfs_create_group(&input_dev->dev.kobj, &al3201_attribute_group);
	if (err) {
		pr_err("%s: could not create sysfs group\n", __func__);
		goto err_sysfs_create_group_light;
	}

	err = sensors_register(data->light_sensor_device,
			       data, additional_light_attrs, "light_sensor");
	if (err) {
		pr_err("%s: cound not register sensor device\n", __func__);
		goto err_sysfs_create_factory_light;
	}

	pr_info("%s: success!\n", __func__);
	goto done;

err_sysfs_create_factory_light:
	sysfs_remove_group(&data->input->dev.kobj, &al3201_attribute_group);
err_sysfs_create_group_light:
	input_unregister_device(data->input);
err_input_register_device_light:
err_input_allocate_device_light:
	destroy_workqueue(data->wq);
err_create_workqueue:
err_initializ_chip:
	mutex_destroy(&data->lock);
	kfree(data);
done:
	return err;
}

static int al3201_remove(struct i2c_client *client)
{
	struct al3201_data *data = i2c_get_clientdata(client);
	sensors_unregister(data->light_sensor_device);
	sysfs_remove_group(&data->input->dev.kobj, &al3201_attribute_group);
	input_unregister_device(data->input);
	al3201_set_power_state(client, OFF);

	if (data->state)
		al3201_disable(data);

	destroy_workqueue(data->wq);
	mutex_destroy(&data->lock);
	kfree(data);

	return 0;
}

static void al3201_shutdown(struct i2c_client *client)
{
	int err = 0;
	struct al3201_data *data = i2c_get_clientdata(client);

	if (data->state) {
		err = al3201_disable(data);
		if (err)
			pr_err("%s: could not disable\n", __func__);
	}
}

static int al3201_suspend(struct device *dev)
{
	int err = 0;
	struct i2c_client *client = to_i2c_client(dev);
	struct al3201_data *data = i2c_get_clientdata(client);

	if (data->state) {
		err = al3201_disable(data);
		if (err)
			pr_err("%s: could not disable\n", __func__);
	}

	return err;
}

static int al3201_resume(struct device *dev)
{
	int err = 0;
	struct i2c_client *client = to_i2c_client(dev);
	struct al3201_data *data = i2c_get_clientdata(client);

	if (data->state) {
		err = al3201_enable(data);
		if (err)
			pr_err("%s: could not enable\n", __func__);
	}
	return err;
}

static const struct i2c_device_id al3201_id[] = {
	{al3201_DRV_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, al3201_id);

static const struct dev_pm_ops al3201_pm_ops = {
	.suspend = al3201_suspend,
	.resume = al3201_resume,
};

static struct i2c_driver al3201_driver = {
	.driver = {
		   .name = al3201_DRV_NAME,
		   .owner = THIS_MODULE,
		   .pm = &al3201_pm_ops,
		   },
	.probe = al3201_probe,
	.remove = al3201_remove,
	.shutdown = al3201_shutdown,
	.id_table = al3201_id,
};

static int __init al3201_init(void)
{
	return i2c_add_driver(&al3201_driver);
}

static void __exit al3201_exit(void)
{
	i2c_del_driver(&al3201_driver);
}

MODULE_DESCRIPTION("AL3201 Ambient light sensor driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);

module_init(al3201_init);
module_exit(al3201_exit);
