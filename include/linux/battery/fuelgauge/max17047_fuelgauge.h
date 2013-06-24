/*
 * max17047_fuelgauge.h
 *
 * Copyright (C) 2011 Samsung Electronics
 * SangYoung Son <hello.son@samsung.com>
 *
 * based on max17042_battery.h
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

#ifndef __MAX17047_BATTERY_H_
#define __MAX17047_BATTERY_H_

/* Slave address should be shifted to the right 1bit.
 * R/W bit should NOT be included.
 */
#define SEC_FUELGAUGE_I2C_SLAVEADDR (0x6D >> 1)

/* MAX17047 Registers. */
#define MAX17047_REG_STATUS		0x00
#define MAX17047_REG_VALRT_TH		0x01
#define MAX17047_REG_TALRT_TH		0x02
#define MAX17047_REG_SALRT_TH		0x03
#define MAX17047_REG_VCELL		0x09
#define MAX17047_REG_TEMPERATURE	0x08
#define MAX17047_REG_AVGVCELL		0x19
#define MAX17047_REG_CONFIG		0x1D
#define MAX17047_REG_VERSION		0x21
#define MAX17047_REG_LEARNCFG		0x28
#define MAX17047_REG_FILTERCFG		0x29
#define MAX17047_REG_MISCCFG		0x2B
#define MAX17047_REG_CGAIN		0x2E
#define MAX17047_REG_RCOMP		0x38
#define MAX17047_REG_VFOCV		0xFB
#define MAX17047_REG_SOC_VF		0xFF

/* TRIM ERROR DETECTION */
#define USE_TRIM_ERROR_DETECTION

struct battery_data_t {
	u8 RCOMP0;
	u8 RCOMP_charging;
	int temp_cohot;
	int temp_cocold;
	bool is_using_model_data;
	u8 *type_str;
};

struct sec_fg_info {
	u16 rcomp_val;
	bool trim_err;
	bool dummy;
};

#endif

