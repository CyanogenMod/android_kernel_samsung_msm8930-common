/*
 * charging_temperature_data.h
 *
 * header file supporting temperature functions for Samsung device
 *
 * COPYRIGHT(C) Samsung Electronics Co., Ltd. 2012-2017 All Right Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CHARGING_TEMPERATURE_DATA_H
#define __CHARGING_TEMPERATURE_DATA_H __FILE__

#define DEFAULT_HIGH_BLOCK_TEMP	650
#define DEFAULT_HIGH_RECOVER_TEMP	430
#define DEFAULT_LOW_BLOCK_TEMP	-50
#define DEFAULT_LOW_RECOVER_TEMP	0

/* static const int temp_table[][2] = { */
static const struct pm8xxx_adc_map_pt temp_table[] = {
	{176175,	 800},
	{207608,	 750},
	{241736,	 700},
	{282817,	 650},
	{322315,	 620},
	{350051,	 600},
	{372193,	 580},
	{390293,	 550},
	{480453,	 500},
	{534549,	 470},
	{570613,	 450},
	{606677,	 430},
	{660773,	 400},
	{750933,	 350},
	{841093,	 300},
	{931253,	 250},
	{1021413,	 200},
	{1111573,	 150},
	{1201733,	 100},
	{1291893,	  50},
	{1350492,	  20},
	{1382053,	   0},
	{1428241,	 -30},
	{1446550,	 -50},
	{1567698,	-100},
	{1587330,	-150},
	{1652015,	-200},
};

#endif /* __CHARGING_TEMPERATURE_DATA_H */
