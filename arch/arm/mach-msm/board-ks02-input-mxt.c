/*
 * Copyright (C) 2011 Google, Inc.
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

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <asm/mach-types.h>
#include <linux/regulator/consumer.h>
#include <mach/msm8930-gpio.h>
#include <linux/delay.h>
#include "board-8930.h"
#include "devices-msm8x60.h"
#include <linux/i2c/mxt224s_ks02.h>

#define MSM_8930_GSBI3_QUP_I2C_BUS_ID	3

void tsp_register_callback(struct tsp_callbacks *cb)
{
	charger_callbacks = cb;
	pr_debug("[TSP]mxt224_register_callback\n");

}

static void mxt224_request_gpio(void)
{
	int ret;
	pr_info("[TSP] request gpio\n");

	ret = gpio_request(GPIO_TSP_D_EN, "mxt_tsp_d_en");
	if (ret) {
		pr_err("[TSP]%s: unable to request mxt_tsp_d_en [%d]\n",
				__func__, GPIO_TSP_D_EN);
	}
}

static int mxt224s_power_on(void)
{
	int ret = 0;

	pr_info("[TSP] power ON \n");

	ret = gpio_direction_output(GPIO_TSP_D_EN, 1);

	if (ret) {
		pr_err("[TSP]%s: unable to set mxt_tsp_d_en [%d]\n",
				__func__, GPIO_TSP_D_EN);
	}


	msleep(30);

	return 1;

}

static int mxt224s_power_off(void)
{
	int ret;

	pr_info("[TSP] power OFF /n");

	ret = gpio_direction_output(GPIO_TSP_D_EN, 0);

	if (ret) {
		pr_err("[TSP]%s: unable to set mxt_tsp_d_en [%d]\n",
				__func__, GPIO_TSP_D_EN);
	}

	msleep(30);

	return 1;
}

/*
	Configuration for MXT224-S
*/
#define MXT224S_MAX_MT_FINGERS		10
#define MXT224S_CHRGTIME_BATT		25
#define MXT224S_CHRGTIME_CHRG		60
#define MXT224S_THRESHOLD_BATT		60
#define MXT224S_THRESHOLD_CHRG		70
#define MXT224S_CALCFG_BATT		210
#define MXT224S_CALCFG_CHRG		210

static u8 t7_config_s[] = { GEN_POWERCONFIG_T7,
0x20, 0xFF, 0x32, 0x03
};

static u8 t8_config_s[] = { GEN_ACQUISITIONCONFIG_T8,
0x15, 0x00, 0x05, 0x01, 0x00, 0x00, 0x04, 0x23, 0x28, 0x32
};

static u8 t9_config_s[] = {TOUCH_MULTITOUCHSCREEN_T9,
	0x83, 0x00, 0x00, 0x13, 0x0B, 0x00, 0x70, 0x37, 0x02, 0x07,
	0x0A, 0x0A, 0x01, 0x41, 0x0A, 0x0F, 0x1E, 0x0A, 0x1F, 0x03,
	0xDF, 0x01, 0x0F, 0x0F, 0x19, 0x19, 0x80, 0x00, 0xC0, 0x00,
	0x1E, 0x0F, 0x00, 0x00, 0x00, 0x00};

static u8 t9_config_s_ta[] = {TOUCH_MULTITOUCHSCREEN_T9,
	0x83, 0x00, 0x00, 0x13, 0x0B, 0x00, 0x70, 0x3F, 0x02, 0x07,
	0x0A, 0x0A, 0x01, 0x41, 0x0A, 0x0F, 0x1E, 0x0A, 0x1F, 0x03,
	0xDF, 0x01, 0x0F, 0x0F, 0x19, 0x19, 0x80, 0x00, 0xC0, 0x00,
	0x1E, 0x0F, 0x00, 0x00, 0x00, 0x00};

static u8 t15_config_s[] = {TOUCH_KEYARRAY_T15,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00};

static u8 t18_config_s[] = {SPT_COMCONFIG_T18,
	0x00, 0x00};

static u8 t19_config_s[] = {SPT_GPIOPWM_T19,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static u8 t23_config_s[] = {TOUCH_PROXIMITY_T23,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00};

static u8 t25_config_s[] = {SPT_SELFTEST_T25,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00};

static u8 t40_config_s[] = {PROCI_GRIPSUPPRESSION_T40,
	0x00, 0x00, 0x00, 0x00, 0x00};

static u8 t42_config_s[] = { PROCI_TOUCHSUPPRESSION_T42,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 t46_config_s[] = { SPT_CTECONFIG_T46,
	0x04, 0x00, 0x10, 0x0A, 0x00, 0x01, 0x03, 0x00, 0x00, 0x01
};

static u8 t46_config_s_ta[] = { SPT_CTECONFIG_T46,
	0x04, 0x00, 0x10, 0x14, 0x00, 0x01, 0x03, 0x00, 0x00, 0x01
};

static u8 t47_config_s[] = { PROCI_STYLUS_T47,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00};

static u8 t55_config_s[] = { PROCI_ADAPTIVETHRESHOLD_T55,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static u8 t56_config_s[] = { PROCI_SHIELDLESS_T56,
 	0x00, 0x00, 0x01, 0x19, 0x09, 0x0A, 0x0B, 0x0B, 0x0B, 0x0C,
	0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
	0x0D, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00};


static u8 t57_config_s[] = { PROCI_EXTRATOUCHSCREENDATA_T57,
	0xE3, 0x0F, 0x04};

static u8 t61_config_s[] = {SPT_TIMER_T61,
	0x03, 0x00, 0x00, 0x00, 0x00};

static u8 t62_config_s[] = { PROCG_NOISESUPPRESSION_T62,
	0x7D, 0x01, 0x00, 0x01, 0x0A, 0x00, 0x10, 0x1A, 0x03, 0x01,
	0x00, 0x02, 0x00, 0x01, 0x02, 0x00, 0x05, 0x05, 0x05, 0x50,
	0x0F, 0x0A, 0x30, 0x0F, 0x3F, 0x18, 0x10, 0x04, 0x64, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x1E, 0x01, 0x05, 0x01, 0x30,
	0x0A, 0x0F, 0x0F, 0x00, 0x00, 0xE7, 0xEC, 0xDE, 0x50, 0x00,
	0x00, 0x12, 0x0A, 0x02};

static u8 t62_config_s_ta[] = { PROCG_NOISESUPPRESSION_T62,
	0x7D, 0x01, 0x00, 0x01, 0x0A, 0x00, 0x10, 0x03, 0x02, 0x00,
	0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x05, 0x05, 0x05, 0x50,
	0x0F, 0x0A, 0x26, 0x19, 0x3F, 0x12, 0x0C, 0x04, 0x64, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x28, 0x01, 0x05, 0x01, 0x30,
	0x0A, 0x0F, 0x0F, 0x00, 0x00, 0xE7, 0xEC, 0xDE, 0x50, 0x00,
	0x00, 0x12, 0x0A, 0x02};

static u8 end_config_s[] = { RESERVED_T255 };

static const u8 *mxt224s_config[] = {
	t7_config_s,
	t8_config_s,
	t9_config_s,
	t15_config_s,
	t18_config_s,
	t19_config_s,
	t23_config_s,
	t25_config_s,
	t40_config_s,
	t42_config_s,
	t46_config_s,
	t47_config_s,
	t55_config_s,
	t56_config_s,
	t57_config_s,
	t61_config_s,
	t62_config_s,
	end_config_s
};

static struct mxt224s_platform_data mxt224s_data = {
	.max_finger_touches = MXT224S_MAX_MT_FINGERS,
	.gpio_read_done = GPIO_TOUCH_IRQ,
	.min_x = 0,
	.max_x = 479,
	.min_y = 0,
	.max_y = 799,
	.min_z = 0,
	.max_z = 255,
	.min_w = 0,
	.max_w = 30,
	.config = mxt224s_config,
	.config_e = mxt224s_config,
	.chrgtime_batt = 0,
	.chrgtime_charging = 0,
	.atchcalst = 0,
	.atchcalsthr = 0,
	.tchthr_batt = 0,
	.tchthr_charging = 0,
	.tchthr_batt_e = 0,
	.tchthr_charging_e = 0,
	.calcfg_batt_e = 0,
	.calcfg_charging_e = 0,
	.atchcalsthr_e = 0,
	.atchfrccalthr_e = 0,
	.atchfrccalratio_e = 0,
	.idlesyncsperx_batt = 0,
	.idlesyncsperx_charging = 0,
	.actvsyncsperx_batt = 0,
	.actvsyncsperx_charging = 0,
	.idleacqint_batt = 0,
	.idleacqint_charging = 0,
	.actacqint_batt = 0,
	.actacqint_charging = 0,
	.xloclip_batt = 0,
	.xloclip_charging = 0,
	.xhiclip_batt = 0,
	.xhiclip_charging = 0,
	.yloclip_batt = 0,
	.yloclip_charging = 0,
	.yhiclip_batt = 0,
	.yhiclip_charging = 0,
	.xedgectrl_batt = 0,
	.xedgectrl_charging = 0,
	.xedgedist_batt = 0,
	.xedgedist_charging = 0,
	.yedgectrl_batt = 0,
	.yedgectrl_charging = 0,
	.yedgedist_batt = 0,
	.yedgedist_charging = 0,
	.tchhyst_batt = 0,
	.tchhyst_charging = 0,
	.t9_config_batt = t9_config_s,
	.t9_config_chrg = t9_config_s_ta,
	.t46_config_batt = t46_config_s,
	.t46_config_chrg = t46_config_s_ta,
	.t62_config_batt = t62_config_s,
	.t62_config_chrg = t62_config_s_ta,
	.request_gpio = mxt224_request_gpio,
	.power_on = mxt224s_power_on,
	.power_off = mxt224s_power_off,
	.register_cb = tsp_register_callback,
	.config_fw_version = "M550_AT_130502",
};

/* I2C3 */
static struct i2c_board_info mxt224s_info[] __initdata = {
	{
	I2C_BOARD_INFO(MXT_DEV_NAME, 0x4b),
	.platform_data = &mxt224s_data,
	.irq = MSM_GPIO_TO_INT(GPIO_TOUCH_IRQ),
	},
};

void __init input_touchscreen_init(void)
{

		gpio_tlmm_config(GPIO_CFG(GPIO_TOUCH_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 1);

		gpio_tlmm_config(GPIO_CFG(GPIO_TSP_SEL, 0,
		GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);

		mxt224_request_gpio();

		mxt224s_power_on();

		i2c_register_board_info(MSM_8930_GSBI3_QUP_I2C_BUS_ID,
					mxt224s_info,
					ARRAY_SIZE(mxt224s_info));

}
