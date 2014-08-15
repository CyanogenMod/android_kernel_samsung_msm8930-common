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
#include <linux/mfd/pm8xxx/pm8921-sec-charger.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/msm8960-gpio.h>

#include "devices-msm8x60.h"
#include "board-8960.h"

#if defined(CONFIG_BATTERY_SAMSUNG)
#if defined (CONFIG_MACH_COMANCHE)
/*
#include <linux/battery/sec_battery_configuration.h>
*/
#include <linux/battery/sec_fuelgauge.h>
#include <linux/battery/sec_charger.h>
#else
#include <linux/battery/sec_battery.h>
#include <linux/battery/sec_fuelgauge.h>
#include <linux/battery/sec_charger.h>
#endif
#define SEC_BATTERY_PMIC_NAME ""

static unsigned int sec_bat_recovery_mode;

static int msm_otg_pmic_gpio_config(
		int gpio, int direction, int pullup, int value)
{
	struct pm_gpio param;
	int rc = 0;
	int out_strength = 0;

	if (direction == PM_GPIO_DIR_IN)
		out_strength = PM_GPIO_STRENGTH_NO;
	else
		out_strength = PM_GPIO_STRENGTH_HIGH;

	param.direction = direction;
	param.output_buffer = PM_GPIO_OUT_BUF_CMOS;
	param.output_value = value;
	param.pull = pullup;
	param.vin_sel = PM_GPIO_VIN_S4;
	param.out_strength = out_strength;
	param.function = PM_GPIO_FUNC_NORMAL;
	param.inv_int_pol = 0;
	param.disable_pin = 0;

	rc = pm8xxx_gpio_config(
		PM8921_GPIO_PM_TO_SYS(gpio), &param);
	if (rc < 0) {
		pr_err("failed to configure vbus_in gpio\n");
		return rc;
	}

	return rc;
}

static bool sec_bat_adc_none_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_none_exit(void) {return true; }
static int sec_bat_adc_none_read(unsigned int channel) {return 0; }

static bool sec_bat_adc_ap_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ap_exit(void) {return true; }
static int sec_bat_adc_ap_read(unsigned int channel)
{

#if defined(CONFIG_MACH_COMANCHE)
	int rc, data=-1;
#else
int rc, data;
#endif
	struct pm8xxx_adc_chan_result result;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_TEMP:
		rc = pm8xxx_adc_mpp_config_read(
			PM8XXX_AMUX_MPP_7, ADC_MPP_1_AMUX6, &result);
		if (rc) {
			pr_err("error reading mpp %d, rc = %d\n",
				PM8XXX_AMUX_MPP_7, rc);
			return rc;
		}

		/* use measurement, no need to scale */
		data = (int)result.measurement;
		break;
	case SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT:
		rc = pm8xxx_adc_mpp_config_read(
			PM8XXX_AMUX_MPP_10, ADC_MPP_1_AMUX6, &result);
		if (rc) {
			pr_err("error reading mpp %d, rc = %d\n",
				PM8XXX_AMUX_MPP_10, rc);
			return rc;
		}

		/* use measurement, no need to scale */
		data = (int)result.measurement;
		break;
	}

	return data;
}

static bool sec_bat_adc_ic_init(
		struct platform_device *pdev) {return true; }
static bool sec_bat_adc_ic_exit(void) {return true; }
static int sec_bat_adc_ic_read(unsigned int channel) {return 0; }

static bool sec_bat_gpio_init(void)
{
	msm_otg_pmic_gpio_config(PMIC_GPIO_BATT_INT,
		PM_GPIO_DIR_IN, PM_GPIO_PULL_NO, 1);

	msm_otg_pmic_gpio_config(PMIC_GPIO_CHG_STAT,
		PM_GPIO_DIR_IN, PM_GPIO_PULL_UP_30, 1);

	msm_otg_pmic_gpio_config(PMIC_GPIO_OTG_POWER,
		PM_GPIO_DIR_IN, PM_GPIO_PULL_NO, 1);

	return true;
}

static bool sec_fg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SCL, 0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SDA,  0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SCL, 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SDA, 1);

	gpio_tlmm_config(GPIO_CFG(GPIO_FUEL_INT,  0, GPIO_CFG_INPUT,
		GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);

	return true;
}

static bool sec_chg_gpio_init(void)
{
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SCL, 0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_tlmm_config(GPIO_CFG(GPIO_FUELGAUGE_I2C_SDA,  0, GPIO_CFG_OUTPUT,
		 GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SCL, 1);
	gpio_set_value(GPIO_FUELGAUGE_I2C_SDA, 1);

	if (system_rev >= BOARD_REV01)
		msm_otg_pmic_gpio_config(PMIC_GPIO_CHG_EN,
			PM_GPIO_DIR_OUT, PM_GPIO_PULL_NO, 1);

	return true;
}

static bool sec_bat_is_lpm(void) {return (bool)poweroff_charging; }

static void sec_bat_initial_check(void)
{
	union power_supply_propval value;

	switch (msm8960_get_cable_status()) {
	case CABLE_TYPE_AC:
		value.intval = POWER_SUPPLY_TYPE_MAINS;
		break;
	case CABLE_TYPE_MISC:
		value.intval = POWER_SUPPLY_TYPE_MISC;
		break;
	case CABLE_TYPE_USB:
		value.intval = POWER_SUPPLY_TYPE_USB;
		break;
	case CABLE_TYPE_CARDOCK:
		value.intval = POWER_SUPPLY_TYPE_CARDOCK;
		break;
	case CABLE_TYPE_NONE:
		value.intval = POWER_SUPPLY_TYPE_BATTERY;
		break;
	default:
		pr_err("%s: invalid cable :%d\n",
			__func__, msm8960_get_cable_status());
		return;
	}

	psy_do_property("battery", set,
		POWER_SUPPLY_PROP_ONLINE, value);
}

static bool sec_bat_check_jig_status(void) {return false; }
static bool sec_bat_switch_to_check(void) {return true; }
static bool sec_bat_switch_to_normal(void) {return true; }

static int current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
static int sec_bat_check_cable_callback(void)
{
	/* When TA(or USB) cable is inserted,
	 * bat_irq_thread is called, before fsa9485_charger_cb.
	 * Both can trigger the cable/monitor work.
	 * Should be changed the order of them bcz of below issue.
	 * e.g. If the phone is in high temperature and inserting TA,
	 * 1. bat_irq_thread > cable_work > monitor work > high temp stop
	 * 2.            fsa9485_charger_cb > cable_work > charging start
	 * At that time, as soon as stop charging bcz of high temp,
	 * charging start by cable_work of fsa9485.
	 * Add msleep to fix the this issue.
	 */
	msleep(500);
	if (current_cable_type ==
		POWER_SUPPLY_TYPE_BATTERY &&
		gpio_get_value_cansleep(
		PM8921_GPIO_PM_TO_SYS(
		PMIC_GPIO_OTG_POWER))) {
		pr_info("%s : VBUS IN\n", __func__);
		return POWER_SUPPLY_TYPE_UARTOFF;
	}

	if ((current_cable_type ==
		POWER_SUPPLY_TYPE_UARTOFF ||
		current_cable_type ==
		POWER_SUPPLY_TYPE_CARDOCK) &&
		!gpio_get_value_cansleep(
		PM8921_GPIO_PM_TO_SYS(
		PMIC_GPIO_OTG_POWER))) {
		pr_info("%s : VBUS OUT\n", __func__);
		return POWER_SUPPLY_TYPE_BATTERY;
	}

	return current_cable_type;
}

static bool sec_bat_check_cable_result_callback(
				int cable_type)
{
	current_cable_type = cable_type;

	switch (cable_type) {
	case POWER_SUPPLY_TYPE_USB:
		pr_info("%s set vbus applied\n",
			__func__);
		break;

	case POWER_SUPPLY_TYPE_BATTERY:
		pr_info("%s set vbus cut\n",
			__func__);
		msm_otg_set_charging_state(0);
		break;
	case POWER_SUPPLY_TYPE_MAINS:
		msm_otg_set_charging_state(1);
		break;
	default:
		pr_err("%s cable type (%d)\n",
			__func__, cable_type);
		return false;
	}
	return true;
}

/* callback for battery check
 * return : bool
 * true - battery detected, false battery NOT detected
 */
static bool sec_bat_check_callback(void)
{
	int i, present;

	present = 0;

	if (sec_bat_recovery_mode == 1 || system_state == SYSTEM_RESTART) {
		pr_info("%s : recovery/restart, skip batt check\n", __func__);
		present = 1;
		pm8921_enable_batt_therm(0);
		return present;
	}

	pm8921_enable_batt_therm(1);
	/* check battery 10 times */
	for (i = 0; i < 10; i++) {
		msleep(200);

		if (sec_bat_recovery_mode == 1
			|| system_state == SYSTEM_RESTART) {
			pr_info("%s : recovery/restart, skip batt check\n",
					__func__);
			present = 1;
			pm8921_enable_batt_therm(0);
			break;
		}

		present = !gpio_get_value_cansleep(
			PM8921_GPIO_PM_TO_SYS(
			PMIC_GPIO_BATT_INT));

		/* If the battery is missing, then check more */
		if (present) {
			i++;
			break;
		}
	}

	pm8921_enable_batt_therm(0);
	pr_info("%s : battery is %s (%d time%c)\n",
		__func__, present ? "present" : "absent",
		i, (i == 1) ? ' ' : 's');

	return present ? true : false;
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


static sec_bat_adc_table_data_t temp_table[] = {
	{27592,	650},
	{27999,	600},
	{28563,	550},
	{29211,	500},
	{29948,	450},
	{30769,	400},
	{36990,	100},
	{37991,	50},
	{38910,	0},
	{39658,	-50},
	{40443,	-100},
	{41034,	-150},
	{41523,	-200},
	{41825,	-250},
	{42158,	-300},
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

static sec_charging_current_t charging_current_table[] = {
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
	{1000,	1050,	150,	0},
	{1000,	500,	150,	0},
	{1000,	500,	150,	0},
	{1000,	500,	150,	0},
	{1000,	500,	150,	0},
	{1000,	700,	150,	0},
	{0,	0,	0,	0},
	{1000,	1050,	150,	0},
	{0,	0,	0,	0},
	{0,	0,	0,	0},
};

static int polling_time_table[] = {
	10,	/* BASIC */
	30,	/* CHARGING */
	30,	/* DISCHARGING */
	30,	/* NOT_CHARGING */
	300,	/* SLEEP */
};

/* for MAX17048 */
static struct battery_data_t comanche_battery_data[] = {
	/* SDI battery data (High voltage 4.35V) */
	{
		.RCOMP0 = 0x57,
		.RCOMP_charging = 0x67,
		.temp_cohot = -100,
		.temp_cocold = -4500,
		.is_using_model_data = true,
		.type_str = "SDI",
	}
};

static sec_battery_platform_data_t sec_battery_pdata = {
	/* NO NEED TO BE CHANGED */
	.initial_check = sec_bat_initial_check,
	.bat_gpio_init = sec_bat_gpio_init,
	.fg_gpio_init = sec_fg_gpio_init,
	.chg_gpio_init = sec_chg_gpio_init,

	.is_lpm = sec_bat_is_lpm,
	.check_jig_status = sec_bat_check_jig_status,
	.check_cable_callback =
		sec_bat_check_cable_callback,
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

	.adc_check_count = 7,
	.adc_type = {
		SEC_BATTERY_ADC_TYPE_NONE,	/* CABLE_CHECK */
		SEC_BATTERY_ADC_TYPE_NONE,	/* BAT_CHECK */
		SEC_BATTERY_ADC_TYPE_AP,	/* TEMP */
		SEC_BATTERY_ADC_TYPE_AP,	/* TEMP_AMB */
		SEC_BATTERY_ADC_TYPE_AP,	/* FULL_CHECK */
	},

	/* Battery */
	.vendor = "SDI SDI",
	.technology = POWER_SUPPLY_TECHNOLOGY_LION,
	.battery_data = (void *)comanche_battery_data,
	.bat_gpio_ta_nconnected = 0,
	.bat_polarity_ta_nconnected = 0,
	.bat_irq =
		PM8921_GPIO_IRQ(PM8921_IRQ_BASE, PMIC_GPIO_OTG_POWER),
	.bat_irq_attr =
		IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.cable_check_type =
		SEC_BATTERY_CABLE_CHECK_PSY |
		SEC_BATTERY_CABLE_CHECK_INT,
	.cable_source_type =
		SEC_BATTERY_CABLE_SOURCE_EXTERNAL |
		SEC_BATTERY_CABLE_SOURCE_CALLBACK,

	.event_check = true,
	.event_waiting_time = 600,

	/* Monitor setting */
	.polling_type = SEC_BATTERY_MONITOR_ALARM,
	.monitor_initial_count = 3,

	/* Battery check */
	.battery_check_type = SEC_BATTERY_CHECK_CALLBACK,
	.check_count = 0,
	/* Battery check by ADC */
	.check_adc_max = 0,
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
	.temp_check_count = 2,
	.temp_high_threshold_event = 650,
	.temp_high_recovery_event = 415,
	.temp_low_threshold_event = -30,
	.temp_low_recovery_event = 0,
	.temp_high_threshold_normal = 465,
	.temp_high_recovery_normal = 415,
	.temp_low_threshold_normal = -30,
	.temp_low_recovery_normal = 0,
	.temp_high_threshold_lpm = 450,
	.temp_high_recovery_lpm = 420,
	.temp_low_threshold_lpm = -30,
	.temp_low_recovery_lpm = 0,

	.full_check_type = SEC_BATTERY_FULLCHARGED_CHGPSY,
	.full_check_count = 3,
/*
	.full_check_adc_1st = 20000,
	.full_check_adc_2nd = 20000,
*/
	.chg_gpio_full_check = 0,
	.chg_polarity_full_check = 1,
	.full_condition_type = 0,

	.recharge_condition_type =
		SEC_BATTERY_RECHARGE_CONDITION_SOC |
		SEC_BATTERY_RECHARGE_CONDITION_VCELL,
	.recharge_condition_soc = 98,
	.recharge_condition_vcell = 4165,

	.charging_total_time = 6 * 60 * 60,
	.recharging_total_time = 90 * 60,
	.charging_reset_time = 10 * 60,

	/* Fuel Gauge */
	.fg_irq = MSM_GPIO_TO_INT(GPIO_FUEL_INT),
	.fg_irq_attr =
		IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	.fuel_alert_soc = 4,
	.repeated_fuelalert = false,
	.capacity_calculation_type =
		SEC_FUELGAUGE_CAPACITY_TYPE_RAW |
		SEC_FUELGAUGE_CAPACITY_TYPE_SCALE |
		SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE,
		/* SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC, */
	.capacity_max = 990,
	.capacity_max_margin = 50,
	.capacity_min = 0,

	/* Charger */
	.chg_gpio_en = PM8921_GPIO_PM_TO_SYS(PMIC_GPIO_CHG_EN),
	.chg_polarity_en = 0,
	.chg_gpio_status = 0,
	.chg_polarity_status = 0,
	.chg_irq = 0,
	.chg_irq_attr = 0,
	.chg_float_voltage = 4200,
};

static struct platform_device sec_device_battery = {
	.name = "sec-battery",
	.id = -1,
	.dev.platform_data = &sec_battery_pdata,
};

static struct i2c_gpio_platform_data gpio_i2c_data_fgchg = {
	.sda_pin = GPIO_FUELGAUGE_I2C_SDA,
	.scl_pin = GPIO_FUELGAUGE_I2C_SCL,
};

struct platform_device sec_device_fgchg = {
	.name = "i2c-gpio",
	.id = MSM_FUELGAUGE_I2C_BUS_ID,
	.dev.platform_data = &gpio_i2c_data_fgchg,
};

static struct i2c_board_info sec_brdinfo_fgchg[] __initdata = {
	{
		I2C_BOARD_INFO("sec-charger",
			SEC_CHARGER_I2C_SLAVEADDR),
		.platform_data	= &sec_battery_pdata,
	},
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
__setup("androidboot.batt_check_recovery=", sec_bat_current_boot_mode);

void __init msm8960_init_battery(void)
{
	/* board dependent changes in booting */
	switch (system_rev) {
	case BOARD_REV01:
		sec_battery_pdata.full_check_type =
			SEC_BATTERY_FULLCHARGED_CHGGPIO;
		break;
	}

	platform_add_devices(
		msm8960_battery_devices,
		ARRAY_SIZE(msm8960_battery_devices));

	i2c_register_board_info(
		MSM_FUELGAUGE_I2C_BUS_ID,
		sec_brdinfo_fgchg,
		ARRAY_SIZE(sec_brdinfo_fgchg));
}

#endif

