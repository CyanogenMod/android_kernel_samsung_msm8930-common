/*
 * Copyright (C) 2012 Samsung Electronics, Inc.
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
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/gpio.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/msm8930-gpio.h>

#include "devices-msm8x60.h"
#include "board-8930.h"

#if defined(CONFIG_BATTERY_SAMSUNG)
#include <linux/battery/sec_battery.h>
#include <linux/battery/sec_fuelgauge.h>
#include <linux/battery/sec_charger.h>

#define SEC_BATTERY_PMIC_NAME ""

sec_battery_platform_data_t sec_battery_pdata;
static unsigned int sec_bat_recovery_mode;

static sec_charging_current_t charging_current_table[] = {
	{1800,	2100,	200,	40*60},	/* Unknown */
	{0,	0,	0,	0},	/* Battery */
	{0,	0,	0,	0},	/* UPS */
	{1800,	2100,	200,	40*60},	/* MAINS */
	{460,	460,	200,	40*60},	/* USB */
	{460,	460,	200,	40*60},	/* USB_DCP */
	{1000,	1000,	200,	40*60},	/* USB_CDP */
	{460,	460,	200,	40*60},	/* USB_ACA */
	{1700,	2100,	200,	40*60},	/* MISC */
	{0,	0,	0,	0},	/* Cardock */
	{500,	500,	200,	40*60},	/* Wireless */
	{1800,	2100,	200,	40*60},	/* UartOff */
	{0,	0,	0,	0},	/* OTG */
	{0,	0,	0,	0},	/* BMS */
};

static bool sec_bat_adc_none_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_none_exit(void) {return true; }
static int sec_bat_adc_none_read(unsigned int channel) {return 0; }

static bool sec_bat_adc_ap_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ap_exit(void) {return true; }

static struct battery_data_t biscotto_battery_data[];
static int sec_bat_adc_ap_read(unsigned int channel)
{
	int rc = -1, data = -1;
	struct pm8xxx_adc_chan_result result;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_TEMP:
	case SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT:
		rc = pm8xxx_adc_mpp_config_read(
			PM8XXX_AMUX_MPP_3, ADC_MPP_1_AMUX6, &result);
		if (rc) {
			pr_err("error reading mpp %d, rc = %d\n",
				PM8XXX_AMUX_MPP_3, rc);
			return rc;
		}

		/* use measurement, no need to scale */
		data = (int)result.measurement;
		break;

	case SEC_BAT_ADC_CHANNEL_VOLTAGE_NOW:
		pm8xxx_adc_read(CHANNEL_VBAT, &result); 

		data = (int)result.physical;
		break;

	case SEC_BAT_ADC_CHANNEL_CURRENT_NOW:
		rc = pm8xxx_adc_mpp_config_read(
			PM8XXX_AMUX_MPP_7, ADC_MPP_1_AMUX6, &result);
		if (rc) {
			pr_err("error reading mpp %d, rc = %d\n",
				PM8XXX_AMUX_MPP_7, rc);
			return rc;
		}

		/* use measurement, no need to scale */
		data = (int)result.measurement;

		/* MPP7 error in discharging */
		if (data > biscotto_battery_data[0].adc2current_table[
			biscotto_battery_data[0].adc2current_table_size-1].adc) {
			pr_err("Invalid adc value: %d, %d\n", data,
				biscotto_battery_data[0].adc2current_table_size);
			data = 0;
		}
		break;

	default:
		pr_err("Invalid adc channel: %d\n", channel);
		break;
	}

	return data;
}

static bool sec_bat_adc_ic_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ic_exit(void) {return true; }
static int sec_bat_adc_ic_read(unsigned int channel) {return 0; }

static void sec_bat_isr_callback(void)
{
}

static bool sec_bat_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_BATT_INT, 0,
		GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	return true;
}

static struct i2c_gpio_platform_data gpio_i2c_data_fgchg = {
	.sda_pin = GPIO_FUELGAUGE_I2C_SDA,
	.scl_pin = GPIO_FUELGAUGE_I2C_SCL,
};

static bool sec_fg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(gpio_i2c_data_fgchg.scl_pin, 0,
			GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(gpio_i2c_data_fgchg.sda_pin,  0,
			GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(gpio_i2c_data_fgchg.scl_pin, 1);
	gpio_set_value(gpio_i2c_data_fgchg.sda_pin, 1);

	return true;
}

static bool sec_chg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_CHG_INT, 0,
		GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_CHG_DET_N, 0,
		GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);

	return true;
}

static bool sec_bat_is_lpm(void) {return (bool)poweroff_charging; }

int current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
int extended_cable_type;

static void sec_bat_initial_check(void)
{
	union power_supply_propval value;

	if (POWER_SUPPLY_TYPE_BATTERY != current_cable_type) {
		value.intval = current_cable_type;
		psy_do_property("battery", set,
			POWER_SUPPLY_PROP_ONLINE, value);
	}
}
#if defined(CONFIG_USB_SWITCH_TSU6721)
extern void tsu6721_monitor(void);
#endif

static void sec_bat_monitor_additional_check(void)
{
	gpio_set_value(GPIO_5V_ENABLE, 1);
	pr_info("%s: power pack enabled\n", __func__);

#if defined(CONFIG_USB_SWITCH_TSU6721)
	tsu6721_monitor();
#endif
}

static bool sec_bat_check_jig_status(void)
{
	return (current_cable_type == POWER_SUPPLY_TYPE_UARTOFF);
}
static bool sec_bat_check_vbus_status(void)
{
	pr_debug("VBUS status (%d) caller: %pF\n",
		gpio_get_value(GPIO_OTG_TEST), __builtin_return_address(0));

	return (bool)gpio_get_value(GPIO_OTG_TEST);
}
static bool sec_bat_check_external_charging_status(void)
{
	pr_debug("%s: external charging (%d)\n", __func__,
		gpio_get_value(GPIO_5V_ENABLE));

	return (bool)gpio_get_value(GPIO_5V_ENABLE);
}

static bool sec_bat_switch_to_check(void) {return true; }
static bool sec_bat_switch_to_normal(void) {return true; }

static int sec_bat_check_cable_callback(void)
{
	pr_debug("%s: cable type 0x%x\n", __func__, current_cable_type);
	return current_cable_type;
}

static int sec_bat_get_cable_from_extended_cable_type(
	int input_extended_cable_type)
{
	int cable_main, cable_sub, cable_power;
	int cable_type = POWER_SUPPLY_TYPE_UNKNOWN;

	cable_main = GET_MAIN_CABLE_TYPE(input_extended_cable_type);
	if (cable_main != POWER_SUPPLY_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_MAIN_MASK) |
			(cable_main << ONLINE_TYPE_MAIN_SHIFT);
	cable_sub = GET_SUB_CABLE_TYPE(input_extended_cable_type);
	if (cable_sub != ONLINE_SUB_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_SUB_MASK) |
			(cable_sub << ONLINE_TYPE_SUB_SHIFT);
	cable_power = GET_POWER_CABLE_TYPE(input_extended_cable_type);
	if (cable_power != ONLINE_POWER_TYPE_UNKNOWN)
		extended_cable_type = (extended_cable_type &
			~(int)ONLINE_TYPE_PWR_MASK) |
			(cable_power << ONLINE_TYPE_PWR_SHIFT);

	switch (cable_main) {
	case POWER_SUPPLY_TYPE_CARDOCK:
		switch (cable_power) {
		case ONLINE_POWER_TYPE_BATTERY:
			cable_type = POWER_SUPPLY_TYPE_BATTERY;
			break;
		case ONLINE_POWER_TYPE_TA:
			switch (cable_sub) {
			case ONLINE_SUB_TYPE_MHL:
				cable_type = POWER_SUPPLY_TYPE_USB;
				break;
			case ONLINE_SUB_TYPE_AUDIO:
			case ONLINE_SUB_TYPE_DESK:
			case ONLINE_SUB_TYPE_SMART_NOTG:
			case ONLINE_SUB_TYPE_KBD:
				cable_type = POWER_SUPPLY_TYPE_MAINS;
				break;
			case ONLINE_SUB_TYPE_SMART_OTG:
				cable_type = POWER_SUPPLY_TYPE_CARDOCK;
				break;
			}
			break;
		case ONLINE_POWER_TYPE_USB:
			cable_type = POWER_SUPPLY_TYPE_USB;
			break;
		default:
			cable_type = POWER_SUPPLY_TYPE_BATTERY; //current_cable_type;
			break;
		}
		break;
	case POWER_SUPPLY_TYPE_MISC:
		switch (cable_sub) {
		case ONLINE_SUB_TYPE_MHL:
			switch (cable_power) {
			case ONLINE_POWER_TYPE_BATTERY:
				cable_type = POWER_SUPPLY_TYPE_BATTERY;
				break;
			case ONLINE_POWER_TYPE_TA:
				cable_type = POWER_SUPPLY_TYPE_MISC;
				break;
			case ONLINE_POWER_TYPE_USB:
				cable_type = POWER_SUPPLY_TYPE_USB;
				break;
			default:
				cable_type = cable_main;
			}
			break;
		default:
			cable_type = cable_main;
			break;
		}
		break;
	default:
		cable_type = cable_main;
		break;
	}

	current_cable_type = cable_type;

	return cable_type;
}

static bool sec_bat_check_cable_result_callback(
				int cable_type)
{
//	struct regulator *l29;
	current_cable_type = cable_type;

/*
	if(system_rev >= 0x8)
	{
		if (current_cable_type == POWER_SUPPLY_TYPE_BATTERY)
		{
			pr_info("%s set ldo off\n", __func__);
			l29 = regulator_get(NULL, "8921_l29");
			if(l29 > 0)
			{
				regulator_disable(l29);
			}
		}
		else
		{
			pr_info("%s set ldo on\n", __func__);
			l29 = regulator_get(NULL, "8921_l29");
			if(l29 > 0)
			{
				regulator_enable(l29);
			}
		}
	}
*/
	return true;
}

/* callback for battery check
 * return : bool
 * true - battery detected, false battery NOT detected
 */
static bool sec_bat_check_callback(void)
{
	return !gpio_get_value(GPIO_BATT_INT);
}

static bool sec_bat_check_result_callback(void) {return true; }

/* callback for OVP/UVLO check
 * return : int
 * battery health
 */
static int sec_bat_ovp_uvlo_callback(void)
{
	int health;
	health = POWER_SUPPLY_HEALTH_GOOD;

	return health;
}

static bool sec_bat_ovp_uvlo_result_callback(int health) {return true; }

/*
 * val.intval : temperature
 */
static bool sec_bat_get_temperature_callback(
		enum power_supply_property psp,
		union power_supply_propval *val) {return true; }
static bool sec_fg_fuelalert_process(bool is_fuel_alerted) {return true; }

static const sec_bat_adc_table_data_t temp_table[] = {
	{217100,	700},
	{257600,	650},
	{281500,	620},
	{305600,	600},
	{317600,	580},
	{356100,	550},
	{400800,	500},
	{455400,	470},
	{480300,	450},
	{520600,	430},
	{571400,	400},
	{663400,	350},
	{757900,	300},
	{878300,	250},
	{988700,	200},
	{1099300,	150},
	{1198900,	100},
	{1291100,	50},
	{1373500,	0},
	{1424800,	-30},
	{1457900,	-50},
	{1486000,	-70},
	{1557800,	-100},
	{1613400,	-150},
	{1662100,	-200},
	{1700600,	-250},
	{1727700,	-300},
};

/* ADC region should be exclusive */
static sec_bat_adc_region_t cable_adc_value_table[] = {
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
	{0,	0},
};

static int polling_time_table[] = {
	10,	/* BASIC */
	30,	/* CHARGING */
	30,	/* DISCHARGING */
	30,	/* NOT_CHARGING */
	300,	/* SLEEP */
};

/* for ADC fuelgauge */
/* soc should be 0.01% unit */
static const sec_bat_adc_table_data_t adc_ocv2soc_table[] = {
	{3400,	0},
	{3650,	500},
	{3700,	1000},
	{3770,	2500},
	{3820,	4500},
	{3960,	6500},
	{4350,	10000},
};

static const sec_bat_adc_table_data_t adc_adc2vcell_table[] = {
	{2500000,	2500},
	{3400000,	3400},
	{4350000,	4350},
	{5000000,	5000},
};

static const sec_bat_adc_table_data_t adc_adc2current_table[] = {
	{120000,	0},
	{130000,	135},
	{140000,	145},
	{167000,	180},
	{185000,	200},
	{200000,	220},
	{230000,	250},
	{245000,	280},
	{260000,	300},
	{278000,	320},
	{292000,	340},
	{300000,	355},
	{315000,	370},
	{325000,	385},
	{335000,	400},
	{350000,	420},
	{360000,	435},
	{375000,	450},
	{390000,	460},
	{400000,	480},
	{410000,	500},
	{430000,	520},
	{450000,	550},
	{470000,	580},
	{480000,	600},
	{500000,	650},
	{560000,	780},
	{590000,	820},
	{600000,	850},
	{630000,	900},
	{710000,	1000},
	{800000,	1100},
	{820000,	1200},
	{910000,	1300},
	{1000000,	1400},
	{1200000,	1700},
	{1380000,	2000},
};

static const int event_compensation_voltage_table[] = {
	0,	/* BATT_EVENT_CALL */
	0,	/* BATT_EVENT_2G_CALL */
	0,	/* BATT_EVENT_TALK_GSM */
	0,	/* BATT_EVENT_3G_CALL */
	0,	/* BATT_EVENT_TALK_WCDMA */
	0,	/* BATT_EVENT_MUSIC */
	0,	/* BATT_EVENT_VIDEO */
	0,	/* BATT_EVENT_BROWSER */
	0,	/* BATT_EVENT_HOTSPOT */
	0,	/* BATT_EVENT_CAMERA */
	0,	/* BATT_EVENT_CAMCORDER */
	0,	/* BATT_EVENT_DATA_CALL */
	20,	/* BATT_EVENT_WIFI */
	0,	/* BATT_EVENT_WIBRO */
	0,	/* BATT_EVENT_LTE */
	30,	/* BATT_EVENT_LCD */
	0,	/* BATT_EVENT_GPS */
	50,	/* BATT_EVENT_BOOTING */
};

/* this compensation table is for 1A charging current */
static const sec_bat_adc_table_data_t cable_comp_voltage_table[] = {
	{3850,	-211},
	{4030,	-194},
	{4100,	-183},
	{4220,	-177},
	{4300,	-166},
	{4310,	0},
};

static struct battery_data_t biscotto_battery_data[] = {
	/* SDI battery data (High voltage 4.35V) */
	{
		.adc_check_count = 10,
		.monitor_polling_time = 5,
		.ocv2soc_table = adc_ocv2soc_table,
		.ocv2soc_table_size =
			sizeof(adc_ocv2soc_table) /
			sizeof(sec_bat_adc_table_data_t),
		.adc2vcell_table = adc_adc2vcell_table,
		.adc2vcell_table_size =
			sizeof(adc_adc2vcell_table) /
			sizeof(sec_bat_adc_table_data_t),
		.adc2current_table = adc_adc2current_table,
		.adc2current_table_size =
			sizeof(adc_adc2current_table) /
			sizeof(sec_bat_adc_table_data_t),
		.event_comp_voltage = event_compensation_voltage_table,
		.event_comp_voltage_size =
			sizeof(event_compensation_voltage_table) /
			sizeof(sec_bat_adc_table_data_t),
		.cable_comp_voltage = cable_comp_voltage_table,
		.cable_comp_voltage_size =
			sizeof(cable_comp_voltage_table) /
			sizeof(sec_bat_adc_table_data_t),
#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
		.delta_comp_limit = 15000,
		.delta_check_time = 15,
		.delta_reset_time = 60,
#endif
		.type_str = "SDI",
	}
};

sec_battery_platform_data_t sec_battery_pdata = {
	/* NO NEED TO BE CHANGED */
	.initial_check = sec_bat_initial_check,
	.monitor_additional_check = sec_bat_monitor_additional_check,
	.bat_gpio_init = sec_bat_gpio_init,
	.fg_gpio_init = sec_fg_gpio_init,
	.chg_gpio_init = sec_chg_gpio_init,
	.bat_isr_callback = sec_bat_isr_callback,

	.is_lpm = sec_bat_is_lpm,
	.check_jig_status = sec_bat_check_jig_status,
	.check_vbus_status = sec_bat_check_vbus_status,
	.check_external_charging_status =
		sec_bat_check_external_charging_status,
	.check_cable_callback =
		sec_bat_check_cable_callback,
	.get_cable_from_extended_cable_type =
		sec_bat_get_cable_from_extended_cable_type,
	.cable_switch_check = sec_bat_switch_to_check,
	.cable_switch_normal = sec_bat_switch_to_normal,
	.check_cable_result_callback =
		sec_bat_check_cable_result_callback,
	.check_battery_callback =
		sec_bat_check_callback,
	.check_battery_result_callback =
		sec_bat_check_result_callback,
	.ovp_uvlo_callback = sec_bat_ovp_uvlo_callback,
	.ovp_uvlo_result_callback =
		sec_bat_ovp_uvlo_result_callback,
	.fuelalert_process = sec_fg_fuelalert_process,
	.get_temperature_callback =
		sec_bat_get_temperature_callback,
	.adc_api[SEC_BATTERY_ADC_TYPE_NONE] = {
		.init = sec_bat_adc_none_init,
		.exit = sec_bat_adc_none_exit,
		.read = sec_bat_adc_none_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_AP] = {
		.init = sec_bat_adc_ap_init,
		.exit = sec_bat_adc_ap_exit,
		.read = sec_bat_adc_ap_read
		},
	.adc_api[SEC_BATTERY_ADC_TYPE_IC] = {
		.init = sec_bat_adc_ic_init,
		.exit = sec_bat_adc_ic_exit,
		.read = sec_bat_adc_ic_read
		},
	.cable_adc_value = cable_adc_value_table,
	.charging_current = charging_current_table,
	.polling_time = polling_time_table,
	/* NO NEED TO BE CHANGED */

	.pmic_name = SEC_BATTERY_PMIC_NAME,

	.adc_check_count = 6,
	.adc_type = {
		SEC_BATTERY_ADC_TYPE_NONE,	/* CABLE_CHECK */
		SEC_BATTERY_ADC_TYPE_AP,	/* BAT_CHECK */
		SEC_BATTERY_ADC_TYPE_AP,	/* TEMP */
		SEC_BATTERY_ADC_TYPE_AP,	/* TEMP_AMB */
		SEC_BATTERY_ADC_TYPE_AP,	/* FULL_CHECK */
		SEC_BATTERY_ADC_TYPE_AP,	/* VOLTAGE_NOW */
		SEC_BATTERY_ADC_TYPE_AP,	/* CURRENT_NOW */
	},

	/* Battery */
	.vendor = "SDI SDI",
	.technology = POWER_SUPPLY_TECHNOLOGY_LION,
	.battery_data = (void *)biscotto_battery_data,
	.bat_gpio_ta_nconnected = 0,
	.bat_polarity_ta_nconnected = 0,
	.bat_irq = 0,
	.bat_irq_attr = 0,
	.cable_check_type =
		SEC_BATTERY_CABLE_CHECK_PSY |
		SEC_BATTERY_CABLE_CHECK_NOINCOMPATIBLECHARGE,
	.cable_source_type =
		SEC_BATTERY_CABLE_SOURCE_EXTERNAL |
		SEC_BATTERY_CABLE_SOURCE_EXTENDED,

	.event_check = true,
	.event_waiting_time = 0,

	/* Monitor setting */
	.polling_type = SEC_BATTERY_MONITOR_ALARM,
	.monitor_initial_count = 3,

	/* Battery check */
	.battery_check_type = SEC_BATTERY_CHECK_CALLBACK,
	.check_count = 0,
	/* Battery check by ADC */
	.check_adc_max = 1440,
	.check_adc_min = 0,

	/* OVP/UVLO check */
	.ovp_uvlo_check_type = SEC_BATTERY_OVP_UVLO_CHGPOLLING,

	/* Temperature check */
	.thermal_source = SEC_BATTERY_THERMAL_SOURCE_ADC,
	.temp_adc_table = temp_table,
	.temp_adc_table_size =
		sizeof(temp_table)/sizeof(sec_bat_adc_table_data_t),
	.temp_amb_adc_table = temp_table,
	.temp_amb_adc_table_size =
		sizeof(temp_table)/sizeof(sec_bat_adc_table_data_t),

	.temp_check_type = SEC_BATTERY_TEMP_CHECK_TEMP,
	.temp_check_count = 1,

	.temp_high_threshold_event = 480,
	.temp_high_recovery_event = 430,
	.temp_low_threshold_event = -60,
	.temp_low_recovery_event = 0,

	.temp_high_threshold_normal = 480,
	.temp_high_recovery_normal = 430,
	.temp_low_threshold_normal = -60,
	.temp_low_recovery_normal = 0,

	.temp_high_threshold_lpm = 460,
	.temp_high_recovery_lpm = 430,
	.temp_low_threshold_lpm = -40,
	.temp_low_recovery_lpm = 5,

	.full_check_type = SEC_BATTERY_FULLCHARGED_CHGINT,
	.full_check_type_2nd = SEC_BATTERY_FULLCHARGED_TIME,
	.full_check_count = 1,
	.chg_gpio_full_check = 0,
	.chg_polarity_full_check = 1,
	.full_condition_type = SEC_BATTERY_FULL_CONDITION_SOC |
		SEC_BATTERY_FULL_CONDITION_NOTIMEFULL |
		SEC_BATTERY_FULL_CONDITION_VCELL,
	.full_condition_soc = 90,
	.full_condition_vcell = 4250,

	.recharge_check_count = 2,
	.recharge_condition_type =
		SEC_BATTERY_RECHARGE_CONDITION_VCELL,
	.recharge_condition_vcell = 4300,

	.charging_total_time = 6 * 60 * 60,
	.recharging_total_time = 90 * 60,
	.charging_reset_time = 0,

	/* Fuel Gauge */
	.fg_irq = 0,
   	.fg_irq_attr = 0,
	.fuel_alert_soc = 4,
	.repeated_fuelalert = false,
	.capacity_calculation_type =
		SEC_FUELGAUGE_CAPACITY_TYPE_RAW |
		SEC_FUELGAUGE_CAPACITY_TYPE_SCALE |
		SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE,
	.capacity_max = 1000,
	.capacity_max_margin = 50,
	.capacity_min = 0,

	/* Charger */
	.charger_name = "sec-charger",
	.chg_gpio_en = 0,
	.chg_polarity_en = 0,
	.chg_gpio_status = 0,
	.chg_polarity_status = 0,
	.chg_irq = MSM_GPIO_TO_INT(GPIO_CHG_INT),
	.chg_irq_attr = IRQF_TRIGGER_FALLING,
	.chg_float_voltage = 4340,
};

static struct platform_device sec_device_battery = {
	.name = "sec-battery",
	.id = -1,
	.dev.platform_data = &sec_battery_pdata,
};

struct platform_device sec_device_fgchg = {
	.name = "i2c-gpio",
	.id = MSM_FUELGAUGE_I2C_BUS_ID,
	.dev.platform_data = &gpio_i2c_data_fgchg,
};

static struct i2c_board_info sec_brdinfo_fgchg[] __initdata = {
#if !defined(CONFIG_MFD_MAX77693)
	{
		I2C_BOARD_INFO("sec-charger",
			SEC_CHARGER_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
#endif
	{
		I2C_BOARD_INFO("sec-fuelgauge",
			SEC_FUELGAUGE_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
};

static struct platform_device *msm8960_battery_devices[] __initdata = {
	&sec_device_fgchg,
	&sec_device_battery,
};

static int __init sec_bat_current_boot_mode(char *mode)
{
	/*
	*	1 is recovery booting
	*	0 is normal booting
	*/

	if (strncmp(mode, "1", 1) == 0)
		sec_bat_recovery_mode = 1;
	else
		sec_bat_recovery_mode = 0;

	pr_info("%s : %s", __func__, sec_bat_recovery_mode == 1 ?
				"recovery" : "normal");

	return 1;
}
__setup("androidboot.boot_recovery=", sec_bat_current_boot_mode);

void __init msm8960_init_battery(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SCL, 0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SDA,  0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SCL, 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SDA, 1);

	gpio_i2c_data_fgchg.sda_pin = GPIO_FUELGAUGE_I2C_SDA;
	gpio_i2c_data_fgchg.scl_pin = GPIO_FUELGAUGE_I2C_SCL;

	platform_add_devices(
		msm8960_battery_devices,
		ARRAY_SIZE(msm8960_battery_devices));

	i2c_register_board_info(
		MSM_FUELGAUGE_I2C_BUS_ID,
		sec_brdinfo_fgchg,
		ARRAY_SIZE(sec_brdinfo_fgchg));
}

#endif

