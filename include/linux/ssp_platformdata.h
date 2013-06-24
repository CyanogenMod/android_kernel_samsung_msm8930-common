/*
 * Copyright (C) 2011 Samsung Electronics. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef _SSP_PLATFORMDATA_H_
#define _SSP_PLATFORMDATA_H_

/* Change revision */
#define SSP_MCU_L0	0
#define SSP_MCU_L5	1

/* POSITION VALUES */
/* K330 */
#define K330_TOP_LEFT_UPPER			3
#define K330_TOP_RIGHT_UPPER			0
#define K330_TOP_RIGHT_LOWER			1
#define K330_TOP_LEFT_LOWER			2
#define K330_BOTTOM_LEFT_UPPER		5
#define K330_BOTTOM_RIGHT_UPPER		4
#define K330_BOTTOM_RIGHT_LOWER		7
#define K330_BOTTOM_LEFT_LOWER		6
/* MPU6500 */
#define MPU6500_TOP_LEFT_UPPER		0
#define MPU6500_TOP_RIGHT_UPPER		1
#define MPU6500_TOP_RIGHT_LOWER		2
#define MPU6500_TOP_LEFT_LOWER		3
#define MPU6500_BOTTOM_LEFT_UPPER	4
#define MPU6500_BOTTOM_RIGHT_UPPER	7
#define MPU6500_BOTTOM_RIGHT_LOWER	6
#define MPU6500_BOTTOM_LEFT_LOWER	5
/* YAS532 */
#define YAS532_TOP_LEFT_UPPER			1
#define YAS532_TOP_RIGHT_UPPER		2
#define YAS532_TOP_RIGHT_LOWER		3
#define YAS532_TOP_LEFT_LOWER			0
#define YAS532_BOTTOM_LEFT_UPPER		7
#define YAS532_BOTTOM_RIGHT_UPPER	6
#define YAS532_BOTTOM_RIGHT_LOWER	5
#define YAS532_BOTTOM_LEFT_LOWER		4

/* AK8963C */
#define AK8963C_TOP_UPPER_LEFT		2
#define AK8963C_TOP_UPPER_RIGHT		3
#define AK8963C_TOP_LOWER_RIGHT		0
#define AK8963C_TOP_LOWER_LEFT		1
#define AK8963C_BOTTOM_UPPER_LEFT	6
#define AK8963C_BOTTOM_UPPER_RIGHT	5
#define AK8963C_BOTTOM_LOWER_RIGHT	4
#define AK8963C_BOTTOM_LOWER_LEFT	7

#ifdef CONFIG_SENSORS_SSP_SHTC1
/**
 * struct cp_thm_adc_table - adc to temperature table for PAM thermistor
 * driver
 * @adc: adc value
 * @temperature: temperature(C) * 10
 */
struct cp_thm_adc_table {
	unsigned int adc;
	int temperature;
};
#endif

struct ssp_platform_data {
	int (*wakeup_mcu)(void);
	int (*check_mcu_ready)(void);
	int (*check_mcu_busy)(void);
	int (*set_mcu_reset)(int);
	int (*check_ap_rev)(void);
	int (*read_chg)(void);
	int (*check_changes)(void);
	void (*get_positions)(int *, int *);
	void (*set_prox_light_power)(bool);
        void (*set_prox_led_power)(bool);
        void (*sensor_power_on_vdd)(int, int);
};
#endif
