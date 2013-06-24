/*
 *  max17047_fuelgauge.c
 *  Samsung MAX17047 Fuel Gauge Driver
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

#include <linux/rtc.h>
#include <linux/battery/sec_fuelgauge.h>

static int max17047_i2c_read(struct i2c_client *client, int reg, u8 *buf)
{
	int ret;

	ret = i2c_smbus_read_i2c_block_data(client, reg, 2, buf);
	if (ret < 0)
		pr_err("%s: err %d, reg: 0x%02x\n", __func__, ret, reg);

	return ret;
}



static int max17047_i2c_write(struct i2c_client *client, int reg, u8 *buf)
{
	int ret;

	ret = i2c_smbus_write_i2c_block_data(client, reg, 2, buf);
	if (ret < 0)
		pr_err("%s: err %d, reg: 0x%02x, data: 0x%x%x\n", __func__,
				ret, reg, buf[0], buf[1]);

	return ret;
}

static int max17047_read_word(struct i2c_client *client, int reg)
{
	int ret;

	ret = i2c_smbus_read_word_data(client, reg);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static void max17047_test_read(struct i2c_client *client)
{
	int reg;
	u8 data[2];
	int i;
	u8 buf[673];

	struct timespec ts;
	struct rtc_time tm;
	pr_info("%s\n", __func__);

	getnstimeofday(&ts);
	rtc_time_to_tm(ts.tv_sec, &tm);

	pr_info("%s: %d/%d/%d %02d:%02d\n", __func__,
					tm.tm_mday,
					tm.tm_mon + 1,
					tm.tm_year + 1900,
					tm.tm_hour,
					tm.tm_min);

	i = 0;
	for (reg = 0; reg < 0x50; reg++) {
		if (!(reg & 0xf))
			i += sprintf(buf + i, "\n%02x| ", reg);
		max17047_i2c_read(client, reg, data);
		i += sprintf(buf + i, "%02x%02x ", data[1], data[0]);
	}
	for (reg = 0xe0; reg < 0x100; reg++) {
		if (!(reg & 0xf))
			i += sprintf(buf + i, "\n%02x| ", reg);
		max17047_i2c_read(client, reg, data);
		i += sprintf(buf + i, "%02x%02x ", data[1], data[0]);
	}

	pr_info("    0    1    2    3    4    5    6    7");
	pr_cont("    8    9    a    b    c    d    e    f");
	pr_cont("%s\n", buf);
}

static void fg_read_regs(struct i2c_client *client, char *str)
{
	int data = 0;
	u32 addr = 0;

	for (addr = 0x0; addr <= 0x50; addr++) {
		data = max17047_read_word(client, addr);
		sprintf(str + strlen(str), "0x%04x, ", data);
	}

	/* "#" considered as new line in application */
	sprintf(str+strlen(str), "#");

	for (addr = 0xe0; addr <= 0x100; addr++) {
		data = max17047_read_word(client, addr);
		sprintf(str + strlen(str), "0x%04x, ", data);
	}
}

static u16 max17047_get_rcomp(struct i2c_client *client)
{
	u16 w_data;
	int temp;

	temp = max17047_read_word(client, MAX17047_REG_RCOMP);

	w_data = swab16(temp);

	dev_dbg(&client->dev,
		"%s : current rcomp = 0x%04x\n",
		__func__, w_data);

	return w_data;
}

#if 0
static void max17047_set_rcomp(struct i2c_client *client, u16 new_rcomp)
{
	i2c_smbus_write_word_data(client,
		MAX17047_REG_RCOMP, swab16(new_rcomp));
}

static void max17047_rcomp_update(struct i2c_client *client, int temp)
{
	struct sec_fuelgauge_info *fuelgauge =
				i2c_get_clientdata(client);
	union power_supply_propval value;

	int starting_rcomp = 0;
	int new_rcomp = 0;
	int rcomp_current = 0;

	rcomp_current = max17047_get_rcomp(client);

	psy_do_property("battery", get,
		POWER_SUPPLY_PROP_STATUS, value);

	if (value.intval == POWER_SUPPLY_STATUS_CHARGING) /* in charging */
		starting_rcomp = get_battery_data(fuelgauge).RCOMP_charging;
	else
		starting_rcomp = get_battery_data(fuelgauge).RCOMP0;

	if (temp > RCOMP0_TEMP)
		new_rcomp = starting_rcomp + ((temp - RCOMP0_TEMP) *
			get_battery_data(fuelgauge).temp_cohot / 1000);
	else if (temp < RCOMP0_TEMP)
		new_rcomp = starting_rcomp + ((temp - RCOMP0_TEMP) *
			get_battery_data(fuelgauge).temp_cocold / 1000);
	else
		new_rcomp = starting_rcomp;

	if (new_rcomp > 255)
		new_rcomp = 255;
	else if (new_rcomp < 0)
		new_rcomp = 0;

	new_rcomp <<= 8;
	new_rcomp &= 0xff00;
	/* not related to RCOMP */
	new_rcomp |= (rcomp_current & 0xff);

	if (rcomp_current != new_rcomp) {
		dev_dbg(&client->dev,
			"%s : RCOMP 0x%04x -> 0x%04x (0x%02x)\n",
			__func__, rcomp_current, new_rcomp,
			new_rcomp >> 8);
		max17047_set_rcomp(client, new_rcomp);
	}
}
#endif

static void max17047_get_version(struct i2c_client *client)
{
	u16 w_data;
	int temp;

	temp = max17047_read_word(client, MAX17047_REG_VERSION);

	w_data = swab16(temp);

	dev_info(&client->dev,
		"MAX17047 Fuel-Gauge Ver 0x%04x\n", w_data);
}

static void max17047_reg_init(struct i2c_client *client)
{
	u8 i2c_data[2];
	pr_debug("%s\n", __func__);

	i2c_data[1] = 0x00;
	i2c_data[0] = 0x00;
	max17047_i2c_write(client, MAX17047_REG_CGAIN, i2c_data);

	i2c_data[1] = 0x00;
	i2c_data[0] = 0x03;
	max17047_i2c_write(client, MAX17047_REG_MISCCFG, i2c_data);

	i2c_data[1] = 0x07;
	i2c_data[0] = 0x00;
	max17047_i2c_write(client, MAX17047_REG_LEARNCFG, i2c_data);

}

/* SOC% alert, disabled(0xFF00) */
static void max17047_set_salrt(struct i2c_client *client, u8 min, u8 max)
{
	u8 i2c_data[2];
	pr_info("%s: min(%d%%), max(%d%%)\n", __func__, min, max);

	i2c_data[1] = max;
	i2c_data[0] = min;
	max17047_i2c_write(client, MAX17047_REG_SALRT_TH, i2c_data);

	max17047_i2c_read(client, MAX17047_REG_SALRT_TH, i2c_data);
	if ((i2c_data[0] != min) || (i2c_data[1] != max))
		pr_err("%s: SALRT_TH is not valid (0x%02d%02d ? 0x%02d%02d)\n",
			__func__, i2c_data[1], i2c_data[0], max, min);
}

/* Temperature alert, disabled(0x7F80) */
static void max17047_set_talrt(struct i2c_client *client, u8 min, u8 max)
{
	u8 i2c_data[2];
	pr_info("%s: min(0x%02x), max(0x%02x)\n", __func__, min, max);

	i2c_data[1] = max;
	i2c_data[0] = min;
	max17047_i2c_write(client, MAX17047_REG_TALRT_TH, i2c_data);

	max17047_i2c_read(client, MAX17047_REG_TALRT_TH, i2c_data);
	if ((i2c_data[0] != min) || (i2c_data[1] != max))
		pr_err("%s: TALRT_TH is not valid (0x%02d%02d ? 0x%02d%02d)\n",
			__func__, i2c_data[1], i2c_data[0], max, min);
}

/* Voltage alert, disabled(0xFF00) */
static void max17047_set_valrt(struct i2c_client *client, u8 min, u8 max)
{
	u8 i2c_data[2];
	pr_info("%s: min(%dmV), max(%dmV)\n", __func__, min * 20, max * 20);

	i2c_data[1] = max;
	i2c_data[0] = min;
	max17047_i2c_write(client, MAX17047_REG_VALRT_TH, i2c_data);

	max17047_i2c_read(client, MAX17047_REG_VALRT_TH, i2c_data);
	if ((i2c_data[0] != min) || (i2c_data[1] != max))
		pr_err("%s: VALRT_TH is not valid (0x%02d%02d ? 0x%02d%02d)\n",
			__func__, i2c_data[1], i2c_data[0], max, min);
}

static void max17047_alert_init(struct i2c_client *client)
{
	u8 i2c_data[2];
	pr_debug("%s\n", __func__);

	/* SALRT Threshold setting */
	/* min 1%, max disable */
	max17047_set_salrt(client, 0x01, 0xFF);

	/* TALRT Threshold setting */
	/* min disable, max disable */
	max17047_set_talrt(client, 0x80, 0x7F);

	/* VALRT Threshold setting */
	/* min disable, max disable */
	max17047_set_valrt(client, 0x00, 0xFF);

	/* Enable SOC alerts */
	max17047_i2c_read(client, MAX17047_REG_CONFIG, i2c_data);
	i2c_data[0] |= (0x1 << 2);
	max17047_i2c_write(client, MAX17047_REG_CONFIG, i2c_data);
}

/* max17047_get_XXX(); Return current value and update data value */
static int max17047_get_vfocv(struct i2c_client *client)
{
	u8 data[2];
	int ret;
	u32 vfocv;
	u32 temp;
	pr_debug("%s\n", __func__);

	ret = max17047_i2c_read(client, MAX17047_REG_VFOCV, data);
	if (ret < 0)
		return ret;

	temp = ((data[0] >> 3) + (data[1] << 5)) * 625;
	vfocv = temp / 1000;

	pr_debug("%s: VFOCV(0x%02x%02x, %d)\n", __func__,
		 data[1], data[0], vfocv);
	
	return vfocv;
}

static int max17047_get_vcell(struct i2c_client *client)
{
	u8 data[2];
	int ret;
	u32 vcell;
	u32 temp;
	pr_debug("%s\n", __func__);

	ret = max17047_i2c_read(client, MAX17047_REG_VCELL, data);
	if (ret < 0)
		return ret;

	temp = ((data[0] >> 3) + (data[1] << 5)) * 625;
	vcell = temp / 1000;

	pr_debug("%s: VCELL(0x%02x%02x, %d)\n", __func__,
		 data[1], data[0], vcell);
	
	return vcell;
}

static int max17047_get_avgvcell(struct i2c_client *client)
{
	u8 data[2];
	int ret;
	u32 avgvcell;
	u32 temp;
	pr_debug("%s\n", __func__);

	ret = max17047_i2c_read(client, MAX17047_REG_AVGVCELL, data);
	if (ret < 0)
		return ret;

	temp = ((data[0] >> 3) + (data[1] << 5)) * 625;
	avgvcell = temp / 1000;

	pr_debug("%s: AVGVCELL(0x%02x%02x, %d)\n", __func__,
		 data[1], data[0], avgvcell);
	
	return avgvcell;
}

static int max17047_get_rawsoc(struct i2c_client *client)
{
	u8 data[2];
	int ret;
	int rawsoc;
	pr_debug("%s\n", __func__);

	ret = max17047_i2c_read(client, MAX17047_REG_SOC_VF, data);
	if (ret < 0)
		return ret;

	rawsoc = (data[1] * 100) + (data[0] * 100 / 256);

	pr_debug("%s: RAWSOC(0x%02x%02x, %d)\n", __func__,
		 data[1], data[0], rawsoc);
	return rawsoc;
}

static void max17047_reset_soc(struct i2c_client *client)
{
	u8 data[2];
	pr_info("%s: Before quick-start - "
		"VFOCV(%d), VFSOC(%d)\n", __func__,
				max17047_get_vfocv(client),
				max17047_get_rawsoc(client));
	max17047_test_read(client);

	if (max17047_i2c_read(client, MAX17047_REG_MISCCFG, data) < 0)
		return;

	/* Set bit10 makes quick start */
	data[1] |= (0x1 << 2);
	max17047_i2c_write(client, MAX17047_REG_MISCCFG, data);

	msleep(500);

	pr_info("%s: After quick-start - "
		"VFOCV(%d), VFSOC(%d)\n", __func__,
				max17047_get_vfocv(client),
				max17047_get_rawsoc(client));
	max17047_test_read(client);

	return;
}

#ifdef USE_TRIM_ERROR_DETECTION
/* Temp: Init max17047 sample has trim value error. For detecting that. */
#define TRIM_ERROR_DETECT_VOLTAGE1	2500000
#define TRIM_ERROR_DETECT_VOLTAGE2	3600000
static bool max17047_detect_trim_error(struct i2c_client *client)
{
	bool ret = false;
	int vcell, soc;

	vcell = max17047_get_vcell(client);
	soc = max17047_get_rawsoc(client);

	if (((vcell < TRIM_ERROR_DETECT_VOLTAGE1) ||
		(vcell == TRIM_ERROR_DETECT_VOLTAGE2)) && (soc == 0)) {
		pr_err("%s: (maybe)It's a trim error version. "
			"VCELL(%d), VFSOC(%d)\n", __func__, vcell, soc);
		ret = true;
	}

	return ret;
}
#endif

bool sec_hal_fg_init(struct i2c_client *client)
{
	struct sec_fuelgauge_info *fuelgauge = i2c_get_clientdata(client);
	
	max17047_get_version(client);
	max17047_reg_init(client);
	fuelgauge->info.rcomp_val = max17047_get_rcomp(client);

#ifdef USE_TRIM_ERROR_DETECTION
	/* trim error detect */
	fuelgauge->info.trim_err = max17047_detect_trim_error(client);
#endif

	pr_info("%s: rcomp = 0x%x, trim = %d\n", __func__,
			fuelgauge->info.rcomp_val, fuelgauge->info.trim_err);

	return true;
}

bool sec_hal_fg_suspend(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_resume(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_fuelalert_init(struct i2c_client *client, int soc)
{
	max17047_alert_init(client);

	return true;
}

bool sec_hal_fg_is_fuelalerted(struct i2c_client *client)
{
	/* only for s-alert */
	/* TBD */

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
	max17047_reset_soc(client);
	return true;
}

bool sec_hal_fg_get_property(struct i2c_client *client,
			     enum power_supply_property psp,
			     union power_supply_propval *val)
{
	switch (psp) {
		/* Cell voltage (VCELL, mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = max17047_get_vcell(client);
		break;
		/* Additional Voltage Information (mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
		switch (val->intval) {
		case SEC_BATTEY_VOLTAGE_AVERAGE:
			val->intval = max17047_get_avgvcell(client);
			break;
		case SEC_BATTEY_VOLTAGE_OCV:
			val->intval = max17047_get_vfocv(client);
			break;
		}
		break;
		/* Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		break;
		/* Average Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		break;
		/* SOC (%) */
	case POWER_SUPPLY_PROP_CAPACITY:
		if (val->intval == SEC_FUELGAUGE_CAPACITY_TYPE_RAW)
			val->intval = max17047_get_rawsoc(client);
		else
			val->intval = max17047_get_rawsoc(client) / 10;
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
		/* temperature is 0.1 degree, should be divide by 10 */
		//max17047_rcomp_update(client, val->intval / 10);
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
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_fuelgauge_info *fg =
		container_of(psy, struct sec_fuelgauge_info, psy_fg);
	int i = 0;
	char *str = NULL;

	switch (offset) {
	case FG_DATA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%02x%02x\n",
			fg->reg_data[1], fg->reg_data[0]);
		break;
	case FG_REGS:
		str = kzalloc(sizeof(char)*1024, GFP_KERNEL);
		if (!str)
			return -ENOMEM;

		fg_read_regs(fg->client, str);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%s\n",
			str);

		kfree(str);
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
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_fuelgauge_info *fg =
		container_of(psy, struct sec_fuelgauge_info, psy_fg);
	int ret = 0;
	int x = 0;
	u16 data;

	switch (offset) {
	case FG_REG:
		if (sscanf(buf, "%x\n", &x) == 1) {
			fg->reg_addr = x;
			data = max17047_read_word(
				fg->client, fg->reg_addr);
			fg->reg_data[0] = (data & 0xff00) >> 8;
			fg->reg_data[1] = (data & 0x00ff);

			dev_dbg(&fg->client->dev,
				"%s: (read) addr = 0x%x, data = 0x%02x%02x\n",
				 __func__, fg->reg_addr,
				 fg->reg_data[1], fg->reg_data[0]);
			ret = count;
		}
		break;
	case FG_DATA:
		if (sscanf(buf, "%x\n", &x) == 1) {
			dev_dbg(&fg->client->dev,
				"%s: (write) addr = 0x%x, data = 0x%04x\n",
				__func__, fg->reg_addr, x);
			i2c_smbus_write_word_data(fg->client,
				fg->reg_addr, swab16(x));
			ret = count;
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

