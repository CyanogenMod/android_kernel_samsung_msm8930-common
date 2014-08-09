/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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
#define pr_fmt(fmt)	"%s: " fmt, __func__

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/android_alarm.h>
#include <linux/mfd/pm8xxx/pm8921-charger.h>
#include <linux/mfd/pm8xxx/pm8921-sec-charger.h>
#include <linux/mfd/pm8xxx/pm8921-sec-attrs.h>
#include <linux/mfd/pm8xxx/pm8921-bms.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <linux/mfd/pm8xxx/ccadc.h>
#include <linux/mfd/pm8xxx/core.h>
#include <linux/regulator/consumer.h>
#include <linux/interrupt.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/workqueue.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/ratelimit.h>

#include <mach/msm_xo.h>
#include <mach/msm_hsusb.h>
#include <mach/msm8930-gpio.h>

#include <linux/battery/sec_charging_common.h>
#include <linux/i2c/tsu6721.h>

#define PM8917_SEC_CHARGER_CODE

#define CHG_BUCK_CLOCK_CTRL	0x14

#define PBL_ACCESS1		0x04
#define PBL_ACCESS2		0x05
#define SYS_CONFIG_1		0x06
#define SYS_CONFIG_2		0x07
#define CHG_CNTRL		0x204
#define CHG_IBAT_MAX		0x205
#define CHG_TEST		0x206
#define CHG_BUCK_CTRL_TEST1	0x207
#define CHG_BUCK_CTRL_TEST2	0x208
#define CHG_BUCK_CTRL_TEST3	0x209
#define COMPARATOR_OVERRIDE	0x20A
#define PSI_TXRX_SAMPLE_DATA_0	0x20B
#define PSI_TXRX_SAMPLE_DATA_1	0x20C
#define PSI_TXRX_SAMPLE_DATA_2	0x20D
#define PSI_TXRX_SAMPLE_DATA_3	0x20E
#define PSI_CONFIG_STATUS	0x20F
#define CHG_IBAT_SAFE		0x210
#define CHG_ITRICKLE		0x211
#define CHG_CNTRL_2		0x212
#define CHG_VBAT_DET		0x213
#define CHG_VTRICKLE		0x214
#define CHG_ITERM		0x215
#define CHG_CNTRL_3		0x216
#define CHG_VIN_MIN		0x217
#define CHG_TWDOG		0x218
#define CHG_TTRKL_MAX		0x219
#define CHG_TEMP_THRESH		0x21A
#define CHG_TCHG_MAX		0x21B
#define USB_OVP_CONTROL		0x21C
#define DC_OVP_CONTROL		0x21D
#define USB_OVP_TEST		0x21E
#define DC_OVP_TEST		0x21F
#define CHG_VDD_MAX		0x220
#define CHG_VDD_SAFE		0x221
#define CHG_VBAT_BOOT_THRESH	0x222
#define USB_OVP_TRIM		0x355
#define BUCK_CONTROL_TRIM1	0x356
#define BUCK_CONTROL_TRIM2	0x357
#define BUCK_CONTROL_TRIM3	0x358
#define BUCK_CONTROL_TRIM4	0x359
#define CHG_DEFAULTS_TRIM	0x35A
#define CHG_ITRIM		0x35B
#define CHG_TTRIM		0x35C
#define CHG_COMP_OVR		0x20A
#define IUSB_FINE_RES		0x2B6
#define OVP_USB_UVD		0x2B7

/* check EOC every 10 seconds */
#define EOC_CHECK_PERIOD_MS	10000
/* check for USB unplug every 200 msecs */
#define UNPLUG_CHECK_WAIT_PERIOD_MS 1000
#define UNPLUG_CHECK_RAMP_MS 25
#define USB_TRIM_ENTRIES 16  /* [110612] QC USB soft trim patch */

#ifdef CONFIG_SEC_DEBUG_SUBSYS
u8 uvd_thresh;
#endif

enum chg_fsm_state {
	FSM_STATE_OFF_0 = 0,
	FSM_STATE_BATFETDET_START_12 = 12,
	FSM_STATE_BATFETDET_END_16 = 16,
	FSM_STATE_ON_CHG_HIGHI_1 = 1,
	FSM_STATE_ATC_2A = 2,
	FSM_STATE_ATC_2B = 18,
	FSM_STATE_ON_BAT_3 = 3,
	FSM_STATE_ATC_FAIL_4 = 4 ,
	FSM_STATE_DELAY_5 = 5,
	FSM_STATE_ON_CHG_AND_BAT_6 = 6,
	FSM_STATE_FAST_CHG_7 = 7,
	FSM_STATE_TRKL_CHG_8 = 8,
	FSM_STATE_CHG_FAIL_9 = 9,
	FSM_STATE_EOC_10 = 10,
	FSM_STATE_ON_CHG_VREGOK_11 = 11,
	FSM_STATE_ATC_PAUSE_13 = 13,
	FSM_STATE_FAST_CHG_PAUSE_14 = 14,
	FSM_STATE_TRKL_CHG_PAUSE_15 = 15,
	FSM_STATE_START_BOOT = 20,
	FSM_STATE_FLCB_VREGOK = 21,
	FSM_STATE_FLCB = 22,
};

struct fsm_state_to_batt_status {
	enum chg_fsm_state	fsm_state;
	int			batt_state;
};

/*
static struct fsm_state_to_batt_status map[] = {
	{FSM_STATE_OFF_0, POWER_SUPPLY_STATUS_UNKNOWN},
	{FSM_STATE_BATFETDET_START_12, POWER_SUPPLY_STATUS_UNKNOWN},
	{FSM_STATE_BATFETDET_END_16, POWER_SUPPLY_STATUS_UNKNOWN},
*/	/*
	 * for CHG_HIGHI_1 report NOT_CHARGING if battery missing,
	 * too hot/cold, charger too hot
	 *//*
	{FSM_STATE_ON_CHG_HIGHI_1, POWER_SUPPLY_STATUS_FULL},
	{FSM_STATE_ATC_2A, POWER_SUPPLY_STATUS_CHARGING},
	{FSM_STATE_ATC_2B, POWER_SUPPLY_STATUS_CHARGING},
	{FSM_STATE_ON_BAT_3, POWER_SUPPLY_STATUS_DISCHARGING},
	{FSM_STATE_ATC_FAIL_4, POWER_SUPPLY_STATUS_DISCHARGING},
	{FSM_STATE_DELAY_5, POWER_SUPPLY_STATUS_UNKNOWN },
	{FSM_STATE_ON_CHG_AND_BAT_6, POWER_SUPPLY_STATUS_CHARGING},
	{FSM_STATE_FAST_CHG_7, POWER_SUPPLY_STATUS_CHARGING},
	{FSM_STATE_TRKL_CHG_8, POWER_SUPPLY_STATUS_CHARGING},
	{FSM_STATE_CHG_FAIL_9, POWER_SUPPLY_STATUS_DISCHARGING},
	{FSM_STATE_EOC_10, POWER_SUPPLY_STATUS_FULL},
	{FSM_STATE_ON_CHG_VREGOK_11, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_ATC_PAUSE_13, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_FAST_CHG_PAUSE_14, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_TRKL_CHG_PAUSE_15, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_START_BOOT, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_FLCB_VREGOK, POWER_SUPPLY_STATUS_NOT_CHARGING},
	{FSM_STATE_FLCB, POWER_SUPPLY_STATUS_NOT_CHARGING},
};
*/
enum chg_regulation_loop {
	VDD_LOOP = BIT(3),
	BAT_CURRENT_LOOP = BIT(2),
	INPUT_CURRENT_LOOP = BIT(1),
	INPUT_VOLTAGE_LOOP = BIT(0),
	CHG_ALL_LOOPS = VDD_LOOP | BAT_CURRENT_LOOP
			| INPUT_CURRENT_LOOP | INPUT_VOLTAGE_LOOP,
};

enum pmic_chg_interrupts {
	USBIN_VALID_IRQ = 0,
	USBIN_OV_IRQ,
	BATT_INSERTED_IRQ,
	VBATDET_LOW_IRQ,
	USBIN_UV_IRQ,
	VBAT_OV_IRQ,
	CHGWDOG_IRQ,
	VCP_IRQ,
	ATCDONE_IRQ,
	ATCFAIL_IRQ,
	CHGDONE_IRQ,
	CHGFAIL_IRQ,
	CHGSTATE_IRQ,
	LOOP_CHANGE_IRQ,
	FASTCHG_IRQ,
	TRKLCHG_IRQ,
	BATT_REMOVED_IRQ,
	BATTTEMP_HOT_IRQ,
	CHGHOT_IRQ,
	BATTTEMP_COLD_IRQ,
	CHG_GONE_IRQ,
	BAT_TEMP_OK_IRQ,
	COARSE_DET_LOW_IRQ,
	VDD_LOOP_IRQ,
	VREG_OV_IRQ,
	VBATDET_IRQ,
	BATFET_IRQ,
	PSI_IRQ,
	DCIN_VALID_IRQ,
	DCIN_OV_IRQ,
	DCIN_UV_IRQ,
	PM_CHG_MAX_INTS,
};

struct bms_notify {
	int			is_battery_full;
	int			is_charging;
	struct	work_struct	work;
};

/**
 * struct pm8921_chg_chip -device information
 * @dev:			device pointer to access the parent
 * @usb_present:		present status of usb
 * @dc_present:			present status of dc
 * @usb_charger_current:	usb current to charge the battery with used when
 *				the usb path is enabled or charging is resumed
 * @safety_time:		max time for which charging will happen
 * @update_time:		how frequently the userland needs to be updated
 * @max_voltage_mv:		the max volts the batt should be charged up to
 * @min_voltage_mv:		the min battery voltage before turning the FETon
 * @uvd_voltage_mv:		(PM8917 only) the falling UVD threshold voltage
 * @alarm_low_mv:		the battery alarm voltage low
 * @alarm_high_mv:		the battery alarm voltage high
 * @cool_temp_dc:		the cool temp threshold in deciCelcius
 * @warm_temp_dc:		the warm temp threshold in deciCelcius
 * @resume_voltage_delta:	the voltage delta from vdd max at which the
 *				battery should resume charging
 * @term_current:		The charging based term current
 *
 */
struct pm8921_chg_chip {
	struct device			*dev;
	unsigned int			usb_present;
	unsigned int			dc_present;
	unsigned int			usb_charger_current;
	unsigned int			max_bat_chg_current;
	unsigned int			pmic_chg_irq[PM_CHG_MAX_INTS];
	unsigned int			safety_time;
	unsigned int			ttrkl_time;
	unsigned int			update_time;
	unsigned int			sleep_update_time;
	unsigned int			max_voltage_mv;
	unsigned int			min_voltage_mv;
	unsigned int			uvd_voltage_mv;
	unsigned int			alarm_low_mv;
	unsigned int			alarm_high_mv;
	int				cool_temp_dc;
	int				warm_temp_dc;
	unsigned int			temp_check_period;
	unsigned int			cool_bat_chg_current;
	unsigned int			warm_bat_chg_current;
	unsigned int			cool_bat_voltage;
	unsigned int			warm_bat_voltage;
	unsigned int			is_bat_cool;
	unsigned int			is_bat_warm;
	/* disable VBATDET for improper FSB state as 1(On high) */
	int						resume_voltage_delta;
	int				resume_charge_percent;
	unsigned int			term_current;
	unsigned int			vbat_channel;
	unsigned int			batt_temp_channel;
	unsigned int			batt_id_channel;
	struct power_supply		usb_psy;
	struct power_supply		dc_psy;
	struct power_supply		*ext_psy;
	struct power_supply		batt_psy;
	struct power_supply		fg_psy;
	struct dentry			*dent;
	struct bms_notify		bms_notify;
	 /* [110612] QC USB soft trim patch */
	int				*usb_trim_table;
	bool				ext_charging;
	bool				ext_charge_done;
	bool				iusb_fine_res;
	bool				dc_unplug_check;
	DECLARE_BITMAP(enabled_irqs, PM_CHG_MAX_INTS);
	struct work_struct		battery_id_valid_work;
	int64_t				batt_id_min;
	int64_t				batt_id_max;
	int				trkl_voltage;
	int				weak_voltage;
	int				trkl_current;
	int				weak_current;
	int				vin_min;
	unsigned int			*thermal_mitigation;
	int				thermal_levels;
	struct delayed_work		update_heartbeat_work;
	struct delayed_work		eoc_work;
	struct delayed_work		unplug_check_work;
	struct delayed_work		vin_collapse_check_work;
	struct wake_lock		eoc_wake_lock;
	enum pm8921_chg_cold_thr	cold_thr;
	enum pm8921_chg_hot_thr		hot_thr;
	int				rconn_mohm;
	enum pm8921_chg_led_src_config	led_src_config;
	bool				host_mode;
	bool				has_dc_supply;
	u8				active_path;
	int				recent_reported_soc;
	/* samsung add variable */
#ifdef PM8917_SEC_CHARGER_CODE
	enum cable_type_t			cable_type;
	bool			charging_enabled;
	unsigned int			hw_rev;
	unsigned int			batt_status;
	unsigned int			batt_present;
	unsigned int			health_cnt;
	unsigned int			batt_health;
	unsigned long		charging_start_time;
	unsigned long		charging_passed_time;
	unsigned long		charging_term_time;
	bool				is_recharging;
	bool				is_chgtime_expired;
/*	unsigned int			batt_soc; */
/*	unsigned int			batt_curr; */
/*	unsigned int			batt_vcell; */
	unsigned int			batt_temp;
	unsigned int			batt_temp_adc;
	bool				slate_mode;
	bool				factory_mode;
	int					capacity_value[10];

	/* event set */
	unsigned int event;
	unsigned int event_wait;
	struct alarm event_termination_alarm;
	ktime_t	last_event_time;

	struct alarm update_alarm;
	ktime_t last_update_time;

	int (*get_cable_type)(void);
	bool (*get_lpm_mode)(void);

	unsigned int			capacity_max;
	unsigned int			capacity_old;
	unsigned int			capacity_raw;
	bool				initial_update_of_soc;
	bool				is_in_sleep;
	int				siop_level;

	unsigned int			ui_term_cnt;
	unsigned int			recharging_cnt;
	int			initial_count;
	bool				boot_completed;
	int			wc_w_gpio;
	int			wc_w_state;
	int			wpc_acok;
	bool		disable_aicl;
	int		cable_exception;

	struct pm8921_sec_battery_data *batt_pdata;
	struct wake_lock monitor_wake_lock;
	struct wake_lock cable_wake_lock;
#endif
};

/* user space parameter to limit usb current */
static unsigned int usb_max_current;
/*
 * usb_target_ma is used for wall charger
 * adaptive input current limiting only. Use
 * pm_iusbmax_get() to get current maximum usb current setting.
 */
static int usb_target_ma;
static int charging_disabled;
static int thermal_mitigation;

static struct pm8921_chg_chip *the_chip;

static void handle_cable_insertion_removal(struct pm8921_chg_chip *chip);
static void handle_cable_insertion_removal_for_wq(struct pm8921_chg_chip *chip);
/* static void batt_status_update(struct pm8921_chg_chip *chip); */
static int get_prop_batt_temp(struct pm8921_chg_chip *chip);
static int get_prop_charge_type(struct pm8921_chg_chip *chip);
static int get_prop_batt_status(struct pm8921_chg_chip *chip);

static int pm_chg_masked_write(struct pm8921_chg_chip *chip, u16 addr,
							u8 mask, u8 val)
{
	int rc;
	u8 reg;

	rc = pm8xxx_readb(chip->dev->parent, addr, &reg);
	if (rc) {
		pr_err("pm8xxx_readb failed: addr=%03X, rc=%d\n", addr, rc);
		return rc;
	}
	reg &= ~mask;
	reg |= val & mask;
	rc = pm8xxx_writeb(chip->dev->parent, addr, reg);
	if (rc) {
		pr_err("pm8xxx_writeb failed: addr=%03X, rc=%d\n", addr, rc);
		return rc;
	}
	return 0;
}

static int pm_chg_get_rt_status(struct pm8921_chg_chip *chip, int irq_id)
{
	return pm8xxx_read_irq_stat(chip->dev->parent,
					chip->pmic_chg_irq[irq_id]);
}

/* Treat OverVoltage/UnderVoltage as source missing */
static int is_usb_chg_plugged_in(struct pm8921_chg_chip *chip)
{
	return pm_chg_get_rt_status(chip, USBIN_VALID_IRQ);
}

/* Treat OverVoltage/UnderVoltage as source missing */
static int is_dc_chg_plugged_in(struct pm8921_chg_chip *chip)
{
	return pm_chg_get_rt_status(chip, DCIN_VALID_IRQ);
}

static int is_batfet_closed(struct pm8921_chg_chip *chip)
{
	return pm_chg_get_rt_status(chip, BATFET_IRQ);
}
#define CAPTURE_FSM_STATE_CMD	0xC2
#define READ_BANK_7		0x70
#define READ_BANK_4		0x40
static int pm_chg_get_fsm_state(struct pm8921_chg_chip *chip)
{
	u8 temp;
	int err, ret = 0;

	temp = CAPTURE_FSM_STATE_CMD;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return err;
	}

	temp = READ_BANK_7;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return err;
	}

	err = pm8xxx_readb(chip->dev->parent, CHG_TEST, &temp);
	if (err) {
		pr_err("pm8xxx_readb fail: addr=%03X, rc=%d\n", CHG_TEST, err);
		return err;
	}
	/* get the lower 4 bits */
	ret = temp & 0xF;

	temp = READ_BANK_4;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return err;
	}

	err = pm8xxx_readb(chip->dev->parent, CHG_TEST, &temp);
	if (err) {
		pr_err("pm8xxx_readb fail: addr=%03X, rc=%d\n", CHG_TEST, err);
		return err;
	}
	/* get the upper 1 bit */
	ret |= (temp & 0x1) << 4;
	return  ret;
}

#define READ_BANK_6		0x60
static int pm_chg_get_regulation_loop(struct pm8921_chg_chip *chip)
{
	u8 temp;
	int err;

	temp = READ_BANK_6;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return err;
	}

	err = pm8xxx_readb(chip->dev->parent, CHG_TEST, &temp);
	if (err) {
		pr_err("pm8xxx_readb fail: addr=%03X, rc=%d\n", CHG_TEST, err);
		return err;
	}

	/* return the lower 4 bits */
	return temp & CHG_ALL_LOOPS;
}

#define CHG_USB_SUSPEND_BIT  BIT(2)
static int pm_chg_usb_suspend_enable(struct pm8921_chg_chip *chip, int enable)
{
	return pm_chg_masked_write(chip, CHG_CNTRL_3, CHG_USB_SUSPEND_BIT,
			enable ? CHG_USB_SUSPEND_BIT : 0);
}

#define CHG_EN_BIT	BIT(7)
static int pm_chg_auto_enable(struct pm8921_chg_chip *chip, int enable)
{
	ktime_t current_time;
	struct timespec ts;

	current_time = alarm_get_elapsed_realtime();
	ts = ktime_to_timespec(current_time);

	if (enable) { /* Charging */
		if (chip->initial_count <= 0) {
			if (chip->charging_start_time == 0)
				chip->charging_start_time = ts.tv_sec;

			chip->charging_enabled = 1;
		}
	} else { /* Discahrging */
		chip->charging_enabled = 0;

		chip->charging_start_time = 0;
		chip->charging_passed_time = 0;
		chip->is_recharging = false;
	}
	chip->health_cnt = 0;
	chip->recharging_cnt = 0;

	return pm_chg_masked_write(chip, CHG_CNTRL_3, CHG_EN_BIT,
				enable ? CHG_EN_BIT : 0);
}

#define CHG_FAILED_CLEAR	BIT(0)
#define ATC_FAILED_CLEAR	BIT(1)
static int pm_chg_failed_clear(struct pm8921_chg_chip *chip, int clear)
{
	int rc;

	rc = pm_chg_masked_write(chip, CHG_CNTRL_3, ATC_FAILED_CLEAR,
				clear ? ATC_FAILED_CLEAR : 0);
	rc |= pm_chg_masked_write(chip, CHG_CNTRL_3, CHG_FAILED_CLEAR,
				clear ? CHG_FAILED_CLEAR : 0);
	return rc;
}

#define CHG_CHARGE_DIS_BIT	BIT(1)
static int pm_chg_charge_dis(struct pm8921_chg_chip *chip, int disable)
{
	return pm_chg_masked_write(chip, CHG_CNTRL, CHG_CHARGE_DIS_BIT,
				disable ? CHG_CHARGE_DIS_BIT : 0);
}

static int pm_is_chg_charge_dis(struct pm8921_chg_chip *chip)
{
	u8 temp;

	pm8xxx_readb(chip->dev->parent, CHG_CNTRL, &temp);
	return  temp & CHG_CHARGE_DIS_BIT;
}
#define PM8921_CHG_V_MIN_MV	3240
#define PM8921_CHG_V_STEP_MV	20
#define PM8921_CHG_V_STEP_10MV_OFFSET_BIT	BIT(7)
#define PM8921_CHG_VDDMAX_MAX	4500
#define PM8921_CHG_VDDMAX_MIN	3400
#define PM8921_CHG_V_MASK	0x7F
static int __pm_chg_vddmax_set(struct pm8921_chg_chip *chip, int voltage)
{
	int remainder;
	u8 temp = 0;

	if (voltage < PM8921_CHG_VDDMAX_MIN
			|| voltage > PM8921_CHG_VDDMAX_MAX) {
		pr_err("bad mV=%d asked to set\n", voltage);
		return -EINVAL;
	}

	temp = (voltage - PM8921_CHG_V_MIN_MV) / PM8921_CHG_V_STEP_MV;

	remainder = voltage % 20;
	if (remainder >= 10)
		temp |= PM8921_CHG_V_STEP_10MV_OFFSET_BIT;

	pr_debug("voltage=%d setting %02x\n", voltage, temp);
	return pm8xxx_writeb(chip->dev->parent, CHG_VDD_MAX, temp);
}

static int pm_chg_vddmax_get(struct pm8921_chg_chip *chip, int *voltage)
{
	u8 temp;
	int rc;

	rc = pm8xxx_readb(chip->dev->parent, CHG_VDD_MAX, &temp);
	if (rc) {
		pr_err("rc = %d while reading vdd max\n", rc);
		*voltage = 0;
		return rc;
	}
	*voltage = (int)(temp & PM8921_CHG_V_MASK) * PM8921_CHG_V_STEP_MV
							+ PM8921_CHG_V_MIN_MV;
	if (temp & PM8921_CHG_V_STEP_10MV_OFFSET_BIT)
		*voltage =  *voltage + 10;
	return 0;
}

static int pm_chg_vddmax_set(struct pm8921_chg_chip *chip, int voltage)
{
	int current_mv, ret, steps, i;
	bool increase;

	ret = 0;

	if (voltage < PM8921_CHG_VDDMAX_MIN
		|| voltage > PM8921_CHG_VDDMAX_MAX) {
		pr_err("bad mV=%d asked to set\n", voltage);
		return -EINVAL;
	}

	ret = pm_chg_vddmax_get(chip, &current_mv);
	if (ret) {
		pr_err("Failed to read vddmax rc=%d\n", ret);
		return -EINVAL;
	}
	if (current_mv == voltage)
		return 0;

	/* Only change in increments when USB is present */
	if (is_usb_chg_plugged_in(chip)) {
		if (current_mv < voltage) {
			steps = (voltage - current_mv) / PM8921_CHG_V_STEP_MV;
			increase = true;
		} else {
			steps = (current_mv - voltage) / PM8921_CHG_V_STEP_MV;
			increase = false;
		}

		pr_info("pm_chg_vddmax_set : steps(%d), increase(%d), voltage(%d), current_mv(%d)-----\n",
			steps, (int)increase, voltage, current_mv);

		for (i = 0; i < steps; i++) {
			if (increase)
				current_mv += PM8921_CHG_V_STEP_MV;
			else
				current_mv -= PM8921_CHG_V_STEP_MV;
			ret |= __pm_chg_vddmax_set(chip, current_mv);
		}
	}
	ret |= __pm_chg_vddmax_set(chip, voltage);
	return ret;
}

#define PM8921_CHG_VDDSAFE_MIN	3400
#define PM8921_CHG_VDDSAFE_MAX	4500
static int pm_chg_vddsafe_set(struct pm8921_chg_chip *chip, int voltage)
{
	u8 temp;

	if (voltage < PM8921_CHG_VDDSAFE_MIN
			|| voltage > PM8921_CHG_VDDSAFE_MAX) {
		pr_err("bad mV=%d asked to set\n", voltage);
		return -EINVAL;
	}
	temp = (voltage - PM8921_CHG_V_MIN_MV) / PM8921_CHG_V_STEP_MV;
	pr_debug("voltage=%d setting %02x\n", voltage, temp);
	return pm_chg_masked_write(chip, CHG_VDD_SAFE, PM8921_CHG_V_MASK, temp);
}

#define PM8921_CHG_VBATDET_MIN	3240
#define PM8921_CHG_VBATDET_MAX	5780
static int pm_chg_vbatdet_set(struct pm8921_chg_chip *chip, int voltage)
{
	u8 temp;

	if (voltage < PM8921_CHG_VBATDET_MIN
			|| voltage > PM8921_CHG_VBATDET_MAX) {
		pr_err("bad mV=%d asked to set\n", voltage);
		return -EINVAL;
	}
	temp = (voltage - PM8921_CHG_V_MIN_MV) / PM8921_CHG_V_STEP_MV;
	pr_debug("voltage=%d setting %02x\n", voltage, temp);
	return pm_chg_masked_write(chip, CHG_VBAT_DET, PM8921_CHG_V_MASK, temp);
}

#define PM8921_CHG_VINMIN_MIN_MV	3800
#define PM8921_CHG_VINMIN_STEP_MV	100
#define PM8921_CHG_VINMIN_USABLE_MAX	6500
#define PM8921_CHG_VINMIN_USABLE_MIN	4300
#define PM8921_CHG_VINMIN_MASK		0x1F
static int pm_chg_vinmin_set(struct pm8921_chg_chip *chip, int voltage)
{
	u8 temp;

	if (voltage < PM8921_CHG_VINMIN_USABLE_MIN
			|| voltage > PM8921_CHG_VINMIN_USABLE_MAX) {
		pr_err("bad mV=%d asked to set\n", voltage);
		return -EINVAL;
	}
	temp = (voltage - PM8921_CHG_VINMIN_MIN_MV) / PM8921_CHG_VINMIN_STEP_MV;
	pr_debug("voltage=%d setting %02x\n", voltage, temp);
	return pm_chg_masked_write(chip, CHG_VIN_MIN, PM8921_CHG_VINMIN_MASK,
									temp);
}

static int pm_chg_vinmin_get(struct pm8921_chg_chip *chip)
{
	u8 temp;
	int rc, voltage_mv;

	rc = pm8xxx_readb(chip->dev->parent, CHG_VIN_MIN, &temp);
	temp &= PM8921_CHG_VINMIN_MASK;

	voltage_mv = PM8921_CHG_VINMIN_MIN_MV +
			(int)temp * PM8921_CHG_VINMIN_STEP_MV;

	return voltage_mv;
}

#define PM8917_USB_UVD_MIN_MV	3850
#define PM8917_USB_UVD_MAX_MV	4350
#define PM8917_USB_UVD_STEP_MV	100
#define PM8917_USB_UVD_MASK	0x7
static int pm_chg_uvd_threshold_set(struct pm8921_chg_chip *chip, int thresh_mv)
{
	u8 temp;

	if (thresh_mv < PM8917_USB_UVD_MIN_MV
			|| thresh_mv > PM8917_USB_UVD_MAX_MV) {
		pr_err("bad mV=%d asked to set\n", thresh_mv);
		return -EINVAL;
	}
	temp = (thresh_mv - PM8917_USB_UVD_MIN_MV) / PM8917_USB_UVD_STEP_MV;
	return pm_chg_masked_write(chip, OVP_USB_UVD,
				PM8917_USB_UVD_MASK, temp);
}

#define PM8921_CHG_IBATMAX_MIN	325
#define PM8921_CHG_IBATMAX_MAX	2000
#define PM8921_CHG_I_MIN_MA	225
#define PM8921_CHG_I_STEP_MA	50
#define PM8921_CHG_I_MASK	0x3F
static int pm_chg_ibatmax_set(struct pm8921_chg_chip *chip, int chg_current)
{
	u8 temp;

	pr_info("%s : ibat is chagned (%d)---------\n", __func__, chg_current);
	if (chg_current < PM8921_CHG_IBATMAX_MIN
			|| chg_current > PM8921_CHG_IBATMAX_MAX) {
		pr_err("bad mA=%d asked to set\n", chg_current);
		return -EINVAL;
	}
	temp = (chg_current - PM8921_CHG_I_MIN_MA) / PM8921_CHG_I_STEP_MA;
	return pm_chg_masked_write(chip, CHG_IBAT_MAX, PM8921_CHG_I_MASK, temp);
}

#define PM8921_CHG_IBATSAFE_MIN	225
#define PM8921_CHG_IBATSAFE_MAX	3375
static int pm_chg_ibatsafe_set(struct pm8921_chg_chip *chip, int chg_current)
{
	u8 temp;

	if (chg_current < PM8921_CHG_IBATSAFE_MIN
			|| chg_current > PM8921_CHG_IBATSAFE_MAX) {
		pr_err("bad mA=%d asked to set\n", chg_current);
		return -EINVAL;
	}
	temp = (chg_current - PM8921_CHG_I_MIN_MA) / PM8921_CHG_I_STEP_MA;
	return pm_chg_masked_write(chip, CHG_IBAT_SAFE,
						PM8921_CHG_I_MASK, temp);
}

#define PM8921_CHG_ITERM_MIN_MA		50
#define PM8921_CHG_ITERM_MAX_MA		200
#define PM8921_CHG_ITERM_STEP_MA	10
#define PM8921_CHG_ITERM_MASK		0xF
static int pm_chg_iterm_set(struct pm8921_chg_chip *chip, int chg_current)
{
	u8 temp;

	if (chg_current < PM8921_CHG_ITERM_MIN_MA
			|| chg_current > PM8921_CHG_ITERM_MAX_MA) {
		pr_err("bad mA=%d asked to set\n", chg_current);
		return -EINVAL;
	}

	temp = (chg_current - PM8921_CHG_ITERM_MIN_MA)
				/ PM8921_CHG_ITERM_STEP_MA;
	return pm_chg_masked_write(chip, CHG_ITERM, PM8921_CHG_ITERM_MASK,
					 temp);
}

static int pm_chg_iterm_get(struct pm8921_chg_chip *chip, int *chg_current)
{
	u8 temp;
	int rc;

	rc = pm8xxx_readb(chip->dev->parent, CHG_ITERM, &temp);
	if (rc) {
		pr_err("err=%d reading CHG_ITEM\n", rc);
		*chg_current = 0;
		return rc;
	}
	temp &= PM8921_CHG_ITERM_MASK;
	*chg_current = (int)temp * PM8921_CHG_ITERM_STEP_MA
					+ PM8921_CHG_ITERM_MIN_MA;
	return 0;
}

struct usb_ma_limit_entry {
	int	usb_ma;
	u8	value;
};

/* USB Trim tables */
/* [110612] QC USB soft trim patch */
static int usb_trim_8038_table[USB_TRIM_ENTRIES] = {
	0x0,
	0x0,
	-0x9,
	0x0,
	-0xD,
	0x0,
	-0x10,
	-0x11,
	0x0,
	0x0,
	-0x25,
	0x0,
	-0x28,
	0x0,
	-0x32,
	0x0
};
/* [110612] QC USB soft trim patch */
static int usb_trim_8917_table[USB_TRIM_ENTRIES] = {
	0x0,
	0x0,
	0xA,
	0xC,
	0x10,
	0x10,
	0x13,
	0x14,
	0x13,
	0x3,
	0x1A,
	0x1D,
	0x1D,
	0x21,
	0x24,
	0x26
};

/* Maximum USB  setting table */
static struct usb_ma_limit_entry usb_ma_table[] = {
	{100, 0x0},
	{200, 0x1},
	{500, 0x2},
	{600, 0x3},
	{700, 0x4},
	{800, 0x5},
	{850, 0x6},
	{900, 0x8},
	{950, 0x7},
	{1000, 0x9},
	{1100, 0xA},
	{1200, 0xB},
	{1300, 0xC},
	{1400, 0xD},
	{1500, 0xE},
	{1600, 0xF},
};

#define REG_SBI_CONFIG			0x04F
#define PAGE3_ENABLE_MASK		0x6
#define USB_OVP_TRIM_MASK		0x3F
#define USB_OVP_TRIM_PM8917_MASK	0x7F
#define USB_OVP_TRIM_MIN		0x00
#define REG_USB_OVP_TRIM_ORIG_LSB	0x10A
#define REG_USB_OVP_TRIM_ORIG_MSB	0x09C
#define REG_USB_OVP_TRIM_PM8917		0x2B5
#define REG_USB_OVP_TRIM_PM8917_BIT	BIT(0)
static int pm_chg_usb_trim(struct pm8921_chg_chip *chip, int index)
{
	u8 temp, sbi_config, msb, lsb, mask;
	s8 trim;
	int rc = 0;
	static u8 usb_trim_reg_orig = 0xFF;

	/* No trim data for PM8921 */
	if (!chip->usb_trim_table)
		return 0;

	if (usb_trim_reg_orig == 0xFF) {
		rc = pm8xxx_readb(chip->dev->parent,
				REG_USB_OVP_TRIM_ORIG_MSB, &msb);
		if (rc) {
			pr_err("error = %d reading sbi config reg\n", rc);
			return rc;
		}

		rc = pm8xxx_readb(chip->dev->parent,
				REG_USB_OVP_TRIM_ORIG_LSB, &lsb);
		if (rc) {
			pr_err("error = %d reading sbi config reg\n", rc);
			return rc;
		}

		msb = msb >> 5;
		lsb = lsb >> 5;
		usb_trim_reg_orig = msb << 3 | lsb;

		if (pm8xxx_get_version(chip->dev->parent)
				== PM8XXX_VERSION_8917) {
			rc = pm8xxx_readb(chip->dev->parent,
					REG_USB_OVP_TRIM_PM8917, &msb);
			if (rc) {
				pr_err("error = %d reading config reg\n", rc);
				return rc;
			}

			msb = msb & REG_USB_OVP_TRIM_PM8917_BIT;
			usb_trim_reg_orig |= msb << 6;
		}
	}

	/* use the original trim value */
	trim = usb_trim_reg_orig;

	trim += chip->usb_trim_table[index];
	if (trim < 0)
		trim = 0;

	pr_err("trim_orig %d write 0x%x index=%d value 0x%x to USB_OVP_TRIM\n",
		usb_trim_reg_orig, trim, index, chip->usb_trim_table[index]);

	rc = pm8xxx_readb(chip->dev->parent, REG_SBI_CONFIG, &sbi_config);
	if (rc) {
		pr_err("error = %d reading sbi config reg\n", rc);
		return rc;
	}

	temp = sbi_config | PAGE3_ENABLE_MASK;
	rc = pm8xxx_writeb(chip->dev->parent, REG_SBI_CONFIG, temp);
	if (rc) {
		pr_err("error = %d writing sbi config reg\n", rc);
		return rc;
	}

	mask = USB_OVP_TRIM_MASK;

	if (pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8917)
		mask = USB_OVP_TRIM_PM8917_MASK;

	rc = pm_chg_masked_write(chip, USB_OVP_TRIM, mask, trim);
	if (rc) {
		pr_err("error = %d writing USB_OVP_TRIM\n", rc);
		return rc;
	}

	rc = pm8xxx_writeb(chip->dev->parent, REG_SBI_CONFIG, sbi_config);
	if (rc) {
		pr_err("error = %d writing sbi config reg\n", rc);
		return rc;
	}
	return rc;
}

#define PM8921_CHG_IUSB_MASK 0x1C
#define PM8921_CHG_IUSB_SHIFT 2
#define PM8921_CHG_IUSB_MAX  7
#define PM8921_CHG_IUSB_MIN  0
#define PM8917_IUSB_FINE_RES BIT(0)
static int pm_chg_iusbmax_set(struct pm8921_chg_chip *chip, int index)
{
	u8 temp, fineres, reg_val;
	int rc;

	reg_val = usb_ma_table[index].value >> 1;
	fineres = PM8917_IUSB_FINE_RES & usb_ma_table[index].value;

	if (reg_val < PM8921_CHG_IUSB_MIN || reg_val > PM8921_CHG_IUSB_MAX) {
		pr_err("bad mA=%d asked to set\n", reg_val);
		return -EINVAL;
	}
	temp = reg_val << PM8921_CHG_IUSB_SHIFT;

	/* IUSB_FINE_RES */
	if (chip->iusb_fine_res) {
		/* Clear IUSB_FINE_RES bit to avoid overshoot */
		rc = pm_chg_masked_write(chip, IUSB_FINE_RES,
			PM8917_IUSB_FINE_RES, 0);

		rc |= pm_chg_masked_write(chip, PBL_ACCESS2,
			PM8921_CHG_IUSB_MASK, temp);

		if (rc) {
			pr_err("Failed to write PBL_ACCESS2 rc=%d\n", rc);
			return rc;
		}

		if (fineres) {
			rc = pm_chg_masked_write(chip, IUSB_FINE_RES,
				PM8917_IUSB_FINE_RES, fineres);
			if (rc) {
				pr_err("Failed to write ISUB_FINE_RES rc=%d\n",
					rc);
				return rc;
			}
		}
	} else {
		rc = pm_chg_masked_write(chip, PBL_ACCESS2,
			PM8921_CHG_IUSB_MASK, temp);
		if (rc) {
			pr_err("Failed to write PBL_ACCESS2 rc=%d\n", rc);
			return rc;
		}
	}

	rc = pm_chg_usb_trim(chip, index);
	if (rc)
			pr_err("unable to set usb trim rc = %d\n", rc);

	return rc;
}

static int pm_chg_iusbmax_get(struct pm8921_chg_chip *chip, int *mA)
{
	u8 temp, fineres;
	int rc, i;

	fineres = 0;
	*mA = 0;
	rc = pm8xxx_readb(chip->dev->parent, PBL_ACCESS2, &temp);
	if (rc) {
		pr_err("err=%d reading PBL_ACCESS2\n", rc);
		return rc;
	}

	if (chip->iusb_fine_res) {
		rc = pm8xxx_readb(chip->dev->parent, IUSB_FINE_RES, &fineres);
		if (rc) {
			pr_err("err=%d reading IUSB_FINE_RES\n", rc);
			return rc;
		}
	}
	temp &= PM8921_CHG_IUSB_MASK;
	temp = temp >> PM8921_CHG_IUSB_SHIFT;

	temp = (temp << 1) | (fineres & PM8917_IUSB_FINE_RES);
	for (i = ARRAY_SIZE(usb_ma_table) - 1; i >= 0; i--) {
		if (usb_ma_table[i].value == temp)
			break;
	}

	*mA = usb_ma_table[i].usb_ma;

	return rc;
}

#define PM8921_CHG_WD_MASK 0x1F
static int pm_chg_disable_wd(struct pm8921_chg_chip *chip)
{
	/* writing 0 to the wd timer disables it */
	return pm_chg_masked_write(chip, CHG_TWDOG, PM8921_CHG_WD_MASK, 0);
}

#define PM8921_CHG_TCHG_MASK	0x7F
#define PM8921_CHG_TCHG_MIN	4
#define PM8921_CHG_TCHG_MAX	512
#define PM8921_CHG_TCHG_STEP	4
static int pm_chg_tchg_max_set(struct pm8921_chg_chip *chip, int minutes)
{
	u8 temp;

	if (minutes < PM8921_CHG_TCHG_MIN || minutes > PM8921_CHG_TCHG_MAX) {
		pr_err("bad max minutes =%d asked to set\n", minutes);
		return -EINVAL;
	}

	temp = (minutes - 1)/PM8921_CHG_TCHG_STEP;
	return pm_chg_masked_write(chip, CHG_TCHG_MAX, PM8921_CHG_TCHG_MASK,
					 temp);
}

#define PM8921_CHG_TTRKL_MASK	0x3F
#define PM8921_CHG_TTRKL_MIN	1
#define PM8921_CHG_TTRKL_MAX	64
static int pm_chg_ttrkl_max_set(struct pm8921_chg_chip *chip, int minutes)
{
	u8 temp;

	if (minutes < PM8921_CHG_TTRKL_MIN || minutes > PM8921_CHG_TTRKL_MAX) {
		pr_err("bad max minutes =%d asked to set\n", minutes);
		return -EINVAL;
	}

	temp = minutes - 1;
	return pm_chg_masked_write(chip, CHG_TTRKL_MAX, PM8921_CHG_TTRKL_MASK,
					 temp);
}

#define PM8921_CHG_VTRKL_MIN_MV		2050
#define PM8921_CHG_VTRKL_MAX_MV		2800
#define PM8921_CHG_VTRKL_STEP_MV	50
#define PM8921_CHG_VTRKL_SHIFT		4
#define PM8921_CHG_VTRKL_MASK		0xF0
static int pm_chg_vtrkl_low_set(struct pm8921_chg_chip *chip, int millivolts)
{
	u8 temp;

	if (millivolts < PM8921_CHG_VTRKL_MIN_MV
			|| millivolts > PM8921_CHG_VTRKL_MAX_MV) {
		pr_err("bad voltage = %dmV asked to set\n", millivolts);
		return -EINVAL;
	}

	temp = (millivolts - PM8921_CHG_VTRKL_MIN_MV)/PM8921_CHG_VTRKL_STEP_MV;
	temp = temp << PM8921_CHG_VTRKL_SHIFT;
	return pm_chg_masked_write(chip, CHG_VTRICKLE, PM8921_CHG_VTRKL_MASK,
					 temp);
}

#define PM8921_CHG_VWEAK_MIN_MV		2100
#define PM8921_CHG_VWEAK_MAX_MV		3600
#define PM8921_CHG_VWEAK_STEP_MV	100
#define PM8921_CHG_VWEAK_MASK		0x0F
static int pm_chg_vweak_set(struct pm8921_chg_chip *chip, int millivolts)
{
	u8 temp;

	if (millivolts < PM8921_CHG_VWEAK_MIN_MV
			|| millivolts > PM8921_CHG_VWEAK_MAX_MV) {
		pr_err("bad voltage = %dmV asked to set\n", millivolts);
		return -EINVAL;
	}

	temp = (millivolts - PM8921_CHG_VWEAK_MIN_MV)/PM8921_CHG_VWEAK_STEP_MV;
	return pm_chg_masked_write(chip, CHG_VTRICKLE, PM8921_CHG_VWEAK_MASK,
					 temp);
}

#define PM8921_CHG_ITRKL_MIN_MA		50
#define PM8921_CHG_ITRKL_MAX_MA		200
#define PM8921_CHG_ITRKL_MASK		0x0F
#define PM8921_CHG_ITRKL_STEP_MA	10
static int pm_chg_itrkl_set(struct pm8921_chg_chip *chip, int milliamps)
{
	u8 temp;

	if (milliamps < PM8921_CHG_ITRKL_MIN_MA
		|| milliamps > PM8921_CHG_ITRKL_MAX_MA) {
		pr_err("bad current = %dmA asked to set\n", milliamps);
		return -EINVAL;
	}

	temp = (milliamps - PM8921_CHG_ITRKL_MIN_MA)/PM8921_CHG_ITRKL_STEP_MA;

	return pm_chg_masked_write(chip, CHG_ITRICKLE, PM8921_CHG_ITRKL_MASK,
					 temp);
}

#define PM8921_CHG_IWEAK_MIN_MA		325
#define PM8921_CHG_IWEAK_MAX_MA		525
#define PM8921_CHG_IWEAK_SHIFT		7
#define PM8921_CHG_IWEAK_MASK		0x80
static int pm_chg_iweak_set(struct pm8921_chg_chip *chip, int milliamps)
{
	u8 temp;

	if (milliamps < PM8921_CHG_IWEAK_MIN_MA
		|| milliamps > PM8921_CHG_IWEAK_MAX_MA) {
		pr_err("bad current = %dmA asked to set\n", milliamps);
		return -EINVAL;
	}

	if (milliamps < PM8921_CHG_IWEAK_MAX_MA)
		temp = 0;
	else
		temp = 1;

	temp = temp << PM8921_CHG_IWEAK_SHIFT;
	return pm_chg_masked_write(chip, CHG_ITRICKLE, PM8921_CHG_IWEAK_MASK,
					 temp);
}

#define PM8921_CHG_BATT_TEMP_THR_COLD	BIT(1)
#define PM8921_CHG_BATT_TEMP_THR_COLD_SHIFT	1
static int pm_chg_batt_cold_temp_config(struct pm8921_chg_chip *chip,
					enum pm8921_chg_cold_thr cold_thr)
{
	u8 temp;

	temp = cold_thr << PM8921_CHG_BATT_TEMP_THR_COLD_SHIFT;
	temp = temp & PM8921_CHG_BATT_TEMP_THR_COLD;
	return pm_chg_masked_write(chip, CHG_CNTRL_2,
					PM8921_CHG_BATT_TEMP_THR_COLD,
					 temp);
}

#define PM8921_CHG_BATT_TEMP_THR_HOT		BIT(0)
#define PM8921_CHG_BATT_TEMP_THR_HOT_SHIFT	0
static int pm_chg_batt_hot_temp_config(struct pm8921_chg_chip *chip,
					enum pm8921_chg_hot_thr hot_thr)
{
	u8 temp;

	temp = hot_thr << PM8921_CHG_BATT_TEMP_THR_HOT_SHIFT;
	temp = temp & PM8921_CHG_BATT_TEMP_THR_HOT;
	return pm_chg_masked_write(chip, CHG_CNTRL_2,
					PM8921_CHG_BATT_TEMP_THR_HOT,
					 temp);
}

#define PM8921_CHG_LED_SRC_CONFIG_SHIFT	4
#define PM8921_CHG_LED_SRC_CONFIG_MASK	0x30
static int pm_chg_led_src_config(struct pm8921_chg_chip *chip,
				enum pm8921_chg_led_src_config led_src_config)
{
	u8 temp;

	if (led_src_config < LED_SRC_GND ||
			led_src_config > LED_SRC_BYPASS)
		return -EINVAL;

	if (led_src_config == LED_SRC_BYPASS)
		return 0;

	temp = led_src_config << PM8921_CHG_LED_SRC_CONFIG_SHIFT;

	return pm_chg_masked_write(chip, CHG_CNTRL_3,
					PM8921_CHG_LED_SRC_CONFIG_MASK, temp);
}

static int64_t read_battery_id(struct pm8921_chg_chip *chip)
{
	int rc;
	struct pm8xxx_adc_chan_result result;

	rc = pm8xxx_adc_read(chip->batt_id_channel, &result);
	if (rc) {
		pr_err("error reading batt id channel = %d, rc = %d\n",
					chip->vbat_channel, rc);
		return rc;
	}
	pr_debug("batt_id phy = %lld meas = 0x%llx\n", result.physical,
						result.measurement);
	return result.physical;
}

static int is_battery_valid(struct pm8921_chg_chip *chip)
{
	int64_t rc;

	if (chip->batt_id_min == 0 && chip->batt_id_max == 0)
		return 1;

	rc = read_battery_id(chip);
	if (rc < 0) {
		pr_err("error reading batt id channel = %d, rc = %lld\n",
					chip->vbat_channel, rc);
		/* assume battery id is valid when adc error happens */
		return 1;
	}

	if (rc < chip->batt_id_min || rc > chip->batt_id_max) {
		pr_err("batt_id phy =%lld is not valid\n", rc);
		return 0;
	}
	return 1;
}

static void check_battery_valid(struct pm8921_chg_chip *chip)
{
	if (is_battery_valid(chip) == 0) {
		pr_err("batt_id not valid, disbling charging\n");
		pm_chg_auto_enable(chip, 0);
	} else {
		pm_chg_auto_enable(chip, !charging_disabled);
	}
}

static void battery_id_valid(struct work_struct *work)
{
	struct pm8921_chg_chip *chip = container_of(work,
				struct pm8921_chg_chip, battery_id_valid_work);

	check_battery_valid(chip);
}

static void pm8921_chg_enable_irq(struct pm8921_chg_chip *chip, int interrupt)
{
	if (!__test_and_set_bit(interrupt, chip->enabled_irqs)) {
		dev_dbg(chip->dev, "%d\n", chip->pmic_chg_irq[interrupt]);
		enable_irq(chip->pmic_chg_irq[interrupt]);
	}
}

static void pm8921_chg_disable_irq(struct pm8921_chg_chip *chip, int interrupt)
{
	if (__test_and_clear_bit(interrupt, chip->enabled_irqs)) {
		dev_dbg(chip->dev, "%d\n", chip->pmic_chg_irq[interrupt]);
		disable_irq_nosync(chip->pmic_chg_irq[interrupt]);
	}
}

static int pm8921_chg_is_enabled(struct pm8921_chg_chip *chip, int interrupt)
{
	return test_bit(interrupt, chip->enabled_irqs);
}

static bool is_ext_charging(struct pm8921_chg_chip *chip)
{
	union power_supply_propval ret = {0,};

	if (!chip->ext_psy)
		return false;
	if (chip->ext_psy->get_property(chip->ext_psy,
					POWER_SUPPLY_PROP_CHARGE_TYPE, &ret))
		return false;
	if (ret.intval > POWER_SUPPLY_CHARGE_TYPE_NONE)
		return ret.intval;

	return false;
}

static bool is_ext_trickle_charging(struct pm8921_chg_chip *chip)
{
	union power_supply_propval ret = {0,};

	if (!chip->ext_psy)
		return false;
	if (chip->ext_psy->get_property(chip->ext_psy,
					POWER_SUPPLY_PROP_CHARGE_TYPE, &ret))
		return false;
	if (ret.intval == POWER_SUPPLY_CHARGE_TYPE_TRICKLE)
		return true;

	return false;
}

static int is_battery_charging(int fsm_state)
{
	if (is_ext_charging(the_chip))
		return 1;

	switch (fsm_state) {
	case FSM_STATE_ATC_2A:
	case FSM_STATE_ATC_2B:
	case FSM_STATE_ON_CHG_AND_BAT_6:
	case FSM_STATE_FAST_CHG_7:
	case FSM_STATE_TRKL_CHG_8:
		return 1;
	}
	return 0;
}

static void bms_notify(struct work_struct *work)
{
	struct bms_notify *n = container_of(work, struct bms_notify, work);

	if (n->is_charging) {
		pm8921_bms_charging_began();
	} else {
		pm8921_bms_charging_end(n->is_battery_full);
		n->is_battery_full = 0;
	}
}

static void bms_notify_check(struct pm8921_chg_chip *chip)
{
	int fsm_state, new_is_charging;

	fsm_state = pm_chg_get_fsm_state(chip);
	new_is_charging = is_battery_charging(fsm_state);

	if (chip->bms_notify.is_charging ^ new_is_charging) {
		chip->bms_notify.is_charging = new_is_charging;
		schedule_work(&(chip->bms_notify.work));
	}
}

static enum power_supply_property pm_power_props_usb[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_SCOPE,
	POWER_SUPPLY_PROP_HEALTH,
};

static enum power_supply_property pm_power_props_mains[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
};

static char *pm_power_supplied_to[] = {
	"battery",
};

#define USB_WALL_THRESHOLD_MA	500
static int pm_power_get_property_mains(struct power_supply *psy,
				  enum power_supply_property psp,
				  union power_supply_propval *val)
{
	/* Check if called before init */
	if (!the_chip)
		return -EINVAL;

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
#if 0
		if (the_chip->has_dc_supply) {
			val->intval = 1;
			return 0;
		}

		if (charging_disabled)
			return 0;

		/* check external charger first before the dc path */
		if (is_ext_charging(the_chip)) {
			val->intval = 1;
			return 0;
		}

		if (pm_is_chg_charge_dis(the_chip)) {
			val->intval = 0;
			return 0;
		}

		if (the_chip->dc_present) {
			val->intval = 1;
			return 0;
		}
#endif

	/* Set enable=1 only if the AC charger is connected */
	if (the_chip->batt_status == POWER_SUPPLY_STATUS_DISCHARGING)
		val->intval = 0;
	else {
		if (the_chip->cable_type == CABLE_TYPE_MISC ||
		    the_chip->cable_type == CABLE_TYPE_UARTOFF ||
		    the_chip->cable_type == CABLE_TYPE_DESK_DOCK ||
		    the_chip->cable_type == CABLE_TYPE_CARDOCK) {
			if (is_usb_chg_plugged_in(the_chip))
				val->intval = 1;
			else
				val->intval = 0;
		} else {
			/* org */
			if (the_chip->cable_type == CABLE_TYPE_AC)
				val->intval = 1;
#ifdef CONFIG_WIRELESS_CHARGING
			if (the_chip->cable_type == CABLE_TYPE_WPC)
				val->intval = 1;
#endif
		}
	}
		/* USB with max current greater than 500 mA connected */
		if (usb_target_ma > USB_WALL_THRESHOLD_MA)
			val->intval = is_usb_chg_plugged_in(the_chip);
			return 0;

		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = get_prop_charge_type(the_chip);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int disable_aicl(int disable)
{
	if (disable != POWER_SUPPLY_HEALTH_UNKNOWN
		&& disable != POWER_SUPPLY_HEALTH_GOOD) {
		pr_err("called with invalid param :%d\n", disable);
		return -EINVAL;
	}

	if (!the_chip) {
		pr_err("%s called before init\n", __func__);
		return -EINVAL;
	}

	pr_debug("Disable AICL = %d\n", disable);
	the_chip->disable_aicl = disable;
	return 0;
}

static int switch_usb_to_charge_mode(struct pm8921_chg_chip *chip)
{
	int rc;

	if (!chip->host_mode)
		return 0;

	/* enable usbin valid comparator and remove force usb ovp fet off */
	rc = pm8xxx_writeb(chip->dev->parent, USB_OVP_TEST, 0xB2);
	if (rc < 0) {
		pr_err("Failed to write 0xB2 to USB_OVP_TEST rc = %d\n", rc);
		return rc;
	}

	chip->host_mode = 0;

	return 0;
}

static int switch_usb_to_host_mode(struct pm8921_chg_chip *chip)
{
	int rc;

	if (chip->host_mode)
		return 0;

	/* disable usbin valid comparator and force usb ovp fet off */
	rc = pm8xxx_writeb(chip->dev->parent, USB_OVP_TEST, 0xB3);
	if (rc < 0) {
		pr_err("Failed to write 0xB3 to USB_OVP_TEST rc = %d\n", rc);
		return rc;
	}

	chip->host_mode = 1;

	return 0;
}

static int pm_power_set_property_usb(struct power_supply *psy,
				  enum power_supply_property psp,
				  const union power_supply_propval *val)
{
	/* Check if called before init */
	if (!the_chip)
		return -EINVAL;

	switch (psp) {
	case POWER_SUPPLY_PROP_SCOPE:
		if (val->intval == POWER_SUPPLY_SCOPE_SYSTEM)
			return switch_usb_to_host_mode(the_chip);
		if (val->intval == POWER_SUPPLY_SCOPE_DEVICE)
			return switch_usb_to_charge_mode(the_chip);
		else
			return -EINVAL;
		break;
	case POWER_SUPPLY_PROP_TYPE:
		return pm8921_set_usb_power_supply_type(val->intval);
	case POWER_SUPPLY_PROP_HEALTH:
		/* UNKNOWN(0) means enable aicl, GOOD(1) means disable aicl */
		return disable_aicl(val->intval);
	default:
		return -EINVAL;
	}
	return 0;
}

static int usb_property_is_writeable(struct power_supply *psy,
						enum power_supply_property psp)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_HEALTH:
		return 1;
	default:
		break;
	}

	return 0;
}

static int pm_power_get_property_usb(struct power_supply *psy,
				  enum power_supply_property psp,
				  union power_supply_propval *val)
{
	int current_max;

	/* Check if called before init */
	if (!the_chip)
		return -EINVAL;

	switch (psp) {
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		if (pm_is_chg_charge_dis(the_chip)) {
			val->intval = 0;
		} else {
			pm_chg_iusbmax_get(the_chip, &current_max);
			val->intval = current_max;
		}
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = get_prop_charge_type(the_chip);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
	case POWER_SUPPLY_PROP_ONLINE:
#if 0
		val->intval = 0;
		if (charging_disabled)
			return 0;

		/*
		 * if drawing any current from usb is disabled behave
		 * as if no usb cable is connected
		 */
		if (pm_is_chg_charge_dis(the_chip))
			return 0;
#endif
		/* Set enable=1 only if the USB charger is connected */
		switch (the_chip->cable_type) {
		case CABLE_TYPE_USB:
		case CABLE_TYPE_CDP:
			val->intval = 1;
			break;
		default:
			val->intval = 0;
			break;
		}

		/* USB charging */
		if (usb_target_ma < USB_WALL_THRESHOLD_MA)
			val->intval = is_usb_chg_plugged_in(the_chip);
		else
		    return 0;
		break;

	case POWER_SUPPLY_PROP_SCOPE:
		if (the_chip->host_mode)
			val->intval = POWER_SUPPLY_SCOPE_SYSTEM;
		else
			val->intval = POWER_SUPPLY_SCOPE_DEVICE;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static enum power_supply_property msm_batt_power_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_ENERGY_FULL,
	POWER_SUPPLY_PROP_CHARGE_NOW,
};
static enum power_supply_property msm_fg_power_props[] = {
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_CURRENT_NOW,
};

static int get_prop_battery_uvolts(struct pm8921_chg_chip *chip)
{
	int rc;
	struct pm8xxx_adc_chan_result result;

	rc = pm8xxx_adc_read(chip->vbat_channel, &result);
	if (rc) {
		pr_err("error reading adc channel = %d, rc = %d\n",
					chip->vbat_channel, rc);
		return rc;
	}
	pr_debug("mvolts phy = %lld meas = 0x%llx\n", result.physical,
						result.measurement);
	return (int)result.physical;
}

static unsigned int voltage_based_capacity(struct pm8921_chg_chip *chip)
{
	unsigned int current_voltage_uv = get_prop_battery_uvolts(chip);
	unsigned int current_voltage_mv = current_voltage_uv / 1000;
	unsigned int low_voltage = chip->min_voltage_mv;
	unsigned int high_voltage = chip->max_voltage_mv;
	static unsigned int capa_cnt;
	static unsigned int capacity_sum;
	int capa_max = 0;
	int capa_min = 100;
	int i;

	if (capa_cnt == 20)
		capa_cnt = 10;

	pr_debug("low_voltage = %d, high_voltage = %d, current_voltage_mv = %d\n",
		low_voltage, high_voltage, current_voltage_mv);

	if (current_voltage_mv <= low_voltage)
		return 0;
	else if (current_voltage_mv >= high_voltage)
		return 100;
	else {
		if (capa_cnt < 10) {
			chip->capacity_value[capa_cnt]
				= (current_voltage_mv - low_voltage) * 100
				/ (high_voltage - low_voltage);
			capacity_sum += chip->capacity_value[capa_cnt];
			capa_cnt++;
			return capacity_sum / capa_cnt;
		} else {
			for (i = 0; i < 10; i++) {
				capa_max = (chip->capacity_value[i] > capa_max)
					? chip->capacity_value[i] : capa_max;
				capa_min = (chip->capacity_value[i] < capa_min)
					? chip->capacity_value[i] : capa_min;
			}
			capacity_sum -= chip->capacity_value[capa_cnt - 10];
			chip->capacity_value[capa_cnt - 10] =
				(current_voltage_mv - low_voltage) * 100
				/ (high_voltage - low_voltage);
			pr_info("voltage_based_input_SOC(%d)\n",
					chip->capacity_value[capa_cnt - 10]);
			capacity_sum += chip->capacity_value[capa_cnt - 10];
			capa_cnt++;
			return (capacity_sum - capa_max - capa_min) / 8;
		}
	}
}

static int get_prop_batt_present(struct pm8921_chg_chip *chip)
{
	int repeat_cnt;

	repeat_cnt = 0;
	do {
		if (!check_jig_state() && repeat_cnt)
			mdelay(20);
#if defined(CONFIG_MACH_EXPRESS)
		if (chip->hw_rev < 4)
			chip->batt_present =
				pm_chg_get_rt_status(chip, BATT_INSERTED_IRQ);
		else
			chip->batt_present = !gpio_get_value(GPIO_BATT_INT);
#else
		chip->batt_present = !gpio_get_value(GPIO_BATT_INT);
#endif
	} while (!chip->batt_present && repeat_cnt++ < 5);

	return chip->batt_present;
}

static int param_force_voltage_based_soc;
module_param(param_force_voltage_based_soc, int, 0644);

/* capacity is  0.1% unit */
static int sec_fg_get_scaled_capacity(
			struct pm8921_chg_chip *chip, int raw_soc)
{
	int scaled_soc;

	scaled_soc = (raw_soc < chip->batt_pdata->capacity_min) ?
		0 : ((raw_soc - chip->batt_pdata->capacity_min) * 1000 /
		(chip->capacity_max - chip->batt_pdata->capacity_min));

	pr_debug("%s: scaled capacity (%d.%d)\n",
		__func__, scaled_soc/10, scaled_soc%10);

	return scaled_soc;
}

/* capacity is  0.1% unit */
static int sec_fg_calculate_dynamic_scale(
			struct pm8921_chg_chip *chip, int raw_soc)
{
	raw_soc += 10;
	if (raw_soc <
		chip->batt_pdata->capacity_max -
		chip->batt_pdata->capacity_max_margin)
		chip->capacity_max =
			chip->batt_pdata->capacity_max -
			chip->batt_pdata->capacity_max_margin;
	else
		chip->capacity_max =
			(raw_soc > 1000) ? 1000 : raw_soc;

	/* fixed bug in scale (100.0 -> 98.9 -> 97.8.. , no 99%) */
	/* (99 / 100) --> (991 / 1000) */
	chip->capacity_max =
		((chip->capacity_max - chip->batt_pdata->capacity_min)
		* 991 / 1000) + chip->batt_pdata->capacity_min;

	/* updated old capacity as full (integer) */
	chip->capacity_old = 100;

	pr_info("%s: %d is used for capacity_max\n",
		__func__, chip->capacity_max);

	return chip->capacity_max;
}

/* capacity is integer */
/*
static int sec_fg_get_atomic_capacity(struct pm8921_chg_chip *chip, int soc)
{
	int return_soc;

	return_soc = soc;
*/
	/* temporarily block atomic function for BMS tunning */
	/*
	if (chip->capacity_old < soc)
		return_soc = chip->capacity_old + 1;
	else if (chip->capacity_old > soc)
		return_soc = chip->capacity_old - 1;
	*/

	/* keep SOC stable in abnormal status */
/*	if ((chip->batt_status == POWER_SUPPLY_STATUS_CHARGING &&
		chip->capacity_old > soc) ||
		((chip->batt_status == POWER_SUPPLY_STATUS_DISCHARGING ||
		chip->batt_status == POWER_SUPPLY_STATUS_NOT_CHARGING) &&
		chip->capacity_old < soc))
		return_soc = chip->capacity_old;
*/
	/* updated old capacity */
/*	chip->capacity_old = return_soc;

	pr_debug("%s: capacity_old (%d)\n",
		__func__, chip->capacity_old);

	return return_soc;
}
*/
static int get_prop_batt_capacity(struct pm8921_chg_chip *chip)
{
	int percent_soc;

	/*if (param_force_voltage_based_soc) {
		percent_soc = voltage_based_capacity(chip);
		goto out;
	}*/

	if (check_jig_state() && !chip->batt_present) {
		percent_soc = voltage_based_capacity(chip);
		pr_info("voltage_based_capacity SOC(%d) : JIG\n",
			percent_soc);
	} else {
		percent_soc = pm8921_bms_get_percent_charge();
		pr_debug("pm8921_bms_get_percent_charge SOC(%d)\n",
			percent_soc);
	}

	if (percent_soc == -ENXIO)
		percent_soc = voltage_based_capacity(chip);


	if (percent_soc <= 10)
		pr_warn_ratelimited("low battery charge = %d%%\n",
						percent_soc);

	if (chip->recent_reported_soc == (chip->resume_charge_percent + 1)
			&& percent_soc == chip->resume_charge_percent) {
		pr_debug("soc fell below %d. charging enabled.\n",
						chip->resume_charge_percent);
		if (chip->is_bat_warm)
			pr_warn_ratelimited("battery is warm = %d, do not resume charging at %d%%.\n",
					chip->is_bat_warm,
					chip->resume_charge_percent);
		else if (chip->is_bat_cool)
			pr_warn_ratelimited("battery is cool = %d, do not resume charging at %d%%.\n",
					chip->is_bat_cool,
					chip->resume_charge_percent);
		/* disable VBATDET for improper FSB state as 1(On high) */
		/*else
			pm_chg_vbatdet_set(the_chip, PM8921_CHG_VBATDET_MAX);*/
	}

	chip->capacity_raw = percent_soc;

	percent_soc = sec_fg_get_scaled_capacity(chip,
		percent_soc * 10) / 10;

	/* (Only for atomic capacity)
	 * In initial time, capacity_old is 0.
	 * and in resume from sleep,
	 * capacity_old is too different from actual soc.
	 * should update capacity_old
	 * by val->intval in booting or resume.
	 */
	/*
	if (chip->initial_update_of_soc) {
		//updated old capacity
		chip->capacity_old = percent_soc;
		chip->initial_update_of_soc = false;

		pr_info("%s: initialize old capacity (%d)\n",
			__func__, chip->capacity_old);
	}

	percent_soc = sec_fg_get_atomic_capacity(chip, percent_soc);
	*/

	if (percent_soc < 0)
		percent_soc = 0;
	else if (percent_soc > 100)
		percent_soc = 100;

	chip->recent_reported_soc = percent_soc;

	if (percent_soc <= 10)
		pr_warn("low battery charge = %d%%\n", percent_soc);

	pr_debug("%s: raw soc (%d), adj_soc (%d)\n",
		__func__, chip->capacity_raw, percent_soc);
	return percent_soc;
}
/* unused function 
static int get_prop_batt_current_max(struct pm8921_chg_chip *chip)
{
	return pm8921_bms_get_current_max();
}
*/
static int get_prop_batt_current(struct pm8921_chg_chip *chip)
{
	int result_ua, rc;

	rc = pm8921_bms_get_battery_current(&result_ua);
	if (rc == -ENXIO)
		rc = pm8xxx_ccadc_get_battery_current(&result_ua);

	if (rc) {
		pr_err("unable to get batt current rc = %d\n", rc);
		return rc;
	} else {
		return result_ua;
	}
}

static int get_prop_batt_fcc(struct pm8921_chg_chip *chip)
{
	int rc;

	rc = pm8921_bms_get_fcc();
	if (rc < 0)
		pr_err("unable to get batt fcc rc = %d\n", rc);
	return rc;
}

static int get_prop_batt_charge_now(struct pm8921_chg_chip *chip)
{
	int rc;
	int cc_uah;

	rc = pm8921_bms_cc_uah(&cc_uah);

	if (rc == 0)
		return cc_uah;

	pr_err("unable to get batt fcc rc = %d\n", rc);
	return rc;
}

static int get_prop_batt_health(struct pm8921_chg_chip *chip)
{
	int temp;
	int highblock, highrecover, lowblock, lowrecover;

	/* battery check */
	if (get_prop_batt_present(chip)) {
		if (chip->batt_health == POWER_SUPPLY_HEALTH_UNSPEC_FAILURE)
			chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;
		pr_debug("%s : battery is present\n", __func__);
	} else {
		chip->batt_health = POWER_SUPPLY_HEALTH_UNSPEC_FAILURE;
		pr_info("%s : battery is absent\n", __func__);
		goto return_health;
	}

	if (ovp_state && !is_usb_chg_plugged_in(chip)) {
		chip->batt_health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
		chip->health_cnt = 0;
		pr_info("%s : battery is overvoltage\n", __func__);
		goto return_health;
	} else {
		if (chip->batt_health == POWER_SUPPLY_HEALTH_OVERVOLTAGE) {
			chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;
			pr_info("%s : battery OVP is recovered\n", __func__);
		}
	}

	temp = get_prop_batt_temp(chip);

	if (chip->batt_pdata->event_check && chip->event) {
		highblock = chip->batt_pdata->temp_high_block_event;
		highrecover = chip->batt_pdata->temp_high_recover_event;
		lowblock = chip->batt_pdata->temp_low_block_event;
		lowrecover = chip->batt_pdata->temp_low_recover_event;
#if defined(CONFIG_WIRELESS_CHARGING)
	} else if (chip->cable_type == CABLE_TYPE_WPC) {
		highblock = chip->batt_pdata->temp_high_block_wc;
		highrecover = chip->batt_pdata->temp_high_recover_wc;
		lowblock = chip->batt_pdata->temp_low_block_wc;
		lowrecover = chip->batt_pdata->temp_low_recover_wc;
#endif
	} else if (chip->get_lpm_mode()) {
		highblock = chip->batt_pdata->temp_high_block_lpm;
		highrecover = chip->batt_pdata->temp_high_recover_lpm;
		lowblock = chip->batt_pdata->temp_low_block_lpm;
		lowrecover = chip->batt_pdata->temp_low_recover_lpm;
	} else {
		highblock = chip->batt_pdata->temp_high_block_normal;
		highrecover = chip->batt_pdata->temp_high_recover_normal;
		lowblock = chip->batt_pdata->temp_low_block_normal;
		lowrecover = chip->batt_pdata->temp_low_recover_normal;
	}

	pr_debug("%s : temperature limit HB(%d),HR(%d),LB(%d),LR(%d)\n",
		__func__, highblock, highrecover, lowblock, lowrecover);
	if (chip->batt_health == POWER_SUPPLY_HEALTH_GOOD) {
		if (temp >= highblock) {
			if (chip->health_cnt >= 0) {
				chip->batt_health =
					POWER_SUPPLY_HEALTH_OVERHEAT;
				pr_info("%s : battery is overheat\n", __func__);
			}
			chip->health_cnt++;
			pr_info("%s : overheat_cnt(%d)\n",
				__func__, chip->health_cnt);
		} else if (temp <= lowblock) {
			if (chip->health_cnt >= 0) {
				chip->batt_health =
					POWER_SUPPLY_HEALTH_COLD;
				pr_info("%s : battery is cold\n", __func__);
			}
			chip->health_cnt++;
			pr_info("%s : cold_cnt(%d)\n",
				__func__, chip->health_cnt);
		}
	} else {
		if (temp <= highrecover && temp >= lowrecover) {
			pr_info("%s : battery recovering from thermal discharging\n",
				__func__);
			chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;
			chip->health_cnt = 0;
		}
	}
/*	temp = pm_chg_get_rt_status(chip, BATTTEMP_HOT_IRQ);
	if (temp)
		return POWER_SUPPLY_HEALTH_OVERHEAT;

	temp = pm_chg_get_rt_status(chip, BATTTEMP_COLD_IRQ);
	if (temp)
		return POWER_SUPPLY_HEALTH_COLD;
*/

return_health:
	/* no UI full-charged in NOT charging */
	if (chip->batt_health != POWER_SUPPLY_HEALTH_GOOD)
		chip->ui_term_cnt = 0;
	return chip->batt_health;
}

static int get_prop_charge_type(struct pm8921_chg_chip *chip)
{
	int temp;

	if (!chip->batt_present)
		return POWER_SUPPLY_CHARGE_TYPE_NONE;

	if (is_ext_trickle_charging(chip))
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;

	if (is_ext_charging(chip))
		return POWER_SUPPLY_CHARGE_TYPE_FAST;

	temp = pm_chg_get_rt_status(chip, TRKLCHG_IRQ);
	if (temp)
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;

	temp = pm_chg_get_rt_status(chip, FASTCHG_IRQ);
	if (temp)
		return POWER_SUPPLY_CHARGE_TYPE_FAST;

	return POWER_SUPPLY_CHARGE_TYPE_NONE;
}

static void pm8917_disable_charging(struct pm8921_chg_chip *chip)
{
	usb_target_ma  = 0;
	pm_chg_auto_enable(chip, 0);
	chip->charging_enabled = 0;
	chip->health_cnt = 0;
	chip->recharging_cnt = 0;

	chip->charging_start_time = 0;
	chip->charging_passed_time = 0;
	chip->is_recharging = false;
}

static void pm8917_enable_charging(struct pm8921_chg_chip *chip)
{
	ktime_t current_time;
	struct timespec ts;

	current_time = alarm_get_elapsed_realtime();
	ts = ktime_to_timespec(current_time);

	switch (chip->cable_type) {
	case CABLE_TYPE_USB:
	case CABLE_TYPE_AC:
	case CABLE_TYPE_CDP:
	case CABLE_TYPE_MISC:
	case CABLE_TYPE_UARTOFF:
	case CABLE_TYPE_AUDIO_DOCK:
#if defined(CONFIG_WIRELESS_CHARGING)
	case CABLE_TYPE_WPC:
#endif

	/* set CHG_EN bit to enable charging*/
	pm_chg_auto_enable(chip, 1);

//	if (chip->batt_pdata->siop_table) { // don't need siop_table, so remove it.
	if (1) {
		/* adapted SIOP */
		usb_target_ma =
			chip->batt_pdata->chg_current_table[
			chip->cable_type].vbus * chip->siop_level / 100;

		if (usb_target_ma > 0  &&
			usb_target_ma <
			chip->batt_pdata->chg_current_table[
			CABLE_TYPE_USB].vbus)
			usb_target_ma =
			chip->batt_pdata->chg_current_table[
			CABLE_TYPE_USB].vbus;
	} else {
		usb_target_ma =
			chip->batt_pdata->chg_current_table[
			chip->cable_type].vbus;
	}

		pm8921_charger_vbus_draw(usb_target_ma);

		pm8921_set_max_battery_charge_current(
			chip->batt_pdata->chg_current_table[
				chip->cable_type].ibat);
		break;
	default:
		break;
	}

	if (chip->charging_start_time == 0)
		chip->charging_start_time = ts.tv_sec;

	chip->charging_enabled = 1;
	chip->health_cnt = 0;	/* second check, plz */
	chip->recharging_cnt = 0;
}

static int get_prop_batt_status(struct pm8921_chg_chip *chip)
{
	int fsm_state = pm_chg_get_fsm_state(chip);
	int batt_capacity = chip->recent_reported_soc;
	int batt_voltage = get_prop_battery_uvolts(chip) / 1000;
	/*int i;*/

	/*for (i = 0; i < ARRAY_SIZE(map); i++)
		if (map[i].fsm_state == fsm_state)
			chip->batt_status = map[i].batt_state;

	if (fsm_state == FSM_STATE_ON_CHG_HIGHI_1) {
		if (!pm_chg_get_rt_status(chip, BATT_INSERTED_IRQ)
			|| pm_chg_get_rt_status(chip, VBATDET_LOW_IRQ))

			chip->batt_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
	}*/

	if (chip->cable_type == CABLE_TYPE_INCOMPATIBLE ||
		(ovp_state && !is_usb_chg_plugged_in(chip))) {
		chip->batt_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		return chip->batt_status;
	}

	if (chip->cable_type == CABLE_TYPE_UARTOFF &&
		!is_usb_chg_plugged_in(chip)) {
		chip->batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
		return chip->batt_status;
	}

	if (chip->cable_type == CABLE_TYPE_DESK_DOCK) {
		chip->batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
		return chip->batt_status;
	}

	if (chip->cable_type != CABLE_TYPE_NONE) {
		if (chip->ext_charge_done) {
			chip->batt_status = POWER_SUPPLY_STATUS_FULL;
			return chip->batt_status;
		}

		if ((chip->batt_status == POWER_SUPPLY_STATUS_CHARGING ||
			chip->batt_status == POWER_SUPPLY_STATUS_FULL) &&
			chip->batt_health == POWER_SUPPLY_HEALTH_GOOD &&
			fsm_state == FSM_STATE_ON_CHG_HIGHI_1 &&
			chip->boot_completed) {
			pr_info("%s: Full charged %d,%d,%d,%d\n", __func__,
				chip->batt_status, chip->batt_health,
				fsm_state, chip->charging_enabled);
			if ((batt_capacity > (chip->batt_pdata->capacity_max -
				chip->batt_pdata->capacity_max_margin) / 10)) {
				chip->batt_status = POWER_SUPPLY_STATUS_FULL;
				if (chip->charging_enabled)
					pm8917_disable_charging(chip);
				return chip->batt_status;
			} else {
				pr_err("%s: Not enough charging SOC:%d,%dmV\n",
					__func__, batt_capacity, batt_voltage);
				chip->batt_status = POWER_SUPPLY_STATUS_UNKNOWN;
				power_supply_changed(&chip->batt_psy);
			}
		}

		chip->usb_present = is_usb_chg_plugged_in(chip);
		pr_debug("%s: cable(usb:%d,dc:%d)\n",
			__func__, chip->usb_present, chip->dc_present);
		if (chip->usb_present || chip->dc_present) {
			if (chip->batt_health
				!= POWER_SUPPLY_HEALTH_GOOD) {
				chip->batt_status =
					POWER_SUPPLY_STATUS_NOT_CHARGING;
					pm8917_disable_charging(chip);
			} else if ((chip->batt_health ==
				POWER_SUPPLY_HEALTH_GOOD)
				&& (chip->batt_status ==
					POWER_SUPPLY_STATUS_NOT_CHARGING)) {
					if (chip->is_recharging)
						chip->batt_status =
						POWER_SUPPLY_STATUS_FULL;
					else
						chip->batt_status =
						POWER_SUPPLY_STATUS_CHARGING;
					pm8917_enable_charging(chip);
					pr_info("%s: Charging Recovered\n",
						__func__);
			} else {
				if (chip->batt_status !=
					POWER_SUPPLY_STATUS_FULL)
					chip->batt_status =
						POWER_SUPPLY_STATUS_CHARGING;
			}

			/* no full-charged check in initial time */
			if (chip->initial_count > 0)
				return chip->batt_status;

			if (chip->ui_term_cnt > 1) {
				chip->batt_status = POWER_SUPPLY_STATUS_FULL;
				if (chip->ui_term_cnt == 2) {
					if (chip->batt_pdata->
						charging_term_time) {
						chip->charging_term_time =
							chip->
							charging_passed_time +
							chip->batt_pdata->
							charging_term_time;
						pr_info("%s: term timer"
							" (%ld sec)\n",
							__func__,
							chip->
							charging_term_time);
					}
					wake_lock_timeout(
						&chip->monitor_wake_lock, 5*HZ);
					pr_info("%s: Charging Terminated\n",
						__func__);
					chip->ui_term_cnt++;
				}
			} else {
			/*for UI full charged (charging current is minus(uA))*/
				if ((get_prop_batt_current(chip) / (-1000) <=
					(int)chip->batt_pdata->
					ui_term_current) &&
					(batt_voltage >= chip->batt_pdata->
					 recharging_voltage - 150) &&
					(batt_capacity >
					(chip->batt_pdata->capacity_max -
					chip->batt_pdata->
					capacity_max_margin) / 10)) {
					chip->ui_term_cnt++;
					pr_info("%s: check ui term (cnt:%d)\n",
						__func__, chip->ui_term_cnt);
				} else
					chip->ui_term_cnt = 0;
			}

			return chip->batt_status;
		}
	} else {
		chip->batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
	}

	return chip->batt_status;
}


#if defined(CONFIG_MACH_CANE)
extern unsigned int system_rev;
#endif

static int get_prop_batt_temp(struct pm8921_chg_chip *chip)
{
	int rc;
	struct pm8xxx_adc_chan_result result;

#if defined(CONFIG_MACH_CANE)
	if(system_rev==0x04)
	{
		chip->batt_temp = 300;
		chip->batt_temp_adc = 300;

		goto temp_check_done;
	}
#endif

#if defined(CONFIG_MACH_BAFFINVETD_CHN_3G) || defined(CONFIG_MACH_LOGANRE_EUR_LTE)
	if (chip->hw_rev < 1) {
#else
	if (chip->hw_rev < 3) {
#endif
		chip->batt_temp = 250;
		chip->batt_temp_adc = 250;

		goto temp_check_done;
	}

	rc = pm8xxx_adc_mpp_config_read(TEMP_GPIO,
		TEMP_ADC_CHNNEL, &result);
	if (rc) {
		pr_err("error reading mpp %d, rc = %d\n",
			TEMP_GPIO, rc);
		return rc;
	}

	pr_debug("[battery] batt_temp phy = %lld meas = 0x%llx\n",
		result.physical, result.measurement);

	chip->batt_temp = (int)result.physical;
	chip->batt_temp_adc = (int)result.measurement;

temp_check_done:
	return chip->batt_temp;
}

static void  sec_bat_event_program_alarm(
	struct pm8921_chg_chip *chip, int seconds)
{
	ktime_t low_interval = ktime_set(seconds - 10, 0);
	ktime_t slack = ktime_set(20, 0);
	ktime_t next;

	next = ktime_add(chip->last_event_time, low_interval);
	alarm_start_range(&chip->event_termination_alarm,
		next, ktime_add(next, slack));
}

static void sec_bat_event_expired_timer_func(struct alarm *alarm)
{
	struct pm8921_chg_chip *chip =
		container_of(alarm, struct pm8921_chg_chip,
			event_termination_alarm);

	chip->event &= (~chip->event_wait);
	dev_info(chip->dev,
		"%s: event expired (0x%x)\n", __func__, chip->event);
}

static void sec_bat_event_set(
	struct pm8921_chg_chip *chip, int event, int enable)
{
	if (!chip->batt_pdata->event_check)
		return;

	/* ignore duplicated deactivation of same event
	 * only if the event is one last event
	 */
	if (!enable && (chip->event == chip->event_wait)) {
		dev_info(chip->dev,
			"%s: ignore duplicated deactivation of same event\n",
			__func__);
		return;
	}

	alarm_cancel(&chip->event_termination_alarm);
	chip->event &= (~chip->event_wait);

	if (enable) {
		chip->event_wait = 0;
		chip->event |= event;

		dev_info(chip->dev,
			"%s: event set (0x%x)\n", __func__, chip->event);
	} else {
		if (chip->event == 0) {
			dev_dbg(chip->dev,
				"%s: nothing to clear\n", __func__);
			return;	/* nothing to clear */
		}
		chip->event_wait = event;
		chip->last_event_time = alarm_get_elapsed_realtime();

		sec_bat_event_program_alarm(chip,
			chip->batt_pdata->event_waiting_time);
		dev_info(chip->dev,
			"%s: start timer (curr 0x%x, wait 0x%x)\n",
			__func__, chip->event, chip->event_wait);
	}
}
static int pm_fg_power_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct pm8921_chg_chip *chip = container_of(psy, struct pm8921_chg_chip,
								fg_psy);
	switch (psp) {
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = get_prop_battery_uvolts(chip);
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = get_prop_batt_capacity(chip);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = get_prop_batt_current(chip);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}
#define SLOW_SCALE 91
static int pm_batt_power_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct pm8921_chg_chip *chip = container_of(psy, struct pm8921_chg_chip,
								batt_psy);
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		/*val->intval = chip->charging_enabled;*/
		switch (chip->cable_type) {
		case CABLE_TYPE_NONE:
			val->intval = POWER_SUPPLY_TYPE_BATTERY;
			break;
		case CABLE_TYPE_USB:
			val->intval = POWER_SUPPLY_TYPE_USB;
			break;
		case CABLE_TYPE_AC:
			val->intval = POWER_SUPPLY_TYPE_MAINS;
			break;
		case CABLE_TYPE_MISC:
			val->intval = POWER_SUPPLY_TYPE_MISC;
			break;
		case CABLE_TYPE_UARTOFF:
			val->intval = POWER_SUPPLY_TYPE_UARTOFF;
			break;
		case CABLE_TYPE_CDP:
			val->intval = POWER_SUPPLY_TYPE_USB_CDP;
			break;
		case CABLE_TYPE_OTG:
			val->intval = POWER_SUPPLY_TYPE_OTG;
			break;
#if defined(CONFIG_WIRELESS_CHARGING)
		case CABLE_TYPE_WPC:
			val->intval = POWER_SUPPLY_TYPE_WPC;
			break;
#endif
		case CABLE_TYPE_CARDOCK:
			val->intval = POWER_SUPPLY_TYPE_CARDOCK;
			break;
		case CABLE_TYPE_DESK_DOCK:
			val->intval = POWER_SUPPLY_TYPE_DESK_DOCK;
			break;
		case CABLE_TYPE_INCOMPATIBLE:
			val->intval = POWER_SUPPLY_TYPE_UNKNOWN;
			break;
		default:
			val->intval = POWER_SUPPLY_TYPE_UNKNOWN;
			break;
		}

		break;
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = chip->batt_status;
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = get_prop_charge_type(chip);
		if (val->intval == POWER_SUPPLY_CHARGE_TYPE_FAST &&
			chip->initial_count < 1 &&
#if defined(CONFIG_WIRELESS_CHARGING)
			chip->cable_type != CABLE_TYPE_WPC &&
#endif
			usb_target_ma < chip->batt_pdata->chg_current_table[
			chip->cable_type].vbus * SLOW_SCALE / 100) {
			pr_info("%s: slow rate charging !!\n", __func__);
			val->intval = POWER_SUPPLY_CHARGE_TYPE_SLOW;
		}
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = chip->batt_health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = chip->batt_present;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		val->intval = chip->max_voltage_mv * 1000;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
		val->intval = chip->min_voltage_mv * 1000;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
		val->intval = get_prop_battery_uvolts(chip);
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (chip->batt_status == POWER_SUPPLY_STATUS_FULL)
			val->intval = 100;
		else
			val->intval = get_prop_batt_capacity(chip);
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		val->intval = usb_target_ma * 1000;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		val->intval = get_prop_batt_current(chip);
		val->intval *= -1;
		break;
	case POWER_SUPPLY_PROP_TEMP:
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		val->intval = get_prop_batt_temp(chip);
		break;
	case POWER_SUPPLY_PROP_ENERGY_FULL:
	case POWER_SUPPLY_PROP_ENERGY_NOW:
		val->intval = get_prop_batt_fcc(chip) * 1000;
		break;
	case POWER_SUPPLY_PROP_CHARGE_NOW:
		val->intval = get_prop_batt_charge_now(chip);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
#ifdef PM8917_SEC_CHARGER_CODE
static void handle_usb_insertion_removal(struct pm8921_chg_chip *chip);
static void handle_usb_insertion_removal_for_wq(struct pm8921_chg_chip *chip);
#define VIN_MIN_COLLAPSE_CHECK_MS	50
static int pm_batt_power_set_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       const union power_supply_propval *val)
{
	struct pm8921_chg_chip *chip = container_of(psy, struct pm8921_chg_chip,
								batt_psy);
	enum cable_type_t new_cable_type;
	int batt_capacity;

	if (!chip->dev) {
		pr_err("called before init\n");
		goto error_check;
	}

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		/* cable is attached or detached. called by usb switch ic */

		chip->cable_exception = CABLE_TYPE_NONE;
		switch (val->intval) {
		case POWER_SUPPLY_TYPE_BATTERY:
#if defined(CONFIG_WIRELESS_CHARGING)
			if (!gpio_get_value_cansleep(chip->wc_w_gpio)) {
				new_cable_type = CABLE_TYPE_WPC;
				break;
			}
#endif
			if (chip->batt_status == POWER_SUPPLY_STATUS_FULL) {
				batt_capacity = get_prop_batt_capacity(chip);
				sec_fg_calculate_dynamic_scale(chip,
					chip->capacity_raw * 10);
			}
			new_cable_type = CABLE_TYPE_NONE;
			break;
		case POWER_SUPPLY_TYPE_MAINS:
			new_cable_type = CABLE_TYPE_AC;
			break;
		case POWER_SUPPLY_TYPE_USB:
		case POWER_SUPPLY_TYPE_USB_ACA:
		case POWER_SUPPLY_TYPE_USB_DCP:
			new_cable_type = CABLE_TYPE_USB;
			break;
		case POWER_SUPPLY_TYPE_MISC:
			new_cable_type = CABLE_TYPE_MISC;
			break;
		case POWER_SUPPLY_TYPE_USB_CDP:
			new_cable_type = CABLE_TYPE_CDP;
			break;
		case POWER_SUPPLY_TYPE_CARDOCK:
			if (is_usb_chg_plugged_in(chip))
				new_cable_type = CABLE_TYPE_USB;
			else
				new_cable_type = CABLE_TYPE_CARDOCK;
			break;
		case POWER_SUPPLY_TYPE_DESK_DOCK:
			if (is_usb_chg_plugged_in(chip))
				new_cable_type = CABLE_TYPE_USB;
			else
				new_cable_type = CABLE_TYPE_DESK_DOCK;
			break;
		case POWER_SUPPLY_TYPE_UARTOFF:
			new_cable_type = CABLE_TYPE_UARTOFF;
			break;
#if defined(CONFIG_WIRELESS_CHARGING)
		case POWER_SUPPLY_TYPE_WPC:
			if (chip->cable_type != CABLE_TYPE_NONE)
				return 0;

			new_cable_type = CABLE_TYPE_WPC;
			break;
#endif
		case POWER_SUPPLY_TYPE_UNKNOWN:
			new_cable_type = CABLE_TYPE_INCOMPATIBLE;
			break;
		default:
			return -EINVAL;
		}

		if (new_cable_type == chip->cable_type && !ovp_state)
			pr_err("Same cable type, no need to change\n");
		else {
			chip->cable_type = new_cable_type;
			handle_cable_insertion_removal(chip);
		}
		break;
	default:
		return -EINVAL;
	}

error_check:
	return 0;
}
#endif

ssize_t sec_bat_show_attrs(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct pm8921_chg_chip *chip =
		container_of(psy, struct pm8921_chg_chip, batt_psy);

	int i = 0, val = 0;
	int batt_capacity;
	const ptrdiff_t offset = attr - sec_battery_attrs;

	switch (offset) {
	case BATT_RESET_SOC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			bms_reset);
		break;
	case BATT_READ_RAW_SOC:
		batt_capacity = get_prop_batt_capacity(chip);
		val = chip->capacity_raw;
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val*100);
		break;
	case BATT_READ_ADJ_SOC:
		val = get_prop_batt_capacity(chip);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	case BATT_TYPE:
		/* work later */
		break;
	case BATT_VFOCV:
		val = get_prop_battery_uvolts(chip) / 1000;
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	case BATT_VOL_ADC:
		break;
	case BATT_VOL_ADC_CAL:
		break;
	case BATT_VOL_AVER:
		break;
	case BATT_VOL_ADC_AVER:
		break;
	case BATT_TEMP_ADC:
#if defined(CONFIG_MACH_BAFFINVETD_CHN_3G)
		val = chip->batt_temp_adc;
#else
		val = get_prop_batt_temp(chip);
#endif
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	case BATT_TEMP_AVER:
		break;
	case BATT_TEMP_ADC_AVER:
		break;
	case BATT_VF_ADC:
		break;

	case BATT_LP_CHARGING:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			chip->get_lpm_mode() ? 1 : 0);
		break;
	case SIOP_ACTIVATED:
		break;
	case SIOP_LEVEL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			chip->siop_level);
		break;
	case BATT_CHARGING_SOURCE:
		switch (chip->cable_type) {
		case CABLE_TYPE_NONE:
			val = POWER_SUPPLY_TYPE_BATTERY;
			break;
		case CABLE_TYPE_USB:
			val = POWER_SUPPLY_TYPE_USB;
			break;
		case CABLE_TYPE_AC:
			val = POWER_SUPPLY_TYPE_MAINS;
			break;
		case CABLE_TYPE_MISC:
			val = POWER_SUPPLY_TYPE_MISC;
			break;
		case CABLE_TYPE_UARTOFF:
			val = POWER_SUPPLY_TYPE_UARTOFF;
			break;
		case CABLE_TYPE_CDP:
			val = POWER_SUPPLY_TYPE_USB_CDP;
			break;
		case CABLE_TYPE_OTG:
			val = POWER_SUPPLY_TYPE_OTG;
			break;
#if defined(CONFIG_WIRELESS_CHARGING)
		case CABLE_TYPE_WPC:
			val = POWER_SUPPLY_TYPE_WPC;
			break;
#endif
		default:
			val = POWER_SUPPLY_TYPE_UNKNOWN;
			break;
		}
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	case FG_REG_DUMP:
		break;
	case FG_RESET_CAP:
		break;
	case FG_CAPACITY:
		/* this case for current based FG */
		break;
	case AUTH:
		break;
	case CHG_CURRENT_ADC:
		 val = get_prop_batt_current(chip);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	case WC_ADC:
		break;
	case WC_STATUS:
#if defined(CONFIG_WIRELESS_CHARGING)
		val = !gpio_get_value_cansleep(chip->wc_w_gpio);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
#endif
		break;

	case FACTORY_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			chip->factory_mode);
		break;
	case UPDATE:
		break;
	case TEST_MODE:
		break;
	case BATT_EVENT_GSM_CALL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_2G_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_WCDMA_CALL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_3G_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_MUSIC:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_MUSIC) ? 1 : 0);
		break;
	case BATT_EVENT_VIDEO:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_VIDEO) ? 1 : 0);
		break;
	case BATT_EVENT_BROWSER:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_BROWSER) ? 1 : 0);
		break;
	case BATT_EVENT_CAMERA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_CAMERA) ? 1 : 0);
		break;
	case BATT_EVENT_DATA_CALL:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_DATA_CALL) ? 1 : 0);
		break;
	case BATT_EVENT_WIFI:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_WIFI) ? 1 : 0);
		break;
	case BATT_EVENT_LTE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			(chip->event & EVENT_LTE) ? 1 : 0);
		break;
	case BATT_EVENT:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			chip->event);
		break;
	case BATT_SLATE_MODE:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			       chip->slate_mode);
		break;
	default:
		i = -EINVAL;
	}

	return i;
}
ssize_t sec_fg_show_attrs(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct pm8921_chg_chip *chip =
		container_of(psy, struct pm8921_chg_chip, fg_psy);

	int i = 0, val = 0;
	const ptrdiff_t offset = attr - sec_fuelgauge_attrs;

	switch (offset) {
	case FG_CURR_UA:
		val = get_prop_batt_current(chip);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n",
			val);
		break;
	default:
		i = -EINVAL;
	}

	return i;
}
ssize_t sec_bat_store_attrs(
					struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct pm8921_chg_chip *chip =
		container_of(psy, struct pm8921_chg_chip, batt_psy);

	int ret = 0, x = 0;
	const ptrdiff_t offset = attr - sec_battery_attrs;
	int charging_current;

	switch (offset) {
	case BATT_RESET_SOC:
		if (sscanf(buf, "%d\n", &x) == 1) {
			pr_debug("%s : reset BMS module\n", __func__);
			bms_reset = 1;
			bms_quickstart();
		}
		ret = count;
		break;
	case BATT_READ_RAW_SOC:
		break;
	case BATT_READ_ADJ_SOC:
		break;
	case BATT_TYPE:
		break;
	case BATT_VFOCV:
		break;
	case BATT_VOL_ADC:
		break;
	case BATT_VOL_ADC_CAL:
		break;
	case BATT_VOL_AVER:
		break;
	case BATT_VOL_ADC_AVER:
		break;
	case BATT_TEMP_ADC:
		break;
	case BATT_TEMP_AVER:
		break;
	case BATT_TEMP_ADC_AVER:
		break;
	case BATT_VF_ADC:
		break;

	case BATT_LP_CHARGING:
		break;
	case SIOP_ACTIVATED:
		break;
	case SIOP_LEVEL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			if (chip->batt_status == POWER_SUPPLY_STATUS_CHARGING) {
#ifdef 	CONFIG_MACH_BAFFIN		
				pr_info("%s : SIOP level to %d, original is %d\n",
						__func__, x, chip->siop_level);

				if(x>50)
				{
					x=100;
				}
				else if(x>0)
				{
					x=50;
				}
				else if(x<=0)
				{
					x=0;
				}
				
				if((chip->batt_pdata->chg_current_table[chip->cable_type].vbus > chip->batt_pdata->chg_current_table[CABLE_TYPE_USB].vbus)&&(x != chip->siop_level))
				{
					charging_current =
						chip->batt_pdata->chg_current_table[
						chip->cable_type].vbus * x / 100;

					usb_target_ma = charging_current;
					pm8921_charger_vbus_draw(usb_target_ma);

					chip->siop_level = x;
					pr_info("%s : SIOP level to %d(%dmA)\n",
						__func__, chip->siop_level,
						usb_target_ma);
				}
#else
				charging_current =
					chip->batt_pdata->chg_current_table[
					chip->cable_type].vbus * x / 100;

				if (charging_current > 0  &&
					charging_current <
					chip->batt_pdata->chg_current_table[
					CABLE_TYPE_USB].vbus)
					charging_current =
					chip->batt_pdata->chg_current_table[
					CABLE_TYPE_USB].vbus;

				usb_target_ma = charging_current;
				if (x != chip->siop_level) {
					schedule_delayed_work(
					&the_chip->vin_collapse_check_work,
					round_jiffies_relative(msecs_to_jiffies
					(VIN_MIN_COLLAPSE_CHECK_MS)));
					pm8921_charger_vbus_draw(usb_target_ma);
				}

				chip->siop_level = x;
				pr_info("%s : SIOP level to %d(%dmA)\n",
					__func__, chip->siop_level,
					usb_target_ma);
#endif				
			}
			ret = count;
		}
		break;
	case BATT_CHARGING_SOURCE:
		break;
	case FG_REG_DUMP:
		break;
	case FG_RESET_CAP:
		break;
	case FG_CAPACITY:
		break;
	case AUTH:
		break;
	case CHG_CURRENT_ADC:
		break;
	case WC_ADC:
		break;
	case WC_STATUS:
		break;
	case FACTORY_MODE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			chip->factory_mode = x ? true : false;
			ret = count;
		}
		break;
	case UPDATE:
		ret = count;
		break;
	case TEST_MODE:
		break;

	case BATT_EVENT_GSM_CALL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, BATT_EVENT_GSM_CALL, x);
			ret = count;
		}
		break;
	case BATT_EVENT_WCDMA_CALL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, BATT_EVENT_WCDMA_CALL, x);
			ret = count;
		}
		break;
	case BATT_EVENT_MUSIC:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_MUSIC, x);
			ret = count;
		}
		break;
	case BATT_EVENT_VIDEO:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_VIDEO, x);
			ret = count;
		}
		break;
	case BATT_EVENT_BROWSER:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_BROWSER, x);
			ret = count;
		}
		break;
	case BATT_EVENT_HOTSPOT:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_HOTSPOT, x);
			ret = count;
		}
		break;
	case BATT_EVENT_CAMERA:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_CAMERA, x);
			ret = count;
		}
		break;
	case BATT_EVENT_DATA_CALL:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_DATA_CALL, x);
			ret = count;
		}
		break;
	case BATT_EVENT_WIFI:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_WIFI, x);
			ret = count;
		}
		break;
	case BATT_EVENT_LTE:
		if (sscanf(buf, "%d\n", &x) == 1) {
			sec_bat_event_set(chip, EVENT_LTE, x);
			ret = count;
		}
		break;
	case BATT_SLATE_MODE:
		{
			pr_info("%s : BATT_SLATE_MODE %s\n",
				__func__, buf);
			if (sscanf(buf, "%d\n", &x) == 1) {
				if (x == 1) {
					chip->slate_mode = true;
					chip->cable_type =
						CABLE_TYPE_NONE;
					pr_debug("batt_slate_mode, charging stop\n");
					handle_cable_insertion_removal(
						chip);
				} else if (x == 0) {
					chip->slate_mode = false;
					chip->cable_type =
						CABLE_TYPE_UARTOFF;
					pr_debug("batt_slate_mode, charging startn");
					handle_cable_insertion_removal(
						chip);
				} else
					pr_info("%s:Invalid\n", __func__);
				ret = count;
			}
		}
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}
ssize_t sec_fg_store_attrs(
					struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	/*struct power_supply *psy = dev_get_drvdata(dev);
	struct pm8921_chg_chip *chip =
		container_of(psy, struct pm8921_chg_chip, fg_psy);
*/
	int ret = 0;
	const ptrdiff_t offset = attr - sec_fuelgauge_attrs;

	switch (offset) {
	case FG_CURR_UA:
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static int sec_bat_create_attrs(struct device *dev)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(sec_battery_attrs); i++) {
		rc = device_create_file(dev, &sec_battery_attrs[i]);
		if (rc)
			goto create_attrs_failed;
	}
	goto create_attrs_succeed;

create_attrs_failed:
	while (i--)
		device_remove_file(dev, &sec_battery_attrs[i]);
create_attrs_succeed:
	return rc;
}
static int sec_fg_create_attrs(struct device *dev)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(sec_fuelgauge_attrs); i++) {
		rc = device_create_file(dev, &sec_fuelgauge_attrs[i]);
		if (rc)
			goto create_attrs_failed;
	}
	goto create_attrs_succeed;

create_attrs_failed:
	while (i--)
		device_remove_file(dev, &sec_fuelgauge_attrs[i]);
create_attrs_succeed:
	return rc;
}
static void (*notify_vbus_state_func_ptr)(int);
static int usb_chg_current;

int pm8921_charger_register_vbus_sn(void (*callback)(int))
{
	pr_debug("%p\n", callback);
	notify_vbus_state_func_ptr = callback;
	return 0;
}
EXPORT_SYMBOL_GPL(pm8921_charger_register_vbus_sn);

/* this is passed to the hsusb via platform_data msm_otg_pdata */
void pm8921_charger_unregister_vbus_sn(void (*callback)(int))
{
	pr_debug("%p\n", callback);
	notify_vbus_state_func_ptr = NULL;
}
EXPORT_SYMBOL_GPL(pm8921_charger_unregister_vbus_sn);
#ifdef VBUS_INTERRUPT_THROUGH_PMIC
static void notify_usb_of_the_plugin_event(int plugin)
{
	plugin = !!plugin;
	if (notify_vbus_state_func_ptr) {
		pr_debug("notifying plugin\n");
		(*notify_vbus_state_func_ptr) (plugin);
	} else {
		pr_debug("unable to notify plugin\n");
	}
}
#endif
/* This function sets the Vbus input current limit */
static void __pm8921_charger_vbus_draw(unsigned int mA)
{
	int i, rc;
	if (!the_chip) {
		pr_err("called before init\n");
		return;
	}

	/* [110312] QC patch - limit max charging current */
	if (usb_max_current && mA > usb_max_current) {
		pr_debug("restricting usb current to %d instead of %d\n",
					usb_max_current, mA);
		mA = usb_max_current;
	}

	pr_info("%s: vbus is changed (%d)--------\n", __func__, mA);
	/* if vbus 0mA, cut-off the Vbus current */
	if (mA <= 2) {
		usb_chg_current = 0;
		rc = pm_chg_iusbmax_set(the_chip, 0);
		if (rc)
			pr_err("unable to set iusb to %d rc = %d\n", 0, rc);

		rc = pm_chg_usb_suspend_enable(the_chip, 1);
		if (rc)
			pr_err("fail to set suspend bit rc=%d\n", rc);
	/* else vbus is set, configure Vbus current */
	} else {
		rc = pm_chg_usb_suspend_enable(the_chip, 0);
		if (rc)
			pr_err("fail to reset suspend bit rc=%d\n", rc);
		for (i = ARRAY_SIZE(usb_ma_table) - 1; i >= 0; i--) {
			if (usb_ma_table[i].usb_ma <= mA)
				break;
		}

		/* Check if IUSB_FINE_RES is available */
		while ((usb_ma_table[i].value & PM8917_IUSB_FINE_RES)
				&& !the_chip->iusb_fine_res)
			i--;
		if (i < 0)
			i = 0;
		rc = pm_chg_iusbmax_set(the_chip, i);
		if (rc)
			pr_err("unable to set iusb to %d rc = %d\n", i, rc);
	}
}

/* USB calls these to tell us how much max usb current the system can draw */
void pm8921_charger_vbus_draw(unsigned int mA)
{
	int set_usb_now_ma;

	pr_debug("Enter charge=%d\n", mA);

	/*
	 * Reject VBUS requests if USB connection is the only available
	 * power source. This makes sure that if booting without
	 * battery the iusb_max value is not decreased avoiding potential
	 * brown_outs.
	 *
	 * This would also apply when the battery has been
	 * removed from the running system.
	 */
	if (the_chip && !get_prop_batt_present(the_chip)
		&& !is_dc_chg_plugged_in(the_chip)) {
		if (!the_chip->has_dc_supply) {
			pr_err("rejected: no other power source connected\n");
			return;
		}
	}

	if (usb_max_current && mA > usb_max_current) {
		pr_warn("restricting usb current to %d instead of %d\n",
					usb_max_current, mA);
		mA = usb_max_current;
	}
	if (usb_target_ma == 0 && mA > USB_WALL_THRESHOLD_MA)
		usb_target_ma = mA;

	if (mA > USB_WALL_THRESHOLD_MA)
		set_usb_now_ma = USB_WALL_THRESHOLD_MA;
	else
		set_usb_now_ma = mA;

	if (the_chip && the_chip->disable_aicl)
		set_usb_now_ma = mA;

	if (the_chip)
		__pm8921_charger_vbus_draw(set_usb_now_ma);
	else
		/*
		 * called before pmic initialized,
		 * save this value and use it at probe
		 */
		usb_chg_current = set_usb_now_ma;
}
EXPORT_SYMBOL_GPL(pm8921_charger_vbus_draw);

int pm8921_charger_enable(bool enable)
{
	int rc;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	enable = !!enable;
	rc = pm_chg_auto_enable(the_chip, enable);
	if (rc)
		pr_err("Failed rc=%d\n", rc);
	return rc;
}
EXPORT_SYMBOL(pm8921_charger_enable);

int pm8921_is_usb_chg_plugged_in(void)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return is_usb_chg_plugged_in(the_chip);
}
EXPORT_SYMBOL(pm8921_is_usb_chg_plugged_in);

int pm8921_is_dc_chg_plugged_in(void)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return is_dc_chg_plugged_in(the_chip);
}
EXPORT_SYMBOL(pm8921_is_dc_chg_plugged_in);

int pm8921_is_battery_present(void)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return the_chip->batt_present;
}
EXPORT_SYMBOL(pm8921_is_battery_present);

int pm8921_is_batfet_closed(void)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return is_batfet_closed(the_chip);
}
EXPORT_SYMBOL(pm8921_is_batfet_closed);
/*
 * Disabling the charge current limit causes current
 * current limits to have no monitoring. An adequate charger
 * capable of supplying high current while sustaining VIN_MIN
 * is required if the limiting is disabled.
 */
int pm8921_disable_input_current_limit(bool disable)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	if (disable) {
		pr_warn("Disabling input current limit!\n");

		return pm8xxx_writeb(the_chip->dev->parent,
			 CHG_BUCK_CTRL_TEST3, 0xF2);
	}
	return 0;
}
EXPORT_SYMBOL(pm8921_disable_input_current_limit);

int pm8917_set_under_voltage_detection_threshold(int mv)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return pm_chg_uvd_threshold_set(the_chip, mv);
}
EXPORT_SYMBOL(pm8917_set_under_voltage_detection_threshold);

int pm8921_set_max_battery_charge_current(int ma)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	pr_err("pm8921_set_max_battery_charge_current (%dmA)--------\n", ma);
	return pm_chg_ibatmax_set(the_chip, ma);
}
EXPORT_SYMBOL(pm8921_set_max_battery_charge_current);

int pm8921_disable_source_current(bool disable)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	if (disable)
		pr_warn("current drawn from chg=0, battery provides current\n");

	pm_chg_usb_suspend_enable(the_chip, disable);

	return pm_chg_charge_dis(the_chip, disable);
}
EXPORT_SYMBOL(pm8921_disable_source_current);

int pm8921_regulate_input_voltage(int voltage)
{
	int rc;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	rc = pm_chg_vinmin_set(the_chip, voltage);

	if (rc == 0)
		the_chip->vin_min = voltage;

	return rc;
}

#define USB_OV_THRESHOLD_MASK  0x60
#define USB_OV_THRESHOLD_SHIFT  5
int pm8921_usb_ovp_set_threshold(enum pm8921_usb_ov_threshold ov)
{
	u8 temp;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}

	if (ov > PM_USB_OV_7V) {
		pr_err("limiting to over voltage threshold to 7volts\n");
		ov = PM_USB_OV_7V;
	}

	temp = USB_OV_THRESHOLD_MASK & (ov << USB_OV_THRESHOLD_SHIFT);

	return pm_chg_masked_write(the_chip, USB_OVP_CONTROL,
				USB_OV_THRESHOLD_MASK, temp);
}
EXPORT_SYMBOL(pm8921_usb_ovp_set_threshold);

#define USB_DEBOUNCE_TIME_MASK	0x06
#define USB_DEBOUNCE_TIME_SHIFT 1
int pm8921_usb_ovp_set_hystersis(enum pm8921_usb_debounce_time ms)
{
	u8 temp;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}

	if (ms > PM_USB_DEBOUNCE_80P5MS) {
		pr_err("limiting debounce to 80.5ms\n");
		ms = PM_USB_DEBOUNCE_80P5MS;
	}

	temp = USB_DEBOUNCE_TIME_MASK & (ms << USB_DEBOUNCE_TIME_SHIFT);

	return pm_chg_masked_write(the_chip, USB_OVP_CONTROL,
				USB_DEBOUNCE_TIME_MASK, temp);
}
EXPORT_SYMBOL(pm8921_usb_ovp_set_hystersis);

#define USB_OVP_DISABLE_MASK	0x80
int pm8921_usb_ovp_disable(int disable)
{
	u8 temp = 0;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}

	if (disable)
		temp = USB_OVP_DISABLE_MASK;

	return pm_chg_masked_write(the_chip, USB_OVP_CONTROL,
				USB_OVP_DISABLE_MASK, temp);
}

bool pm8921_is_battery_charging(int *source)
{
	int fsm_state, is_charging, dc_present, usb_present;

	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	fsm_state = pm_chg_get_fsm_state(the_chip);
	is_charging = is_battery_charging(fsm_state);
	if (is_charging == 0) {
		*source = PM8921_CHG_SRC_NONE;
		return is_charging;
	}

	if (source == NULL)
		return is_charging;

	/* the battery is charging, the source is requested, find it */
	dc_present = is_dc_chg_plugged_in(the_chip);
	usb_present = is_usb_chg_plugged_in(the_chip);

	if (dc_present && !usb_present)
		*source = PM8921_CHG_SRC_DC;

	if (usb_present && !dc_present)
		*source = PM8921_CHG_SRC_USB;

	if (usb_present && dc_present)
		/*
		 * The system always chooses dc for charging since it has
		 * higher priority.
		 */
		*source = PM8921_CHG_SRC_DC;

	return is_charging;
}
EXPORT_SYMBOL(pm8921_is_battery_charging);

int pm8921_set_usb_power_supply_type(enum power_supply_type type)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}

	if (type < POWER_SUPPLY_TYPE_USB)
		return -EINVAL;

	the_chip->usb_psy.type = type;
	power_supply_changed(&the_chip->usb_psy);
	power_supply_changed(&the_chip->dc_psy);
	return 0;
}
EXPORT_SYMBOL_GPL(pm8921_set_usb_power_supply_type);

int pm8921_batt_temperature(void)
{
	if (!the_chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}
	return get_prop_batt_temp(the_chip);
}

#define WPC_ACOK_ENABLE	0
#define WPC_ACOK_DISABLE	1
#ifdef PM8917_SEC_CHARGER_CODE
static void handle_cable_insertion_removal(struct pm8921_chg_chip *chip)
{
	pr_info("batt_present = %d\n", chip->batt_present);

	/*if (is_usb_chg_plugged_in(chip))
		bms_dis_seq(&chip->bms_dis_seq_work);
	else
		pm8xxx_writeb(chip->dev->parent, 0x224, 0x95);*/

	if (usb_target_ma)
		schedule_delayed_work(
			&the_chip->vin_collapse_check_work,
			round_jiffies_relative(msecs_to_jiffies
			(VIN_MIN_COLLAPSE_CHECK_MS)));
	else
	    handle_usb_insertion_removal(chip);

	/* initialize charging setting */
	chip->charging_enabled = 0;
	chip->health_cnt = 0;
	chip->ui_term_cnt = 0;
	chip->recharging_cnt = 0;

	chip->charging_start_time = 0;
	chip->charging_passed_time = 0;
	chip->is_recharging = false;
	chip->is_chgtime_expired = false;

	/* initialize health in cable event */
	chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;

	switch (chip->cable_type) {
	case CABLE_TYPE_NONE:
	case CABLE_TYPE_INCOMPATIBLE:
#if defined(CONFIG_WIRELESS_CHARGING)
		gpio_set_value(chip->wpc_acok, WPC_ACOK_ENABLE);
#endif
		disable_aicl(0);
		pr_debug("%s : cable is NONE or INCOMPATIBLE, batt_present(%d)\n",
			__func__, chip->batt_present);
		pm8917_disable_charging(chip);
		pm8921_charger_vbus_draw(VUBS_IN_CURR_NONE);
		break;
#if defined(CONFIG_WIRELESS_CHARGING)
	case CABLE_TYPE_WPC:
		disable_aicl(1);
		gpio_set_value(chip->wpc_acok, WPC_ACOK_ENABLE);
		pr_info("%s : WPC  is inserted, chg_current 700, batt_present(%d)\n",
			__func__, chip->batt_present);
		if (chip->batt_present && (!chip->charging_enabled ||
			chip->wc_w_state)) {
			mdelay(200);
			pm8917_enable_charging(chip);
		 }
	break;
#endif
	case CABLE_TYPE_USB:
		pr_info("%s : USB is inserted, chg_current 500, batt_present(%d)\n",
			__func__, chip->batt_present);
		if (chip->batt_present)
			pm8917_enable_charging(chip);
		break;
	case CABLE_TYPE_AC:
		pr_info("%s : TA is inserted, chg_current 1000, batt_present(%d)\n",
			__func__, chip->batt_present);
	case CABLE_TYPE_CDP:
	case CABLE_TYPE_MISC:
	case CABLE_TYPE_UARTOFF:
	case CABLE_TYPE_AUDIO_DOCK:
#if defined(CONFIG_WIRELESS_CHARGING)
		gpio_set_value(chip->wpc_acok, WPC_ACOK_DISABLE);
#endif
		if (chip->batt_present && (!chip->charging_enabled ||
			chip->wc_w_state)) {
			mdelay(200);
			pm8917_enable_charging(chip);
		 }
		break;
	default:
		break;
	}

	alarm_cancel(&chip->event_termination_alarm);
	wake_lock(&chip->monitor_wake_lock);
	schedule_delayed_work(&chip->update_heartbeat_work, 0);

	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->batt_psy);
}

static void handle_cable_insertion_removal_for_wq(struct pm8921_chg_chip *chip)
{
	pr_info("batt_present = %d\n", chip->batt_present);

	/*if (is_usb_chg_plugged_in(chip))
		bms_dis_seq(&chip->bms_dis_seq_work);
	else
		pm8xxx_writeb(chip->dev->parent, 0x224, 0x95);*/

	if (usb_target_ma)
		schedule_delayed_work(
			&the_chip->vin_collapse_check_work,
			round_jiffies_relative(msecs_to_jiffies
			(VIN_MIN_COLLAPSE_CHECK_MS)));
	else
	    handle_usb_insertion_removal_for_wq(chip);

	/* initialize charging setting */
	chip->charging_enabled = 0;
	chip->health_cnt = 0;
	chip->ui_term_cnt = 0;
	chip->recharging_cnt = 0;

	chip->charging_start_time = 0;
	chip->charging_passed_time = 0;
	chip->is_recharging = false;
	chip->is_chgtime_expired = false;

	/* initialize health in cable event */
	chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;

	switch (chip->cable_type) {
	case CABLE_TYPE_NONE:
	case CABLE_TYPE_INCOMPATIBLE:
#if defined(CONFIG_WIRELESS_CHARGING)
		gpio_set_value(chip->wpc_acok, WPC_ACOK_ENABLE);
#endif
		disable_aicl(0);
		pr_debug("%s : cable is NONE or INCOMPATIBLE, batt_present(%d)\n",
			__func__, chip->batt_present);
		pm8917_disable_charging(chip);
		pm8921_charger_vbus_draw(VUBS_IN_CURR_NONE);
		break;
#if defined(CONFIG_WIRELESS_CHARGING)
	case CABLE_TYPE_WPC:
		disable_aicl(1);
		gpio_set_value(chip->wpc_acok, WPC_ACOK_ENABLE);
		pr_info("%s : WPC  is inserted, chg_current 700, batt_present(%d)\n",
			__func__, chip->batt_present);
		if (chip->batt_present && (!chip->charging_enabled ||
			chip->wc_w_state)) {
			mdelay(200);
			pm8917_enable_charging(chip);
		 }
	break;
#endif
	case CABLE_TYPE_USB:
		pr_info("%s : USB is inserted, chg_current 500, batt_present(%d)\n",
			__func__, chip->batt_present);
		if (chip->batt_present)
			pm8917_enable_charging(chip);
		break;
	case CABLE_TYPE_AC:
		pr_info("%s : TA is inserted, chg_current 1000, batt_present(%d)\n",
			__func__, chip->batt_present);
	case CABLE_TYPE_CDP:
	case CABLE_TYPE_MISC:
	case CABLE_TYPE_UARTOFF:
	case CABLE_TYPE_AUDIO_DOCK:
#if defined(CONFIG_WIRELESS_CHARGING)
		gpio_set_value(chip->wpc_acok, WPC_ACOK_DISABLE);
#endif
		if (chip->batt_present && (!chip->charging_enabled ||
			chip->wc_w_state)) {
			mdelay(200);
			pm8917_enable_charging(chip);
		 }
		break;
	default:
		break;
	}

	alarm_cancel(&chip->event_termination_alarm);
	wake_lock(&chip->monitor_wake_lock);
	schedule_delayed_work(&chip->update_heartbeat_work, 0);

	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->batt_psy);
}
#endif

static void handle_usb_insertion_removal(struct pm8921_chg_chip *chip)
{
	int usb_present = 0;
	int cnt = 0;

	pm_chg_failed_clear(chip, 1);

	while (!usb_present && (cnt++ < 3)) {
		usb_present = is_usb_chg_plugged_in(chip);
		if (!usb_present)
			mdelay(100);
	}
	pr_info("%s : usb_present(%d)\n", __func__, usb_present);

	if (chip->usb_present ^ usb_present) {
#ifdef VBUS_INTERRUPT_THROUGH_PMIC
		notify_usb_of_the_plugin_event(usb_present);
#endif
		chip->usb_present = usb_present;
		power_supply_changed(&chip->usb_psy);
		power_supply_changed(&chip->batt_psy);
		pm8921_bms_calibrate_hkadc();
	}
	if (usb_present) {
		schedule_delayed_work(&chip->unplug_check_work,
			msecs_to_jiffies(UNPLUG_CHECK_RAMP_MS));
		pm8921_chg_enable_irq(chip, CHG_GONE_IRQ);
	} else {
		/* USB unplugged reset target current */
		usb_target_ma = 0;
		pm8921_chg_disable_irq(chip, CHG_GONE_IRQ);
	}
	bms_notify_check(chip);
}

static void handle_usb_insertion_removal_for_wq(struct pm8921_chg_chip *chip)
{
	int usb_present = 0;
	int cnt = 0;

	pm_chg_failed_clear(chip, 1);

	while (!usb_present && (cnt++ < 3)) {
		usb_present = is_usb_chg_plugged_in(chip);
		if (!usb_present)
			msleep(100);
	}
	pr_info("%s : usb_present(%d)\n", __func__, usb_present);

	if (chip->usb_present ^ usb_present) {
#ifdef VBUS_INTERRUPT_THROUGH_PMIC
		notify_usb_of_the_plugin_event(usb_present);
#endif
		chip->usb_present = usb_present;
		power_supply_changed(&chip->usb_psy);
		power_supply_changed(&chip->batt_psy);
		pm8921_bms_calibrate_hkadc();
	}
	if (usb_present) {
		schedule_delayed_work(&chip->unplug_check_work,
			msecs_to_jiffies(UNPLUG_CHECK_RAMP_MS));
		pm8921_chg_enable_irq(chip, CHG_GONE_IRQ);
	} else {
		/* USB unplugged reset target current */
		usb_target_ma = 0;
		pm8921_chg_disable_irq(chip, CHG_GONE_IRQ);
	}
	bms_notify_check(chip);
}

static void handle_stop_ext_chg(struct pm8921_chg_chip *chip)
{
	if (!chip->ext_psy) {
		pr_debug("external charger not registered.\n");
		return;
	}

	if (!chip->ext_charging) {
		pr_debug("already not charging.\n");
		return;
	}

	power_supply_set_charge_type(chip->ext_psy,
					POWER_SUPPLY_CHARGE_TYPE_NONE);
	pm8921_disable_source_current(false); /* release BATFET */
	power_supply_changed(&chip->dc_psy);
	chip->ext_charging = false;
	chip->ext_charge_done = false;
	bms_notify_check(chip);
	/* Update battery charging LEDs and user space battery info */
	power_supply_changed(&chip->batt_psy);
}

static void handle_start_ext_chg(struct pm8921_chg_chip *chip)
{
	int dc_present;
	int batt_present;
	int batt_temp_ok;
	unsigned long delay =
		round_jiffies_relative(msecs_to_jiffies(EOC_CHECK_PERIOD_MS));

	if (!chip->ext_psy) {
		pr_debug("external charger not registered.\n");
		return;
	}

	if (chip->ext_charging) {
		pr_debug("already charging.\n");
		return;
	}

	dc_present = is_dc_chg_plugged_in(the_chip);
	batt_present = pm_chg_get_rt_status(chip, BATT_INSERTED_IRQ);
	batt_temp_ok = pm_chg_get_rt_status(chip, BAT_TEMP_OK_IRQ);

	if (!dc_present) {
		pr_warn("%s. dc not present.\n", __func__);
		return;
	}
	if (!batt_present) {
		pr_warn("%s. battery not present.\n", __func__);
		return;
	}
	if (!batt_temp_ok) {
		pr_warn("%s. battery temperature not ok.\n", __func__);
		return;
	}
	pm8921_disable_source_current(true); /* Force BATFET=ON */

	schedule_delayed_work(&chip->unplug_check_work,
		msecs_to_jiffies(UNPLUG_CHECK_RAMP_MS));
	pm8921_chg_enable_irq(chip, CHG_GONE_IRQ);

	power_supply_set_online(chip->ext_psy, dc_present);
	power_supply_set_charge_type(chip->ext_psy,
					POWER_SUPPLY_CHARGE_TYPE_FAST);
	power_supply_changed(&chip->dc_psy);
	chip->ext_charging = true;
	chip->ext_charge_done = false;
	bms_notify_check(chip);
	/* Start BMS */
	schedule_delayed_work(&chip->eoc_work, delay);
	wake_lock(&chip->eoc_wake_lock);
	/* Update battery charging LEDs and user space battery info */
	power_supply_changed(&chip->batt_psy);
}

static void turn_off_ovp_fet(struct pm8921_chg_chip *chip, u16 ovptestreg)
{
	u8 temp;
	int rc;

	rc = pm8xxx_writeb(chip->dev->parent, ovptestreg, 0x30);
	if (rc) {
		pr_err("Failed to write 0x30 to OVP_TEST rc = %d\n", rc);
		return;
	}
	rc = pm8xxx_readb(chip->dev->parent, ovptestreg, &temp);
	if (rc) {
		pr_err("Failed to read from OVP_TEST rc = %d\n", rc);
		return;
	}
	/* set ovp fet disable bit and the write bit */
	temp |= 0x81;
	rc = pm8xxx_writeb(chip->dev->parent, ovptestreg, temp);
	if (rc) {
		pr_err("Failed to write 0x%x OVP_TEST rc=%d\n", temp, rc);
		return;
	}
}

static void turn_on_ovp_fet(struct pm8921_chg_chip *chip, u16 ovptestreg)
{
	u8 temp;
	int rc;

	rc = pm8xxx_writeb(chip->dev->parent, ovptestreg, 0x30);
	if (rc) {
		pr_err("Failed to write 0x30 to OVP_TEST rc = %d\n", rc);
		return;
	}
	rc = pm8xxx_readb(chip->dev->parent, ovptestreg, &temp);
	if (rc) {
		pr_err("Failed to read from OVP_TEST rc = %d\n", rc);
		return;
	}
	/* unset ovp fet disable bit and set the write bit */
	temp &= 0xFE;
	temp |= 0x80;
	rc = pm8xxx_writeb(chip->dev->parent, ovptestreg, temp);
	if (rc) {
		pr_err("Failed to write 0x%x to OVP_TEST rc = %d\n",
								temp, rc);
		return;
	}
}

static int param_open_ovp_counter = 10;
module_param(param_open_ovp_counter, int, 0644);

/*
Function : is_active_chg_plugged_in
Description : check if USBIN, or DCIN is active. and return the result.
*/
#define USB_ACTIVE_BIT BIT(5)
#define DC_ACTIVE_BIT BIT(6)
static int is_active_chg_plugged_in(struct pm8921_chg_chip *chip,
						u8 active_chg_mask)
{
	if (active_chg_mask & USB_ACTIVE_BIT)
		return pm_chg_get_rt_status(chip, USBIN_VALID_IRQ);
	else if (active_chg_mask & DC_ACTIVE_BIT)
		return pm_chg_get_rt_status(chip, DCIN_VALID_IRQ);
	else
		return 0;
}

#define WRITE_BANK_4		0xC0
#define OVP_DEBOUNCE_TIME 0x06
static void unplug_ovp_fet_open(struct pm8921_chg_chip *chip)
{
	int chg_gone = 0, active_chg_plugged_in = 0;
	int count = 0;
	u8 active_mask = 0;
	u16 ovpreg, ovptestreg;

	if (is_usb_chg_plugged_in(chip) &&
		(chip->active_path & USB_ACTIVE_BIT)) {
		ovpreg = USB_OVP_CONTROL;
		ovptestreg = USB_OVP_TEST;
		active_mask = USB_ACTIVE_BIT;
	} else if (is_dc_chg_plugged_in(chip) &&
		(chip->active_path & DC_ACTIVE_BIT)) {
		ovpreg = DC_OVP_CONTROL;
		ovptestreg = DC_OVP_TEST;
		active_mask = DC_ACTIVE_BIT;
	} else {
		return;
	}

	while (count++ < param_open_ovp_counter) {
		pm_chg_masked_write(chip, ovpreg, OVP_DEBOUNCE_TIME, 0x0);
		usleep(10);
		active_chg_plugged_in
			= is_active_chg_plugged_in(chip, active_mask);
		chg_gone = pm_chg_get_rt_status(chip, CHG_GONE_IRQ);
		pr_debug("OVP FET count = %d chg_gone=%d, active_valid = %d\n",
					count, chg_gone, active_chg_plugged_in);

		/* note usb_chg_plugged_in=0 => chg_gone=1 */
		if (chg_gone == 1 && active_chg_plugged_in == 1) {
			pr_debug("since chg_gone = 1 dis ovp_fet for 20msec\n");
			turn_off_ovp_fet(chip, ovptestreg);

			msleep(20);

			turn_on_ovp_fet(chip, ovptestreg);
		} else {
			break;
		}
	}
	if (pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8917)
		pm_chg_masked_write(chip, ovpreg, OVP_DEBOUNCE_TIME, 0x6);
	else
		pm_chg_masked_write(chip, ovpreg, OVP_DEBOUNCE_TIME, 0x2);

	pr_debug("Exit count=%d chg_gone=%d, active_valid=%d\n",
		count, chg_gone, active_chg_plugged_in);
	return;
}

static int find_usb_ma_value(int value)
{
	int i;

	for (i = ARRAY_SIZE(usb_ma_table) - 1; i >= 0; i--) {
		if (value >= usb_ma_table[i].usb_ma)
			break;
	}

	return i;
}

static void decrease_usb_ma_value(int *value)
{
	int i;

	if (value) {
		i = find_usb_ma_value(*value);
		if (i > 0)
			i--;
		while (!the_chip->iusb_fine_res && i > 0
			&& (usb_ma_table[i].value & PM8917_IUSB_FINE_RES))
			i--;
		*value = usb_ma_table[i].usb_ma;
	}
}

static void increase_usb_ma_value(int *value)
{
	int i;

	if (value) {
		i = find_usb_ma_value(*value);

		if (i < (ARRAY_SIZE(usb_ma_table) - 1))
			i++;
		/* Get next correct entry if IUSB_FINE_RES is not available */
		while (!the_chip->iusb_fine_res
			&& (usb_ma_table[i].value & PM8917_IUSB_FINE_RES)
			&& i < (ARRAY_SIZE(usb_ma_table) - 1))
			i++;

		*value = usb_ma_table[i].usb_ma;
	}
}

static void vin_collapse_check_worker(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct pm8921_chg_chip *chip = container_of(dwork,
			struct pm8921_chg_chip, vin_collapse_check_work);

	/* AICL only for wall-chargers */
	if (is_usb_chg_plugged_in(chip) &&
		usb_target_ma > USB_WALL_THRESHOLD_MA) {
		/* decrease usb_target_ma */
		decrease_usb_ma_value(&usb_target_ma);
		/* reset here, increase in unplug_check_worker */
		__pm8921_charger_vbus_draw(USB_WALL_THRESHOLD_MA);
		pr_debug("usb_now=%d, usb_target = %d\n",
				USB_WALL_THRESHOLD_MA, usb_target_ma);
		if (!delayed_work_pending(&chip->unplug_check_work))
			schedule_delayed_work(&chip->unplug_check_work,
				      msecs_to_jiffies
						(UNPLUG_CHECK_WAIT_PERIOD_MS));
	} else {
		handle_usb_insertion_removal_for_wq(chip);
	}
}

#define VIN_MIN_COLLAPSE_CHECK_MS	50
static irqreturn_t usbin_valid_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	pr_info("%s: activated =====================\n", __func__);
	wake_lock_timeout(&chip->cable_wake_lock, 5*HZ);

#if 0
	if (usb_target_ma)
		schedule_delayed_work(&the_chip->vin_collapse_check_work,
			round_jiffies_relative(msecs_to_jiffies
			(VIN_MIN_COLLAPSE_CHECK_MS)));
	else
	    handle_usb_insertion_removal(data);
#endif

	if (chip->cable_exception &&
		is_usb_chg_plugged_in(data)) {
		pr_debug("USBIN is recovered. start charging\n");
		chip->cable_type = chip->cable_exception;
		handle_cable_insertion_removal(data);
	}
	if (chip->charging_enabled && (!is_usb_chg_plugged_in(data))
			&& !ovp_state) {
		pr_debug("USBIN is gone. stop charging\n");
		chip->cable_exception = chip->cable_type;
		chip->cable_type = CABLE_TYPE_NONE;
		handle_cable_insertion_removal(data);
	}
	return IRQ_HANDLED;
}

static irqreturn_t batt_inserted_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int status;

	status = pm_chg_get_rt_status(chip, BATT_INSERTED_IRQ);
	schedule_work(&chip->battery_id_valid_work);
	handle_start_ext_chg(chip);
	pr_debug("battery present=%d", status);
	power_supply_changed(&chip->batt_psy);
	return IRQ_HANDLED;
}

/*
 * this interrupt used to restart charging a battery.
 *
 * Note: When DC-inserted the VBAT can't go low.
 * VPH_PWR is provided by the ext-charger.
 * After End-Of-Charging from DC, charging can be resumed only
 * if DC is removed and then inserted after the battery was in use.
 * Therefore the handle_start_ext_chg() is not called.
 */
static irqreturn_t vbatdet_low_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int high_transition;

	high_transition = pm_chg_get_rt_status(chip, VBATDET_LOW_IRQ);

	if (high_transition) {
		/* enable auto charging */
		pm_chg_auto_enable(chip, !charging_disabled);
		chip->is_recharging = true;
		pr_info("batt fell below resume voltage %s\n",
			charging_disabled ? "" : "charger enabled");
	}
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));

	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->dc_psy);

	return IRQ_HANDLED;
}

static irqreturn_t chgwdog_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t vcp_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t atcdone_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t atcfail_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t chgdone_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	pr_debug("state_changed_to=%d\n", pm_chg_get_fsm_state(data));

	handle_stop_ext_chg(chip);

	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->dc_psy);

	bms_notify_check(chip);

	return IRQ_HANDLED;
}

static irqreturn_t chgfail_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int ret;

	ret = pm_chg_failed_clear(chip, 1);
	if (ret)
		pr_err("Failed to write CHG_FAILED_CLEAR bit\n");

	pr_err("batt_present = %d, batt_temp_ok = %d, state_changed_to=%d\n",
			chip->batt_present,
			pm_chg_get_rt_status(chip, BAT_TEMP_OK_IRQ),
			pm_chg_get_fsm_state(data));

	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->dc_psy);
	return IRQ_HANDLED;
}

static irqreturn_t chgstate_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	pr_debug("state_changed_to=%d\n", pm_chg_get_fsm_state(data));
	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	power_supply_changed(&chip->dc_psy);

	bms_notify_check(chip);

	return IRQ_HANDLED;
}

enum {
	PON_TIME_25NS	= 0x04,
	PON_TIME_50NS	= 0x08,
	PON_TIME_100NS	= 0x0C,
};

static void set_min_pon_time(struct pm8921_chg_chip *chip, int pon_time_ns)
{
	u8 temp;
	int rc;

	rc = pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, 0x40);
	if (rc) {
		pr_err("Failed to write 0x70 to CTRL_TEST3 rc = %d\n", rc);
		return;
	}
	rc = pm8xxx_readb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, &temp);
	if (rc) {
		pr_err("Failed to read CTRL_TEST3 rc = %d\n", rc);
		return;
	}
	/* clear the min pon time select bit */
	temp &= 0xF3;
	/* set the pon time */
	temp |= (u8)pon_time_ns;
	/* write enable bank 4 */
	temp |= 0x80;
	rc = pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, temp);
	if (rc) {
		pr_err("Failed to write 0x%x to CTRL_TEST3 rc=%d\n", temp, rc);
		return;
	}
}

static void attempt_reverse_boost_fix(struct pm8921_chg_chip *chip)
{
	pr_debug("Start\n");
	set_min_pon_time(chip, PON_TIME_100NS);
	pm_chg_vinmin_set(chip, chip->vin_min + 200);
	msleep(250);
	pm_chg_vinmin_set(chip, chip->vin_min);
	set_min_pon_time(chip, PON_TIME_25NS);
	pr_debug("End\n");
}

#define VIN_ACTIVE_BIT BIT(0)
#define UNPLUG_WRKARND_RESTORE_WAIT_PERIOD_US 200
#define VIN_MIN_INCREASE_MV 100
/*
Function : unplug_check_worker
Description :
*/
static void unplug_check_worker(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct pm8921_chg_chip *chip = container_of(dwork,
				struct pm8921_chg_chip, unplug_check_work);
	u8 reg_loop, active_path;
	int rc, ibat, active_chg_plugged_in, usb_ma;
	int chg_gone = 0;
	bool ramp = false;

	reg_loop = 0;

	rc = pm8xxx_readb(chip->dev->parent, PBL_ACCESS1, &active_path);
	if (rc) {
		pr_err("Failed to read PBL_ACCESS1 rc=%d\n", rc);
		return;
	}
	chip->active_path = active_path;

	active_chg_plugged_in = is_active_chg_plugged_in(chip, active_path);
	pr_debug("active_path = 0x%x, active_chg_plugged_in = %d\n",
			active_path, active_chg_plugged_in);

	/* case1. USBIN active */
	if (active_path & USB_ACTIVE_BIT) {
		pr_debug("USB charger active\n");

		pm_chg_iusbmax_get(chip, &usb_ma);

		if (usb_ma <= 100) {
			pr_debug(
				"Unenumerated or suspended usb_ma = %d skip\n",
				usb_ma);
			goto check_again_later;
		}
	/* case2. DCIN active */
	} else if (active_path & DC_ACTIVE_BIT) {
		pr_debug("DC charger active\n");
		/*
		 * Some board designs are not prone to reverse boost on DC
		 * charging path
		 */
		if (!chip->dc_unplug_check)
			return;
	} else {
		/* No charger active */
		if (!(is_usb_chg_plugged_in(chip)
				&& !(is_dc_chg_plugged_in(chip)))) {
			pr_debug(
			"Stop: chg removed reg_loop = %d, fsm = %d ibat = %d\n",
				pm_chg_get_regulation_loop(chip),
				pm_chg_get_fsm_state(chip),
				get_prop_batt_current(chip)
				);
			return;
		} else {
			goto check_again_later;
		}
	}
	/* AICL only for usb path and that too only for usb wall chargers */
	if ((active_path & USB_ACTIVE_BIT) && usb_target_ma > 0) {
		reg_loop = pm_chg_get_regulation_loop(chip);
			pr_debug("reg_loop=0x%x usb_ma = %d\n",
				reg_loop, usb_ma);
		if ((reg_loop & VIN_ACTIVE_BIT) &&
			(usb_ma > USB_WALL_THRESHOLD_MA)
			&& !charging_disabled && !the_chip->disable_aicl) {
			decrease_usb_ma_value(&usb_ma);
			usb_target_ma = usb_ma;
			/* end AICL here */
			__pm8921_charger_vbus_draw(usb_ma);
			pr_debug("usb_now=%d, usb_target = %d\n",
				usb_ma, usb_target_ma);
		}
	}

	reg_loop = pm_chg_get_regulation_loop(chip);
	pr_debug("reg_loop=0x%x usb_ma = %d\n", reg_loop, usb_ma);

	ibat = get_prop_batt_current(chip);
	if (reg_loop & VIN_ACTIVE_BIT) {
		if (ibat > 0) {
			pr_debug("revboost ibat = %d fsm = %d loop = 0x%x\n",
				ibat, pm_chg_get_fsm_state(chip), reg_loop);
			attempt_reverse_boost_fix(chip);
			/* after reverse boost fix check if the active
			 * charger was detected as removed */
			active_chg_plugged_in
				= is_active_chg_plugged_in(chip,
					active_path);
			pr_debug("revboost post: active_chg_plugged_in = %d\n",
					active_chg_plugged_in);
		}
	}

	active_chg_plugged_in = is_active_chg_plugged_in(chip, active_path);
	pr_debug("active_path = 0x%x, active_chg = %d\n",
			active_path, active_chg_plugged_in);
	chg_gone = pm_chg_get_rt_status(chip, CHG_GONE_IRQ);

	if (chg_gone == 1  && active_chg_plugged_in == 1) {
		pr_debug("chg_gone=%d, active_chg_plugged_in = %d\n",
					chg_gone, active_chg_plugged_in);
		unplug_ovp_fet_open(chip);
	}

	/* AICL only for usb path and that too only for usb wall chargers */
	if (!(reg_loop & VIN_ACTIVE_BIT) && (active_path & USB_ACTIVE_BIT)
		&& usb_target_ma > 0
		&& !charging_disabled && !the_chip->disable_aicl) {
		/* only increase iusb_max if vin loop not active */
		if (usb_ma < usb_target_ma) {
			increase_usb_ma_value(&usb_ma);
			__pm8921_charger_vbus_draw(usb_ma);
			pr_debug("usb_now=%d, usb_target = %d\n",
					usb_ma, usb_target_ma);
			ramp = true;
		} else {
			usb_target_ma = usb_ma;
		}
	}
check_again_later:
	pr_debug("ramp: %d\n", ramp);
	/* schedule to check again later */
	if (ramp)
		schedule_delayed_work(&chip->unplug_check_work,
			msecs_to_jiffies(UNPLUG_CHECK_RAMP_MS));
	else
		schedule_delayed_work(&chip->unplug_check_work,
			msecs_to_jiffies(UNPLUG_CHECK_WAIT_PERIOD_MS));
}

static irqreturn_t loop_change_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	pr_debug("fsm_state=%d reg_loop=0x%x\n",
		pm_chg_get_fsm_state(data),
		pm_chg_get_regulation_loop(data));
	schedule_work(&chip->unplug_check_work.work);
	return IRQ_HANDLED;
}

static irqreturn_t fastchg_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int high_transition;

	high_transition = pm_chg_get_rt_status(chip, FASTCHG_IRQ);
	if (high_transition && !delayed_work_pending(&chip->eoc_work)) {
		wake_lock(&chip->eoc_wake_lock);
		schedule_delayed_work(&chip->eoc_work,
				      round_jiffies_relative(msecs_to_jiffies
						     (EOC_CHECK_PERIOD_MS)));
	}
	power_supply_changed(&chip->batt_psy);
	bms_notify_check(chip);
	return IRQ_HANDLED;
}

static irqreturn_t trklchg_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	power_supply_changed(&chip->batt_psy);
	return IRQ_HANDLED;
}

static irqreturn_t batt_removed_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int status;

	status = pm_chg_get_rt_status(chip, BATT_REMOVED_IRQ);
	pr_debug("battery present=%d state=%d", !status,
					 pm_chg_get_fsm_state(data));
	handle_stop_ext_chg(chip);
	power_supply_changed(&chip->batt_psy);
	return IRQ_HANDLED;
}

static irqreturn_t batttemp_hot_irq_handler(int irq, void *data)
{
	/*struct pm8921_chg_chip *chip = data;*/

	/* handle_stop_ext_chg(chip); */
	/* power_supply_changed(&chip->batt_psy); */
	return IRQ_HANDLED;
}

static irqreturn_t chghot_irq_handler(int irq, void *data)
{
	/*struct pm8921_chg_chip *chip = data;*/

	pr_debug("Chg hot fsm_state=%d\n", pm_chg_get_fsm_state(data));
	/* power_supply_changed(&chip->batt_psy); */
	/* power_supply_changed(&chip->usb_psy); */
	/* handle_stop_ext_chg(chip); */
	return IRQ_HANDLED;
}

static irqreturn_t batttemp_cold_irq_handler(int irq, void *data)
{
	/*struct pm8921_chg_chip *chip = data;*/

	pr_debug("Batt cold fsm_state=%d\n", pm_chg_get_fsm_state(data));
	/* handle_stop_ext_chg(chip); */

	/* power_supply_changed(&chip->batt_psy); */
	/* power_supply_changed(&chip->usb_psy); */
	return IRQ_HANDLED;
}

static irqreturn_t chg_gone_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int chg_gone, usb_chg_plugged_in;

	usb_chg_plugged_in = is_usb_chg_plugged_in(chip);
	chg_gone = pm_chg_get_rt_status(chip, CHG_GONE_IRQ);

	pr_debug("chg_gone=%d, usb_valid = %d\n", chg_gone, usb_chg_plugged_in);
	pr_debug("Chg gone fsm_state=%d\n", pm_chg_get_fsm_state(data));

	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	return IRQ_HANDLED;
}
/*
 *
 * bat_temp_ok_irq_handler - is edge triggered, hence it will
 * fire for two cases:
 *
 * If the interrupt line switches to high temperature is okay
 * and thus charging begins.
 * If bat_temp_ok is low this means the temperature is now
 * too hot or cold, so charging is stopped.
 *
 */
static irqreturn_t bat_temp_ok_irq_handler(int irq, void *data)
{
	int bat_temp_ok;
	struct pm8921_chg_chip *chip = data;

	bat_temp_ok = pm_chg_get_rt_status(chip, BAT_TEMP_OK_IRQ);

	pr_debug("batt_temp_ok = %d fsm_state%d\n",
			 bat_temp_ok, pm_chg_get_fsm_state(data));

	if (bat_temp_ok)
		handle_start_ext_chg(chip);
	else
		handle_stop_ext_chg(chip);

	power_supply_changed(&chip->batt_psy);
	power_supply_changed(&chip->usb_psy);
	bms_notify_check(chip);
	return IRQ_HANDLED;
}

static irqreturn_t coarse_det_low_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t vdd_loop_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t vreg_ov_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t vbatdet_irq_handler(int irq, void *data)
{
	pr_debug("fsm_state=%d\n", pm_chg_get_fsm_state(data));
	return IRQ_HANDLED;
}

static irqreturn_t batfet_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	pr_debug("vreg ov\n");
	power_supply_changed(&chip->batt_psy);
	return IRQ_HANDLED;
}

static irqreturn_t dcin_valid_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int dc_present;

	dc_present = pm_chg_get_rt_status(chip, DCIN_VALID_IRQ);
	if (chip->ext_psy)
		power_supply_set_online(chip->ext_psy, dc_present);
	chip->dc_present = dc_present;
	if (dc_present)
		handle_start_ext_chg(chip);
	else
		handle_stop_ext_chg(chip);

	return IRQ_HANDLED;
}

static irqreturn_t dcin_ov_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	handle_stop_ext_chg(chip);
	return IRQ_HANDLED;
}

static irqreturn_t dcin_uv_irq_handler(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;

	handle_stop_ext_chg(chip);

	return IRQ_HANDLED;
}

static int __pm_batt_external_power_changed_work(struct device *dev, void *data)
{
	struct power_supply *psy = &the_chip->batt_psy;
	struct power_supply *epsy = dev_get_drvdata(dev);
	int i, dcin_irq;

	/* Only search for external supply if none is registered */
	if (!the_chip->ext_psy) {
		dcin_irq = the_chip->pmic_chg_irq[DCIN_VALID_IRQ];
		for (i = 0; i < epsy->num_supplicants; i++) {
			if (!strncmp(epsy->supplied_to[i], psy->name, 7)) {
				if (!strncmp(epsy->name, "dc", 2)) {
					the_chip->ext_psy = epsy;
					dcin_valid_irq_handler(dcin_irq,
							the_chip);
				}
			}
		}
	}
	return 0;
}

static void pm_batt_external_power_changed(struct power_supply *psy)
{
	/* Only look for an external supply if it hasn't been registered */
	if (!the_chip->ext_psy)
		class_for_each_device(power_supply_class, NULL, psy,
					 __pm_batt_external_power_changed_work);
}

static void pm_update_alarm(struct alarm *alarm)
{
	struct pm8921_chg_chip *chip = container_of(alarm,
				struct pm8921_chg_chip, update_alarm);

	if (!chip->is_in_sleep) {
		wake_lock(&chip->monitor_wake_lock);
		schedule_delayed_work(&chip->update_heartbeat_work, 0);
	}
}

static void pm_program_alarm(struct pm8921_chg_chip *chip, int seconds)
{
	ktime_t low_interval = ktime_set(seconds, 0);
	ktime_t slack = ktime_set(10, 0);
	ktime_t next;

	next = ktime_add(chip->last_update_time, low_interval);
	alarm_start_range(&chip->update_alarm,
			next, ktime_add(next, slack));
}

static bool pm_abs_time_management(struct pm8921_chg_chip *chip)
{
	unsigned long charging_time;
	ktime_t	current_time;
	struct timespec ts;

	current_time = alarm_get_elapsed_realtime();
	ts = ktime_to_timespec(current_time);

	if ((chip->is_chgtime_expired) &&
		(chip->usb_present || chip->dc_present)) {
		pm8917_enable_charging(chip);
		chip->is_chgtime_expired = false;
		chip->is_recharging = true;
		pr_info("%s: Restart charging after timer expired\n", __func__);
	}
	if (chip->charging_start_time == 0) {
		pr_debug("%s: Charging Disabled\n", __func__);
		return true;
	}

	if (ts.tv_sec >= chip->charging_start_time) {
		charging_time = ts.tv_sec - chip->charging_start_time;
		pr_debug("charging_start_time(%ld)\n",
			chip->charging_start_time);
	} else {
		charging_time = 0xFFFFFFFF - chip->charging_start_time
		    + ts.tv_sec;
		pr_debug("charging_start_time(%ld)\n",
			chip->charging_start_time);
	}

	chip->charging_passed_time = charging_time;
	pr_info("%s: Charging Time : %ld secs (rechg:%d)", __func__,
		chip->charging_passed_time, chip->is_recharging);

	switch (chip->batt_status) {
	case POWER_SUPPLY_STATUS_FULL:
		if (!chip->is_recharging &&
			chip->batt_pdata->charging_term_time &&
			(charging_time > chip->charging_term_time)) {
			pr_info("%s: Charging Terminated by Timer\n", __func__);
			pm8917_disable_charging(chip);

			return false;
		}
		if (chip->is_recharging && (charging_time >
			chip->batt_pdata->recharging_total_time)) {
			pr_info("%s: Recharging Timer Expired\n", __func__);
			chip->is_recharging = false;
			chip->is_chgtime_expired = true;
			pm8917_disable_charging(chip);

			return false;
		}
		break;
	case POWER_SUPPLY_STATUS_CHARGING:
		if (chip->is_recharging &&
			(charging_time >
				chip->batt_pdata->recharging_total_time)) {
			pr_info("%s: Recharging Timer Expired\n", __func__);
			chip->is_recharging = false;
			chip->is_chgtime_expired = true;
			wake_lock_timeout(
						&chip->cable_wake_lock, 90*HZ);
			pm8917_disable_charging(chip);

			return false;
		} else if (!chip->is_recharging &&
			(charging_time >
				chip->batt_pdata->charging_total_time)) {
			pr_info("%s: Charging Timer Expired\n", __func__);
			chip->is_chgtime_expired = true;
			wake_lock_timeout(
						&chip->cable_wake_lock, 90*HZ);

			pm8917_disable_charging(chip);
			return false;
		}
		break;
	default:
		pr_debug("%s: Undefine Battery Status\n", __func__);
		return true;
	}

	return true;
}

/**
 * update_heartbeat - internal function to update userspace
 *		per update_time minutes
 *
 */
static void update_heartbeat(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct pm8921_chg_chip *chip = container_of(dwork,
				struct pm8921_chg_chip, update_heartbeat_work);
	int batt_voltage;
	int batt_capacity;
	int fsm_state;

	if (chip->is_in_sleep)
		chip->is_in_sleep = false;

	fsm_state = pm_chg_get_fsm_state(chip);

	pm_chg_failed_clear(chip, 1);

	batt_capacity = get_prop_batt_capacity(chip);

	pm_abs_time_management(chip);

	get_prop_batt_health(chip);
	get_prop_batt_status(chip);

	if (chip->batt_pdata->monitor_additional_check)
		chip->batt_pdata->monitor_additional_check();

	/* check for recharging by battery voltage */
	if ((chip->batt_status == POWER_SUPPLY_STATUS_FULL) &&
		!chip->charging_enabled &&
		!chip->is_recharging &&
		(chip->batt_health == POWER_SUPPLY_HEALTH_GOOD)) {
		batt_voltage = get_prop_battery_uvolts(chip) / 1000;
		if (batt_voltage <
			(chip->batt_pdata->recharging_voltage))
			chip->recharging_cnt++;
		else
			chip->recharging_cnt = 0;

		if (chip->recharging_cnt > 3) {
			pm8917_enable_charging(chip);
			chip->is_recharging = true;
			pr_info("%s: Restart charging by battery voltage\n",
				__func__);
		}
	}

	pr_info("health(%d),stat(%d),chgen(%d),fsm(%d),cable(%d),extchg(%d)\n",
		chip->batt_health, chip->batt_status, chip->charging_enabled,
		fsm_state, chip->cable_type, chip->ext_charging);
	pr_info("soc(%d/%d), v(%d), i(%d), temp(%d), batt(%d), absexpr(%d), siop(%d)\n",
		batt_capacity,
		chip->capacity_raw,
		get_prop_battery_uvolts(chip),
		get_prop_batt_current(chip),
		chip->batt_temp, chip->batt_present, chip->is_chgtime_expired,
		chip->siop_level);

	power_supply_changed(&chip->batt_psy);
#if 0
	schedule_delayed_work(&chip->update_heartbeat_work,
			      round_jiffies_relative(msecs_to_jiffies
						     (chip->update_time)));
#else
	chip->last_update_time = alarm_get_elapsed_realtime();
	if (chip->initial_count > 0) {
		chip->initial_count--;
		if ((chip->cable_type != CABLE_TYPE_NONE &&
			!chip->charging_enabled) ||
			(chip->cable_type == CABLE_TYPE_NONE &&
			chip->charging_enabled))
			handle_cable_insertion_removal_for_wq(chip);

		pm_program_alarm(chip, 1);
	} else{
		chip->boot_completed = true;
		pm_program_alarm(chip, chip->update_time / 1000);
	}
#endif
	wake_unlock(&chip->monitor_wake_lock);
}
#define VDD_LOOP_ACTIVE_BIT	BIT(3)
#define VDD_MAX_INCREASE_MV	400
static int vdd_max_increase_mv = VDD_MAX_INCREASE_MV;
module_param(vdd_max_increase_mv, int, 0644);

static int ichg_threshold_ua = -400000;
module_param(ichg_threshold_ua, int, 0644);
#define MIN_DELTA_MV_TO_INCREASE_VDD_MAX 	13
#define PM8921_CHG_VDDMAX_RES_MV	10
static void adjust_vdd_max_for_fastchg(struct pm8921_chg_chip *chip,
						int vbat_batt_terminal_uv)
{
	int adj_vdd_max_mv, programmed_vdd_max;
	int vbat_batt_terminal_mv;
	int reg_loop;
	int delta_mv = 0;

	if (chip->rconn_mohm == 0) {
		pr_debug("Exiting as rconn_mohm is 0\n");
		return;
	}
	/* adjust vdd_max only in normal temperature zone */
	if (chip->is_bat_cool || chip->is_bat_warm) {
		pr_debug("Exiting is_bat_cool = %d is_batt_warm = %d\n",
				chip->is_bat_cool, chip->is_bat_warm);
		return;
	}

	reg_loop = pm_chg_get_regulation_loop(chip);
	if (!(reg_loop & VDD_LOOP_ACTIVE_BIT)) {
		pr_debug("Exiting Vdd loop is not active reg loop=0x%x\n",
			reg_loop);
		return;
	}
	vbat_batt_terminal_mv = vbat_batt_terminal_uv/1000;
	pm_chg_vddmax_get(the_chip, &programmed_vdd_max);

	delta_mv =  chip->max_voltage_mv - vbat_batt_terminal_mv;
		
	if (delta_mv > 0) /* meaning we want to increase the vddmax */ {
		if (delta_mv < MIN_DELTA_MV_TO_INCREASE_VDD_MAX) {
			pr_debug("vbat term = %d is not low enough to warrant a increase in vdd_max\n", vbat_batt_terminal_mv);
			return;
		}
	}

	adj_vdd_max_mv = programmed_vdd_max + delta_mv;
	pr_debug("vdd_max needs to be changed by %d mv from %d to %d\n",
			delta_mv,
			programmed_vdd_max,
			adj_vdd_max_mv);

	if (adj_vdd_max_mv < chip->max_voltage_mv) {
		pr_debug("adj vdd_max lower than default max voltage\n");
		return;
	}

	adj_vdd_max_mv = (adj_vdd_max_mv / PM8921_CHG_VDDMAX_RES_MV)
						* PM8921_CHG_VDDMAX_RES_MV;

	if (adj_vdd_max_mv > (chip->max_voltage_mv + vdd_max_increase_mv))
		adj_vdd_max_mv = chip->max_voltage_mv + vdd_max_increase_mv;
	pr_debug("adjusting vdd_max_mv to %d to make "
		"vbat_batt_termial_uv = %d to %d\n",
		adj_vdd_max_mv, vbat_batt_terminal_uv, chip->max_voltage_mv);
	pm_chg_vddmax_set(chip, adj_vdd_max_mv);
}

static void set_appropriate_vbatdet(struct pm8921_chg_chip *chip)
{
	if (chip->is_bat_cool)
		pm_chg_vbatdet_set(the_chip,
			the_chip->cool_bat_voltage
			- the_chip->resume_voltage_delta);
	else if (chip->is_bat_warm)
		pm_chg_vbatdet_set(the_chip,
			the_chip->warm_bat_voltage
			- the_chip->resume_voltage_delta);
	else
		pm_chg_vbatdet_set(the_chip,
			the_chip->max_voltage_mv
			- the_chip->resume_voltage_delta);
}

static void set_appropriate_battery_current(struct pm8921_chg_chip *chip)
{
	unsigned int chg_current = chip->max_bat_chg_current;

	if (chip->is_bat_cool)
		chg_current = min(chg_current, chip->cool_bat_chg_current);

	if (chip->is_bat_warm)
		chg_current = min(chg_current, chip->warm_bat_chg_current);

	if (thermal_mitigation != 0 && chip->thermal_mitigation)
		chg_current = min(chg_current,
				chip->thermal_mitigation[thermal_mitigation]);

	pm_chg_ibatmax_set(the_chip, chg_current);
}

#define TEMP_HYSTERISIS_DECIDEGC 20
static void battery_cool(bool enter)
{
	pr_debug("enter = %d\n", enter);
	if (enter == the_chip->is_bat_cool)
		return;
	the_chip->is_bat_cool = enter;
	if (enter)
		pm_chg_vddmax_set(the_chip, the_chip->cool_bat_voltage);
	else
		pm_chg_vddmax_set(the_chip, the_chip->max_voltage_mv);
	set_appropriate_battery_current(the_chip);
	set_appropriate_vbatdet(the_chip);
}

static void battery_warm(bool enter)
{
	pr_debug("enter = %d\n", enter);
	if (enter == the_chip->is_bat_warm)
		return;
	the_chip->is_bat_warm = enter;
	if (enter)
		pm_chg_vddmax_set(the_chip, the_chip->warm_bat_voltage);
	else
		pm_chg_vddmax_set(the_chip, the_chip->max_voltage_mv);

	set_appropriate_battery_current(the_chip);
	set_appropriate_vbatdet(the_chip);
}

static void check_temp_thresholds(struct pm8921_chg_chip *chip)
{
	int temp = 0;

	temp = get_prop_batt_temp(chip);
	pr_debug("temp = %d, warm_thr_temp = %d, cool_thr_temp = %d\n",
			temp, chip->warm_temp_dc,
			chip->cool_temp_dc);

	if (chip->warm_temp_dc != INT_MIN) {
		if (chip->is_bat_warm
			&& temp < chip->warm_temp_dc - TEMP_HYSTERISIS_DECIDEGC)
			battery_warm(false);
		else if (!chip->is_bat_warm && temp >= chip->warm_temp_dc)
			battery_warm(true);
	}

	if (chip->cool_temp_dc != INT_MIN) {
		if (chip->is_bat_cool
			&& temp > chip->cool_temp_dc + TEMP_HYSTERISIS_DECIDEGC)
			battery_cool(false);
		else if (!chip->is_bat_cool && temp <= chip->cool_temp_dc)
			battery_cool(true);
	}
}

enum {
	CHG_IN_PROGRESS,
	CHG_NOT_IN_PROGRESS,
	CHG_FINISHED,
};

#define VBAT_TOLERANCE_MV	70
#define CHG_DISABLE_MSLEEP	100
static int is_charging_finished(struct pm8921_chg_chip *chip,
			int vbat_batt_terminal_uv, int ichg_meas_ma)
{
	int vbat_programmed, iterm_programmed, vbat_intended;
	int regulation_loop, fast_chg, vcp;
	int rc;
	static int last_vbat_programmed = -EINVAL;

	if (!is_ext_charging(chip)) {
		/* return if the battery is not being fastcharged */
		fast_chg = pm_chg_get_rt_status(chip, FASTCHG_IRQ);
		pr_debug("fast_chg = %d\n", fast_chg);
		if (fast_chg == 0)
			return CHG_NOT_IN_PROGRESS;

		vcp = pm_chg_get_rt_status(chip, VCP_IRQ);
		pr_debug("vcp = %d\n", vcp);
		if (vcp == 1)
			return CHG_IN_PROGRESS;

		/* reset count if battery is hot/cold */
		rc = pm_chg_get_rt_status(chip, BAT_TEMP_OK_IRQ);
		pr_debug("batt_temp_ok = %d\n", rc);
		if (rc == 0)
			return CHG_IN_PROGRESS;

		rc = pm_chg_vddmax_get(chip, &vbat_programmed);
		if (rc) {
			pr_err("couldnt read vddmax rc = %d\n", rc);
			return CHG_IN_PROGRESS;
		}
		pr_debug("vddmax = %d vbat_batt_terminal_uv=%d\n",
			 vbat_programmed, vbat_batt_terminal_uv);

		if (last_vbat_programmed == -EINVAL)
			last_vbat_programmed = vbat_programmed;
		if (last_vbat_programmed !=  vbat_programmed) {
			/* vddmax changed, reset and check again */
			pr_debug("vddmax = %d last_vdd_max=%d\n",
				 vbat_programmed, last_vbat_programmed);
			last_vbat_programmed = vbat_programmed;
			return CHG_IN_PROGRESS;
		}

		if (chip->is_bat_cool)
			vbat_intended = chip->cool_bat_voltage;
		else if (chip->is_bat_warm)
			vbat_intended = chip->warm_bat_voltage;
		else
			vbat_intended = chip->max_voltage_mv;

		if (vbat_batt_terminal_uv / 1000
				< vbat_intended - MIN_DELTA_MV_TO_INCREASE_VDD_MAX) {
			pr_debug("terminal_uv:%d < vbat_intended:%d - hyst:%d\n",
							vbat_batt_terminal_uv,
							vbat_intended,
							MIN_DELTA_MV_TO_INCREASE_VDD_MAX);
			return CHG_IN_PROGRESS;
		}

		regulation_loop = pm_chg_get_regulation_loop(chip);
		if (regulation_loop < 0) {
			pr_err("couldnt read the regulation loop err=%d\n",
				regulation_loop);
			return CHG_IN_PROGRESS;
		}
		pr_debug("regulation_loop=%d\n", regulation_loop);

		if (regulation_loop != 0 && regulation_loop != VDD_LOOP)
			return CHG_IN_PROGRESS;
	} /* !is_ext_charging */

	/* reset count if battery chg current is more than iterm */
	rc = pm_chg_iterm_get(chip, &iterm_programmed);
	if (rc) {
		pr_err("couldnt read iterm rc = %d\n", rc);
		return CHG_IN_PROGRESS;
	}

	pr_debug("iterm_programmed = %d ichg_meas_ma=%d\n",
				iterm_programmed, ichg_meas_ma);
	/*
	 * ichg_meas_ma < 0 means battery is drawing current
	 * ichg_meas_ma > 0 means battery is providing current
	 */
	if (ichg_meas_ma > 0)
		return CHG_IN_PROGRESS;

	if (ichg_meas_ma * -1 > iterm_programmed)
		return CHG_IN_PROGRESS;

	return CHG_FINISHED;
}

/**
 * eoc_worker - internal function to check if battery EOC
 *		has happened
 *
 * If all conditions favouring, if the charge current is
 * less than the term current for three consecutive times
 * an EOC has happened.
 * The wakelock is released if there is no need to reshedule
 * - this happens when the battery is removed or EOC has
 * happened
 */
#define CONSECUTIVE_COUNT	3
static void eoc_worker(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct pm8921_chg_chip *chip = container_of(dwork,
				struct pm8921_chg_chip, eoc_work);
	static int count;
	int end;
	int vbat_meas_uv, vbat_meas_mv;
	int ichg_meas_ua, ichg_meas_ma;
	int vbat_batt_terminal_uv;

	pm_chg_failed_clear(chip, 1);

	pm8921_bms_get_simultaneous_battery_voltage_and_current(
					&ichg_meas_ua,	&vbat_meas_uv);
	vbat_meas_mv = vbat_meas_uv / 1000;
	/* rconn_mohm is in milliOhms */
	ichg_meas_ma = ichg_meas_ua / 1000;
	vbat_batt_terminal_uv = vbat_meas_uv
					+ ichg_meas_ma
					* the_chip->rconn_mohm;

	end = is_charging_finished(chip, vbat_batt_terminal_uv, ichg_meas_ma);

	if (end == CHG_NOT_IN_PROGRESS) {
		count = 0;
		wake_unlock(&chip->eoc_wake_lock);
		return;
	}

	if (end == CHG_FINISHED)
		count++;
	else
		count = 0;

	if (count == CONSECUTIVE_COUNT) {
		count = 0;
		pr_info("End of Charging\n");
		/* set the vbatdet back, in case it was changed
		 * to trigger charging */
		if (chip->is_bat_cool) {
			pm_chg_vbatdet_set(the_chip,
				the_chip->cool_bat_voltage
				- the_chip->resume_voltage_delta);
		} else if (chip->is_bat_warm) {
			pm_chg_vbatdet_set(the_chip,
				the_chip->warm_bat_voltage
				- the_chip->resume_voltage_delta);
		} else {
			pm_chg_vbatdet_set(the_chip,
				the_chip->max_voltage_mv
				- the_chip->resume_voltage_delta);
		}

		pm_chg_auto_enable(chip, 0);
		chip->is_recharging = false;

		if (is_ext_charging(chip))
			chip->ext_charge_done = true;

		if (chip->is_bat_warm || chip->is_bat_cool)
			chip->bms_notify.is_battery_full = 0;
		else
			chip->bms_notify.is_battery_full = 1;
		/* declare end of charging by invoking chgdone interrupt */
		chgdone_irq_handler(chip->pmic_chg_irq[CHGDONE_IRQ], chip);
		wake_unlock(&chip->eoc_wake_lock);
	} else {
		check_temp_thresholds(chip);
		adjust_vdd_max_for_fastchg(chip, vbat_batt_terminal_uv);
		pr_debug("EOC count = %d\n", count);
		schedule_delayed_work(&chip->eoc_work,
			      round_jiffies_relative(msecs_to_jiffies
						     (EOC_CHECK_PERIOD_MS)));
	}
}

/**
 * set_disable_status_param -
 *
 * Internal function to disable battery charging and also disable drawing
 * any current from the source. The device is forced to run on a battery
 * after this.
 */
static int set_disable_status_param(const char *val, struct kernel_param *kp)
{
	int ret;
	struct pm8921_chg_chip *chip = the_chip;

	ret = param_set_int(val, kp);
	if (ret) {
		pr_err("error setting value %d\n", ret);
		return ret;
	}
	pr_info("factory set disable param to %d\n", charging_disabled);
	if (chip) {
		pm_chg_auto_enable(chip, !charging_disabled);
		pm_chg_charge_dis(chip, charging_disabled);
	}
	return 0;
}
module_param_call(disabled, set_disable_status_param, param_get_uint,
					&charging_disabled, 0644);

static int rconn_mohm;
static int set_rconn_mohm(const char *val, struct kernel_param *kp)
{
	int ret;
	struct pm8921_chg_chip *chip = the_chip;

	ret = param_set_int(val, kp);
	if (ret) {
		pr_err("error setting value %d\n", ret);
		return ret;
	}
	if (chip)
		chip->rconn_mohm = rconn_mohm;
	return 0;
}
module_param_call(rconn_mohm, set_rconn_mohm, param_get_uint,
					&rconn_mohm, 0644);
/**
 * set_thermal_mitigation_level -
 *
 * Internal function to control battery charging current to reduce
 * temperature
 */
static int set_therm_mitigation_level(const char *val, struct kernel_param *kp)
{
	int ret;
	struct pm8921_chg_chip *chip = the_chip;

	ret = param_set_int(val, kp);
	if (ret) {
		pr_err("error setting value %d\n", ret);
		return ret;
	}

	if (!chip) {
		pr_err("called before init\n");
		return -EINVAL;
	}

	if (!chip->thermal_mitigation) {
		pr_err("no thermal mitigation\n");
		return -EINVAL;
	}

	if (thermal_mitigation < 0
		|| thermal_mitigation >= chip->thermal_levels) {
		pr_err("out of bound level selected\n");
		return -EINVAL;
	}

	set_appropriate_battery_current(chip);
	return ret;
}
module_param_call(thermal_mitigation, set_therm_mitigation_level,
					param_get_uint,
					&thermal_mitigation, 0644);

static int set_usb_max_current(const char *val, struct kernel_param *kp)
{
	int ret, mA;
	struct pm8921_chg_chip *chip = the_chip;

	ret = param_set_int(val, kp);
	if (ret) {
		pr_err("error setting value %d\n", ret);
		return ret;
	}
	if (chip) {
		pr_warn("setting current max to %d\n", usb_max_current);
		pm_chg_iusbmax_get(chip, &mA);
		if (mA > usb_max_current)
			pm8921_charger_vbus_draw(usb_max_current);
		return 0;
	}
	return -EINVAL;
}
module_param_call(usb_max_current, set_usb_max_current,
	param_get_uint, &usb_max_current, 0644);

static void free_irqs(struct pm8921_chg_chip *chip)
{
	int i;

	for (i = 0; i < PM_CHG_MAX_INTS; i++)
		if (chip->pmic_chg_irq[i]) {
			free_irq(chip->pmic_chg_irq[i], chip);
			chip->pmic_chg_irq[i] = 0;
		}
}

/* determines the initial present states */
static void __devinit determine_initial_state(struct pm8921_chg_chip *chip)
{
	int fsm_state;
	int is_fast_chg;

	chip->dc_present = !!is_dc_chg_plugged_in(chip);
	chip->usb_present = !!is_usb_chg_plugged_in(chip);

#ifdef VBUS_INTERRUPT_THROUGH_PMIC
	notify_usb_of_the_plugin_event(chip->usb_present);
#endif
	if (chip->usb_present) {
		schedule_delayed_work(&chip->unplug_check_work,
			msecs_to_jiffies(UNPLUG_CHECK_WAIT_PERIOD_MS));
		pm8921_chg_enable_irq(chip, CHG_GONE_IRQ);
	}

	pm8921_chg_enable_irq(chip, DCIN_VALID_IRQ);
	pm8921_chg_enable_irq(chip, USBIN_VALID_IRQ);
	pm8921_chg_enable_irq(chip, BATT_REMOVED_IRQ);
	pm8921_chg_enable_irq(chip, BATT_INSERTED_IRQ);
	pm8921_chg_enable_irq(chip, DCIN_OV_IRQ);
	pm8921_chg_enable_irq(chip, DCIN_UV_IRQ);
	pm8921_chg_enable_irq(chip, CHGFAIL_IRQ);
	pm8921_chg_enable_irq(chip, FASTCHG_IRQ);
	/* pm8921_chg_enable_irq(chip, VBATDET_LOW_IRQ); */
	pm8921_chg_enable_irq(chip, BAT_TEMP_OK_IRQ);

	if (get_prop_batt_present(the_chip) || is_dc_chg_plugged_in(the_chip))
		if (usb_chg_current)
			/*
			 * Reissue a vbus draw call only if a battery
			 * or DC is present. We don't want to brown out the
			 * device if usb is its only source
			 */
			__pm8921_charger_vbus_draw(usb_chg_current);
	usb_chg_current = 0;

	/*
	 * The bootloader could have started charging, a fastchg interrupt
	 * might not happen. Check the real time status and if it is fast
	 * charging invoke the handler so that the eoc worker could be
	 * started
	 */
	is_fast_chg = pm_chg_get_rt_status(chip, FASTCHG_IRQ);
	if (is_fast_chg)
		fastchg_irq_handler(chip->pmic_chg_irq[FASTCHG_IRQ], chip);

	fsm_state = pm_chg_get_fsm_state(chip);
	if (is_battery_charging(fsm_state)) {
		chip->bms_notify.is_charging = 1;
		pm8921_bms_charging_began();
	}

	check_battery_valid(chip);

	pr_debug("usb = %d, dc = %d  batt = %d state=%d\n",
			chip->usb_present,
			chip->dc_present,
			get_prop_batt_present(chip),
			fsm_state);

	/* Determine which USB trim column to use */
	if (pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8917)
		chip->usb_trim_table = usb_trim_8917_table;
	else if (pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8038)
		chip->usb_trim_table = usb_trim_8038_table;
}

struct pm_chg_irq_init_data {
	unsigned int	irq_id;
	char		*name;
	unsigned long	flags;
	irqreturn_t	(*handler)(int, void *);
};

#define CHG_IRQ(_id, _flags, _handler) \
{ \
	.irq_id		= _id, \
	.name		= #_id, \
	.flags		= _flags, \
	.handler	= _handler, \
}
struct pm_chg_irq_init_data chg_irq_data[] = {
	CHG_IRQ(USBIN_VALID_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						usbin_valid_irq_handler),
	CHG_IRQ(BATT_INSERTED_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						batt_inserted_irq_handler),
	CHG_IRQ(VBATDET_LOW_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						vbatdet_low_irq_handler),
	CHG_IRQ(CHGWDOG_IRQ, IRQF_TRIGGER_RISING, chgwdog_irq_handler),
	CHG_IRQ(VCP_IRQ, IRQF_TRIGGER_RISING, vcp_irq_handler),
	CHG_IRQ(ATCDONE_IRQ, IRQF_TRIGGER_RISING, atcdone_irq_handler),
	CHG_IRQ(ATCFAIL_IRQ, IRQF_TRIGGER_RISING, atcfail_irq_handler),
	CHG_IRQ(CHGDONE_IRQ, IRQF_TRIGGER_RISING, chgdone_irq_handler),
	CHG_IRQ(CHGFAIL_IRQ, IRQF_TRIGGER_RISING, chgfail_irq_handler),
	CHG_IRQ(CHGSTATE_IRQ, IRQF_TRIGGER_RISING, chgstate_irq_handler),
	CHG_IRQ(LOOP_CHANGE_IRQ, IRQF_TRIGGER_RISING, loop_change_irq_handler),
	CHG_IRQ(FASTCHG_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						fastchg_irq_handler),
	CHG_IRQ(TRKLCHG_IRQ, IRQF_TRIGGER_RISING, trklchg_irq_handler),
	CHG_IRQ(BATT_REMOVED_IRQ, IRQF_TRIGGER_RISING,
						batt_removed_irq_handler),
	CHG_IRQ(BATTTEMP_HOT_IRQ, IRQF_TRIGGER_RISING,
						batttemp_hot_irq_handler),
	CHG_IRQ(CHGHOT_IRQ, IRQF_TRIGGER_RISING, chghot_irq_handler),
	CHG_IRQ(BATTTEMP_COLD_IRQ, IRQF_TRIGGER_RISING,
						batttemp_cold_irq_handler),
	CHG_IRQ(CHG_GONE_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						chg_gone_irq_handler),
	CHG_IRQ(BAT_TEMP_OK_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						bat_temp_ok_irq_handler),
	CHG_IRQ(COARSE_DET_LOW_IRQ, IRQF_TRIGGER_RISING,
						coarse_det_low_irq_handler),
	CHG_IRQ(VDD_LOOP_IRQ, IRQF_TRIGGER_RISING, vdd_loop_irq_handler),
	CHG_IRQ(VREG_OV_IRQ, IRQF_TRIGGER_RISING, vreg_ov_irq_handler),
	CHG_IRQ(VBATDET_IRQ, IRQF_TRIGGER_RISING, vbatdet_irq_handler),
	CHG_IRQ(BATFET_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						batfet_irq_handler),
	CHG_IRQ(DCIN_VALID_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						dcin_valid_irq_handler),
	CHG_IRQ(DCIN_OV_IRQ, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						dcin_ov_irq_handler),
	CHG_IRQ(DCIN_UV_IRQ, IRQF_TRIGGER_RISING, dcin_uv_irq_handler),
};

static int __devinit request_irqs(struct pm8921_chg_chip *chip,
					struct platform_device *pdev)
{
	struct resource *res;
	int ret, i;

	ret = 0;
	bitmap_fill(chip->enabled_irqs, PM_CHG_MAX_INTS);

	for (i = 0; i < ARRAY_SIZE(chg_irq_data); i++) {
		res = platform_get_resource_byname(pdev, IORESOURCE_IRQ,
				chg_irq_data[i].name);
		if (res == NULL) {
			pr_err("couldn't find %s\n", chg_irq_data[i].name);
			goto err_out;
		}
		chip->pmic_chg_irq[chg_irq_data[i].irq_id] = res->start;
		ret = request_irq(res->start, chg_irq_data[i].handler,
			chg_irq_data[i].flags,
			chg_irq_data[i].name, chip);
		if (ret < 0) {
			pr_err("couldn't request %d (%s) %d\n", res->start,
					chg_irq_data[i].name, ret);
			chip->pmic_chg_irq[chg_irq_data[i].irq_id] = 0;
			goto err_out;
		}
		pm8921_chg_disable_irq(chip, chg_irq_data[i].irq_id);
	}
	return 0;

err_out:
	free_irqs(chip);
	return -EINVAL;
}

static void pm8921_chg_force_19p2mhz_clk(struct pm8921_chg_chip *chip)
{
	int err;
	u8 temp;

	temp  = 0xD1;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD3;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD1;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD5;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	udelay(183);

	temp  = 0xD1;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD0;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}
	udelay(32);

	temp  = 0xD1;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD3;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}
}

static void pm8921_chg_set_hw_clk_switching(struct pm8921_chg_chip *chip)
{
	int err;
	u8 temp;

	temp  = 0xD1;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}

	temp  = 0xD0;
	err = pm8xxx_writeb(chip->dev->parent, CHG_TEST, temp);
	if (err) {
		pr_err("Error %d writing %d to addr %d\n", err, temp, CHG_TEST);
		return;
	}
}

#define VREF_BATT_THERM_FORCE_ON	BIT(7)
static void detect_battery_removal(struct pm8921_chg_chip *chip)
{
	u8 temp;

	pm8xxx_readb(chip->dev->parent, CHG_CNTRL, &temp);
	pr_debug("upon restart CHG_CNTRL = 0x%x\n",  temp);

	if (!(temp & VREF_BATT_THERM_FORCE_ON))
		/*
		 * batt therm force on bit is battery backed and is default 0
		 * The charger sets this bit at init time. If this bit is found
		 * 0 that means the battery was removed. Tell the bms about it
		 */
		pm8921_bms_invalidate_shutdown_soc();
}

#define ENUM_TIMER_STOP_BIT	BIT(1)
#define BOOT_DONE_BIT		BIT(6)
#define CHG_BATFET_ON_BIT	BIT(3)
#define CHG_VCP_EN		BIT(0)
#define CHG_BAT_TEMP_DIS_BIT	BIT(2)
#define SAFE_CURRENT_MA		1500
static int __devinit pm8921_chg_hw_init(struct pm8921_chg_chip *chip)
{
	int rc;
	int vdd_safe;

	/* forcing 19p2mhz before accessing any charger registers */
	pm8921_chg_force_19p2mhz_clk(chip);

	detect_battery_removal(chip);

	rc = pm_chg_masked_write(chip, SYS_CONFIG_2,
					BOOT_DONE_BIT, BOOT_DONE_BIT);
	if (rc) {
		pr_err("Failed to set BOOT_DONE_BIT rc=%d\n", rc);
		return rc;
	}

	vdd_safe = chip->max_voltage_mv + VDD_MAX_INCREASE_MV;

	if (vdd_safe > PM8921_CHG_VDDSAFE_MAX)
		vdd_safe = PM8921_CHG_VDDSAFE_MAX;

	rc = pm_chg_vddsafe_set(chip, vdd_safe);

	if (rc) {
		pr_err("Failed to set safe voltage to %d rc=%d\n",
						chip->max_voltage_mv, rc);
		return rc;
	}
	rc = pm_chg_vbatdet_set(chip,
				chip->max_voltage_mv
				- chip->resume_voltage_delta);
	if (rc) {
		pr_err("Failed to set vbatdet comprator voltage to %d rc=%d\n",
			chip->max_voltage_mv - chip->resume_voltage_delta, rc);
		return rc;
	}

	rc = pm_chg_vddmax_set(chip, chip->max_voltage_mv);
	if (rc) {
		pr_err("Failed to set max voltage to %d rc=%d\n",
						chip->max_voltage_mv, rc);
		return rc;
	}
	rc = pm_chg_ibatsafe_set(chip, SAFE_CURRENT_MA);
	if (rc) {
		pr_err("Failed to set max voltage to %d rc=%d\n",
						SAFE_CURRENT_MA, rc);
		return rc;
	}

	rc = pm_chg_ibatmax_set(chip, chip->max_bat_chg_current);
	if (rc) {
		pr_err("Failed to set max current to 400 rc=%d\n", rc);
		return rc;
	}

	rc = pm_chg_iterm_set(chip, chip->term_current);
	if (rc) {
		pr_err("Failed to set term current to %d rc=%d\n",
						chip->term_current, rc);
		return rc;
	}

	/* Disable the ENUM TIMER */
	rc = pm_chg_masked_write(chip, PBL_ACCESS2, ENUM_TIMER_STOP_BIT,
			ENUM_TIMER_STOP_BIT);
	if (rc) {
		pr_err("Failed to set enum timer stop rc=%d\n", rc);
		return rc;
	}

	if (chip->safety_time != 0) {
		rc = pm_chg_tchg_max_set(chip, chip->safety_time);
		if (rc) {
			pr_err("Failed to set max time to %d minutes rc=%d\n",
							chip->safety_time, rc);
			return rc;
		}
	}

	if (chip->ttrkl_time != 0) {
		rc = pm_chg_ttrkl_max_set(chip, chip->ttrkl_time);
		if (rc) {
			pr_err("Failed to set trkl time to %d minutes rc=%d\n",
							chip->safety_time, rc);
			return rc;
		}
	}

	if (chip->vin_min != 0) {
		rc = pm_chg_vinmin_set(chip, chip->vin_min);
		if (rc) {
			pr_err("Failed to set vin min to %d mV rc=%d\n",
							chip->vin_min, rc);
			return rc;
		}
	} else {
		chip->vin_min = pm_chg_vinmin_get(chip);
	}

	rc = pm_chg_disable_wd(chip);
	if (rc) {
		pr_err("Failed to disable wd rc=%d\n", rc);
		return rc;
	}

	/*BTM(BAT_THM) disable*/
	rc = pm_chg_masked_write(chip, CHG_CNTRL_2,
			CHG_BAT_TEMP_DIS_BIT, CHG_BAT_TEMP_DIS_BIT);
	if (rc) {
		pr_err("Failed to enable temp control chg rc=%d\n", rc);
		return rc;
	}
	/* switch to a 3.2Mhz for the buck */
	rc = pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CLOCK_CTRL, 0x13);
	if (rc) {
		pr_err("Failed to switch buck clk rc=%d\n", rc);
		return rc;
	}

	if (chip->trkl_voltage != 0) {
		rc = pm_chg_vtrkl_low_set(chip, chip->trkl_voltage);
		if (rc) {
			pr_err("Failed to set trkl voltage to %dmv  rc=%d\n",
							chip->trkl_voltage, rc);
			return rc;
		}
	}

	if (chip->weak_voltage != 0) {
		rc = pm_chg_vweak_set(chip, chip->weak_voltage);
		if (rc) {
			pr_err("Failed to set weak voltage to %dmv  rc=%d\n",
							chip->weak_voltage, rc);
			return rc;
		}
	}

	if (chip->trkl_current != 0) {
		rc = pm_chg_itrkl_set(chip, chip->trkl_current);
		if (rc) {
			pr_err("Failed to set trkl current to %dmA  rc=%d\n",
							chip->trkl_current, rc);
			return rc;
		}
	}

	if (chip->weak_current != 0) {
		rc = pm_chg_iweak_set(chip, chip->weak_current);
		if (rc) {
			pr_err("Failed to set weak current to %dmA  rc=%d\n",
							chip->weak_current, rc);
			return rc;
		}
	}

	rc = pm_chg_batt_cold_temp_config(chip, chip->cold_thr);
	if (rc) {
		pr_err("Failed to set cold config %d  rc=%d\n",
						chip->cold_thr, rc);
	}

	rc = pm_chg_batt_hot_temp_config(chip, chip->hot_thr);
	if (rc) {
		pr_err("Failed to set hot config %d  rc=%d\n",
						chip->hot_thr, rc);
	}

	rc = pm_chg_led_src_config(chip, chip->led_src_config);
	if (rc) {
		pr_err("Failed to set charger LED src config %d  rc=%d\n",
						chip->led_src_config, rc);
	}

	/* Workarounds for die 3.0 */
	if (pm8xxx_get_revision(chip->dev->parent) == PM8XXX_REVISION_8921_3p0
	&& pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8921)
		pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, 0xAC);

	/* Enable isub_fine resolution AICL for PM8917 */
	if (pm8xxx_get_version(chip->dev->parent) == PM8XXX_VERSION_8917) {
#ifdef CONFIG_SEC_DEBUG_SUBSYS
		rc = pm8xxx_readb(chip->dev->parent, OVP_USB_UVD, &uvd_thresh);
		if (rc) {
			pr_err("read failed: addr=%03X, rc=%d\n",
				OVP_USB_UVD, rc);
			return rc;
		}
		pr_info("[PMIC 8917] Get UVD threshlod(%d)\n", uvd_thresh);
//		sec_pmic_get_revision();
#endif
		/* Set PM8917 USB_OVP debounce time to 15 ms */
		rc = pm_chg_masked_write(chip, USB_OVP_CONTROL,
			OVP_DEBOUNCE_TIME, 0x6);
		if (rc) {
			pr_err("Failed to set USB OVP db rc=%d\n", rc);
			return rc;
		}

		/* Enable isub_fine resolution AICL for PM8917 */
		chip->iusb_fine_res = true;
		if (chip->uvd_voltage_mv)
			rc = pm_chg_uvd_threshold_set(chip,
					chip->uvd_voltage_mv);
			if (rc) {
				pr_err("Failed to set UVD threshold %drc=%d\n",
						chip->uvd_voltage_mv, rc);
			return rc;
		}
	}

	pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, 0xD9);

	/* Disable EOC FSM processing */
	pm8xxx_writeb(chip->dev->parent, CHG_BUCK_CTRL_TEST3, 0x91);

	rc = pm_chg_masked_write(chip, CHG_CNTRL, VREF_BATT_THERM_FORCE_ON,
						VREF_BATT_THERM_FORCE_ON);
	if (rc)
		pr_err("Failed to Force Vref therm rc=%d\n", rc);

	rc = pm_chg_charge_dis(chip, charging_disabled);
	if (rc) {
		pr_err("Failed to disable CHG_CHARGE_DIS bit rc=%d\n", rc);
		return rc;
	}

	rc = pm_chg_auto_enable(chip, !charging_disabled);
	if (rc) {
		pr_err("Failed to enable charging rc=%d\n", rc);
		return rc;
	}

	pm8921_usb_ovp_disable(1);

	return 0;
}

static int get_rt_status(void *data, u64 *val)
{
	int i = (int)data;
	int ret;

	/* global irq number is passed in via data */
	ret = pm_chg_get_rt_status(the_chip, i);
	*val = ret;
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(rt_fops, get_rt_status, NULL, "%llu\n");

static int get_fsm_status(void *data, u64 *val)
{
	u8 temp;

	temp = pm_chg_get_fsm_state(the_chip);
	*val = temp;
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(fsm_fops, get_fsm_status, NULL, "%llu\n");

static int get_reg_loop(void *data, u64 *val)
{
	u8 temp;

	if (!the_chip) {
		pr_err("%s called before init\n", __func__);
		return -EINVAL;
	}
	temp = pm_chg_get_regulation_loop(the_chip);
	*val = temp;
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(reg_loop_fops, get_reg_loop, NULL, "0x%02llx\n");

static int get_reg(void *data, u64 *val)
{
	int addr = (int)data;
	int ret;
	u8 temp;

	ret = pm8xxx_readb(the_chip->dev->parent, addr, &temp);
	if (ret) {
		pr_err("pm8xxx_readb to %x value =%d errored = %d\n",
			addr, temp, ret);
		return -EAGAIN;
	}
	*val = temp;
	return 0;
}

static int set_reg(void *data, u64 val)
{
	int addr = (int)data;
	int ret;
	u8 temp;

	temp = (u8) val;
	ret = pm8xxx_writeb(the_chip->dev->parent, addr, temp);
	if (ret) {
		pr_err("pm8xxx_writeb to %x value =%d errored = %d\n",
			addr, temp, ret);
		return -EAGAIN;
	}
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(reg_fops, get_reg, set_reg, "0x%02llx\n");

enum {
	BAT_WARM_ZONE,
	BAT_COOL_ZONE,
};
static int get_warm_cool(void *data, u64 *val)
{
	if (!the_chip) {
		pr_err("%s called before init\n", __func__);
		return -EINVAL;
	}
	if ((int)data == BAT_WARM_ZONE)
		*val = the_chip->is_bat_warm;
	if ((int)data == BAT_COOL_ZONE)
		*val = the_chip->is_bat_cool;
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(warm_cool_fops, get_warm_cool, NULL, "0x%lld\n");

static void create_debugfs_entries(struct pm8921_chg_chip *chip)
{
	int i;

	chip->dent = debugfs_create_dir("pm8921_chg", NULL);

	if (IS_ERR(chip->dent)) {
		pr_err("pmic charger couldnt create debugfs dir\n");
		return;
	}

	debugfs_create_file("CHG_CNTRL", 0644, chip->dent,
			    (void *)CHG_CNTRL, &reg_fops);
	debugfs_create_file("CHG_CNTRL_2", 0644, chip->dent,
			    (void *)CHG_CNTRL_2, &reg_fops);
	debugfs_create_file("CHG_CNTRL_3", 0644, chip->dent,
			    (void *)CHG_CNTRL_3, &reg_fops);
	debugfs_create_file("PBL_ACCESS1", 0644, chip->dent,
			    (void *)PBL_ACCESS1, &reg_fops);
	debugfs_create_file("PBL_ACCESS2", 0644, chip->dent,
			    (void *)PBL_ACCESS2, &reg_fops);
	debugfs_create_file("SYS_CONFIG_1", 0644, chip->dent,
			    (void *)SYS_CONFIG_1, &reg_fops);
	debugfs_create_file("SYS_CONFIG_2", 0644, chip->dent,
			    (void *)SYS_CONFIG_2, &reg_fops);
	debugfs_create_file("CHG_VDD_MAX", 0644, chip->dent,
			    (void *)CHG_VDD_MAX, &reg_fops);
	debugfs_create_file("CHG_VDD_SAFE", 0644, chip->dent,
			    (void *)CHG_VDD_SAFE, &reg_fops);
	debugfs_create_file("CHG_VBAT_DET", 0644, chip->dent,
			    (void *)CHG_VBAT_DET, &reg_fops);
	debugfs_create_file("CHG_IBAT_MAX", 0644, chip->dent,
			    (void *)CHG_IBAT_MAX, &reg_fops);
	debugfs_create_file("CHG_IBAT_SAFE", 0644, chip->dent,
			    (void *)CHG_IBAT_SAFE, &reg_fops);
	debugfs_create_file("CHG_VIN_MIN", 0644, chip->dent,
			    (void *)CHG_VIN_MIN, &reg_fops);
	debugfs_create_file("CHG_VTRICKLE", 0644, chip->dent,
			    (void *)CHG_VTRICKLE, &reg_fops);
	debugfs_create_file("CHG_ITRICKLE", 0644, chip->dent,
			    (void *)CHG_ITRICKLE, &reg_fops);
	debugfs_create_file("CHG_ITERM", 0644, chip->dent,
			    (void *)CHG_ITERM, &reg_fops);
	debugfs_create_file("CHG_TCHG_MAX", 0644, chip->dent,
			    (void *)CHG_TCHG_MAX, &reg_fops);
	debugfs_create_file("CHG_TWDOG", 0644, chip->dent,
			    (void *)CHG_TWDOG, &reg_fops);
	debugfs_create_file("CHG_TEMP_THRESH", 0644, chip->dent,
			    (void *)CHG_TEMP_THRESH, &reg_fops);
	debugfs_create_file("CHG_COMP_OVR", 0644, chip->dent,
			    (void *)CHG_COMP_OVR, &reg_fops);
	debugfs_create_file("CHG_BUCK_CTRL_TEST1", 0644, chip->dent,
			    (void *)CHG_BUCK_CTRL_TEST1, &reg_fops);
	debugfs_create_file("CHG_BUCK_CTRL_TEST2", 0644, chip->dent,
			    (void *)CHG_BUCK_CTRL_TEST2, &reg_fops);
	debugfs_create_file("CHG_BUCK_CTRL_TEST3", 0644, chip->dent,
			    (void *)CHG_BUCK_CTRL_TEST3, &reg_fops);
	debugfs_create_file("CHG_TEST", 0644, chip->dent,
			    (void *)CHG_TEST, &reg_fops);

	debugfs_create_file("FSM_STATE", 0644, chip->dent, NULL,
			    &fsm_fops);

	debugfs_create_file("REGULATION_LOOP_CONTROL", 0644, chip->dent, NULL,
			    &reg_loop_fops);

	debugfs_create_file("BAT_WARM_ZONE", 0644, chip->dent,
				(void *)BAT_WARM_ZONE, &warm_cool_fops);
	debugfs_create_file("BAT_COOL_ZONE", 0644, chip->dent,
				(void *)BAT_COOL_ZONE, &warm_cool_fops);

	for (i = 0; i < ARRAY_SIZE(chg_irq_data); i++) {
		if (chip->pmic_chg_irq[chg_irq_data[i].irq_id])
			debugfs_create_file(chg_irq_data[i].name, 0444,
				chip->dent,
				(void *)chg_irq_data[i].irq_id,
				&rt_fops);
	}
}

static int pm8921_charger_suspend_noirq(struct device *dev)
{
	int rc;
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	rc = pm_chg_masked_write(chip, CHG_CNTRL, VREF_BATT_THERM_FORCE_ON, 0);
	if (rc)
		pr_err("Failed to Force Vref therm off rc=%d\n", rc);
	pm8921_chg_set_hw_clk_switching(chip);
	return 0;
}

static int pm8921_charger_resume_noirq(struct device *dev)
{
	int rc;
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	pm8921_chg_force_19p2mhz_clk(chip);

	rc = pm_chg_masked_write(chip, CHG_CNTRL, VREF_BATT_THERM_FORCE_ON,
						VREF_BATT_THERM_FORCE_ON);
	if (rc)
		pr_err("Failed to Force Vref therm on rc=%d\n", rc);
	return 0;
}

static int pm8921_charger_resume(struct device *dev)
{
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	if (pm8921_chg_is_enabled(chip, LOOP_CHANGE_IRQ)) {
		disable_irq_wake(chip->pmic_chg_irq[LOOP_CHANGE_IRQ]);
		pm8921_chg_disable_irq(chip, LOOP_CHANGE_IRQ);
	}

	chip->initial_update_of_soc = true;
	return 0;
}

static int pm8921_charger_suspend(struct device *dev)
{
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	if (is_usb_chg_plugged_in(chip)) {
		pm8921_chg_enable_irq(chip, LOOP_CHANGE_IRQ);
		enable_irq_wake(chip->pmic_chg_irq[LOOP_CHANGE_IRQ]);
	}

	return 0;
}

static int pm8921_charger_prepare(struct device *dev)
{
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	pr_debug("%s start\n", __func__);

	chip->is_in_sleep = true;

	alarm_cancel(&chip->update_alarm);
	cancel_delayed_work_sync(&chip->update_heartbeat_work);

	if (chip->recent_reported_soc <= chip->batt_pdata->poweroff_check_soc ||
		((chip->ui_term_cnt > 0) &&
		chip->batt_status != POWER_SUPPLY_STATUS_FULL)) {
		pr_info("%s battery short update time !!\n", __func__);
		pm_program_alarm(chip, chip->update_time / 1000);
	} else
		pm_program_alarm(chip, chip->sleep_update_time);

	pr_debug("%s end\n", __func__);

	return 0;
}

static void pm8921_charger_complete(struct device *dev)
{
	struct pm8921_chg_chip *chip = dev_get_drvdata(dev);

	pr_debug("%s start\n", __func__);

	alarm_cancel(&chip->update_alarm);

	wake_lock(&chip->monitor_wake_lock);
	schedule_delayed_work(&chip->update_heartbeat_work, 0);

	pr_debug("%s end\n", __func__);

	return;
}

#if defined(CONFIG_WIRELESS_CHARGING)
static irqreturn_t wpc_charger_irq(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	int wc_w_state;
	union power_supply_propval value;

	wc_w_state = !gpio_get_value_cansleep(chip->wc_w_gpio);

	if ((chip->wc_w_state == 0) && (wc_w_state == 1) &&
			(chip->charging_enabled == 0)) {
		value.intval = POWER_SUPPLY_TYPE_WPC;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		pr_debug("%s: wpc activated, set V_INT as PN\n",
				__func__);
	} else if ((chip->wc_w_state == 1) && (wc_w_state == 0) &&
			(chip->cable_type == CABLE_TYPE_WPC)) {
		value.intval =
			POWER_SUPPLY_TYPE_BATTERY;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		pr_debug("%s: wpc deactivated, set V_INT as PD\n",
				__func__);
	}
	pr_debug("%s: w(%d to %d)\n", __func__,
			chip->wc_w_state, wc_w_state);
	chip->wc_w_state = wc_w_state;

	return IRQ_HANDLED;
}
#endif
static irqreturn_t batt_int_irq(int irq, void *data)
{
	struct pm8921_chg_chip *chip = data;
	
	chip->batt_present = !gpio_get_value_cansleep(GPIO_BATT_INT);
	pr_info("charging enabled : %d, batt_present: %d\n",
			chip->charging_enabled, chip->batt_present);
	if (chip->charging_enabled && !chip->batt_present)
		pm_chg_usb_suspend_enable(chip, 1);

	return IRQ_HANDLED;
}

static int __devinit pm8921_charger_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct pm8921_chg_chip *chip;
	const struct pm8921_charger_platform_data *pdata
				= pdev->dev.platform_data;

	if (!pdata) {
		pr_err("missing platform data\n");
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct pm8921_chg_chip),
					GFP_KERNEL);
	if (!chip) {
		pr_err("Cannot allocate pm_chg_chip\n");
		return -ENOMEM;
	}

	chip->batt_pdata = pdata->batt_pdata;

	chip->dev = &pdev->dev;
	chip->safety_time = pdata->safety_time;
	chip->ttrkl_time = pdata->ttrkl_time;
	chip->update_time = pdata->update_time;
	chip->sleep_update_time = pdata->sleep_update_time;
	chip->max_voltage_mv = pdata->max_voltage;
	chip->alarm_low_mv = pdata->alarm_low_mv;
	chip->alarm_high_mv = pdata->alarm_high_mv;
	chip->min_voltage_mv = pdata->min_voltage;
	chip->uvd_voltage_mv = pdata->uvd_thresh_voltage;
	chip->resume_voltage_delta = pdata->resume_voltage_delta;
	chip->resume_charge_percent = pdata->resume_charge_percent;
	chip->term_current = pdata->term_current;
	chip->vbat_channel = pdata->charger_cdata.vbat_channel;
	chip->batt_temp_channel = pdata->charger_cdata.batt_temp_channel;
	chip->batt_id_channel = pdata->charger_cdata.batt_id_channel;
	chip->batt_id_min = pdata->batt_id_min;
	chip->batt_id_max = pdata->batt_id_max;
	if (pdata->cool_temp != INT_MIN)
		chip->cool_temp_dc = pdata->cool_temp * 10;
	else
		chip->cool_temp_dc = INT_MIN;

	if (pdata->warm_temp != INT_MIN)
		chip->warm_temp_dc = pdata->warm_temp * 10;
	else
		chip->warm_temp_dc = INT_MIN;

	chip->temp_check_period = pdata->temp_check_period;
	chip->dc_unplug_check = pdata->dc_unplug_check;
	chip->max_bat_chg_current = pdata->max_bat_chg_current;
	/* Assign to corresponding module parameter */
	usb_max_current = pdata->usb_max_current;
	chip->cool_bat_chg_current = pdata->cool_bat_chg_current;
	chip->warm_bat_chg_current = pdata->warm_bat_chg_current;
	chip->cool_bat_voltage = pdata->cool_bat_voltage;
	chip->warm_bat_voltage = pdata->warm_bat_voltage;
	chip->trkl_voltage = pdata->trkl_voltage;
	chip->weak_voltage = pdata->weak_voltage;
	chip->trkl_current = pdata->trkl_current;
	chip->weak_current = pdata->weak_current;
	chip->vin_min = pdata->vin_min;
	chip->thermal_mitigation = pdata->thermal_mitigation;
	chip->thermal_levels = pdata->thermal_levels;

	chip->cold_thr = pdata->cold_thr;
	chip->hot_thr = pdata->hot_thr;
	chip->rconn_mohm = pdata->rconn_mohm;
	chip->led_src_config = pdata->led_src_config;
	chip->has_dc_supply = pdata->has_dc_supply;

	chip->batt_health = POWER_SUPPLY_HEALTH_GOOD;
	chip->charging_start_time = 0;
	chip->charging_passed_time = 0;
	chip->is_recharging = false;
	chip->is_chgtime_expired = false;
	chip->health_cnt = 0;
	chip->slate_mode = false;
	chip->factory_mode = false;
	chip->siop_level = 100;

	chip->initial_count = 3;
	chip->capacity_max = chip->batt_pdata->capacity_max;
	chip->initial_update_of_soc = true;
	chip->is_in_sleep = false;
	chip->ui_term_cnt = 0;
	chip->recharging_cnt = 0;
	chip->batt_present = 1;
	chip->boot_completed = false;
	chip->cable_exception = CABLE_TYPE_NONE;

	wake_lock_init(&chip->monitor_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-charger-monitor");
	wake_lock_init(&chip->cable_wake_lock, WAKE_LOCK_SUSPEND,
		       "sec-charger-cable");

	alarm_init(&chip->event_termination_alarm,
			ANDROID_ALARM_ELAPSED_REALTIME_WAKEUP,
			sec_bat_event_expired_timer_func);

	if (pdata->get_board_rev) {
		chip->hw_rev = pdata->get_board_rev();
		pr_info("%s : board_rev(%d)\n", __func__, chip->hw_rev);
	} else {
		chip->hw_rev = 1;
	}

	pr_info("battery(PM8917) hw init, board_rev(%d)\n", chip->hw_rev);
	rc = pm8921_chg_hw_init(chip);
	if (rc) {
		pr_err("couldn't init hardware rc=%d\n", rc);
		goto free_chip;
	}
	pr_info("battery(PM8917) register supply start\n");
	chip->usb_psy.name = "usb";
	chip->usb_psy.type = POWER_SUPPLY_TYPE_USB;
	chip->usb_psy.supplied_to = pm_power_supplied_to;
	chip->usb_psy.num_supplicants = ARRAY_SIZE(pm_power_supplied_to);
	chip->usb_psy.properties = pm_power_props_usb;
	chip->usb_psy.num_properties = ARRAY_SIZE(pm_power_props_usb);
	chip->usb_psy.get_property = pm_power_get_property_usb;
	chip->usb_psy.set_property = pm_power_set_property_usb;
	chip->usb_psy.property_is_writeable = usb_property_is_writeable;

	chip->dc_psy.name = "ac";
	chip->dc_psy.type = POWER_SUPPLY_TYPE_MAINS;
	chip->dc_psy.supplied_to = pm_power_supplied_to;
	chip->dc_psy.num_supplicants = ARRAY_SIZE(pm_power_supplied_to);
	chip->dc_psy.properties = pm_power_props_mains;
	chip->dc_psy.num_properties = ARRAY_SIZE(pm_power_props_mains);
	chip->dc_psy.get_property = pm_power_get_property_mains;

	chip->batt_psy.name = "battery";
	chip->batt_psy.type = POWER_SUPPLY_TYPE_BATTERY;
	chip->batt_psy.properties = msm_batt_power_props;
	chip->batt_psy.num_properties = ARRAY_SIZE(msm_batt_power_props);
	chip->batt_psy.get_property = pm_batt_power_get_property;
	chip->batt_psy.set_property = pm_batt_power_set_property;
	chip->batt_psy.external_power_changed = pm_batt_external_power_changed;

	chip->fg_psy.name = "fuelgauge";
	chip->fg_psy.properties = msm_fg_power_props;
	chip->fg_psy.num_properties = ARRAY_SIZE(msm_fg_power_props);
	chip->fg_psy.type = POWER_SUPPLY_TYPE_UNKNOWN;
	chip->fg_psy.get_property = pm_fg_power_get_property;
	chip->get_lpm_mode = pdata->get_lpm_mode;
	chip->wc_w_gpio = pdata->wc_w_gpio;
	chip->wpc_acok =  pdata->wpc_acok;

	rc = power_supply_register(chip->dev, &chip->usb_psy);
	if (rc < 0) {
		pr_err("power_supply_register usb failed rc = %d\n", rc);
		goto free_chip;
	}

	rc = power_supply_register(chip->dev, &chip->dc_psy);
	if (rc < 0) {
		pr_err("power_supply_register usb failed rc = %d\n", rc);
		goto unregister_usb;
	}

	rc = power_supply_register(chip->dev, &chip->batt_psy);
	if (rc < 0) {
		pr_err("power_supply_register batt failed rc = %d\n", rc);
		goto unregister_fg;
	}

	rc = power_supply_register(chip->dev, &chip->fg_psy);
	if (rc < 0) {
		pr_err("power_supply_register batt failed rc = %d\n", rc);
		goto unregister_fg;
	}

	/* [101712] temporary codes until common battery codes are applied */
	sec_bat_create_attrs(chip->batt_psy.dev);
	sec_fg_create_attrs(chip->fg_psy.dev);
	platform_set_drvdata(pdev, chip);
	the_chip = chip;
	pr_info("battery(PM8917) register supply end\n");

	wake_lock_init(&chip->eoc_wake_lock, WAKE_LOCK_SUSPEND, "pm8921_eoc");
	INIT_DELAYED_WORK(&chip->eoc_work, eoc_worker);
	INIT_DELAYED_WORK(&chip->vin_collapse_check_work,
						vin_collapse_check_worker);
	INIT_DELAYED_WORK(&chip->unplug_check_work, unplug_check_worker);

	INIT_WORK(&chip->bms_notify.work, bms_notify);
	INIT_WORK(&chip->battery_id_valid_work, battery_id_valid);

	INIT_DELAYED_WORK(&chip->update_heartbeat_work, update_heartbeat);

	rc = request_irqs(chip, pdev);
	if (rc) {
		pr_err("couldn't register interrupts rc=%d\n", rc);
		goto unregister_batt;
	}

	enable_irq_wake(chip->pmic_chg_irq[USBIN_VALID_IRQ]);
	enable_irq_wake(chip->pmic_chg_irq[DCIN_VALID_IRQ]);
	enable_irq_wake(chip->pmic_chg_irq[BAT_TEMP_OK_IRQ]);
	/* enable_irq_wake(chip->pmic_chg_irq[VBATDET_LOW_IRQ]); */
	enable_irq_wake(chip->pmic_chg_irq[FASTCHG_IRQ]);

#if defined(CONFIG_WIRELESS_CHARGING)
	if (pdata->wpc_int_init)
		pdata->wpc_int_init();
	chip->wc_w_state = !gpio_get_value_cansleep(chip->wc_w_gpio);
#endif

	create_debugfs_entries(chip);

	/* determine what state the charger is in */
	determine_initial_state(chip);

	chip->cable_type = pdata->get_cable_type();
#if defined(CONFIG_WIRELESS_CHARGING)
	if (chip->wc_w_gpio) {
		int ret;
		int irq_num = gpio_to_irq(chip->wc_w_gpio);
		gpio_direction_input(chip->wc_w_gpio);
		ret = request_threaded_irq(irq_num,
				NULL, wpc_charger_irq,
				IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				"wpc-irq", chip);
		if (ret)
			pr_err("%s: Failed to Request IRQ (%d)\n",
				__func__, ret);
		ret = enable_irq_wake(irq_num);
		if (ret)
			pr_err("%s: Failed to Request IRQ wake (%d)\n",
					__func__, ret);

		if (!gpio_get_value_cansleep(chip->wc_w_gpio))
			chip->cable_type = CABLE_TYPE_WPC;
	}
#endif
	gpio_direction_input(GPIO_BATT_INT);
	rc = request_threaded_irq(gpio_to_irq(GPIO_BATT_INT),
			NULL, batt_int_irq,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			"batt-irq", chip);
	if (rc)
		pr_err("%s: Failed to Request IRQ (%d)\n",
			__func__, rc);

	pr_info("battery(PM8917) cable_type (%d)\n", chip->cable_type);

	if (chip->update_time) {
		chip->last_update_time = alarm_get_elapsed_realtime();
		alarm_init(&chip->update_alarm,
			ANDROID_ALARM_ELAPSED_REALTIME_WAKEUP,
			pm_update_alarm);

		INIT_DELAYED_WORK(&chip->update_heartbeat_work,
							update_heartbeat);

		wake_unlock(&chip->monitor_wake_lock);
		schedule_delayed_work(&chip->update_heartbeat_work, 0);
	}


	pr_info("battery(PM8917) probe end\n");

	return 0;

unregister_batt:
	power_supply_unregister(&chip->batt_psy);
unregister_fg:
	power_supply_unregister(&chip->dc_psy);
unregister_usb:
	power_supply_unregister(&chip->usb_psy);
free_chip:
	wake_lock_destroy(&chip->monitor_wake_lock);
	kfree(chip);
	return rc;
}

static int __devexit pm8921_charger_remove(struct platform_device *pdev)
{
	struct pm8921_chg_chip *chip = platform_get_drvdata(pdev);

	alarm_cancel(&chip->event_termination_alarm);
	power_supply_unregister(&chip->dc_psy);
	power_supply_unregister(&chip->usb_psy);
	power_supply_unregister(&chip->batt_psy);

	wake_lock_destroy(&chip->monitor_wake_lock);

	free_irqs(chip);
	platform_set_drvdata(pdev, NULL);
	the_chip = NULL;
	kfree(chip);
	return 0;
}
static const struct dev_pm_ops pm8921_pm_ops = {
	.prepare	= pm8921_charger_prepare,
	.suspend	= pm8921_charger_suspend,
	.suspend_noirq  = pm8921_charger_suspend_noirq,
	.resume_noirq   = pm8921_charger_resume_noirq,
	.resume		= pm8921_charger_resume,
	.complete	= pm8921_charger_complete,
};
static struct platform_driver pm8921_charger_driver = {
	.probe		= pm8921_charger_probe,
	.remove		= __devexit_p(pm8921_charger_remove),
	.driver		= {
			.name	= PM8921_CHARGER_DEV_NAME,
			.owner	= THIS_MODULE,
			.pm	= &pm8921_pm_ops,
	},
};

static int __init pm8921_charger_init(void)
{
	return platform_driver_register(&pm8921_charger_driver);
}

static void __exit pm8921_charger_exit(void)
{
	platform_driver_unregister(&pm8921_charger_driver);
}

late_initcall(pm8921_charger_init);
module_exit(pm8921_charger_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("PMIC8921 charger/battery driver");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:" PM8921_CHARGER_DEV_NAME);
