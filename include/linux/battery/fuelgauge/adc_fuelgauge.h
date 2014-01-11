/*
 * adc_fuelgauge.h
 * Samsung ADC Fuel Gauge Header
 *
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

#ifndef __ADC_FUELGAUGE_H
#define __ADC_FUELGAUGE_H __FILE__

#include <linux/battery/sec_battery.h>

/*#define SEC_FUELGAUGE_ADC_DELTA_COMPENSATION*/

/* Slave address should be shifted to the right 1bit.
 * R/W bit should NOT be included.
 */
#define SEC_FUELGAUGE_I2C_SLAVEADDR (0x6D >> 1)

#define ADC_HISTORY_SIZE	30

enum {
	ADC_CHANNEL_VOLTAGE_AVG = 0,
	ADC_CHANNEL_VOLTAGE_OCV,
	ADC_CHANNEL_NUM
};
		
struct adc_sample_data {
	unsigned int cnt;
	int total_adc;
	int average_adc;
	int adc_arr[ADC_HISTORY_SIZE];
	int index;
};

struct battery_data_t {
	unsigned int adc_check_count;
	unsigned int monitor_polling_time;
	unsigned int channel_voltage;
	unsigned int channel_current;
	const sec_bat_adc_table_data_t *ocv2soc_table;
	unsigned int ocv2soc_table_size;
	const sec_bat_adc_table_data_t *adc2vcell_table;
	unsigned int adc2vcell_table_size;
	const sec_bat_adc_table_data_t *adc2current_table;
	unsigned int adc2current_table_size;
	const int *event_comp_voltage;
	unsigned int event_comp_voltage_size;
	/* this compensation table is for 1A charging current */
	const sec_bat_adc_table_data_t *cable_comp_voltage;
	unsigned int cable_comp_voltage_size;
#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
	int delta_comp_limit;		/* in uV */
	int delta_check_time;
	int delta_reset_time;
#endif
	u8 *type_str;
};

struct sec_fg_info {
	int voltage_now;
	int voltage_avg;
	int voltage_ocv;
	int current_now;
	int current_avg;
	int capacity;
	struct adc_sample_data	adc_sample[ADC_CHANNEL_NUM];

	struct timeval last_vcell_check_time;
#if defined(SEC_FUELGAUGE_ADC_DELTA_COMPENSATION)
	int delta_voltage_now_in_sec;		/* in uV */
	/* current that compensation happened */
	int current_compensation;
#endif
	int reset_percentage;

	int is_init;
	struct mutex adclock;
	struct delayed_work monitor_work;
};

#endif /* __ADC_FUELGAUGE_H */
