/*
 * Driver for keys on GPIO lines capable of generating interrupts.
 *
 * Copyright 2005 Phil Blundell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <asm/gpio.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/earlysuspend.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <mach/irqs.h>
#include <mach/msm8930-gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

#ifdef CONFIG_CPU_FREQ
#include <mach/cpufreq.h>
#endif

/*
Melfas touchkey register
*/
#define KEYCODE_REG 0x00
#define FIRMWARE_VERSION 0x02
#define TOUCHKEY_MODULE_VERSION 0x03
#define TOUCHKEY_ADDRESS	0x20

#define UPDOWN_EVENT_BIT 0x08
#define KEYCODE_BIT 0x07
#define ESD_STATE_BIT 0x10
//#define LED_BRIGHT_BIT 0x14
//#define LED_BRIGHT 0x0C /*0~14*/
#define I2C_M_WR 0 /* for i2c */

#define DEVICE_NAME "melfas-touchkey"
#define INT_PEND_BASE	0xE0200A54

#define MCS5080_CHIP		0x03

#define MCS5080_ver_03	0x03  /*M550 v03*/
#define MCS5080_ver_04	0x04  /*M550 v04*/
#define MCS5080_ver_05	0x05  /*M550 v05*/
#define MCS5080_ver_07	0x07  /*M550 v07*/
#define MCS5080_ver_09	0x09  /*M550 v09*/

// if you want to see log, set this definition to NULL or KERN_WARNING
#define TCHKEY_KERN_DEBUG      KERN_DEBUG
#define _3_TOUCH_SDA_28V GPIO_TOUCHKEY_I2C_SDA
#define _3_TOUCH_SCL_28V GPIO_TOUCHKEY_I2C_SCL
#define _3_GPIO_TOUCH_INT	GPIO_TKEY_INT
#define IRQ_TOUCH_INT gpio_to_irq(GPIO_TKEY_INT)

#define TOUCHKEY_KEYCODE_MENU 	KEY_MENU
#define TOUCHKEY_KEYCODE_HOME 	KEY_HOMEPAGE
#define TOUCHKEY_KEYCODE_BACK 	KEY_BACK
#define TOUCHKEY_KEYCODE_SEARCH 	KEY_END
#define FLIP_CLOSE 0
#define FLIP_OPEN 1

static int touchkey_keycode[5] = {0,KEY_MENU , KEY_HOMEPAGE, KEY_BACK, 0};
static u8 activation_onoff = 1;	// 0:deactivate   1:activate
static u8 is_suspending = 0;
static u8 user_press_on = 0;
static u8 touchkey_dead = 0;
static u8 menu_sensitivity = 0;
static u8 back_sensitivity = 0;
static u8 home_sensitivity = 0;

static int touchkey_enable = 0;
static int led_onoff = 0;
static int ssuepend = -1;
static u8 version_info[3];
static void	__iomem	*gpio_pend_mask_mem;
static int Flip_status=-1;

struct i2c_touchkey_driver {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct work_struct work;
	struct early_suspend	early_suspend;
};
struct i2c_touchkey_driver *touchkey_driver = NULL;
struct workqueue_struct *touchkey_wq;
extern struct class *sec_class;
struct device *sec_touchkey;
struct mutex melfas_tsk_lock;

static const struct i2c_device_id melfas_touchkey_id[] = {
	{"melfas-touchkey", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, melfas_touchkey_id);

extern void get_touchkey_data(u8 *data, u8 length);
extern int system_rev;
static int last_fw_rev =0;

static void init_hw(void);
int touchkey_pmic_control(int onoff);
static int i2c_touchkey_probe(struct i2c_client *client, const struct i2c_device_id *id);
static void melfas_touchkey_switch_early_suspend(int FILP_STATE,int firmup_onoff);
static void melfas_touchkey_switch_early_resume(int FILP_STATE,int firmup_onoff);

struct i2c_driver touchkey_i2c_driver =
{
	.driver = {
		   .name = "melfas-touchkey",
	},
	.id_table = melfas_touchkey_id,
	.probe = i2c_touchkey_probe,
};


static int i2c_touchkey_read(u8 reg, u8 *val, unsigned int len)
{
	int	err=0;
	int	retry = 3;
	struct i2c_msg	msg[1];

	if((touchkey_driver == NULL)||touchkey_dead)
	{
		return -ENODEV;
	}

	while(retry--)
	{
		msg->addr	= touchkey_driver->client->addr;
		msg->flags = I2C_M_RD;
		msg->len   = len;
		msg->buf   = val;
		err = i2c_transfer(touchkey_driver->client->adapter, msg, 1);

		if (err >= 0)
		{
			return 0;
		}
		printk(KERN_ERR "%s %d i2c transfer error\n", __func__, __LINE__);/* add by inter.park */
		mdelay(10);
	}

	return err;

}

static int i2c_touchkey_write(u8 *val, unsigned int len)
{
	int err;
	struct i2c_msg msg[1];

	if((touchkey_driver == NULL)||is_suspending||touchkey_dead)
	{
		return -ENODEV;
	}

	msg->addr = touchkey_driver->client->addr;
	msg->flags = I2C_M_WR;
	msg->len = len;
	msg->buf = val;

	err = i2c_transfer(touchkey_driver->client->adapter, msg, 1);

	if (err >= 0) return 0;

	printk(KERN_ERR "%s %d i2c transfer error\n", __func__, __LINE__);

	return err;
}

static unsigned int touch_state_val;
//extern unsigned int touch_state_val;
extern void TSP_forced_release(void);
void  touchkey_work_func(struct work_struct * p)
{
	u8 data[14];
	int keycode;

	if (Flip_status == FLIP_CLOSE || (!gpio_get_value(_3_GPIO_TOUCH_INT) && !touchkey_dead)) {
		disable_irq_nosync(touchkey_driver->client->irq);
			i2c_touchkey_read(KEYCODE_REG, data, 14);
			keycode = touchkey_keycode[data[0] & KEYCODE_BIT];
			printk(KERN_ERR "[TKEY] %sdata[0] = %d\n",__func__,data[0] & KEYCODE_BIT);
		if(activation_onoff){
			if(data[0] & UPDOWN_EVENT_BIT) // key released
			{
				user_press_on = 0;
				input_report_key(touchkey_driver->input_dev, touchkey_keycode[data[0] &  KEYCODE_BIT], 0);
				input_sync(touchkey_driver->input_dev);
					printk(KERN_ERR "[TKEY] R\n");
			}
			else // key pressed
			{
				if(touch_state_val == 1)
				{
					printk(KERN_ERR "touchkey pressed but don't send event because touch is pressed. \n");
				}
				else
				{
					if(keycode == TOUCHKEY_KEYCODE_BACK)
					{
						user_press_on = 3;
						back_sensitivity = data[5];
					}
					else if(keycode == TOUCHKEY_KEYCODE_MENU)
					{
						user_press_on = 1;
						menu_sensitivity = data[3];
					}
					else if(keycode == TOUCHKEY_KEYCODE_HOME)
					{
						user_press_on = 2;
						home_sensitivity = data[4];
					}
					input_report_key(touchkey_driver->input_dev, keycode,1);
					input_sync(touchkey_driver->input_dev);
						printk(KERN_ERR "[TKEY] P\n");
				}
			}
		}
		enable_irq(touchkey_driver->client->irq);
	}else
		printk(KERN_ERR "[TKEY] not enabled Flip=%d,INT=%d,tkey_dead=%d \n",\
			Flip_status,!gpio_get_value(_3_GPIO_TOUCH_INT),!touchkey_dead);
	return ;
}

static irqreturn_t touchkey_interrupt(int irq, void *dummy)
{
	queue_work(touchkey_wq, &touchkey_driver->work);
	return IRQ_HANDLED;
}

void samsung_switching_tkey(int flip)
{
	if (touchkey_driver == NULL)
		return;

	printk(KERN_ERR "[TKEY] switching_tkey, Flip_status(0 OPEN/1 CLOSE) : %d, flip : %d,%d,%d \n", Flip_status, flip,touchkey_enable,is_suspending);
	
	if (Flip_status != flip)
	{
		Flip_status=flip;
		if (flip == FLIP_CLOSE) {
			if(touchkey_enable != 1 && is_suspending != 1)
			melfas_touchkey_switch_early_resume(flip,0);
		}
		else {
			if(touchkey_enable != 0 && is_suspending != 1)
			melfas_touchkey_switch_early_suspend(flip,0);
			}
	}
}
EXPORT_SYMBOL(samsung_switching_tkey);

static void melfas_touchkey_switch_early_suspend(int FILP_STATE,int firmup_onoff){

	unsigned char data;
	data = 0x02;
	touchkey_enable = 0;
	printk(KERN_DEBUG "[TKEY] switch_early_suspend +++\n");
	mutex_lock(&melfas_tsk_lock);
	if (touchkey_driver == NULL)
		goto end;
	
	if(user_press_on == 1)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_MENU, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release MENU\n");
	}
	else if(user_press_on == 2)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_HOME, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release HOME\n");
	}
	else if(user_press_on == 3)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_BACK, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release BACK\n");
	}
	
	user_press_on = 0;

	i2c_touchkey_write(&data, 1);	//Key LED force off
	if(!firmup_onoff)
	disable_irq(touchkey_driver->client->irq);
	
	gpio_direction_output(_3_TOUCH_SDA_28V, 0);
	gpio_direction_output(_3_TOUCH_SCL_28V, 0);
	
	touchkey_pmic_control(0);
	msleep(30);

	ssuepend = 1;
#ifdef USE_IRQ_FREE
	free_irq(touchkey_driver->client->irq, NULL);
#endif

end:
	mutex_unlock(&melfas_tsk_lock);

	printk(KERN_DEBUG "[TKEY] switch_early_suspend ---\n");
}

static void melfas_touchkey_switch_early_resume(int FILP_STATE,int firmup_onoff){
	unsigned char data1 = 0x01;
//	u8 led_bright[2]= {LED_BRIGHT_BIT,LED_BRIGHT};
	printk(KERN_DEBUG "[TKEY] switch_early_resume +++\n");
	mutex_lock(&melfas_tsk_lock);
	
	if (((Flip_status == FLIP_OPEN) && (!firmup_onoff)) || touchkey_driver == NULL) {
		printk(KERN_DEBUG "[TKEY] FLIP_OPEN ---\n");
		goto end;
		}
	touchkey_pmic_control(1);	
	gpio_set_value(_3_TOUCH_SDA_28V, 1);
	gpio_set_value(_3_TOUCH_SCL_28V, 1);
	msleep(100);

#ifdef USE_IRQ_FREE
	msleep(50);

	printk("%s, %d\n",__func__, __LINE__);
	err = request_threaded_irq(touchkey_driver->client->irq, NULL, touchkey_interrupt,
			IRQF_DISABLED | IRQF_TRIGGER_LOW | IRQF_ONESHOT, "touchkey_int", NULL);
	if (err) {
		printk(KERN_ERR "%s Can't allocate irq .. %d\n", __FUNCTION__, err);
	}
#endif
	if(!firmup_onoff)
	enable_irq(touchkey_driver->client->irq);
	touchkey_enable = 1;
	ssuepend =0;
	if (led_onoff){
//		i2c_touchkey_write(led_bright, 2);
		i2c_touchkey_write(&data1, 1);
	}

end:
	mutex_unlock(&melfas_tsk_lock);
	printk(KERN_DEBUG "[TKEY] switch_early_resume---\n");
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void melfas_touchkey_early_suspend(struct early_suspend *h)
{
	unsigned char data = 0x02;
    printk(KERN_ERR"[TKEY] early_suspend +++\n");
	mutex_lock(&melfas_tsk_lock);
	if(user_press_on == 1)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_MENU, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release MENU\n");
	}
	else if(user_press_on == 2)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_HOME, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release HOME\n");
	}
	else if(user_press_on == 3)	{
		input_report_key(touchkey_driver->input_dev, TOUCHKEY_KEYCODE_BACK, 0);
		input_sync(touchkey_driver->input_dev);
		printk("[TKEY] force release BACK\n");
	}
	user_press_on = 0;

	if ((touchkey_enable == 0 && is_suspending == 1) || ssuepend == 1) {
	printk(KERN_ERR"[TKEY] already suspend %d,%d,%d---\n",touchkey_enable,is_suspending,ssuepend);
	is_suspending = 1;
	goto end;
	}

	touchkey_enable = 0;
	is_suspending = 1;
	ssuepend = 0;
	led_onoff =0;
	

	if(touchkey_dead)
	{
		printk(KERN_ERR "touchkey died after ESD");
		goto end;
	}
	i2c_touchkey_write(&data, 1);	/*Key LED force off*/
	disable_irq(touchkey_driver->client->irq);
	gpio_set_value(_3_TOUCH_SDA_28V, 0);
	gpio_set_value(_3_TOUCH_SCL_28V, 0);
	touchkey_pmic_control(0);
	msleep(30);

end:
	mutex_unlock(&melfas_tsk_lock);
    printk(KERN_ERR"[TKEY] early_suspend ---\n");
}

static void melfas_touchkey_early_resume(struct early_suspend *h)
{
	unsigned char data1 = 0x01;
//	u8 led_bright[2]= {LED_BRIGHT_BIT,LED_BRIGHT};
    printk(KERN_ERR"[TKEY] early_resume +++\n");
	mutex_lock(&melfas_tsk_lock);

	if((touchkey_enable == 1 && is_suspending == 0) || Flip_status == 1) {
	 printk(KERN_ERR"[TKEY] already resume or FLIP open %d,%d,%d---\n",touchkey_enable,is_suspending,Flip_status);
	 is_suspending = 0;
	 ssuepend = 1;
	 goto end;
	}

	touchkey_enable = 1;
	is_suspending = 0;
	ssuepend = 0;
	
	if(touchkey_dead)
	{
		printk(KERN_ERR "touchkey died after ESD");
		goto end;
	}
	touchkey_pmic_control(1);
	gpio_set_value(_3_TOUCH_SDA_28V, 1);
	gpio_set_value(_3_TOUCH_SCL_28V, 1);
	msleep(100);
	
	enable_irq(touchkey_driver->client->irq);
	if (led_onoff){
//		i2c_touchkey_write(led_bright, 2);
		i2c_touchkey_write(&data1, 1);
	}
end:
	mutex_unlock(&melfas_tsk_lock);
   printk(KERN_ERR"[TKEY] early_resume ---\n");
}
#endif	// End of CONFIG_HAS_EARLYSUSPEND

extern int mcsdl_download_binary_data(u8 chip_ver);

static int i2c_touchkey_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct input_dev *input_dev;
	int err = 0;
printk(KERN_ERR "[TKEY] %s\n",__func__);
	printk("melfas touchkey probe called!\n");
	touchkey_driver = kzalloc(sizeof(struct i2c_touchkey_driver), GFP_KERNEL);
	if (touchkey_driver == NULL)
	{
		dev_err(dev, "failed to create our state\n");
		return -ENOMEM;
	}

	touchkey_driver->client = client;

	touchkey_driver->client->irq = IRQ_TOUCH_INT;

	strlcpy(touchkey_driver->client->name, "melfas-touchkey", I2C_NAME_SIZE);

	input_dev = input_allocate_device();

	if (!input_dev)
		return -ENOMEM;

	touchkey_driver->input_dev = input_dev;

	input_dev->name = DEVICE_NAME;
	input_dev->phys = "melfas-touchkey/input0";
	input_dev->id.bustype = BUS_HOST;

	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_LED, input_dev->evbit);
	set_bit(LED_MISC, input_dev->ledbit);
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(touchkey_keycode[1], input_dev->keybit);
	set_bit(touchkey_keycode[2], input_dev->keybit);
	set_bit(touchkey_keycode[3], input_dev->keybit);
	set_bit(touchkey_keycode[4], input_dev->keybit);

	mutex_init(&melfas_tsk_lock);

	err = input_register_device(input_dev);
	if (err)
	{
		input_free_device(input_dev);
		return err;
	}

	gpio_pend_mask_mem = ioremap(INT_PEND_BASE, 0x10);
	touchkey_wq = create_singlethread_workqueue("melfas_touchkey_wq");
	if (!touchkey_wq)
		return -ENOMEM;

	INIT_WORK(&touchkey_driver->work, touchkey_work_func);

#ifdef CONFIG_HAS_EARLYSUSPEND
	touchkey_driver->early_suspend.suspend = melfas_touchkey_early_suspend;
	touchkey_driver->early_suspend.resume = melfas_touchkey_early_resume;
	register_early_suspend(&touchkey_driver->early_suspend);
#endif				/* CONFIG_HAS_EARLYSUSPEND */
	touchkey_enable = 1;
	if (request_irq(touchkey_driver->client->irq, touchkey_interrupt, IRQF_DISABLED, DEVICE_NAME, touchkey_driver))
	{
		printk(KERN_ERR "%s Can't allocate irq ..\n", __FUNCTION__);
		return -EBUSY;
	}
	return 0;
}
int touchkey_pmic_control(int onoff)
{
	static struct regulator *reg_l29;
	static struct regulator *reg_l30;

	int ret = 0;
	printk(KERN_ERR "%s: power %s\n", __func__, onoff ? "on" : "off");
	
	if (!reg_l29) {
		reg_l29 = regulator_get(NULL, "8921_l29");
		if (IS_ERR(reg_l29)) {
			pr_err("%s: could not get 8917_l29, rc = %ld\n",
				__func__, PTR_ERR(reg_l29));
			return 1;
		}
		ret = regulator_set_voltage(reg_l29, 1800000, 1800000);
		if (ret) {
			pr_err("%s: unable to set l29 voltage to 1.8V\n",
				__func__);
			return ret;
		}
	}
	if (!reg_l30) {
		reg_l30 = regulator_get(NULL, "8917_l30");
		if (IS_ERR(reg_l30)) {
			pr_err("%s: could not get 8917_l30, rc = %ld\n",
				__func__, PTR_ERR(reg_l30));
			return 1;
		}
		ret = regulator_set_voltage(reg_l30, 3300000, 3300000);
		if (ret) {
			pr_err("%s: unable to set l30 voltage to 3.3V\n",
				__func__);
			return ret;
		}
	}
	if (onoff) {
		ret = regulator_enable(reg_l29);
		if (ret) {
			pr_err("%s: enable l29 failed, rc=%d\n",
				__func__, ret);
			return ret;
		}
		ret = regulator_enable(reg_l30);
		if (ret) {
			pr_err("%s: enable l30 failed, rc=%d\n",
				__func__, ret);
			return ret;
		}
	} else {
		if (regulator_is_enabled(reg_l29))
			ret = regulator_disable(reg_l29);
		else
			printk(KERN_ERR
				"%s: rugulator L29(1.8V) is disabled\n",
					__func__);
		if (ret) {
			pr_err("%s: disable l29 failed, rc=%d\n",
				__func__, ret);
			return ret;
		}
		if (regulator_is_enabled(reg_l30))
			ret = regulator_disable(reg_l30);
		else
			printk(KERN_ERR
				"%s: rugulator L30(3.3V) is disabled\n",
					__func__);
		if (ret) {
			pr_err("%s: disable l30 failed, rc=%d\n",
				__func__, ret);
			return ret;
		}
	}
	return 0;	
}
static void init_hw(void)
{
printk(KERN_ERR "[TKEY] %s\n",__func__);
	touchkey_pmic_control(1);

	gpio_tlmm_config(GPIO_CFG(GPIO_TKEY_INT, 0,
			GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), 1);

	irq_set_irq_type(IRQ_TOUCH_INT, IRQF_TRIGGER_FALLING);
}


int touchkey_update_open (struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t touchkey_update_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

ssize_t touchkey_update_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return count;
}

int touchkey_update_release (struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t touchkey_activation_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	printk(TCHKEY_KERN_DEBUG "called %s\n", __func__);
	sscanf(buf, "%hhu", &activation_onoff);
	printk(TCHKEY_KERN_DEBUG "deactivation test = %d\n", activation_onoff);
	return size;
}

static ssize_t touchkey_version_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	printk(TCHKEY_KERN_DEBUG "called %s\n", __func__);
	return sprintf(buf, "0x%02x\n", version_info[1]);
}

static ssize_t touchkey_recommend_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	u8 recommended_ver;
	printk(TCHKEY_KERN_DEBUG "called %s\n", __func__);
	recommended_ver = last_fw_rev;

	return sprintf(buf, "0x%02x\n", recommended_ver);
}

static ssize_t touchkey_firmup_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
	if(Flip_status){
		printk(KERN_ERR "[TKEY] flip opened\n");
		melfas_touchkey_switch_early_resume(Flip_status,1);
	}else{
		disable_irq(touchkey_driver->client->irq);
	}
	printk(TCHKEY_KERN_DEBUG "Touchkey firm-up start!\n");
	get_touchkey_data(version_info, 3);
	printk(TCHKEY_KERN_DEBUG "F/W version: 0x%x, Module version:0x%x\n", version_info[1], version_info[2]);
	if ((version_info[1] < last_fw_rev) || (version_info[1] == 0xff)){
		mdelay(350);
		mcsdl_download_binary_data(MCS5080_CHIP);
		mdelay(100);
		get_touchkey_data(version_info, 3);
		printk(TCHKEY_KERN_DEBUG "Updated F/W version: 0x%x, Module version:0x%x\n", version_info[1], version_info[2]);
	}
	else
		printk(KERN_ERR "Touchkey IC module is new, can't update!");
	if(Flip_status){
		printk(KERN_ERR "[TKEY] flip opened\n");
		melfas_touchkey_switch_early_suspend(Flip_status,1);
	}else{
		enable_irq(touchkey_driver->client->irq);
	}
	return size;
}

static ssize_t touchkey_init_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	printk(TCHKEY_KERN_DEBUG"called %s \n", __func__);
	return sprintf(buf, "%d\n", touchkey_dead);
}

static ssize_t touchkey_menu_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	printk(TCHKEY_KERN_DEBUG "called %s \n", __func__);
	return sprintf(buf, "%d\n", menu_sensitivity);
}
static ssize_t touchkey_home_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	printk(TCHKEY_KERN_DEBUG "called %s \n", __func__);
	return sprintf(buf, "%d\n", home_sensitivity);
}

static ssize_t touchkey_back_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	printk(TCHKEY_KERN_DEBUG "called %s \n", __func__);
	return sprintf(buf, "%d\n", back_sensitivity);
}

static ssize_t touch_led_control(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	u8 data;
//	u8 led_bright[2]= {LED_BRIGHT_BIT,LED_BRIGHT};
	//data = 1 on, 0 off
	sscanf(buf, "%hhu", &data);
	led_onoff = (data == 0) ? 0 : 1;
	data = (data == 0) ? 2 : 1;
	printk(KERN_ERR "%s,led_onoff:%d,data:%d\n",__func__,led_onoff,data);
	if (!touchkey_enable)
		return -1;
//	i2c_touchkey_write(led_bright, 2);
	i2c_touchkey_write(&data, 1);	// LED on(data=1) or off(data=2)
	return size;
}

static ssize_t touchkey_enable_disable(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	// this function is called when platform shutdown thread begins
	printk(TCHKEY_KERN_DEBUG "called %s %c \n",__func__, *buf);
	if(*buf == '0')
	{
		is_suspending = 1;
	    disable_irq(touchkey_driver->client->irq);
	}
	else
	{
	    printk(KERN_ERR "%s: unknown command %c \n",__func__, *buf);
	}

        return size;
}

static ssize_t touchkey_raw_data0_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	u8 data[14];
	int raw_data=0;
	if (Flip_status == FLIP_CLOSE || (!gpio_get_value(_3_GPIO_TOUCH_INT) && !touchkey_dead)) {
			i2c_touchkey_read(KEYCODE_REG, data, 14);
			raw_data = data[3] + data[10];
	}else
		printk(KERN_ERR "[TKEY] not enabled Flip=%d,INT=%d,tkey_dead=%d \n",\
			Flip_status,!gpio_get_value(_3_GPIO_TOUCH_INT),!touchkey_dead);

	return sprintf(buf, "%d\n", raw_data);
}
static ssize_t touchkey_raw_data1_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	u8 data[14];
	int raw_data=0;
	if (Flip_status == FLIP_CLOSE || (!gpio_get_value(_3_GPIO_TOUCH_INT) && !touchkey_dead)) {
			i2c_touchkey_read(KEYCODE_REG, data, 14);
			raw_data = data[5] + data[12];
	}else
		printk(KERN_ERR "[TKEY] not enabled Flip=%d,INT=%d,tkey_dead=%d \n",\
			Flip_status,!gpio_get_value(_3_GPIO_TOUCH_INT),!touchkey_dead);

	return sprintf(buf, "%d\n", raw_data);
}
static ssize_t touchkey_raw_data2_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	u8 data[14];
	int raw_data=0;
	if (Flip_status == FLIP_CLOSE || (!gpio_get_value(_3_GPIO_TOUCH_INT) && !touchkey_dead)) {
			i2c_touchkey_read(KEYCODE_REG, data, 14);
			raw_data = data[4] + data[11];
	}else
		printk(KERN_ERR "[TKEY] not enabled Flip=%d,INT=%d,tkey_dead=%d \n",\
			Flip_status,!gpio_get_value(_3_GPIO_TOUCH_INT),!touchkey_dead);

	return sprintf(buf, "%d\n", raw_data);

}
static ssize_t touchkey_threshold_show(struct device *dev,struct device_attribute *attr, char *buf)
{
	u8 data[14];
	int key_threshold=0;
	if (Flip_status == FLIP_CLOSE || (!gpio_get_value(_3_GPIO_TOUCH_INT) && !touchkey_dead)) {
			i2c_touchkey_read(KEYCODE_REG, data, 14);
			key_threshold = data[13];
	}else
		printk(KERN_ERR "[TKEY] not enabled Flip=%d,INT=%d,tkey_dead=%d \n",\
			Flip_status,!gpio_get_value(_3_GPIO_TOUCH_INT),!touchkey_dead);

	return sprintf(buf, "%d\n", key_threshold);
}
static DEVICE_ATTR(touchkey_activation, 0664, NULL, touchkey_activation_store);
static DEVICE_ATTR(touchkey_firm_version_panel, S_IRUGO, touchkey_version_show, NULL);
static DEVICE_ATTR(touchkey_firm_version_phone, S_IRUGO, touchkey_recommend_show, NULL);
static DEVICE_ATTR(recommended_version, S_IRUGO, touchkey_recommend_show, NULL);
static DEVICE_ATTR(touchkey_firm_update, S_IRUGO | S_IWUSR | S_IWGRP, NULL, touchkey_firmup_store);
static DEVICE_ATTR(touchkey_init, S_IRUGO, touchkey_init_show, NULL);
static DEVICE_ATTR(touchkey_menu, S_IRUGO, touchkey_menu_show, NULL);
static DEVICE_ATTR(touchkey_back, S_IRUGO, touchkey_back_show, NULL);
static DEVICE_ATTR(touchkey_home, S_IRUGO, touchkey_home_show, NULL);
static DEVICE_ATTR(brightness, 0664, NULL, touch_led_control);
static DEVICE_ATTR(enable_disable, 0664, NULL, touchkey_enable_disable);

static DEVICE_ATTR(touchkey_raw_data0, S_IRUGO, touchkey_raw_data0_show, NULL);
static DEVICE_ATTR(touchkey_raw_data1, S_IRUGO, touchkey_raw_data1_show, NULL);
static DEVICE_ATTR(touchkey_raw_data2, S_IRUGO, touchkey_raw_data2_show, NULL);
static DEVICE_ATTR(touchkey_threshold, S_IRUGO, touchkey_threshold_show, NULL);


static int __init touchkey_init(void)
{

	int ret = 0;

	u8 updated = 0;
	printk(KERN_ERR "[TKEY] %s\n",__func__);
	sec_touchkey=device_create(sec_class, NULL, 0, NULL, "sec_touchkey");

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_activation) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_activation.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_firm_version_panel) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_firm_version_panel.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_firm_version_phone) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_firm_version_phone.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_recommended_version) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_recommended_version.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_firm_update) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_firm_update.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_init) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_init.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_menu) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_menu.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_back) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_back.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_home) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_home.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_brightness) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_brightness.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_enable_disable) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_enable_disable.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_raw_data0) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_raw_data0.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_raw_data1) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_raw_data1.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_raw_data2) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_raw_data2.attr.name);

	if (device_create_file(sec_touchkey, &dev_attr_touchkey_threshold) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_touchkey_threshold.attr.name);

	init_hw();

	msleep(100);

	get_touchkey_data(version_info, 3);

//-------------------   Auto Firmware Update Routine Start   -------------------//
	if(system_rev < 0x08)
		last_fw_rev = MCS5080_ver_07;
	else
		last_fw_rev = MCS5080_ver_09;
	printk(TCHKEY_KERN_DEBUG "%s F/W version: 0x%x, Module version:0x%x,last_fw_rev:0x%x\n",__FUNCTION__, version_info[1], version_info[2],last_fw_rev);
		if (version_info[1] == 0xff) {
			mdelay(350);
			mcsdl_download_binary_data(MCS5080_CHIP);
			updated = 1;
		}
		else
		{
		if (version_info[1] < last_fw_rev)
				{
					printk(KERN_ERR"Touchkey IC F/W update start!!");
					mdelay(350);
					mcsdl_download_binary_data(MCS5080_CHIP);
					mdelay(100);
					updated = 1;
				} else
					printk(KERN_ERR"Touchkey IC F/W is last, can't update!");
			if (updated)
			{
				get_touchkey_data(version_info, 3);
				printk(TCHKEY_KERN_DEBUG "Updated F/W version: 0x%x, Module version:0x%x\n", version_info[1], version_info[2]);
			}
		}
//-------------------   Auto Firmware Update Routine End   -------------------//

	ret = i2c_add_driver(&touchkey_i2c_driver);

	if(ret||(touchkey_driver==NULL))
	{
		touchkey_dead = 1;
		printk("ret = %d, touch_driver= %p:", ret, touchkey_driver);
		printk(KERN_ERR
		       "melfas touch keypad registration failed, module not inserted.ret= %d\n",
		       ret);
	}

	return ret;
}

static void __exit touchkey_exit(void)
{
printk(KERN_ERR "[TKEY] %s\n",__func__);
	i2c_del_driver(&touchkey_i2c_driver);
	if (touchkey_wq)
		destroy_workqueue(touchkey_wq);
	mutex_destroy(&melfas_tsk_lock);
	gpio_free(_3_GPIO_TOUCH_INT);
}

module_init(touchkey_init);
module_exit(touchkey_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("@@@");
MODULE_DESCRIPTION("melfas touch keypad");
