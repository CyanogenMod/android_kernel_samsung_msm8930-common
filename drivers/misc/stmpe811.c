/* drivers/misc/stmpe811.c
 *
 * Copyright (C) 2011 Samsung Electronics Co, Ltd.
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

#include <linux/types.h>
#include <linux/irq.h>
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/firmware.h>
#include <linux/wakelock.h>
#include <linux/blkdev.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <mach/gpio.h>
#include <linux/stmpe811.h>

#define STMPE811_CHIP_ID                0x00
#define STMPE811_ID_VER                 0x02
#define STMPE811_SYS_CTRL1              0x03
#define STMPE811_SYS_CTRL2              0x04
#define STMPE811_INT_CTRL               0x09
#define STMPE811_INT_EN                 0x0A
#define STMPE811_INT_STA                0x0B
#define STMPE811_ADC_INT_EN             0x0E
#define STMPE811_ADC_INT_STA            0x0F
#define STMPE811_ADC_CTRL1              0x20
#define STMPE811_ADC_CTRL2              0x21
#define STMPE811_ADC_CAPT               0x22
#define STMPE811_ADC_DATA_CH0           0x30
#define STMPE811_ADC_DATA_CH1           0x32
#define STMPE811_ADC_DATA_CH2           0x34
#define STMPE811_ADC_DATA_CH3           0x36
#define STMPE811_ADC_DATA_CH4           0x38
#define STMPE811_ADC_DATA_CH5           0x3A
#define STMPE811_ADC_DATA_CH6           0x3C
#define STMPE811_ADC_DATA_CH7           0x3E
#define STMPE811_GPIO_AF                0x17
#define STMPE811_TSC_CTRL               0x40

struct stmpe811_adc_data {
	struct i2c_client       *client;
	struct mutex            adc_lock;
	struct delayed_work     init_work;
};

static struct stmpe811_adc_data *local_adc_data;

s32 stmpe811_adc_get_value(u8 channel)
{
	struct stmpe811_adc_data *adc_data = local_adc_data;
	struct i2c_client *client = adc_data->client;

	s32 w_data = 0;
	int data_channel_addr = 0;
	s32 ddata;

	mutex_lock(&adc_data->adc_lock);
	/* delay stablization time */
	msleep(30);
	i2c_smbus_write_byte_data(client, STMPE811_ADC_CAPT, 0xff);
	msleep(30);
	data_channel_addr = STMPE811_ADC_DATA_CH0 + (channel * 2);

	ddata = i2c_smbus_read_word_data(client, data_channel_addr);
	if (ddata < 0) {
		printk(KERN_INFO "%s: Failed to read ADC_DATA_CH(%d).\n",
					__func__, channel);
		mutex_unlock(&adc_data->adc_lock);
		return ddata;
	}

	w_data = ((ddata & 0xff) << 8 | (ddata >> 8)) & 0xfff;
	printk(KERN_INFO "%s: ADC_DATA_CH(%d) = 0x%x, %d\n",
					__func__, channel, w_data, w_data);

	mutex_unlock(&adc_data->adc_lock);

	return w_data;
}

#define	ADC_DATA_CH	0

static ssize_t adc_test_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{

	struct stmpe811_adc_data *adc_data = dev_get_drvdata(dev);
	struct i2c_client *client = adc_data->client;
	s32 ret;

	ret = stmpe811_adc_get_value(ADC_DATA_CH);
	if (ret < 0) {
		dev_err(&client->dev,
			"%s: err at read adc %d\n", __func__, ret);
		return snprintf(buf, 9, "UNKNOWN\n");
	}
	printk(KERN_INFO "%s: accessory adc[ch%d]: %x\n", __func__, ADC_DATA_CH, ret);

	if (ret > 0xe38 && ret < 0xed8)
		return snprintf(buf, 3, "1c\n");

	return snprintf(buf, 4, "%x\n", ret);
}

static ssize_t usb_state_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	if (current_cable_type == 3)
		return snprintf(buf, 22, "USB_STATE_CONFIGURED\n");

	return snprintf(buf, 25, "USB_STATE_NOTCONFIGURED\n");
}

static DEVICE_ATTR(adc, S_IRUGO, adc_test_show, NULL);
static DEVICE_ATTR(usb_state, S_IRUGO, usb_state_show, NULL);

static int stmpe811_adc_i2c_remove(struct i2c_client *client)
{
	struct stmpe811_adc_data *adc = i2c_get_clientdata(client);
	mutex_destroy(&adc->adc_lock);
	kfree(adc);

	return 0;
}

static void stmpe811_reg_init(struct stmpe811_adc_data *data)
{
	struct stmpe811_adc_data *adc_data = data;
	struct i2c_client *client = adc_data->client;

	int ret;
	/* intialize stmpe811 control register */
	ret = i2c_smbus_read_word_data(client, STMPE811_CHIP_ID);
	printk(KERN_INFO "%s: CHIP_ID: %x\n", __func__, ret);

	if (ret < 0) {
		printk(KERN_ERR "%s: Failed to read STMPE811_CHIP_ID : 0x%x\n",
				__func__, ret);
		return;
	}
	/* soft rest */
	i2c_smbus_write_byte_data(client, STMPE811_SYS_CTRL1, 0x02);
	msleep(20);
	/* clock on:GPIO&ADC, off:TS&TSC */
	i2c_smbus_write_byte_data(client, STMPE811_SYS_CTRL2, 0x0a);
	ret = i2c_smbus_read_byte_data(client, STMPE811_SYS_CTRL2);
	printk(KERN_INFO "STMPE811_SYS_CTRL2 = 0x%x\n", ret);
	/* disable interrupt */
	i2c_smbus_write_byte_data(client, STMPE811_INT_EN, 0x00);
	ret = i2c_smbus_read_byte_data(client, STMPE811_INT_EN);
	printk(KERN_INFO "STMPE811_INT_EN = 0x%x\n", ret);
	/* adc conversion time:64, 12bit ADC oper, internal */
	i2c_smbus_write_byte_data(client, STMPE811_ADC_CTRL1, 0x3c);
	ret = i2c_smbus_read_byte_data(client, STMPE811_ADC_CTRL1);
	printk(KERN_INFO "STMPE811_ADC_CTRL1 = 0x%x\n", ret);
	/* clock speed 6.5MHz */
	i2c_smbus_write_byte_data(client, STMPE811_ADC_CTRL2, 0x03);
	ret = i2c_smbus_read_byte_data(client, STMPE811_ADC_CTRL2);
	printk(KERN_INFO "STMPE811_ADC_CTRL2 = 0x%x\n", ret);

	/* It should be ADC settings.
	 * So the value should be 0x00 instead of 0xFF
	 * gpio 0-3 -> ADC
	 */
	i2c_smbus_write_byte_data(client, STMPE811_GPIO_AF, 0x00);
	ret = i2c_smbus_read_byte_data(client, STMPE811_GPIO_AF);
	printk(KERN_INFO "STMPE811_GPIO_AF = 0x%x\n", ret);
	/* init Ch[0]=ADC_CHECK(battery), ch[3]=Accessory */
	i2c_smbus_write_byte_data(client, STMPE811_ADC_CAPT, 0x90);

	i2c_smbus_write_byte_data(client, STMPE811_TSC_CTRL, 0x00);
	ret = i2c_smbus_read_byte_data(client, STMPE811_TSC_CTRL);
	printk(KERN_INFO "STMPE811_TSC_CTRL = 0x%x\n", ret);

	pr_info("%s success\n", __func__);

}

static int stmpe811_adc_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct stmpe811_adc_data *adc_data;
	struct device *sec_adc_dev;

	int ret;

	printk(KERN_INFO "%s: probe start!\n", __func__);

	if (!i2c_check_functionality(adapter,
		I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
		return -EIO;

	adc_data = kzalloc(sizeof(struct stmpe811_adc_data), GFP_KERNEL);
	if (adc_data == NULL) {
		printk(KERN_ERR "failed to allocate memory\n");
		return -ENOMEM;
	}

	adc_data->client = client;
	i2c_set_clientdata(client, adc_data);

	mutex_init(&adc_data->adc_lock);

	local_adc_data = adc_data;

	/* set sysfs for adc test mode*/
	sec_adc_dev = device_create(sec_class, NULL, 0, NULL, "adcswitch");
	if (IS_ERR(sec_adc_dev)) {
		printk(KERN_ERR "%s: Failed to create device (switch)!\n",
				__func__);
		ret = PTR_ERR(sec_adc_dev);
		goto err_create_device;
	}

	ret = device_create_file(sec_adc_dev, &dev_attr_adc);
	if (ret < 0) {
		printk(KERN_ERR "failed to create device file(%s)!\n",
				dev_attr_adc.attr.name);
		goto err_create_file_adc;
	}

	ret = device_create_file(sec_adc_dev, &dev_attr_usb_state);
	if (ret < 0) {
		printk(KERN_ERR "failed to create device file(%s)!\n",
				dev_attr_usb_state.attr.name);
		goto err_create_file_usb_state;
	}

	dev_set_drvdata(sec_adc_dev, adc_data);
	stmpe811_reg_init(adc_data);

	return 0;

err_create_file_usb_state:
	device_remove_file(sec_adc_dev, &dev_attr_usb_state);
err_create_file_adc:
	device_remove_file(sec_adc_dev, &dev_attr_adc);
err_create_device:
	if (client->irq)
		free_irq(client->irq, adc_data);

	mutex_destroy(&adc_data->adc_lock);
	i2c_set_clientdata(client, NULL);
	kfree(adc_data);
	return ret;
}


static const struct i2c_device_id stmpe811_id[] = {
	{"stmpe811", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, stmpe811_adc_device_id);


static struct i2c_driver stmpe811_adc_i2c_driver = {
	.driver = {
		.name = "stmpe811",
		.owner = THIS_MODULE,
	},
	.probe  = stmpe811_adc_i2c_probe,
	.remove = stmpe811_adc_i2c_remove,
	.id_table       = stmpe811_id,
};

static int __init stmpe811_adc_init(void)
{
	return i2c_add_driver(&stmpe811_adc_i2c_driver);
}
module_init(stmpe811_adc_init);

static void __exit stmpe811_adc_exit(void)
{
	i2c_del_driver(&stmpe811_adc_i2c_driver);
}

module_exit(stmpe811_adc_exit);

MODULE_AUTHOR("Samsung");
MODULE_DESCRIPTION("Samsung STMPE811 ADC driver");
MODULE_LICENSE("GPL");
