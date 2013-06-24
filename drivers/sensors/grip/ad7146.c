/**
 *\mainpage
 * AD7146 MLD Driver
\n
 * @copyright 2013 Analog Devices Inc.
\n
 * Licensed under the GPL Version 3 or later.
 * \date      April-2013
 * \version   Driver 1.3
 * \version   Android ICS 4.0
 * \version   Linux 3.0.15
 */


/**
 * \file ad7146.c
 * This file is the core driver part of AD7146 for Event interface
 * It also has routines for interrupt handling,suspend, resume,
 * initialization routines etc.
 * AD7146 MLD Driver
 * Copyright 2013 Analog Devices Inc.
 * Licensed under the GPL Version 3 or later.
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/pm.h>
#include <linux/module.h>
#include <linux/wakelock.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/input/ad7146.h>
#include <linux/sensors_core.h>

#define VENDOR_NAME "ADI"
#define DEVICE_NAME "AD7146"

#define AD7146_DEBUG
#define AD7146_DEBUG_L2

#ifdef AD7146_DEBUG
#define AD7146_DRIVER_DBG(format, arg...) pr_info("[AD7146]:"format,\
						 ## arg)
#else
#define AD7146_DRIVER_DBG(format, arg...) if (0)
#endif

#ifdef AD7146_DEBUG_L2
#define AD7146_DRIVER_DBG_L2(format, arg...) pr_info("[AD7146]:"format,\
						    ## arg)
#else
#define AD7146_DRIVER_DBG_L2(format, arg...) if (0)
#endif

/**
Look up table for the Sensitivity values of the AD7146
*/
static int SS_LUT[16] = {
	2500, 2973, 3440, 3908,
	4379, 4847, 5315, 5783,
	6251, 6722, 7190, 7658,
	8128, 8596, 9064, 9532
};

/**
 *    The global mutex for the locking of the ISR.
 */
DEFINE_MUTEX(interrupt_thread_mutex);
/*
 This elaborates the sysfs attributes used in the driver
 */
/*--------------------------------------------------------------*/
static ssize_t store_dumpregs(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t count);
static DEVICE_ATTR(dump, S_IRWXUGO, NULL, store_dumpregs);
/*--------------------------------------------------------------*/
static ssize_t show_interrupt_status(struct device *dev,
			      struct device_attribute *attr, char *buf);
static ssize_t store_interrupt_status(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count);
static DEVICE_ATTR(status, S_IRWXUGO, show_interrupt_status, store_interrupt_status);
/*--------------------------------------------------------------*/
static ssize_t store_enable(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count);
static ssize_t show_enable(struct device *dev,
			      struct device_attribute *attr, char *buf);

static DEVICE_ATTR(enable, S_IRWXUGO, show_enable, store_enable);
/*--------------------------------------------------------------*/
static ssize_t do_calibrate(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count);

static DEVICE_ATTR(calibrate, S_IRWXUGO, NULL, do_calibrate);
/*--------------------------------------------------------------*/
/**
SYSFS attributes list
*/
static struct attribute *ad7146_sysfs_entries[] = {
	&dev_attr_calibrate.attr,
	&dev_attr_enable.attr,
	&dev_attr_dump.attr,
	&dev_attr_status.attr,
	NULL
};
/**
SYSFS attribute Group
*/
static struct attribute_group ad7146_attr_group = {
	.name = NULL,
	.attrs = ad7146_sysfs_entries,
};

/**
OFF_SET calculation function for the algorithm
*/
static inline unsigned int OFF_CALC(unsigned short val1, unsigned short val2,
			     unsigned short Sens, unsigned int Type)
{
	unsigned int ret;
	if (Type)
		ret = ((abs(val1 - val2) * 100 * 100) /
			(SS_LUT[((Sens & POS_SENS) >> 8)]));
	else
		ret = ((abs(val1 - val2) * 100 * 100) /
			(SS_LUT[((Sens & NEG_SENS))]));
	if (ret < PW_ON_OFFSET_MIN)
		return PW_ON_OFFSET_MIN;
	else
		return ret;
}

/**
  Writes to the Device register through i2C.
  Used to Write the data to the I2C client's Register through the i2c protocol

  @param data The data to be written
  @param reg The register address
  @param dev The Device Structure
  @return 0 on success

  @see ad7146_i2c_read
 */
static int ad7146_i2c_write(struct device *dev, unsigned short reg,
		unsigned short data)
{
	struct i2c_client *client = to_i2c_client(dev);
	int ret;
	u8 *_reg = (u8 *)&reg;
	u8 *_data = (u8 *)&data;

	u8 tx[4] = {
		_reg[1],
		_reg[0],
		_data[1],
		_data[0]
	};
	ret = i2c_master_send(client, tx, 4);
	if (ret < 0)
		dev_err(&client->dev, "I2C write error (%d)\n", ret);
	return ret;
}
/**
  Reads data from the Device register through i2C.
  This is used to read the data from the AD7146 I2C client

  @param dev The Device Structure (Standard linux call)
  @param reg The register address to be read.
  @param data The data Read from the Given address.
  @return The number of bytes transfered as an integer

  @see ad7146_i2c_write
 */

static int ad7146_i2c_read(struct device *dev, unsigned short reg,
		unsigned short *data)
{
	struct i2c_client *client = to_i2c_client(dev);
	int ret;
	u8 *_reg = (u8 *)&reg;
	u8 *_data = (u8 *)data;

	u8 tx[2] = {
		_reg[1],
		_reg[0]
	};
	u8 rx[2];

	ret = i2c_master_send(client, tx, 2);
	if (ret >= 0)
		ret = i2c_master_recv(client, rx, 2);

	if (unlikely(ret < 0)) {
		dev_err(&client->dev, "I2C read error (%d)\n", ret);
	} else {
		_data[0] = rx[1];
		_data[1] = rx[0];
	}

	return ret;
}

static int sensorshutdownmode(struct ad7146_chip *ad7146)
{
	unsigned short data = 0;

	mutex_lock(&interrupt_thread_mutex);

	ad7146->read(ad7146->dev, AD7146_PWR_CTRL, &data);
	data = (data & 0xFC);
	ad7146->write(ad7146->dev, AD7146_PWR_CTRL, data);

	mutex_unlock(&interrupt_thread_mutex);
	return 0;
}

/**
  This function is used to indicate Full Grip state by event
  @param ad7146 The AD7146 chip structure pointer
  @return void

 */
static inline void indicatefullgripstate(struct ad7146_chip *ad7146)
{
	AD7146_DRIVER_DBG("indicatefullgripstate()\n");
	if (ad7146->eventcheck == 1) {
		input_report_rel(ad7146->input, REL_MISC, EVENT_FULL_GRIP + 1);
		input_sync(ad7146->input);
	}
}

/**
  This function is used to indicate No Grip state by event
  @param ad7146 The AD7146 chip structure pointer
  @return void

 */
static inline void indicatenormalstate(struct ad7146_chip *ad7146)
{
	AD7146_DRIVER_DBG("indicatenormalstate()\n");
	if (ad7146->eventcheck == 1) {
		input_report_rel(ad7146->input, REL_MISC, EVENT_NO_GRIP + 1);
		input_sync(ad7146->input);
	}
}

/**
This Function is used to determine the No Grip and
 enter to the NORMAL mode of operation
  @param ad7146 The Chip Structure.
  @return void - Nothing returned
 */

static void initnormalgrip(struct ad7146_chip *ad7146)
{
	unsigned short data = 0;
	unsigned int lcnt = 0;

	indicatenormalstate(ad7146);
	ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG,
		      DISABLE_INT);
	ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG,
		      DISABLE_INT);

	ad7146->Init_at_Full_Grip = 0;
	ad7146->write(ad7146->dev,
		      AD7146_STG_CAL_EN_REG, DISABLE_INT);
	ad7146->write(ad7146->dev,
		      LOW_OFFSET_0_REG, ad7146->stg0_low_offset);
	for (lcnt = 0; lcnt < (sizeof(ad7146->hw->normal_regs)/sizeof(int));
	     lcnt++) {
		unsigned short addr;
		unsigned short value;
		addr = (unsigned short)((ad7146->hw->normal_regs[lcnt] &
					0xffff0000) >> 16);
		value = (unsigned short)(ad7146->hw->normal_regs[lcnt] &
				0x0000ffff);
		if ((addr == STG_LOW_INT_EN_REG)  ||
		    (addr == STG_HIGH_INT_EN_REG) ||
		    (addr == STG_COM_INT_EN_REG)) {
			value = 0;
		}
		/*Force calibrate - done afterwards*/
		if (addr == AD7146_AMB_COMP_CTRL0_REG)
			value = value & 0xBFFF;
		AD7146_DRIVER_DBG_L2("writing Addr %x Val %x\n", addr, value);
		ad7146->write(ad7146->dev, addr, value);
	}

	data = 0;
	ad7146->read(ad7146->dev,
		     AD7146_AMB_COMP_CTRL0_REG, &data);
	/*Set the 14th bit For Force calibrate*/
	data = data | AD7146_FORCED_CAL_MASK;
	ad7146->write(ad7146->dev,
		      AD7146_AMB_COMP_CTRL0_REG, data);
	ad7146->pw_on_grip_status = DRIVER_STATE_NORMAL;
	msleep(SLEEP_TIME_TO_CALI_INT);
	AD7146_DRIVER_DBG("Forced Calibration in func %s\n", __func__);

	ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG, ENABLE_STG0);
	ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG, ENABLE_STG0);
}

/**
This Function is used in the POWER_ON GRIP detection to
 determine the Full Grip status.
@param ad7146 The Chip Structure.
@return void - Nothing returned
*/

static void initfullgrip(struct ad7146_chip *ad7146)
{
	unsigned int cal_offset;
	unsigned short data = 0;
	unsigned short sensitivity = 0;

	indicatefullgripstate(ad7146);
	ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG,
		      DISABLE_INT);
	ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG,
		      DISABLE_INT);

	ad7146->write(ad7146->dev, AD7146_STG_CAL_EN_REG,
		      DISABLE_INT);
	ad7146->read(ad7146->dev, CDC_RESULT_S0_REG, &data);
	ad7146->read(ad7146->dev, DRIVER_STG0_SENSITIVITY, &sensitivity);

	cal_offset = OFF_CALC(data, ad7146->hw->fixed_th_full,
			      sensitivity, 0);
	ad7146->write(ad7146->dev, LOW_OFFSET_0_REG,
		      (unsigned short)cal_offset);
	AD7146_DRIVER_DBG_L2("CDC 0x%d Lth 0x%d Sens 0x%x ST0_OFF_L 0x%x\n",
			     data, ad7146->hw->fixed_th_full, sensitivity,
			     cal_offset);
	cal_offset = 0;
	sensitivity = 0;

	ad7146->write(ad7146->dev, AD7146_STG_CAL_EN_REG,
		      ENABLE_STG0);
	ad7146->read(ad7146->dev,
		     AD7146_AMB_COMP_CTRL0_REG, &data);
	/*Set the 14th bit For Force calibrate*/
	data = data | AD7146_FORCED_CAL_MASK;
	ad7146->write(ad7146->dev,
		      AD7146_AMB_COMP_CTRL0_REG, data);
	ad7146->pw_on_grip_status = DRIVER_STATE_FULL_GRIP;

	if (ad7146->Init_at_Full_Grip) {
		msleep(SLEEP_TIME_TO_CALI_INIT);
		ad7146->Init_at_Full_Grip = 0;
	} else {
		msleep(SLEEP_TIME_TO_CALI_INT);
	}

	ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG, ENABLE_STG0);

	AD7146_DRIVER_DBG("Forced Calibration in func %s\n", __func__);
}

/**
  This function is used to identify and report the first grip status.
  @param ad7146 The AD7146 chip structure pointer
  @return void - Nothing Returned
*/

static int sensorgrip(struct ad7146_chip *ad7146)
{
	unsigned short conv0_avg = 0;

	mutex_lock(&interrupt_thread_mutex);
	ad7146->read(ad7146->dev, CDC_RESULT_S0_REG, &conv0_avg);
	AD7146_DRIVER_DBG("conv0_avg = 0x%5d\n", conv0_avg);

	if(conv0_avg == 0x0000)
		conv0_avg = 0xFFFF;

	if (conv0_avg < ad7146->hw->fixed_th_full) {
		initnormalgrip(ad7146);
	} else {
		initfullgrip(ad7146);
	}

	mutex_unlock(&interrupt_thread_mutex) ;
	return 0;
}

/**
  This is to configure the device with the register set defined in platform file.
  Finally calibration is done and status registers will be cleared.
 * @param  ad7146 The Device structure
 * @return void  Nothing Returned
 */

static int ad7146_hw_init(struct ad7146_chip *ad7146)
{
	int lcnt = 0;
	unsigned short data = 0;

	mutex_lock(&interrupt_thread_mutex) ;
	/** configuration CDC and interrupts */
	for (lcnt = 0; lcnt < (sizeof(ad7146->hw->regs)/sizeof(int)); lcnt++) {
		unsigned short addr;
		unsigned short value;
		addr = (unsigned short)((ad7146->hw->regs[lcnt] &
					0xffff0000) >> 16);
		value = (unsigned short)(ad7146->hw->regs[lcnt] &
				0x0000ffff);
		AD7146_DRIVER_DBG_L2("Addr %x Val %x\n", addr,
				     value);
		if (addr == AD7146_AMB_COMP_CTRL0_REG)
			value = value & 0xBFFF;
		if (addr == LOW_OFFSET_0_REG)
			ad7146->stg0_low_offset = value;
		ad7146->write(ad7146->dev, addr, value);
	}

	ad7146->read(ad7146->dev,
		     AD7146_AMB_COMP_CTRL0_REG, &data);
	data = data | AD7146_FORCED_CAL_MASK;
	ad7146->write(ad7146->dev,
		      AD7146_AMB_COMP_CTRL0_REG, data);
	msleep(SLEEP_TIME_TO_CALI_INIT);

	ad7146->prev_state_value = -1;
	ad7146->state_value = 0;
	ad7146->prevhigh = 0;

	ad7146->Init_at_Full_Grip = 1;
	AD7146_DRIVER_DBG("Force calibration done\n");
	mutex_unlock(&interrupt_thread_mutex);

	return 0;
}

/**
  This is used to Force Calibrate the device through i2c.
  This functions fore Calibrate the Device AD7146 at run time.

  @param dev The Device Id and Information structure(Linux Standard argument)
  @param attr standard Linux Device attributes to the AD7146
  @param buf The buffer to store the data to be written
  @param count The count of bytes to be transfered to the Device

  \note This is evoked upon an echo request in /../sysfs/<Device> region.
  \note This also prints the results in the console for the user.
  \note The calibration is invoked forcefully upon user request.
  \note Effects are immediate in this calibration.

  @return count of data written
 */
static ssize_t do_calibrate(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct ad7146_chip *ad7146 = dev_get_drvdata(dev);
	int err ;
	unsigned long val;
	unsigned short u16temp;
	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;
	mutex_lock(&interrupt_thread_mutex);

	ad7146->read(ad7146->dev, AD7146_AMB_COMP_CTRL0_REG, &u16temp);
	u16temp = u16temp | AD7146_FORCED_CAL_MASK;
	ad7146->write(ad7146->dev, AD7146_AMB_COMP_CTRL0_REG, u16temp);
	msleep(SLEEP_TIME_TO_CALI_INIT);

	AD7146_DRIVER_DBG("Force calibration done\n");
	mutex_unlock(&interrupt_thread_mutex);
	return count;
}

/**
This Function is used for the creating a Register Dump of the registers of the AD7146.

  @param dev The Device Id and Information structure(Linux Standard argument)
  @param attr standard Linux Device attributes to the AD7146
  @param buf The buffer to store the data to be written
  @param count The count of bytes to be transfered to the Device

  \note This is evoked upon an echo request in /sys/../<Device> region.
  \note This also prints the results in the console for the user.
  @return count of data written
 */
static ssize_t store_dumpregs(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct ad7146_chip *ad7146 = dev_get_drvdata(dev);
	int err;
	unsigned long val;
	unsigned short u16temp;
	unsigned int u32_lpcnt = 0;
	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;
	mutex_lock(&interrupt_thread_mutex);
	pr_info("[AD7146]: Bank 1 register\n");
	for (u32_lpcnt = 0; u32_lpcnt < 0x16; u32_lpcnt++) {
		ad7146->read(ad7146->dev, (unsigned short)u32_lpcnt, &u16temp);
		pr_info("[AD7146]: Reg 0X%x val 0x%x\n",
		       u32_lpcnt, u16temp);
	}

	ad7146->read(ad7146->dev, (unsigned short)0x042, &u16temp);
	pr_info("[AD7146]: Reg 0X0042 val 0x%x\n", u16temp);

	ad7146->read(ad7146->dev, (unsigned short)0x0045, &u16temp);
	pr_info("[AD7146]: Reg 0X0045 val 0x%x\n", u16temp);

	pr_info("[AD7146]: Bank 2 register - Config\n");
	for (u32_lpcnt = 0x080; u32_lpcnt < 0x090; u32_lpcnt++) {
		ad7146->read(ad7146->dev, (unsigned short)u32_lpcnt, &u16temp);
		pr_info("[AD7146]: Reg 0X%x val 0x%x\n",
		       u32_lpcnt, u16temp);
	}

	pr_info("[AD7146]: Bank 3 register - Results\n");
	for (u32_lpcnt = 0x0E0; u32_lpcnt < 0x128; u32_lpcnt++) {
		ad7146->read(ad7146->dev, (unsigned short)u32_lpcnt, &u16temp);
		pr_info("[AD7146]: Reg 0X%x val 0x%x\n",
		       u32_lpcnt, u16temp);
	}

	mutex_unlock(&interrupt_thread_mutex);
	return count;
}



static ssize_t show_interrupt_status(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct ad7146_chip  *ad7146 = dev_get_drvdata(dev);

	ad7146->read(ad7146->dev, STG_LOW_INT_STA_REG,
			&ad7146->low_status);
	ad7146->read(ad7146->dev, STG_HIGH_INT_STA_REG,
			&ad7146->high_status);
	pr_info("[AD7146]: low_status 0X%d high_status 0x%d\n",
		       ad7146->low_status, ad7146->high_status);
	return sprintf(buf, "%d", ad7146->eventcheck);
}
static ssize_t store_interrupt_status(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct ad7146_chip *ad7146 = dev_get_drvdata(dev);
	unsigned long val;
	int err;
	int lcnt = 0, i = 0, u32_lpcnt = 0;
	unsigned short u16temp;
	unsigned short data = 0;
	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	ad7146->write(ad7146->dev,
			DRIVER_STG0_SENSITIVITY, (unsigned short)(val & 0x0000ffff));

	for (i = 0xC000 ; i < 0xFFFF ; i ++) {
		/** configuration CDC and interrupts */
		for (lcnt = 0; lcnt < (sizeof(ad7146->hw->regs)/sizeof(int)); lcnt++) {
			unsigned short addr;
			unsigned short value;
			addr = (unsigned short)((ad7146->hw->regs[lcnt] &
						0xffff0000) >> 16);
			if ((addr == 0x0082) || (addr == 0x008A)) {
				value = i;
			} else {
				value = (unsigned short)(ad7146->hw->regs[lcnt] &
					0x0000ffff);
			}
			ad7146->write(ad7146->dev, addr, value);
		}

		ad7146->read(ad7146->dev,
				AD7146_AMB_COMP_CTRL0_REG, &data);
		data = data | AD7146_FORCED_CAL_MASK;
		ad7146->write(ad7146->dev,
				AD7146_AMB_COMP_CTRL0_REG, data);

		msleep(SLEEP_TIME_TO_CALI_INIT);

		u32_lpcnt = 0x0B;
		ad7146->read(ad7146->dev, (unsigned short)u32_lpcnt, &u16temp);

		if ((23574 < u16temp) && (u16temp < 25574))
			pr_info("[AD7146]: #### offset (0x%4X) %d pin CDC val %5d\n", i, u32_lpcnt - 0x0B, u16temp);

	}

	ad7146->prev_state_value = -1;
	ad7146->state_value = 0;
	ad7146->prevhigh = 0;

	return count;
}


/**
This Function is used to enable or to disable the device the Sysfs attribute is
given as "enable" writing a '0' Disables the device.
While writing a '1' enables the device.

  @param dev The Device Id and Information structure(Linux Standard argument)
  @param attr standard Linux Device attributes to the AD7146.
  @param buf The buffer to store the data to be written.
  @param count The count of bytes to be transfered to the Device.

  \note This is evoked upon an echo request in /sys/../<Device> region.
  \note This also prints the results in the console for the user.

@return count of data written.
*/
static ssize_t store_enable(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct ad7146_chip *ad7146 = dev_get_drvdata(dev);
	int val, err;

	err = kstrtoint(buf, 10, &val);

	if (err < 0) {
		pr_err("%s, kstrtoint failed\n", __func__);
	} else {
		pr_info("%s: enable %d val %d\n", __func__, ad7146->eventcheck, val);
		if ((val == 1) || (val == 0)) {
			if (ad7146->eventcheck != ((unsigned short) val)) {
				ad7146->eventcheck = (unsigned short) val;

				pr_info("%s: Init_at_Full_Grip %d\n", __func__, ad7146->Init_at_Full_Grip);
				if (ad7146->eventcheck == 1) {
					ad7146_hw_init(ad7146);
					msleep(SLEEP_TIME_TO_LOW_POWER);

					enable_irq(ad7146->irq);
					enable_irq_wake(ad7146->irq);

					sensorgrip(ad7146);
				} else {
					disable_irq_wake(ad7146->irq);
					disable_irq(ad7146->irq);

					ad7146->write(ad7146->dev, STG_COM_INT_EN_REG,
						      DISABLE_INT);
					ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG,
						      DISABLE_INT);
					ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG,
						      DISABLE_INT);
					sensorshutdownmode(ad7146);
				}
			}
		}
	}
	return count;
}

/**
This Function is used to show the enabled status of the device.
Status '1' signifies the device is ENABLED,
while the status '0' signifies a DISABLED device.

  @param dev The Device Id and Information structure(Linux Standard argument)
  @param attr standard Linux Device attributes to the AD7146.
  @param buf The buffer to store the data to be written.

  \note This is evoked upon an cat request in /sys/../<Device> region.
  \note This also prints the results in the console for the user.

  @return The count of data written.
*/
static ssize_t show_enable(struct device *dev,
			      struct device_attribute *attr, char *buf)
{
	struct ad7146_chip  *ad7146 = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", ad7146->eventcheck);
}

/**
  This callback_tmrfn is the callback function from MLD state machine
  for the event intimation of the AD7146 device.
  Here the current & previous grip state are compared & the event is sent
  accordingly.

  @param  data unsigned long data
  @return 0 on success

 */
static inline void callback_tmrfn(unsigned long data)
{
	struct ad7146_chip *ad7146 = (struct ad7146_chip *)data;
	short state_value = ad7146->state_value;
	short prev_state_value = ad7146->prev_state_value;

	if (state_value != prev_state_value) {
		ad7146->prev_state_value = ad7146->state_value;
		if (ad7146->eventcheck == 1) {
			input_report_rel(ad7146->input, REL_MISC, (EVENT_NO_GRIP |
				    ((unsigned int)state_value)) + 1);
			input_sync(ad7146->input);
		}
	}
}

/**
  This MLD State Machine is used to determine the current grip state

  @param ad7146 The AD7146 chip structure
  @return 0 on success

 */
static int sldstateMachine(struct ad7146_chip *ad7146)
{
	unsigned short high = (ad7146->high_status & 3);
	unsigned short prevhigh = (ad7146->prevhigh & 3);
	short state_value = ad7146->state_value;

	if ((high == 0) && (prevhigh == 0)) {
		state_value = 0;
		high = 0;
		ad7146->high_status = high;
		ad7146->state_value = state_value;
		return 0;
	}

	if ((high == 1) && (prevhigh == 0)) {
		AD7146_DRIVER_DBG("State = 1e\n");
		state_value = 1;
	}

	if ((high == 0) && (prevhigh == 1)) {
		AD7146_DRIVER_DBG("State = 0x\n");
		state_value = 0;
	}

	prevhigh = high;
	ad7146->high_status = high;
	ad7146->prevhigh    = prevhigh;
	ad7146->state_value = state_value;
	callback_tmrfn((unsigned long)ad7146);
	return 0;
}


/**
 * \fn static int ad7146_hw_detect(struct ad7146_chip *ad7146)
 * This Routine reads the Device ID to confirm the existance
 * of the Device in the System.

 @param  ad7146 The Device structure
 @return 0 on Successful detection of the device,-ENODEV on err.
 */

static int ad7146_hw_detect(struct ad7146_chip *ad7146)
{
	unsigned short data;

	ad7146->read(ad7146->dev, AD7146_PARTID_REG, &data);
	switch (data & 0xFFF0) {
	case AD7146_PARTID:
		ad7146->product = AD7146_PRODUCT_ID;
		ad7146->version = data & 0xF;
		dev_info(ad7146->dev, "[AD7146]: found AD7146 , rev:%d\n",
			 ad7146->version);
		return 0;

	default:
		dev_err(ad7146->dev,
			"[AD7146]: ad7146 Not Found,ID %04x\n", data);
		return -ENODEV;
	}
}

/**
  This function is used check the stage & accordingly calculate the
  hysteresis compensation required.

  @param ad7146 The AD7146 chip structure pointer
  @return void - Nothing returned
*/
static void ad7146_hysteresis_comp(struct ad7146_chip *ad7146)
{
	unsigned short u16_high_threshold = 0;
	unsigned short u16_sf_ambient = 0;
	unsigned int result = 0;

	if (ad7146->high_status > (short)ad7146->prevhigh) {
		if ((ad7146->high_status & 1) && (ad7146->state_value != 1)) {
			ad7146->read(ad7146->dev, DRIVER_STG0_HIGH_THRESHOLD,
					&u16_high_threshold);
			ad7146->read(ad7146->dev, DRIVER_STG0_SF_AMBIENT,
					&u16_sf_ambient);
			result = HYS(u16_sf_ambient, u16_high_threshold);
			ad7146->write(ad7146->dev, DRIVER_STG0_HIGH_THRESHOLD,
					(unsigned short)result);
			AD7146_DRIVER_DBG_L2("N STG0 HT 0x%d->0x%d\n",
					     u16_high_threshold, result);
		}

	} else if (ad7146->high_status < ad7146->prevhigh) {
		if ((!(ad7146->high_status & 1)) &&
		    (ad7146->prevhigh & 1)) {
			ad7146->read(ad7146->dev, DRIVER_STG0_HIGH_THRESHOLD,
					&u16_high_threshold);
			ad7146->read(ad7146->dev, DRIVER_STG0_SF_AMBIENT,
					&u16_sf_ambient);
			result = HYS_POS(u16_sf_ambient, u16_high_threshold);
			ad7146->write(ad7146->dev, DRIVER_STG0_HIGH_THRESHOLD,
					(unsigned short)result);
			AD7146_DRIVER_DBG_L2("P STG0 HT 0x%d->0x%d\n",
					     u16_high_threshold, result);
		}
	}
}
/**
  IRQ Handler -- Handles the Grip & Normal mode of proximity detection
  @param handle The data of the AD7146 Device
  @param irq The Interrupt Request queue to be assigned for the device.

  @return IRQ_HANDLED
 */
static irqreturn_t ad7146_isr(int irq, void *handle)
{
	struct ad7146_chip *ad7146 = handle;
	mutex_lock(&interrupt_thread_mutex) ;

	wake_lock_timeout(&ad7146->grip_wake_lock, 3 * HZ);

	if (!work_pending(&ad7146->work)) {
		schedule_work(&ad7146->work);
	} else {
	/*Cleared the interrupt for future intterupts to occur*/
		ad7146->read(ad7146->dev, STG_LOW_INT_STA_REG,
				&ad7146->low_status);
		ad7146->read(ad7146->dev, STG_HIGH_INT_STA_REG,
				&ad7146->high_status);
	}

	mutex_unlock(&interrupt_thread_mutex);
	return IRQ_HANDLED;
}

/**
  Interrupt work Handler -- Handles the Grip & Normal mode of proximity
  detection from the ISR
  @param work The work structure for the AD7146 chip

  @return void Nothing returned
 */
static void ad7146_interrupt_thread(struct work_struct *work)
{
	struct ad7146_chip *ad7146 =  container_of(work,
						   struct ad7146_chip, work);
	unsigned short data = 0;

	mutex_lock(&interrupt_thread_mutex);

	ad7146->read(ad7146->dev, STG_HIGH_INT_STA_REG,
		     &ad7146->high_status);
	ad7146->read(ad7146->dev, STG_LOW_INT_STA_REG,
		     &ad7146->low_status);
	AD7146_DRIVER_DBG_L2("HS %x LS%x\n", ad7146->high_status,
			     ad7146->low_status);

	if (ad7146->pw_on_grip_status == DRIVER_STATE_FULL_GRIP) {
		if ((ad7146->low_status & 1) == 1) {
				initnormalgrip(ad7146);
		} else {
			ad7146->read(ad7146->dev, CDC_RESULT_S0_REG,
				     &data);
			if (data < ad7146->hw->fixed_th_full)
				initnormalgrip(ad7146);
		}
	} else {
		if (ad7146->low_status != 0) {
			ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG,
				      DISABLE_INT);
			ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG,
				      DISABLE_INT);
			ad7146->read(ad7146->dev, AD7146_AMB_COMP_CTRL0_REG,
				     &data);
			/*Set the 14th bit For Force calibrate*/
			data = data | AD7146_FORCED_CAL_MASK;
			ad7146->write(ad7146->dev, AD7146_AMB_COMP_CTRL0_REG,
				      data);
			msleep(SLEEP_TIME_TO_CALI_INT);
			AD7146_DRIVER_DBG("FCalib in Low INT\n");
			ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG,
				      ENABLE_STG0);
			ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG,
				      ENABLE_STG0);
		} else {

			ad7146_hysteresis_comp(ad7146);
			sldstateMachine(ad7146);
		}
	}

	mutex_unlock(&interrupt_thread_mutex);
}

static ssize_t ad7146_name_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", DEVICE_NAME);
}

static ssize_t ad7146_vendor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", VENDOR_NAME);
}

static ssize_t ad7146_raw_data_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct ad7146_chip *ad7146 = dev_get_drvdata(dev);
	unsigned short u16temp;

	ad7146->read(ad7146->dev, CDC_RESULT_S0_REG, &u16temp);
	pr_info("%s, raw_data : %d\n", __func__, u16temp);

	return snprintf(buf, PAGE_SIZE, "%d\n", (unsigned int)u16temp);
}

static struct device_attribute dev_attr_sensor_name =
	__ATTR(name, S_IRUSR | S_IRGRP, ad7146_name_show, NULL);
static struct device_attribute dev_attr_sensor_vendor =
	__ATTR(vendor, S_IRUSR | S_IRGRP, ad7146_vendor_show, NULL);
static struct device_attribute dev_attr_sensor_raw_data =
	__ATTR(raw_data, S_IRUSR | S_IRGRP, ad7146_raw_data_show, NULL);

static struct device_attribute *ad7146_attrs[] = {
	&dev_attr_sensor_name,
	&dev_attr_sensor_vendor,
	&dev_attr_sensor_raw_data,
	NULL,
};

/**
  Device probe function
  All initialization routines are handled here like the ISR registration,
  Work creation,Input device registration,SYSFS attributes creation etc.

  @param i2c_client the i2c structure of the ad7146 device/client.
  @param i2c_device_id The i2c_device_id for the supported i2c device.

  @return 0 on success,and On failure -ENOMEM, -EINVAL ,etc., will be returned
 */
static int __devinit ad7146_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int ret = -EINVAL;
	struct input_dev *input = NULL;
	struct device *dev = &client->dev;
	struct ad7146_chip *ad7146 = NULL;

	pr_info("%s: called", __func__);

	if (client == NULL) {
		pr_err("%s: Client doesn't exist\n", __func__);
		return ret;
	}

	if (client->irq <= 0) {
		pr_err("%s: IRQ not configured!\n", __func__);
		return ret;
	}

	if (dev->platform_data == NULL) {
		pr_err("%s: Platform Data Not Found\n", __func__);
		return ret;
	}

	ad7146 = kzalloc(sizeof(*ad7146), GFP_KERNEL);
	if (!ad7146) {
		pr_err("%s: Memory allocation fail\n", __func__);
		ret = -ENOMEM;
		return ret;
	}

	ad7146->hw = dev->platform_data;
	ad7146->read = ad7146_i2c_read;
	ad7146->write = ad7146_i2c_write;
	ad7146->dev = dev;

	/* check if the device is existing by reading device id of AD7146 */
	ret = ad7146_hw_detect(ad7146);
	if (ret)
		goto err_kzalloc_mem;

	i2c_set_clientdata(client, ad7146);

	INIT_WORK(&ad7146->work, ad7146_interrupt_thread);

	/*
	 * Allocate and register ad7146 input device
	 */
	input = input_allocate_device();
	if (!input) {
		pr_err("%s: could not allocate input device\n", __func__);
		ret = -ENOMEM;
		goto err_kzalloc_mem;
	}

	ad7146->input = input;
	input_set_drvdata(ad7146->input, ad7146);
	input->name = "grip_sensor";
	set_bit(EV_REL, input->evbit);
	input_set_capability(input, EV_REL, REL_MISC);

	ret = input_register_device(input);
	if (ret) {
		pr_err("%s: could not input_register_device(input);\n", __func__);
		ret = -ENOMEM;
		goto err_input_register_device;
	}

	ad7146->eventcheck = 0;
	pr_info("%s: HIGH_THRESHOLD %d\n", __func__,
	       ad7146->hw->fixed_th_full);

	wake_lock_init(&ad7146->grip_wake_lock, WAKE_LOCK_SUSPEND, "grip_wake_lock");

	ret = gpio_request(client->irq, "gpio_grip_int");
	pr_info("### : gpio request %d\n", client->irq);
	if (ret < 0) {
		pr_err("%s: gpio %d request failed (%d)\n",
		       __func__, client->irq, ret);
		goto err_gpio_request;
	}

	ret = gpio_direction_input(client->irq);
	if (ret < 0) {
		pr_err("%s: failed to set gpio %d as input (%d)\n",
		       __func__, client->irq, ret);
		goto err_free_irq;
	}

	ad7146->irq = gpio_to_irq(client->irq);
	pr_info("%s: irq %d\n", __func__, ad7146->irq);

	ret = request_threaded_irq(ad7146->irq, NULL, ad7146_isr,
			IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
			dev_name(dev), ad7146);

	if (ret) {
		pr_err("%s: irq %d Driver init Failed", __func__, ad7146->irq);
		goto err_free_irq;
	}
	disable_irq(ad7146->irq);


	ret = sysfs_create_group(&input->dev.kobj, &ad7146_attr_group);
	if (ret) {
		pr_err("%s: cound not register sensor device\n", __func__);
		goto err_sysfs_create_input;
	}

	ret = sensors_register(ad7146->grip_dev, ad7146, ad7146_attrs, "grip_sensor");
	if (ret) {
		pr_err("%s: cound not register sensor device\n", __func__);
		goto err_sysfs_create_factory_grip;
	}

	/* initialize and request sw/hw resources */
	sensorshutdownmode(ad7146);

	return 0;

err_sysfs_create_factory_grip:
	sysfs_remove_group(&input->dev.kobj, &ad7146_attr_group);
err_sysfs_create_input:
	pr_info("### : ad7146 %d\n", ad7146->irq);
	free_irq(ad7146->irq, ad7146);
err_free_irq:
	pr_info("### : client %d\n", client->irq);
	gpio_free(client->irq);
err_gpio_request:
	wake_lock_destroy(&ad7146->grip_wake_lock);
	input_unregister_device(input);
err_input_register_device:
	input_free_device(input);
err_kzalloc_mem:
	kfree(ad7146);
	return ret;
}

/**
  Removes the Device.
  This is used to Remove the device or the I2C client from the system

  @param client The Client Id to be removed
  @return 0 on success
 */
static int __devexit ad7146_i2c_remove(struct i2c_client *client)
{
	struct ad7146_chip *ad7146 = i2c_get_clientdata(client);

	pr_info("%s, Start\n", __func__);
	if (ad7146 != NULL) {
		if (ad7146->eventcheck == ENABLE_INTERRUPTS) {
			disable_irq(ad7146->irq);
			disable_irq_wake(ad7146->irq);
			ad7146->write(ad7146->dev, STG_COM_INT_EN_REG, DISABLE_INT);
			ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG, DISABLE_INT);
			ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG, DISABLE_INT);
			msleep(SLEEP_TIME_TO_CALI_INT);
		}
		sysfs_remove_group(&ad7146->input->dev.kobj, &ad7146_attr_group);
		free_irq(ad7146->irq, ad7146);
		gpio_free(client->irq);
		input_unregister_device(ad7146->input);
		input_free_device(ad7146->input);
		kfree(ad7146);
	}
	return 0;
}

void ad7146_shutdown(struct i2c_client *client)
{
	struct ad7146_chip *ad7146 = i2c_get_clientdata(client);
	pr_info("%s, Start\n", __func__);
	if (ad7146 != NULL) {
		if (ad7146->eventcheck == ENABLE_INTERRUPTS) {
			disable_irq(ad7146->irq);
			disable_irq_wake(ad7146->irq);
			ad7146->write(ad7146->dev, STG_COM_INT_EN_REG, DISABLE_INT);
			ad7146->write(ad7146->dev, STG_HIGH_INT_EN_REG, DISABLE_INT);
			ad7146->write(ad7146->dev, STG_LOW_INT_EN_REG, DISABLE_INT);
			msleep(SLEEP_TIME_TO_CALI_INT);
		}
		sysfs_remove_group(&ad7146->input->dev.kobj, &ad7146_attr_group);
		free_irq(ad7146->irq, ad7146);
		gpio_free(client->irq);
		input_unregister_device(ad7146->input);
		input_free_device(ad7146->input);
		kfree(ad7146);
	}
}
/**
Device ID table for the AD7146 driver
*/
static int grip_i2c_suspend(struct device *dev)
{

	return 0;
}

static int grip_i2c_resume(struct device *dev)
{

	return 0;
}


static const struct i2c_device_id ad7146_id[] = {
	{ "ad7146_SAR_NORM", 0 },
	{ "ad7146_SAR_PROX", 1 },
	{ "ad7146_SAR", 2 }, {},
};
MODULE_DEVICE_TABLE(i2c, ad7146_id);

static const struct dev_pm_ops grip_dev_pm_ops = {
	.suspend = grip_i2c_suspend,
	.resume = grip_i2c_resume,
};

/**
  The file Operation Table
 */
struct i2c_driver ad7146_i2c_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.pm = &grip_dev_pm_ops,
	},
	.probe    = ad7146_probe,
	.shutdown = ad7146_shutdown,
	.remove   = __devexit_p(ad7146_i2c_remove),
	.id_table = ad7146_id,
};

/**
  This is an init function called during module insertion --
  calls in turn i2c driver probe function
 */
static __init int ad7146_i2c_init(void)
{
	pr_info("%s, start\n", __func__);
	return i2c_add_driver(&ad7146_i2c_driver);
}
module_init(ad7146_i2c_init);

/**
  Called during the module removal
 */
static __exit void ad7146_i2c_exit(void)
{
	i2c_del_driver(&ad7146_i2c_driver);
}

module_exit(ad7146_i2c_exit);
MODULE_DESCRIPTION("Analog Devices ad7146 MLD Driver");
MODULE_AUTHOR("Analog Devices");
MODULE_LICENSE("GPL");

