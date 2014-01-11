/*
 *  adc_fuelgauge.c
 *  Samsung ADC Fuel Gauge Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define DEBUG

#include <linux/battery/sec_fuelgauge.h>
#include <linux/time.h>
#if 0
static int adc_write_reg(struct i2c_client *client, int reg, u8 value)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, value);

	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static int adc_read_reg(struct i2c_client *client, int reg)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static int adc_read_word(struct i2c_client *client, int reg)
{
	int ret;

	ret = i2c_smbus_read_word_data(client, reg);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}
#endif

static int adc_get_adc_data(struct sec_fuelgauge_info *fuelgauge,
		int adc_ch, int count)
{
	int adc_data;
	int adc_max;
	int adc_min;
	int adc_total;
	int i;

	adc_data = 0;
	adc_max = 0;
	adc_min = 0;
	adc_total = 0;

	for (i = 0; i < count; i++) {
		mutex_lock(&fuelgauge->info.adclock);
		adc_data = adc_read(fuelgauge->pdata, adc_ch);
		mutex_unlock(&fuelgauge->info.adclock);

		if (adc_data < 0)
			goto err;

		if (i != 0) {
			if (adc_data > adc_max)
				adc_max = adc_data;
			else if (adc_data < adc_min)
				adc_min = adc_data;
		} else {
			adc_max = adc_data;
			adc_min = adc_data;
		}
		adc_total += adc_data;
	}

	return (adc_total - adc_max - adc_min) / (count - 2);
err:
	return adc_data;
}

static int adc_get_adc_value(
		struct sec_fuelgauge_info *fuelgauge, int channel)
{
	int adc;

	adc = adc_get_adc_data(fuelgauge, channel,
		get_battery_data(fuelgauge).adc_check_count);

	if (adc < 0) {
		dev_err(&fuelgauge->client->dev,
			"%s: Error in ADC\n", __func__);
		return adc;
	}

	return adc;
}

static unsigned long adc_calculate_average(
		struct sec_fuelgauge_info *fuelgauge,
		int channel, int adc)
{
	unsigned int cnt = 0;
	int total_adc = 0;
	int average_adc = 0;
	int index = 0;

	cnt = fuelgauge->info.adc_sample[channel].cnt;
	total_adc = fuelgauge->info.adc_sample[channel].total_adc;

	if (adc < 0) {
		dev_err(&fuelgauge->client->dev,
			"%s : Invalid ADC : %d\n", __func__, adc);
		adc = fuelgauge->info.adc_sample[channel].average_adc;
	}

	if (cnt < ADC_HISTORY_SIZE) {
		fuelgauge->info.adc_sample[channel].adc_arr[cnt] =
			adc;
		fuelgauge->info.adc_sample[channel].index = cnt;
		fuelgauge->info.adc_sample[channel].cnt = ++cnt;

		total_adc += adc;
		average_adc = total_adc / cnt;
	} else {
		index = fuelgauge->info.adc_sample[channel].index;
		if (++index >= ADC_HISTORY_SIZE)
			index = 0;

		total_adc = total_adc -
			fuelgauge->info.adc_sample[channel].adc_arr[index] +
			adc;
		average_adc = total_adc / ADC_HISTORY_SIZE;

		fuelgauge->info.adc_sample[channel].adc_arr[index] = adc;
		fuelgauge->info.adc_sample[channel].index = index;
	}

	fuelgauge->info.adc_sample[channel].total_adc = total_adc;
	fuelgauge->info.adc_sample[channel].average_adc =
		average_adc;

	return average_adc;
}

static int adc_get_data_by_adc(
		struct sec_fuelgauge_info *fuelgauge,
		const sec_bat_adc_table_data_t *adc_table,
		unsigned int adc_table_size, int adc)
{
	int data = 0;
	int low = 0;
	int high = 0;
	int mid = 0;

	if (adc_table[0].adc >= adc) {
		data = adc_table[0].data;
		goto data_by_adc_goto;
	} else if (adc_table[adc_table_size-1].adc <= adc) {
		data = adc_table[adc_table_size-1].data;
		goto data_by_adc_goto;
	}

	high = adc_table_size - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (adc_table[mid].adc > adc)
			high = mid - 1;
		else if (adc_table[mid].adc < adc)
			low = mid + 1;
		else {
			data = adc_table[mid].data;
			goto data_by_adc_goto;
		}
	}

	data = adc_table[low].data;
	data += ((adc_table[high].data - adc_table[low].data) *
		(adc - adc_table[low].adc)) /
		(adc_table[high].adc - adc_table[low].adc);

data_by_adc_goto:
	dev_dbg(&fuelgauge->client->dev,
		"%s: adc(%d), data(%d), high(%d), low(%d)\n",
		__func__, adc, data, high, low);

	return data;
}

#if 0
static int adc_get_adc_by_data(
		struct sec_fuelgauge_info *fuelgauge,
		const sec_bat_adc_table_data_t *adc_table,
		unsigned int adc_table_size, int data)
{
	int adc = 0;
	int low = 0;
	int high = 0;
	int mid = 0;

	if (adc_table[0].data >= data) {
		adc = adc_table[0].adc;
		goto adc_by_data_goto;
	} else if (adc_table[adc_table_size-1].data <= data) {
		adc = adc_table[adc_table_size-1].adc;
		goto adc_by_data_goto;
	}

	high = adc_table_size - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (adc_table[mid].data > data)
			high = mid - 1;
		else if (adc_table[mid].data < data)
			low = mid + 1;
		else {
			adc = adc_table[mid].adc;
			goto adc_by_data_goto;
		}
	}

	adc = adc_table[low].adc;
	adc += ((adc_table[high].adc - adc_table[low].adc) *
		(data - adc_table[low].data)) /
		(adc_table[high].data - adc_table[low].data);

adc_by_data_goto:
	dev_dbg(&fuelgauge->client->dev,
		"%s: data(%d), adc(%d), high(%d), low(%d)\n",
		__func__, data, adc, high, low);

	return adc;
}
#endif

static int get_cable_compensation_voltage(
	struct sec_fuelgauge_info *fuelgauge, int vcell)
{
	int comp_value;

	comp_value = 0;
	comp_value = adc_get_data_by_adc(fuelgauge,
		get_battery_data(fuelgauge).cable_comp_voltage,
		get_battery_data(fuelgauge).cable_comp_voltage_size,
		vcell);

	/* no need to reset SOC with 0mA charging current */
	if (!fuelgauge->info.current_now)
		fuelgauge->info.reset_percentage = 0;

	/* rescale by charging current */
	comp_value = comp_value * fuelgauge->info.current_now / 1000;

	dev_dbg(&fuelgauge->client->dev,
		"%s: cable comp value (%dmV, current %dmA)\n", __func__,
		comp_value, fuelgauge->info.current_now);

	return comp_value;
}

static int get_event_compensation_voltage(
	struct sec_fuelgauge_info *fuelgauge, int event)
{
	int i, comp_value;

	comp_value = 0;
	for (i = 0; i < BATT_EVENT_NUM; i++) {
		if (event & (0x1 << i)) {
			comp_value += get_battery_data(fuelgauge).
				event_comp_voltage[i];
			dev_dbg(&fuelgauge->client->dev,
				"%s: event number (%d), comp value (%d)\n",
				__func__, i, get_battery_data(fuelgauge).
				event_comp_voltage[i]);
		}
	}

	dev_dbg(&fuelgauge->client->dev,
		"%s: event comp value (%dmV)\n", __func__, comp_value);
	return comp_value;
}

#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
static int get_delta_compensation_voltage(
	struct sec_fuelgauge_info *fuelgauge, int vcell, int vcell_raw)
{
	int last_time, delta_time, delta_voltage_now, delta_current_now;
	int delta_voltage_now_in_sec, delta_compensation_voltage;

	delta_compensation_voltage = 0;
	last_time = fuelgauge->info.last_vcell_check_time.tv_sec;
	do_gettimeofday(&(fuelgauge->info.last_vcell_check_time));
	if (last_time) {
		delta_time = fuelgauge->info.last_vcell_check_time.tv_sec -
			last_time;

		if (delta_time > get_battery_data(fuelgauge).delta_reset_time) {
			dev_dbg(&fuelgauge->client->dev,
				"%s: reset delta compensation\n", __func__);
			delta_voltage_now = 0;
			delta_current_now = 0;
			fuelgauge->info.current_compensation = 0;
			goto no_delta_compensation;
		}

		/* to get compensation voltage,
		 * use raw vcell without compensation
		 */
		delta_compensation_voltage = adc_get_data_by_adc(fuelgauge,
			get_battery_data(fuelgauge).cable_comp_voltage,
			get_battery_data(fuelgauge).cable_comp_voltage_size,
			vcell_raw) * fuelgauge->info.current_compensation /
			1000;

		delta_voltage_now = vcell + delta_compensation_voltage -
			fuelgauge->info.voltage_now;

		fuelgauge->info.delta_voltage_now_in_sec =
			delta_voltage_now * 1000 / delta_time;

		if (delta_voltage_now < 0)
			delta_voltage_now_in_sec =
				-(fuelgauge->info.delta_voltage_now_in_sec);
		else
			delta_voltage_now_in_sec =
				fuelgauge->info.delta_voltage_now_in_sec;

		if ((delta_time <
			get_battery_data(fuelgauge).delta_check_time) &&
			(delta_voltage_now_in_sec >
			get_battery_data(fuelgauge).delta_comp_limit)) {
			/* to get delta current,
			 * use raw vcell without compensation
			 */
			delta_current_now = -(delta_voltage_now * 1000 /
				adc_get_data_by_adc(fuelgauge,
				get_battery_data(fuelgauge).cable_comp_voltage,
				get_battery_data(fuelgauge).cable_comp_voltage_size,
				vcell));

			/* to show compensation value, added minus */
			dev_dbg(&fuelgauge->client->dev,
				"%s: delta compensation (%dmA)\n",
				__func__, -delta_current_now);
			fuelgauge->info.current_compensation +=
				delta_current_now;

			/* to get compensation voltage,
			 * use raw vcell without compensation
			 */
			delta_compensation_voltage =
				adc_get_data_by_adc(fuelgauge,
				get_battery_data(fuelgauge).cable_comp_voltage,
				get_battery_data(fuelgauge).cable_comp_voltage_size,
				vcell_raw) *
				fuelgauge->info.current_compensation / 1000;
		} else
			delta_current_now =
				fuelgauge->info.current_compensation;

		if (!delta_compensation_voltage) {
			dev_dbg(&fuelgauge->client->dev,
				"%s: no need delta compensation "
				"vcell raw %dmV, comp vol %dmV, dv %dmV\n",
				__func__, vcell_raw, delta_compensation_voltage,
				delta_voltage_now);
			fuelgauge->info.current_compensation = 0;
		}

no_delta_compensation:
		dev_dbg(&fuelgauge->client->dev,
			"%s: dtime %dsec, dvoltage %dmV, dcurrent %dmA, "
			"dvoltage/sec %duV, delta comp voltage %dmV, "
			"compensation current %dmA\n",
			__func__, delta_time,
			delta_voltage_now, delta_current_now,
			fuelgauge->info.delta_voltage_now_in_sec,
			delta_compensation_voltage,
			fuelgauge->info.current_compensation);
	}

	return delta_compensation_voltage;
}
#endif

static int adc_get_vcell(struct i2c_client *client)
{
	union power_supply_propval cable;
	union power_supply_propval event;
	union power_supply_propval is_charging;
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int vcell, vcell_raw;

	vcell = adc_get_data_by_adc(fuelgauge,
		get_battery_data(fuelgauge).adc2vcell_table,
		get_battery_data(fuelgauge).adc2vcell_table_size,
		adc_get_adc_value(fuelgauge, SEC_BAT_ADC_CHANNEL_VOLTAGE_NOW));
	vcell_raw = vcell;

	psy_do_property("battery", get,
		POWER_SUPPLY_PROP_ONLINE, cable);
	/* get current event status */
	event.intval = BATT_EVENT;
	psy_do_property("battery", get,
		POWER_SUPPLY_PROP_TECHNOLOGY, event);
	psy_do_property("battery", get,
		POWER_SUPPLY_PROP_CHARGE_NOW, is_charging);

	if (is_charging.intval != SEC_BATTERY_CHARGING_NONE) {
		/* compensate voltage by cable only in charging status */
		if (cable.intval != POWER_SUPPLY_TYPE_BATTERY)
			vcell += get_cable_compensation_voltage(
				fuelgauge, vcell);
	} else {
		/* need compensation before cable detection
		 * in power-off charging
		 */
		if ((cable.intval == POWER_SUPPLY_TYPE_BATTERY) &&
			(fuelgauge->pdata->is_lpm() ||
			fuelgauge->pdata->check_vbus_status())) {
			dev_dbg(&client->dev, "%s: VBUS compensation\n",
				__func__);
			vcell += get_cable_compensation_voltage(
				fuelgauge, vcell);
		}
	}

	if (event.intval) {
		if (fuelgauge->pdata->check_vbus_status() &&
			(event.intval & EVENT_BOOTING))
			dev_dbg(&client->dev, "%s: no event compensation "
				"in booting with charging\n", __func__);
		else
			vcell += get_event_compensation_voltage(
				fuelgauge, event.intval);
	}

#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
	if (!fuelgauge->pdata->monitor_initial_count)
		vcell += get_delta_compensation_voltage(
			fuelgauge, vcell, vcell_raw);
#endif

	return vcell;
}

static int adc_get_avg_vcell(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int voltage_avg;

	voltage_avg = (int)adc_calculate_average(fuelgauge,
		ADC_CHANNEL_VOLTAGE_AVG, fuelgauge->info.voltage_now);

#if defined(DEBUG)
	{
		int i;
		printk("%s : ", __func__);
		for(i=0;i<ADC_HISTORY_SIZE;i++)
			printk("%d, ", fuelgauge->info.
				adc_sample[ADC_CHANNEL_VOLTAGE_AVG].adc_arr[i]);
		printk("\n");
	}
#endif

	return voltage_avg;
}

static int adc_get_ocv(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int voltage_ocv;

	voltage_ocv = (int)adc_calculate_average(fuelgauge,
		ADC_CHANNEL_VOLTAGE_OCV, fuelgauge->info.voltage_avg);

#if defined(DEBUG)
	{
		int i;
		printk("%s : ", __func__);
		for(i=0;i<ADC_HISTORY_SIZE;i++)
			printk("%d, ", fuelgauge->info.
				adc_sample[ADC_CHANNEL_VOLTAGE_OCV].adc_arr[i]);
		printk("\n");
	}
#endif

	return voltage_ocv;
}

/* soc should be 0.01% unit */
static int adc_get_soc(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int soc;

	soc = adc_get_data_by_adc(fuelgauge,
		get_battery_data(fuelgauge).ocv2soc_table,
		get_battery_data(fuelgauge).ocv2soc_table_size,
		fuelgauge->info.voltage_ocv);

	return soc;
}

static int adc_get_current(struct i2c_client *client)
{
	union power_supply_propval value;
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int current_now;

	current_now = adc_get_data_by_adc(fuelgauge,
		get_battery_data(fuelgauge).adc2current_table,
		get_battery_data(fuelgauge).adc2current_table_size,
		adc_get_adc_value(fuelgauge, SEC_BAT_ADC_CHANNEL_CURRENT_NOW));

	if (!current_now) {
		psy_do_property("sec-charger", get,
			POWER_SUPPLY_PROP_CURRENT_NOW, value);
		current_now = value.intval;
	}

	return current_now;
}

/* judge power off or not by current_avg */
static int adc_get_current_average(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	union power_supply_propval value_bat;
	union power_supply_propval value_chg;
	int vcell, soc, curr_avg;

	psy_do_property("sec-charger", get,
		POWER_SUPPLY_PROP_CURRENT_NOW, value_chg);
	psy_do_property("battery", get,
		POWER_SUPPLY_PROP_HEALTH, value_bat);
	vcell = fuelgauge->info.voltage_now;
	soc = fuelgauge->info.capacity / 100;

	/* if 0% && under 3.4v && low power charging(1000mA), power off */
	if (!fuelgauge->pdata->is_lpm() && (soc <= 0) && (vcell < 3400) &&
			((value_chg.intval < 1000) ||
			((value_bat.intval == POWER_SUPPLY_HEALTH_OVERHEAT) ||
			(value_bat.intval == POWER_SUPPLY_HEALTH_COLD)))) {
		dev_info(&client->dev, "%s: SOC(%d), Vnow(%d), Inow(%d)\n",
			__func__, soc, vcell, value_chg.intval);
		curr_avg = -1;
	} else {
		curr_avg = value_chg.intval;
	}

	return curr_avg;
}

static void adc_reset(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int i;

	for (i = 0; i < ADC_CHANNEL_NUM; i++) {
		fuelgauge->info.adc_sample[i].cnt = 0;
		fuelgauge->info.adc_sample[i].total_adc = 0;
	}

#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
	fuelgauge->info.current_compensation = 0;
#endif

	cancel_delayed_work(&fuelgauge->info.monitor_work);
	schedule_delayed_work(&fuelgauge->info.monitor_work, 0);
}

static void adc_reset_voltage_avg(
	struct i2c_client *client, int voltage_now, int percentage)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int i;

	for (i = 0; i < (ADC_HISTORY_SIZE * percentage / 100); i++)
		adc_calculate_average(fuelgauge,
			ADC_CHANNEL_VOLTAGE_AVG, voltage_now);
#if defined(DEBUG)
	{
		int k;
		printk("%s : ", __func__);
		for(k=0;k<ADC_HISTORY_SIZE;k++)
			printk("%d, ", fuelgauge->info.
				adc_sample[ADC_CHANNEL_VOLTAGE_AVG].adc_arr[k]);
		printk("\n");
	}
#endif

	dev_dbg(&client->dev, "%s: reset average voltage as %dmV (%d%%)\n",
		__func__, voltage_now, percentage);
}

static void adc_reset_voltage_ocv(struct i2c_client *client, int voltage_avg)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	int i;

	for (i = 0; i < ADC_HISTORY_SIZE; i++)
		adc_calculate_average(fuelgauge,
			ADC_CHANNEL_VOLTAGE_OCV, voltage_avg);
#if defined(DEBUG)
	{
		int k;
		printk("%s : ", __func__);
		for(k=0;k<ADC_HISTORY_SIZE;k++)
			printk("%d, ", fuelgauge->info.
				adc_sample[ADC_CHANNEL_VOLTAGE_OCV].adc_arr[k]);
		printk("\n");
	}
#endif

	dev_dbg(&client->dev, "%s: reset open circuit voltage as %dmV\n",
		__func__, voltage_avg);
}

static void adc_get_reset_percentage(struct sec_fuelgauge_info *fuelgauge)
{
	int last_time, delta_time;

	if (fuelgauge->info.is_init)
		fuelgauge->info.reset_percentage = 100;
	else {
		last_time = fuelgauge->info.last_vcell_check_time.tv_sec;
		do_gettimeofday(&(fuelgauge->info.last_vcell_check_time));
		if (last_time) {
			delta_time = fuelgauge->info.
				last_vcell_check_time.tv_sec - last_time;
			dev_dbg(&fuelgauge->client->dev,
				"%s: delta time %dsec\n",
				__func__, delta_time);
			if (delta_time >= (fuelgauge->pdata->polling_time[
				SEC_BATTERY_POLLING_TIME_SLEEP] * 90 / 100))
				fuelgauge->info.reset_percentage = 67;
			else if (delta_time >= (fuelgauge->pdata->polling_time[
				SEC_BATTERY_POLLING_TIME_CHARGING] * 90 / 100))
				fuelgauge->info.reset_percentage = 17;
			else
				fuelgauge->info.reset_percentage = 0;
		} else
			fuelgauge->info.reset_percentage = 0;
	}
}

static void adc_monitor_work(struct work_struct *work)
{
	struct sec_fuelgauge_info *fuelgauge =
		container_of(work, struct sec_fuelgauge_info,
		info.monitor_work.work);

	fuelgauge->info.current_now = adc_get_current(fuelgauge->client);
	fuelgauge->info.voltage_now = adc_get_vcell(fuelgauge->client);
	if (fuelgauge->info.reset_percentage)
		adc_reset_voltage_avg(fuelgauge->client,
			fuelgauge->info.voltage_now,
			fuelgauge->info.reset_percentage);
	fuelgauge->info.voltage_avg = adc_get_avg_vcell(fuelgauge->client);
	if (fuelgauge->info.reset_percentage) {
		adc_reset_voltage_ocv(fuelgauge->client,
			fuelgauge->info.voltage_avg);
		fuelgauge->info.reset_percentage = 0;
	}
	fuelgauge->info.voltage_ocv = adc_get_ocv(fuelgauge->client);
	fuelgauge->info.current_avg =
		adc_get_current_average(fuelgauge->client);
	fuelgauge->info.capacity = adc_get_soc(fuelgauge->client);

	dev_info(&fuelgauge->client->dev,
		"%s:Vnow(%dmV),Vavg(%dmV),Vocv(%dmV),"
		"Inow(%dmA),Iavg(%dmA),SOC(%d%%)\n", __func__,
		fuelgauge->info.voltage_now, fuelgauge->info.voltage_avg,
		fuelgauge->info.voltage_ocv, fuelgauge->info.current_now,
		fuelgauge->info.current_avg, fuelgauge->info.capacity);

	if (fuelgauge->pdata->monitor_initial_count)
		schedule_delayed_work(&fuelgauge->info.monitor_work, HZ);
	else
		schedule_delayed_work(&fuelgauge->info.monitor_work,
			HZ * get_battery_data(fuelgauge).monitor_polling_time);

	if (fuelgauge->info.is_init) {
		fuelgauge->info.is_init--;
		adc_get_reset_percentage(fuelgauge);
	}

	/* save time of monitor */
	do_gettimeofday(&(fuelgauge->info.last_vcell_check_time));
}

bool sec_hal_fg_init(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
			i2c_get_clientdata(client);

	fuelgauge->info.last_vcell_check_time.tv_sec = 0;
	fuelgauge->info.is_init = 2;
	adc_get_reset_percentage(fuelgauge);

	mutex_init(&fuelgauge->info.adclock);
	INIT_DELAYED_WORK_DEFERRABLE(&fuelgauge->info.monitor_work,
		adc_monitor_work);

	schedule_delayed_work(&fuelgauge->info.monitor_work, HZ);
	return true;
}

bool sec_hal_fg_suspend(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_resume(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge =
			i2c_get_clientdata(client);

	adc_get_reset_percentage(fuelgauge);
	schedule_delayed_work(&fuelgauge->info.monitor_work, 0);
	return true;
}

bool sec_hal_fg_fuelalert_init(struct i2c_client *client, int soc)
{
	return true;
}

bool sec_hal_fg_is_fuelalerted(struct i2c_client *client)
{
	return false;
}

bool sec_hal_fg_fuelalert_process(void *irq_data, bool is_fuel_alerted)
{
	return true;
}

bool sec_hal_fg_full_charged(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_reset(struct i2c_client *client)
{
	adc_reset(client);
	return true;
}

bool sec_hal_fg_get_property(struct i2c_client *client,
			     enum power_supply_property psp,
			     union power_supply_propval *val)
{
	struct sec_fuelgauge_info *fuelgauge =
			i2c_get_clientdata(client);

	switch (psp) {
		/* Cell voltage (VCELL, mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = fuelgauge->info.voltage_now;
		break;
		/* Additional Voltage Information (mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
		switch (val->intval) {
		case SEC_BATTEY_VOLTAGE_AVERAGE:
			val->intval = fuelgauge->info.voltage_avg;
			break;
		case SEC_BATTEY_VOLTAGE_OCV:
			val->intval = fuelgauge->info.voltage_ocv;
			break;
		}
		break;
		/* Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = fuelgauge->info.current_now;
		break;
		/* Average Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		val->intval = fuelgauge->info.current_avg;
		break;
		/* SOC (%) */
	case POWER_SUPPLY_PROP_CAPACITY:
		if (val->intval == SEC_FUELGAUGE_CAPACITY_TYPE_RAW)
			val->intval = fuelgauge->info.capacity;
		else
			val->intval = fuelgauge->info.capacity / 10;
		break;
		/* Battery Temperature */
	case POWER_SUPPLY_PROP_TEMP:
		/* Target Temperature */
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		break;

	default:
		return false;
	}
	return true;
}

bool sec_hal_fg_set_property(struct i2c_client *client,
			     enum power_supply_property psp,
			     const union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		break;
		/* Battery Temperature */
	case POWER_SUPPLY_PROP_TEMP:
		break;
		/* Target Temperature */
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		break;
	default:
		return false;
	}
	return true;
}

ssize_t sec_hal_fg_show_attrs(struct device *dev,
				const ptrdiff_t offset, char *buf)
{
	int i = 0;

	switch (offset) {
	case FG_DATA:
	case FG_REGS:
		break;
	default:
		i = -EINVAL;
		break;
	}

	return i;
}

ssize_t sec_hal_fg_store_attrs(struct device *dev,
				const ptrdiff_t offset,
				const char *buf, size_t count)
{
	int ret = 0;

	switch (offset) {
	case FG_REG:
	case FG_DATA:
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}
