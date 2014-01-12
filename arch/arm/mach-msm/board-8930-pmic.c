/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/mfd/pm8xxx/pm8038.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <linux/msm_ssbi.h>
#include <asm/mach-types.h>
#include <mach/msm_bus_board.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#include <mach/msm8930-gpio.h>
#include "devices.h"
#include "board-8930.h"
#include <linux/mfd/pm8xxx/pm8921-sec-charger.h>

struct pm8xxx_gpio_init {
	unsigned			gpio;
	struct pm_gpio			config;
};

struct pm8xxx_mpp_init {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

#define PM8038_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8038_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8038_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8038_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8038_GPIO_DISABLE(_gpio) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM8038_GPIO_VIN_L11, \
			 0, 0, 0, 1)

#define PM8038_GPIO_OUTPUT(_gpio, _val) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_INPUT(_gpio, _pull) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8038_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8917_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8917_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8917_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8917_GPIO_DISABLE(_gpio) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM_GPIO_VIN_S4, \
			 0, 0, 0, 1)

#define PM8917_GPIO_OUTPUT(_gpio, _val) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_INPUT(_gpio, _pull) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8917_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_VIN_PAIRED(_gpio, _direction, _vin) \
	PM8917_GPIO_INIT(_gpio, _direction, PM_GPIO_OUT_BUF_CMOS, 0, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_PAIRED, 0, 0)			

/* GPIO and MPP configurations for MSM8930 + PM8038 targets */

/* Initial PM8038 GPIO configurations */
static struct pm8xxx_gpio_init pm8038_gpios[] __initdata = {
	/* SD Card Detect Pin on PMIC GPIO */
#ifdef PMIC_GPIO_SD_CARD_DET_N
	PM8038_GPIO_INPUT(1, PM_GPIO_PULL_NO),
#elif defined(PMIC_GPIO_BACKLIGHT_PWM)
	PM8038_GPIO_OUTPUT_FUNC(1, 0, PM_GPIO_FUNC_1),
#endif
	/* keys GPIOs */
	/* haptics gpio */
	PM8038_GPIO_OUTPUT_FUNC(7, 0, PM_GPIO_FUNC_1),
	/* MHL PWR EN */
	PM8038_GPIO_OUTPUT_VIN(5, 1, PM8038_GPIO_VIN_VPH),
};

/* Initial PM8038 MPP configurations */
static struct pm8xxx_mpp_init pm8038_mpps[] __initdata = {
	/* External 5V regulator enable; shared by HDMI and USB_OTG switches. */
	PM8038_MPP_INIT(3, D_INPUT, PM8038_MPP_DIG_LEVEL_VPH, DIN_TO_INT),
};

/* GPIO and MPP configurations for MSM8930 + PM8917 targets */

/* Initial PM8917 GPIO configurations */
static struct pm8xxx_gpio_init pm8917_gpios[] __initdata = {
	/* SD Card Detect Pin on PMIC GPIO */
#ifdef PMIC_GPIO_SD_CARD_DET_N
	PM8917_GPIO_INPUT(1, PM_GPIO_PULL_NO),
#endif
	/* keys GPIOs */
#if defined(CONFIG_KEYBOARD_MATRIX)
	PM8917_GPIO_INPUT(1, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(2, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(3, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(4, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(6, PM_GPIO_PULL_UP_31P5),

	PM8917_GPIO_OUTPUT_FUNC(11, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(13, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(14, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(16, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(17, 0, PM_GPIO_FUNC_NORMAL),
#endif
#if defined(CONFIG_KEYBOARD_PMIC8XXX)
	PM8917_GPIO_INPUT(1, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(2, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(3, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(4, PM_GPIO_PULL_UP_31P5),
	PM8917_GPIO_INPUT(5, PM_GPIO_PULL_UP_31P5),

	PM8917_GPIO_OUTPUT_FUNC(9, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(10, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(11, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(12, 0, PM_GPIO_FUNC_NORMAL),
	PM8917_GPIO_OUTPUT_FUNC(13, 0, PM_GPIO_FUNC_NORMAL),
#endif
};

#if defined(CONFIG_MACH_MELIUS_CHN_CTC)
static struct pm8xxx_gpio_init pm8917_gpios_uart_paired[] __initdata = {
	PM8917_GPIO_VIN_PAIRED(8,PM_GPIO_DIR_OUT,PM_GPIO_VIN_S4),
	PM8917_GPIO_VIN_PAIRED(9,PM_GPIO_DIR_IN,PM_GPIO_VIN_S4),
};
#endif

/* Initial PM8917 MPP configurations */
static struct pm8xxx_mpp_init pm8917_mpps[] __initdata = {
	/* Configuration for reading pa_therm1(MPP_08)*/						
#if defined(CONFIG_MACH_MELIUS)
		PM8917_MPP_INIT(9, A_INPUT, PM8XXX_MPP_AIN_AMUX_CH8, AOUT_CTRL_DISABLE),
#else
		PM8917_MPP_INIT(PM8XXX_AMUX_MPP_8, A_INPUT, PM8XXX_MPP_AIN_AMUX_CH8,
								DOUT_CTRL_LOW),
#endif
#if defined(CONFIG_MACH_SERRANO_EUR_3G) || defined(CONFIG_MACH_SERRANO_EUR_LTE) \
	|| defined(CONFIG_MACH_SERRANO_KOR_LTE)
		PM8917_MPP_INIT(2, SINK, PM8XXX_MPP_CS_OUT_5MA, CS_CTRL_DISABLE),
#endif
};

void __init msm8930_pm8038_gpio_mpp_init(void)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(pm8038_gpios); i++) {
		rc = pm8xxx_gpio_config(pm8038_gpios[i].gpio,
					&pm8038_gpios[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
			break;
		}
	}

	/* Initial MPP configuration. */
	for (i = 0; i < ARRAY_SIZE(pm8038_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8038_mpps[i].mpp,
					&pm8038_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
			break;
		}
	}
}

#if defined(CONFIG_MACH_MELIUS_CHN_CTC)
extern unsigned int system_rev;
#endif
void __init msm8930_pm8917_gpio_mpp_init(void)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(pm8917_gpios); i++) {
		rc = pm8xxx_gpio_config(pm8917_gpios[i].gpio,
					&pm8917_gpios[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
			break;
		}
	}

#if defined(CONFIG_MACH_MELIUS_CHN_CTC)
	if(system_rev == 0x09) {
		for (i = 0; i < ARRAY_SIZE(pm8917_gpios_uart_paired); i++) {
			rc = pm8xxx_gpio_config(pm8917_gpios_uart_paired[i].gpio,
						&pm8917_gpios_uart_paired[i].config);
			if (rc) {
				pr_err("%s: pm8917_gpios_uart_paired: rc=%d\n", __func__, rc);
				break;
			}
		}
	}
#endif
	/* Initial MPP configuration. */
	for (i = 0; i < ARRAY_SIZE(pm8917_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8917_mpps[i].mpp,
					&pm8917_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
			break;
		}
	}
}

static struct pm8xxx_adc_amux pm8038_adc_channels_data[] = {
	{"vcoin", CHANNEL_VCOIN, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vbat", CHANNEL_VBAT, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"dcin", CHANNEL_DCIN, CHAN_PATH_SCALING4, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ichg", CHANNEL_ICHG, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vph_pwr", CHANNEL_VPH_PWR, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ibat", CHANNEL_IBAT, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"batt_therm", CHANNEL_BATT_THERM, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_BATT_THERM},
	{"batt_id", CHANNEL_BATT_ID, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"usbin", CHANNEL_USBIN, CHAN_PATH_SCALING3, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pmic_therm", CHANNEL_DIE_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PMIC_THERM},
	{"625mv", CHANNEL_625MV, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"125v", CHANNEL_125V, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"chg_temp", CHANNEL_CHG_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pa_therm1", ADC_MPP_1_AMUX4, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
	{"xo_therm", CHANNEL_MUXOFF, CHAN_PATH_SCALING1, AMUX_RSV0,
		ADC_DECIMATION_TYPE2, ADC_SCALE_XOTHERM},
	{"pa_therm0", ADC_MPP_1_AMUX3, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
	{"dev_mpp_7", ADC_MPP_1_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_SEC_BOARD_THERM},  /*main_thm */
#ifdef CONFIG_SAMSUNG_JACK
	{"earjack", ADC_MPP_1_AMUX6_SCALE_DEFAULT,
		CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
#endif
};

static struct pm8xxx_adc_properties pm8038_adc_data = {
	.adc_vdd_reference	= 1800, /* milli-voltage for this adc */
	.bitresolution		= 15,
	.bipolar                = 0,
};

static struct pm8xxx_adc_platform_data pm8038_adc_pdata = {
	.adc_channel            = pm8038_adc_channels_data,
	.adc_num_board_channel  = ARRAY_SIZE(pm8038_adc_channels_data),
	.adc_prop               = &pm8038_adc_data,
	.adc_mpp_base		= PM8038_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_irq_platform_data pm8xxx_irq_pdata __devinitdata = {
	.irq_base		= PM8038_IRQ_BASE,
	.devirq			= MSM_GPIO_TO_INT(104),
	.irq_trigger_flag	= IRQF_TRIGGER_LOW,
};

static struct pm8xxx_gpio_platform_data pm8xxx_gpio_pdata __devinitdata = {
	.gpio_base	= PM8038_GPIO_PM_TO_SYS(1),
};

static struct pm8xxx_mpp_platform_data pm8xxx_mpp_pdata __devinitdata = {
	.mpp_base	= PM8038_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_rtc_platform_data pm8xxx_rtc_pdata __devinitdata = {
	.rtc_write_enable	= false,
	.rtc_alarm_powerup	= false,
};

static struct pm8xxx_pwrkey_platform_data pm8xxx_pwrkey_pdata = {
	.pull_up		= 1,
	.kpd_trigger_delay_us	= 15625,
	.wakeup			= 1,
};

#ifdef CONFIG_KEYBOARD_PMIC8XXX
static const unsigned int pm8xxx_keymap[] = {
/* KEY(row, col, keycode) */
/* row = scan, col - sense */
KEY(0, 0, KEY_SEND), 
KEY(0, 1, KEY_BACKSPACE),
KEY(0, 2, KEY_HOMEPAGE),
KEY(0, 3, KEY_CAMERA),	
KEY(0, 4, KEY_ENTER), 

KEY(1, 0, KEY_1), 
KEY(1, 1, KEY_2),	
KEY(1, 2, KEY_3), 
KEY(1, 3, KEY_RIGHT),
KEY(1, 4, KEY_DOWN), 

KEY(2, 0, KEY_4), 
KEY(2, 1, KEY_5), 
KEY(2, 2, KEY_6), 
KEY(2, 3, KEY_BACK),
KEY(2, 4, KEY_LEFT), 

KEY(3, 0, KEY_7), 
KEY(3, 1, KEY_8), 
KEY(3, 2, KEY_9), 
KEY(3, 3, KEY_UP),

KEY(4, 0, KEY_NUMERIC_STAR),
KEY(4, 1, KEY_0),
KEY(4, 2, KEY_NUMERIC_POUND),
KEY(4, 3, KEY_MENU), 
};

static struct matrix_keymap_data pm8xxx_keymap_data = {
	.keymap_size	= ARRAY_SIZE(pm8xxx_keymap),
	.keymap			= pm8xxx_keymap,
};

static struct pm8xxx_keypad_platform_data pm8xxx_keypad_pdata = {
	.input_name			= "ks02_3x4_keypad",
	.input_phys_device	= "ks02_3x4/input0",
	.num_rows			= 5,
	.num_cols			= 5,	
	.rows_gpio_start	= PM8917_GPIO_PM_TO_SYS(9),
	.cols_gpio_start	= PM8917_GPIO_PM_TO_SYS(1),
	.debounce_ms		= 15,
	.scan_delay_ms		= 32,
	.row_hold_ns		= 91500,
	.wakeup 			= 1,
	.keymap_data		= &pm8xxx_keymap_data,
};
#endif


static int pm8921_therm_mitigation[] = {
	1100,
	700,
	600,
	325,
};

/* it has to be matched with cable_type_t */
static struct pm8921_charging_current charging_current_table[] = {
		{ 0,			0},		/* NONE */
		{ 500,		475},	/* USB */
		{ 1000,		1500},	/* AC */
		{ 1000,		1500},	/* MISC */
		{ 1000,		1500},	/* CARDOCK */
		{ 1000,		1500},	/* UARTOFF */
		{ 0,			0},	/* JIG */
		{ 0,			0},	/* UNKNOWN */
		{ 1000,		1500},	/* CDP */
		{ 1000,		1500},	/* SMART_DOCK */
		{ 0,			0},	/* OTG */
		{ 1000,		1500},	/* AUDIO_DOCK */
#ifdef CONFIG_WIRELESS_CHARGING
		{ 700,		1000},	/* WPC */
		{ 1000,		1000},	/* INCOMPATIBLE */
		{ 1000,		1500},	/* DEST_DOCK */
#endif
};
#if defined(CONFIG_USB_SWITCH_TSU6721) && \
	(defined(CONFIG_MACH_SERRANO_EUR_LTE) || defined(CONFIG_MACH_SERRANO_EUR_3G) || defined(CONFIG_MACH_SERRANO_KOR_LTE))
extern void tsu6721_monitor(void);
#endif

static void sec_bat_monitor_additional_check(void)
{
#if defined(CONFIG_USB_SWITCH_TSU6721) && \
	(defined(CONFIG_MACH_SERRANO_EUR_LTE) || defined(CONFIG_MACH_SERRANO_EUR_3G) || defined(CONFIG_MACH_SERRANO_KOR_LTE))
	tsu6721_monitor();
#endif
}

static struct pm8921_sec_battery_data pm8921_battery_pdata __devinitdata = {
	/* for absolute timer (first) */
	.charging_total_time			= 6 * 60 * 60,	/* 6hr */
	/* for recharging timer (second) */
	.recharging_total_time			= 90 * 60,	/* 1.5hr */
#if defined(CONFIG_MACH_SERRANO_EUR_LTE) || defined(CONFIG_MACH_SERRANO_EUR_3G) || defined(CONFIG_MACH_CANE_EUR_3G) \
	|| defined(CONFIG_MACH_SERRANO_KOR_LTE)
	/* temperature set (event) */
	.temp_high_block_event		= 609,
	.temp_high_recover_event		= 422,
	.temp_low_block_event			= -60,
	.temp_low_recover_event		= -6,
	/* temperature set (normal) */
	.temp_high_block_normal		= 609,
	.temp_high_recover_normal		= 422,
	.temp_low_block_normal		= -60,
	.temp_low_recover_normal		= -6,
	/* temperature set (lpm) */
	.temp_high_block_lpm			= 609,
	.temp_high_recover_lpm		= 422,
	.temp_low_block_lpm			= -60,
	.temp_low_recover_lpm			= -6,
#else
	/* temperature set (event) */
	.temp_high_block_event		= 600,
	.temp_high_recover_event		= 400,
	.temp_low_block_event			= -50,
	.temp_low_recover_event		= 0,
	/* temperature set (normal) */
	.temp_high_block_normal		= 600,
	.temp_high_recover_normal		= 400,
	.temp_low_block_normal		= -50,
	.temp_low_recover_normal		= 0,
	/* temperature set (lpm) */
	.temp_high_block_lpm			= 600,
	.temp_high_recover_lpm		= 400,
	.temp_low_block_lpm			= -50,
	.temp_low_recover_lpm			= 0,
#endif
	.event_check = true,
	.event_waiting_time = 600, /* 10min */
	/* capacity is	0.1% unit */
	.capacity_max = 1000,
	.capacity_max_margin = 50,
	.capacity_min = 0,

#if defined(CONFIG_MACH_WILCOX_EUR_LTE)
	.ui_term_current = 180,
#else
	.ui_term_current = 130,
#endif
	.charging_term_time = 0,
	.recharging_voltage = 4300,
	.poweroff_check_soc = 1,

	.chg_current_table	= charging_current_table,
	.monitor_additional_check = sec_bat_monitor_additional_check,
};

#ifdef CONFIG_SAMSUNG_LPM_MODE
bool sec_bat_is_lpm(void) {return (bool)poweroff_charging; }
#endif

#define R_CONN	45	
#define R_SENSE 10000

#define MAX_VOLTAGE_MV		4350
#define CHG_TERM_MA		60
static struct pm8921_charger_platform_data pm8921_chg_pdata __devinitdata = {
	.safety_time		= 512,
	.update_time		= 60000,
	.sleep_update_time	= 600,
	.cool_temp		= INT_MIN,
	.warm_temp		= INT_MIN,
#if defined(CONFIG_PM8921_SEC_CHARGER)
	.get_cable_type		= msm8930_get_cable_status,
	.get_lpm_mode		= sec_bat_is_lpm,
	.get_board_rev		= msm8930_get_board_rev,
#if defined(CONFIG_WIRELESS_CHARGING)
	.wc_w_gpio		= PMIC_GPIO_WPC_INT,
	.wpc_acok		= PMIC_GPIO_WPC_ACOK,
	.wpc_int_init		= wpc_hw_init,
#endif
#endif
	.max_voltage		= MAX_VOLTAGE_MV,
	.min_voltage		= 3400,
#if defined(CONFIG_MACH_WILCOX_EUR_LTE)
	.weak_voltage	= 3000,
#else
	.weak_voltage	= 3200,
#endif
	.trkl_current		= 200,
	.trkl_voltage	= 2100,
	.uvd_thresh_voltage = 4050,
	.alarm_low_mv		= 3400,
	.alarm_high_mv		= 4000,
	/* disable VBATDET for improper FSB state as 1(On high) */
	.resume_voltage_delta	= (-50),	/* VBATDET voltage (4.40V) */
	.resume_charge_percent	= 99,
	.term_current		= CHG_TERM_MA,
	.temp_check_period	= 1,
	.max_bat_chg_current	= 2000,
	.usb_max_current		= 1000,
	.cool_bat_chg_current	= 350,
	.warm_bat_chg_current	= 350,
	.cool_bat_voltage	= MAX_VOLTAGE_MV,
	.warm_bat_voltage	= MAX_VOLTAGE_MV,
	.thermal_mitigation	= pm8921_therm_mitigation,
	.thermal_levels		= ARRAY_SIZE(pm8921_therm_mitigation),
	.rconn_mohm		= R_CONN,
	.dc_unplug_check	= true,
	.batt_pdata		= &pm8921_battery_pdata,
};

static struct pm8xxx_vibrator_platform_data pm8038_vib_pdata = {
	.initial_vibrate_ms = 500,
	.level_mV = 3000,
	.max_timeout_ms = 15000,
};

#define PM8038_WLED_MAX_CURRENT		25
#define PM8XXX_LED_PWM_PERIOD		1000
#define PM8XXX_LED_PWM_DUTY_MS		20
#define PM8038_RGB_LED_MAX_CURRENT	12

static struct led_info pm8038_led_info[] = {
	[0] = {
		.name			= "wled",
		.default_trigger	= "bkl_trigger",
	},
	[1] = {
		.name			= "led:rgb_red",
		.default_trigger	= "battery-charging",
	},
	[2] = {
		.name			= "led:rgb_green",
	},
	[3] = {
		.name			= "led:rgb_blue",
	},
};

static struct led_platform_data pm8038_led_core_pdata = {
	.num_leds = ARRAY_SIZE(pm8038_led_info),
	.leds = pm8038_led_info,
};

static struct wled_config_data wled_cfg = {
	.dig_mod_gen_en = true,
	.cs_out_en = true,
	.ctrl_delay_us = 0,
	.op_fdbck = true,
	.ovp_val = WLED_OVP_32V,
	.boost_curr_lim = WLED_CURR_LIMIT_525mA,
	.num_strings = 1,
};

static int pm8038_led0_pwm_duty_pcts[56] = {
		1, 4, 8, 12, 16, 20, 24, 28, 32, 36,
		40, 44, 46, 52, 56, 60, 64, 68, 72, 76,
		80, 84, 88, 92, 96, 100, 100, 100, 98, 95,
		92, 88, 84, 82, 78, 74, 70, 66, 62, 58,
		58, 54, 50, 48, 42, 38, 34, 30, 26, 22,
		14, 10, 6, 4, 1
};

/*
 * Note: There is a bug in LPG module that results in incorrect
 * behavior of pattern when LUT index 0 is used. So effectively
 * there are 63 usable LUT entries.
 */
static struct pm8xxx_pwm_duty_cycles pm8038_led0_pwm_duty_cycles = {
	.duty_pcts = (int *)&pm8038_led0_pwm_duty_pcts,
	.num_duty_pcts = ARRAY_SIZE(pm8038_led0_pwm_duty_pcts),
	.duty_ms = PM8XXX_LED_PWM_DUTY_MS,
	.start_idx = 1,
};

static struct pm8xxx_led_config pm8038_led_configs[] = {
	[0] = {
		.id = PM8XXX_ID_WLED,
		.mode = PM8XXX_LED_MODE_MANUAL,
		.max_current = PM8038_WLED_MAX_CURRENT,
		.default_state = 0,
		.wled_cfg = &wled_cfg,
	},
	[1] = {
		.id = PM8XXX_ID_RGB_LED_RED,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 5,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
	},
	[2] = {
		.id = PM8XXX_ID_RGB_LED_GREEN,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 4,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
	},
	[3] = {
		.id = PM8XXX_ID_RGB_LED_BLUE,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 3,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
	},
};

static struct pm8xxx_led_platform_data pm8xxx_leds_pdata = {
	.led_core = &pm8038_led_core_pdata,
	.configs = pm8038_led_configs,
	.num_configs = ARRAY_SIZE(pm8038_led_configs),
};

static struct pm8xxx_ccadc_platform_data pm8xxx_ccadc_pdata = {
	.r_sense_uohm		= R_SENSE,
	.calib_delay_ms		= 600000,
	.periodic_wakeup		= true,
};

static struct pm8xxx_misc_platform_data pm8xxx_misc_pdata = {
	.priority		= 0,
};

/*
 *	0x254=0xC8 (Threshold=110, preamp bias=01)
 *	0x255=0xC1 (Hold=110, max attn=0000, mute=1)
 *	0x256=0xB0 (decay=101, attack=10, delay=0)
 */

static struct pm8xxx_spk_platform_data pm8xxx_spk_pdata = {
	.spk_add_enable		= false,
	.cd_ng_threshold	= 0x6,
	.cd_nf_preamp_bias	= 0x1,
	.cd_ng_hold		= 0x6,
	.cd_ng_max_atten	= 0x0,
	.noise_mute		= 1,
	.cd_ng_decay_rate	= 0x5,
	.cd_ng_attack_rate	= 0x2,
	.cd_delay		= 0x0,
};

static struct pm8921_bms_platform_data pm8921_bms_pdata __devinitdata = {
	.battery_type			= BATT_SEC,
	.r_sense_uohm			= R_SENSE,
	.v_cutoff			= 3400,
	.max_voltage_uv			= MAX_VOLTAGE_MV * 1000,
	.shutdown_soc_valid_limit	= 20,
	.adjust_soc_low_threshold	= 25,
	.chg_term_ua			= CHG_TERM_MA * 1000,
	.rconn_mohm			= R_CONN,
	.normal_voltage_calc_ms		= 20000,
	.low_voltage_calc_ms		= 1000,
	.disable_flat_portion_ocv = 1,
	.ocv_dis_high_soc = 99,
	.ocv_dis_low_soc = 1,
	.alarm_low_mv		= 3400,
	.alarm_high_mv		= 4000,
	.high_ocv_correction_limit_uv		= 100,
	.low_ocv_correction_limit_uv		= 150,
	.cutoff_ocv_correction_uv	= 200,
	.hold_soc_est				= 3,
	.get_board_rev		= msm8930_get_board_rev,
};

static struct pm8038_platform_data pm8038_platform_data __devinitdata = {
	.irq_pdata		= &pm8xxx_irq_pdata,
	.gpio_pdata		= &pm8xxx_gpio_pdata,
	.mpp_pdata		= &pm8xxx_mpp_pdata,
	.rtc_pdata              = &pm8xxx_rtc_pdata,
	.pwrkey_pdata		= &pm8xxx_pwrkey_pdata,
	.misc_pdata		= &pm8xxx_misc_pdata,
	.regulator_pdatas	= msm8930_pm8038_regulator_pdata,
	.charger_pdata		= &pm8921_chg_pdata,
	.bms_pdata		= &pm8921_bms_pdata,
	.adc_pdata		= &pm8038_adc_pdata,
	.leds_pdata		= &pm8xxx_leds_pdata,
	.ccadc_pdata		= &pm8xxx_ccadc_pdata,
	.spk_pdata		= &pm8xxx_spk_pdata,
};

static struct msm_ssbi_platform_data msm8930_ssbi_pm8038_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name			= "pm8038-core",
		.platform_data		= &pm8038_platform_data,
	},
};

/* PM8917 platform data */

static struct pm8xxx_adc_amux pm8917_adc_channels_data[] = {
	{"vcoin", CHANNEL_VCOIN, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vbat", CHANNEL_VBAT, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"dcin", CHANNEL_DCIN, CHAN_PATH_SCALING4, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ichg", CHANNEL_ICHG, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vph_pwr", CHANNEL_VPH_PWR, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ibat", CHANNEL_IBAT, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"batt_therm", CHANNEL_BATT_THERM, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_BATT_THERM},
	{"batt_id", CHANNEL_BATT_ID, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"usbin", CHANNEL_USBIN, CHAN_PATH_SCALING3, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pmic_therm", CHANNEL_DIE_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PMIC_THERM},
	{"625mv", CHANNEL_625MV, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"125v", CHANNEL_125V, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"chg_temp", CHANNEL_CHG_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pa_therm1", ADC_MPP_1_AMUX8, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},	/* mpp_08 */
	{"xo_therm", CHANNEL_MUXOFF, CHAN_PATH_SCALING1, AMUX_RSV0,
		ADC_DECIMATION_TYPE2, ADC_SCALE_XOTHERM},
	{"pa_therm0", ADC_MPP_1_AMUX3, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
	{"dev_mpp_3", ADC_MPP_1_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_SEC_BOARD_THERM},  /*main_thm */
#if defined(CONFIG_MACH_MELIUS)
	{"dev_mpp_8", ADC_MPP_2_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},  /*vf_adc*/
#endif
#if defined(CONFIG_MACH_BISCOTTO)
	{"dev_mpp_7", ADC_MPP_2_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},  /*charging current*/
#endif
#ifdef CONFIG_SAMSUNG_JACK
#ifdef CONFIG_SAMSUNG_JACK_ADC_SCALE3
	{"amux_2_6", ADC_MPP_2_AMUX6, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
#endif
	{"earjack", ADC_MPP_1_AMUX6_SCALE_DEFAULT,
		CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
#endif
};

static struct pm8xxx_adc_properties pm8917_adc_data = {
	.adc_vdd_reference	= 1800, /* milli-voltage for this adc */
	.bitresolution		= 15,
	.bipolar                = 0,
};

static struct pm8xxx_adc_platform_data pm8917_adc_pdata = {
	.adc_channel            = pm8917_adc_channels_data,
	.adc_num_board_channel  = ARRAY_SIZE(pm8917_adc_channels_data),
	.adc_prop               = &pm8917_adc_data,
	.adc_mpp_base		= PM8917_MPP_PM_TO_SYS(1),
};

static struct pm8921_platform_data pm8917_platform_data __devinitdata = {
	.irq_pdata		= &pm8xxx_irq_pdata,
	.gpio_pdata		= &pm8xxx_gpio_pdata,
	.mpp_pdata		= &pm8xxx_mpp_pdata,
	.rtc_pdata              = &pm8xxx_rtc_pdata,
	.pwrkey_pdata		= &pm8xxx_pwrkey_pdata,
	.misc_pdata		= &pm8xxx_misc_pdata,
	.regulator_pdatas	= msm8930_pm8917_regulator_pdata,
	.charger_pdata		= &pm8921_chg_pdata,
	.bms_pdata		= &pm8921_bms_pdata,
	.adc_pdata		= &pm8917_adc_pdata,
	.ccadc_pdata		= &pm8xxx_ccadc_pdata,
#if defined(CONFIG_KEYBOARD_PMIC8XXX)
	.keypad_pdata		= &pm8xxx_keypad_pdata,			
#endif
};

static struct msm_ssbi_platform_data msm8930_ssbi_pm8917_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name			= "pm8921-core",
		.platform_data		= &pm8917_platform_data,
	},
};

void __init msm8930_init_pmic(void)
{
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917) {
		/* PM8038 configuration */
#if !defined (CONFIG_SEC_DISABLE_HARDRESET)	
		pmic_reset_irq = PM8038_IRQ_BASE + PM8038_RESOUT_IRQ;
#endif	
		msm8960_device_ssbi_pmic.dev.platform_data =
					&msm8930_ssbi_pm8038_pdata;
		pm8038_platform_data.num_regulators
			= msm8930_pm8038_regulator_pdata_len;
		if (machine_is_msm8930_mtp())
			pm8921_bms_pdata.battery_type = BATT_PALLADIUM;
		else if (machine_is_msm8930_cdp())
			pm8921_chg_pdata.has_dc_supply = true;
		if (machine_is_msm8930_evt())
			pm8038_platform_data.vibrator_pdata =
				&pm8038_vib_pdata;
	} else {
		/* PM8917 configuration */
#if !defined (CONFIG_SEC_DISABLE_HARDRESET)		
		pmic_reset_irq = PM8917_IRQ_BASE + PM8921_RESOUT_IRQ;
#endif		
		msm8960_device_ssbi_pmic.dev.platform_data =
					&msm8930_ssbi_pm8917_pdata;
		pm8917_platform_data.num_regulators
			= msm8930_pm8917_regulator_pdata_len;
		if (machine_is_msm8930_mtp())
			pm8921_bms_pdata.battery_type = BATT_PALLADIUM;
		else if (machine_is_msm8930_cdp())
			pm8921_chg_pdata.has_dc_supply = true;
	}

	if (!machine_is_msm8930_mtp())
		pm8921_chg_pdata.battery_less_hardware = 1;
}
