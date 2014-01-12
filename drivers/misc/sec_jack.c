/*
 *  headset/ear-jack device detection driver.
 *
 *  Copyright (C) 2010 Samsung Electronics Co.Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/sysdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/switch.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/wakelock.h>
#include <linux/slab.h>
#include <linux/sec_jack.h>

#define MODULE_NAME "sec_jack:"
#define MAX_ZONE_LIMIT		10
#define SEND_KEY_CHECK_TIME_MS	60
#define DET_CHECK_TIME_MS	100
#define WAKE_LOCK_TIME		(HZ * 5)
#define WAKE_LOCK_TIME_IN_SENDKEY (HZ * 1)

#define CONFIG_SYSFS_SEC_SND_JACK
#if defined (CONFIG_MACH_MELIUS_EUR_OPEN) || defined (CONFIG_MACH_SERRANO_ATT) || defined(CONFIG_MACH_SERRANO_VZW) || defined(CONFIG_MACH_SERRANO_USC) || defined(CONFIG_MACH_SERRANO_LRA)
extern unsigned int system_rev;
#endif

#if defined (CONFIG_MACH_MELIUS_CHN_CTC)
static int nv_hw_revision=0;
#endif

static bool recheck_jack;
struct switch_dev switch_jack_detection = {
	.name = "h2w",     /* /sys/class/switch/h2w/state */
};

/* To support samsung factory test */
struct switch_dev switch_sendend = {
	.name = "send_end", /* /sys/class/switch/send_end/state */
};

struct sec_jack_info {
	struct sec_jack_platform_data *pdata;
	struct input_dev *input;
	struct wake_lock det_wake_lock;
	struct sec_jack_zone *zone;
	int keypress_code;
	bool send_key_pressed;
	bool send_key_irq_enabled;
	unsigned int cur_jack_type;
	struct work_struct  det_work;
	struct work_struct  sendkey_work;
	struct delayed_work  powerup_work;
	bool is_ready;
};

struct adc_queue {
	int array[30];
	int head, tail;
};

static struct adc_queue *init_adc_queue(void)
{
	struct adc_queue *adc_q;

	adc_q = kzalloc(sizeof(struct adc_queue), GFP_KERNEL);
	adc_q->head = (int)NULL;
	adc_q->tail = (int)NULL;

	return adc_q;
}

static void add_adc_queue(struct adc_queue *adc_q, int adc)
{
	if (adc_q->tail > 30 && adc_q->head == 0) {
		pr_err("%s : adc queue is full\n", __func__);
		return;
	}

	adc_q->array[adc_q->tail] = adc;
	(adc_q->tail)++;
}

static void print_adc_queue(struct adc_queue *adc_q)
{
	int i = 0;

	pr_info("%s : adc read count (%d)\n", __func__, adc_q->tail);

	if (adc_q->tail == adc_q->head) {
		pr_err("%s : adc queue is empty\n", __func__);
		kfree(adc_q);
		return;
	}

	for (i = adc_q->head; i < adc_q->tail; i++) {
		printk(KERN_CONT "%d\t", adc_q->array[adc_q->head]);
		(adc_q->head)++;
	}

	printk(KERN_CONT "\n");
	kfree(adc_q);
}

static void set_send_key_state(struct sec_jack_info *hi, int state)
{
	struct sec_jack_platform_data *pdata = hi->pdata;
	struct sec_jack_buttons_zone *btn_zones = pdata->buttons_zones;
	int adc;
	int i;

	adc = pdata->get_adc_value();
	pr_info(MODULE_NAME "%s adc=%d, state=%d\n", __func__, adc, state);

#if defined(CONFIG_MACH_MELIUS_CHN_CTC)
	if (nv_hw_revision > 7 || nv_hw_revision < 2)
		btn_zones = pdata->buttons_zones_rev08;
#elif defined(CONFIG_MACH_MELIUS_EUR_OPEN)
	if (system_rev == 10)
		btn_zones = pdata->buttons_zones_rev06;
#elif defined (CONFIG_MACH_SERRANO_ATT) || defined(CONFIG_MACH_SERRANO_VZW) || defined(CONFIG_MACH_SERRANO_USC) || defined(CONFIG_MACH_SERRANO_LRA)
	if (system_rev > 2)
		btn_zones = pdata->buttons_zones_rev03;
#endif
	if (state != 0) {
		for (i = 0; i < pdata->num_buttons_zones; i++)
			if (adc >= btn_zones[i].adc_low &&
			    adc <= btn_zones[i].adc_high) {
				hi->keypress_code = btn_zones[i].code;
				input_report_key(hi->input,
						 btn_zones[i].code, state);
				input_sync(hi->input);
				switch_set_state(&switch_sendend, state);
				hi->send_key_pressed = state;
				pr_info(MODULE_NAME "%s: keycode=%d, is pressed\n",
					__func__, btn_zones[i].code);
				return;
			}
	} else {
		input_report_key(hi->input, hi->keypress_code, state);
		input_sync(hi->input);
		switch_set_state(&switch_sendend, state);
		hi->send_key_pressed = state;
	}
}

static void sec_jack_set_type(struct sec_jack_info *hi, int jack_type)
{
	struct sec_jack_platform_data *pdata = hi->pdata;

	/* this can happen during slow inserts where we think we identified
	 * the type but then we get another interrupt and do it again
	 */

	if (jack_type == hi->cur_jack_type) {
		pr_err(MODULE_NAME "%s return, same type reason %d\n",
			__func__, jack_type);
		return;
	}

	if (jack_type == SEC_HEADSET_4POLE) {
		/* for a 4 pole headset, enable irq
		   for detecting send/end key presses */
		if (!hi->send_key_irq_enabled) {
			pr_info(MODULE_NAME "%s send_int enabled\n", __func__);
			enable_irq(pdata->send_int);
			enable_irq_wake(pdata->send_int);
			hi->send_key_irq_enabled = 1;
		}
	} else {
		/* for all other jacks, disable send/end irq */
		if (hi->send_key_irq_enabled) {
			pr_info(MODULE_NAME "%s send_int disabled\n", __func__);
			disable_irq(pdata->send_int);
			disable_irq_wake(pdata->send_int);
			hi->send_key_irq_enabled = 0;
		}
		if (hi->send_key_pressed) {
			set_send_key_state(hi, 0);
			pr_info(MODULE_NAME "%s : BTN set released by jack switch to %d\n",
					__func__, jack_type);
		}
	}

	pr_info(MODULE_NAME "%s : jack_type = %d\n", __func__, jack_type);
	/* prevent suspend to allow user space to respond to switch */
	wake_lock_timeout(&hi->det_wake_lock, WAKE_LOCK_TIME);

	hi->cur_jack_type = jack_type;
	switch_set_state(&switch_jack_detection, jack_type);

	/* micbias is left enabled for 4pole and disabled otherwise */
	pdata->set_micbias_state(hi->send_key_irq_enabled);

}

static void handle_jack_not_inserted(struct sec_jack_info *hi)
{
	pr_info("%s\n", __func__);
	sec_jack_set_type(hi, SEC_JACK_NO_DEVICE);
	hi->pdata->set_micbias_state(false);
}

static void determine_jack_type(struct sec_jack_info *hi)
{
	struct sec_jack_zone *zones = hi->pdata->zones;
	struct sec_jack_platform_data *pd = hi->pdata;
	int size = hi->pdata->num_zones;
	int count[MAX_ZONE_LIMIT] = {0};
	int adc;
	int i;

	struct adc_queue *adc_q = init_adc_queue();
#if defined (CONFIG_MACH_MELIUS_CHN_CTC)
	if (nv_hw_revision > 7 || nv_hw_revision < 2)
		zones = pd->zones_rev08;
#elif defined (CONFIG_MACH_SERRANO_ATT) || defined(CONFIG_MACH_SERRANO_VZW) || defined(CONFIG_MACH_SERRANO_USC) || defined(CONFIG_MACH_SERRANO_LRA)
	if (system_rev > 2)
		zones = pd->zones_rev03;
#endif
	while (pd->get_det_jack_state()) {
#ifdef CONFIG_SAMSUNG_JACK_ADC_SCALE3
		adc = pd->get_ear_adc_value();
#else
		adc = pd->get_adc_value();
#endif
		if (adc < 0)
			break;

		add_adc_queue(adc_q, adc);

		/* determine the type of headset based on the
		 * adc value.  An adc value can fall in various
		 * ranges or zones.  Within some ranges, the type
		 * can be returned immediately.  Within others, the
		 * value is considered unstable and we need to sample
		 * a few more types (up to the limit determined by
		 * the range) before we return the type for that range.
		 */
		for (i = 0; i < size; i++) {
			if (adc <= zones[i].adc_high) {
				if (++count[i] > zones[i].check_count) {
					print_adc_queue(adc_q);
					pr_debug(MODULE_NAME "determine_jack_type %d, %d, %d\n",
						zones[i].adc_high, count[i],
						zones[i].check_count);
#if defined(CONFIG_SAMSUNG_JACK_GNDLDET)
					/* G plus L Detection */
					if (!hi->pdata->get_gnd_jack_state()) {
						pr_err("%s : err 2nd det jack state\n",
							__func__);
						handle_jack_not_inserted(hi);
						recheck_jack = false;
						return;
					}
#else
					if (recheck_jack == true && i == 3) {
						pr_err(MODULE_NAME "something wrong connectoin!\n");
						handle_jack_not_inserted(hi);
						recheck_jack = false;
						return;
					}
#endif
					sec_jack_set_type(hi,
							zones[i].jack_type);
					/* mic_bias remains enabled
					 * in race condition.
					 */
					if (hi->cur_jack_type !=
							SEC_HEADSET_4POLE) {
						pd->set_micbias_state(false);
						pr_info(MODULE_NAME "forced mic_bias disable\n");
					}
					recheck_jack = false;
					return;
				}
#if defined CONFIG_MACH_GOLDEN
				msleep(zones[i].delay_ms);
#else
				usleep_range(zones[i].delay_ms*1000, zones[i].delay_ms*1000);
#endif
				break;
			}
		}
	}
	/* jack removed before detection complete */
	recheck_jack = false;
	pr_err("%s : err det jack state\n", __func__);
	print_adc_queue(adc_q);
	handle_jack_not_inserted(hi);
}

#ifdef CONFIG_SYSFS_SEC_SND_JACK
static ssize_t  key_state_onoff_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_jack_info *hi = dev_get_drvdata(dev);
	int value = 0;

	if (hi->send_key_pressed != true)
		value = 0;
	else
		value = 1;
	return snprintf(buf, sizeof(buf), "%d\n", value);
}

static DEVICE_ATTR(key_state, 0664 , key_state_onoff_show,
		NULL);

static ssize_t  earjack_state_onoff_show(struct device *dev,
		struct device_attribute *attr, char *buf)

{
	struct sec_jack_info *hi = dev_get_drvdata(dev);
	int value = 0;

	if (hi->cur_jack_type == SEC_HEADSET_4POLE)
		value = 1;
	else
		value = 0;
	return snprintf(buf, sizeof(buf), "%d\n", value);
}

static DEVICE_ATTR(state, 0664 , earjack_state_onoff_show,
		NULL);

#ifdef CONFIG_SYSFS_SEC_SND_SELECT_JACK
static ssize_t select_jack_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("%s : operate nothing\n", __func__);
	return 0;
}

static ssize_t select_jack_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct sec_jack_info *hi = dev_get_drvdata(dev);
	struct sec_jack_platform_data *pdata = hi->pdata;
	int value = 0;


	sscanf(buf, "%d", &value);
	pr_err("%s: User  selection : 0X%x", __func__, value);
	if (value == SEC_HEADSET_4POLE) {
		pdata->set_micbias_state(true);
		msleep(100);
	}

	sec_jack_set_type(hi, value);

	return size;
}

static DEVICE_ATTR(select_jack, 0664, select_jack_show,
		select_jack_store);

static ssize_t reselect_jack_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("%s : operate nothing\n", __func__);
	return 0;
}

static ssize_t reselect_jack_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct sec_jack_info *hi = dev_get_drvdata(dev);
	int value = 0;


	sscanf(buf, "%d", &value);
	pr_err("%s: User  selection : 0X%x", __func__, value);

	if (value == 1) {
		recheck_jack = true;
		determine_jack_type(hi);
	}

	return size;
}

static DEVICE_ATTR(reselect_jack, 0664, reselect_jack_show,
		reselect_jack_store);
#endif
#endif

#if defined (CONFIG_MACH_MELIUS_CHN_CTC)
static ssize_t sec_jack_set_nv_hw_ver_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("%s : version info read [%d]\n", __func__, nv_hw_revision);
	return 0;
}

static ssize_t sec_jack_set_nv_hw_ver_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	nv_hw_revision = 10*(buf[5]- '0') +(buf[6] - '0');
//	pr_info("%s : version info 1st[%c] 2nd[%c]\n", __func__, buf[5], buf[6]);

	if((nv_hw_revision < 15 ) && (nv_hw_revision >= 0)) {
		pr_info("%s : version info valid  [%s] out val[%d]\n", __func__, buf, nv_hw_revision);
	}
	else {
		nv_hw_revision = 0;
		pr_info("%s : version info wrong[%s]out val [%d]\n", __func__, buf, nv_hw_revision);
	}
	return size;
}

static DEVICE_ATTR(set_hw_rev, 0664, sec_jack_set_nv_hw_ver_show,
		sec_jack_set_nv_hw_ver_store);
#endif

static irqreturn_t sec_jack_send_key_irq_handler(int irq, void *handle)
{
	struct sec_jack_info *hi = (struct sec_jack_info *)handle;

	pr_info(MODULE_NAME "%s : irq is %d.\n", __func__, irq);

	if (hi->is_ready)
		schedule_work(&hi->sendkey_work);

	return IRQ_HANDLED;
}

static void sec_jack_send_key_work_func(struct work_struct *work)
{
	struct sec_jack_info *hi =
		container_of(work, struct sec_jack_info, sendkey_work);
	struct sec_jack_platform_data *pdata = hi->pdata;
	int time_left_ms = SEND_KEY_CHECK_TIME_MS;
	int send_key_state = 0;

	wake_lock_timeout(&hi->det_wake_lock, WAKE_LOCK_TIME_IN_SENDKEY);
	/* debounce send/end key */
	while (time_left_ms > 0 && !hi->send_key_pressed) {
		send_key_state = pdata->get_send_key_state();
		if (!send_key_state || !pdata->get_det_jack_state() ||
#if defined(CONFIG_SAMSUNG_JACK_GNDLDET)
				 !pdata->get_gnd_jack_state() ||
#endif
				hi->cur_jack_type != SEC_HEADSET_4POLE) {
			/* button released or jack removed or more
			 * strangely a non-4pole headset
			 */
			pr_info(MODULE_NAME "%s : ignored button (%d %d %d)\n",
					__func__, !send_key_state,
					!pdata->get_det_jack_state(),
					hi->cur_jack_type !=
						 SEC_HEADSET_4POLE);
			return;
		}
		usleep_range(10000, 10000);
		time_left_ms -= 10;
	}

	/* report state change of the send_end_key */
	if (hi->send_key_pressed != send_key_state) {
		set_send_key_state(hi, send_key_state);
		pr_info(MODULE_NAME "%s : BTN is %s.\n",
				__func__,
				send_key_state ? "pressed" : "released");
	}

	return;
}

static irqreturn_t sec_jack_det_irq_handler(int irq, void *handle)
{
	struct sec_jack_info *hi = (struct sec_jack_info *)handle;

	if (likely(hi->is_ready))
		schedule_work(&hi->det_work);
	else
		pr_err(MODULE_NAME "%s : ready = %d.\n",
			__func__, hi->is_ready);

	return IRQ_HANDLED;
}

static void sec_jack_det_work_func(struct work_struct *work)
{
	struct sec_jack_info *hi =
		container_of(work, struct sec_jack_info, det_work);

	struct sec_jack_platform_data *pdata = hi->pdata;
	int time_left_ms = DET_CHECK_TIME_MS;
	int jack_state = 0;

	pr_debug(MODULE_NAME "%s\n", __func__);

	/* threaded irq can sleep */
	wake_lock_timeout(&hi->det_wake_lock, WAKE_LOCK_TIME);

	/* debounce headset jack.  don't try to determine the type of
	 * headset until the detect state is true for a while.
	 */
	while (time_left_ms > 0) {
		jack_state = pdata->get_det_jack_state();
		if (!jack_state) {
			pr_err("%s : err det jack state %d\n",
				__func__, jack_state);
			/* jack not detected. */
			handle_jack_not_inserted(hi);
			return;
		}
		usleep_range(10000, 10000);
		time_left_ms -= 10;
	}

	/* set mic bias to enable adc */
	pdata->set_micbias_state(true);

	/* to reduce noise in earjack when attaching */
	/* msleep(200); */

	/* jack presence was detected the whole time, figure out which type */
	determine_jack_type(hi);

	return;
}

static void sec_jack_powerup_work_func(struct work_struct *work)
{
	struct sec_jack_info *hi =
		container_of(work, struct sec_jack_info, powerup_work.work);

	hi->is_ready = true;
	sec_jack_det_irq_handler(hi->pdata->det_int, hi);

	return;
}

static int sec_jack_probe(struct platform_device *pdev)
{
	struct sec_jack_info *hi;
	struct sec_jack_platform_data *pdata = pdev->dev.platform_data;
	int ret;
	int sec_jack_keycode[] = {KEY_MEDIA, KEY_VOLUMEUP, KEY_VOLUMEDOWN};
	int i;
	struct class *audio;
	struct device *earjack;

	pr_info(MODULE_NAME "%s : Registering jack driver\n", __func__);
	if (!pdata) {
		pr_err("%s : pdata is NULL.\n", __func__);
		return -ENODEV;
	}
	if (!pdata->get_det_jack_state	||
			!pdata->get_send_key_state || !pdata->zones ||
			!pdata->set_micbias_state ||
#if defined(CONFIG_SAMSUNG_JACK_GNDLDET)
			!pdata->get_gnd_jack_state ||
#endif
#ifdef CONFIG_SAMSUNG_JACK_ADC_SCALE3
			!pdata->get_ear_adc_value ||
#endif
			!pdata->get_adc_value ||
			pdata->num_zones > MAX_ZONE_LIMIT) {
		pr_err("%s : need to check pdata\n", __func__);
		return -ENODEV;
	}

	hi = kzalloc(sizeof(struct sec_jack_info), GFP_KERNEL);
	if (hi == NULL) {
		pr_err("%s : Failed to allocate memory.\n", __func__);
		return -ENOMEM;
	}

	hi->is_ready = false;
	hi->pdata = pdata;
	hi->input = input_allocate_device();
	if (hi->input == NULL) {
		ret = -ENOMEM;
		pr_err("%s : Failed to allocate input device.\n", __func__);
		goto err_request_input_dev;
	}

	hi->input->name = "sec_jack";

	for (i = 0 ; i < 3; i++)
		input_set_capability(hi->input, EV_KEY, sec_jack_keycode[i]);
	ret = input_register_device(hi->input);

	if (ret) {
		pr_err("%s : Failed to register driver\n", __func__);
		goto err_register_input_dev;
	}

	ret = switch_dev_register(&switch_jack_detection);
	if (ret < 0) {
		pr_err("%s : Failed to register switch device\n", __func__);
		goto err_switch_dev_register;
	}

	ret = switch_dev_register(&switch_sendend);
	if (ret < 0) {
		pr_err("%s : Failed to register switch device\n", __func__);
		goto err_switch_dev_register;
	}

	wake_lock_init(&hi->det_wake_lock, WAKE_LOCK_SUSPEND, "sec_jack_det");

#ifdef CONFIG_SYSFS_SEC_SND_JACK
	audio = class_create(THIS_MODULE, "audio");
	if (IS_ERR(audio))
		pr_err("Failed to create class(audio)!\n");

	earjack = device_create(audio, NULL, 0, NULL, "earjack");
	if (IS_ERR(earjack))
		pr_err("Failed to create device(earjack)!\n");

	ret = device_create_file(earjack, &dev_attr_key_state);
	if (ret)
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
				dev_attr_key_state.attr.name);

	ret = device_create_file(earjack, &dev_attr_state);
	if (ret)
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
				dev_attr_state.attr.name);
#ifdef CONFIG_SYSFS_SEC_SND_SELECT_JACK
	ret = device_create_file(earjack, &dev_attr_select_jack);
	if (ret)
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
				dev_attr_select_jack.attr.name);

	ret = device_create_file(earjack, &dev_attr_reselect_jack);
	if (ret)
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
				dev_attr_reselect_jack.attr.name);
#endif
#endif

#if defined (CONFIG_MACH_MELIUS_CHN_CTC)
	ret = device_create_file(earjack, &dev_attr_set_hw_rev);
	if (ret)
		pr_err("Failed to create device file in sysfs entries(%s)!\n",
				dev_attr_set_hw_rev.attr.name);
#endif

	INIT_WORK(&hi->det_work, sec_jack_det_work_func);

	ret = request_threaded_irq(pdata->det_int, NULL,
			sec_jack_det_irq_handler,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |
			IRQF_ONESHOT, "sec_headset_detect", hi);

	if (ret) {
		pr_err("%s : Failed to request_irq.\n", __func__);
		goto err_request_detect_irq;
	}

	/* to handle insert/removal when we're sleeping in a call */
	ret = enable_irq_wake(pdata->det_int);
	if (ret) {
		pr_err("%s : Failed to enable_irq_wake.\n", __func__);
		goto err_enable_irq_wake;
	}

	INIT_WORK(&hi->sendkey_work, sec_jack_send_key_work_func);

	ret = request_threaded_irq(pdata->send_int, NULL,
			sec_jack_send_key_irq_handler,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |
			IRQF_ONESHOT,
			"sec_headset_send_key", hi);
	if (ret) {
		pr_err("%s : Failed to request_irq.\n", __func__);

		goto err_request_send_key_irq;
	}

	/* start with send/end interrupt disable. we only enable it
	 * when we detect a 4 pole headset
	 */
	disable_irq(pdata->send_int);
	dev_set_drvdata(earjack, hi);

	/* call irq_thread forcely because of missing interrupt when booting.
	 * 2000ms delay is enough to waiting for adc driver registration.
	 */
	INIT_DELAYED_WORK(&hi->powerup_work, sec_jack_powerup_work_func);
	schedule_delayed_work(&hi->powerup_work, msecs_to_jiffies(2000));

	return 0;

err_request_send_key_irq:
	disable_irq_wake(pdata->det_int);
err_enable_irq_wake:
	free_irq(pdata->det_int, hi);
err_request_detect_irq:
	wake_lock_destroy(&hi->det_wake_lock);
	switch_dev_unregister(&switch_jack_detection);
	switch_dev_unregister(&switch_sendend);
err_switch_dev_register:
	input_unregister_device(hi->input);
	goto err_request_input_dev;
err_register_input_dev:
	input_free_device(hi->input);
err_request_input_dev:
	kfree(hi);

	return ret;
}

static int sec_jack_remove(struct platform_device *pdev)
{

	struct sec_jack_info *hi = dev_get_drvdata(&pdev->dev);

	pr_info(MODULE_NAME "%s :\n", __func__);
	/* rebalance before free */
	if (hi->send_key_irq_enabled)
		disable_irq_wake(hi->pdata->send_int);
	else
		enable_irq(hi->pdata->send_int);
	free_irq(hi->pdata->send_int, hi);
	disable_irq_wake(hi->pdata->det_int);
	free_irq(hi->pdata->det_int, hi);
	wake_lock_destroy(&hi->det_wake_lock);
	switch_dev_unregister(&switch_jack_detection);
	switch_dev_unregister(&switch_sendend);
	input_unregister_device(hi->input);
	kfree(hi);

	return 0;
}

static struct platform_driver sec_jack_driver = {
	.probe = sec_jack_probe,
	.remove = sec_jack_remove,
	.driver = {
		.name = "sec_jack",
		.owner = THIS_MODULE,
	},
};
static int __init sec_jack_init(void)
{
	return platform_driver_register(&sec_jack_driver);
}

static void __exit sec_jack_exit(void)
{
	platform_driver_unregister(&sec_jack_driver);
}

module_init(sec_jack_init);
module_exit(sec_jack_exit);

MODULE_AUTHOR("ms17.kim@samsung.com");
MODULE_DESCRIPTION("Samsung Electronics Corp Ear-Jack detection driver");
MODULE_LICENSE("GPL");
