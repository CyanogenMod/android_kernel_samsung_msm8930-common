/* sr352_regs_50hz.h
 *
 * Driver for sr352 (3MP Camera) from siliconfile
 *
 * Copyright (C) 2013, SAMSUNG ELECTRONICS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Change Date: 2013.04.23
 */

#ifndef _SR352_REGS_H_
#define _SR352_REGS_H_

/* ===================================================*/
/* Name     : sr352 module                       */
/* Version  :                                   */
/* PLL mode : off   McLK - 24MHz                      */
/* fPS      :                                         */
/* PRVIEW   : 800*600                                 */
/* Made by  : PARTRON                             */
/* date     : 13/03/25                               */
/* ===================================================*/

#define SensorConfigScript u16

static SensorConfigScript sr352_Init_Reg[] = {

///////////////////////////////////////////////////////////////////////////////
// Sensor Initial Start
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// mcu clock enable for bus release
///////////////////////////////////////////////////////////////////////////////
0x0326,
0x1089,
0x1080,

///////////////////////////////////////////////////////////////////////////////
// reset
///////////////////////////////////////////////////////////////////////////////
0x0300,
0x0101,
0x0107,
0x0101,

0x0daa, // ESD Check Register
0x0faa, // ESD Check Register

///////////////////////////////////////////////////////////////////////////////
// pad drive / pll setting
///////////////////////////////////////////////////////////////////////////////

0x0300,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0207,		// pclk_drive = 000b, i2c_drive = 111b
0x0c07,		// d_pad_drive = 000b, gpio_pad_drive = 111b
//////////////////////////////////////////////////////////////////
0x0725, //mode_pll1  24mhz / (5+1) = 4mhz
0x086c, //mode_pll2  isp clk = 108Mhz;
0x0985, //mode_pll3  // MIPI 4x div 1/2 // isp clk div = 1/4 //Preview
0x07A5,
0x07A5,
0x07A5,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0A60, // mode_pll4 for mipi mode
0x0Ae0, // mode_pll4 for mipi mode

0x0326,
0x1B03,		// bus clk div = 1/4

///////////////////////////////////////////////////////////////////////////////
// 7 Page(memory configuration)
///////////////////////////////////////////////////////////////////////////////
0x0307,
0x2101,	// SSD sram clock inv on
0x3345,	// bit[6]:C-NR DC ���� ����

///////////////////////////////////////////////////////////////////////////////
// mcu reset
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x1080,		// mcu reset
0x1089,		// mcu clk enable
0x1108,		// xdata clear
0x1100,		// xdata clear
0xff01,   // delay 10ms

///////////////////////////////////////////////////////////////////////////////
// opt download
///////////////////////////////////////////////////////////////////////////////

0x030A,
0x1200,	// otp clock enable

// timing for 108mhz
0x403B,	// otp cfg 1
0x4155,	// otp cfg 2
0x423B,	// otp cfg 3
0x433B,	// otp cfg 4
0x443B,	// otp cfg 5
0x452B,	// otp cfg 6
0x4671,	// otp cfg 7
0x470B,	// otp cfg 8
0x4803,	// otp cfg 9
0x496A,	// otp cfg 10
0x4A3B,	// otp cfg 11
0x4B85,	// otp cfg 12
0x4C55,	// otp cfg 13

0xff01,	//delay 10ms

// downlaod otp - system data
0x2000,	// otp addr = Otp:0000h
0x2100,	// otp addr = Otp:0000h
0x2000,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2100,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1301,	// start download system data
0x1300,	// toggle start

0xff01,   // delay 10ms

// download otp - mcu data
0x2000,	// otp addr = Otp:0080h
0x2180,	// otp addr = Otp:0080h
0x2000,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2e01,	// otp download size = 0100
0x2f00,	// otp download size = 0100
0x1801,	// link xdata to otp
0x3010,	// otp mcu buffer addr = Xdata:105Dh
0x315D,	// otp mcu buffer addr = Xdata:105Dh
0x1302,	// start download mcu data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

// download otp - dpc data
0x2001,	// otp addr = Otp:0180h
0x2180,	// otp addr = Otp:0180h
0x2001,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1801,	// link xdata to otp
0x3033,	// otp mcu buffer addr = Xdata:3384h
0x3184,	// otp mcu buffer addr = Xdata:3384h
0x1304,	// start download dpc data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

0x030A,
0x1280,	// otp clock disable

///////////////////////////////////////////////////////////////////////////////
// TAP for capture function
///////////////////////////////////////////////////////////////////////////////

0x0326,

0x1600,		// set tap address (high)
0x1700,		// set tap address (low)
0x1801,		// use tap memory

0x4002,		// set auto increment mode
0x4400,		// select rom
0x4500,		// set high address
0x4600,		// set low address

// tap code download - start
// (caution : data length must be even)
0x4290,
0x4281,
0x42f0,
0x42e0,
0x4254,
0x4207,
0x42d3,
0x4294,
0x4202,
0x4240,
0x421e,
0x4290,
0x4206,
0x42af,
0x42e0,
0x42b4,
0x4201,
0x4207,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e1,
0x4210,
0x4290,
0x4206,
0x42af,
0x42e0,
0x42d3,
0x4294,
0x4202,
0x4250,
0x4213,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e1,
0x420c,
0x4290,
0x4206,
0x428c,
0x42e4,
0x42f0,
0x42a3,
0x42f0,
0x42a3,
0x42f0,
0x42a3,
0x42f0,
0x4222,
0x4290,
0x421f,
0x4221,
0x42e0,
0x4230,
0x42e1,
0x4255,
0x4290,
0x4290,
0x42b4,
0x42e0,
0x4230,
0x42e5,
0x424e,
0x42e0,
0x4230,
0x42e1,
0x424a,
0x4290,
0x421f,
0x422d,
0x4274,
0x4208,
0x42f0,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e4,
0x421d,
0x4290,
0x4200,
0x4288,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4200,
0x4289,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4200,
0x428a,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4200,
0x428b,
0x4280,
0x421b,
0x4290,
0x4200,
0x428c,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4200,
0x428d,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4200,
0x428e,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4200,
0x428f,
0x42e0,
0x4290,
0x4206,
0x428f,
0x42f0,
0x4222,
0x4290,
0x421f,
0x422d,
0x4274,
0x4204,
0x42f0,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e4,
0x421d,
0x4290,
0x4206,
0x4266,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4206,
0x4267,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4206,
0x4268,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4206,
0x4269,
0x4280,
0x421b,
0x4290,
0x4206,
0x426a,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4206,
0x426b,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4206,
0x426c,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4206,
0x426d,
0x42e0,
0x4290,
0x4206,
0x428f,
0x42f0,
0x4222,
0x420a,
// tap code download - end

0x4401,		// select ram

0x16f8,		// set tap address (high)
0x1700,		// set tap address (low)

///////////////////////////////////////////////////////////////////////////////
// 0 Page
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1041, //binning + prev1
0x1190, //Fixed mode off
0x1200,
0x1328,
0x1501,
0x1700, // Clock inversion off
0x1800,
0x1d05,	//Group_frame_update
0x1E01,	//Group_frame_update_reset
0x2000,
0x2100, // preview row start set
0x2200,
0x2300, // preview col start set
0x2406, // height = 1536
0x2500,
0x2608, // width = 2048
0x2700,

///////////////////////////////////////////////////////////////////////////////
//ONE LINE SETTING
0x0300,
0x4c08, // 1Line = 2200  : 054(HBLANK) + 2146(Active Pixel)
0x4d98,

///////////////////////////////////////////////////////////////////////////////
0x5200,	//Vsync H
0x5314,	//Vsync L
///////////////////////////////////////////////////////////////////////////////

//Pixel windowing
0x8000, // bayer y start
0x8100,
0x8206, // bayer height
0x8324,
0x8400,	//pixel_col_start
0x8500,
0x8608,	//pixel_width
0x8724,

///////////////////////////////////////////////////////////////////////////////
// 1 Page
///////////////////////////////////////////////////////////////////////////////

0x0301,
0x1062,	// BLC=ON, column BLC, col_OBP DPC
0x1111,   // BLC offset ENB + Adaptive BLC ENB B[4]
0x1200,
0x1339,	// BLC(Frame BLC ofs - Column ALC ofs)+FrameALC skip
0x1400,
0x238F,	// Frame BLC avg ���� for 8 frame
0x5004, // blc height = 4
0x5144,
0x6000,
0x6100,
0x6200,
0x6300,
0x787f,	// ramp_rst_offset = 128
0x7904,	// ramp offset
0x7b04,	// ramp offset
0x7e00,

///////////////////////////////////////////////////////////////////////////////
// 2 Page
///////////////////////////////////////////////////////////////////////////////

0x0302,
0x1b80,
0x1d40,
0x2310,
0x4008,
0x418a,	// 20130213 Rev BC ver. ADC input range @ 800mv
0x460a,	// + 3.3V, -0.9V
0x4717, // 20121129 2.9V
0x481a,
0x4913,
0x54c0,
0x5540,
0x5633,
0xa002,
0xa1a8,
0xa204,
0xa379,
0xa404,
0xa5dc,
0xa608,
0xa766,
0xa802,
0xa97b,
0xaa03,
0xab4f,
0xac03,
0xada0,
0xae05,
0xaf43,

///////////////////////////////////////////////////////////////////////////////
// 3 Page
///////////////////////////////////////////////////////////////////////////////

0x0303,
0x1a06, // cds_s1
0x1b7c,
0x1c02,
0x1d88,
0x1e06,
0x1f7c,
0x4200,
0x43b0,
0x4601,
0x4700,
0x4a00,
0x4bae,
0x4e00,
0x4fae,
0x5200,
0x53aa,
0x5600,
0x57aa,
0x5A00,
0x5baa,
0x6A00,
0x6Bf8,
0x7206, // s_addr_cut
0x7390,
0x7806, // rx half_rst
0x798b,
0x7A06,
0x7B95,
0x7C06,
0x7D8b,
0x7E06,
0x7F95,
0x8406, // tx half_rst
0x858b,
0x8606,
0x8795,
0x8806,
0x898b,
0x8A06,
0x8B95,
0x9206, // sx
0x9381,
0x9606,
0x9781,
0x9806, // sxb
0x9981,
0x9c06,
0x9d81,

0xb601, // --------------> s_hb_cnt_hold = 500
0xb7f4,
0xc000, // i_addr_mux_prev
0xc1b4,
0xc200,
0xc3f4,
0xc400,
0xc5b4,
0xc600,
0xc7f4,
0xc800, // i_addr_cut_prev
0xc9b8,
0xca00,
0xcbf0,
0xcc00,
0xcdb8,
0xce00,
0xcff0,
0xd000, // Rx_exp_prev
0xd1ba,
0xd200,
0xd3ee,
0xd400,
0xd5ba,
0xd600,
0xd7ee,
0xd800, // Tx_exp_prev
0xd9bc,
0xdA00,
0xdBec,
0xdC00,
0xdDbc,
0xdE00,
0xdFec,

0xe000,
0xe120,
0xfc06, // clamp_sig
0xfd78,

///////////////////////////////////////////////////////////////////////////////
// 4 Page
///////////////////////////////////////////////////////////////////////////////

0x0304,
0x1003,	//Ramp multiple sampling

0x5a06, // cds_pxl_smpl
0x5b78,
0x5e06,
0x5f78,
0x6206,
0x6378,

///////////////////////////////////////////////////////////////////////////////
// mcu start
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x6200,	// normal mode start
0x6500,	// watchdog disable
0x1009,	// mcu reset release
//Analog setting ���Ŀ� MCU�� reset ��Ŵ.

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1001, // otp_dpc_ctl
0x1111, //Preview1 0410
0x1202, //Preview1 0410

///////////////////////////////////////////////////////////////////////////////
// 15 Page (LSC)
///////////////////////////////////////////////////////////////////////////////

0x0315,
0x1000,	// LSC OFF
0x1100, //gap y disable

///////////////////////////////////////////////////////////////////////////////
// set lsc parameter
///////////////////////////////////////////////////////////////////////////////

0x030a,
0x1901,

0x1180, // B[7] LSC burst mode ENB

0x0326,
0x4002,	// auto increment enable
0x4401,
0x45a3,	// LSC bank0 start addr H
0x4600,	// LSC bank0 start addr L

//LSC G channel reg________________________ 20130422 LSC Blending DNP 90_CWF 5_TL84 5

0x0e01, //BURST_START

0x423c, //G Value
0x4213,
0x42af,
0x423d,
0x4263,
0x42b1,
0x4237,
0x42a3,
0x423d,
0x4230,
0x4202,
0x42cf,
0x422b,
0x4222,
0x42b0,
0x422c,
0x42d2,
0x42fe,
0x4233,
0x42f3,
0x427e,
0x423b,
0x4273,
0x42d7,
0x423a,
0x42d3,
0x42e1,
0x4237,
0x4293,
0x4283,
0x4238,
0x42d3,
0x4260,
0x4231,
0x42e2,
0x42d4,
0x4228,
0x42d2,
0x4257,
0x4223,
0x4292,
0x4237,
0x4225,
0x4262,
0x428f,
0x422d,
0x4283,
0x4224,
0x4236,
0x4293,
0x4296,
0x4239,
0x42b3,
0x42a0,
0x4236,
0x4293,
0x4257,
0x4234,
0x4253,
0x4210,
0x422c,
0x4212,
0x426b,
0x4221,
0x42a1,
0x42df,
0x421b,
0x42f1,
0x42bf,
0x421d,
0x42f2,
0x4220,
0x4227,
0x4222,
0x42cb,
0x4231,
0x42b3,
0x4255,
0x4238,
0x4293,
0x42bd,
0x4234,
0x42b3,
0x422a,
0x4230,
0x42a2,
0x42c8,
0x4226,
0x4272,
0x4201,
0x421a,
0x4261,
0x4264,
0x4214,
0x4221,
0x4244,
0x4216,
0x4281,
0x42ab,
0x4220,
0x42a2,
0x4274,
0x422d,
0x4283,
0x4222,
0x4236,
0x42f3,
0x42bd,
0x4233,
0x4253,
0x4209,
0x422d,
0x42d2,
0x428c,
0x4221,
0x4281,
0x42a5,
0x4214,
0x4230,
0x42fd,
0x420d,
0x42a0,
0x42dc,
0x4210,
0x4241,
0x424c,
0x421b,
0x4222,
0x4228,
0x4229,
0x42f2,
0x42fb,
0x4235,
0x4273,
0x42b3,
0x4231,
0x4262,
0x42e1,
0x422a,
0x42c2,
0x4249,
0x421c,
0x4291,
0x424d,
0x420e,
0x4270,
0x42a2,
0x4207,
0x42d0,
0x4280,
0x420a,
0x4270,
0x42f2,
0x4215,
0x42d1,
0x42dc,
0x4225,
0x42e2,
0x42cc,
0x4233,
0x4243,
0x429c,
0x4230,
0x4292,
0x42cb,
0x4228,
0x42c2,
0x421f,
0x4219,
0x4251,
0x4214,
0x420a,
0x42a0,
0x4263,
0x4204,
0x4200,
0x4243,
0x4206,
0x42b0,
0x42b7,
0x4212,
0x4241,
0x42a8,
0x4223,
0x4242,
0x42b0,
0x4231,
0x42f3,
0x428e,
0x422f,
0x42b2,
0x42b5,
0x4227,
0x4201,
0x42fa,
0x4216,
0x42b0,
0x42e7,
0x4207,
0x42c0,
0x4236,
0x4201,
0x4230,
0x4216,
0x4203,
0x42e0,
0x428a,
0x420f,
0x4271,
0x427f,
0x4220,
0x42f2,
0x4295,
0x4230,
0x42a3,
0x4280,
0x422f,
0x42a2,
0x42b2,
0x4226,
0x4291,
0x42ef,
0x4215,
0x42e0,
0x42d8,
0x4206,
0x42d0,
0x4226,
0x4200,
0x4240,
0x4206,
0x4202,
0x42f0,
0x427a,
0x420e,
0x4271,
0x4270,
0x4220,
0x4242,
0x428f,
0x4230,
0x4293,
0x4283,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422f,
0x4242,
0x42ad,
0x4226,
0x4261,
0x42ec,
0x4215,
0x42c0,
0x42d6,
0x4206,
0x42c0,
0x4224,
0x4200,
0x4230,
0x4205,
0x4202,
0x42e0,
0x4279,
0x420e,
0x4261,
0x426d,
0x4220,
0x4222,
0x428d,
0x4230,
0x4253,
0x427e,
0x4230,
0x4202,
0x42bd,
0x4227,
0x42a2,
0x4203,
0x4217,
0x4260,
0x42f0,
0x4208,
0x4260,
0x4240,
0x4201,
0x42d0,
0x4220,
0x4204,
0x4280,
0x4294,
0x4210,
0x4211,
0x4287,
0x4221,
0x4292,
0x42a0,
0x4231,
0x4293,
0x4291,
0x4230,
0x4292,
0x42ce,
0x4229,
0x4242,
0x4226,
0x4219,
0x42d1,
0x421b,
0x420b,
0x4210,
0x426a,
0x4204,
0x4280,
0x424c,
0x4207,
0x4230,
0x42bf,
0x4212,
0x42b1,
0x42ae,
0x4223,
0x4292,
0x42b8,
0x4232,
0x42a3,
0x429c,
0x4231,
0x42e2,
0x42ee,
0x422b,
0x42e2,
0x425d,
0x421d,
0x42e1,
0x4261,
0x420f,
0x4270,
0x42b1,
0x4208,
0x42f0,
0x4291,
0x420b,
0x42a1,
0x4206,
0x4217,
0x4211,
0x42ed,
0x4226,
0x42f2,
0x42e3,
0x4234,
0x42f3,
0x42bb,
0x4233,
0x4263,
0x420e,
0x422e,
0x4262,
0x4296,
0x4222,
0x4251,
0x42b1,
0x4214,
0x42e1,
0x4209,
0x420e,
0x4270,
0x42ea,
0x4211,
0x4211,
0x425b,
0x421b,
0x42f2,
0x4233,
0x422a,
0x4273,
0x420b,
0x4237,
0x4223,
0x42d9,
0x4235,
0x4213,
0x4235,
0x4231,
0x4292,
0x42da,
0x4227,
0x42b2,
0x4214,
0x421b,
0x4291,
0x4278,
0x4215,
0x4271,
0x425a,
0x4218,
0x4201,
0x42c5,
0x4222,
0x4212,
0x4287,
0x422e,
0x42a3,
0x4241,
0x4239,
0x42e3,
0x42fc,
0x4234,
0x4213,
0x423a,
0x4233,
0x4223,
0x4201,
0x422b,
0x4282,
0x4264,
0x4221,
0x4271,
0x42df,
0x421c,
0x4211,
0x42c4,
0x421e,
0x4272,
0x4223,
0x4227,
0x4212,
0x42c4,
0x4231,
0x4243,
0x425f,
0x423a,
0x4293,
0x42f3,
0x4231,
0x42e3,
0x423a,
0x4235,
0x4263,
0x4231,
0x422f,
0x4232,
0x42b1,
0x4227,
0x4222,
0x4242,
0x4222,
0x4282,
0x422b,
0x4224,
0x4292,
0x427e,
0x422c,
0x4213,
0x4204,
0x4234,
0x42a3,
0x428a,
0x423a,
0x42a3,
0x42c9,
0x4234,
0x4233,
0x423b,
0x4237,
0x42b3,
0x4261,
0x4232,
0x42e2,
0x42fe,
0x422c,
0x42c2,
0x42a5,
0x4229,
0x4202,
0x4291,
0x422a,
0x42b2,
0x42d9,
0x4231,
0x4203,
0x4244,
0x4237,
0x42f3,
0x42b5,
0x423a,
0x42a3,
0x42f4,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x425f, //R Value
0x42a5,
0x42d8,
0x4261,
0x4235,
0x42e5,
0x4258,
0x42e5,
0x4229,
0x424c,
0x4244,
0x4273,
0x4244,
0x42e4,
0x424d,
0x4248,
0x4244,
0x42d9,
0x4254,
0x4245,
0x42af,
0x4260,
0x42c6,
0x423c,
0x425f,
0x4276,
0x4235,
0x425b,
0x4235,
0x425c,
0x4256,
0x4295,
0x4222,
0x424b,
0x4254,
0x423a,
0x423c,
0x4283,
0x4272,
0x4234,
0x4283,
0x424a,
0x4238,
0x4223,
0x42df,
0x4245,
0x4294,
0x42db,
0x4255,
0x4205,
0x429b,
0x4259,
0x4285,
0x42f8,
0x425b,
0x4265,
0x423a,
0x4250,
0x42e4,
0x42b0,
0x4242,
0x42b3,
0x429b,
0x4231,
0x42c2,
0x42c1,
0x4229,
0x4212,
0x4297,
0x422d,
0x4203,
0x4234,
0x423b,
0x42e4,
0x4257,
0x424e,
0x4245,
0x424b,
0x4259,
0x4236,
0x422b,
0x4259,
0x4265,
0x4203,
0x424b,
0x42f4,
0x4243,
0x4239,
0x42f2,
0x42f7,
0x4226,
0x4272,
0x4204,
0x421d,
0x4231,
0x42d9,
0x4221,
0x4232,
0x4280,
0x4231,
0x42a3,
0x42d2,
0x4247,
0x42d5,
0x4205,
0x4257,
0x42a6,
0x423e,
0x4257,
0x42f4,
0x42d4,
0x4247,
0x4293,
0x42e3,
0x4232,
0x4202,
0x4266,
0x421c,
0x42e1,
0x4268,
0x4213,
0x4261,
0x423c,
0x4217,
0x4291,
0x42eb,
0x4228,
0x42d3,
0x4255,
0x4242,
0x4204,
0x42c9,
0x4255,
0x4246,
0x422f,
0x4255,
0x4204,
0x4298,
0x4242,
0x42f3,
0x427c,
0x422a,
0x4281,
0x42e4,
0x4214,
0x4290,
0x42e4,
0x420b,
0x4240,
0x42b9,
0x420f,
0x4251,
0x4267,
0x4220,
0x42e2,
0x42e0,
0x423b,
0x42e4,
0x4282,
0x4252,
0x4236,
0x4214,
0x4253,
0x42d4,
0x4275,
0x423f,
0x42c3,
0x4237,
0x4225,
0x4241,
0x4289,
0x420e,
0x42e0,
0x428b,
0x4205,
0x42b0,
0x4260,
0x4209,
0x42c1,
0x420d,
0x421b,
0x4252,
0x428e,
0x4237,
0x4294,
0x4253,
0x4250,
0x4256,
0x4206,
0x4252,
0x4264,
0x4254,
0x423d,
0x4223,
0x4201,
0x4221,
0x4291,
0x424c,
0x420b,
0x4210,
0x424c,
0x4201,
0x42b0,
0x4221,
0x4205,
0x42e0,
0x42ce,
0x4217,
0x4262,
0x424f,
0x4234,
0x4224,
0x4229,
0x424e,
0x4255,
0x42f1,
0x4252,
0x4254,
0x424e,
0x423c,
0x4272,
0x42ed,
0x4220,
0x4231,
0x4236,
0x4209,
0x4280,
0x4233,
0x4200,
0x4250,
0x4208,
0x4204,
0x4250,
0x42b7,
0x4215,
0x42e2,
0x4237,
0x4232,
0x42f4,
0x421d,
0x424d,
0x42e5,
0x42ef,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4252,
0x4244,
0x424d,
0x423c,
0x4262,
0x42ec,
0x4220,
0x4231,
0x4234,
0x4209,
0x4280,
0x4232,
0x4200,
0x4240,
0x4207,
0x4204,
0x4240,
0x42b5,
0x4215,
0x42b2,
0x4235,
0x4232,
0x42d4,
0x421c,
0x424d,
0x42f5,
0x42f2,
0x4253,
0x4234,
0x4262,
0x423e,
0x4203,
0x420d,
0x4222,
0x4261,
0x4258,
0x420b,
0x42b0,
0x4256,
0x4202,
0x4260,
0x422c,
0x4206,
0x4270,
0x42d8,
0x4217,
0x42f2,
0x4258,
0x4234,
0x42d4,
0x4235,
0x424f,
0x4246,
0x4203,
0x4254,
0x4234,
0x4280,
0x4240,
0x42e3,
0x4247,
0x4226,
0x4271,
0x4299,
0x420f,
0x42b0,
0x4296,
0x4206,
0x4270,
0x426c,
0x420a,
0x4281,
0x4217,
0x421b,
0x42e2,
0x4296,
0x4238,
0x4224,
0x425c,
0x4250,
0x42c6,
0x420c,
0x4256,
0x4254,
0x42b1,
0x4244,
0x42d3,
0x429c,
0x422c,
0x4292,
0x4203,
0x4216,
0x4220,
0x42fd,
0x420c,
0x42c0,
0x42d1,
0x4210,
0x42b1,
0x427f,
0x4222,
0x4252,
0x42f5,
0x423d,
0x4244,
0x429c,
0x4253,
0x42d6,
0x422f,
0x4258,
0x4204,
0x42e3,
0x4249,
0x4273,
0x42fe,
0x4233,
0x42d2,
0x4282,
0x421e,
0x42a1,
0x4282,
0x4215,
0x4201,
0x4256,
0x4219,
0x4212,
0x4203,
0x422a,
0x4253,
0x4267,
0x4243,
0x4234,
0x42e0,
0x4257,
0x4246,
0x4258,
0x425a,
0x4255,
0x421d,
0x424e,
0x4254,
0x426e,
0x423c,
0x42b3,
0x4222,
0x4229,
0x4222,
0x422f,
0x4220,
0x4202,
0x4206,
0x4223,
0x42e2,
0x42ae,
0x4234,
0x4263,
0x42f5,
0x424a,
0x4205,
0x4236,
0x425b,
0x4216,
0x427d,
0x4258,
0x42e5,
0x422a,
0x4251,
0x4254,
0x42be,
0x4244,
0x4203,
0x42b8,
0x4233,
0x42c2,
0x42e5,
0x422b,
0x4292,
0x42c0,
0x422f,
0x4263,
0x4257,
0x423d,
0x42c4,
0x426c,
0x424f,
0x4295,
0x426e,
0x425c,
0x4296,
0x4275,
0x4255,
0x42c5,
0x4226,
0x4255,
0x4245,
0x4213,
0x424b,
0x4234,
0x4249,
0x423e,
0x4213,
0x4294,
0x4237,
0x4203,
0x4277,
0x423a,
0x4294,
0x4201,
0x4247,
0x4224,
0x42ea,
0x4255,
0x42e5,
0x42b8,
0x425c,
0x4276,
0x423a,
0x4258,
0x4275,
0x427c,
0x425e,
0x4245,
0x42b9,
0x4257,
0x4255,
0x4229,
0x424d,
0x4254,
0x4293,
0x4247,
0x4274,
0x427e,
0x424a,
0x42c4,
0x42fb,
0x4255,
0x4295,
0x42b7,
0x4261,
0x4236,
0x4252,
0x4261,
0x42f6,
0x4270,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4240, //B Value
0x4263,
0x42cb,
0x423e,
0x4263,
0x42cc,
0x4239,
0x4283,
0x426e,
0x4233,
0x42c3,
0x4212,
0x422f,
0x42f2,
0x42f7,
0x4230,
0x42d3,
0x422a,
0x4235,
0x42b3,
0x428a,
0x423b,
0x42d3,
0x42e6,
0x423b,
0x42e4,
0x4218,
0x4239,
0x4253,
0x4285,
0x4237,
0x4253,
0x4252,
0x4231,
0x42a2,
0x42e1,
0x422a,
0x4272,
0x427a,
0x4226,
0x4232,
0x425d,
0x4227,
0x4232,
0x429a,
0x422d,
0x4243,
0x4210,
0x4234,
0x42a3,
0x4279,
0x4239,
0x4223,
0x42ab,
0x4237,
0x42b3,
0x423f,
0x4230,
0x4242,
0x42d9,
0x4229,
0x42d2,
0x4255,
0x4221,
0x4221,
0x42e2,
0x421c,
0x4271,
0x42c3,
0x421d,
0x42a2,
0x420a,
0x4224,
0x42d2,
0x4296,
0x422d,
0x4263,
0x420b,
0x4236,
0x4253,
0x42bf,
0x4235,
0x4243,
0x420c,
0x422c,
0x4242,
0x4290,
0x4224,
0x4211,
0x42eb,
0x421a,
0x4201,
0x4266,
0x4214,
0x42a1,
0x4249,
0x4216,
0x4231,
0x4298,
0x421e,
0x4262,
0x423c,
0x4229,
0x4212,
0x42d5,
0x4233,
0x42a3,
0x42a0,
0x4233,
0x4272,
0x42e5,
0x4229,
0x4242,
0x4255,
0x421f,
0x4271,
0x4294,
0x4214,
0x4201,
0x4203,
0x420e,
0x4250,
0x42e4,
0x4210,
0x4231,
0x423d,
0x4219,
0x4211,
0x42f3,
0x4225,
0x4262,
0x42aa,
0x4231,
0x4293,
0x4288,
0x4231,
0x4242,
0x42b8,
0x4225,
0x42c2,
0x420e,
0x421a,
0x4281,
0x423e,
0x420e,
0x4260,
0x42a8,
0x4208,
0x42a0,
0x428b,
0x420a,
0x4290,
0x42e7,
0x4214,
0x4201,
0x42aa,
0x4221,
0x4282,
0x427a,
0x422f,
0x4233,
0x426c,
0x422f,
0x4242,
0x4299,
0x4223,
0x42d1,
0x42e3,
0x4217,
0x4201,
0x4204,
0x420a,
0x4280,
0x426a,
0x4204,
0x42c0,
0x424d,
0x4206,
0x42e0,
0x42af,
0x4210,
0x42a1,
0x427a,
0x421e,
0x42d2,
0x425c,
0x422d,
0x4293,
0x4255,
0x422e,
0x4252,
0x4280,
0x4221,
0x42b1,
0x42bb,
0x4214,
0x4260,
0x42d4,
0x4207,
0x4290,
0x423a,
0x4201,
0x42c0,
0x421e,
0x4203,
0x42f0,
0x4280,
0x420d,
0x42f1,
0x4250,
0x421c,
0x42c2,
0x423f,
0x422c,
0x4213,
0x4243,
0x422d,
0x4272,
0x4272,
0x4220,
0x42d1,
0x42a9,
0x4213,
0x4220,
0x42c2,
0x4206,
0x4250,
0x4226,
0x4200,
0x4290,
0x420b,
0x4202,
0x42d0,
0x426f,
0x420c,
0x42d1,
0x4241,
0x421b,
0x42f2,
0x4238,
0x422b,
0x42d3,
0x4242,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422d,
0x4212,
0x426c,
0x4220,
0x4281,
0x42a5,
0x4212,
0x42c0,
0x42be,
0x4206,
0x4200,
0x4222,
0x4200,
0x4220,
0x4205,
0x4202,
0x4290,
0x426c,
0x420c,
0x42a1,
0x423e,
0x421b,
0x42d2,
0x4235,
0x422b,
0x4293,
0x423d,
0x422e,
0x4202,
0x427d,
0x4221,
0x42b1,
0x42ba,
0x4214,
0x4250,
0x42d5,
0x4207,
0x42a0,
0x423b,
0x4201,
0x42b0,
0x421f,
0x4204,
0x4210,
0x4285,
0x420e,
0x4251,
0x4258,
0x421d,
0x4252,
0x424c,
0x422c,
0x42c3,
0x424c,
0x422e,
0x4272,
0x428d,
0x4223,
0x4231,
0x42db,
0x4216,
0x4280,
0x42fb,
0x420a,
0x4200,
0x4263,
0x4204,
0x4250,
0x4248,
0x4206,
0x42b0,
0x42ad,
0x4210,
0x42a1,
0x427c,
0x421f,
0x4242,
0x4262,
0x422e,
0x4213,
0x425f,
0x4230,
0x4292,
0x42b3,
0x4225,
0x42d2,
0x420d,
0x421a,
0x4231,
0x423b,
0x420e,
0x4210,
0x42a3,
0x4208,
0x4270,
0x428a,
0x420a,
0x42d0,
0x42f0,
0x4214,
0x42b1,
0x42b7,
0x4222,
0x4282,
0x428b,
0x4230,
0x4273,
0x4282,
0x4231,
0x42d2,
0x42d1,
0x4228,
0x4262,
0x4240,
0x421e,
0x4221,
0x4281,
0x4212,
0x42d0,
0x42f2,
0x420d,
0x4250,
0x42db,
0x420f,
0x42c1,
0x423b,
0x4219,
0x4231,
0x42f7,
0x4225,
0x42d2,
0x42b3,
0x4232,
0x42a3,
0x42a0,
0x4234,
0x4222,
0x42fd,
0x422b,
0x4282,
0x4282,
0x4223,
0x4201,
0x42da,
0x4218,
0x42c1,
0x4256,
0x4213,
0x42c1,
0x4240,
0x4216,
0x4211,
0x429c,
0x421e,
0x42c2,
0x4245,
0x4229,
0x42e2,
0x42ec,
0x4235,
0x42d3,
0x42ce,
0x4234,
0x4203,
0x4208,
0x422d,
0x4202,
0x42a7,
0x4226,
0x4242,
0x421e,
0x421d,
0x42c1,
0x42ad,
0x4219,
0x4261,
0x429c,
0x421b,
0x4271,
0x42eb,
0x4223,
0x4202,
0x4279,
0x422c,
0x4213,
0x4204,
0x4236,
0x4273,
0x42cb,
0x4233,
0x42c3,
0x422f,
0x4232,
0x4222,
0x42ff,
0x422c,
0x4242,
0x428b,
0x4225,
0x4222,
0x422a,
0x4221,
0x4262,
0x421d,
0x4223,
0x4262,
0x4263,
0x4229,
0x42f2,
0x42da,
0x4231,
0x42c3,
0x4259,
0x4238,
0x42d3,
0x42c1,
0x4238,
0x42e3,
0x4256,
0x4237,
0x4233,
0x4257,
0x4232,
0x4232,
0x42f7,
0x422c,
0x4282,
0x42a7,
0x4229,
0x4262,
0x429e,
0x422b,
0x4252,
0x42db,
0x4230,
0x42e3,
0x423b,
0x4237,
0x4283,
0x42ae,
0x423b,
0x4224,
0x4216,

0x0e00, //BURST_END

0x030a,
0x1902,	// Bus Switch

0x0315,	// Shading FPGA(Hi-352)
0x1001,	// LSC ON
0x1100, //gap y off, gap x off

0x2780,	// LSC G
0x2880,	// LSC B
0x2980,	// LSC R

///////////////////////////////////
// 10 Page Saturation (H/W)
///////////////////////////////////
0x0310,
0x1001,
0x1210, //YOFS ENB
0x1700, //20121127 CSP option
0x1800, //20121127 CSP option
0x2000, //16_235 range scale down off
0x6003, //Sat ENB Transfer Function     //Transfunction on

///////////////////////////////////
// 11 Page D-LPF (H/W)
///////////////////////////////////
0x0311, //11 page
0x100f, //D-LPF ENB //DPC marker

0x1228, //20121120 character long line detection th
0x132c, //20121120 character short line detection th

0x1d12, // ORG_STD Ctrl
0x1e00,// 20130410_STD 03 -> 00
0x2178, // Color STD Gain
//Bayer Sharpness Gain Ctrl
0xb722, //SpColor_gain1
0xb822, //SpColor_gain2
0xb921, //SpColor_gain3
0xba1e, //SpColor_gain4
0xbb1c, //SpColor_gain5
0xbc1a, //SpColor_gain6

0xf280, //pga_dark1_hi //Enter Dark1
0xf376, //pga_dark_lo  //Escape Dark1
///////////////////////////////////
// 12 Page DPC,GBGR (H/W)//////////
///////////////////////////////////
0x0312, //12 page
0x1057, //DPC ON
0x1230,
0x2b08, //white_th
0x2c08, //middle_h_th
0x2d08, //middle_l_th
0x2e06, //dark_th
0x2f10, //20121127 _DPC TH
0x3010, //20121127 _DPC TH
0x3110, //20121127 _DPC TH
0x3210, //20121127 _DPC TH
0x4188, //GBGR Cut off //46

///////////////////////////////////
// 12 Page CI-LPF (H/W)////////////
///////////////////////////////////

0xEF01, //Interpol Color filter On/Off

///////////////////////////////////
// 13 Page YC-2D_Y-NR (H/W)/////////
///////////////////////////////////
0x0313,

0x802d, //YC-2D_C-NR ENB, C-Filter DC option on B[7] //DC on 8b //DC off 2d
0x81ff, // add 20121210
0x82fe, // add 20121210

0x8532,
0x8608, // add 20121210

//==========================================================================
// C-Filter PS Reducing (Mask-Size Adjustment)

0x8790,//C-mask near STD TH
0x8870,//C-mask middle STD TH
0x8950,//C-mask far STD TH

0x8a86, //color STD

0x970f, // C-filter Lum gain 1
0x980e,
0x990d,
0x9a0c,
0x9b0b,
0x9c0a,
0x9d09,
0x9e08,

0xa70f, // C-filter STD gain 1
0xa80e,
0xa90d,
0xaa0c,
0xab0b,
0xac0a,
0xad09,
0xae08,

//==========================================================================

///////////////////////////////////
// 14 Page YC-2D_Sharpness(H/W)
///////////////////////////////////
0x0314,
0x7720, //Yc2d_ee_color_gain1
0x7820, //Yc2d_ee_color_gain2
0x7920, //Yc2d_ee_color_gain3
0x7a1c, //Yc2d_ee_color_gain4
0x7b1b, //Yc2d_ee_color_gain5
0x7c1a, //Yc2d_ee_color_gain6
0x7d19, //Yc2d_ee_color_gain7
0x7e18, //Yc2d_ee_color_gain8

0xc000, //Yc2d_ee_lclip_gain_n1
0xc100, //Yc2d_ee_lclip_gain_n2
0xc200, //Yc2d_ee_lclip_gain_n3
0xc300, //Yc2d_ee_lclip_gain_n4
0xc401, //Yc2d_ee_lclip_gain_n5

///////////////////////////////////////////////////////////////////////////////
// 16 Page CMC / AWB Gain
///////////////////////////////////////////////////////////////////////////////

0x0316,
0x107f,	// CMC ENB	3f(spGrap off) 7f(spGrap on)
0x2052,// PS / LN

0xa003,	// WB gain on
0xa205,	// R_h (12bit = 8bit * 16)
0xa380,	// R_l
0xa407,	// B_h (12bit = 8bit * 16)
0xa580,	// B_l

0xD001,	//Bayer gain enable
///////////////////////////////////////////////////////////////////////////////
// 17 Page Gamma
///////////////////////////////////////////////////////////////////////////////

0x0317,
0x1007,	// GMA ENB //PS On
0x1252,// old:43 new:65

///////////////////////////////////////////////////////////////////////////////
// 18 Page MCMC
///////////////////////////////////////////////////////////////////////////////

0x0318,	// Page 18
0x1001,	// mcmc_ctl1
0x117f,	// mcmc_ctl2
0x5310,	// mcmc_ctl3

0x561b,	// mcmc_glb_sat_lvl_sp1
0x5739,	// mcmc_glb_sat_lvl_sp2
0x585a,	// mcmc_glb_sat_lvl_sp3
0x5980,	// mcmc_glb_sat_lvl_sp4
0x5aa6,	// mcmc_glb_sat_lvl_sp5
0x5bc1,	// mcmc_glb_sat_lvl_sp6
0x5ce8,	// mcmc_glb_sat_lvl_sp7
0x5d38,	// mcmc_glb_sat_gain_sp1
0x5e3a,	// mcmc_glb_sat_gain_sp2
0x5f3c,	// mcmc_glb_sat_gain_sp3
0x603f,	// mcmc_glb_sat_gain_sp4
0x613f,	// mcmc_glb_sat_gain_sp5
0x623f,	// mcmc_glb_sat_gain_sp6
0x633f,	// mcmc_glb_sat_gain_sp7
0x643f,	// mcmc_glb_sat_gain_sp8
0x6500,	// mcmc_std_ctl1
0x6600,	// mcmc_std_ctl2
0x6700,	// mcmc_std_ctl3

0x6cff,	// mcmc_lum_ctl1 sat hue offset
0x6d3f,	// mcmc_lum_ctl2 gain
0x6e00,	// mcmc_lum_ctl3 hue
0x6f00,	// mcmc_lum_ctl4 rgb offset
0x7000,	// mcmc_lum_ctl5 rgb scale

0xa100,
0xa201,	//star gain enb

///////////////////////////////////////////////////////////////////////////////
// 1A Page_RGB Y-NR, Y-Sharpness
///////////////////////////////////////////////////////////////////////////////

0x031a,
0x309f,	// RGB-Sharpness ENB // Flat-region RGB ENB B[1] //Green dis [7] On

0x8D20, //RGB-Color_Gain1
0x8E20, //RGB-Color_Gain2
0x8F20, //RGB-Color_Gain3
0x9020, //RGB-Color_Gain4
0x9120, //RGB-Color_Gain5

///////////////////////////////////////////////////////////////////////////////
// 20 Page (FZY)
///////////////////////////////////////////////////////////////////////////////

0x0320, //Page 20
0x1220,

0x1800,//Check Flicker Lock Off

0x3600, //EXP Unit 
0x3708,
0x3898,

0x51ff, //PGA Max
0x5220, //PGA Min x0.9

0x61FF,	// max ramp gain
0x6200,	// min ramp gain
0x60E0,	// ramp gain

0x803a, //Y Target
///////////////////////////////////////////////////////////////////////////////
// 23 Page (AFC)
///////////////////////////////////////////////////////////////////////////////

0x0323, //Page 23
0x147A, //Flicker Line 100
0x1566, //Flicker Line 120
0x1001, //Frame Interval

///////////////////////////////////////////////////////////////////////////////
// 2A Page (SSD)
///////////////////////////////////////////////////////////////////////////////

0x032A,
0x1011,
0x1101,
0x1650,	//SSD B gain int gain 1.5

0x0300,
0x0100,
0xff02,

0x0320,
0x10bd, //50hz bd, 60hz ad
0x2000, //Start ExpTime 120fps
0x2106,
0x22d9,
0x2320,

0x03c1,
0x1006, // ssd tranfer disable
0xff02, // 20ms

0x0300,
0x0101,	// Sleep on

0x03c1,
0x1007, // ssd tranfer enable
///////////////////////////////////////////////////////////////////////////////
//
// F/W setting start
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// C0 Page (SYSTEM)
///////////////////////////////////////////////////////////////////////////////
//OPCLK Setting
////54mhz = 337F980
0x03C0,
0x1403,
0x1537,
0x16F9,
0x1780,

///////////////////////////////////////////////////////////////////////////////
// C6 Page (SSD Y weight)
///////////////////////////////////////////////////////////////////////////////
//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

///////////////////////////////////////////////////////////////////////////////
// C7 Page (AE)
///////////////////////////////////////////////////////////////////////////////
//Shutter Setting
	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
0x1230, // Fast speed
0x15c0, // SSD Patch Weight Y Mean On

0x1e03, // Band1 Step
0x1f06, // Band2 Step
0x2008, // Band3 Step

0x2149, // Band1 Gain 30fps
0x2253, // Band2 Gain 15fps
0x2378,// Band3 Gain 12fps

0x3608, // Max 8fps
0x3708, // Max 8fps

0x3d22, // YTh Lock, Unlock0

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0x7082, // 50hz 82, 60hz 02
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x4400, //Start ExpTime 120fps
0x4506,	
0x46d9,	
0x4720,	

0xa748, //Start ExpTime 120fps float
0xa8db,	
0xa924,	
0xaa00,	

0x0320, //HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x10f0,	//AE On 50hz f0, 60hz d0

///////////////////////////////////////////////////////////////////////////////
// D9 Page (Capture/Preview)
///////////////////////////////////////////////////////////////////////////////
0x03d9,
0x7ce0, 
0x8c20,	//en_ramp_gain_auto

///////////////////////////////////////////////////////////////////////////////
// C8 ~ CC Page (AWB)
///////////////////////////////////////////////////////////////////////////////
0x03C8,
0x0e01, // burst start

0x10D2,
0x11c3,
0x12e0,
0x131a, //bCtl4
0x149f,
0x15c4,
0x1600,
0x1734,
0x1855,
0x1922,
0x1a22,
0x1b44,
0x1c44,
0x1d66,
0x1e66,
0x1f88,
0x2088,
0x2104,
0x2230,
0x2308,
0x2400,
0x251e,
0x2630, //init awb speed
0x276a,
0x2880,
0x2910,
0x2a04,
0x2b0a,
0x2c04,
0x2d0c,
0x2e1e,
0x2f00, //dwOutdoorCondInTh
0x3000, //dwOutdoorCondInTh_n01
0x3121, //dwOutdoorCondInTh_n02
0x3234, //dwOutdoorCondInTh_n03
0x3300, //dwOutdoorCondOutTh
0x3400, //dwOutdoorCondOutTh_n01
0x3527, //dwOutdoorCondOutTh_n02
0x3610, //dwOutdoorCondOutTh_n03
0x3700, //dwEvTh
0x3800, //dwEvTh_n01
0x3904, //dwEvTh_n02 //840fps
0x3a11, //dwEvTh_n03
0x3b00, //dwEvTh_a01
0x3c00, //dwEvTh_a01_n01
0x3d08, //dwEvTh_a01_n02
0x3e23, //dwEvTh_a01_n03 //480fps
0x3f00, //dwEvTh_a02
0x4000, //dwEvTh_a02_n01
0x4120,//dwEvTh_a02_n02
0x428D,//dwEvTh_a02_n03 //120fps
0x4300, //dwEvTh_a03
0x4400, //dwEvTh_a03_n01
0x4561, //dwEvTh_a03_n02
0x46a8,//dwEvTh_a03_n03 //40fps
0x4700, //dwEvTh_a04
0x4804, //dwEvTh_a04_n01
0x49c4, //dwEvTh_a04_n02
0x4ab4, //dwEvTh_a04_n03
0x4b00, //dwEvTh_a05
0x4c0b, //dwEvTh_a05_n01
0x4d71, //dwEvTh_a05_n02
0x4eb0, //dwEvTh_a05_n03
0x4f96,//aAglMaxMinLmt
0x504a,//aAglMaxMinLmt_a01
0x519C, //aAglMaxMinLmt_a02
0x5260, //aAglMaxMinLmt_a03
0x53a0, //aAglMaxMinLmt_a04
0x5420, //aAglMaxMinLmt_a05
0x55a0, //aAglMaxMinLmt_a06
0x5632, //aAglMaxMinLmt_a07
0x5776,//aTgtWhtRgnBgMaxMinLmt
0x5856,//aTgtWhtRgnBgMaxMinLmt_a01
0x5982, //aTgtWhtRgnBgMaxMinLmt_a02
0x5a52, //aTgtWhtRgnBgMaxMinLmt_a03
0x5b98, //aTgtWhtRgnBgMaxMinLmt_a04
0x5c20, //aTgtWhtRgnBgMaxMinLmt_a05
0x5d98, //aTgtWhtRgnBgMaxMinLmt_a06
0x5e32, //aTgtWhtRgnBgMaxMinLmt_a07

0x5f10, //bTgtWhtRgnBgStep
0x600a, //BpOption distance weight  def : 0a -> 04 -> 02

0x611e,
0x6234,
0x6380,
0x6410,
0x6501,
0x6604,
0x670e,
0x6800,
0x6932,
0x6a00,
0x6ba2,
0x6c02,
0x6d00,
0x6e00,
0x6f00,
0x7000,
0x7100,
0x7200,
0x7300,
0x7400,
0x7500,
0x7655,
0x7755,
0x7855,
0x7955,
0x7a55,
0x7b55,
0x7c55,
0x7d55,
0x7e55,
0x7f55,
0x8055,
0x8155,
0x8255,
0x8355,
0x8455,
0x8555,
0x8655,
0x8755,
0x8855,
0x8955,
0x8a55,
0x8b55,
0x8c55,
0x8d55,
0x8e55,
0x8f55,
0x9055,
0x9100,
0x9200,

0x9300, //Indoor_wRgIntOfs
0x94a0, //Indoor_wRgIntOfs_n01
0x9500, //Indoor_wBgIntOfs
0x96c0, //Indoor_wBgIntOfs_n01
0x9610, //Indoor_bRgStep
0x9710, //Indoor_bBgStep
0x9926, //Indoor_aTgtWhtRgnBg
0x9a29, //Indoor_aTgtWhtRgnBg_a01
0x9b2c, //Indoor_aTgtWhtRgnBg_a02
0x9c38, //Indoor_aTgtWhtRgnBg_a03
0x9d43, //Indoor_aTgtWhtRgnBg_a04
0x9e4d, //Indoor_aTgtWhtRgnBg_a05
0x9f59, //Indoor_aTgtWhtRgnBg_a06
0xa064, //Indoor_aTgtWhtRgnBg_a07
0xa16f, //Indoor_aTgtWhtRgnBg_a08
0xa27b, //Indoor_aTgtWhtRgnBg_a09
0xa38e,//Indoor_aTgtWhtRgnBg_a10
0xa4a0,//Indoor_aTgtWhtRgnRgLtLmt
0xa598,//Indoor_aTgtWhtRgnRgLtLmt_a01
0xa682,//Indoor_aTgtWhtRgnRgLtLmt_a02
0xa76d,//Indoor_aTgtWhtRgnRgLtLmt_a03
0xa860,//Indoor_aTgtWhtRgnRgLtLmt_a04
0xa956,//Indoor_aTgtWhtRgnRgLtLmt_a05
0xaa4c,//Indoor_aTgtWhtRgnRgLtLmt_a06
0xab45,//Indoor_aTgtWhtRgnRgLtLmt_a07
0xac40,//Indoor_aTgtWhtRgnRgLtLmt_a08
0xad3c,//Indoor_aTgtWhtRgnRgLtLmt_a09
0xae39,//Indoor_aTgtWhtRgnRgLtLmt_a10
0xafaa,//Indoor_aTgtWhtRgnRgRtLmt
0xb0a9,//Indoor_aTgtWhtRgnRgRtLmt_a01
0xb1a8,//Indoor_aTgtWhtRgnRgRtLmt_a02
0xb2a2,//Indoor_aTgtWhtRgnRgRtLmt_a03
0xb395,//Indoor_aTgtWhtRgnRgRtLmt_a04
0xb488,//Indoor_aTgtWhtRgnRgRtLmt_a05
0xb578,//Indoor_aTgtWhtRgnRgRtLmt_a06
0xb665,//Indoor_aTgtWhtRgnRgRtLmt_a07
0xb75a,//Indoor_aTgtWhtRgnRgRtLmt_a08
0xb850,//Indoor_aTgtWhtRgnRgRtLmt_a09
0xb94a,//Indoor_aTgtWhtRgnRgRtLmt_a10
0xba1b, //Indoor_aOptWhtRgnBg
0xbb1d, //Indoor_aOptWhtRgnBg_a01
0xbc1f, //Indoor_aOptWhtRgnBg_a02
0xbd2a, //Indoor_aOptWhtRgnBg_a03
0xbe38, //Indoor_aOptWhtRgnBg_a04
0xbf47, //Indoor_aOptWhtRgnBg_a05
0xc054, //Indoor_aOptWhtRgnBg_a06
0xc161, //Indoor_aOptWhtRgnBg_a07
0xc272, //Indoor_aOptWhtRgnBg_a08
0xc382, //Indoor_aOptWhtRgnBg_a09
0xc49a,//Indoor_aOptWhtRgnBg_a10
0xc5ad, //Indoor_aOptWhtRgnRgLtLmt
0xc698,//Indoor_aOptWhtRgnRgLtLmt_a01
0xc78a,//Indoor_aOptWhtRgnRgLtLmt_a02
0xc874,//Indoor_aOptWhtRgnRgLtLmt_a03
0xc95f,//Indoor_aOptWhtRgnRgLtLmt_a04
0xca50,//Indoor_aOptWhtRgnRgLtLmt_a05
0xcb46,//Indoor_aOptWhtRgnRgLtLmt_a06
0xcc40,//Indoor_aOptWhtRgnRgLtLmt_a07
0xcd39,//Indoor_aOptWhtRgnRgLtLmt_a08
0xce35,//Indoor_aOptWhtRgnRgLtLmt_a09
0xcf33,//Indoor_aOptWhtRgnRgLtLmt_a10
0xd0ba,//Indoor_aOptWhtRgnRgRtLmt
0xd1b9,//Indoor_aOptWhtRgnRgRtLmt_a01
0xd2b8,//Indoor_aOptWhtRgnRgRtLmt_a02
0xd3b5, //Indoor_aOptWhtRgnRgRtLmt_a03
0xd4ae, //Indoor_aOptWhtRgnRgRtLmt_a04
0xd5a1, //Indoor_aOptWhtRgnRgRtLmt_a05
0xd68c, //Indoor_aOptWhtRgnRgRtLmt_a06
0xd778,//Indoor_aOptWhtRgnRgRtLmt_a07
0xd860,//Indoor_aOptWhtRgnRgRtLmt_a08
0xd954,//Indoor_aOptWhtRgnRgRtLmt_a09
0xda4d,//Indoor_aOptWhtRgnRgRtLmt_a10

0xdb36, //Indoor_aCtmpWgtWdhTh
0xdc40, //Indoor_aCtmpWgtWdhTh_a01
0xdd4c, //Indoor_aCtmpWgtWdhTh_a02
0xde5c, //Indoor_aCtmpWgtWdhTh_a03
0xdf6e, //Indoor_aCtmpWgtWdhTh_a04
0xe07f, //Indoor_aCtmpWgtWdhTh_a05
0xe1a4, //Indoor_aCtmpWgtWdhTh_a06
0xe227, //Indoor_aCtmpWgtHgtTh
0xe332, //Indoor_aCtmpWgtHgtTh_a01
0xe43c, //Indoor_aCtmpWgtHgtTh_a02
0xe548, //Indoor_aCtmpWgtHgtTh_a03
0xe65c, //Indoor_aCtmpWgtHgtTh_a04
0xe770, //Indoor_aCtmpWgtHgtTh_a05
0xe87c, //Indoor_aCtmpWgtHgtTh_a06
0xe986, //Indoor_aCtmpWgtHgtTh_a07
0xea90, //Indoor_aCtmpWgtHgtTh_a08
0xeb11, //Indoor_aCtmpWgt
0xec11, //Indoor_aCtmpWgt_a01
0xed12, //Indoor_aCtmpWgt_a02
0xee11, //Indoor_aCtmpWgt_a03
0xef11, //Indoor_aCtmpWgt_a04
0xf033, //Indoor_aCtmpWgt_a05
0xf111, //Indoor_aCtmpWgt_a06
0xf214, //Indoor_aCtmpWgt_a07
0xf343, //Indoor_aCtmpWgt_a08
0xf411, //Indoor_aCtmpWgt_a09
0xf555, //Indoor_aCtmpWgt_a10
0xf641, //Indoor_aCtmpWgt_a11
0xf716, //Indoor_aCtmpWgt_a12
0xf865, //Indoor_aCtmpWgt_a13
0xf911, //Indoor_aCtmpWgt_a14
0xfa48, //Indoor_aCtmpWgt_a15
0xfb61, //Indoor_aCtmpWgt_a16
0xfc11, //Indoor_aCtmpWgt_a17
0xfd46, //Indoor_aCtmpWgt_a18
0x0e00, // burst end

0x03c9, //c9 page
0x0e01, // burst start

0x1011, //Indoor_aCtmpWgt_a19
0x1111, //Indoor_aCtmpWgt_a20
0x1223, //Indoor_aCtmpWgt_a21
0x1311, //Indoor_aCtmpWgt_a22
0x1411, //Indoor_aCtmpWgt_a23
0x1510, //Indoor_aCtmpWgt_a24

0x1611,//Indoor_aYlvlWgt
0x1711,//Indoor_aYlvlWgt_a01
0x1811,//Indoor_aYlvlWgt_a02
0x1911,//Indoor_aYlvlWgt_a03
0x1a11,//Indoor_aYlvlWgt_a04
0x1b11,//Indoor_aYlvlWgt_a05
0x1c11,//Indoor_aYlvlWgt_a06
0x1d11,//Indoor_aYlvlWgt_a07
0x1e11,//Indoor_aYlvlWgt_a08
0x1f11,//Indoor_aYlvlWgt_a09
0x2011,//Indoor_aYlvlWgt_a10
0x2122,//Indoor_aYlvlWgt_a11
0x2222,//Indoor_aYlvlWgt_a12
0x2334,//Indoor_aYlvlWgt_a13
0x2432,//Indoor_aYlvlWgt_a14
0x2521,//Indoor_aYlvlWgt_a15

0x2633, //Indoor_aTgtAngle
0x273f, //Indoor_aTgtAngle_a01
0x2843, //Indoor_aTgtAngle_a02
0x2950,//Indoor_aTgtAngle_a03
0x2a68,//Indoor_aTgtAngle_a04
0x2b28,//Indoor_aRgTgtOfs
0x2c14,//Indoor_aRgTgtOfs_a01
0x2d02,//Indoor_aRgTgtOfs_a02
0x2e00,//Indoor_aRgTgtOfs_a03
0x2f08,//Indoor_aRgTgtOfs_a04
0x30d4,//Indoor_aBgTgtOfs
0x31ca,//Indoor_aBgTgtOfs_a01
0x32aa,//Indoor_aBgTgtOfs_a02
0x33a4,//Indoor_aBgTgtOfs_a03
0x3488,//Indoor_aBgTgtOfs_a04

0x3500,//bRgDefTgt //indoor
0x3600,//bBgDefTgt //indoor

0x3720,//Indoor_aWhtPtTrcAglOfs
0x381e,//Indoor_aWhtPtTrcAglOfs_a01
0x391c,//Indoor_aWhtPtTrcAglOfs_a02
0x3a1a,//Indoor_aWhtPtTrcAglOfs_a03
0x3b18,//Indoor_aWhtPtTrcAglOfs_a04
0x3c16,//Indoor_aWhtPtTrcAglOfs_a05
0x3d14,//Indoor_aWhtPtTrcAglOfs_a06
0x3e14,//Indoor_aWhtPtTrcAglOfs_a07
0x3f13,//Indoor_aWhtPtTrcAglOfs_a08
0x4012,//Indoor_aWhtPtTrcAglOfs_a09
0x4104,//Indoor_bWhtPtTrcCnt
0x4214,//Indoor_aRtoDiffThNrBp
0x433c,//Indoor_aRtoDiffThNrBp_a01
0x4428,//Indoor_aAglDiffThTrWhtPt
0x4550,//Indoor_aAglDiffThTrWhtPt_a01
0x46aa,//Indoor_bWgtRatioTh1
0x47a0,//Indoor_bWgtRatioTh2
0x4844,//Indoor_bWgtOfsTh1
0x4940,//Indoor_bWgtOfsTh2
0x4a5a,//Indoor_bWhtPtCorAglMin
0x4b70,//Indoor_bWhtPtCorAglMax
0x4c04,//Indoor_bYlvlMin
0x4df8,//Indoor_bYlvlMax
0x4e28,//Indoor_bPxlWgtLmtLoTh
0x4f78,//Indoor_bPxlWgtLmtHiTh
0x5000,//Indoor_SplBldWgt_1
0x5100,//Indoor_SplBldWgt_2
0x5264,//Indoor_SplBldWgt_3
0x5360,//Indoor_TgtOff_StdHiTh
0x5430,//Indoor_TgtOff_StdLoTh
0x5505,//Indoor_wInitRg
0x56d0,//Indoor_wInitRg_n01
0x5706,//Indoor_wInitBg
0x5840,//Indoor_wInitBg_n01

0x5902, //Indoor_aRatioBox
0x5aee, //Indoor_aRatioBox_a01
0x5b06, //Indoor_aRatioBox_a02
0x5c40, //Indoor_aRatioBox_a03
0x5d08, //Indoor_aRatioBox_a04
0x5e34, //Indoor_aRatioBox_a05
0x5f0b,//Indoor_aRatioBox_a06
0x6054,//Indoor_aRatioBox_a07
0x6103, //Indoor_aRatioBox_a08
0x6252, //Indoor_aRatioBox_a09
0x6307, //Indoor_aRatioBox_a10
0x64d0, //Indoor_aRatioBox_a11
0x6506, //Indoor_aRatioBox_a12
0x66a4, //Indoor_aRatioBox_a13
0x6708, //Indoor_aRatioBox_a14
0x68fc, //Indoor_aRatioBox_a15
0x6903, //Indoor_aRatioBox_a16
0x6ae8, //Indoor_aRatioBox_a17
0x6b0a, //Indoor_aRatioBox_a18
0x6c8c, //Indoor_aRatioBox_a19
0x6d04, //Indoor_aRatioBox_a20
0x6eb0, //Indoor_aRatioBox_a21
0x6f07, //Indoor_aRatioBox_a22
0x706c, //Indoor_aRatioBox_a23
0x7104, //Indoor_aRatioBox_a24
0x72e2, //Indoor_aRatioBox_a25
0x730c, //Indoor_aRatioBox_a26
0x741c, //Indoor_aRatioBox_a27
0x7503, //Indoor_aRatioBox_a28
0x7684, //Indoor_aRatioBox_a29
0x7705, //Indoor_aRatioBox_a30
0x78dc, //Indoor_aRatioBox_a31
0x7905, //Indoor_aRatioBox_a32
0x7adc, //Indoor_aRatioBox_a33
0x7b0c, //Indoor_aRatioBox_a34
0x7ce4, //Indoor_aRatioBox_a35
0x7d01, //Indoor_aRatioBox_a36
0x7ef4, //Indoor_aRatioBox_a37
0x7f05, //Indoor_aRatioBox_a38
0x8000, //Indoor_aRatioBox_a39

0x8100, //Outdoor_wRgIntOfs
0x8240, //Outdoor_wRgIntOfs_n01
0x8301, //Outdoor_wBgIntOfs
0x8400, //Outdoor_wBgIntOfs_n01
0x8510, //Outdoor_bRgStep
0x8610, //Outdoor_bBgStep
0x8751, //Outdoor_aTgtWhtRgnBg
0x8852, //Outdoor_aTgtWhtRgnBg_a01
0x8953, //Outdoor_aTgtWhtRgnBg_a02
0x8a57, //Outdoor_aTgtWhtRgnBg_a03
0x8b5e, //Outdoor_aTgtWhtRgnBg_a04
0x8c64, //Outdoor_aTgtWhtRgnBg_a05
0x8d6A, //Outdoor_aTgtWhtRgnBg_a06
0x8e6F, //Outdoor_aTgtWhtRgnBg_a07
0x8f75, //Outdoor_aTgtWhtRgnBg_a08
0x907c, //Outdoor_aTgtWhtRgnBg_a09
0x9184, //Outdoor_aTgtWhtRgnBg_a10
0x925D, //Outdoor_aTgtWhtRgnRgLtLmt
0x9357, //Outdoor_aTgtWhtRgnRgLtLmt_a01
0x9451, //Outdoor_aTgtWhtRgnRgLtLmt_a02
0x9550, //Outdoor_aTgtWhtRgnRgLtLmt_a03
0x964e,//Outdoor_aTgtWhtRgnRgLtLmt_a04
0x974c,//Outdoor_aTgtWhtRgnRgLtLmt_a05
0x984b,//Outdoor_aTgtWhtRgnRgLtLmt_a06
0x9949,//Outdoor_aTgtWhtRgnRgLtLmt_a07
0x9a47, //Outdoor_aTgtWhtRgnRgLtLmt_a08
0x9b46,//Outdoor_aTgtWhtRgnRgLtLmt_a09
0x9c45,//Outdoor_aTgtWhtRgnRgLtLmt_a10
0x9d64, //Outdoor_aTgtWhtRgnRgRtLmt
0x9e63, //Outdoor_aTgtWhtRgnRgRtLmt_a01
0x9f62, //Outdoor_aTgtWhtRgnRgRtLmt_a02
0xa062, //Outdoor_aTgtWhtRgnRgRtLmt_a03
0xa161, //Outdoor_aTgtWhtRgnRgRtLmt_a04
0xa260, //Outdoor_aTgtWhtRgnRgRtLmt_a05
0xa35e, //Outdoor_aTgtWhtRgnRgRtLmt_a06
0xa45d,//Outdoor_aTgtWhtRgnRgRtLmt_a07
0xa55c,//Outdoor_aTgtWhtRgnRgRtLmt_a08
0xa65a, //Outdoor_aTgtWhtRgnRgRtLmt_a09
0xa757,//Outdoor_aTgtWhtRgnRgRtLmt_a10
0xa840, //Outdoor_aOptWhtRgnBg
0xa945, //Outdoor_aOptWhtRgnBg_a01
0xaa4b, //Outdoor_aOptWhtRgnBg_a02
0xab54, //Outdoor_aOptWhtRgnBg_a03
0xac60, //Outdoor_aOptWhtRgnBg_a04
0xad6c, //Outdoor_aOptWhtRgnBg_a05
0xae76, //Outdoor_aOptWhtRgnBg_a06
0xaf7f, //Outdoor_aOptWhtRgnBg_a07
0xb08c, //Outdoor_aOptWhtRgnBg_a08
0xb195, //Outdoor_aOptWhtRgnBg_a09
0xb2a0, //Outdoor_aOptWhtRgnBg_a10
0xb36a, //Outdoor_aOptWhtRgnRgLtLmt
0xb45b, //Outdoor_aOptWhtRgnRgLtLmt_a01
0xb553, //Outdoor_aOptWhtRgnRgLtLmt_a02
0xb64c, //Outdoor_aOptWhtRgnRgLtLmt_a03
0xb746, //Outdoor_aOptWhtRgnRgLtLmt_a04
0xb842, //Outdoor_aOptWhtRgnRgLtLmt_a05
0xb93e, //Outdoor_aOptWhtRgnRgLtLmt_a06
0xba3c, //Outdoor_aOptWhtRgnRgLtLmt_a07
0xbb3a, //Outdoor_aOptWhtRgnRgLtLmt_a08
0xbc39, //Outdoor_aOptWhtRgnRgLtLmt_a09
0xbd37, //Outdoor_aOptWhtRgnRgLtLmt_a10
0xbe7d, //Outdoor_aOptWhtRgnRgRtLmt
0xbf7c, //Outdoor_aOptWhtRgnRgRtLmt_a01
0xc079, //Outdoor_aOptWhtRgnRgRtLmt_a02
0xc176, //Outdoor_aOptWhtRgnRgRtLmt_a03
0xc26f, //Outdoor_aOptWhtRgnRgRtLmt_a04
0xc36a, //Outdoor_aOptWhtRgnRgRtLmt_a05
0xc466, //Outdoor_aOptWhtRgnRgRtLmt_a06
0xc563, //Outdoor_aOptWhtRgnRgRtLmt_a07
0xc65B, //Outdoor_aOptWhtRgnRgRtLmt_a08
0xc754, //Outdoor_aOptWhtRgnRgRtLmt_a09
0xc84a, //Outdoor_aOptWhtRgnRgRtLmt_a10

0xc942, //Outdoor_aCtmpWgtWdhTh
0xca4c,//Outdoor_aCtmpWgtWdhTh_a01
0xcb54,//Outdoor_aCtmpWgtWdhTh_a02
0xcc5c,//Outdoor_aCtmpWgtWdhTh_a03
0xcd64,//Outdoor_aCtmpWgtWdhTh_a04
0xce6c,//Outdoor_aCtmpWgtWdhTh_a05
0xcf74,//Outdoor_aCtmpWgtWdhTh_a06
0xd042, //Outdoor_aCtmpWgtHgtTh
0xd152, //Outdoor_aCtmpWgtHgtTh_a01
0xd258, //Outdoor_aCtmpWgtHgtTh_a02
0xd35e, //Outdoor_aCtmpWgtHgtTh_a03
0xd464, //Outdoor_aCtmpWgtHgtTh_a04
0xd56a, //Outdoor_aCtmpWgtHgtTh_a05
0xd672, //Outdoor_aCtmpWgtHgtTh_a06
0xd77a, //Outdoor_aCtmpWgtHgtTh_a07
0xd888, //Outdoor_aCtmpWgtHgtTh_a08
0xd911, //Outdoor_aCtmpWgt
0xda23,//Outdoor_aCtmpWgt_a01
0xdb22,//Outdoor_aCtmpWgt_a02
0xdc11, //Outdoor_aCtmpWgt_a03
0xdd22,//Outdoor_aCtmpWgt_a04
0xde22, //Outdoor_aCtmpWgt_a05
0xdf11, //Outdoor_aCtmpWgt_a06
0xe033, //Outdoor_aCtmpWgt_a07
0xe131,//Outdoor_aCtmpWgt_a08
0xe212, //Outdoor_aCtmpWgt_a09
0xe366,//Outdoor_aCtmpWgt_a10
0xe441,//Outdoor_aCtmpWgt_a11
0xe513, //Outdoor_aCtmpWgt_a12
0xe677,//Outdoor_aCtmpWgt_a13
0xe741,//Outdoor_aCtmpWgt_a14
0xe813,//Outdoor_aCtmpWgt_a15
0xe974, //Outdoor_aCtmpWgt_a16
0xea11, //Outdoor_aCtmpWgt_a17
0xeb23,//Outdoor_aCtmpWgt_a18
0xec53,//Outdoor_aCtmpWgt_a19
0xed11, //Outdoor_aCtmpWgt_a20
0xee43,//Outdoor_aCtmpWgt_a21
0xef31,//Outdoor_aCtmpWgt_a22
0xf011, //Outdoor_aCtmpWgt_a23
0xf111, //Outdoor_aCtmpWgt_a24

0xf212,//aYlvlWgt
0xf334,//aYlvlWgt_a01
0xf443,//aYlvlWgt_a02
0xf532,//aYlvlWgt_a03
0xf622,//aYlvlWgt_a04
0xf711, //aYlvlWgt_a05
0xf811, //aYlvlWgt_a06
0xf911, //aYlvlWgt_a07
0xfa11, //aYlvlWgt_a08
0xfb11, //aYlvlWgt_a09
0xfc11,//aYlvlWgt_a10
0xfd11, //aYlvlWgt_a11
0x0e00, // burst end

//Page ca
0x03ca,
0x0e01, // burst start

0x1011, //aYlvlWgt_a12
0x1122, //aYlvlWgt_a13
0x1222, //aYlvlWgt_a14
0x1311, //aYlvlWgt_a15

0x1464, //Outdoor_aTgtAngle
0x156b, //Outdoor_aTgtAngle_a01
0x1670, //Outdoor_aTgtAngle_a02
0x177a,//Outdoor_aTgtAngle_a03
0x1884,//Outdoor_aTgtAngle_a04
0x191a,//Outdoor_aRgTgtOfs
0x1a12,//Outdoor_aRgTgtOfs_a01
0x1b0a,//Outdoor_aRgTgtOfs_a02
0x1c04,//Outdoor_aRgTgtOfs_a03
0x1d04,//Outdoor_aRgTgtOfs_a04
0x1e9e,//Outdoor_aBgTgtOfs
0x1f88, //Outdoor_aBgTgtOfs_a01
0x2086,//Outdoor_aBgTgtOfs_a02
0x2182,//Outdoor_aBgTgtOfs_a03
0x2280,//Outdoor_aBgTgtOfs_a04
0x2384,//Outdoor_bRgDefTgt
0x2488, //Outdoor_bBgDefTgt

0x251c, //Outdoor_aWhtPtTrcAglOfs
0x261a, //Outdoor_aWhtPtTrcAglOfs_a01
0x2718, //Outdoor_aWhtPtTrcAglOfs_a02
0x2816, //Outdoor_aWhtPtTrcAglOfs_a03
0x2914, //Outdoor_aWhtPtTrcAglOfs_a04
0x2a12, //Outdoor_aWhtPtTrcAglOfs_a05
0x2b10, //Outdoor_aWhtPtTrcAglOfs_a06
0x2c0f, //Outdoor_aWhtPtTrcAglOfs_a07
0x2d0e, //Outdoor_aWhtPtTrcAglOfs_a08
0x2e0e, //Outdoor_aWhtPtTrcAglOfs_a09
0x2f0a, //Outdoor_bWhtPtTrcCnt
0x3028, //Outdoor_aRtoDiffThNrBp
0x3148, //Outdoor_aRtoDiffThNrBp_a01
0x3228, //Outdoor_aAglDiffThTrWhtPt
0x3350, //Outdoor_aAglDiffThTrWhtPt_a01
0x34aa, //Outdoor_bWgtRatioTh1
0x35a0, //Outdoor_bWgtRatioTh2
0x360a, //Outdoor_bWgtOfsTh1
0x37a0, //Outdoor_bWgtOfsTh2
0x386d, //Outdoor_bWhtPtCorAglMin
0x3978, //Outdoor_bWhtPtCorAglMax
0x3a04, //Outdoor_bYlvlMin
0x3bf8, //Outdoor_bYlvlMax
0x3c28, //Outdoor_bPxlWgtLmtLoTh
0x3d78, //Outdoor_bPxlWgtLmtHiTh
0x3e00, //Outdoor_SplBldWgt_1
0x3f00, //Outdoor_SplBldWgt_2
0x4064, //Outdoor_SplBldWgt_3
0x4160, //Outdoor_TgtOff_StdHiTh
0x4230, //Outdoor_TgtOff_StdLoTh
0x4304,
0x44c0,
0x4507,
0x46c0,
0x4702, //Outdoor_aRatioBox
0x48b2, //Outdoor_aRatioBox_a01
0x4905, //Outdoor_aRatioBox_a02
0x4adc, //Outdoor_aRatioBox_a03
0x4b0a, //Outdoor_aRatioBox_a04
0x4c28, //Outdoor_aRatioBox_a05
0x4d0c, //Outdoor_aRatioBox_a06
0x4e1c, //Outdoor_aRatioBox_a07
0x4f02, //Outdoor_aRatioBox_a08
0x50ee, //Outdoor_aRatioBox_a09
0x5106, //Outdoor_aRatioBox_a10
0x5272, //Outdoor_aRatioBox_a11
0x5308, //Outdoor_aRatioBox_a12
0x5498, //Outdoor_aRatioBox_a13
0x550a, //Outdoor_aRatioBox_a14
0x56f0, //Outdoor_aRatioBox_a15
0x5703, //Outdoor_aRatioBox_a16
0x5820, //Outdoor_aRatioBox_a17
0x5907, //Outdoor_aRatioBox_a18
0x5a08, //Outdoor_aRatioBox_a19
0x5b07, //Outdoor_aRatioBox_a20
0x5c6c, //Outdoor_aRatioBox_a21
0x5d09, //Outdoor_aRatioBox_a22
0x5e60, //Outdoor_aRatioBox_a23
0x5f03, //Outdoor_aRatioBox_a24
0x6084, //Outdoor_aRatioBox_a25
0x6107, //Outdoor_aRatioBox_a26
0x62d0, //Outdoor_aRatioBox_a27
0x6306, //Outdoor_aRatioBox_a28
0x6440, //Outdoor_aRatioBox_a29
0x6508, //Outdoor_aRatioBox_a30
0x6634, //Outdoor_aRatioBox_a31
0x6703, //Outdoor_aRatioBox_a32
0x68e8, //Outdoor_aRatioBox_a33
0x6908, //Outdoor_aRatioBox_a34
0x6ad0, //Outdoor_aRatioBox_a35
0x6b04, //Outdoor_aRatioBox_a36
0x6c4c, //Outdoor_aRatioBox_a37
0x6d07, //Outdoor_aRatioBox_a38
0x6e08, //Outdoor_aRatioBox_a39

0x6f04,
0x7000,

0x7105, //Out2_Adt_RgainMin
0x7200, //Out2_Adt_RgainMin_n01
0x7305, //Out2_Adt_RgainMax
0x74d0,//Out2_Adt_RgainMax_n01
0x7504, //Out2_Adt_GgainMin
0x7600, //Out2_Adt_GgainMin_n01
0x7704, //Out2_Adt_GgainMax
0x7800, //Out2_Adt_GgainMax_n01
0x7905, //Out2_Adt_BgainMin
0x7ae0, //Out2_Adt_BgainMin_n01
0x7b07, //Out2_Adt_BgainMax
0x7c00,//Out2_Adt_BgainMax_n01

0x7d05, //Out1_Adt_RgainMin
0x7e40,//Out1_Adt_RgainMin_n01
0x7f06, //Out1_Adt_RgainMax
0x8080, //Out1_Adt_RgainMax_n01
0x8104, //Out1_Adt_GgainMin
0x8200, //Out1_Adt_GgainMin_n01
0x8304, //Out1_Adt_GgainMax
0x8400, //Out1_Adt_GgainMax_n01
0x8505, //Out1_Adt_BgainMin
0x8680, //Out1_Adt_BgainMin_n01
0x8707, //Out1_Adt_BgainMax
0x88e0, //Out1_Adt_BgainMax_n01

0x8904, //In_Adt_RgainMin
0x8a00, //In_Adt_RgainMin_n01
0x8b0d, //In_Adt_RgainMax
0x8c00, //In_Adt_RgainMax_n01
0x8d04, //In_Adt_GgainMin
0x8e00, //In_Adt_GgainMin_n01
0x8f05, //In_Adt_GgainMax
0x9080, //In_Adt_GgainMax_n01
0x9104, //In_Adt_BgainMin
0x9200, //In_Adt_BgainMin_n01
0x930d, //In_Adt_BgainMax
0x9480, //In_Adt_BgainMax_n01

0x9504, //Manual_Adt_RgainMin
0x9600, //Manual_Adt_RgainMin_n01
0x970d, //Manual_Adt_RgainMax
0x9800, //Manual_Adt_RgainMax_n01
0x9904, //Manual_Adt_GgainMin
0x9a00, //Manual_Adt_GgainMin_n01
0x9b04, //Manual_Adt_GgainMax
0x9c80, //Manual_Adt_GgainMax_n01
0x9d04, //Manual_Adt_BgainMin
0x9e00, //Manual_Adt_BgainMin_n01
0x9f0b, //Manual_Adt_BgainMax
0xa000, //Manual_Adt_BgainMax_n01

0x0e00, //burst end

0x03C8,
0x11C3,	//AWB reset


/////////////////////////////////////////////////////////////////////////////////
// CD page(OTP control)
/////////////////////////////////////////////////////////////////////////////////
0x03CD,
0x1003,

0x2210,
//Manual Typical colo ratio write
0x271A, //Typical RG=0.685*1000 = 6850 = 1AC2
0x28C2,
0x2910, //Typical BG=0.430*1000 = 4300 = 10CC
0x2ACC,
0x2b0a,//+/-10 valid ratio check

///////////////////////////////////////////////////////////////////////////////
// Color ratio setting
/////////////////////////////////////////////////////////////////////////////////
0x03CE,
0x3098,	//Color ratio on
0x3101,
0x3304,	//R gain def
0x3400,
0x3504,	//G gain def
0x3600,
0x3704,	//B gain def
0x3800,

0x4500, //Outdoor In EvTh
0x4600,
0x4727,
0x4810,
0x4900, //Outdoor Out EvTh
0x4a00,
0x4b4e,
0x4c20,

0x553e, //Low In Th
0x564c, //Low Out Th
0x575c, //High Out Th
0x586c, //High In Th

0x6286, //Out weight
0x6386, //Indoor weight
0x6488, //Dark weight
0x65d8, //Low weight
0x6686, //High weight
///////////////////////////////////////////////////////////////////////////////
// D3 ~ D8 Page (Adaptive)
///////////////////////////////////////////////////////////////////////////////

0x03d3,	// Adaptive start

0x0e01, // burst start

0x1000,
0x1100,
0x1200,
0x1300,
0x1400,
0x1500,
0x1600,
0x1700,
0x1800,
0x1900,

0x1a00,	// Def_Yoffset
0x1b23,	// DYOFS_Ratio
0x1c04,	// DYOFS_Limit

0x1d00,//EV Th OutEnd
0x1e00,
0x1f20,
0x208d,

0x2100,//EV Th OutStr def :  80fps  Ag 1x Dg 1x
0x2200,
0x2330,
0x24d4,

0x2500,	//EV Th Dark1Str
0x260b,
0x2771,
0x28b0,

0x2900,	//EV Th Dark1End
0x2a0d,
0x2bbb,
0x2ca0,

0x2d00,	//EV Th Dark2Str
0x2e12,
0x2f76,
0x3090,

0x3100,	//EV Th Dark2End
0x321e,
0x3384,
0x3480,

0x354b, //Ctmp LT End
0x3652, //Ctmp LT Str
0x3769,	//Ctmp HT Str
0x3873,	//Ctmp HT End

0x3900,	// LSC_EvTh_OutEnd_4
0x3a00,	// LSC_EvTh_OutEnd_3
0x3b13,	// LSC_EvTh_OutEnd_2
0x3c88,	// LSC_EvTh_OutEnd_1    def : 200fps  Ag 1x Dg 1x

0x3d00,	// LSC_EvTh_OutStr_4
0x3e00,	// LSC_EvTh_OutStr_3
0x3f30,	// LSC_EvTh_OutStr_2
0x40d4,	// LSC_EvTh_OutStr_1    def :  80fps  Ag 1x Dg 1x

0x4100,	// LSC_EvTh_Dark1Str_4
0x4205,	// LSC_EvTh_Dark1Str_3
0x43b8,	// LSC_EvTh_Dark1Str_2
0x44d8,	// LSC_EvTh_Dark1Str_1  def :  8fps  Ag 3x Dg 1x

0x4500,	// LSC_EvTh_Dark1End_4
0x460b,	// LSC_EvTh_Dark1End_3
0x4771,	// LSC_EvTh_Dark1End_2
0x48b0,	// LSC_EvTh_Dark1End_1  def :  8fps  Ag 6x Dg 1x

0x4900,	// LSC_EvTh_Dark2Str_4
0x4a0f,	// LSC_EvTh_Dark2Str_3
0x4b42,	// LSC_EvTh_Dark2Str_2
0x4c40,	// LSC_EvTh_Dark2Str_1  def :  8fps  Ag 8x Dg 1x

0x4d00,	// LSC_EvTh_Dark2End_4
0x4e1e,	// LSC_EvTh_Dark2End_3
0x4f84,	// LSC_EvTh_Dark2End_2
0x5080,	// LSC_EvTh_Dark2End_1  def :  4fps  Ag 8x Dg 1x

0x5155,//LSC Ctmp LTEnd Out
0x5264,	//LSC Ctmp LTStr Out
0x5378,	//LSC Ctmp HTStr Out
0x5486,	//LSC Ctmp HTEnd Out

0x5546,	//LSC Ctmp LTEnd In
0x5656,	//LSC Ctmp LTStr In
0x576e,	//LSC Ctmp HTStr In
0x5876,	//LSC Ctmp HTEnd In

0x5950,	// LSC_CTmpTh_LT_End_Dark
0x5a78,	// LSC_CTmpTh_LT_Str_Dark
0x5ba0,	// LSC_CTmpTh_HT_Str_Dark
0x5cb4,	// LSC_CTmpTh_HT_End_Dark

0x5d00,	// UniScn_EvMinTh_4
0x5e00,	// UniScn_EvMinTh_3
0x5f04,	// UniScn_EvMinTh_2
0x60e2,	// UniScn_EvMinTh_1    def : 600fps  Ag 1x Dg 1x

0x6100,	// UniScn_EvMaxTh_4
0x6205,	// UniScn_EvMaxTh_3
0x63b8,	// UniScn_EvMaxTh_2
0x64d8,	// UniScn_EvMaxTh_1     def :  8fps  Ag 3x Dg 1x

0x654e,	// UniScn_AglMinTh_1
0x6650,	// UniScn_AglMinTh_2
0x6773,	// UniScn_AglMaxTh_1
0x687d,	// UniScn_AglMaxTh_2

0x6903,	// UniScn_YstdMinTh
0x6a0a,	// UniScn_YstdMaxTh
0x6b1e,	// UniScn_BPstdMinTh
0x6c34,	// UniScn_BPstdMaxTh

0x6d64,	// Ytgt_ColWgt_Out
0x6e64,	// Ytgt_ColWgt_Dark
0x6f64,	// ColSat_ColWgt_Out
0x7064,	// ColSat_ColWgt_Dark
0x7164,	// CMC_ColWgt_Out
0x7264,	// CMC_ColWgt_Dark
0x7364,	// MCMC_ColWgt_Out
0x7464,	// MCMC_ColWgt_Dark
0x7564,	// CustomReg_CorWgt_Out
0x7664,	// CustomReg_CorWgt_Dark

0x7764,	// UniScn_Y_Ratio
0x7850,	// UniScn_Cb_Ratio
0x7950,	// UniScn_Cr_Ratio

0x7a00,	// Ytgt_offset
0x7b00,	// CbSat_offset
0x7c00,	// CrSat_offset

0x7d34,	// Y_target_Outdoor
0x7e34,	// Y_target_Indoor
0x7f34,	// Y_target_Dark1
0x8034,	// Y_target_Dark2
0x8135,// Y_target_LowTemp
0x822f,// Y_target_HighTemp

0x8398, // Cb_Outdoor
0x8495,	// Cb _Sat_Indoor
0x85a0,	// Cb _Sat_Dark1
0x868a,	// Cb _Sat_Dark2
0x8788,	// Cb _Sat_LowTemp
0x8892,	// Cb _Sat_HighTemp

0x8988,	// Cr _Sat_Outdoor
0x8a90,	// Cr _Sat_Indoor
0x8ba0,	// Cr _Sat_Dark1
0x8c84,	// Cr _Sat_Dark2
0x8d75,	// Cr _Sat_LowTemp
0x8e8c,	// Cr _Sat_HighTemp

0x8f82,	// BLC_ofs_r_Outdoor
0x9081,	// BLC_ofs_b_Outdoor
0x9182,	// BLC_ofs_gr_Outdoor
0x9282,	// BLC_ofs_gb_Outdoor

0x9381,	// BLC_ofs_r_Indoor
0x9480,	// BLC_ofs_b_Indoor
0x9581,	// BLC_ofs_gr_Indoor
0x9681,	// BLC_ofs_gb_Indoor

0x9782,	// BLC_ofs_r_Dark1
0x9882,	// BLC_ofs_b_Dark1
0x9982,	// BLC_ofs_gr_Dark1
0x9a82,	// BLC_ofs_gb_Dark1

0x9b82,	// BLC_ofs_r_Dark2
0x9c82,	// BLC_ofs_b_Dark2
0x9d82,	// BLC_ofs_gr_Dark2
0x9e82,	// BLC_ofs_gb_Dark2

0x9f00,	//LSC Out_L ofs G
0xa000,	//LSC Out_L ofs B
0xa100,	//LSC Out_L ofs R
0xa280,	//LSC Out_L Gain G
0xa382,	//LSC Out_L Gain B
0xa486,	//LSC Out_L Gain R

0xa500,	//LSC Out_M ofs G
0xa600,	//LSC Out_M ofs B
0xa700,	//LSC Out_M ofs R
0xa880,	//LSC Out_M Gain G
0xa982,	//LSC Out_M Gain B
0xaa80,	//LSC Out_M Gain R

0xab00,	//LSC Out_H ofs G
0xac00,	//LSC Out_H ofs B
0xad00,	//LSC Out_H ofs R
0xae80,	//LSC Out_H Gain G
0xaf84,	//LSC Out_H Gain B
0xb078,	//LSC Out_H Gain R

0xb100,	// LSC0_Ind_LowTmp        offset g
0xb200,	// LSC1_Ind_LowTmp        offset b
0xb300,	// LSC2_Ind_LowTmp        offset r
0xb480,	// LSC3_Ind_LowTmp        gain g
0xb580,	// LSC4_Ind_LowTmp        gain b
0xb688,	// LSC5_Ind_LowTmp        gain r

0xb700,	// LSC0_Ind_MiddleTmp     offset g
0xb800,	// LSC1_Ind_MiddleTmp     offset b
0xb900,	// LSC2_Ind_MiddleTmp     offset r
0xba80,	// LSC3_Ind_MiddleTmp     gain g
0xbb80,	// LSC4_Ind_MiddleTmp     gain b
0xbc7e,	// LSC5_Ind_MiddleTmp     gain r

0xbd00,	// LSC0_Ind_HighTmp       offset g
0xbe00,	// LSC1_Ind_HighTmp       offset b
0xbf00,	// LSC2_Ind_HighTmp       offset r
0xc080,	// LSC3_Ind_HighTmp       gain g
0xc180,	// LSC4_Ind_HighTmp       gain b
0xc27e,	// LSC5_Ind_HighTmp       gain r

0xc300,	// LSC0_Dark1_LowTmp      offset g
0xc400,	// LSC1_Dark1_LowTmp      offset b
0xc500,	// LSC2_Dark1_LowTmp      offset r
0xc668,	// LSC3_Dark1_LowTmp      gain g
0xc768,	// LSC4_Dark1_LowTmp      gain b
0xc868,	// LSC5_Dark1_LowTmp      gain r

0xc900,	// LSC0_Dark1_MiddleTmp   offset g
0xca00,	// LSC1_Dark1_MiddleTmp   offset b
0xcb00,	// LSC2_Dark1_MiddleTmp   offset r
0xcc68,	// LSC3_Dark1_MiddleTmp   gain g
0xcd68,	// LSC4_Dark1_MiddleTmp   gain b
0xce68,	// LSC5_Dark1_MiddleTmp   gain r

0xcf00,	// LSC0_Dark1_HighTmp   offset g
0xd000,	// LSC1_Dark1_HighTmp   offset b
0xd100,	// LSC2_Dark1_HighTmp   offset r
0xd268,	// LSC3_Dark1_HighTmp   gain g
0xd368,	// LSC4_Dark1_HighTmp   gain b
0xd468,	// LSC5_Dark1_HighTmp   gain r

0xd500,	// LSC0_Dark2           offset g
0xd600,	// LSC1_Dark2           offset b
0xd700,	// LSC2_Dark2           offset r
0xd868,	// LSC3_Dark2           gain g
0xd968,	// LSC4_Dark2           gain b
0xda68,	// LSC5_Dark2           gain r

0xdb2f, //CMCSIGN_Out
0xdc55, //CMC_Out_00
0xdd1c, //CMC_Out_01
0xde07, //CMC_Out_02
0xdf0a, //CMC_Out_03
0xe051, //CMC_Out_04
0xe107, //CMC_Out_05
0xe201, //CMC_Out_06
0xe314, //CMC_Out_07
0xe455, //CMC_Out_08

0xe504,	// CMC_Out_LumTh1      CMC SP gain axis X(luminance)
0xe60a,	// CMC_Out_LumTh2
0xe710,	// CMC_Out_LumTh3
0xe818,	// CMC_Out_LumTh4
0xe920,	// CMC_Out_LumTh5
0xea28,	// CMC_Out_LumTh6
0xeb40,	// CMC_Out_LumTh7

0xec20,	// CMC_Out_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0xed20,	// CMC_Out_LumGain2_R
0xee20,	// CMC_Out_LumGain3_R
0xef20,	// CMC_Out_LumGain4_R
0xf020,	// CMC_Out_LumGain5_R
0xf120,	// CMC_Out_LumGain6_R
0xf220,	// CMC_Out_LumGain7_R
0xf320,	// CMC_Out_LumGain8_R    20 = x1.0

0xf420,	// CMC_Out_LumGain1_G  CMC SP G gain axis Y (gain):: max32
0xf520,	// CMC_Out_LumGain2_G
0xf620,	// CMC_Out_LumGain3_G
0xf720,	// CMC_Out_LumGain4_G
0xf820,	// CMC_Out_LumGain5_G
0xf920,	// CMC_Out_LumGain6_G
0xfa20,	// CMC_Out_LumGain7_G
0xfb20,	// CMC_Out_LumGain8_G    20 = x1.0

0xfc20,	// CMC_Out_LumGain1_B  CMC SP B gain axis Y (gain):: max32
0xfd20,	// CMC_Out_LumGain2_B
0x0e00, // burst end

0x03d4,	// page D4
0x0e01, // burst start

0x1020,	// CMC_Out_LumGain3_B
0x1120,	// CMC_Out_LumGain4_B
0x1220,	// CMC_Out_LumGain5_B
0x1320,	// CMC_Out_LumGain6_B
0x1420,	// CMC_Out_LumGain7_B
0x1520,	// CMC_Out_LumGain8_B    20 = x1.0

0x162f, //CMCSIGN_In_Mid
0x1753, //CMC_In_Mid_00
0x1816, //CMC_In_Mid_01
0x1903, //CMC_In_Mid_02
0x1a10, //CMC_In_Mid_03
0x1b53, //CMC_In_Mid_04
0x1c03, //CMC_In_Mid_05
0x1d04, //CMC_In_Mid_06
0x1e1d, //CMC_In_Mid_07
0x1f61, //CMC_In_Mid_08

0x2004,	// CMC_Ind_LumTh1     CMC SP gain axis X(luminance)
0x210a,	// CMC_Ind_LumTh2
0x2210,	// CMC_Ind_LumTh3
0x2318,	// CMC_Ind_LumTh4
0x2420,	// CMC_Ind_LumTh5
0x2528,	// CMC_Ind_LumTh6
0x2640,	// CMC_Ind_LumTh7

0x2708,	// CMC_Ind_LumGain1_R   CMC SP R gain axis Y (gain):: max32
0x2812,	// CMC_Ind_LumGain2_R
0x2918,	// CMC_Ind_LumGain3_R
0x2a1c,	// CMC_Ind_LumGain4_R
0x2b1e,	// CMC_Ind_LumGain5_R
0x2c20,	// CMC_Ind_LumGain6_R
0x2d20,	// CMC_Ind_LumGain7_R
0x2e20,	// CMC_Ind_LumGain8_R    20 = x1.0

0x2f08,	// CMC_Ind_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x3012,	// CMC_Ind_LumGain2_G
0x3118,	// CMC_Ind_LumGain3_G
0x321c,	// CMC_Ind_LumGain4_G
0x331e,	// CMC_Ind_LumGain5_G
0x3420,	// CMC_Ind_LumGain6_G
0x3520,	// CMC_Ind_LumGain7_G
0x3620,	// CMC_Ind_LumGain8_G    20 = x1.0

0x3708,	// CMC_Ind_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x3812,	// CMC_Ind_LumGain2_B
0x3918,	// CMC_Ind_LumGain3_B
0x3a1c,	// CMC_Ind_LumGain4_B
0x3b1e,	// CMC_Ind_LumGain5_B
0x3c20,	// CMC_Ind_LumGain6_B
0x3d20,	// CMC_Ind_LumGain7_B
0x3e20,	// CMC_Ind_LumGain8_B   20 = x1.0

0x3f2f, //CMCSIGN_Dark1
0x4053, //CMC_Dark1_00
0x411c, //CMC_Dark1_01
0x4209, //CMC_Dark1_02
0x430e, //CMC_Dark1_03
0x4453, //CMC_Dark1_04
0x4505, //CMC_Dark1_05
0x4603, //CMC_Dark1_06
0x4723, //CMC_Dark1_07
0x4866, //CMC_Dark1_08

0x4904,	// CMC_Dark1_LumTh1     CMC SP gain axis X(luminance)
0x4a0a,	// CMC_Dark1_LumTh2
0x4b10,	// CMC_Dark1_LumTh3
0x4c18,	// CMC_Dark1_LumTh4
0x4d20,	// CMC_Dark1_LumTh5
0x4e28,	// CMC_Dark1_LumTh6
0x4f40,	// CMC_Dark1_LumTh7

0x5008,	// CMC_Dark1_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0x5112,	// CMC_Dark1_LumGain2_R
0x5218,	// CMC_Dark1_LumGain3_R
0x531c,	// CMC_Dark1_LumGain4_R
0x541e,	// CMC_Dark1_LumGain5_R
0x5520,	// CMC_Dark1_LumGain6_R
0x5620,	// CMC_Dark1_LumGain7_R
0x5720,	// CMC_Dark1_LumGain8_R    20 = x1.0

0x5808,	// CMC_Dark1_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x5912,	// CMC_Dark1_LumGain2_G
0x5a18,	// CMC_Dark1_LumGain3_G
0x5b1c,	// CMC_Dark1_LumGain4_G
0x5c1e,	// CMC_Dark1_LumGain5_G
0x5d20,	// CMC_Dark1_LumGain6_G
0x5e20,	// CMC_Dark1_LumGain7_G
0x5f20,	// CMC_Dark1_LumGain8_G    20 = x1.0

0x6008,	// CMC_Dark1_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x6112,	// CMC_Dark1_LumGain2_B
0x6218,	// CMC_Dark1_LumGain3_B
0x631c,	// CMC_Dark1_LumGain4_B
0x641e,	// CMC_Dark1_LumGain5_B
0x6520,	// CMC_Dark1_LumGain6_B
0x6620,	// CMC_Dark1_LumGain7_B
0x6720,	// CMC_Dark1_LumGain8_B   20 = x1.0

0x682f, //CMCSIGN_Dark2
0x6953, //CMC_Dark2_00
0x6a1c, //CMC_Dark2_01
0x6b09, //CMC_Dark2_02
0x6c0e, //CMC_Dark2_03
0x6d53, //CMC_Dark2_04
0x6e05, //CMC_Dark2_05
0x6f03, //CMC_Dark2_06
0x7023, //CMC_Dark2_07
0x7166, //CMC_Dark2_08

0x7204,	// CMC_Dark2_LumTh1        CMC SP gain axis X(luminance)
0x730a,	// CMC_Dark2_LumTh2
0x7410,	// CMC_Dark2_LumTh3
0x7518,	// CMC_Dark2_LumTh4
0x7620,	// CMC_Dark2_LumTh5
0x7728,	// CMC_Dark2_LumTh6
0x7840,	// CMC_Dark2_LumTh7

0x7915,	// CMC_Dark2_LumGain1_R    CMC SP R gain
0x7a18,	// CMC_Dark2_LumGain2_R
0x7b1e,	// CMC_Dark2_LumGain3_R
0x7c1f,	// CMC_Dark2_LumGain4_R
0x7d20,	// CMC_Dark2_LumGain5_R
0x7e20,	// CMC_Dark2_LumGain6_R
0x7f20,	// CMC_Dark2_LumGain7_R
0x8020,	// CMC_Dark2_LumGain8_R    20 = x1.

0x8115,	// CMC_Dark2_LumGain1_G    CMC SP G gain
0x8218,	// CMC_Dark2_LumGain2_G
0x831e,	// CMC_Dark2_LumGain3_G
0x841f,	// CMC_Dark2_LumGain4_G
0x8520,	// CMC_Dark2_LumGain5_G
0x8620,	// CMC_Dark2_LumGain6_G
0x8720,	// CMC_Dark2_LumGain7_G
0x8820,	// CMC_Dark2_LumGain8_G    20 = x1.

0x8915,	// CMC_Dark2_LumGain1_B    CMC SP B gain
0x8a18,	// CMC_Dark2_LumGain2_B
0x8b1e,	// CMC_Dark2_LumGain3_B
0x8c1f,	// CMC_Dark2_LumGain4_B
0x8d20,	// CMC_Dark2_LumGain5_B
0x8e20,	// CMC_Dark2_LumGain6_B
0x8f20,	// CMC_Dark2_LumGain7_B
0x9020,	// CMC_Dark2_LumGain8_B    20 = x1.0

0x912f, // CMCSIGN_In_Low
0x9253, // CMC_In_Low_00
0x931e, //CMC_In_Low_01
0x940b, //CMC_In_Low_02
0x9518, //CMC_In_Low_03
0x9661, // CMC_In_Low_04
0x9709, //CMC_In_Low_05
0x9804, //CMC_In_Low_06
0x9914, //CMC_In_Low_07
0x9a58, // CMC_In_Low_08

0x9b04,	// CMC_LowTemp_LumTh1     CMC SP gain axis X(luminance)
0x9c0a,	// CMC_LowTemp_LumTh2
0x9d10,	// CMC_LowTemp_LumTh3
0x9e18,	// CMC_LowTemp_LumTh4
0x9f20,	// CMC_LowTemp_LumTh5
0xa028,	// CMC_LowTemp_LumTh6
0xa140,	// CMC_LowTemp_LumTh7

0xa220,	// CMC_LowTemp_LumGain1_R    CMC SP R gain
0xa320,	// CMC_LowTemp_LumGain2_R
0xa420,	// CMC_LowTemp_LumGain3_R
0xa520,	// CMC_LowTemp_LumGain4_R
0xa620,	// CMC_LowTemp_LumGain5_R
0xa720,	// CMC_LowTemp_LumGain6_R
0xa820,	// CMC_LowTemp_LumGain7_R
0xa920,	// CMC_LowTemp_LumGain8_R    20 = x1.0

0xaa20,	// CMC_LowTemp_LumGain1_G    CMC SP G gain
0xab20,	// CMC_LowTemp_LumGain2_G
0xac20,	// CMC_LowTemp_LumGain3_G
0xad20,	// CMC_LowTemp_LumGain4_G
0xae20,	// CMC_LowTemp_LumGain5_G
0xaf20,	// CMC_LowTemp_LumGain6_G
0xb020,	// CMC_LowTemp_LumGain7_G
0xb120,	// CMC_LowTemp_LumGain8_G    20 = x1.0

0xb220,	// CMC_LowTemp_LumGain1_B    CMC SP B gain
0xb320,	// CMC_LowTemp_LumGain2_B
0xb420,	// CMC_LowTemp_LumGain3_B
0xb520,	// CMC_LowTemp_LumGain4_B
0xb620,	// CMC_LowTemp_LumGain5_B
0xb720,	// CMC_LowTemp_LumGain6_B
0xb820,	// CMC_LowTemp_LumGain7_B
0xb920,	// CMC_LowTemp_LumGain8_B    20 = x1.0

0xba2d, //CMCSIGN_In_High
0xbb55, //CMC_In_High_00
0xbc21, //CMC_In_High_01
0xbd0c, //CMC_In_High_02
0xbe08, //CMC_In_High_03
0xbf55, //CMC_In_High_04
0xc00d, //CMC_In_High_05
0xc103, //CMC_In_High_06
0xc218, //CMC_In_High_07
0xc355, //CMC_In_High_08

0xc404,	// CMC_HighTemp_LumTh1       CMC SP gain axis X(luminance)
0xc50a,	// CMC_HighTemp_LumTh2
0xc610,	// CMC_HighTemp_LumTh3
0xc718,	// CMC_HighTemp_LumTh4
0xc820,	// CMC_HighTemp_LumTh5
0xc928,	// CMC_HighTemp_LumTh6
0xca40,	// CMC_HighTemp_LumTh7

0xcb20,	// CMC_HighTemp_LumGain1_R   CMC SP R gain
0xcc20,	// CMC_HighTemp_LumGain2_R
0xcd20,	// CMC_HighTemp_LumGain3_R
0xce20,	// CMC_HighTemp_LumGain4_R
0xcf20,	// CMC_HighTemp_LumGain5_R
0xd020,	// CMC_HighTemp_LumGain6_R
0xd120,	// CMC_HighTemp_LumGain7_R
0xd220,	// CMC_HighTemp_LumGain8_R    20 = x1.0

0xd320,	// CMC_HighTemp_LumGain1_G   CMC SP G gain
0xd420,	// CMC_HighTemp_LumGain2_G
0xd520,	// CMC_HighTemp_LumGain3_G
0xd620,	// CMC_HighTemp_LumGain4_G
0xd720,	// CMC_HighTemp_LumGain5_G
0xd820,	// CMC_HighTemp_LumGain6_G
0xd920,	// CMC_HighTemp_LumGain7_G
0xda20,	// CMC_HighTemp_LumGain8_G    20 = x1.

0xdb20,	// CMC_HighTemp_LumGain1_B   CMC SP B gain
0xdc20,	// CMC_HighTemp_LumGain2_B
0xdd20,	// CMC_HighTemp_LumGain3_B
0xde20,	// CMC_HighTemp_LumGain4_B
0xdf20,	// CMC_HighTemp_LumGain5_B
0xe020,	// CMC_HighTemp_LumGain6_B
0xe120,	// CMC_HighTemp_LumGain7_B
0xe220,	// CMC_HighTemp_LumGain8_B   20 = x1.0

////////////////////
// Adaptive Gamma //
////////////////////

0xe300,	// GMA_OUT
0xe403,
0xe508,
0xe60f,
0xe715,
0xe820,
0xe92d,
0xea3b,
0xeb47,
0xec53,
0xed5c,
0xee65,
0xef6d,
0xf075,
0xf17c,
0xf282,
0xf388,
0xf48d,
0xf591,
0xf696,
0xf79a,
0xf8a1,
0xf9a9,
0xfab0,
0xfbbe,
0xfcc8,
0xfdd2,

0x0e00, // burst end

0x03d5,	// Page d5

0x0e01, // burst start

0x10db,
0x11e3,
0x12ea,
0x13f0,
0x14f5,
0x15fb,
0x16ff,

0x1700,	//GMA_IN
0x1803,
0x1908,
0x1a0f,
0x1b15,
0x1c20,
0x1d2d,
0x1e3b,
0x1f47,
0x2053,
0x215c,
0x2265,
0x236d,
0x2475,
0x257c,
0x2682,
0x2788,
0x288d,
0x2991,
0x2a96,
0x2b9a,
0x2ca1,
0x2da9,
0x2eb0,
0x2fbe,
0x30c8,
0x31d2,
0x32db,
0x33e3,
0x34ea,
0x35f0,
0x36f5,
0x37fb,
0x38ff,

0x3900,	// GMA_D1
0x3a06,
0x3b0e,
0x3c17,
0x3d1f,
0x3e2c,
0x3f3c,
0x404c,
0x4157,
0x4261,
0x436b,
0x4476,
0x457e,
0x4685,
0x478c,
0x4892,
0x4998,
0x4a9c,
0x4ba2,
0x4ca6,
0x4dab,
0x4eb4,
0x4fbc,
0x50c2,
0x51cb,
0x52d3,
0x53db,
0x54e1,
0x55e7,
0x56ed,
0x57f3,
0x58f8,
0x59fc,
0x5aff,

0x5b00,	// GMA_D2
0x5c08,
0x5d12,
0x5e1b,
0x5f24,
0x6032,
0x613f,
0x624c,
0x6357,
0x6461,
0x656c,
0x6679,
0x6782,
0x688c,
0x6994,
0x6a99,
0x6b9f,
0x6ca4,
0x6daa,
0x6eaf,
0x6fb4,
0x70ba,
0x71c0,
0x72c5,
0x73cd,
0x74d5,
0x75dc,
0x76e1,
0x77e7,
0x78ed,
0x79f3,
0x7af8,
0x7bfc,
0x7cff,

///////////////////
// Adaptive MCMC //
///////////////////

// Outdoor MCMC
0x7d15, //Outdoor_delta1
0x7e19, //Outdoor_center1
0x7f0f, //Outdoor_delta2
0x8086, //Outdoor_center2
0x8117, //Outdoor_delta3
0x82bd, //Outdoor_center3
0x8317, //Outdoor_delta4
0x84ee, //Outdoor_center4
0x8593, //Outdoor_delta5
0x8625, //Outdoor_center5
0x8793, //Outdoor_delta6
0x8851, //Outdoor_center6
                 
0x8940, // Outdoor_sat_gain1
0x8a40, // Outdoor_sat_gain2
0x8b40, // Outdoor_sat_gain3
0x8c40, // Outdoor_sat_gain4
0x8d40, // Outdoor_sat_gain5
0x8e40, // Outdoor_sat_gain6
0x8f94, // Outdoor_hue_angle1
0x9089, //Outdoor_hue_angle2
0x9110, //Outdoor_hue_angle3
0x9214, //Outdoor_hue_angle4
0x930b, //Outdoor_hue_angle5
0x9487, //Outdoor_hue_angle6

0x9500,	// MCMC24_Outdoor  mcmc_rgb_ofs_sign_r
0x9600,	// MCMC25_Outdoor  mcmc_rgb_ofs_sign_g
0x9700,	// MCMC26_Outdoor  mcmc_rgb_ofs_sign_b

0x9800,	// MCMC27_Outdoor  mcmc_rgb_ofs_r1 R
0x9900,	// MCMC28_Outdoor  mcmc_rgb_ofs_r1 G
0x9a00,	// MCMC29_Outdoor  mcmc_rgb_ofs_r1 B

0x9b00,	// MCMC30_Outdoor  mcmc_rgb_ofs_r2 R
0x9c00,	// MCMC31_Outdoor  mcmc_rgb_ofs_r2 G
0x9d00,	// MCMC32_Outdoor  mcmc_rgb_ofs_r2 B

0x9e00,	// MCMC33_Outdoor  mcmc_rgb_ofs_r3 R
0x9f00,	// MCMC34_Outdoor  mcmc_rgb_ofs_r3 G
0xa000,	// MCMC35_Outdoor  mcmc_rgb_ofs_r3 B

0xa100,	// MCMC36_Outdoor  mcmc_rgb_ofs_r4 R
0xa200,	// MCMC37_Outdoor  mcmc_rgb_ofs_r4 G
0xa300,	// MCMC38_Outdoor  mcmc_rgb_ofs_r4 B

0xa400,	// MCMC39_Outdoor  mcmc_rgb_ofs_r5 R
0xa500,	// MCMC40_Outdoor  mcmc_rgb_ofs_r5 G
0xa600,	// MCMC41_Outdoor  mcmc_rgb_ofs_r5 B

0xa700,	// MCMC42_Outdoor  mcmc_rgb_ofs_r6 R
0xa800,	// MCMC43_Outdoor  mcmc_rgb_ofs_r6 G
0xa900,	// MCMC44_Outdoor  mcmc_rgb_ofs_r6 B

0xaa00,	// MCMC45_Outdoor  mcmc_std_offset1
0xab00,	// MCMC46_Outdoor  mcmc_std_offset2
0xacff,	// MCMC47_Outdoor  mcmc_std_th_max
0xad00,	// MCMC48_Outdoor  mcmc_std_th_min

0xae3f,	// MCMC49_Outdoor  mcmc_lum_gain_wgt_th1 R1 magenta
0xaf3f,	// MCMC50_Outdoor  mcmc_lum_gain_wgt_th2 R1
0xb03f,	// MCMC51_Outdoor  mcmc_lum_gain_wgt_th3 R1
0xb13f,	// MCMC52_Outdoor  mcmc_lum_gain_wgt_th4 R1
0xb230,	// MCMC53_Outdoor  mcmc_rg1_lum_sp1      R1
0xb350,	// MCMC54_Outdoor  mcmc_rg1_lum_sp2      R1
0xb480,	// MCMC55_Outdoor  mcmc_rg1_lum_sp3      R1
0xb5b0,	// MCMC56_Outdoor  mcmc_rg1_lum_sp4      R1

0xb63f,	// MCMC57_Outdoor  mcmc_lum_gain_wgt_th1 R2 red
0xb73f,	// MCMC58_Outdoor  mcmc_lum_gain_wgt_th2 R2
0xb83f,	// MCMC59_Outdoor  mcmc_lum_gain_wgt_th3 R2
0xb93f,	// MCMC60_Outdoor  mcmc_lum_gain_wgt_th4 R2
0xba28,	// MCMC61_Outdoor  mcmc_rg2_lum_sp1      R2
0xbb50,	// MCMC62_Outdoor  mcmc_rg2_lum_sp2      R2
0xbc80,	// MCMC63_Outdoor  mcmc_rg2_lum_sp3      R2
0xbdb0,	// MCMC64_Outdoor  mcmc_rg2_lum_sp4      R2

0xbe3f,	// MCMC65_Outdoor  mcmc_lum_gain_wgt_th1 R3 yellow
0xbf3f,	// MCMC66_Outdoor  mcmc_lum_gain_wgt_th2 R3
0xc030,// MCMC67_Outdoor  mcmc_lum_gain_wgt_th3 R3
0xc12a,// MCMC68_Outdoor  mcmc_lum_gain_wgt_th4 R3
0xc220,// MCMC69_Outdoor  mcmc_rg3_lum_sp1      R3
0xc340,// MCMC70_Outdoor  mcmc_rg3_lum_sp2      R3
0xc470,// MCMC71_Outdoor  mcmc_rg3_lum_sp3      R3
0xc5b0,	// MCMC72_Outdoor  mcmc_rg3_lum_sp4      R3

0xc63f,	// MCMC73_Outdoor  mcmc_lum_gain_wgt_th1 R4 Green
0xc73f,	// MCMC74_Outdoor  mcmc_lum_gain_wgt_th2 R4
0xc83f,	// MCMC75_Outdoor  mcmc_lum_gain_wgt_th3 R4
0xc93f,	// MCMC76_Outdoor  mcmc_lum_gain_wgt_th4 R4
0xca10,	// MCMC77_Outdoor  mcmc_rg4_lum_sp1      R4
0xcb30,	// MCMC78_Outdoor  mcmc_rg4_lum_sp2      R4
0xcc60,	// MCMC79_Outdoor  mcmc_rg4_lum_sp3      R4
0xcd90,	// MCMC80_Outdoor  mcmc_rg4_lum_sp4      R4

0xce3f,	// MCMC81_Outdoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0xcf3f,	// MCMC82_Outdoor  mcmc_rg5_gain_wgt_th2 R5
0xd03f,	// MCMC83_Outdoor  mcmc_rg5_gain_wgt_th3 R5
0xd13f,	// MCMC84_Outdoor  mcmc_rg5_gain_wgt_th4 R5
0xd228,	// MCMC85_Outdoor  mcmc_rg5_lum_sp1      R5
0xd350,	// MCMC86_Outdoor  mcmc_rg5_lum_sp2      R5
0xd480,	// MCMC87_Outdoor  mcmc_rg5_lum_sp3      R5
0xd5b0,	// MCMC88_Outdoor  mcmc_rg5_lum_sp4      R5

0xd63f,	// MCMC89_Outdoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0xd73f,	// MCMC90_Outdoor  mcmc_rg6_gain_wgt_th2 R6
0xd83f,	// MCMC91_Outdoor  mcmc_rg6_gain_wgt_th3 R6
0xd93f,	// MCMC92_Outdoor  mcmc_rg6_gain_wgt_th4 R6
0xda28,	// MCMC93_Outdoor  mcmc_rg6_lum_sp1      R6
0xdb50,	// MCMC94_Outdoor  mcmc_rg6_lum_sp2      R6
0xdc80,	// MCMC95_Outdoor  mcmc_rg6_lum_sp3      R6
0xddb0,	// MCMC96_Outdoor  mcmc_rg6_lum_sp4      R6

0xde1e,	// MCMC97_Outdoor  mcmc2_allgain_x1
0xdf3c,	// MCMC98_Outdoor  mcmc2_allgain_x2
0xe03c,	// MCMC99_Outdoor  mcmc2_allgain_x4
0xe11e,	// MCMC100_Outdoor mcmc2_allgain_x5
0xe21e,	// MCMC101_Outdoor mcmc2_allgain_x7
0xe33c,	// MCMC102_Outdoor mcmc2_allgain_x8
0xe43c,	// MCMC103_Outdoor mcmc2_allgain_x10
0xe51e,	// MCMC104_Outdoor mcmc2_allgain_x11

0xe616, //Outdoor_allgain_y1
0xe716, //Outdoor_allgain_y2
0xe815, //Outdoor_allgain_y3
0xe914, //Outdoor_allgain_y4
0xea14, //Outdoor_allgain_y5
0xeb16, //Outdoor_allgain_y6
0xec18, //Outdoor_allgain_y7
0xed1a, //Outdoor_allgain_y8
0xee1c, //Outdoor_allgain_y9
0xef19, //Outdoor_allgain_y10
0xf014, //Outdoor_allgain_y11
0xf114, //Outdoor_allgain_y12

// Indoor MCMC
0xf210, // Indoor_delta1
0xf31e, // Indoor_center1
0xf40b, //Indoor_delta2
0xf56f, //Indoor_center2
0xf610, // Indoor_delta3
0xf79c, // Indoor_center3
0xf807, // Indoor_delta4
0xf9b8, // Indoor_center4
0xfa90, //Indoor_delta5
0xfb2d, //Indoor_center5
0xfc92, //Indoor_delta6
0xfd4f, //Indoor_center6
0x0e00, // burst end

0x03d6,	// Page D6

0x0e01, // burst start

0x1040, //Indoor_sat_gain1
0x1140, //Indoor_sat_gain2
0x1240, //Indoor_sat_gain3
0x1340, //Indoor_sat_gain4
0x1440, //Indoor_sat_gain5
0x1540, //Indoor_sat_gain6

0x1600, //Indoor_hue_angle1
0x178a, //Indoor_hue_angle2
0x1800, //Indoor_hue_angle3
0x1900, //Indoor_hue_angle4
0x1a00, //Indoor_hue_angle5
0x1b02, //Indoor_hue_angle6

0x1c00,	// MCMC24_Indoor   mcmc_rgb_ofs_sign_r
0x1d00,	// MCMC25_Indoor   mcmc_rgb_ofs_sign_g
0x1e00,	// MCMC26_Indoor   mcmc_rgb_ofs_sign_b

0x1f00,	// MCMC27_Indoor   mcmc_rgb_ofs_r1 R
0x2000,	// MCMC28_Indoor   mcmc_rgb_ofs_r1 G
0x2100,	// MCMC29_Indoor   mcmc_rgb_ofs_r1 B

0x2200,	// MCMC30_Indoor   mcmc_rgb_ofs_r2 R
0x2300,	// MCMC31_Indoor   mcmc_rgb_ofs_r2 G
0x2400,	// MCMC32_Indoor   mcmc_rgb_ofs_r2 B

0x2500,	// MCMC33_Indoor   mcmc_rgb_ofs_r3 R
0x2600,	// MCMC34_Indoor   mcmc_rgb_ofs_r3 G
0x2700,	// MCMC35_Indoor   mcmc_rgb_ofs_r3 B

0x2800,	// MCMC36_Indoor   mcmc_rgb_ofs_r4 R
0x2900,	// MCMC37_Indoor   mcmc_rgb_ofs_r4 G
0x2a00,	// MCMC38_Indoor   mcmc_rgb_ofs_r4 B

0x2b00,	// MCMC39_Indoor   mcmc_rgb_ofs_r5 R
0x2c00,	// MCMC40_Indoor   mcmc_rgb_ofs_r5 G
0x2d00,	// MCMC41_Indoor   mcmc_rgb_ofs_r5 B

0x2e00,	// MCMC42_Indoor  mcmc_rgb_ofs_r6 R
0x2f00,	// MCMC43_Indoor  mcmc_rgb_ofs_r6 G
0x3000,	// MCMC44_Indoor  mcmc_rgb_ofs_r6 B

0x3100,	// MCMC45_Indoor  mcmc_std_offset1
0x3200,	// MCMC46_Indoor  mcmc_std_offset2
0x33ff,	// MCMC47_Indoor  mcmc_std_th_max
0x3400,	// MCMC48_Indoor  mcmc_std_th_min

0x3510,	// MCMC49_Indoor  mcmc_lum_gain_wgt_th1 R1 magenta
0x3621,	// MCMC50_Indoor  mcmc_lum_gain_wgt_th2 R1
0x3734,	// MCMC51_Indoor  mcmc_lum_gain_wgt_th3 R1
0x383f,	// MCMC52_Indoor  mcmc_lum_gain_wgt_th4 R1
0x3908,	// MCMC53_Indoor  mcmc_rg1_lum_sp1      R1
0x3a15,	// MCMC54_Indoor  mcmc_rg1_lum_sp2      R1
0x3b2f,	// MCMC55_Indoor  mcmc_rg1_lum_sp3      R1
0x3c51,	// MCMC56_Indoor  mcmc_rg1_lum_sp4      R1

0x3d3f,	// MCMC57_Indoor  mcmc_lum_gain_wgt_th1 R2 red
0x3e3f,	// MCMC58_Indoor  mcmc_lum_gain_wgt_th2 R2
0x3f3f,	// MCMC59_Indoor  mcmc_lum_gain_wgt_th3 R2
0x403f,	// MCMC60_Indoor  mcmc_lum_gain_wgt_th4 R2
0x4128,	// MCMC61_Indoor  mcmc_rg2_lum_sp1      R2
0x4250,	// MCMC62_Indoor  mcmc_rg2_lum_sp2      R2
0x4380,	// MCMC63_Indoor  mcmc_rg2_lum_sp3      R2
0x44b0,	// MCMC64_Indoor  mcmc_rg2_lum_sp4      R2

0x453f,	// MCMC65_Indoor  mcmc_lum_gain_wgt_th1 R3 yellow
0x463f,	// MCMC66_Indoor  mcmc_lum_gain_wgt_th2 R3
0x473f,	// MCMC67_Indoor  mcmc_lum_gain_wgt_th3 R3
0x483f,	// MCMC68_Indoor  mcmc_lum_gain_wgt_th4 R3
0x4928,	// MCMC69_Indoor  mcmc_rg3_lum_sp1      R3
0x4a50,	// MCMC70_Indoor  mcmc_rg3_lum_sp2      R3
0x4b80,	// MCMC71_Indoor  mcmc_rg3_lum_sp3      R3
0x4cb0,	// MCMC72_Indoor  mcmc_rg3_lum_sp4      R3

0x4d3f,	// MCMC73_Indoor  mcmc_lum_gain_wgt_th1 R4 Green
0x4e3f,	// MCMC74_Indoor  mcmc_lum_gain_wgt_th2 R4
0x4f3f,	// MCMC75_Indoor  mcmc_lum_gain_wgt_th3 R4
0x503f,	// MCMC76_Indoor  mcmc_lum_gain_wgt_th4 R4
0x5110,	// MCMC77_Indoor  mcmc_rg4_lum_sp1      R4
0x5230,	// MCMC78_Indoor  mcmc_rg4_lum_sp2      R4
0x5360,	// MCMC79_Indoor  mcmc_rg4_lum_sp3      R4
0x5490,	// MCMC80_Indoor  mcmc_rg4_lum_sp4      R4

0x553f,	// MCMC81_Indoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0x563f,	// MCMC82_Indoor  mcmc_rg5_gain_wgt_th2 R5
0x573f,	// MCMC83_Indoor  mcmc_rg5_gain_wgt_th3 R5
0x583f,	// MCMC84_Indoor  mcmc_rg5_gain_wgt_th4 R5
0x5928,	// MCMC85_Indoor  mcmc_rg5_lum_sp1      R5
0x5a50,	// MCMC86_Indoor  mcmc_rg5_lum_sp2      R5
0x5b80,	// MCMC87_Indoor  mcmc_rg5_lum_sp3      R5
0x5cb0,	// MCMC88_Indoor  mcmc_rg5_lum_sp4      R5

0x5d3f,	// MCMC89_Indoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0x5e3f,	// MCMC90_Indoor  mcmc_rg6_gain_wgt_th2 R6
0x5f3f,	// MCMC91_Indoor  mcmc_rg6_gain_wgt_th3 R6
0x603f,	// MCMC92_Indoor  mcmc_rg6_gain_wgt_th4 R6
0x6128,	// MCMC93_Indoor  mcmc_rg6_lum_sp1      R6
0x6250,	// MCMC94_Indoor  mcmc_rg6_lum_sp2      R6
0x6380,	// MCMC95_Indoor  mcmc_rg6_lum_sp3      R6
0x64b0,	// MCMC96_Indoor  mcmc_rg6_lum_sp4      R6

0x651d,	// MCMC97_Indoor  mcmc2_allgain_x1
0x663b,	// MCMC98_Indoor  mcmc2_allgain_x2
0x673b,	// MCMC99_Indoor  mcmc2_allgain_x4
0x681d,	// MCMC100_Indoor mcmc2_allgain_x5
0x691d,	// MCMC101_Indoor mcmc2_allgain_x7
0x6a3b,	// MCMC102_Indoor mcmc2_allgain_x8
0x6b3b,	// MCMC103_Indoor mcmc2_allgain_x10
0x6c1d,	// MCMC104_Indoor mcmc2_allgain_x11

0x6d0e,	// MCMC105_Indoor mcmc2_allgain_y0
0x6e0f,	// MCMC106_Indoor mcmc2_allgain_y1
0x6f0f,	// MCMC107_Indoor mcmc2_allgain_y2
0x700f,	// MCMC108_Indoor mcmc2_allgain_y3
0x710f,	// MCMC109_Indoor mcmc2_allgain_y4
0x7210,	// MCMC110_Indoor mcmc2_allgain_y5
0x7310,	// MCMC111_Indoor mcmc2_allgain_y6
0x7410,	// MCMC112_Indoor mcmc2_allgain_y7
0x7510,	// MCMC113_Indoor mcmc2_allgain_y8
0x760f,	// MCMC114_Indoor mcmc2_allgain_y9
0x770e,	// MCMC115_Indoor mcmc2_allgain_y10
0x780d,	// MCMC116_Indoor mcmc2_allgain_y11

// Dark1 MCMC
0x7917, //Dark1_delta1
0x7a56, //Dark1_center1
0x7b10, //Dark1_delta2
0x7c70, //Dark1_center2
0x7d10, //Dark1_delta3
0x7e9c, //Dark1_center3
0x7f18, //Dark1_delta4
0x80db, //Dark1_center4
0x8198, //Dark1_delta5
0x8226, //Dark1_center5
0x8399, //Dark1_delta6
0x845b, //Dark1_center6

0x8540, //Dark1_sat_gain1
0x8640, //Dark1_sat_gain2
0x8740, //Dark1_sat_gain3
0x8840, //Dark1_sat_gain4
0x8940, //Dark1_sat_gain5
0x8a40, //Dark1_sat_gain6
0x8b91, //Dark1_hue_angle1
0x8c00, //Dark1_hue_angle2
0x8d00, //Dark1_hue_angle3
0x8e0a, //Dark1_hue_angle4
0x8f05, //Dark1_hue_angle5
0x9086, //Dark1_hue_angle6

0x913f,	// MCMC24_Dark1   mcmc_rgb_ofs_sign
0x923f,	// MCMC25_Dark1   mcmc_rgb_ofs_sign
0x933f,	// MCMC26_Dark1   mcmc_rgb_ofs_sign

0x9400,	// MCMC27_Dark1   mcmc_rgb_ofs_r1 R
0x9500,	// MCMC28_Dark1   mcmc_rgb_ofs_r1 G
0x9600,	// MCMC29_Dark1   mcmc_rgb_ofs_r1 B

0x9700,	// MCMC30_Dark1   mcmc_rgb_ofs_r2 R
0x9800,	// MCMC31_Dark1   mcmc_rgb_ofs_r2 G
0x9900,	// MCMC32_Dark1   mcmc_rgb_ofs_r2 B

0x9a00,	// MCMC33_Dark1   mcmc_rgb_ofs_r3 R
0x9b00,	// MCMC34_Dark1   mcmc_rgb_ofs_r3 G
0x9c00,	// MCMC35_Dark1   mcmc_rgb_ofs_r3 B

0x9d00,	// MCMC36_Dark1   mcmc_rgb_ofs_r4 R
0x9e00,	// MCMC37_Dark1   mcmc_rgb_ofs_r4 G
0x9f00,	// MCMC38_Dark1   mcmc_rgb_ofs_r4 B

0xa000,	// MCMC39_Dark1   mcmc_rgb_ofs_r5 R
0xa100,	// MCMC40_Dark1   mcmc_rgb_ofs_r5 G
0xa200,	// MCMC41_Dark1   mcmc_rgb_ofs_r5 B

0xa300,	// MCMC42_Dark1  mcmc_rgb_ofs_r6 R
0xa400,	// MCMC43_Dark1  mcmc_rgb_ofs_r6 G
0xa500,	// MCMC44_Dark1  mcmc_rgb_ofs_r6 B

0xa600,	// MCMC45_Dark1  mcmc_std_offset1
0xa700,	// MCMC46_Dark1  mcmc_std_offset2
0xa8ff,	// MCMC47_Dark1  mcmc_std_th_max
0xa900,	// MCMC48_Dark1  mcmc_std_th_min

0xaa3f,	// MCMC49_Dark1  mcmc_lum_gain_wgt R1
0xab3f,	// MCMC50_Dark1  mcmc_lum_gain_wgt R1
0xac3f,	// MCMC51_Dark1  mcmc_lum_gain_wgt R1
0xad3f,	// MCMC52_Dark1  mcmc_lum_gain_wgt R1
0xae30,	// MCMC53_Dark1  mcmc_rg1_lum_sp1  R1
0xaf50,	// MCMC54_Dark1  mcmc_rg1_lum_sp2  R1
0xb080,	// MCMC55_Dark1  mcmc_rg1_lum_sp3  R1
0xb1b0,	// MCMC56_Dark1  mcmc_rg1_lum_sp4  R1

0xb23f,	// MCMC57_Dark1  mcmc_lum_gain_wgt R2
0xb33f,	// MCMC58_Dark1  mcmc_lum_gain_wgt R2
0xb43f,	// MCMC59_Dark1  mcmc_lum_gain_wgt R2
0xb53f,	// MCMC60_Dark1  mcmc_lum_gain_wgt R2
0xb628,	// MCMC61_Dark1  mcmc_rg2_lum_sp1  R2
0xb750,	// MCMC62_Dark1  mcmc_rg2_lum_sp2  R2
0xb880,	// MCMC63_Dark1  mcmc_rg2_lum_sp3  R2
0xb9b0,	// MCMC64_Dark1  mcmc_rg2_lum_sp4  R2

0xba3f,	// MCMC65_Dark1  mcmc_lum_gain_wgt R3
0xbb3f,	// MCMC66_Dark1  mcmc_lum_gain_wgt R3
0xbc3f,	// MCMC67_Dark1  mcmc_lum_gain_wgt R3
0xbd3f,	// MCMC68_Dark1  mcmc_lum_gain_wgt R3
0xbe28,	// MCMC69_Dark1  mcmc_rg3_lum_sp1  R3
0xbf50,	// MCMC70_Dark1  mcmc_rg3_lum_sp2  R3
0xc080,	// MCMC71_Dark1  mcmc_rg3_lum_sp3  R3
0xc1b0,	// MCMC72_Dark1  mcmc_rg3_lum_sp4  R3

0xc23f,	// MCMC73_Dark1  mcmc_lum_gain_wgt R4
0xc33f,	// MCMC74_Dark1  mcmc_lum_gain_wgt R4
0xc43f,	// MCMC75_Dark1  mcmc_lum_gain_wgt R4
0xc53f,	// MCMC76_Dark1  mcmc_lum_gain_wgt R4
0xc610,	// MCMC77_Dark1  mcmc_rg4_lum_sp1  R4
0xc730,	// MCMC78_Dark1  mcmc_rg4_lum_sp2  R4
0xc860,	// MCMC79_Dark1  mcmc_rg4_lum_sp3  R4
0xc990,	// MCMC80_Dark1  mcmc_rg4_lum_sp4  R4

0xca3f,	// MCMC81_Dark1  mcmc_rg5_gain_wgt R5
0xcb3f,	// MCMC82_Dark1  mcmc_rg5_gain_wgt R5
0xcc3f,	// MCMC83_Dark1  mcmc_rg5_gain_wgt R5
0xcd3f,	// MCMC84_Dark1  mcmc_rg5_gain_wgt R5
0xce28,	// MCMC85_Dark1  mcmc_rg5_lum_sp1  R5
0xcf50,	// MCMC86_Dark1  mcmc_rg5_lum_sp2  R5
0xd080,	// MCMC87_Dark1  mcmc_rg5_lum_sp3  R5
0xd1b0,	// MCMC88_Dark1  mcmc_rg5_lum_sp4  R5

0xd23f,	// MCMC89_Dark1  mcmc_rg6_gain_wgt R6
0xd33f,	// MCMC90_Dark1  mcmc_rg6_gain_wgt R6
0xd43f,	// MCMC91_Dark1  mcmc_rg6_gain_wgt R6
0xd53f,	// MCMC92_Dark1  mcmc_rg6_gain_wgt R6
0xd628,	// MCMC93_Dark1  mcmc_rg6_lum_sp1  R6
0xd750,	// MCMC94_Dark1  mcmc_rg6_lum_sp2  R6
0xd880,	// MCMC95_Dark1  mcmc_rg6_lum_sp3  R6
0xd9b0,	// MCMC96_Dark1  mcmc_rg6_lum_sp4  R6

0xda1c,	// MCMC97_Dark1  mcmc2_allgain_x1
0xdb3a,	// MCMC98_Dark1  mcmc2_allgain_x2
0xdc3a,	// MCMC99_Dark1  mcmc2_allgain_x4
0xdd1c,	// MCMC100_Dark1 mcmc2_allgain_x5
0xde1c,	// MCMC101_Dark1 mcmc2_allgain_x7
0xdf3a,	// MCMC102_Dark1 mcmc2_allgain_x8
0xe03a,	// MCMC103_Dark1 mcmc2_allgain_x10
0xe11c,	// MCMC104_Dark1 mcmc2_allgain_x11

0xe20f, //Dark1_allgain_y1
0xe310, //Dark1_allgain_y2
0xe410, //Dark1_allgain_y3
0xe511, //Dark1_allgain_y4
0xe610, //Dark1_allgain_y5
0xe713, //Dark1_allgain_y6
0xe812, //Dark1_allgain_y7
0xe912, //Dark1_allgain_y8
0xea12, //Dark1_allgain_y9
0xeb11, //Dark1_allgain_y10
0xec10, //Dark1_allgain_y11
0xed0f, //Dark1_allgain_y12

// Dark2 MCMC
0xee17,	// MCMC00_Dark2   mcmc_delta1
0xef56,	// MCMC01_Dark2   mcmc_center1
0xf010,	// MCMC02_Dark2   mcmc_delta2
0xf170,	// MCMC03_Dark2   mcmc_center2
0xf210,	// MCMC04_Dark2   mcmc_delta3
0xf39c,	// MCMC05_Dark2   mcmc_center3
0xf418,	// MCMC06_Dark2   mcmc_delta4
0xf5db,	// MCMC07_Dark2   mcmc_center4
0xf698,	// MCMC08_Dark2   mcmc_delta5
0xf726,	// MCMC09_Dark2   mcmc_center5
0xf899,	// MCMC10_Dark2   mcmc_delta6
0xf95b,	// MCMC11_Dark2   mcmc_center6

0xfa40,	// MCMC12_Dark2   mcmc_sat_gain1
0xfb40,	// MCMC13_Dark2   mcmc_sat_gain2
0xfc40,	// MCMC14_Dark2   mcmc_sat_gain3
0xfd40,	// MCMC15_Dark2   mcmc_sat_gain4
0x0e00, // burst end

0x03d7,// Page D7

0x0e01, // burst start

0x1040,	// MCMC16_Dark2   mcmc_sat_gain5
0x1140,	// MCMC17_Dark2   mcmc_sat_gain6
0x1291,	// MCMC18_Dark2   mcmc_hue_angle1
0x1300,	// MCMC19_Dark2   mcmc_hue_angle2
0x1400,	// MCMC20_Dark2   mcmc_hue_angle3
0x150a,	// MCMC21_Dark2   mcmc_hue_angle4
0x160f,	// MCMC22_Dark2   mcmc_hue_angle5
0x1705,	// MCMC23_Dark2   mcmc_hue_angle6

0x182f,	// MCMC24_Dark2   mcmc_rgb_ofs_sig
0x192f,	// MCMC25_Dark2   mcmc_rgb_ofs_sig
0x1a2f,	// MCMC26_Dark2   mcmc_rgb_ofs_sig

0x1b00,	// MCMC27_Dark2   mcmc_rgb_ofs_r1
0x1c00,	// MCMC28_Dark2   mcmc_rgb_ofs_r1
0x1d00,	// MCMC29_Dark2   mcmc_rgb_ofs_r1

0x1e00,	// MCMC30_Dark2   mcmc_rgb_ofs_r2
0x1f00,	// MCMC31_Dark2   mcmc_rgb_ofs_r2
0x2000,	// MCMC32_Dark2   mcmc_rgb_ofs_r2

0x2100,	// MCMC33_Dark2   mcmc_rgb_ofs_r3
0x2200,	// MCMC34_Dark2   mcmc_rgb_ofs_r3
0x2300,	// MCMC35_Dark2   mcmc_rgb_ofs_r3

0x2400,	// MCMC36_Dark2   mcmc_rgb_ofs_r4
0x2500,	// MCMC37_Dark2   mcmc_rgb_ofs_r4
0x2600,	// MCMC38_Dark2   mcmc_rgb_ofs_r4

0x2700,	// MCMC39_Dark2   mcmc_rgb_ofs_r5
0x2800,	// MCMC40_Dark2   mcmc_rgb_ofs_r5
0x2900,	// MCMC41_Dark2   mcmc_rgb_ofs_r5

0x2a00,	// MCMC42_Dark2  mcmc_rgb_ofs_r6 R
0x2b00,	// MCMC43_Dark2  mcmc_rgb_ofs_r6 G
0x2c00,	// MCMC44_Dark2  mcmc_rgb_ofs_r6 B

0x2d00,	// MCMC45_Dark2  mcmc_std_offset1
0x2e00,	// MCMC46_Dark2  mcmc_std_offset2
0x2fff,	// MCMC47_Dark2  mcmc_std_th_max
0x3000,	// MCMC48_Dark2  mcmc_std_th_min

0x313f,	// MCMC49_Dark2  mcmc_lum_gain_wgt R1
0x323f,	// MCMC50_Dark2  mcmc_lum_gain_wgt R1
0x333f,	// MCMC51_Dark2  mcmc_lum_gain_wgt R1
0x343f,	// MCMC52_Dark2  mcmc_lum_gain_wgt R1
0x3530,	// MCMC53_Dark2  mcmc_rg1_lum_sp1  R1
0x3650,	// MCMC54_Dark2  mcmc_rg1_lum_sp2  R1
0x3780,	// MCMC55_Dark2  mcmc_rg1_lum_sp3  R1
0x38b0,	// MCMC56_Dark2  mcmc_rg1_lum_sp4  R1

0x393f,	// MCMC57_Dark2  mcmc_lum_gain_wgt R2
0x3a3f,	// MCMC58_Dark2  mcmc_lum_gain_wgt R2
0x3b3f,	// MCMC59_Dark2  mcmc_lum_gain_wgt R2
0x3c3f,	// MCMC60_Dark2  mcmc_lum_gain_wgt R2
0x3d28,	// MCMC61_Dark2  mcmc_rg2_lum_sp1  R2
0x3e50,	// MCMC62_Dark2  mcmc_rg2_lum_sp2  R2
0x3f80,	// MCMC63_Dark2  mcmc_rg2_lum_sp3  R2
0x40b0,	// MCMC64_Dark2  mcmc_rg2_lum_sp4  R2

0x413f,	// MCMC65_Dark2  mcmc_lum_gain_wgt R3
0x423f,	// MCMC66_Dark2  mcmc_lum_gain_wgt R3
0x433f,	// MCMC67_Dark2  mcmc_lum_gain_wgt R3
0x443f,	// MCMC68_Dark2  mcmc_lum_gain_wgt R3
0x4528,	// MCMC69_Dark2  mcmc_rg3_lum_sp1  R3
0x4650,	// MCMC70_Dark2  mcmc_rg3_lum_sp2  R3
0x4780,	// MCMC71_Dark2  mcmc_rg3_lum_sp3  R3
0x48b0,	// MCMC72_Dark2  mcmc_rg3_lum_sp4  R3

0x491a,	// MCMC73_Dark2  mcmc_lum_gain_wgt R4
0x4a28,	// MCMC74_Dark2  mcmc_lum_gain_wgt R4
0x4b3f,	// MCMC75_Dark2  mcmc_lum_gain_wgt R4
0x4c3f,	// MCMC76_Dark2  mcmc_lum_gain_wgt R4
0x4d10,	// MCMC77_Dark2  mcmc_rg4_lum_sp1  R4
0x4e30,	// MCMC78_Dark2  mcmc_rg4_lum_sp2  R4
0x4f60,	// MCMC79_Dark2  mcmc_rg4_lum_sp3  R4
0x5090,	// MCMC80_Dark2  mcmc_rg4_lum_sp4  R4

0x511a,	// MCMC81_Dark2  mcmc_rg5_gain_wgt R5
0x5228,	// MCMC82_Dark2  mcmc_rg5_gain_wgt R5
0x533f,	// MCMC83_Dark2  mcmc_rg5_gain_wgt R5
0x543f,	// MCMC84_Dark2  mcmc_rg5_gain_wgt R5
0x5528,	// MCMC85_Dark2  mcmc_rg5_lum_sp1  R5
0x5650,	// MCMC86_Dark2  mcmc_rg5_lum_sp2  R5
0x5780,	// MCMC87_Dark2  mcmc_rg5_lum_sp3  R5
0x58b0,	// MCMC88_Dark2  mcmc_rg5_lum_sp4  R5

0x591a,	// MCMC89_Dark2  mcmc_rg6_gain_wgt R6
0x5a28,	// MCMC90_Dark2  mcmc_rg6_gain_wgt R6
0x5b3f,	// MCMC91_Dark2  mcmc_rg6_gain_wgt R6
0x5c3f,	// MCMC92_Dark2  mcmc_rg6_gain_wgt R6
0x5d28,	// MCMC93_Dark2  mcmc_rg6_lum_sp1  R6
0x5e50,	// MCMC94_Dark2  mcmc_rg6_lum_sp2  R6
0x5f80,	// MCMC95_Dark2  mcmc_rg6_lum_sp3  R6
0x60b0,	// MCMC96_Dark2  mcmc_rg6_lum_sp4  R6

0x611b,	// MCMC97_Dark2  mcmc2_allgain_x1
0x6239,	// MCMC98_Dark2  mcmc2_allgain_x2
0x6339,	// MCMC99_Dark2  mcmc2_allgain_x4
0x641b,	// MCMC100_Dark2 mcmc2_allgain_x5
0x651b,	// MCMC101_Dark2 mcmc2_allgain_x7
0x6639,	// MCMC102_Dark2 mcmc2_allgain_x8
0x6739,	// MCMC103_Dark2 mcmc2_allgain_x10
0x681b,	// MCMC104_Dark2 mcmc2_allgain_x11

0x690f,	// MCMC105_Dark2 mcmc2_allgain_y0
0x6a10,	// MCMC106_Dark2 mcmc2_allgain_y1
0x6b10,	// MCMC107_Dark2 mcmc2_allgain_y2
0x6c11,	// MCMC108_Dark2 mcmc2_allgain_y3
0x6d10,	// MCMC109_Dark2 mcmc2_allgain_y4
0x6e13,	// MCMC110_Dark2 mcmc2_allgain_y5
0x6f12,	// MCMC111_Dark2 mcmc2_allgain_y6
0x7012,	// MCMC112_Dark2 mcmc2_allgain_y7
0x7112,	// MCMC113_Dark2 mcmc2_allgain_y8
0x7211,	// MCMC114_Dark2 mcmc2_allgain_y9
0x7310,	// MCMC115_Dark2 mcmc2_allgain_y10
0x740f,	// MCMC116_Dark2 mcmc2_allgain_y11

// LowTemp MCMC
0x7510,	// MCMC00_LowTemp   mcmc_delta1
0x7639,	// MCMC01_LowTemp   mcmc_center1
0x7710,	// MCMC02_LowTemp   mcmc_delta2
0x7859,	// MCMC03_LowTemp   mcmc_center2
0x7912,	// MCMC04_LowTemp   mcmc_delta3
0x7a9d,	// MCMC05_LowTemp   mcmc_center3
0x7b12,	// MCMC06_LowTemp   mcmc_delta4
0x7cc1,	// MCMC07_LowTemp   mcmc_center4
0x7d18,	// MCMC08_LowTemp   mcmc_delta5
0x7eeb,	// MCMC09_LowTemp   mcmc_center5
0x7f99,	// MCMC10_LowTemp   mcmc_delta6
0x801c,	// MCMC11_LowTemp   mcmc_center6

0x8140,	// MCMC12_LowTemp   mcmc_sat_gain1
0x8240,	// MCMC13_LowTemp   mcmc_sat_gain2
0x8340,	// MCMC14_LowTemp   mcmc_sat_gain3
0x8440,	// MCMC15_LowTemp   mcmc_sat_gain4
0x8540,	// MCMC16_LowTemp   mcmc_sat_gain5
0x8640,	// MCMC17_LowTemp   mcmc_sat_gain6
0x8700,	// MCMC18_LowTemp   mcmc_hue_angle1
0x8800,	// MCMC19_LowTemp   mcmc_hue_angle2
0x8900,	// MCMC20_LowTemp   mcmc_hue_angle3
0x8a00,	// MCMC21_LowTemp   mcmc_hue_angle4
0x8b00,	// MCMC22_LowTemp   mcmc_hue_angle5
0x8c00,	// MCMC23_LowTemp   mcmc_hue_angle6

0x8d1f,	// MCMC24_LowTemp   mcmc_rgb_ofs_sig
0x8e1f,	// MCMC25_LowTemp   mcmc_rgb_ofs_sig
0x8f1f,	// MCMC26_LowTemp   mcmc_rgb_ofs_sig

0x9000,	// MCMC27_LowTemp   mcmc_rgb_ofs_r1
0x9100,	// MCMC28_LowTemp   mcmc_rgb_ofs_r1
0x9200,	// MCMC29_LowTemp   mcmc_rgb_ofs_r1

0x9300,	// MCMC30_LowTemp   mcmc_rgb_ofs_r2
0x9400,	// MCMC31_LowTemp   mcmc_rgb_ofs_r2
0x9500,	// MCMC32_LowTemp   mcmc_rgb_ofs_r2

0x9600,	// MCMC33_LowTemp   mcmc_rgb_ofs_r3
0x9700,	// MCMC34_LowTemp   mcmc_rgb_ofs_r3
0x9800,	// MCMC35_LowTemp   mcmc_rgb_ofs_r3

0x9900,	// MCMC36_LowTemp   mcmc_rgb_ofs_r4
0x9a00,	// MCMC37_LowTemp   mcmc_rgb_ofs_r4
0x9b00,	// MCMC38_LowTemp   mcmc_rgb_ofs_r4

0x9c00,	// MCMC39_LowTemp   mcmc_rgb_ofs_r5
0x9d00,	// MCMC40_LowTemp   mcmc_rgb_ofs_r5
0x9e00,	// MCMC41_LowTemp   mcmc_rgb_ofs_r5

0x9f00,	// MCMC42_LowTemp  mcmc_rgb_ofs_r6 R
0xa000,	// MCMC43_LowTemp  mcmc_rgb_ofs_r6 G
0xa100,	// MCMC44_LowTemp  mcmc_rgb_ofs_r6 B

0xa200,	// MCMC45_LowTemp  mcmc_std_offset1
0xa300,	// MCMC46_LowTemp  mcmc_std_offset2
0xa4ff,	// MCMC47_LowTemp  mcmc_std_th_max
0xa500,	// MCMC48_LowTemp  mcmc_std_th_min

0xa63f,	// MCMC49_LowTemp  mcmc_lum_gain_wgt R1
0xa73f,	// MCMC50_LowTemp  mcmc_lum_gain_wgt R1
0xa83f,	// MCMC51_LowTemp  mcmc_lum_gain_wgt R1
0xa93f,	// MCMC52_LowTemp  mcmc_lum_gain_wgt R1
0xaa30,	// MCMC53_LowTemp  mcmc_rg1_lum_sp1  R1
0xab50,	// MCMC54_LowTemp  mcmc_rg1_lum_sp2  R1
0xac80,	// MCMC55_LowTemp  mcmc_rg1_lum_sp3  R1
0xadb0,	// MCMC56_LowTemp  mcmc_rg1_lum_sp4  R1

0xae3f,	// MCMC57_LowTemp  mcmc_lum_gain_wgt R2
0xaf3f,	// MCMC58_LowTemp  mcmc_lum_gain_wgt R2
0xb03f,	// MCMC59_LowTemp  mcmc_lum_gain_wgt R2
0xb13f,	// MCMC60_LowTemp  mcmc_lum_gain_wgt R2
0xb228,	// MCMC61_LowTemp  mcmc_rg2_lum_sp1  R2
0xb350,	// MCMC62_LowTemp  mcmc_rg2_lum_sp2  R2
0xb480,	// MCMC63_LowTemp  mcmc_rg2_lum_sp3  R2
0xb5b0,	// MCMC64_LowTemp  mcmc_rg2_lum_sp4  R2

0xb63f,	// MCMC65_LowTemp  mcmc_lum_gain_wgt R3
0xb73f,	// MCMC66_LowTemp  mcmc_lum_gain_wgt R3
0xb83f,	// MCMC67_LowTemp  mcmc_lum_gain_wgt R3
0xb93f,	// MCMC68_LowTemp  mcmc_lum_gain_wgt R3
0xba28,	// MCMC69_LowTemp  mcmc_rg3_lum_sp1  R3
0xbb50,	// MCMC70_LowTemp  mcmc_rg3_lum_sp2  R3
0xbc80,	// MCMC71_LowTemp  mcmc_rg3_lum_sp3  R3
0xbdb0,	// MCMC72_LowTemp  mcmc_rg3_lum_sp4  R3

0xbe3f,	// MCMC73_LowTemp  mcmc_lum_gain_wgt R4
0xbf3f,	// MCMC74_LowTemp  mcmc_lum_gain_wgt R4
0xc03f,	// MCMC75_LowTemp  mcmc_lum_gain_wgt R4
0xc13f,	// MCMC76_LowTemp  mcmc_lum_gain_wgt R4
0xc210,	// MCMC77_LowTemp  mcmc_rg4_lum_sp1  R4
0xc330,	// MCMC78_LowTemp  mcmc_rg4_lum_sp2  R4
0xc460,	// MCMC79_LowTemp  mcmc_rg4_lum_sp3  R4
0xc590,	// MCMC80_LowTemp  mcmc_rg4_lum_sp4  R4

0xc63f,	// MCMC81_LowTemp  mcmc_rg5_gain_wgt R5
0xc73f,	// MCMC82_LowTemp  mcmc_rg5_gain_wgt R5
0xc83f,	// MCMC83_LowTemp  mcmc_rg5_gain_wgt R5
0xc93f,	// MCMC84_LowTemp  mcmc_rg5_gain_wgt R5
0xca28,	// MCMC85_LowTemp  mcmc_rg5_lum_sp1  R5
0xcb50,	// MCMC86_LowTemp  mcmc_rg5_lum_sp2  R5
0xcc80,	// MCMC87_LowTemp  mcmc_rg5_lum_sp3  R5
0xcdb0,	// MCMC88_LowTemp  mcmc_rg5_lum_sp4  R5

0xce3f,	// MCMC89_LowTemp  mcmc_rg6_gain_wgt R6
0xcf3f,	// MCMC90_LowTemp  mcmc_rg6_gain_wgt R6
0xd03f,	// MCMC91_LowTemp  mcmc_rg6_gain_wgt R6
0xd13f,	// MCMC92_LowTemp  mcmc_rg6_gain_wgt R6
0xd228,	// MCMC93_LowTemp  mcmc_rg6_lum_sp1  R6
0xd350,	// MCMC94_LowTemp  mcmc_rg6_lum_sp2  R6
0xd480,	// MCMC95_LowTemp  mcmc_rg6_lum_sp3  R6
0xd5b0,	// MCMC96_LowTemp  mcmc_rg6_lum_sp4  R6

0xd61a,	// MCMC97_LowTemp  mcmc2_allgain_x1
0xd738,	// MCMC98_LowTemp  mcmc2_allgain_x2
0xd838,	// MCMC99_LowTemp  mcmc2_allgain_x4
0xd91a,	// MCMC100_LowTemp mcmc2_allgain_x5
0xda1a,	// MCMC101_LowTemp mcmc2_allgain_x7
0xdb38,	// MCMC102_LowTemp mcmc2_allgain_x8
0xdc38,	// MCMC103_LowTemp mcmc2_allgain_x10
0xdd1a,	// MCMC104_LowTemp mcmc2_allgain_x11

0xde10,	// MCMC105_LowTemp mcmc2_allgain_y0
0xdf0f,	// MCMC106_LowTemp mcmc2_allgain_y1
0xe00e,	// MCMC107_LowTemp mcmc2_allgain_y2
0xe10e,	// MCMC108_LowTemp mcmc2_allgain_y3
0xe212,	// MCMC109_LowTemp mcmc2_allgain_y4
0xe316,	// MCMC110_LowTemp mcmc2_allgain_y5
0xe416,	// MCMC111_LowTemp mcmc2_allgain_y6
0xe514,	// MCMC112_LowTemp mcmc2_allgain_y
0xe612,	// MCMC113_LowTemp mcmc2_allgain_y8
0xe710,	// MCMC114_LowTemp mcmc2_allgain_y9
0xe810,	// MCMC115_LowTemp mcmc2_allgain_y10
0xe910,	// MCMC116_LowTemp mcmc2_allgain_y11
0x0e00, // burst end

// HighTemp MCMC
0x03d7, //Page d7
0xea10, //Hi-Temp_delta1
0xeb39, //Hi-Temp_center1
0xec10, //Hi-Temp_delta2
0xed6a, //Hi-Temp_center2
0xee12, //Hi-Temp_delta3
0xef9d, //Hi-Temp_center3
0xf012, //Hi-Temp_delta4
0xf1bd, //Hi-Temp_center4
0xf21e, //Hi-Temp_delta5
0xf3f1, //Hi-Temp_center5
0xf49e, //Hi-Temp_delta6
0xf534, //Hi-Temp_center6
0xf640, //Hi-Temp_sat_gain1
0xf740, //Hi-Temp_sat_gain2
0xf840, //Hi-Temp_sat_gain3
0xf940, //Hi-Temp_sat_gain4
0xfa40, //Hi-Temp_sat_gain5
0xfb40, //Hi-Temp_sat_gain6
0xfc00, //Hi-Temp_hue_angle1
0xfd82, //Hi-Temp_hue_angle2

0x03d8, //Page d8
0x0e01, // burst start

0x1000, //Hi-Temp_hue_angle3
0x1100, //Hi-Temp_hue_angle4
0x1206, //Hi-Temp_hue_angle5
0x1300, //Hi-Temp_hue_angle6
0x1411, //Hi-Temp_rgb_ofs_sign_r
0x1511, //Hi-Temp_rgb_ofs_sign_g
0x1611, //Hi-Temp_rgb_ofs_sign_b
0x1700, //Hi-Temp_rgb_ofs_scl_r1
0x1800, //Hi-Temp_rgb_ofs_scl_g1
0x1900, //Hi-Temp_rgb_ofs_scl_b1
0x1a00, //Hi-Temp_rgb_ofs_scl_r2
0x1b00, //Hi-Temp_rgb_ofs_scl_g2
0x1c00, //Hi-Temp_rgb_ofs_scl_b2
0x1d00, //Hi-Temp_rgb_ofs_scl_r3
0x1e00, //Hi-Temp_rgb_ofs_scl_g3
0x1f00, //Hi-Temp_rgb_ofs_scl_b3
0x2000, //Hi-Temp_rgb_ofs_scl_r4
0x2100, //Hi-Temp_rgb_ofs_scl_g4
0x2200, //Hi-Temp_rgb_ofs_scl_b4
0x2300, //Hi-Temp_rgb_ofs_scl_r5
0x2400, //Hi-Temp_rgb_ofs_scl_g5
0x2500, //Hi-Temp_rgb_ofs_scl_b5
0x2600, //Hi-Temp_rgb_ofs_scl_r6
0x2700, //Hi-Temp_rgb_ofs_scl_g6
0x2800, //Hi-Temp_rgb_ofs_scl_b6
0x2900, //Hi-Temp_std_offset1
0x2a00, //Hi-Temp_std_offset2
0x2bff, //Hi-Temp_std_th_max
0x2c00, //Hi-Temp_std_th_min
0x2d3f, //Hi-Temp_rg1_lum_gain_wgt_th1
0x2e3f, //Hi-Temp_rg1_lum_gain_wgt_th2
0x2f3f, //Hi-Temp_rg1_lum_gain_wgt_th3
0x303f, //Hi-Temp_rg1_lum_gain_wgt_th4
0x3130, //Hi-Temp_rg1_lum_sp1
0x3250, //Hi-Temp_rg1_lum_sp2
0x3380, //Hi-Temp_rg1_lum_sp3
0x34b0, //Hi-Temp_rg1_lum_sp4
0x353f, //Hi-Temp_rg2_gain_wgt_th1
0x363f, //Hi-Temp_rg2_gain_wgt_th2
0x373f, //Hi-Temp_rg2_gain_wgt_th3
0x383f, //Hi-Temp_rg2_gain_wgt_th4
0x3928, //Hi-Temp_rg2_lum_sp1
0x3a50, //Hi-Temp_rg2_lum_sp2
0x3b80, //Hi-Temp_rg2_lum_sp3
0x3cb0, //Hi-Temp_rg2_lum_sp4
0x3d3f, //Hi-Temp_rg3_gain_wgt_th1
0x3e3f, //Hi-Temp_rg3_gain_wgt_th2
0x3f3f, //Hi-Temp_rg3_gain_wgt_th3
0x403f, //Hi-Temp_rg3_gain_wgt_th4
0x4128, //Hi-Temp_rg3_lum_sp1
0x4250, //Hi-Temp_rg3_lum_sp2
0x4380, //Hi-Temp_rg3_lum_sp3
0x44b0, //Hi-Temp_rg3_lum_sp4

0x453f, //Hi-Temp_rg4_gain_wgt_th1
0x463f, //Hi-Temp_rg4_gain_wgt_th2
0x473f, //Hi-Temp_rg4_gain_wgt_th3
0x483f, //Hi-Temp_rg4_gain_wgt_th4
0x4910, //Hi-Temp_rg4_lum_sp1
0x4a30, //Hi-Temp_rg4_lum_sp2
0x4b60, //Hi-Temp_rg4_lum_sp3
0x4c90, //Hi-Temp_rg4_lum_sp4

0x4d3f, //Hi-Temp_rg5_gain_wgt_th1
0x4e3f, //Hi-Temp_rg5_gain_wgt_th2
0x4f3f, //Hi-Temp_rg5_gain_wgt_th3
0x503f, //Hi-Temp_rg5_gain_wgt_th4
0x5128, //Hi-Temp_rg5_lum_sp1
0x5250, //Hi-Temp_rg5_lum_sp2
0x5380, //Hi-Temp_rg5_lum_sp3
0x54b0, //Hi-Temp_rg5_lum_sp4

0x553f, //Hi-Temp_rg6_gain_wgt_th1
0x563f, //Hi-Temp_rg6_gain_wgt_th2
0x573f, //Hi-Temp_rg6_gain_wgt_th3
0x583f, //Hi-Temp_rg6_gain_wgt_th4
0x5928, //Hi-Temp_rg6_lum_sp1
0x5a50, //Hi-Temp_rg6_lum_sp2
0x5b80, //Hi-Temp_rg6_lum_sp3
0x5cb0, //Hi-Temp_rg6_lum_sp4

0x5d19, //Hi-Temp_allgain_x1
0x5e37, //Hi-Temp_allgain_x2
0x5f37, //Hi-Temp_allgain_x3
0x6019, //Hi-Temp_allgain_x4
0x6119, //Hi-Temp_allgain_x5
0x6237, //Hi-Temp_allgain_x6
0x6337, //Hi-Temp_allgain_x7
0x6419, //Hi-Temp_allgain_x8

0x650e, //Hi-Temp_allgain_y0
0x660d, //Hi-Temp_allgain_y1
0x670e, //Hi-Temp_allgain_y2
0x680e, //Hi-Temp_allgain_y3
0x6910, //Hi-Temp_allgain_y4
0x6a10, //Hi-Temp_allgain_y5
0x6b13, //Hi-Temp_allgain_y6
0x6c13, //Hi-Temp_allgain_y7
0x6d14, //Hi-Temp_allgain_y8
0x6e13, //Hi-Temp_allgain_y9
0x6f0f, //Hi-Temp_allgain_y10
0x7011, //Hi-Temp_allgain_y11

0x0e00, // burst end

0x03D3,
0x11FE,	// function block on
0x108F,	// Adaptive on

0x03d8,
0xcc34,
0x03dd,
0xbf34,

///////////////////////////////////////////////////////////////////////////////
// DE ~ E0 Page (DMA Outdoor)
///////////////////////////////////////////////////////////////////////////////

0x03de, //DMA DE Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Outdoor 1111 add 720p 
0x1414,
0x1500, //Outdoor 1114 add 720p 
0x1615,
0x1781, //Outdoor 1115 add 720p 
0x1816,
0x1904, //Outdoor 1116 add 720p 
0x1a17,
0x1b58, //Outdoor 1117 add 720p 
0x1c18,
0x1d30, //Outdoor 1118 add 720p 
0x1e19,
0x1f12, //Outdoor 1119 add 720p 
0x2037,
0x2100, //Outdoor 1137
0x2238,
0x2300, //Outdoor 1138
0x2439,
0x2500, //Outdoor 1139
0x263a,
0x2700, //Outdoor 113a
0x283b,
0x2900, //Outdoor 113b
0x2a3c,
0x2b00, //Outdoor 113c
0x2c3d,
0x2d00, //Outdoor 113d
0x2e3e,
0x2f00, //Outdoor 113e
0x303f,
0x3100, //Outdoor 113f
0x3240,
0x3300, //Outdoor 1140
0x3441,
0x3500, //Outdoor 1141
0x3642,
0x3700, //Outdoor 1142
0x3843,
0x3900, //Outdoor 1143
0x3a49,
0x3b06, //Outdoor 1149 add 720p 
0x3c4a,
0x3d0a, //Outdoor 114a add 720p 
0x3e4b,
0x3f12, //Outdoor 114b add 720p 
0x404c,
0x411c, //Outdoor 114c add 720p 
0x424d,
0x4324, //Outdoor 114d add 720p 
0x444e,
0x4540, //Outdoor 114e add 720p 
0x464f,
0x4780, //Outdoor 114f add 720p 
0x4850,
0x491a, //Outdoor 1150
0x4a51,
0x4b23, //Outdoor 1151
0x4c52,
0x4d2c, //Outdoor 1152
0x4e53,
0x4f3f, //Outdoor 1153
0x5054,
0x513f, //Outdoor 1154
0x5255,
0x533e, //Outdoor 1155
0x5456,
0x553c, //Outdoor 1156
0x5657,
0x573a, //Outdoor 1157
0x5858,
0x593f, //Outdoor 1158
0x5a59,
0x5b3f, //Outdoor 1159
0x5c5a,
0x5d3e, //Outdoor 115a
0x5e5b,
0x5f3a, //Outdoor 115b
0x605c,
0x6137, //Outdoor 115c
0x625d,
0x6334, //Outdoor 115d
0x645e,
0x6532, //Outdoor 115e
0x665f,
0x6730, //Outdoor 115f
0x686e,
0x691c, //Outdoor 116e
0x6a6f,
0x6b18, //Outdoor 116f
0x6c77,
0x6d2b, //Outdoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f2a, //Outdoor 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Outdoor 1179 //Bayer SP Lum Pos3
0x727a,
0x731a, //Outdoor 117a //Bayer SP Lum Pos4
0x747b,
0x751c, //Outdoor 117b //Bayer SP Lum Pos5
0x767c,
0x771a, //Outdoor 117c //Bayer SP Lum Pos6
0x787d,
0x7919, //Outdoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b17, //Outdoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1e, //Outdoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1e, //Outdoor 1180 //Bayer SP Lum Neg2
0x8081,
0x811f, //Outdoor 1181 //Bayer SP Lum Neg3
0x8282,
0x831e, //Outdoor 1182 //Bayer SP Lum Neg4
0x8483,
0x851a, //Outdoor 1183 //Bayer SP Lum Neg5
0x8684,
0x871a, //Outdoor 1184 //Bayer SP Lum Neg6
0x8885,
0x891a, //Outdoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b1a, //Outdoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d1a, //Outdoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f16, //Outdoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9116, //Outdoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9315, //Outdoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9517, //Outdoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9717, //Outdoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9917, //Outdoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b16, //Outdoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Outdoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f20, //Outdoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa123, //Outdoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa321, //Outdoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa521, //Outdoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa720, //Outdoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91b, //Outdoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab18, //Outdoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad2b, //Outdoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf2b, //Outdoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb12b, //Outdoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb32b, //Outdoor 11aa //Bayer SP Edge4
0xb4ab,
0xb52b, //Outdoor 11ab //Bayer SP Edge5
0xb6ac,
0xb72c, //Outdoor 11ac //Bayer SP Edge6
0xb8ad,
0xb931, //Outdoor 11ad //Bayer SP Edge7
0xbaae,
0xbb35, //Outdoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Outdoor 11b7 add 720p 
0xbeb8,
0xbf22, //Outdoor 11b8 add 720p 
0xc0b9,
0xc121, //Outdoor 11b9 add 720p 
0xc2ba,
0xc31e, //Outdoor 11ba add 720p 
0xc4bb,
0xc51c, //Outdoor 11bb add 720p 
0xc6bc,
0xc71a, //Outdoor 11bc add 720p 
0xc8c7,
0xc930, //Outdoor 11c7 //Bayer SP STD1
0xcac8,
0xcb30, //Outdoor 11c8 //Bayer SP STD2
0xccc9,
0xcd30, //Outdoor 11c9 //Bayer SP STD3
0xceca,
0xcf30, //Outdoor 11ca //Bayer SP STD4
0xd0cb,
0xd130, //Outdoor 11cb //Bayer SP STD5
0xd2cc,
0xd330, //Outdoor 11cc //Bayer SP STD6
0xd4cd,
0xd52d, //Outdoor 11cd //Bayer SP STD7
0xd6ce,
0xd72a, //Outdoor 11ce //Bayer SP STD8
0xd8cf,
0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb15,//Outdoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Outdoor 11d1
0xded2,
0xdf40, //Outdoor 11d2
0xe0d3,
0xe1ff, //Outdoor 11d3
0xe2d4,
0xe301, //Outdoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe50a, //Outdoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Outdoor 11d6
0xe8d7,
0xe910,//Outdoor 11d7
0xead8,
0xeb01, //Outdoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed06, //Outdoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Outdoor 11da
0xf0db,
0xf107, //Outdoor 11db
0xf2df,
0xf355, //Outdoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf536, //Outdoor 11e0
0xf6e1,
0xf77a, //Outdoor 11e1
0xf8e2,
0xf935, //Outdoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfba0, //Outdoor 11e3
0xfce4,
0xfd01, //Outdoor 11e4
0x0e00, // burst end

0x03df, //DMA DF Page
0x0e01, // burst start

0x10e5,
0x1120,//Outdoor 11e5
0x12e6,
0x1301, //Outdoor 11e6
0x14e7,
0x151a,//Outdoor 11e7
0x16e8,
0x1701, //Outdoor 11e8
0x18e9,
0x1910, //Outdoor 11e9
0x1aea,
0x1b01, //Outdoor 11ea
0x1ceb,
0x1d12, //Outdoor 11eb
0x1eef,
0x1f33, //Outdoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Outdoor 1240 add 720p 
0x2470,
0x259f, //Outdoor 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Outdoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Outdoor 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Outdoor 1277
0x2c78,
0x2d2f, //Outdoor 1278
0x2e79,
0x2f09, //Outdoor 1279
0x307a,
0x3150, //Outdoor 127a
0x327b,
0x3310, //Outdoor 127b
0x347c,
0x3550, //Outdoor 127c //skin HPF gain
0x367d,
0x3710, //Outdoor 127d
0x387f,
0x3950, //Outdoor 127f
0x3a87,
0x3b3f, //Outdoor 1287 add 720p 
0x3c88,
0x3d3f, //Outdoor 1288 add 720p 
0x3e89,
0x3f3f, //Outdoor 1289 add 720p 
0x408a,
0x413f, //Outdoor 128a add 720p 
0x428b,
0x433f, //Outdoor 128b add 720p 
0x448c,
0x453f, //Outdoor 128c add 720p 
0x468d,
0x473f, //Outdoor 128d add 720p 
0x488e,
0x493f, //Outdoor 128e add 720p 
0x4a8f,
0x4b3f, //Outdoor 128f add 720p 
0x4c90,
0x4d3f, //Outdoor 1290 add 720p 
0x4e91,
0x4f3f, //Outdoor 1291 add 720p 
0x5092,
0x513f, //Outdoor 1292 add 720p 
0x5293,
0x533f, //Outdoor 1293 add 720p 
0x5494,
0x553f, //Outdoor 1294 add 720p 
0x5695,
0x573f, //Outdoor 1295 add 720p 
0x5896,
0x593f, //Outdoor 1296 add 720p 
0x5aae,
0x5b7f, //Outdoor 12ae
0x5caf,
0x5d63,//Outdoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Outdoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Outdoor 12c3 add 720p 
0x62c4,
0x631a, //Outdoor 12c4 add 720p 
0x64c5,
0x650c, //Outdoor 12c5 add 720p 
0x66c6,
0x6791, //Outdoor 12c6
0x68c7,
0x69a4, //Outdoor 12c7
0x6ac8,
0x6b3c, //Outdoor 12c8
0x6cd0,
0x6d08, //Outdoor 12d0 add 720p 
0x6ed1,
0x6f10, //Outdoor 12d1 add 720p 
0x70d2,
0x7118, //Outdoor 12d2 add 720p 
0x72d3,
0x7320, //Outdoor 12d3 add 720p 
0x74d4,
0x7530, //Outdoor 12d4 add 720p 
0x76d5,
0x7760, //Outdoor 12d5 add 720p 
0x78d6,
0x7980, //Outdoor 12d6 add 720p 
0x7ad7,
0x7b30,//Outdoor 12d7
0x7cd8,
0x7d33,//Outdoor 12d8
0x7ed9,
0x7f35,//Outdoor 12d9
0x80da,
0x8135,//Outdoor 12da
0x82db,
0x8334,//Outdoor 12db
0x84dc,
0x8530,//Outdoor 12dc
0x86dd,
0x872a,//Outdoor 12dd
0x88de,
0x8926,//Outdoor 12de
0x8ae0,
0x8b49, //Outdoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Outdoor 12e1
0x8ee2,
0x8f02, //Outdoor 12e2
0x90e3,
0x9120, //Outdoor 12e3 //PS LN graph Y1
0x92e4,
0x9320, //Outdoor 12e4 //PS LN graph Y2
0x94e5,
0x9520, //Outdoor 12e5 //PS LN graph Y3
0x96e6,
0x9720, //Outdoor 12e6 //PS LN graph Y4
0x98e7,
0x9920, //Outdoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b20, //Outdoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d20, //Outdoor 12e9 //PS DY graph Y1
0x9eea,
0x9f20, //Outdoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Outdoor 12eb //PS DY graph Y3
0xa2ec,
0xa31e, //Outdoor 12ec //PS DY graph Y4
0xa4ed,
0xa51d, //Outdoor 12ed //PS DY graph Y5
0xa6ee,
0xa720, //Outdoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Outdoor 12f0
0xaaf1,
0xab2a, //Outdoor 12f1
0xacf2,
0xad32, //Outdoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Outdoor 1310 //Y-NR ENB add  720p 
0xb230,
0xb33f, //Outdoor 1330
0xb431,
0xb53f, //Outdoor 1331
0xb632,
0xb73f, //Outdoor 1332
0xb833,
0xb93f, //Outdoor 1333
0xba34,
0xbb3f, //Outdoor 1334
0xbc35,
0xbd33, //Outdoor 1335
0xbe36,
0xbf2f, //Outdoor 1336
0xc037,
0xc12e, //Outdoor 1337
0xc238,
0xc302, //Outdoor 1338
0xc440,
0xc51e, //Outdoor 1340
0xc641,
0xc722, //Outdoor 1341
0xc842,
0xc962, //Outdoor 1342
0xca43,
0xcb63, //Outdoor 1343
0xcc44,
0xcdff, //Outdoor 1344
0xce45,
0xcf04, //Outdoor 1345
0xd046,
0xd136, //Outdoor 1346
0xd247,
0xd305, //Outdoor 1347
0xd448,
0xd520, //Outdoor 1348
0xd649,
0xd702, //Outdoor 1349
0xd84a,
0xd922, //Outdoor 134a
0xda4b,
0xdb06, //Outdoor 134b
0xdc4c,
0xdd20, //Outdoor 134c
0xde83,
0xdf08, //Outdoor 1383
0xe084,
0xe108, //Outdoor 1384
0xe2b7,
0xe3fd, //Outdoor 13b7
0xe4b8,
0xe5a7, //Outdoor 13b8
0xe6b9,
0xe7fe, //Outdoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Outdoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Outdoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Outdoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Outdoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Outdoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Outdoor 1410
0xf611,
0xf7d8, //Outdoor 1411
0xf812,
0xf910, //Outdoor 1412
0xfa13,
0xfb03, //Outdoor 1413
0xfc14,
0xfd0f, //Outdoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x1015,
0x117b, //Outdoor 1415 // Y Hi filter mask 1/16
0x1216,
0x131c, //Outdoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Outdoor 1417
0x1618,
0x170c, //Outdoor 1418
0x1819,
0x190c, //Outdoor 1419
0x1a1a,
0x1b18, //Outdoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1d, //Outdoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f26, //Outdoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2124, //Outdoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2323, //Outdoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x251a, //Outdoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Outdoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Outdoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b16, //Outdoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d16, //Outdoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f22, //Outdoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3121, //Outdoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3320, //Outdoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x351a, //Outdoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3719, //Outdoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3915, //Outdoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b18, //Outdoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d1b, //Outdoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f24, //Outdoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Outdoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4324, //Outdoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x451a,//Outdoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714,//Outdoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4910,//Outdoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b18,//Outdoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d18,//Outdoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f18, //Outdoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Outdoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x531b, //Outdoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5519, //Outdoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5719, //Outdoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x591e, //Outdoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b25, //Outdoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d25, //Outdoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f24, //Outdoor 1467 //YC2D SP Edge Gain1
0x6068,
0x6124, //Outdoor 1468 //YC2D SP Edge Gain2
0x6269,
0x6328, //Outdoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652e, //Outdoor 146a //YC2D SP Edge Gain4
0x666b,
0x6730, //Outdoor 146b //YC2D SP Edge Gain5
0x686c,
0x6931, //Outdoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b30, //Outdoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d28, //Outdoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f27, //Outdoor 1487 //YC2D SP STD Gain1
0x7088,
0x7128, //Outdoor 1488 //YC2D SP STD Gain2
0x7289,
0x732d, //Outdoor 1489 //YC2D SP STD Gain3
0x748a,
0x752d, //Outdoor 148a //YC2D SP STD Gain4
0x768b,
0x772e, //Outdoor 148b //YC2D SP STD Gain5
0x788c,
0x792b, //Outdoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b28, //Outdoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d25, //Outdoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Outdoor 1497 add 720p 
0x8098,
0x813f, //Outdoor 1498 add 720p 
0x8299,
0x833f, //Outdoor 1499 add 720p 
0x849a,
0x853f, //Outdoor 149a add 720p 
0x869b,
0x873f, //Outdoor 149b add 720p 
0x88a0,
0x893f, //Outdoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Outdoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Outdoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Outdoor 14a3 add 720p 
0x90a4,
0x913f, //Outdoor 14a4 add 720p 
0x92c9,
0x9313, //Outdoor 14c9
0x94ca,
0x9520, //Outdoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Outdoor 1A10 add 720p 
0x9a18,
0x9b3f, //Outdoor 1A18 
0x9c19,
0x9d3f, //Outdoor 1A19
0x9e1a,
0x9f3f, //Outdoor 1A1a
0xa01b,
0xa13f, //Outdoor 1A1b
0xa21c,
0xa33f, //Outdoor 1A1c
0xa41d,
0xa53c, //Outdoor 1A1d
0xa61e,
0xa738, //Outdoor 1A1e
0xa81f,
0xa935, //Outdoor 1A1f
0xaa20,
0xabe7, //Outdoor 1A20 add
0xac2f,
0xadf1, //Outdoor 1A2f add
0xae32,
0xaf87, //Outdoor 1A32 add
0xb034,
0xb1d2, //Outdoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31c, //Outdoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb506,//Outdoor 1A36
0xb637,
0xb740, //Outdoor 1A37
0xb838,
0xb9ff, //Outdoor 1A38
0xba39,
0xbb2e, //Outdoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Outdoor 1A3a
0xbe3b,
0xbf01, //Outdoor 1A3b
0xc03c,
0xc10c, //Outdoor 1A3c
0xc23d,
0xc301, //Outdoor 1A3d
0xc43e,
0xc507, //Outdoor 1A3e
0xc63f,
0xc701, //Outdoor 1A3f
0xc840,
0xc90c, //Outdoor 1A40
0xca41,
0xcb01, //Outdoor 1A41
0xcc42,
0xcd07, //Outdoor 1A42
0xce43,
0xcf2b, //Outdoor 1A43
0xd04d,
0xd10e, //Outdoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd30e, //Outdoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd50e, //Outdoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd713, //Outdoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd917, //Outdoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb17, //Outdoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd17, //Outdoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf17, //Outdoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe114, //Outdoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe311, //Outdoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe510, //Outdoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Outdoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Outdoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Outdoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Outdoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Outdoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf10f, //Outdoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf30f, //Outdoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf510, //Outdoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf717, //Outdoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf919, //Outdoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb17, //Outdoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd16, //Outdoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E0 page.
0xD027,
0xD142,

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x106c,
0x1116, //Outdoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1310, //Outdoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1516, //Outdoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1715, //Outdoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Outdoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Outdoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d13, //Outdoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f13, //Outdoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2113, //Outdoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2329, //Outdoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2529, //Outdoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2729, //Outdoor 1A7f //RGB SP Edge Gain3
0x2880,
0x292f, //Outdoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b2f, //Outdoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d2f, //Outdoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f27, //Outdoor 1A83 //RGB SP Edge Gain7
0x3084,
0x3125, //Outdoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3328, //Outdoor 1A9e //RGB SP STD Gain1
0x349f,
0x3528, //Outdoor 1A9f //RGB SP STD Gain2
0x36a0,
0x372f, //Outdoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x392e, //Outdoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b2d, //Outdoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d2d, //Outdoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f29, //Outdoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4125, //Outdoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4323,//Outdoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Outdoor 1Aa7 add
0x46a8,
0x473f, //Outdoor 1Aa8 add
0x48a9,
0x493f, //Outdoor 1Aa9 add
0x4aaa,
0x4b3f, //Outdoor 1Aaa add
0x4cab,
0x4d3f, //Outdoor 1Aab add
0x4eaf,
0x4f3f, //Outdoor 1Aaf add
0x50b0,
0x513f, //Outdoor 1Ab0 add
0x52b1,
0x533f, //Outdoor 1Ab1 add
0x54b2,
0x553f, //Outdoor 1Ab2 add
0x56b3,
0x573f, //Outdoor 1Ab3 add
0x58ca,
0x5900, //Outdoor 1Aca
0x5ae3,
0x5b13, //Outdoor 1Ae3 add
0x5ce4,
0x5d13, //Outdoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610f, //Outdoor 1070 Trans Func.   130108 Outdoor transFuc Flat graph
0x6271,
0x6300, //Outdoor 1071
0x6472,
0x6500, //Outdoor 1072
0x6673,
0x6700, //Outdoor 1073
0x6874,
0x6900, //Outdoor 1074
0x6a75,
0x6b00, //Outdoor 1075
0x6c76,
0x6d40, //Outdoor 1076
0x6e77,
0x6f40, //Outdoor 1077
0x7078,
0x7100, //Outdoor 1078
0x7279,
0x7340, //Outdoor 1079
0x747a,
0x7500, //Outdoor 107a
0x767b,
0x7740, //Outdoor 107b
0x787c,
0x7900, //Outdoor 107c
0x7a7d,
0x7b07, //Outdoor 107d
0x7c7e,
0x7d0f, //Outdoor 107e
0x7e7f,
0x7f1e, //Outdoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8330, //Outdoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Outdoor 031a (for sun-spot)
0x881b,
0x898c, //Outdoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Outdoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Outdoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9102, //Outdoor 11f0 (for af bug)
0x9203,              
0x9312, // 12 page    
0x9412,              
0x9530, //Outdoor 1212

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E1 ~ E3 Page (DMA Indoor)
///////////////////////////////////////////////////////////////////////////////

0x03e1, //DMA E1 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Indoor 1111 add 720p 
0x1414,
0x1500, //Indoor 1114 add 720p 
0x1615,
0x1781, //Indoor 1115 add 720p 
0x1816,
0x1904, //Indoor 1116 add 720p 
0x1a17,
0x1b58, //Indoor 1117 add 720p 
0x1c18,
0x1d30, //Indoor 1118 add 720p 
0x1e19,
0x1f12, //Indoor 1119 add 720p 
0x2037,
0x2107, //Indoor 1137 
0x2238,
0x2300, //Indoor 1138 //Pre flat R1 LumL
0x2439,
0x25ff, //Indoor 1139 //Pre flat R1 LumH
0x263a,
0x2700, //Indoor 113a
0x283b,
0x2900, //Indoor 113b
0x2a3c,
0x2b00, //Indoor 113c
0x2c3d,
0x2d56, //Indoor 113d
0x2e3e,
0x2f00, //Indoor 113e
0x303f,
0x3100, //Indoor 113f
0x3240,
0x3300, //Indoor 1140
0x3441,
0x352a, //Indoor 1141
0x3642,
0x3700, //Indoor 1142
0x3843,
0x3900, //Indoor 1143
0x3a49,
0x3b06, //Indoor 1149 add 720p 
0x3c4a,
0x3d0a, //Indoor 114a add 720p
0x3e4b,
0x3f12, //Indoor 114b add 720p
0x404c,
0x411c, //Indoor 114c add 720p
0x424d,
0x4324, //Indoor 114d add 720p
0x444e,
0x4540, //Indoor 114e add 720p
0x464f,
0x4780, //Indoor 114f add 720p
0x4850,
0x493f, //Indoor 1150 
0x4a51,
0x4b3f, //Indoor 1151
0x4c52,
0x4d3f, //Indoor 1152
0x4e53,
0x4f3d, //Indoor 1153
0x5054,
0x513c, //Indoor 1154
0x5255,
0x5338, //Indoor 1155
0x5456,
0x5536, //Indoor 1156
0x5657,
0x5734, //Indoor 1157
0x5858,
0x593f, //Indoor 1158
0x5a59,
0x5b3f, //Indoor 1159
0x5c5a,
0x5d3e, //Indoor 115a
0x5e5b,
0x5f38, //Indoor 115b
0x605c,
0x6133, //Indoor 115c
0x625d,
0x6331, //Indoor 115d
0x645e,
0x6530, //Indoor 115e
0x665f,
0x6730, //Indoor 115f
0x686e,
0x6920, //Indoor 116e
0x6a6f,
0x6b18, //Indoor 116f
0x6c77,
0x6d16, //Indoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f16, //Indoor 1178 //Bayer SP Lum Pos2
0x7079,
0x7115, //Indoor 1179 //Bayer SP Lum Pos3
0x727a,
0x7315, //Indoor 117a //Bayer SP Lum Pos4
0x747b,
0x7511, //Indoor 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Indoor 117c //Bayer SP Lum Pos6
0x787d,
0x7910, //Indoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b10, //Indoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d11, //Indoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f11, //Indoor 1180 //Bayer SP Lum Neg2
0x8081,
0x8111, //Indoor 1181 //Bayer SP Lum Neg3
0x8282,
0x8315, //Indoor 1182 //Bayer SP Lum Neg4
0x8483,
0x8516, //Indoor 1183 //Bayer SP Lum Neg5
0x8684,
0x8716, //Indoor 1184 //Bayer SP Lum Neg6
0x8885,
0x8916, //Indoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b16, //Indoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d15, //Indoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f15, //Indoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9113, //Indoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9313, //Indoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9513, //Indoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9713, //Indoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9913, //Indoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Indoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Indoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f16, //Indoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa117, //Indoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa317, //Indoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa51a, //Indoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa71a, //Indoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91a, //Indoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab19, //Indoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad26, //Indoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf26, //Indoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb125, //Indoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb325, //Indoor 11aa //Bayer SP Edge4
0xb4ab,
0xb525, //Indoor 11ab //Bayer SP Edge5
0xb6ac,
0xb725, //Indoor 11ac //Bayer SP Edge6
0xb8ad,
0xb925, //Indoor 11ad //Bayer SP Edge7
0xbaae,
0xbb24, //Indoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Indoor 11b7 add 720p 
0xbeb8,
0xbf22, //Indoor 11b8 add 720p 
0xc0b9,
0xc121, //Indoor 11b9 add 720p 
0xc2ba,
0xc31e, //Indoor 11ba add 720p 
0xc4bb,
0xc51c, //Indoor 11bb add 720p 
0xc6bc,
0xc71a, //Indoor 11bc add 720p 
0xc8c7,
0xc920, //Indoor 11c7 //Bayer SP STD1
0xcac8,
0xcb21, //Indoor 11c8 //Bayer SP STD2
0xccc9,
0xcd22, //Indoor 11c9 //Bayer SP STD3
0xceca,
0xcf24, //Indoor 11ca //Bayer SP STD4
0xd0cb,
0xd124, //Indoor 11cb //Bayer SP STD5
0xd2cc,
0xd324, //Indoor 11cc //Bayer SP STD6
0xd4cd,
0xd520, //Indoor 11cd //Bayer SP STD7
0xd6ce,
0xd71f, //Indoor 11ce //Bayer SP STD8
0xd8cf,
0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb18, //Indoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Indoor 11d1
0xded2,
0xdf40, //Indoor 11d2
0xe0d3,
0xe1ff, //Indoor 11d3
0xe2d4,
0xe302, //Indoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe51c, //Indoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Indoor 11d6
0xe8d7,
0xe910, //Indoor 11d7
0xead8,
0xeb01, //Indoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed0e, //Indoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Indoor 11da
0xf0db,
0xf107, //Indoor 11db
0xf2df,
0xf3cc, //Indoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Indoor 11e0
0xf6e1,
0xf77a, //Indoor 11e1
0xf8e2,
0xf907,//Indoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb18,//Indoor 11e3
0xfce4,
0xfd01, //Indoor 11e4 //Bayer Flat R3 StdL
0x0e00, // burst end

0x03e2, //DMA E2 Page
0x0e01, // burst start

0x10e5,
0x1122,//Indoor 11e5 //Bayer Flat R3 StdH
0x12e6,
0x1300, //Indoor 11e6
0x14e7,
0x150f,//Indoor 11e7
0x16e8,
0x1701, //Indoor 11e8
0x18e9,
0x191d, //Indoor 11e9
0x1aea,
0x1b00, //Indoor 11ea
0x1ceb,
0x1d0a,//Indoor 11eb
0x1eef,
0x1fa0,//Indoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Indoor 1240 add 720p 
0x2470,
0x259f, //Indoor 1270 // Bayer Sharpness ENB add
0x2671,
0x271a, //Indoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Indoor 1272 //Bayer LPF Gain
0x2a77,
0x2b26, //Indoor 1277 //20130412
0x2c78,
0x2d2f, //Indoor 1278
0x2e79,
0x2fff, //Indoor 1279
0x307a,
0x3150, //Indoor 127a
0x327b,
0x3310, //Indoor 127b
0x347c,
0x3564, //Indoor 127c //skin HPF gain
0x367d,
0x3720, //Indoor 127d
0x387f,
0x3950, //Indoor 127f
0x3a87,
0x3b3f, //Indoor 1287 add 720p 
0x3c88,
0x3d3f, //Indoor 1288 add 720p 
0x3e89,
0x3f3f, //Indoor 1289 add 720p 
0x408a,
0x413f, //Indoor 128a add 720p 
0x428b,
0x433f, //Indoor 128b add 720p 
0x448c,
0x453f, //Indoor 128c add 720p 
0x468d,
0x473f, //Indoor 128d add 720p 
0x488e,
0x493f, //Indoor 128e add 720p 
0x4a8f,
0x4b3f, //Indoor 128f add 720p 
0x4c90,
0x4d3f, //Indoor 1290 add 720p 
0x4e91,
0x4f3f, //Indoor 1291 add 720p 
0x5092,
0x513f, //Indoor 1292 add 720p 
0x5293,
0x533f, //Indoor 1293 add 720p 
0x5494,
0x553f, //Indoor 1294 add 720p 
0x5695,
0x573f, //Indoor 1295 add 720p 
0x5896,
0x593f, //Indoor 1296 add 720p 
0x5aae,
0x5b7f, //Indoor 12ae
0x5caf,
0x5d80, //Indoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Indoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Indoor 12c3 add 720p 
0x62c4,
0x631a, //Indoor 12c4 add 720p 
0x64c5,
0x650c, //Indoor 12c5 add 720p 
0x66c6,
0x6791, //Indoor 12c6
0x68c7,
0x69a4, //Indoor 12c7
0x6ac8,
0x6b3c, //Indoor 12c8
0x6cd0,
0x6d08, //Indoor 12d0 add 720p 
0x6ed1,
0x6f10, //Indoor 12d1 add 720p 
0x70d2,
0x7118, //Indoor 12d2 add 720p 
0x72d3,
0x7320, //Indoor 12d3 add 720p 
0x74d4,
0x7530, //Indoor 12d4 add 720p 
0x76d5,
0x7760, //Indoor 12d5 add 720p 
0x78d6,
0x7980, //Indoor 12d6 add 720p 
0x7ad7,
0x7b38, //Indoor 12d7
0x7cd8,
0x7d30, //Indoor 12d8
0x7ed9,
0x7f2a, //Indoor 12d9
0x80da,
0x812a, //Indoor 12da
0x82db,
0x8324, //Indoor 12db
0x84dc,
0x8520, //Indoor 12dc
0x86dd,
0x871a, //Indoor 12dd
0x88de,
0x8916, //Indoor 12de
0x8ae0,
0x8b63, //Indoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Indoor 12e1
0x8ee2,
0x8f02, //Indoor 12e2
0x90e3,
0x9110, //Indoor 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Indoor 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Indoor 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Indoor 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Indoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Indoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Indoor 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Indoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Indoor 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Indoor 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Indoor 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Indoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Indoor 12f0
0xaaf1,
0xab2a, //Indoor 12f1
0xacf2,
0xad32, //Indoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Indoor 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Indoor 1330
0xb431,
0xb520, //Indoor 1331
0xb632,
0xb720, //Indoor 1332
0xb833,
0xb920, //Indoor 1333
0xba34,
0xbb20, //Indoor 1334
0xbc35,
0xbd20, //Indoor 1335
0xbe36,
0xbf20, //Indoor 1336
0xc037,
0xc120, //Indoor 1337
0xc238,
0xc302, //Indoor 1338
0xc440,
0xc518, //Indoor 1340
0xc641,
0xc736, //Indoor 1341
0xc842,
0xc962, //Indoor 1342
0xca43,
0xcb63, //Indoor 1343
0xcc44,
0xcdff, //Indoor 1344
0xce45,
0xcf04, //Indoor 1345
0xd046,
0xd145, //Indoor 1346
0xd247,
0xd305, //Indoor 1347
0xd448,
0xd565, //Indoor 1348
0xd649,
0xd702, //Indoor 1349
0xd84a,
0xd922, //Indoor 134a
0xda4b,
0xdb06, //Indoor 134b
0xdc4c,
0xdd30, //Indoor 134c
0xde83,
0xdf08, //Indoor 1383 //add 20121210
0xe084,
0xe10a, //Indoor 1384 //add 20121210
0xe2b7,
0xe3fa, //Indoor 13b7
0xe4b8,
0xe577, //Indoor 13b8
0xe6b9,
0xe7fe, //Indoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Indoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Indoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Indoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Indoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Indoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Indoor 1410
0xf611,
0xf798, //Indoor 1411
0xf812,
0xf910, //Indoor 1412
0xfa13,
0xfb03, //Indoor 1413
0xfc14,
0xfd10, //Indoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x1015,
0x117b, //Indoor 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Indoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Indoor 1417
0x1618,
0x170c, //Indoor 1418
0x1819,
0x190c, //Indoor 1419
0x1a1a,
0x1b18, //Indoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Indoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f22, //Indoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2121, //Indoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x231c, //Indoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2515, //Indoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2711, //Indoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2910, //Indoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b11, //Indoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Indoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f1a, //Indoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3115, //Indoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3314, //Indoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3513, //Indoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Indoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Indoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b14, //Indoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d15, //Indoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f22, //Indoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Indoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4321, //Indoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4520, //Indoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x471d, //Indoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x491d, //Indoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b1d, //Indoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d1d, //Indoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f19, //Indoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Indoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5316, //Indoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5515, //Indoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5714, //Indoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Indoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b13, //Indoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Indoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f2a, //Indoor 1467 //YC2D SP Edge Gain1
0x6068,
0x612b, //Indoor 1468 //YC2D SP Edge Gain2
0x6269,
0x632c, //Indoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652d, //Indoor 146a //YC2D SP Edge Gain4
0x666b,
0x672d, //Indoor 146b //YC2D SP Edge Gain5
0x686c,
0x692b, //Indoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b2d, //Indoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d2e, //Indoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f14, //Indoor 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Indoor 1488 //YC2D SP STD Gain2
0x7289,
0x7326, //Indoor 1489 //YC2D SP STD Gain3
0x748a,
0x7526, //Indoor 148a //YC2D SP STD Gain4
0x768b,
0x7720, //Indoor 148b //YC2D SP STD Gain5
0x788c,
0x7920, //Indoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Indoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d15, //Indoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Indoor 1497 add 720p 
0x8098,
0x813f, //Indoor 1498 add 720p 
0x8299,
0x833f, //Indoor 1499 add 720p 
0x849a,
0x853f, //Indoor 149a add 720p 
0x869b,
0x873f, //Indoor 149b add 720p 
0x88a0,
0x893f, //Indoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Indoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Indoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Indoor 14a3 add 720p 
0x90a4,
0x913f, //Indoor 14a4 add 720p 
0x92c9,
0x9313, //Indoor 14c9
0x94ca,
0x9527, //Indoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Indoor 1A10 add 720p
0x9a18,
0x9b3f, //Indoor 1A18
0x9c19,
0x9d3f, //Indoor 1A19
0x9e1a,
0x9f2a, //Indoor 1A1a
0xa01b,
0xa127, //Indoor 1A1b
0xa21c,
0xa323, //Indoor 1A1c
0xa41d,
0xa523, //Indoor 1A1d
0xa61e,
0xa723, //Indoor 1A1e
0xa81f,
0xa923, //Indoor 1A1f
0xaa20,
0xabe7, //Indoor 1A20 add 720p 
0xac2f,
0xadf1, //Indoor 1A2f add 720p 
0xae32,
0xaf87, //Indoor 1A32 add 720p 
0xb034,
0xb1d0, //Indoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Indoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Indoor 1A36
0xb637,
0xb740, //Indoor 1A37
0xb838,
0xb9ff, //Indoor 1A38
0xba39,
0xbb1d, //Indoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Indoor 1A3a //RGB Flat R2_Lum H
0xbe3b,
0xbf00, //Indoor 1A3b
0xc03c,
0xc14c, //Indoor 1A3c
0xc23d,
0xc300, //Indoor 1A3d
0xc43e,
0xc513, //Indoor 1A3e
0xc63f,
0xc700, //Indoor 1A3f
0xc840,
0xc92a, //Indoor 1A40
0xca41,
0xcb00, //Indoor 1A41
0xcc42,
0xcd17, //Indoor 1A42
0xce43,
0xcf2c, //Indoor 1A43
0xd04d,
0xd112, //Indoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Indoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd511, //Indoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd710, //Indoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd910, //Indoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Indoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd10, //Indoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf10, //Indoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe113, //Indoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe313, //Indoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Indoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe710, //Indoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe910, //Indoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Indoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed10, //Indoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef10, //Indoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Indoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Indoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf513, //Indoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf714, //Indoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf914, //Indoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb14, //Indoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd15, //Indoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E3 page.
0xD62A,
0xD7FA,

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x106c,
0x1115, //Indoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Indoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Indoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Indoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1913, //Indoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Indoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d14, //Indoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Indoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2114, //Indoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2322, //Indoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2523, //Indoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2724, //Indoor 1A7f //RGB SP Edge Gain3
0x2880,
0x2925, //Indoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b26, //Indoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d27, //Indoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f29, //Indoor 1A83 //RGB SP Edge Gain7
0x3084,
0x312b, //Indoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Indoor 1A9e //RGB SP STD Gain1
0x349f,
0x3523, //Indoor 1A9f //RGB SP STD Gain2
0x36a0,
0x3726, //Indoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x3926, //Indoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b26, //Indoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d26, //Indoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f26, //Indoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4126, //Indoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4334, //Indoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Indoor 1Aa7 add 720p 
0x46a8,
0x473f, //Indoor 1Aa8 add 720p 
0x48a9,
0x493f, //Indoor 1Aa9 add 720p 
0x4aaa,
0x4b3f, //Indoor 1Aaa add 720p 
0x4cab,
0x4d3f, //Indoor 1Aab add 720p 
0x4eaf,
0x4f3f, //Indoor 1Aaf add 720p 
0x50b0,
0x513f, //Indoor 1Ab0 add 720p 
0x52b1,
0x533f, //Indoor 1Ab1 add 720p 
0x54b2,
0x553f, //Indoor 1Ab2 add 720p 
0x56b3,
0x573f, //Indoor 1Ab3 add 720p 
0x58ca,
0x5900, //Indoor 1Aca
0x5ae3,
0x5b13, //Indoor 1Ae3 add
0x5ce4,
0x5d13, //Indoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Indoor 1070 Trans Func.   130108 Indoor transFuc Flat graph
0x6271,
0x6301, //Indoor 1071
0x6472,
0x6589, //Indoor 1072
0x6673,
0x67d4, //Indoor 1073
0x6874,
0x6900, //Indoor 1074
0x6a75,
0x6b00, //Indoor 1075
0x6c76,
0x6d40, //Indoor 1076
0x6e77,
0x6f47, //Indoor 1077
0x7078,
0x71ae, //Indoor 1078
0x7279,
0x7350, //Indoor 1079
0x747a,
0x7500, //Indoor 107a
0x767b,
0x7750, //Indoor 107b
0x787c,
0x7900, //Indoor 107c
0x7a7d,
0x7b05, //Indoor 107d
0x7c7e,
0x7d0f, //Indoor 107e
0x7e7f,
0x7f2f, //Indoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x832a, //Indoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Indoor 031a (for sun-spot)
0x881b,
0x898c, //Indoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Indoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Indoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9103, //Indoor 11f0 (for af bug)
0x9203,             
0x9312, // 12 page   
0x9412,             
0x9508, //Indoor 1212

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E4 ~ E6 Page (DMA Dark1)
///////////////////////////////////////////////////////////////////////////////

0x03e4, //DMA E4 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Dark1 1111 add 720p 
0x1414,
0x1500, //Dark1 1114 add 720p 
0x1615,
0x1781, //Dark1 1115 add 720p 
0x1816,
0x1904, //Dark1 1116 add 720p 
0x1a17,
0x1b58, //Dark1 1117 add 720p 
0x1c18,
0x1d30, //Dark1 1118 add 720p 
0x1e19,
0x1f12, //Dark1 1119 add 720p 
0x2037,
0x211f, //Dark1 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark1 1138 //Pre Flat R1 LumL
0x2439,
0x25ff, //Dark1 1139 //Pre Flat R1 LumH
0x263a,
0x2700, //Dark1 113a
0x283b,
0x2900, //Dark1 113b
0x2a3c,
0x2b00, //Dark1 113c
0x2c3d,
0x2d53, //Dark1 113d
0x2e3e,
0x2f00, //Dark1 113e
0x303f,
0x3100, //Dark1 113f
0x3240,
0x3300, //Dark1 1140
0x3441,
0x352d, //Dark1 1141
0x3642,
0x3700, //Dark1 1142
0x3843,
0x3900, //Dark1 1143
0x3a49,
0x3b06, //Dark1 1149 add 720p 
0x3c4a,
0x3d0a, //Dark1 114a add 720p 
0x3e4b,
0x3f12, //Dark1 114b add 720p 
0x404c,
0x411c, //Dark1 114c add 720p 
0x424d,
0x4324, //Dark1 114d add 720p 
0x444e,
0x4540, //Dark1 114e add 720p 
0x464f,
0x4780, //Dark1 114f add 720p 
0x4850,
0x493f, //Dark1 1150 
0x4a51,
0x4b3f, //Dark1 1151
0x4c52,
0x4d3f, //Dark1 1152
0x4e53,
0x4f3d, //Dark1 1153
0x5054,
0x513c, //Dark1 1154
0x5255,
0x5338, //Dark1 1155
0x5456,
0x5536, //Dark1 1156
0x5657,
0x5734, //Dark1 1157
0x5858,
0x593f, //Dark1 1158
0x5a59,
0x5b3f, //Dark1 1159
0x5c5a,
0x5d3e, //Dark1 115a
0x5e5b,
0x5f38, //Dark1 115b
0x605c,
0x6133, //Dark1 115c
0x625d,
0x6331, //Dark1 115d
0x645e,
0x6530, //Dark1 115e
0x665f,
0x6730, //Dark1 115f
0x686e,
0x6920, //Dark1 116e
0x6a6f,
0x6b18, //Dark1 116f
0x6c77,
0x6d12, //Dark1 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f0f, //Dark1 1178 //Bayer SP Lum Pos2
0x7079,
0x710f, //Dark1 1179 //Bayer SP Lum Pos3
0x727a,
0x7312, //Dark1 117a //Bayer SP Lum Pos4
0x747b,
0x7512, //Dark1 117b //Bayer SP Lum Pos5
0x767c,
0x7712, //Dark1 117c //Bayer SP Lum Pos6
0x787d,
0x7912, //Dark1 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b12, //Dark1 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d12, //Dark1 117f //Bayer SP Lum Neg1
0x7e80,
0x7f0f, //Dark1 1180 //Bayer SP Lum Neg2
0x8081,
0x810f, //Dark1 1181 //Bayer SP Lum Neg3
0x8282,
0x8312, //Dark1 1182 //Bayer SP Lum Neg4
0x8483,
0x8512, //Dark1 1183 //Bayer SP Lum Neg5
0x8684,
0x8712, //Dark1 1184 //Bayer SP Lum Neg6
0x8885,
0x8912, //Dark1 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b12, //Dark1 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d0f, //Dark1 118f //Bayer SP Dy Pos1
0x8e90,
0x8f0f, //Dark1 1190 //Bayer SP Dy Pos2
0x9091,
0x9112, //Dark1 1191 //Bayer SP Dy Pos3
0x9292,
0x9312, //Dark1 1192 //Bayer SP Dy Pos4
0x9493,
0x9512, //Dark1 1193 //Bayer SP Dy Pos5
0x9694,
0x9712, //Dark1 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark1 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b12, //Dark1 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d0f, //Dark1 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f0f, //Dark1 1198 //Bayer SP Dy Neg2
0xa099,
0xa112, //Dark1 1199 //Bayer SP Dy Neg3
0xa29a,
0xa312, //Dark1 119a //Bayer SP Dy Neg4
0xa49b,
0xa512, //Dark1 119b //Bayer SP Dy Neg5
0xa69c,
0xa712, //Dark1 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark1 119d //Bayer SP Dy Neg7
0xaa9e,
0xab12, //Dark1 119e //Bayer SP Dy Neg8
0xaca7,
0xad1c, //Dark1 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark1 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark1 11a9 //Bayer SP Edge3
0xb2aa,
0xb318, //Dark1 11aa //Bayer SP Edge4
0xb4ab,
0xb51d, //Dark1 11ab //Bayer SP Edge5
0xb6ac,
0xb720, //Dark1 11ac //Bayer SP Edge6
0xb8ad,
0xb920, //Dark1 11ad //Bayer SP Edge7
0xbaae,
0xbb20, //Dark1 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Dark1 11b7 add 720p 
0xbeb8,
0xbf22, //Dark1 11b8 add 720p 
0xc0b9,
0xc121, //Dark1 11b9 add 720p 
0xc2ba,
0xc31e, //Dark1 11ba add 720p 
0xc4bb,
0xc51c, //Dark1 11bb add 720p 
0xc6bc,
0xc71a, //Dark1 11bc add 720p 
0xc8c7,
0xc912, //Dark1 11c7 //Bayer SP STD1
0xcac8,
0xcb12, //Dark1 11c8 //Bayer SP STD2
0xccc9,
0xcd13, //Dark1 11c9 //Bayer SP STD3
0xceca,
0xcf18, //Dark1 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark1 11cb //Bayer SP STD5
0xd2cc,
0xd318, //Dark1 11cc //Bayer SP STD6
0xd4cd,
0xd518, //Dark1 11cd //Bayer SP STD7
0xd6ce,
0xd718, //Dark1 11ce //Bayer SP STD8
0xd8cf,
0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark1 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xddff, //Dark1 11d1
0xded2,
0xdf00, //Dark1 11d2
0xe0d3,
0xe1ff, //Dark1 11d3
0xe2d4,
0xe300, //Dark1 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe557,//Dark1 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark1 11d6
0xe8d7,
0xe92a, //Dark1 11d7
0xead8,
0xeb00, //Dark1 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed27, //Dark1 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark1 11da
0xf0db,
0xf120, //Dark1 11db
0xf2df,
0xf3ff, //Dark1 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Dark1 11e0
0xf6e1,
0xf77a, //Dark1 11e1
0xf8e2,
0xf900, //Dark1 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Dark1 11e3
0xfce4,
0xfd01, //Dark1 11e4
0x0e00, // burst end

0x03e5, //DMA E5 Page
0x0e01, // burst start

0x10e5,
0x1121, //Dark1 11e5
0x12e6,
0x1300, //Dark1 11e6
0x14e7,
0x1500, //Dark1 11e7
0x16e8,
0x1701, //Dark1 11e8
0x18e9,
0x191d, //Dark1 11e9
0x1aea,
0x1b00, //Dark1 11ea
0x1ceb,
0x1d00, //Dark1 11eb
0x1eef,
0x1fff, //Dark1 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Dark1 1240 add 720p 
0x2470,
0x259f, //Dark1 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Dark1 1271 //Bayer HPF Gain
0x2872,
0x2916, //Dark1 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Dark1 1277
0x2c78,
0x2d2f, //Dark1 1278
0x2e79,
0x2fff, //Dark1 1279
0x307a,
0x3150, //Dark1 127a
0x327b,
0x3310, //Dark1 127b
0x347c,
0x3564, //Dark1 127c //skin HPF gain
0x367d,
0x3720, //Dark1 127d
0x387f,
0x3950, //Dark1 127f
0x3a87,
0x3b3f, //Dark1 1287 add 720p 
0x3c88,
0x3d3f, //Dark1 1288 add 720p 
0x3e89,
0x3f3f, //Dark1 1289 add 720p 
0x408a,
0x413f, //Dark1 128a add 720p 
0x428b,
0x433f, //Dark1 128b add 720p 
0x448c,
0x453f, //Dark1 128c add 720p 
0x468d,
0x473f, //Dark1 128d add 720p 
0x488e,
0x493f, //Dark1 128e add 720p 
0x4a8f,
0x4b3f, //Dark1 128f add 720p 
0x4c90,
0x4d3f, //Dark1 1290 add 720p 
0x4e91,
0x4f3f, //Dark1 1291 add 720p 
0x5092,
0x513f, //Dark1 1292 add 720p 
0x5293,
0x533f, //Dark1 1293 add 720p 
0x5494,
0x553f, //Dark1 1294 add 720p 
0x5695,
0x573f, //Dark1 1295 add 720p 
0x5896,
0x593f, //Dark1 1296 add 720p 
0x5aae,
0x5b7f, //Dark1 12ae
0x5caf,
0x5d80, //Dark1 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark1 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Dark1 12c3 add 720p 
0x62c4,
0x631a, //Dark1 12c4 add 720p 
0x64c5,
0x650c, //Dark1 12c5 add 720p 
0x66c6,
0x6791, //Dark1 12c6
0x68c7,
0x69a4, //Dark1 12c7
0x6ac8,
0x6b3c, //Dark1 12c8
0x6cd0,
0x6d08, //Dark1 12d0 add 720p 
0x6ed1,
0x6f10, //Dark1 12d1 add 720p 
0x70d2,
0x7118, //Dark1 12d2 add 720p 
0x72d3,
0x7320, //Dark1 12d3 add 720p 
0x74d4,
0x7530, //Dark1 12d4 add 720p 
0x76d5,
0x7760, //Dark1 12d5 add 720p 
0x78d6,
0x7980, //Dark1 12d6 add 720p 
0x7ad7,
0x7b38, //Dark1 12d7
0x7cd8,
0x7d30, //Dark1 12d8
0x7ed9,
0x7f2a, //Dark1 12d9
0x80da,
0x812a, //Dark1 12da
0x82db,
0x8324, //Dark1 12db
0x84dc,
0x8520, //Dark1 12dc
0x86dd,
0x871a, //Dark1 12dd
0x88de,
0x8916, //Dark1 12de
0x8ae0,
0x8b63, //Dark1 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark1 12e1
0x8ee2,
0x8f02, //Dark1 12e2
0x90e3,
0x9110, //Dark1 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark1 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark1 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark1 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark1 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark1 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark1 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark1 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark1 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark1 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark1 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark1 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark1 12f0
0xaaf1,
0xab2a, //Dark1 12f1
0xacf2,
0xad32, //Dark1 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Dark1 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Dark1 1330
0xb431,
0xb520, //Dark1 1331
0xb632,
0xb720, //Dark1 1332
0xb833,
0xb920, //Dark1 1333
0xba34,
0xbb20, //Dark1 1334
0xbc35,
0xbd20, //Dark1 1335
0xbe36,
0xbf20, //Dark1 1336
0xc037,
0xc120, //Dark1 1337
0xc238,
0xc302, //Dark1 1338
0xc440,
0xc518, //Dark1 1340
0xc641,
0xc736, //Dark1 1341
0xc842,
0xc962, //Dark1 1342
0xca43,
0xcb63, //Dark1 1343
0xcc44,
0xcdff, //Dark1 1344
0xce45,
0xcf04, //Dark1 1345
0xd046,
0xd145, //Dark1 1346
0xd247,
0xd305, //Dark1 1347
0xd448,
0xd565, //Dark1 1348
0xd649,
0xd702, //Dark1 1349
0xd84a,
0xd922, //Dark1 134a
0xda4b,
0xdb06, //Dark1 134b
0xdc4c,
0xdd30, //Dark1 134c
0xde83,
0xdf08, //Dark1 1383
0xe084,
0xe10a, //Dark1 1384
0xe2b7,
0xe3fa, //Dark1 13b7
0xe4b8,
0xe577, //Dark1 13b8
0xe6b9,
0xe7fe, //Dark1 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Dark1 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark1 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark1 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark1 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark1 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Dark1 1410
0xf611,
0xf798, //Dark1 1411
0xf812,
0xf910, //Dark1 1412
0xfa13,
0xfb03, //Dark1 1413
0xfc14,
0xfd23,//Dark1 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x1015,
0x117b, //Dark1 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Dark1 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark1 1417
0x1618,
0x170c, //Dark1 1418
0x1819,
0x190c, //Dark1 1419
0x1a1a,
0x1b24,//Dark1 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d24,//Dark1 141b //YC2D Post STD gain Neg
0x1e27,
0x1f14, //Dark1 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2114, //Dark1 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2314, //Dark1 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2514, //Dark1 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Dark1 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark1 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b14, //Dark1 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d14, //Dark1 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f12, //Dark1 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3112, //Dark1 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3312, //Dark1 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark1 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Dark1 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Dark1 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark1 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d12, //Dark1 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f14, //Dark1 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4114, //Dark1 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4314, //Dark1 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4514, //Dark1 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Dark1 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark1 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b14, //Dark1 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d14, //Dark1 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f12, //Dark1 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5112, //Dark1 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5312, //Dark1 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5512, //Dark1 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5712, //Dark1 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5912, //Dark1 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark1 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Dark1 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f20, //Dark1 1467 //YC2D SP Edge Gain1
0x6068,
0x6120, //Dark1 1468 //YC2D SP Edge Gain2
0x6269,
0x6320, //Dark1 1469 //YC2D SP Edge Gain3
0x646a,
0x6520, //Dark1 146a //YC2D SP Edge Gain4
0x666b,
0x6720, //Dark1 146b //YC2D SP Edge Gain5
0x686c,
0x6920, //Dark1 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b20, //Dark1 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d20, //Dark1 146e //YC2D SP Edge Gain8
0x6e87,
0x6f2a,//Dark1 1487 //YC2D SP STD Gain1
0x7088,
0x712a,//Dark1 1488 //YC2D SP STD Gain2
0x7289,
0x732a,//Dark1 1489 //YC2D SP STD Gain3
0x748a,
0x752a,//Dark1 148a //YC2D SP STD Gain4
0x768b,
0x772a,//Dark1 148b //YC2D SP STD Gain5
0x788c,
0x791a, //Dark1 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Dark1 148d //YC2D SP STD Gain7
0x7c8e,
0x7d1a, //Dark1 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Dark1 1497 add 720p 
0x8098,
0x813f, //Dark1 1498 add 720p 
0x8299,
0x833f, //Dark1 1499 add 720p 
0x849a,
0x853f, //Dark1 149a add 720p 
0x869b,
0x873f, //Dark1 149b add 720p 
0x88a0,
0x893f, //Dark1 14a0 add 720p 
0x8aa1,
0x8b3f, //Dark1 14a1 add 720p 
0x8ca2,
0x8d3f, //Dark1 14a2 add 720p 
0x8ea3,
0x8f3f, //Dark1 14a3 add 720p 
0x90a4,
0x913f, //Dark1 14a4 add 720p 
0x92c9,
0x9313, //Dark1 14c9
0x94ca,
0x9527, //Dark1 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Dark1 1A10 add 720p 
0x9a18,
0x9b3f, //Dark1 1A18
0x9c19,
0x9d3f, //Dark1 1A19
0x9e1a,
0x9f2a, //Dark1 1A1a
0xa01b,
0xa127, //Dark1 1A1b
0xa21c,
0xa323, //Dark1 1A1c
0xa41d,
0xa523, //Dark1 1A1d
0xa61e,
0xa723, //Dark1 1A1e
0xa81f,
0xa923, //Dark1 1A1f
0xaa20,
0xabe7, //Dark1 1A20 add 720p 
0xac2f,
0xadf1, //Dark1 1A2f add 720p 
0xae32,
0xaf87, //Dark1 1A32 add 720p 
0xb034,
0xb1d0, //Dark1 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Dark1 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Dark1 1A36
0xb637,
0xb740, //Dark1 1A37
0xb838,
0xb9ff, //Dark1 1A38
0xba39,
0xbb11, //Dark1 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark1 1A3a
0xbe3b,
0xbf00, //Dark1 1A3b
0xc03c,
0xc14c, //Dark1 1A3c
0xc23d,
0xc300, //Dark1 1A3d
0xc43e,
0xc513, //Dark1 1A3e
0xc63f,
0xc700, //Dark1 1A3f
0xc840,
0xc92a, //Dark1 1A40
0xca41,
0xcb00, //Dark1 1A41
0xcc42,
0xcd17, //Dark1 1A42
0xce43,
0xcf2c, //Dark1 1A43
0xd04d,
0xd112, //Dark1 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Dark1 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd512, //Dark1 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark1 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd912, //Dark1 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb12, //Dark1 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd12, //Dark1 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf12, //Dark1 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe112, //Dark1 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe312, //Dark1 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Dark1 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark1 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Dark1 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Dark1 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Dark1 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Dark1 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Dark1 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Dark1 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf512, //Dark1 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf712, //Dark1 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf912, //Dark1 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb12, //Dark1 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd12, //Dark1 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E6 page.
0xDC2E,
0xDDB2,

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark1 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Dark1 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Dark1 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Dark1 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Dark1 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b12, //Dark1 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d12, //Dark1 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f12, //Dark1 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark1 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark1 1A7d //RGB SP Edge Gain1
0x247e,
0x2520, //Dark1 1A7e //RGB SP Edge Gain2
0x267f,
0x2720, //Dark1 1A7f //RGB SP Edge Gain3
0x2880,
0x2920, //Dark1 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b20, //Dark1 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d20, //Dark1 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f20, //Dark1 1A83 //RGB SP Edge Gain7
0x3084,
0x3120, //Dark1 1A84 //RGB SP Edge Gain8
0x329e,
0x331a, //Dark1 1A9e //RGB SP STD Gain1
0x349f,
0x351a, //Dark1 1A9f //RGB SP STD Gain2
0x36a0,
0x371a, //Dark1 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391a, //Dark1 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark1 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d1a, //Dark1 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f1a, //Dark1 1Aa4 //RGB SP STD Gain7
0x40a5,
0x411a, //Dark1 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4336,//Dark1 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Dark1 1Aa7 add 720p
0x46a8,
0x473f, //Dark1 1Aa8 add 720p
0x48a9,
0x493f, //Dark1 1Aa9 add 720p
0x4aaa,
0x4b3f, //Dark1 1Aaa add 720p
0x4cab,
0x4d3f, //Dark1 1Aab add 720p
0x4eaf,
0x4f3f, //Dark1 1Aaf add 720p
0x50b0,
0x513f, //Dark1 1Ab0 add 720p
0x52b1,
0x533f, //Dark1 1Ab1 add 720p
0x54b2,
0x553f, //Dark1 1Ab2 add 720p
0x56b3,
0x573f, //Dark1 1Ab3 add 720p
0x58ca,
0x5900, //Dark1 1Aca
0x5ae3,
0x5b13, //Dark1 1Ae3 add 720p
0x5ce4,
0x5d13, //Dark1 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark1 1070 Trans Func.   130108 Dark1 transFuc Flat graph
0x6271,
0x630a, //Dark1 1071
0x6472,
0x65be, //Dark1 1072
0x6673,
0x67cc, //Dark1 1073
0x6874,
0x6900, //Dark1 1074
0x6a75,
0x6b00, //Dark1 1075
0x6c76,
0x6d20, //Dark1 1076
0x6e77,
0x6f33, //Dark1 1077
0x7078,
0x7133, //Dark1 1078
0x7279,
0x7349, //Dark1 1079
0x747a,
0x7599, //Dark1 107a
0x767b,
0x7749, //Dark1 107b
0x787c,
0x7999, //Dark1 107c
0x7a7d,
0x7b07, //Dark1 107d
0x7c7e,
0x7d0f, //Dark1 107e
0x7e7f,
0x7f1e, //Dark1 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark1 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark1 031a (for sun-spot)
0x881b,
0x897c, //Dark1 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark1 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark1 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9104, //Dark1 11f0 (for af bug)
0x9203,            
0x9312, // 12 page  
0x9412,            
0x9503, //Dark1 1212
0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E7 ~ E9 Page (DMA Dark2)
///////////////////////////////////////////////////////////////////////////////

0x03e7, //DMA E7 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Dark2 1111 add 720p
0x1414,
0x1500, //Dark2 1114 add 720p
0x1615,
0x1781, //Dark2 1115 add 720p
0x1816,
0x1904, //Dark2 1116 add 720p
0x1a17,
0x1b58, //Dark2 1117 add 720p
0x1c18,
0x1d30, //Dark2 1118 add 720p
0x1e19,
0x1f12, //Dark2 1119 add 720p
0x2037,
0x211f, //Dark2 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark2 1138 //Pre Flat R1 LumL
0x2439,
0x25ff, //Dark2 1139 //Pre Flat R1 LumH
0x263a,
0x2700, //Dark2 113a
0x283b,
0x2900, //Dark2 113b
0x2a3c,
0x2b00, //Dark2 113c
0x2c3d,
0x2d53, //Dark2 113d
0x2e3e,
0x2f00, //Dark2 113e
0x303f,
0x3100, //Dark2 113f
0x3240,
0x3300, //Dark2 1140
0x3441,
0x352d, //Dark2 1141
0x3642,
0x3700, //Dark2 1142
0x3843,
0x3900, //Dark2 1143
0x3a49,
0x3b06, //Dark2 1149 add 720p
0x3c4a,
0x3d0a, //Dark2 114a add 720p
0x3e4b,
0x3f12, //Dark2 114b add 720p
0x404c,
0x411c, //Dark2 114c add 720p
0x424d,
0x4324, //Dark2 114d add 720p
0x444e,
0x4540, //Dark2 114e add 720p
0x464f,
0x4780, //Dark2 114f add 720p
0x4850,
0x493f, //Dark2 1150 
0x4a51,
0x4b3f, //Dark2 1151
0x4c52,
0x4d3f, //Dark2 1152
0x4e53,
0x4f3d, //Dark2 1153
0x5054,
0x513c, //Dark2 1154
0x5255,
0x5338, //Dark2 1155
0x5456,
0x5536, //Dark2 1156
0x5657,
0x5734, //Dark2 1157
0x5858,
0x593f, //Dark2 1158
0x5a59,
0x5b3f, //Dark2 1159
0x5c5a,
0x5d3e, //Dark2 115a
0x5e5b,
0x5f38, //Dark2 115b
0x605c,
0x6133, //Dark2 115c
0x625d,
0x6331, //Dark2 115d
0x645e,
0x6530, //Dark2 115e
0x665f,
0x6730, //Dark2 115f
0x686e,
0x6920, //Dark2 116e
0x6a6f,
0x6b18, //Dark2 116f
0x6c77,
0x6d12, //Dark2 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f0f, //Dark2 1178 //Bayer SP Lum Pos2
0x7079,
0x710f, //Dark2 1179 //Bayer SP Lum Pos3
0x727a,
0x7312, //Dark2 117a //Bayer SP Lum Pos4
0x747b,
0x7512, //Dark2 117b //Bayer SP Lum Pos5
0x767c,
0x7712, //Dark2 117c //Bayer SP Lum Pos6
0x787d,
0x7912, //Dark2 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b12, //Dark2 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d12, //Dark2 117f //Bayer SP Lum Neg1
0x7e80,
0x7f0f, //Dark2 1180 //Bayer SP Lum Neg2
0x8081,
0x810f, //Dark2 1181 //Bayer SP Lum Neg3
0x8282,
0x8312, //Dark2 1182 //Bayer SP Lum Neg4
0x8483,
0x8512, //Dark2 1183 //Bayer SP Lum Neg5
0x8684,
0x8712, //Dark2 1184 //Bayer SP Lum Neg6
0x8885,
0x8912, //Dark2 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b12, //Dark2 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d0f, //Dark2 118f //Bayer SP Dy Pos1
0x8e90,
0x8f0f, //Dark2 1190 //Bayer SP Dy Pos2
0x9091,
0x9112, //Dark2 1191 //Bayer SP Dy Pos3
0x9292,
0x9312, //Dark2 1192 //Bayer SP Dy Pos4
0x9493,
0x9512, //Dark2 1193 //Bayer SP Dy Pos5
0x9694,
0x9712, //Dark2 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark2 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b12, //Dark2 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d0f, //Dark2 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f0f, //Dark2 1198 //Bayer SP Dy Neg2
0xa099,
0xa112, //Dark2 1199 //Bayer SP Dy Neg3
0xa29a,
0xa312, //Dark2 119a //Bayer SP Dy Neg4
0xa49b,
0xa512, //Dark2 119b //Bayer SP Dy Neg5
0xa69c,
0xa712, //Dark2 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark2 119d //Bayer SP Dy Neg7
0xaa9e,
0xab12, //Dark2 119e //Bayer SP Dy Neg8
0xaca7,
0xad1c, //Dark2 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark2 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark2 11a9 //Bayer SP Edge3
0xb2aa,
0xb318, //Dark2 11aa //Bayer SP Edge4
0xb4ab,
0xb51d, //Dark2 11ab //Bayer SP Edge5
0xb6ac,
0xb720, //Dark2 11ac //Bayer SP Edge6
0xb8ad,
0xb920, //Dark2 11ad //Bayer SP Edge7
0xbaae,
0xbb20, //Dark2 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Dark2 11b7 add 720p
0xbeb8,
0xbf22, //Dark2 11b8 add 720p
0xc0b9,
0xc121, //Dark2 11b9 add 720p
0xc2ba,
0xc31e, //Dark2 11ba add 720p
0xc4bb,
0xc51c, //Dark2 11bb add 720p
0xc6bc,
0xc71a, //Dark2 11bc add 720p
0xc8c7,
0xc912, //Dark2 11c7 //Bayer SP STD1
0xcac8,
0xcb12, //Dark2 11c8 //Bayer SP STD2
0xccc9,
0xcd13, //Dark2 11c9 //Bayer SP STD3
0xceca,
0xcf18, //Dark2 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark2 11cb //Bayer SP STD5
0xd2cc,
0xd318, //Dark2 11cc //Bayer SP STD6
0xd4cd,
0xd518, //Dark2 11cd //Bayer SP STD7
0xd6ce,
0xd718, //Dark2 11ce //Bayer SP STD8
0xd8cf,
0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark2 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xddff, //Dark2 11d1
0xded2,
0xdf00, //Dark2 11d2
0xe0d3,
0xe1ff, //Dark2 11d3
0xe2d4,
0xe300, //Dark2 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe557,//Dark2 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark2 11d6
0xe8d7,
0xe92a, //Dark2 11d7
0xead8,
0xeb00, //Dark2 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed27, //Dark2 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark2 11da
0xf0db,
0xf120, //Dark2 11db
0xf2df,
0xf3ff, //Dark2 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Dark2 11e0
0xf6e1,
0xf77a, //Dark2 11e1
0xf8e2,
0xf900, //Dark2 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Dark2 11e3
0xfce4,
0xfd01, //Dark2 11e4
0x0e00, // burst end

0x03e8, //DMA E8 Page
0x0e01, // burst start

0x10e5,
0x1121, //Dark2 11e5
0x12e6,
0x1300, //Dark2 11e6
0x14e7,
0x1500, //Dark2 11e7
0x16e8,
0x1701, //Dark2 11e8
0x18e9,
0x191d, //Dark2 11e9
0x1aea,
0x1b00, //Dark2 11ea
0x1ceb,
0x1d00, //Dark2 11eb
0x1eef,
0x1fff, //Dark2 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Dark2 1240 add 720p
0x2470,
0x259f, //Dark2 1270 // Bayer Sharpness ENB add 720p
0x2671,
0x271a, //Dark2 1271 //Bayer HPF Gain
0x2872,
0x2916, //Dark2 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Dark2 1277
0x2c78,
0x2d2f, //Dark2 1278
0x2e79,
0x2fff, //Dark2 1279
0x307a,
0x3150, //Dark2 127a
0x327b,
0x3310, //Dark2 127b
0x347c,
0x3564, //Dark2 127c //skin HPF gain
0x367d,
0x3720, //Dark2 127d
0x387f,
0x3950, //Dark2 127f
0x3a87,
0x3b3f, //Dark2 1287 add 720p
0x3c88,
0x3d3f, //Dark2 1288 add 720p
0x3e89,
0x3f3f, //Dark2 1289 add 720p
0x408a,
0x413f, //Dark2 128a add 720p
0x428b,
0x433f, //Dark2 128b add 720p
0x448c,
0x453f, //Dark2 128c add 720p
0x468d,
0x473f, //Dark2 128d add 720p
0x488e,
0x493f, //Dark2 128e add 720p
0x4a8f,
0x4b3f, //Dark2 128f add 720p
0x4c90,
0x4d3f, //Dark2 1290 add 720p
0x4e91,
0x4f3f, //Dark2 1291 add 720p
0x5092,
0x513f, //Dark2 1292 add 720p
0x5293,
0x533f, //Dark2 1293 add 720p
0x5494,
0x553f, //Dark2 1294 add 720p
0x5695,
0x573f, //Dark2 1295 add 720p
0x5896,
0x593f, //Dark2 1296 add 720p
0x5aae,
0x5b7f, //Dark2 12ae
0x5caf,
0x5d80, //Dark2 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark2 12c0 // CI-LPF ENB add 720p
0x60c3,
0x613c, //Dark2 12c3 add 720p
0x62c4,
0x631a, //Dark2 12c4 add 720p
0x64c5,
0x650c, //Dark2 12c5 add 720p
0x66c6,
0x6791, //Dark2 12c6
0x68c7,
0x69a4, //Dark2 12c7
0x6ac8,
0x6b3c, //Dark2 12c8
0x6cd0,
0x6d08, //Dark2 12d0 add 720p
0x6ed1,
0x6f10, //Dark2 12d1 add 720p
0x70d2,
0x7118, //Dark2 12d2 add 720p
0x72d3,
0x7320, //Dark2 12d3 add 720p
0x74d4,
0x7530, //Dark2 12d4 add 720p
0x76d5,
0x7760, //Dark2 12d5 add 720p
0x78d6,
0x7980, //Dark2 12d6 add 720p
0x7ad7,
0x7b38, //Dark2 12d7
0x7cd8,
0x7d30, //Dark2 12d8
0x7ed9,
0x7f2a, //Dark2 12d9
0x80da,
0x812a, //Dark2 12da
0x82db,
0x8324, //Dark2 12db
0x84dc,
0x8520, //Dark2 12dc
0x86dd,
0x871a, //Dark2 12dd
0x88de,
0x8916, //Dark2 12de
0x8ae0,
0x8b63, //Dark2 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark2 12e1
0x8ee2,
0x8f02, //Dark2 12e2
0x90e3,
0x9110, //Dark2 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark2 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark2 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark2 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark2 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark2 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark2 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark2 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark2 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark2 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark2 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark2 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark2 12f0
0xaaf1,
0xab2a, //Dark2 12f1
0xacf2,
0xad32, //Dark2 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Dark2 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Dark2 1330
0xb431,
0xb520, //Dark2 1331
0xb632,
0xb720, //Dark2 1332
0xb833,
0xb920, //Dark2 1333
0xba34,
0xbb20, //Dark2 1334
0xbc35,
0xbd20, //Dark2 1335
0xbe36,
0xbf20, //Dark2 1336
0xc037,
0xc120, //Dark2 1337
0xc238,
0xc302, //Dark2 1338
0xc440,
0xc518, //Dark2 1340
0xc641,
0xc736, //Dark2 1341
0xc842,
0xc962, //Dark2 1342
0xca43,
0xcb63, //Dark2 1343
0xcc44,
0xcdff, //Dark2 1344
0xce45,
0xcf04, //Dark2 1345
0xd046,
0xd145, //Dark2 1346
0xd247,
0xd305, //Dark2 1347
0xd448,
0xd565, //Dark2 1348
0xd649,
0xd702, //Dark2 1349
0xd84a,
0xd922, //Dark2 134a
0xda4b,
0xdb06, //Dark2 134b
0xdc4c,
0xdd30, //Dark2 134c
0xde83,
0xdf08, //Dark2 1383
0xe084,
0xe10a, //Dark2 1384
0xe2b7,
0xe3fa, //Dark2 13b7
0xe4b8,
0xe577, //Dark2 13b8
0xe6b9,
0xe7fe, //Dark2 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Dark2 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark2 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark2 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark2 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark2 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Dark2 1410
0xf611,
0xf798, //Dark2 1411
0xf812,
0xf910, //Dark2 1412
0xfa13,
0xfb03, //Dark2 1413
0xfc14,
0xfd23,//Dark2 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x1015,
0x117b, //Dark2 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Dark2 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark2 1417
0x1618,
0x170c, //Dark2 1418
0x1819,
0x190c, //Dark2 1419
0x1a1a,
0x1b24,//Dark2 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d24,//Dark2 141b //YC2D Post STD gain Neg
0x1e27,
0x1f14, //Dark2 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2114, //Dark2 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2314, //Dark2 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2514, //Dark2 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Dark2 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark2 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b14, //Dark2 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d14, //Dark2 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f12, //Dark2 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3112, //Dark2 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3312, //Dark2 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark2 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Dark2 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Dark2 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark2 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d12, //Dark2 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f14, //Dark2 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4114, //Dark2 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4314, //Dark2 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4514, //Dark2 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Dark2 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark2 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b14, //Dark2 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d14, //Dark2 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f12, //Dark2 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5112, //Dark2 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5312, //Dark2 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5512, //Dark2 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5712, //Dark2 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5912, //Dark2 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark2 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Dark2 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f20, //Dark2 1467 //YC2D SP Edge Gain1
0x6068,
0x6120, //Dark2 1468 //YC2D SP Edge Gain2
0x6269,
0x6320, //Dark2 1469 //YC2D SP Edge Gain3
0x646a,
0x6520, //Dark2 146a //YC2D SP Edge Gain4
0x666b,
0x6720, //Dark2 146b //YC2D SP Edge Gain5
0x686c,
0x6920, //Dark2 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b20, //Dark2 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d20, //Dark2 146e //YC2D SP Edge Gain8
0x6e87,
0x6f2a,//Dark2 1487 //YC2D SP STD Gain1
0x7088,
0x712a,//Dark2 1488 //YC2D SP STD Gain2
0x7289,
0x732a,//Dark2 1489 //YC2D SP STD Gain3
0x748a,
0x752a,//Dark2 148a //YC2D SP STD Gain4
0x768b,
0x772a,//Dark2 148b //YC2D SP STD Gain5
0x788c,
0x791a, //Dark2 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Dark2 148d //YC2D SP STD Gain7
0x7c8e,
0x7d1a, //Dark2 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Dark2 1497 add 720p
0x8098,
0x813f, //Dark2 1498 add 720p
0x8299,
0x833f, //Dark2 1499 add 720p
0x849a,
0x853f, //Dark2 149a add 720p
0x869b,
0x873f, //Dark2 149b add 720p
0x88a0,
0x893f, //Dark2 14a0 add 720p
0x8aa1,
0x8b3f, //Dark2 14a1 add 720p
0x8ca2,
0x8d3f, //Dark2 14a2 add 720p
0x8ea3,
0x8f3f, //Dark2 14a3 add 720p
0x90a4,
0x913f, //Dark2 14a4 add 720p
0x92c9,
0x9313, //Dark2 14c9
0x94ca,
0x9527, //Dark2 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Dark2 1A10 add 720p
0x9a18,
0x9b3f, //Dark2 1A18
0x9c19,
0x9d3f, //Dark2 1A19
0x9e1a,
0x9f2a, //Dark2 1A1a
0xa01b,
0xa127, //Dark2 1A1b
0xa21c,
0xa323, //Dark2 1A1c
0xa41d,
0xa523, //Dark2 1A1d
0xa61e,
0xa723, //Dark2 1A1e
0xa81f,
0xa923, //Dark2 1A1f
0xaa20,
0xabe7, //Dark2 1A20 add 720p
0xac2f,
0xadf1, //Dark2 1A2f add 720p
0xae32,
0xaf87, //Dark2 1A32 add 720p
0xb034,
0xb1d0, //Dark2 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Dark2 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Dark2 1A36
0xb637,
0xb740, //Dark2 1A37
0xb838,
0xb9ff, //Dark2 1A38
0xba39,
0xbb11, //Dark2 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark2 1A3a
0xbe3b,
0xbf00, //Dark2 1A3b
0xc03c,
0xc14c, //Dark2 1A3c
0xc23d,
0xc300, //Dark2 1A3d
0xc43e,
0xc513, //Dark2 1A3e
0xc63f,
0xc700, //Dark2 1A3f
0xc840,
0xc92a, //Dark2 1A40
0xca41,
0xcb00, //Dark2 1A41
0xcc42,
0xcd17, //Dark2 1A42
0xce43,
0xcf2c, //Dark2 1A43
0xd04d,
0xd112, //Dark2 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Dark2 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd512, //Dark2 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark2 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd912, //Dark2 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb12, //Dark2 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd12, //Dark2 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf12, //Dark2 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe112, //Dark2 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe312, //Dark2 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Dark2 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark2 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Dark2 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Dark2 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Dark2 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Dark2 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Dark2 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Dark2 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf512, //Dark2 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf712, //Dark2 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf912, //Dark2 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb12, //Dark2 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd12, //Dark2 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E9 page.
0xE232,
0xE36A,

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark2 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Dark2 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Dark2 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Dark2 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Dark2 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b12, //Dark2 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d12, //Dark2 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f12, //Dark2 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark2 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark2 1A7d //RGB SP Edge Gain1
0x247e,
0x2520, //Dark2 1A7e //RGB SP Edge Gain2
0x267f,
0x2720, //Dark2 1A7f //RGB SP Edge Gain3
0x2880,
0x2920, //Dark2 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b20, //Dark2 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d20, //Dark2 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f20, //Dark2 1A83 //RGB SP Edge Gain7
0x3084,
0x3120, //Dark2 1A84 //RGB SP Edge Gain8
0x329e,
0x331a, //Dark2 1A9e //RGB SP STD Gain1
0x349f,
0x351a, //Dark2 1A9f //RGB SP STD Gain2
0x36a0,
0x371a, //Dark2 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391a, //Dark2 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark2 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d1a, //Dark2 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f1a, //Dark2 1Aa4 //RGB SP STD Gain7
0x40a5,
0x411a, //Dark2 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4336,//Dark2 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Dark2 1Aa7 add 720p
0x46a8,
0x473f, //Dark2 1Aa8 add 720p
0x48a9,
0x493f, //Dark2 1Aa9 add 720p
0x4aaa,
0x4b3f, //Dark2 1Aaa add 720p
0x4cab,
0x4d3f, //Dark2 1Aab add 720p
0x4eaf,
0x4f3f, //Dark2 1Aaf add 720p
0x50b0,
0x513f, //Dark2 1Ab0 add 720p
0x52b1,
0x533f, //Dark2 1Ab1 add 720p
0x54b2,
0x553f, //Dark2 1Ab2 add 720p
0x56b3,
0x573f, //Dark2 1Ab3 add 720p
0x58ca,
0x5900, //Dark2 1Aca
0x5ae3,
0x5b13, //Dark2 1Ae3 add 720p
0x5ce4,
0x5d13, //Dark2 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark2 1070 Trans Func.   130108 Dark2 transFuc Flat graph
0x6271,
0x6306, //Dark2 1071
0x6472,
0x65be, //Dark2 1072
0x6673,
0x6799, //Dark2 1073
0x6874,
0x6900, //Dark2 1074
0x6a75,
0x6b00, //Dark2 1075
0x6c76,
0x6d20, //Dark2 1076
0x6e77,
0x6f33, //Dark2 1077
0x7078,
0x7133, //Dark2 1078
0x7279,
0x7340, //Dark2 1079
0x747a,
0x7500, //Dark2 107a
0x767b,
0x7740, //Dark2 107b
0x787c,
0x7900, //Dark2 107c
0x7a7d,
0x7b07, //Dark2 107d
0x7c7e,
0x7d0f, //Dark2 107e
0x7e7f,
0x7f1e, //Dark2 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark2 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark2 031a (for sun-spot)
0x881b,
0x897c, //Dark2 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark2 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark2 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9105, //Dark2 11f0 (for af bug)
0x9203,            
0x9312, // 12 page  
0x9412,            
0x9503, //Dark2 1212
0x0e00, // burst end

//--------------------------------------------------------------------------//
// MIPI TX Setting  //PCLK 54MHz
//--------------------------------------------------------------------------//
0x0305,  // Page05
0x1100,  // lvds_ctl_2 //Phone set not continuous           
0x1200,  // crc_ctl
0x1300,  // serial_ctl
0x1400,  // ser_out_ctl_1
0x1500,  // dphy_fifo_ctl
0x1602,  // lvds_inout_ctl1
0x1700,  // lvds_inout_ctl2
0x1880,  // lvds_inout_ctl3
0x1900,  // lvds_inout_ctl4
0x1af0,  // lvds_time_ctl
0x1c01,  // tlpx_time_l_dp
0x1d0f,  // tlpx_time_l_dn
0x1e08,  // hs_zero_time
0x1f0a,  // hs_trail_time
0x21b8,  // hs_sync_code
0x2200,  // frame_start_id
0x2301,  // frame_end_id
0x241e,  // long_packet_id
0x2500,  // s_pkt_wc_h
0x2600,  // s_pkt_wc_l
0x2708,  // lvds_frame_end_cnt_h
0x2800,  // lvds_frame_end_cnt_l
0x2a06,  // lvds_image_width_h
0x2b40,  // lvds_image_width_l
0x2c04,  // lvds_image_height_h
0x2db0,  // lvds_image_height_l
0x3008, // l_pkt_wc_h  // Pre = 1024 * 2 (YUV)
0x3100,  // l_pkt_wc_l
0x320f,  // clk_zero_time
0x330b,  // clk_post_time
0x3403,  // clk_prepare_time
0x3504,  // clk_trail_time
0x3601,  // clk_tlpx_time_dp
0x3706,  // clk_tlpx_time_dn
0x3907,  // lvds_bias_ctl
0x3a00,  // lvds_test_tx
0x4200,  // mipi_test_width_l
0x4300,  // mipi_test_height_l
0x4400,  // mipi_test_size_h
0x4500,  // mipi_test_hsync_st
0x4600,  // mipi_test_hblank
0x4700,  // mipi_test_vsync_st
0x4800,  // mipi_test_vsync_end
0x49ff,  // ulps_size_opt1
0x4a0a,  // ulps_size_opt2
0x4b22,  // ulps_size_opt3
0x4c41,  // hs_wakeup_size_h
0x4d20,  // hs_wakeup_size_l
0x4e00,  // mipi_int_time_h
0x4fff,  // mipi_int_time_l
0x500A,  // cntns_clk_wait_h
0x5100,  // cntns_clk_wait_l
0x5740,  // mipi_dmy_reg
0x6000,  // mipi_frame_pkt_opt
0x6108,  // line_cnt_value_h
0x6200,  // line_cnt_value_l
0x101c,  // lvds_ctl_1

};

/*===========================================*/
/*CAMERA_SNAPSHOT  - �Կ�   */
/*============================================*/
#if 1
static SensorConfigScript sr352_Capture[] ={

0x03c8,	//AWB Off
0x1052,

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0xff0c,	// 120ms

///////////////////////////////////////////
// D9 Page(Capture function)
///////////////////////////////////////////
0x03d9,
0x1c04,	// Capture Pll Div (CAP_OPCLK_DIV / 2.0)
///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1000,	// full
0x1180,
0x1701,  // ISP Divider2 1/2
0x2000,
0x2106,	// row start set
0x2200,
0x2306,	// col start set

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1111, //Full & HD(crop) 0410
0x1200, //Full & HD(crop) 0410

//--------------------------------------//
//PWR margin setting
//--------------------------------------//
0x0311,
0x101F,	//Bit[4]=Hi
0x0312,
0x709F,	//Bit[0]=Hi

};
#endif
#if 0
static SensorConfigScript sr352_Capture_640_480[] ={

///////////////////////////////////////////
//  Scaler 640_480
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1443, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa202, //scale wid
0xa380,
0xa401, //scale hgt
0xa5e0,
0xa601,	//fw scaler col start
0xa703, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x3005,  // l_pkt_wc_h  // Full = 640 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_960_720[] ={

///////////////////////////////////////////
//  Scaler 960_720
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa203, //scale wid
0xa3c0,
0xa402, //scale hgt
0xa5d0,

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x3007,  // l_pkt_wc_h  // Full = 960 * 2 (YUV)
0x3180,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_1280_720[] ={

///////////////////////////////////////////
//  Scaler 1280_720
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa205, //scale wid
0xa300,
0xa402, //scale hgt
0xa5d0,
0xa602,	//fw scaler col start
0xa706, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x300a,  // l_pkt_wc_h  // Full = 1280 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_1280_960[] ={

///////////////////////////////////////////
//  Scaler 1280_960
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa205, //scale wid
0xa300,
0xa403, //scale hgt
0xa5c0,
0xa601,	//fw scaler col start
0xa706, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x300a,  // l_pkt_wc_h  // Full = 1280 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_1536_864[] ={

///////////////////////////////////////////
//  Scaler 1536_864
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa206, //scale wid
0xa300,
0xa403, //scale hgt
0xa560,
0xa603,	//fw scaler col start
0xa707, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x300c,  // l_pkt_wc_h  // Full = 1536 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_1600_1200[] ={

///////////////////////////////////////////
//  Scaler 1600_1200
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa206, //scale wid
0xa340,
0xa404, //scale hgt
0xa5b0,
0xa601,	//fw scaler col start
0xa707, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x300c,  // l_pkt_wc_h  // Full = 1600 * 2 (YUV)
0x3180,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};

static SensorConfigScript sr352_Capture_2048_1152[] ={

///////////////////////////////////////////
//  Scaler 2048_1152
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa208, //scale wid
0xa300,
0xa404, //scale hgt
0xa580,
0xa602,	//fw scaler col start
0xa708, //fw scaler row start

0xa100, //zoom step
0xa0c0, //fw scaler on

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x3010,  // l_pkt_wc_h  // Full = 2048 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------//

};
#endif
static SensorConfigScript sr352_Capture_2048_1536[] ={

///////////////////////////////////////////
//  Windowing
///////////////////////////////////////////
0x0300,
0x2000,
0x2108,
0x2200,
0x2301,
///////////////////////////////////////////
//  Scaler Off
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off

//Scaler
0x03c0,
0xa000, //fw scaler off

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x3010,  // l_pkt_wc_h  // Full = 1280 * 2 (YUV)
0x3100,  // l_pkt_wc_l

//------------------------------------------------//
//TAP Capture Setting
//------------------------------------------------//
0x03c0,
0x9800,
0x9910,
0x9a72,
0x9bf0,		// exp_band_100 * 2
0x9c00,
0x9d0d,
0x9eb2,
0x9f40,		// exp_band_120 * 2

0x0326,
0x63f8,		// set custom call address (high)
0x6400,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call

0xff01, //delay 10ms

0x0300,
0x1e01, // frame update

0x0326,
0x3029,	// Capture On

0x0300,
0x0100,	// sleep off

0x0326,
0x63f8,		// set custom call address (high)
0x6492,		// set custom call address (low)

0x03c0,
0x6601,	// jump custom call
//------------------------------------------------///

};


/*===========================================*/
/* CAMERA_PREVIEW - �Կ� �� ������ ���ͽ� ���� */
/*============================================*/

static SensorConfigScript sr352_preview[] ={

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1041,	// binning + prev1
0x1190,	// 1frame skip
0x1700, // ISP Divider2 1/1
0x2000,
0x2100,	// preview row start set
0x2200,
0x2300,	// preview col start set

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1111, //Preview 0410
0x1202, //Preview1 0410

///////////////////////////////////////////
// 19 Page  Scalor off
///////////////////////////////////////////
0x0319,	//Scaler Off
0x1000,
0x1400, //sawtooth off

0x03c0,	//Scaler Off
0xa000,
0xa600,
0xa700,

//--------------------------------------//
//PWR margin setting
//--------------------------------------//
0x0311,
0x100F,	//Bit[4]=Low
0x709E,	//Bit[0]=Low

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05
0x3008,  // l_pkt_wc_h  // Pre = 1024 * 2 (YUV)
0x3100,  // l_pkt_wc_l
0x101c,  // lvds_ctl_1
//--------------------------------------------------------------------------//

0x0300,
0x1e01, // frame update

0x0326,
0x3028,	// Preview // sleep off �� ��

0x0300,
0x0100,	// sleep off

0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff01, //delay 10ms


};


static SensorConfigScript sr352_preview_176_144[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 176_144
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1443, //sawtooth on 320_240 176_144 pre filter

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa200,
0xa3b0,
0xa400,
0xa590,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3001,  // l_pkt_wc_h  // Pre = 176 * 2 (YUV)
0x3160,  // l_pkt_wc_l
//------------------------------------//

0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};
/*
static SensorConfigScript sr352_preview_320_240[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 320_240
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1443, //sawtooth on 320_240 176_144 pre filter

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa201, //width
0xa340,
0xa400, //height
0xa5f0,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3002,  // l_pkt_wc_h  // Pre = 320 * 2 (YUV)
0x3180,  // l_pkt_wc_l
//------------------------------------//


0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};

static SensorConfigScript sr352_preview_352_288[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 352_288
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa201, //width
0xa360,
0xa401, //height
0xa520,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3002,  // l_pkt_wc_h  // Pre = 352 * 2 (YUV)
0x31c0,  // l_pkt_wc_l
//------------------------------------//

0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};

static SensorConfigScript sr352_preview_528_432[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 528_432
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa202, //width
0xa310,
0xa401, //height
0xa5b0,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3004,  // l_pkt_wc_h  // Pre = 528 * 2 (YUV)
0x3120,  // l_pkt_wc_l
//------------------------------------//
0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms
};
*/
static SensorConfigScript sr352_preview_640_480[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 640_480
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa202, //width
0xa380,
0xa401, //height
0xa5e0,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3005,  // l_pkt_wc_h  // Pre = 640 * 2 (YUV)
0x3100,  // l_pkt_wc_l
//------------------------------------//

0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};
/*
static SensorConfigScript sr352_preview_720_480[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 720_480
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa202, //width
0xa3d0,
0xa401, //height
0xa5e0,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3005,  // l_pkt_wc_h  // Pre = 720 * 2 (YUV)
0x31a0,  // l_pkt_wc_l
//------------------------------------//


0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};*/

static SensorConfigScript sr352_preview_800_480[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 800_480
/////////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa203, //width
0xa320,
0xa401, //height
0xa5e0,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3006,  // l_pkt_wc_h  // Pre = 800 * 2 (YUV)
0x3140,  // l_pkt_wc_l
//------------------------------------//


0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};
/*
static SensorConfigScript sr352_preview_800_600[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 800_600
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa203, //width
0xa320,
0xa402, //height
0xa558,

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3006,  // l_pkt_wc_h  // Pre = 800 * 2 (YUV)
0x3140,  // l_pkt_wc_l
//------------------------------------//


0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};*/

static SensorConfigScript sr352_preview_1024_576[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

///////////////////////////////////////////
//  Scaler 1024x576
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1403, //sawtooth on

//Scaler
0x03c0,
0xa000, //fw scaler off
0xa204, //width
0xa300,
0xa402, //height
0xa540,
0xa600,	//fw scaler col start
0xa700, //fw scaler row start

0xa100, //zoom step 
0xa0c0, //fw scaler on  

0x0319,
0x1007, //hw scaler on

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3008,  // l_pkt_wc_h  // Pre = 1024 * 2 (YUV)
0x3100,  // l_pkt_wc_l
//------------------------------------//


0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};
/*
static SensorConfigScript sr352_preview_1024_768[] =
{
0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	// Sleep On

0x03c1,
0x1007, // ssd tranfer enable

0x0300,
0x2000, // Row Start
0x2100,
0x2200, // Col Start
0x2300,

///////////////////////////////////////////
//  Scaler 1024x768
///////////////////////////////////////////
0x0319,
0x1000, //hw scaler off
0x1400, //sawtooth off

//Scaler
0x03c0,
0xa000, //fw scaler off

///////////////////////////////////////////
// 05 Page MIPI Size
///////////////////////////////////////////
0x0305,  // Page05

0x3008,  // l_pkt_wc_h  // Pre = 1024 * 2 (YUV)
0x3100,  // l_pkt_wc_l
//------------------------------------//
0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0xff01, //delay 10ms

};*/


/*===========================================*/
/*CAMERA_RECORDING WITH 30fps  */
/*============================================*/
#if 1
static SensorConfigScript sr352_recording_50Hz_HD[] = {

///////////////////////////////////////////////////////////////////////////////
// HD Initial Start
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// mcu clock enable for bus release
///////////////////////////////////////////////////////////////////////////////
0x0326,
0x1089,
0x1080,

///////////////////////////////////////////////////////////////////////////////
// reset
///////////////////////////////////////////////////////////////////////////////
0x0300,
0x0101,
0x0107,
0x0101,

0x0daa, // ESD Check Register
0x0faa, // ESD Check Register

///////////////////////////////////////////////////////////////////////////////
// pad drive / pll setting
///////////////////////////////////////////////////////////////////////////////

0x0300,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0207,		// pclk_drive = 000b, i2c_drive = 111b
0x0c07,		// d_pad_drive = 000b, gpio_pad_drive = 111b
//////////////////////////////////////////////////////////////////
0x0725, //mode_pll1  24mhz / (5+1) = 4mhz
0x0856,	//mode_pll2  isp clk = 86Mhz;
0x0981, //mode_pll3  // MIPI 4x div 1/1 // isp clk div = 1/4
0x07A5,
0x07A5,
0x07A5,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0A60, //mode_pll4 for mipi mode
0x0Ae0, //mode_pll4 for mipi mode

0x0326,
0x1B03,		// bus clk div = 1/4

///////////////////////////////////////////////////////////////////////////////
// 7 Page(memory configuration)
///////////////////////////////////////////////////////////////////////////////
0x0307,
0x2101,	//SSD sram clock inv on
0x3345,	//bit[6]:C-NR DC ���� ����

///////////////////////////////////////////////////////////////////////////////
// mcu reset
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x1080,		// mcu reset
0x1089,		// mcu clk enable
0x1108,		// xdata clear
0x1100,		// xdata clear
0xff01,   // delay 10ms

///////////////////////////////////////////////////////////////////////////////
// opt download
///////////////////////////////////////////////////////////////////////////////

0x030A,
0x1200,	// otp clock enable

// timing for 86mhz
0x402f,	// otp cfg 1
0x4155,	// otp cfg 2
0x422f,	// otp cfg 3
0x432f,	// otp cfg 4
0x442f,	// otp cfg 5
0x4522,	// otp cfg 6
0x465a,	// otp cfg 7
0x4709,	// otp cfg 8
0x4802,	// otp cfg 9
0x49b8,	// otp cfg 10
0x4A2f,	// otp cfg 11
0x4B85,	// otp cfg 12
0x4C55,	// otp cfg 13

0xff01,	//delay 10ms

// downlaod otp - system data
0x2000,	// otp addr = Otp:0000h
0x2100,	// otp addr = Otp:0000h
0x2000,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2100,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1301,	// start download system data
0x1300,	// toggle start

0xff01,	//delay 10ms

// download otp - mcu data
0x2000,	// otp addr = Otp:0080h
0x2180,	// otp addr = Otp:0080h
0x2000,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2e01,	// otp download size = 0100
0x2f00,	// otp download size = 0100
0x1801,	// link xdata to otp
0x3010,	// otp mcu buffer addr = Xdata:105Dh
0x315D,	// otp mcu buffer addr = Xdata:105Dh
0x1302,	// start download mcu data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

// download otp - dpc data
0x2001,	// otp addr = Otp:0180h
0x2180,	// otp addr = Otp:0180h
0x2001,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1801,	// link xdata to otp
0x3033,	// otp mcu buffer addr = Xdata:3384h
0x3184,	// otp mcu buffer addr = Xdata:3384h
0x1304,	// start download dpc data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

0x030A,
0x1280,	// otp clock disable

///////////////////////////////////////////////////////////////////////////////
// 0 Page
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1000,	// Full size
0x1190,	// Fixed mode off
0x1200,
0x1300,
0x1501,
0x1700,	// Clock inversion off
0x1800,
0x1D0D,	//Group_frame_update
0x1E01,	//Group_frame_update_reset
0x2000,
0x2100,	// preview row start set
0x2200,
0x2308,	// preview col start set
0x2402,
0x25d0,	// height 720
0x2605,
0x2700,	// width 1280

///////////////////////////////////////////////////////////////////////////////
//ONE LINE SETTING
0x0300,
0x4c07,	// one_line = 1850
0x4d3a,

///////////////////////////////////////////////////////////////////////////////
0x5200,	// Vsync H
0x5314,	// Vsync L
///////////////////////////////////////////////////////////////////////////////

//Pixel windowing
0x8001,	// pixel_row_start for 720p crop mode
0x819e,	// pwin_row_start = 414
0x8202,	// pwin_row_height = 738
0x83e2,
0x8401,	// pwin_col_start = 384
0x8580,
0x8605,	// pwin_col_width = 1312
0x8720,

///////////////////////////////////////////////////////////////////////////////
// 1 Page
///////////////////////////////////////////////////////////////////////////////

0x0301,
0x1062,	// BLC=ON, column BLC, col_OBP DPC
0x1111,   // BLC offset ENB + Adaptive BLC ENB B[4]
0x1200,
0x1339,	// BLC(Frame BLC ofs - Column ALC ofs)+FrameALC skip
0x1400,
0x238F,	// Frame BLC avg ���� for 8 frame
0x5004, // blc height = 4
0x5144,
0x6000,
0x6100,
0x6200,
0x6300,
0x787f,	// ramp_rst_offset = 128
0x7904,	// ramp offset
0x7b04,	// ramp offset
0x7e00,

///////////////////////////////////////////////////////////////////////////////
// 2 Page
///////////////////////////////////////////////////////////////////////////////

0x0302,
0x1b80,
0x1d40,
0x2310,
0x4008,
0x418a,	//20130213 Rev BC ver. ADC input range @ 800mv
0x460a,	// + 3.3V, -0.9V
0x4717, // 20121129 2.9V
0x481a,
0x4913,
0x54c0,
0x5540,
0x5633,
0xa001,
0xa17c,
0xa203,
0xa34d,
0xa403,
0xa5b0,
0xa606,
0xa7f2,
0xa801,
0xa94f,
0xaa02,
0xab23,
0xac02,
0xad74,
0xae04,
0xaf17,

///////////////////////////////////////////////////////////////////////////////
// 3 Page
///////////////////////////////////////////////////////////////////////////////

0x0303,
0x1a06, // cds_s1
0x1b7c,
0x1e06,
0x1f7c,
0x4200,
0x4346,
0x4600,
0x4774,
0x4a00,
0x4b44,
0x4e00,
0x4f44,
0x5200,
0x5320,
0x5600,
0x5720,
0x5A00,
0x5b20,
0x6A00,
0x6B6f,
0x7206, // s_addr_cut
0x7340,
0x7806, // rx half_rst
0x793b,
0x7A06,
0x7B45,
0x7C06,
0x7D3b,
0x7E06,
0x7F45,
0x8406, // tx half_rst
0x853b,
0x8606,
0x8745,
0x8806,
0x893b,
0x8A06,
0x8B45,
0x9206, // sx
0x9331,
0x9606,
0x9731,
0x9806, // sxb
0x9931,
0x9c06,
0x9d31,

0xe000,
0xe120,
0xfc06, // clamp_sig
0xfd38,

///////////////////////////////////////////////////////////////////////////////
// 4 Page
///////////////////////////////////////////////////////////////////////////////

0x0304,
0x1003,	//Ramp multiple sampling

0x5a06, // cds_pxl_smpl
0x5b78,
0x5e06,
0x5f78,
0x6206,
0x6378,

///////////////////////////////////////////////////////////////////////////////
// mcu start
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x6200,	// normal mode start
0x6500,	// watchdog disable
0x1009,	// mcu reset release
//Analog setting ���Ŀ� MCU�� reset ��Ŵ.

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1001, // otp_dpc_ctl
0x1111, //HD 0415
0x1200, //HD 0415

///////////////////////////////////////////////////////////////////////////////
// 15 Page (LSC)
///////////////////////////////////////////////////////////////////////////////

0x0315,
0x1000,	// LSC OFF
0x1100, //gap y disable

///////////////////////////////////////////////////////////////////////////////
// set lsc parameter
///////////////////////////////////////////////////////////////////////////////

0x030a,
0x1901,

0x1180, // B[7] LSC burst mode ENB

0x0326,
0x4002,	// auto increment enable
0x4401,
0x45a3,	// LSC bank0 start addr H
0x4600,	// LSC bank0 start addr L

//LSC G channel reg________________________ 20130416 LSC Blending DNP 90_CWF 5_TL84 5

0x0e01, //BURST_START

0x423c, //G Value 
0x4213,
0x42af,
0x423d,
0x4263,
0x42b1,
0x4237,
0x42a3,
0x423d,
0x4230,
0x4202,
0x42cf,
0x422b,
0x4222,
0x42b0,
0x422c,
0x42d2,
0x42fe,
0x4233,
0x42f3,
0x427e,
0x423b,
0x4273,
0x42d7,
0x423a,
0x42d3,
0x42e1,
0x4237,
0x4293,
0x4283,
0x4238,
0x42d3,
0x4260,
0x4231,
0x42e2,
0x42d4,
0x4228,
0x42d2,
0x4257,
0x4223,
0x4292,
0x4237,
0x4225,
0x4262,
0x428f,
0x422d,
0x4283,
0x4224,
0x4236,
0x4293,
0x4296,
0x4239,
0x42b3,
0x42a0,
0x4236,
0x4293,
0x4257,
0x4234,
0x4253,
0x4210,
0x422c,
0x4212,
0x426b,
0x4221,
0x42a1,
0x42df,
0x421b,
0x42f1,
0x42bf,
0x421d,
0x42f2,
0x4220,
0x4227,
0x4222,
0x42cb,
0x4231,
0x42b3,
0x4255,
0x4238,
0x4293,
0x42bd,
0x4234,
0x42b3,
0x422a,
0x4230,
0x42a2,
0x42c8,
0x4226,
0x4272,
0x4201,
0x421a,
0x4261,
0x4264,
0x4214,
0x4221,
0x4244,
0x4216,
0x4281,
0x42ab,
0x4220,
0x42a2,
0x4274,
0x422d,
0x4283,
0x4222,
0x4236,
0x42f3,
0x42bd,
0x4233,
0x4253,
0x4209,
0x422d,
0x42d2,
0x428c,
0x4221,
0x4281,
0x42a5,
0x4214,
0x4230,
0x42fd,
0x420d,
0x42a0,
0x42dc,
0x4210,
0x4241,
0x424c,
0x421b,
0x4222,
0x4228,
0x4229,
0x42f2,
0x42fb,
0x4235,
0x4273,
0x42b3,
0x4231,
0x4262,
0x42e1,
0x422a,
0x42c2,
0x4249,
0x421c,
0x4291,
0x424d,
0x420e,
0x4270,
0x42a2,
0x4207,
0x42d0,
0x4280,
0x420a,
0x4270,
0x42f2,
0x4215,
0x42d1,
0x42dc,
0x4225,
0x42e2,
0x42cc,
0x4233,
0x4243,
0x429c,
0x4230,
0x4292,
0x42cb,
0x4228,
0x42c2,
0x421f,
0x4219,
0x4251,
0x4214,
0x420a,
0x42a0,
0x4263,
0x4204,
0x4200,
0x4243,
0x4206,
0x42b0,
0x42b7,
0x4212,
0x4241,
0x42a8,
0x4223,
0x4242,
0x42b0,
0x4231,
0x42f3,
0x428e,
0x422f,
0x42b2,
0x42b5,
0x4227,
0x4201,
0x42fa,
0x4216,
0x42b0,
0x42e7,
0x4207,
0x42c0,
0x4236,
0x4201,
0x4230,
0x4216,
0x4203,
0x42e0,
0x428a,
0x420f,
0x4271,
0x427f,
0x4220,
0x42f2,
0x4295,
0x4230,
0x42a3,
0x4280,
0x422f,
0x42a2,
0x42b2,
0x4226,
0x4291,
0x42ef,
0x4215,
0x42e0,
0x42d8,
0x4206,
0x42d0,
0x4226,
0x4200,
0x4240,
0x4206,
0x4202,
0x42f0,
0x427a,
0x420e,
0x4271,
0x4270,
0x4220,
0x4242,
0x428f,
0x4230,
0x4293,
0x4283,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422f,
0x4242,
0x42ad,
0x4226,
0x4261,
0x42ec,
0x4215,
0x42c0,
0x42d6,
0x4206,
0x42c0,
0x4224,
0x4200,
0x4230,
0x4205,
0x4202,
0x42e0,
0x4279,
0x420e,
0x4261,
0x426d,
0x4220,
0x4222,
0x428d,
0x4230,
0x4253,
0x427e,
0x4230,
0x4202,
0x42bd,
0x4227,
0x42a2,
0x4203,
0x4217,
0x4260,
0x42f0,
0x4208,
0x4260,
0x4240,
0x4201,
0x42d0,
0x4220,
0x4204,
0x4280,
0x4294,
0x4210,
0x4211,
0x4287,
0x4221,
0x4292,
0x42a0,
0x4231,
0x4293,
0x4291,
0x4230,
0x4292,
0x42ce,
0x4229,
0x4242,
0x4226,
0x4219,
0x42d1,
0x421b,
0x420b,
0x4210,
0x426a,
0x4204,
0x4280,
0x424c,
0x4207,
0x4230,
0x42bf,
0x4212,
0x42b1,
0x42ae,
0x4223,
0x4292,
0x42b8,
0x4232,
0x42a3,
0x429c,
0x4231,
0x42e2,
0x42ee,
0x422b,
0x42e2,
0x425d,
0x421d,
0x42e1,
0x4261,
0x420f,
0x4270,
0x42b1,
0x4208,
0x42f0,
0x4291,
0x420b,
0x42a1,
0x4206,
0x4217,
0x4211,
0x42ed,
0x4226,
0x42f2,
0x42e3,
0x4234,
0x42f3,
0x42bb,
0x4233,
0x4263,
0x420e,
0x422e,
0x4262,
0x4296,
0x4222,
0x4251,
0x42b1,
0x4214,
0x42e1,
0x4209,
0x420e,
0x4270,
0x42ea,
0x4211,
0x4211,
0x425b,
0x421b,
0x42f2,
0x4233,
0x422a,
0x4273,
0x420b,
0x4237,
0x4223,
0x42d9,
0x4235,
0x4213,
0x4235,
0x4231,
0x4292,
0x42da,
0x4227,
0x42b2,
0x4214,
0x421b,
0x4291,
0x4278,
0x4215,
0x4271,
0x425a,
0x4218,
0x4201,
0x42c5,
0x4222,
0x4212,
0x4287,
0x422e,
0x42a3,
0x4241,
0x4239,
0x42e3,
0x42fc,
0x4234,
0x4213,
0x423a,
0x4233,
0x4223,
0x4201,
0x422b,
0x4282,
0x4264,
0x4221,
0x4271,
0x42df,
0x421c,
0x4211,
0x42c4,
0x421e,
0x4272,
0x4223,
0x4227,
0x4212,
0x42c4,
0x4231,
0x4243,
0x425f,
0x423a,
0x4293,
0x42f3,
0x4231,
0x42e3,
0x423a,
0x4235,
0x4263,
0x4231,
0x422f,
0x4232,
0x42b1,
0x4227,
0x4222,
0x4242,
0x4222,
0x4282,
0x422b,
0x4224,
0x4292,
0x427e,
0x422c,
0x4213,
0x4204,
0x4234,
0x42a3,
0x428a,
0x423a,
0x42a3,
0x42c9,
0x4234,
0x4233,
0x423b,
0x4237,
0x42b3,
0x4261,
0x4232,
0x42e2,
0x42fe,
0x422c,
0x42c2,
0x42a5,
0x4229,
0x4202,
0x4291,
0x422a,
0x42b2,
0x42d9,
0x4231,
0x4203,
0x4244,
0x4237,
0x42f3,
0x42b5,
0x423a,
0x42a3,
0x42f4,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x425f, //R Value
0x42a5,
0x42d8,
0x4261,
0x4235,
0x42e5,
0x4258,
0x42e5,
0x4229,
0x424c,
0x4244,
0x4273,
0x4244,
0x42e4,
0x424d,
0x4248,
0x4244,
0x42d9,
0x4254,
0x4245,
0x42af,
0x4260,
0x42c6,
0x423c,
0x425f,
0x4276,
0x4235,
0x425b,
0x4235,
0x425c,
0x4256,
0x4295,
0x4222,
0x424b,
0x4254,
0x423a,
0x423c,
0x4283,
0x4272,
0x4234,
0x4283,
0x424a,
0x4238,
0x4223,
0x42df,
0x4245,
0x4294,
0x42db,
0x4255,
0x4205,
0x429b,
0x4259,
0x4285,
0x42f8,
0x425b,
0x4265,
0x423a,
0x4250,
0x42e4,
0x42b0,
0x4242,
0x42b3,
0x429b,
0x4231,
0x42c2,
0x42c1,
0x4229,
0x4212,
0x4297,
0x422d,
0x4203,
0x4234,
0x423b,
0x42e4,
0x4257,
0x424e,
0x4245,
0x424b,
0x4259,
0x4236,
0x422b,
0x4259,
0x4265,
0x4203,
0x424b,
0x42f4,
0x4243,
0x4239,
0x42f2,
0x42f7,
0x4226,
0x4272,
0x4204,
0x421d,
0x4231,
0x42d9,
0x4221,
0x4232,
0x4280,
0x4231,
0x42a3,
0x42d2,
0x4247,
0x42d5,
0x4205,
0x4257,
0x42a6,
0x423e,
0x4257,
0x42f4,
0x42d4,
0x4247,
0x4293,
0x42e3,
0x4232,
0x4202,
0x4266,
0x421c,
0x42e1,
0x4268,
0x4213,
0x4261,
0x423c,
0x4217,
0x4291,
0x42eb,
0x4228,
0x42d3,
0x4255,
0x4242,
0x4204,
0x42c9,
0x4255,
0x4246,
0x422f,
0x4255,
0x4204,
0x4298,
0x4242,
0x42f3,
0x427c,
0x422a,
0x4281,
0x42e4,
0x4214,
0x4290,
0x42e4,
0x420b,
0x4240,
0x42b9,
0x420f,
0x4251,
0x4267,
0x4220,
0x42e2,
0x42e0,
0x423b,
0x42e4,
0x4282,
0x4252,
0x4236,
0x4214,
0x4253,
0x42d4,
0x4275,
0x423f,
0x42c3,
0x4237,
0x4225,
0x4241,
0x4289,
0x420e,
0x42e0,
0x428b,
0x4205,
0x42b0,
0x4260,
0x4209,
0x42c1,
0x420d,
0x421b,
0x4252,
0x428e,
0x4237,
0x4294,
0x4253,
0x4250,
0x4256,
0x4206,
0x4252,
0x4264,
0x4254,
0x423d,
0x4223,
0x4201,
0x4221,
0x4291,
0x424c,
0x420b,
0x4210,
0x424c,
0x4201,
0x42b0,
0x4221,
0x4205,
0x42e0,
0x42ce,
0x4217,
0x4262,
0x424f,
0x4234,
0x4224,
0x4229,
0x424e,
0x4255,
0x42f1,
0x4252,
0x4254,
0x424e,
0x423c,
0x4272,
0x42ed,
0x4220,
0x4231,
0x4236,
0x4209,
0x4280,
0x4233,
0x4200,
0x4250,
0x4208,
0x4204,
0x4250,
0x42b7,
0x4215,
0x42e2,
0x4237,
0x4232,
0x42f4,
0x421d,
0x424d,
0x42e5,
0x42ef,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4252,
0x4244,
0x424d,
0x423c,
0x4262,
0x42ec,
0x4220,
0x4231,
0x4234,
0x4209,
0x4280,
0x4232,
0x4200,
0x4240,
0x4207,
0x4204,
0x4240,
0x42b5,
0x4215,
0x42b2,
0x4235,
0x4232,
0x42d4,
0x421c,
0x424d,
0x42f5,
0x42f2,
0x4253,
0x4234,
0x4262,
0x423e,
0x4203,
0x420d,
0x4222,
0x4261,
0x4258,
0x420b,
0x42b0,
0x4256,
0x4202,
0x4260,
0x422c,
0x4206,
0x4270,
0x42d8,
0x4217,
0x42f2,
0x4258,
0x4234,
0x42d4,
0x4235,
0x424f,
0x4246,
0x4203,
0x4254,
0x4234,
0x4280,
0x4240,
0x42e3,
0x4247,
0x4226,
0x4271,
0x4299,
0x420f,
0x42b0,
0x4296,
0x4206,
0x4270,
0x426c,
0x420a,
0x4281,
0x4217,
0x421b,
0x42e2,
0x4296,
0x4238,
0x4224,
0x425c,
0x4250,
0x42c6,
0x420c,
0x4256,
0x4254,
0x42b1,
0x4244,
0x42d3,
0x429c,
0x422c,
0x4292,
0x4203,
0x4216,
0x4220,
0x42fd,
0x420c,
0x42c0,
0x42d1,
0x4210,
0x42b1,
0x427f,
0x4222,
0x4252,
0x42f5,
0x423d,
0x4244,
0x429c,
0x4253,
0x42d6,
0x422f,
0x4258,
0x4204,
0x42e3,
0x4249,
0x4273,
0x42fe,
0x4233,
0x42d2,
0x4282,
0x421e,
0x42a1,
0x4282,
0x4215,
0x4201,
0x4256,
0x4219,
0x4212,
0x4203,
0x422a,
0x4253,
0x4267,
0x4243,
0x4234,
0x42e0,
0x4257,
0x4246,
0x4258,
0x425a,
0x4255,
0x421d,
0x424e,
0x4254,
0x426e,
0x423c,
0x42b3,
0x4222,
0x4229,
0x4222,
0x422f,
0x4220,
0x4202,
0x4206,
0x4223,
0x42e2,
0x42ae,
0x4234,
0x4263,
0x42f5,
0x424a,
0x4205,
0x4236,
0x425b,
0x4216,
0x427d,
0x4258,
0x42e5,
0x422a,
0x4251,
0x4254,
0x42be,
0x4244,
0x4203,
0x42b8,
0x4233,
0x42c2,
0x42e5,
0x422b,
0x4292,
0x42c0,
0x422f,
0x4263,
0x4257,
0x423d,
0x42c4,
0x426c,
0x424f,
0x4295,
0x426e,
0x425c,
0x4296,
0x4275,
0x4255,
0x42c5,
0x4226,
0x4255,
0x4245,
0x4213,
0x424b,
0x4234,
0x4249,
0x423e,
0x4213,
0x4294,
0x4237,
0x4203,
0x4277,
0x423a,
0x4294,
0x4201,
0x4247,
0x4224,
0x42ea,
0x4255,
0x42e5,
0x42b8,
0x425c,
0x4276,
0x423a,
0x4258,
0x4275,
0x427c,
0x425e,
0x4245,
0x42b9,
0x4257,
0x4255,
0x4229,
0x424d,
0x4254,
0x4293,
0x4247,
0x4274,
0x427e,
0x424a,
0x42c4,
0x42fb,
0x4255,
0x4295,
0x42b7,
0x4261,
0x4236,
0x4252,
0x4261,
0x42f6,
0x4270,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4240, //B Value
0x4263,
0x42cb,
0x423e,
0x4263,
0x42cc,
0x4239,
0x4283,
0x426e,
0x4233,
0x42c3,
0x4212,
0x422f,
0x42f2,
0x42f7,
0x4230,
0x42d3,
0x422a,
0x4235,
0x42b3,
0x428a,
0x423b,
0x42d3,
0x42e6,
0x423b,
0x42e4,
0x4218,
0x4239,
0x4253,
0x4285,
0x4237,
0x4253,
0x4252,
0x4231,
0x42a2,
0x42e1,
0x422a,
0x4272,
0x427a,
0x4226,
0x4232,
0x425d,
0x4227,
0x4232,
0x429a,
0x422d,
0x4243,
0x4210,
0x4234,
0x42a3,
0x4279,
0x4239,
0x4223,
0x42ab,
0x4237,
0x42b3,
0x423f,
0x4230,
0x4242,
0x42d9,
0x4229,
0x42d2,
0x4255,
0x4221,
0x4221,
0x42e2,
0x421c,
0x4271,
0x42c3,
0x421d,
0x42a2,
0x420a,
0x4224,
0x42d2,
0x4296,
0x422d,
0x4263,
0x420b,
0x4236,
0x4253,
0x42bf,
0x4235,
0x4243,
0x420c,
0x422c,
0x4242,
0x4290,
0x4224,
0x4211,
0x42eb,
0x421a,
0x4201,
0x4266,
0x4214,
0x42a1,
0x4249,
0x4216,
0x4231,
0x4298,
0x421e,
0x4262,
0x423c,
0x4229,
0x4212,
0x42d5,
0x4233,
0x42a3,
0x42a0,
0x4233,
0x4272,
0x42e5,
0x4229,
0x4242,
0x4255,
0x421f,
0x4271,
0x4294,
0x4214,
0x4201,
0x4203,
0x420e,
0x4250,
0x42e4,
0x4210,
0x4231,
0x423d,
0x4219,
0x4211,
0x42f3,
0x4225,
0x4262,
0x42aa,
0x4231,
0x4293,
0x4288,
0x4231,
0x4242,
0x42b8,
0x4225,
0x42c2,
0x420e,
0x421a,
0x4281,
0x423e,
0x420e,
0x4260,
0x42a8,
0x4208,
0x42a0,
0x428b,
0x420a,
0x4290,
0x42e7,
0x4214,
0x4201,
0x42aa,
0x4221,
0x4282,
0x427a,
0x422f,
0x4233,
0x426c,
0x422f,
0x4242,
0x4299,
0x4223,
0x42d1,
0x42e3,
0x4217,
0x4201,
0x4204,
0x420a,
0x4280,
0x426a,
0x4204,
0x42c0,
0x424d,
0x4206,
0x42e0,
0x42af,
0x4210,
0x42a1,
0x427a,
0x421e,
0x42d2,
0x425c,
0x422d,
0x4293,
0x4255,
0x422e,
0x4252,
0x4280,
0x4221,
0x42b1,
0x42bb,
0x4214,
0x4260,
0x42d4,
0x4207,
0x4290,
0x423a,
0x4201,
0x42c0,
0x421e,
0x4203,
0x42f0,
0x4280,
0x420d,
0x42f1,
0x4250,
0x421c,
0x42c2,
0x423f,
0x422c,
0x4213,
0x4243,
0x422d,
0x4272,
0x4272,
0x4220,
0x42d1,
0x42a9,
0x4213,
0x4220,
0x42c2,
0x4206,
0x4250,
0x4226,
0x4200,
0x4290,
0x420b,
0x4202,
0x42d0,
0x426f,
0x420c,
0x42d1,
0x4241,
0x421b,
0x42f2,
0x4238,
0x422b,
0x42d3,
0x4242,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422d,
0x4212,
0x426c,
0x4220,
0x4281,
0x42a5,
0x4212,
0x42c0,
0x42be,
0x4206,
0x4200,
0x4222,
0x4200,
0x4220,
0x4205,
0x4202,
0x4290,
0x426c,
0x420c,
0x42a1,
0x423e,
0x421b,
0x42d2,
0x4235,
0x422b,
0x4293,
0x423d,
0x422e,
0x4202,
0x427d,
0x4221,
0x42b1,
0x42ba,
0x4214,
0x4250,
0x42d5,
0x4207,
0x42a0,
0x423b,
0x4201,
0x42b0,
0x421f,
0x4204,
0x4210,
0x4285,
0x420e,
0x4251,
0x4258,
0x421d,
0x4252,
0x424c,
0x422c,
0x42c3,
0x424c,
0x422e,
0x4272,
0x428d,
0x4223,
0x4231,
0x42db,
0x4216,
0x4280,
0x42fb,
0x420a,
0x4200,
0x4263,
0x4204,
0x4250,
0x4248,
0x4206,
0x42b0,
0x42ad,
0x4210,
0x42a1,
0x427c,
0x421f,
0x4242,
0x4262,
0x422e,
0x4213,
0x425f,
0x4230,
0x4292,
0x42b3,
0x4225,
0x42d2,
0x420d,
0x421a,
0x4231,
0x423b,
0x420e,
0x4210,
0x42a3,
0x4208,
0x4270,
0x428a,
0x420a,
0x42d0,
0x42f0,
0x4214,
0x42b1,
0x42b7,
0x4222,
0x4282,
0x428b,
0x4230,
0x4273,
0x4282,
0x4231,
0x42d2,
0x42d1,
0x4228,
0x4262,
0x4240,
0x421e,
0x4221,
0x4281,
0x4212,
0x42d0,
0x42f2,
0x420d,
0x4250,
0x42db,
0x420f,
0x42c1,
0x423b,
0x4219,
0x4231,
0x42f7,
0x4225,
0x42d2,
0x42b3,
0x4232,
0x42a3,
0x42a0,
0x4234,
0x4222,
0x42fd,
0x422b,
0x4282,
0x4282,
0x4223,
0x4201,
0x42da,
0x4218,
0x42c1,
0x4256,
0x4213,
0x42c1,
0x4240,
0x4216,
0x4211,
0x429c,
0x421e,
0x42c2,
0x4245,
0x4229,
0x42e2,
0x42ec,
0x4235,
0x42d3,
0x42ce,
0x4234,
0x4203,
0x4208,
0x422d,
0x4202,
0x42a7,
0x4226,
0x4242,
0x421e,
0x421d,
0x42c1,
0x42ad,
0x4219,
0x4261,
0x429c,
0x421b,
0x4271,
0x42eb,
0x4223,
0x4202,
0x4279,
0x422c,
0x4213,
0x4204,
0x4236,
0x4273,
0x42cb,
0x4233,
0x42c3,
0x422f,
0x4232,
0x4222,
0x42ff,
0x422c,
0x4242,
0x428b,
0x4225,
0x4222,
0x422a,
0x4221,
0x4262,
0x421d,
0x4223,
0x4262,
0x4263,
0x4229,
0x42f2,
0x42da,
0x4231,
0x42c3,
0x4259,
0x4238,
0x42d3,
0x42c1,
0x4238,
0x42e3,
0x4256,
0x4237,
0x4233,
0x4257,
0x4232,
0x4232,
0x42f7,
0x422c,
0x4282,
0x42a7,
0x4229,
0x4262,
0x429e,
0x422b,
0x4252,
0x42db,
0x4230,
0x42e3,
0x423b,
0x4237,
0x4283,
0x42ae,
0x423b,
0x4224,
0x4216,

0x0e00, //BURST_END

0x030a,
0x1902,	// Bus Switch

0x0315,	// Shading FPGA(Hi-352)
0x1001,	// LSC ON
0x1106, //gap y enb, gap x enb for 720p

0x2780,	// LSC G
0x2880,	// LSC B
0x2980,	// LSC R

0x308a, //gap x for 720p
0x3168, //gap y for 720p
0x366c, //lsc_win_x for 720p
0x3764, //lsc_win_y for 720p

///////////////////////////////////
// 10 Page Saturation (H/W)
///////////////////////////////////
0x0310,
0x1001,
0x1200, //Y OFS Disable
0x1700, //20121127 CSP option
0x1800, //20121127 CSP option
0x2004, //16_235 range scale down on
0x6003, //Sat ENB Transfer Function     //Transfunction on

///////////////////////////////////
// 11 Page D-LPF (H/W)
///////////////////////////////////
0x0311, //11 page
0x100f, //D-LPF ENB //DPC marker

0x1228, //20121120 character long line detection th
0x132c, //20121120 character short line detection th

0x1d12, // ORG_STD Ctrl
0x1e00,// 20130410_STD 03 -> 00
0x2178, // Color STD Gain
//Bayer Sharpness Gain Ctrl
0xb722, //SpColor_gain1
0xb822, //SpColor_gain2
0xb921, //SpColor_gain3
0xba1e, //SpColor_gain4
0xbb1c, //SpColor_gain5
0xbc1a, //SpColor_gain6

0xf2ff, //pga_dark1_hi
0xf3fc, //pga_dark_lo
///////////////////////////////////
// 12 Page DPC,GBGR (H/W)//////////
///////////////////////////////////
0x0312, //12 page
0x1057, //DPC ON
0x1230,
0x2b08, //white_th
0x2c08, //middle_h_th
0x2d08, //middle_l_th
0x2e06, //dark_th
0x2f10, //20121127 _DPC TH
0x3010, //20121127 _DPC TH
0x3110, //20121127 _DPC TH
0x3210, //20121127 _DPC TH
0x4188, //GBGR Cut off //46

///////////////////////////////////
// 12 Page CI-LPF (H/W)////////////
///////////////////////////////////

0xEF01, //Interpol Color filter On/Off

///////////////////////////////////
// 13 Page YC-2D_Y-NR (H/W)/////////
///////////////////////////////////
0x0313,

0x802d, //YC-2D_C-NR ENB, C-Filter DC option on B[7] //DC on 8b //DC off 2d
0x81ff, // add 20121210
0x82fe, // add 20121210

0x8532,
0x8608, // add 20121210

//==========================================================================
// C-Filter PS Reducing (Mask-Size Adjustment)

0x87ff, //C-mask near STD TH
0x88ff, //C-mask middle STD TH
0x89ff, //C-mask far STD TH

0x8a86, //color STD

0x970f, // C-filter Lum gain 1
0x980e,
0x990d,
0x9a0c,
0x9b0b,
0x9c0a,
0x9d09,
0x9e08,

0xa70f, // C-filter STD gain 1
0xa80e,
0xa90d,
0xaa0c,
0xab0b,
0xac0a,
0xad09,
0xae08,

//==========================================================================

///////////////////////////////////
// 14 Page YC-2D_Sharpness(H/W)
///////////////////////////////////
0x0314,
0x7720, //Yc2d_ee_color_gain1
0x7820, //Yc2d_ee_color_gain2
0x7920, //Yc2d_ee_color_gain3
0x7a1c, //Yc2d_ee_color_gain4
0x7b1b, //Yc2d_ee_color_gain5
0x7c1a, //Yc2d_ee_color_gain6
0x7d19, //Yc2d_ee_color_gain7
0x7e18, //Yc2d_ee_color_gain8

0xc000, //Yc2d_ee_lclip_gain_n1
0xc100, //Yc2d_ee_lclip_gain_n2
0xc200, //Yc2d_ee_lclip_gain_n3
0xc300, //Yc2d_ee_lclip_gain_n4
0xc401, //Yc2d_ee_lclip_gain_n5

///////////////////////////////////////////////////////////////////////////////
// 16 Page CMC / AWB Gain
///////////////////////////////////////////////////////////////////////////////

0x0316,
0x107f,	// CMC ENB	3f(spGrap off) 7f(spGrap on)
0x2052,// PS / LN

0xa003,	// WB gain on
0xa205,	// R_h (12bit = 8bit * 16)
0xa380,	// R_l
0xa407,	// B_h (12bit = 8bit * 16)
0xa580,	// B_l

0xD001,	//Bayer gain enable
///////////////////////////////////////////////////////////////////////////////
// 17 Page Gamma
///////////////////////////////////////////////////////////////////////////////

0x0317,
0x1007,	// GMA ENB //PS On
0x1252,// old:43 new:65

///////////////////////////////////////////////////////////////////////////////
// 18 Page MCMC
///////////////////////////////////////////////////////////////////////////////

0x0318,	// Page 18
0x1001,	// mcmc_ctl1
0x117f,	// mcmc_ctl2
0x5310,	// mcmc_ctl3

0x561b,	// mcmc_glb_sat_lvl_sp1
0x5739,	// mcmc_glb_sat_lvl_sp2
0x585a,	// mcmc_glb_sat_lvl_sp3
0x5980,	// mcmc_glb_sat_lvl_sp4
0x5aa6,	// mcmc_glb_sat_lvl_sp5
0x5bc1,	// mcmc_glb_sat_lvl_sp6
0x5ce8,	// mcmc_glb_sat_lvl_sp7
0x5d38,	// mcmc_glb_sat_gain_sp1
0x5e3a,	// mcmc_glb_sat_gain_sp2
0x5f3c,	// mcmc_glb_sat_gain_sp3
0x603f,	// mcmc_glb_sat_gain_sp4
0x613f,	// mcmc_glb_sat_gain_sp5
0x623f,	// mcmc_glb_sat_gain_sp6
0x633f,	// mcmc_glb_sat_gain_sp7
0x643f,	// mcmc_glb_sat_gain_sp8
0x6500,	// mcmc_std_ctl1
0x6600,	// mcmc_std_ctl2
0x6700,	// mcmc_std_ctl3

0x6cff,	// mcmc_lum_ctl1 sat hue offset
0x6d3f,	// mcmc_lum_ctl2 gain
0x6e00,	// mcmc_lum_ctl3 hue
0x6f00,	// mcmc_lum_ctl4 rgb offset
0x7000,	// mcmc_lum_ctl5 rgb scale

0xa100,
0xa201,	//star gain enb

///////////////////////////////////////////////////////////////////////////////
// 1A Page_RGB Y-NR, Y-Sharpness
///////////////////////////////////////////////////////////////////////////////

0x031a,
0x309f,	// RGB-Sharpness ENB // Flat-region RGB ENB B[1] //Green dis [7] On

0x8D20, //RGB-Color_Gain1
0x8E20, //RGB-Color_Gain2
0x8F20, //RGB-Color_Gain3
0x9020, //RGB-Color_Gain4
0x9120, //RGB-Color_Gain5

///////////////////////////////////////////////////////////////////////////////
// 20 Page (FZY)
///////////////////////////////////////////////////////////////////////////////

0x0320, //Page 20

0x10bd, //50hz bd, 60hz ad
0x12e0, //Dgain off 20 //Dgain enb_2x e0

0x3600, //EXP Unit 
0x3707,
0x383a,

0x51ff, //PGA Max
0x5220, //PGA Min x0.9

0x61FF,	// max ramp gain
0x6200,	// min ramp gain
0x60E0,	// ramp gain

0x7170, //DG MAX //D gain 1x = 80 = x 1 // D gain 2x = 40 = x 1
0x7240, //DG MIN

0x803a, //Y Target

///////////////////////////////////////////////////////////////////////////////
// 23 Page (AFC)
///////////////////////////////////////////////////////////////////////////////

0x0323, //Page 23
0x147A, //Flicker Line 100
0x1566, //Flicker Line 120
0x1001, //Frame Interval

///////////////////////////////////////////////////////////////////////////////
// 2A Page (SSD)
///////////////////////////////////////////////////////////////////////////////

0x032A,
0x1011,
0x1101,
0x1502,	//720p mode on
0x1650,	//SSD B gain int gain 1.5x
0x730C,	//start x in 720p
0x7408,	//start y in 720p
0x7564,	//width   in 720p
0x764A,	//height  in 720p

///////////////////////////////////////////////////////////////////////////////
//
// F/W setting start
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// C0 Page (SYSTEM)
///////////////////////////////////////////////////////////////////////////////
//OPCLK Setting
//43Mhz = 29020C0
0x03C0,
0x1402,
0x1590,
0x1620,
0x17C0,

///////////////////////////////////////////////////////////////////////////////
// C6 Page (SSD Y weight)
///////////////////////////////////////////////////////////////////////////////
//SSD_Matrix
0x03c6,
0x9E11,	//1 Line
0x9F11,
0xA011,
0xA111,
0xA211,
0xA311,

0xA411,	//2 Line
0xA511,
0xA611,
0xA711,
0xA811,
0xA911,

0xAA11,//3 Line
0xAB12,
0xAC22,
0xAD22,
0xAE21,
0xAF11,

0xB011,//4 Line
0xB112,
0xB222,
0xB322,
0xB421,
0xB511,

0xB611,//5 Line
0xB712,
0xB822,
0xB922,
0xBA21,
0xBB11,

0xBC11,//6 Line
0xBD12,
0xBE22,
0xBF22,
0xC021,
0xC111,

0xC211,//7 Line
0xC312,
0xC422,
0xC522,
0xC621,
0xC711,

0xC811,//8 Line
0xC911,
0xCA11,
0xCB11,
0xCC11,
0xCD11,

0xCE11,//9 Line
0xCF11,
0xD011,
0xD111,
0xD211,
0xD311,

///////////////////////////////////////////////////////////////////////////////
// C7 Page (AE)
///////////////////////////////////////////////////////////////////////////////
//Shutter Setting
0x03c7,
0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10
0x1203, // Slow AE
0x15c0, // SSD Patch Weight Y Mean On

0x3618, // Max 24fps
0x3718, // Max 24fps

0x3d22, // YTh Lock, Unlock0

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0x7082, // 50hz 82, 60hz 02
0xff01,
0x4c00, //SW ExpMin = 3700
0x4d00,
0x4e0e,
0x4f74,

0x4400, //Start ExpTime 120fps
0x4505,	
0x4672,	
0x47ba,	
0xa748, //Start ExpTime 120fps flaot
0xa8ae,	
0xa957,	
0xaa40,	

0x0320, //20 page
0x2000, //Start ExpTime 120fps
0x2105,
0x2272,
0x23ba,
0x2800, //HW ExpMin = 3700
0x290e,
0x2a74,

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90

///////////////////////////////////////////////////////////////////////////////
// D9 Page (Capture/Preview)
///////////////////////////////////////////////////////////////////////////////
0x03d9,
0x8c20,	//en_ramp_gain_auto

///////////////////////////////////////////////////////////////////////////////
// C8 ~ CC Page (AWB)
///////////////////////////////////////////////////////////////////////////////
0x03C8,
0x0e01, // burst start

0x10f6,
0x11c3,
0x12e0,
0x131a, //bCtl4
0x149f,
0x15c4,
0x1600,
0x1734,
0x1855,
0x1966,
0x1a66,
0x1b88,
0x1c88,
0x1d88,
0x1e88,
0x1faa,
0x20aa,
0x2100,
0x2201,
0x231e,
0x24ff,
0x251e,
0x2651, //init awb speed
0x27ff,
0x28f0,
0x2901,
0x2a00,
0x2b1e,
0x2c04,
0x2d0c,
0x2e1e,
0x2f00, //dwOutdoorCondInTh
0x3000, //dwOutdoorCondInTh_n01
0x3121, //dwOutdoorCondInTh_n02
0x3234, //dwOutdoorCondInTh_n03
0x3300, //dwOutdoorCondOutTh
0x3400, //dwOutdoorCondOutTh_n01
0x3527, //dwOutdoorCondOutTh_n02
0x3610, //dwOutdoorCondOutTh_n03
0x3700, //dwEvTh
0x3800, //dwEvTh_n01
0x3904,//dwEvTh_n02 //840fps
0x3aa6,//dwEvTh_n03
0x3b00, //dwEvTh_a01
0x3c00, //dwEvTh_a01_n01
0x3d08,//dwEvTh_a01_n02
0x3e23,//dwEvTh_a01_n03 //480fps
0x3f00, //dwEvTh_a02
0x4000, //dwEvTh_a02_n01
0x4120,//dwEvTh_a02_n02
0x428d,//dwEvTh_a02_n03 //120fps
0x4300, //dwEvTh_a03
0x4400, //dwEvTh_a03_n01
0x4561, //dwEvTh_a03_n02
0x46a8,//dwEvTh_a03_n03 //40fps
0x4700, //dwEvTh_a04
0x4804, //dwEvTh_a04_n01
0x49c4, //dwEvTh_a04_n02
0x4ab4, //dwEvTh_a04_n03
0x4b00, //dwEvTh_a05
0x4c0b, //dwEvTh_a05_n01
0x4d71, //dwEvTh_a05_n02
0x4eb0, //dwEvTh_a05_n03
0x4f96,//aAglMaxMinLmt
0x5058,//aAglMaxMinLmt_a01
0x519C,//aAglMaxMinLmt_a02
0x5260, //aAglMaxMinLmt_a03
0x53a0, //aAglMaxMinLmt_a04
0x5420, //aAglMaxMinLmt_a05
0x55a0, //aAglMaxMinLmt_a06
0x5638,//aAglMaxMinLmt_a07
0x5770,//aTgtWhtRgnBgMaxMinLmt      out2 max
0x585e,//aTgtWhtRgnBgMaxMinLmt_a01  out2 min
0x5974,//aTgtWhtRgnBgMaxMinLmt_a02  out1 max
0x5a5c,//aTgtWhtRgnBgMaxMinLmt_a03  out1 min
0x5b98,//aTgtWhtRgnBgMaxMinLmt_a04  in max
0x5c20,//aTgtWhtRgnBgMaxMinLmt_a05  in min
0x5d98,//aTgtWhtRgnBgMaxMinLmt_a06  dark max
0x5e42,//aTgtWhtRgnBgMaxMinLmt_a07  dark min

0x5f10, //bTgtWhtRgnBgStep
0x600a,//BpOption distance weight  def : 0a -> 04 -> 02

0x611e,
0x6234,
0x6380,
0x6410,
0x6501,
0x6604,
0x670e,
0x6800,
0x6932,
0x6a00,
0x6ba2,
0x6c02,
0x6d00,
0x6e00,
0x6f00,
0x7000,
0x7100,
0x7200,
0x7300,
0x7400,
0x7500,
0x7655,
0x7755,
0x7855,
0x7955,
0x7a55,
0x7b55,
0x7c55,
0x7d55,
0x7e55,
0x7f55,
0x8055,
0x8155,
0x8255,
0x8355,
0x8455,
0x8555,
0x8655,
0x8755,
0x8855,
0x8955,
0x8a55,
0x8b55,
0x8c55,
0x8d55,
0x8e55,
0x8f55,
0x9055,
0x9100,
0x9200,

0x9300, //Indoor_wRgIntOfs
0x94a0, //Indoor_wRgIntOfs_n01
0x9500, //Indoor_wBgIntOfs
0x96c0, //Indoor_wBgIntOfs_n01
0x9610, //Indoor_bRgStep
0x9710, //Indoor_bBgStep
0x9926, //Indoor_aTgtWhtRgnBg
0x9a29, //Indoor_aTgtWhtRgnBg_a01
0x9b2c, //Indoor_aTgtWhtRgnBg_a02
0x9c38, //Indoor_aTgtWhtRgnBg_a03
0x9d43, //Indoor_aTgtWhtRgnBg_a04
0x9e4d, //Indoor_aTgtWhtRgnBg_a05
0x9f59, //Indoor_aTgtWhtRgnBg_a06
0xa064, //Indoor_aTgtWhtRgnBg_a07
0xa16f, //Indoor_aTgtWhtRgnBg_a08
0xa27b, //Indoor_aTgtWhtRgnBg_a09
0xa38e,//Indoor_aTgtWhtRgnBg_a10
0xa4a0,//Indoor_aTgtWhtRgnRgLtLmt
0xa598,//Indoor_aTgtWhtRgnRgLtLmt_a01
0xa682,//Indoor_aTgtWhtRgnRgLtLmt_a02
0xa76d,//Indoor_aTgtWhtRgnRgLtLmt_a03
0xa860,//Indoor_aTgtWhtRgnRgLtLmt_a04
0xa956,//Indoor_aTgtWhtRgnRgLtLmt_a05
0xaa4c,//Indoor_aTgtWhtRgnRgLtLmt_a06
0xab45, //Indoor_aTgtWhtRgnRgLtLmt_a07
0xac40, //Indoor_aTgtWhtRgnRgLtLmt_a08
0xad3c, //Indoor_aTgtWhtRgnRgLtLmt_a09
0xae39,//Indoor_aTgtWhtRgnRgLtLmt_a10
0xafaa, //Indoor_aTgtWhtRgnRgRtLmt
0xb0a9, //Indoor_aTgtWhtRgnRgRtLmt_a01
0xb1a8,//Indoor_aTgtWhtRgnRgRtLmt_a02
0xb2a2,//Indoor_aTgtWhtRgnRgRtLmt_a03
0xb395,//Indoor_aTgtWhtRgnRgRtLmt_a04
0xb488,//Indoor_aTgtWhtRgnRgRtLmt_a05
0xb578,//Indoor_aTgtWhtRgnRgRtLmt_a06
0xb665,//Indoor_aTgtWhtRgnRgRtLmt_a07
0xb75a,//Indoor_aTgtWhtRgnRgRtLmt_a08
0xb850,//Indoor_aTgtWhtRgnRgRtLmt_a09
0xb94a,//Indoor_aTgtWhtRgnRgRtLmt_a10
0xba1b, //Indoor_aOptWhtRgnBg
0xbb1d, //Indoor_aOptWhtRgnBg_a01
0xbc1f, //Indoor_aOptWhtRgnBg_a02
0xbd2a, //Indoor_aOptWhtRgnBg_a03
0xbe38, //Indoor_aOptWhtRgnBg_a04
0xbf47, //Indoor_aOptWhtRgnBg_a05
0xc054, //Indoor_aOptWhtRgnBg_a06
0xc161, //Indoor_aOptWhtRgnBg_a07
0xc272, //Indoor_aOptWhtRgnBg_a08
0xc382, //Indoor_aOptWhtRgnBg_a09
0xc49a,//Indoor_aOptWhtRgnBg_a10
0xc5ad, //Indoor_aOptWhtRgnRgLtLmt
0xc698,//Indoor_aOptWhtRgnRgLtLmt_a01
0xc78a,//Indoor_aOptWhtRgnRgLtLmt_a02
0xc874,//Indoor_aOptWhtRgnRgLtLmt_a03
0xc95f,//Indoor_aOptWhtRgnRgLtLmt_a04
0xca50,//Indoor_aOptWhtRgnRgLtLmt_a05
0xcb46,//Indoor_aOptWhtRgnRgLtLmt_a06
0xcc40,//Indoor_aOptWhtRgnRgLtLmt_a07
0xcd39, //Indoor_aOptWhtRgnRgLtLmt_a08
0xce35, //Indoor_aOptWhtRgnRgLtLmt_a09
0xcf33, //Indoor_aOptWhtRgnRgLtLmt_a10
0xd0ba,//Indoor_aOptWhtRgnRgRtLmt
0xd1b9,//Indoor_aOptWhtRgnRgRtLmt_a01
0xd2b8,//Indoor_aOptWhtRgnRgRtLmt_a02
0xd3b5, //Indoor_aOptWhtRgnRgRtLmt_a03
0xd4ae, //Indoor_aOptWhtRgnRgRtLmt_a04
0xd5a1, //Indoor_aOptWhtRgnRgRtLmt_a05
0xd68c, //Indoor_aOptWhtRgnRgRtLmt_a06
0xd778,//Indoor_aOptWhtRgnRgRtLmt_a07
0xd860,//Indoor_aOptWhtRgnRgRtLmt_a08
0xd954,//Indoor_aOptWhtRgnRgRtLmt_a09
0xda4d,//Indoor_aOptWhtRgnRgRtLmt_a10

0xdb36, //Indoor_aCtmpWgtWdhTh
0xdc40, //Indoor_aCtmpWgtWdhTh_a01
0xdd4c, //Indoor_aCtmpWgtWdhTh_a02
0xde5c, //Indoor_aCtmpWgtWdhTh_a03
0xdf6e, //Indoor_aCtmpWgtWdhTh_a04
0xe07f, //Indoor_aCtmpWgtWdhTh_a05
0xe1a4, //Indoor_aCtmpWgtWdhTh_a06
0xe227, //Indoor_aCtmpWgtHgtTh
0xe332, //Indoor_aCtmpWgtHgtTh_a01
0xe43c, //Indoor_aCtmpWgtHgtTh_a02
0xe548,//Indoor_aCtmpWgtHgtTh_a03
0xe65c,//Indoor_aCtmpWgtHgtTh_a04
0xe770,//Indoor_aCtmpWgtHgtTh_a05
0xe87c,//Indoor_aCtmpWgtHgtTh_a06
0xe986,//Indoor_aCtmpWgtHgtTh_a07
0xea90, //Indoor_aCtmpWgtHgtTh_a08
0xeb11, //Indoor_aCtmpWgt
0xec11, //Indoor_aCtmpWgt_a01
0xed12, //Indoor_aCtmpWgt_a02
0xee11, //Indoor_aCtmpWgt_a03
0xef11, //Indoor_aCtmpWgt_a04
0xf033, //Indoor_aCtmpWgt_a05
0xf111, //Indoor_aCtmpWgt_a06
0xf214, //Indoor_aCtmpWgt_a07
0xf343, //Indoor_aCtmpWgt_a08
0xf411, //Indoor_aCtmpWgt_a09
0xf555, //Indoor_aCtmpWgt_a10
0xf641, //Indoor_aCtmpWgt_a11
0xf716, //Indoor_aCtmpWgt_a12
0xf865, //Indoor_aCtmpWgt_a13
0xf911, //Indoor_aCtmpWgt_a14
0xfa48, //Indoor_aCtmpWgt_a15
0xfb61, //Indoor_aCtmpWgt_a16
0xfc11, //Indoor_aCtmpWgt_a17
0xfd46, //Indoor_aCtmpWgt_a18
0x0e00, // burst end

0x03c9, //c9 page
0x0e01, // burst start

0x1011, //Indoor_aCtmpWgt_a19
0x1111, //Indoor_aCtmpWgt_a20
0x1223, //Indoor_aCtmpWgt_a21
0x1311, //Indoor_aCtmpWgt_a22
0x1411, //Indoor_aCtmpWgt_a23
0x1510, //Indoor_aCtmpWgt_a24

0x1611,//Indoor_aYlvlWgt
0x1711,//Indoor_aYlvlWgt_a01
0x1811,//Indoor_aYlvlWgt_a02
0x1911,//Indoor_aYlvlWgt_a03
0x1a11,//Indoor_aYlvlWgt_a04
0x1b11,//Indoor_aYlvlWgt_a05
0x1c11,//Indoor_aYlvlWgt_a06
0x1d11,//Indoor_aYlvlWgt_a07
0x1e11,//Indoor_aYlvlWgt_a08
0x1f11,//Indoor_aYlvlWgt_a09
0x2011,//Indoor_aYlvlWgt_a10
0x2122,//Indoor_aYlvlWgt_a11
0x2222,//Indoor_aYlvlWgt_a12
0x2334,//Indoor_aYlvlWgt_a13
0x2432,//Indoor_aYlvlWgt_a14
0x2521,//Indoor_aYlvlWgt_a15

0x2633, //Indoor_aTgtAngle
0x273f, //Indoor_aTgtAngle_a01
0x2843, //Indoor_aTgtAngle_a02
0x2950,//Indoor_aTgtAngle_a03
0x2a68,//Indoor_aTgtAngle_a04
0x2b28,//Indoor_aRgTgtOfs
0x2c14,//Indoor_aRgTgtOfs_a01
0x2d02,//Indoor_aRgTgtOfs_a02
0x2e00,//Indoor_aRgTgtOfs_a03
0x2f08,//Indoor_aRgTgtOfs_a04
0x30d4,//Indoor_aBgTgtOfs
0x31ca,//Indoor_aBgTgtOfs_a01
0x32aa,//Indoor_aBgTgtOfs_a02
0x33a4,//Indoor_aBgTgtOfs_a03
0x3488,//Indoor_aBgTgtOfs_a04

0x3500, //bRgDefTgt //indoor
0x3600, //bBgDefTgt //indoor

0x3720,//Indoor_aWhtPtTrcAglOfs
0x381e,//Indoor_aWhtPtTrcAglOfs_a01
0x391c,//Indoor_aWhtPtTrcAglOfs_a02
0x3a1a,//Indoor_aWhtPtTrcAglOfs_a03
0x3b18,//Indoor_aWhtPtTrcAglOfs_a04
0x3c16,//Indoor_aWhtPtTrcAglOfs_a05
0x3d14,//Indoor_aWhtPtTrcAglOfs_a06
0x3e14,//Indoor_aWhtPtTrcAglOfs_a07
0x3f13,//Indoor_aWhtPtTrcAglOfs_a08
0x4012,//Indoor_aWhtPtTrcAglOfs_a09
0x4104,//Indoor_bWhtPtTrcCnt
0x4214,//Indoor_aRtoDiffThNrBp
0x433c,//Indoor_aRtoDiffThNrBp_a01
0x4428,//Indoor_aAglDiffThTrWhtPt
0x4550,//Indoor_aAglDiffThTrWhtPt_a01
0x46aa,//Indoor_bWgtRatioTh1
0x47a0,//Indoor_bWgtRatioTh2
0x4844,//Indoor_bWgtOfsTh1
0x4940,//Indoor_bWgtOfsTh2
0x4a5a,//Indoor_bWhtPtCorAglMin
0x4b70,//Indoor_bWhtPtCorAglMax
0x4c04,//Indoor_bYlvlMin
0x4df8,//Indoor_bYlvlMax
0x4e28,//Indoor_bPxlWgtLmtLoTh
0x4f78,//Indoor_bPxlWgtLmtHiTh
0x5000,//Indoor_SplBldWgt_1
0x5100,//Indoor_SplBldWgt_2
0x5264,//Indoor_SplBldWgt_3
0x5360,//Indoor_TgtOff_StdHiTh
0x5430,//Indoor_TgtOff_StdLoTh
0x5505,//Indoor_wInitRg
0x56d0,//Indoor_wInitRg_n01
0x5706,//Indoor_wInitBg
0x5840,//Indoor_wInitBg_n01

0x5902, //Indoor_aRatioBox
0x5aee, //Indoor_aRatioBox_a01
0x5b06, //Indoor_aRatioBox_a02
0x5c40, //Indoor_aRatioBox_a03
0x5d08, //Indoor_aRatioBox_a04
0x5e34, //Indoor_aRatioBox_a05
0x5f0b,//Indoor_aRatioBox_a06
0x6054,//Indoor_aRatioBox_a07
0x6103, //Indoor_aRatioBox_a08
0x6252, //Indoor_aRatioBox_a09
0x6307, //Indoor_aRatioBox_a10
0x64d0, //Indoor_aRatioBox_a11
0x6506, //Indoor_aRatioBox_a12
0x66a4, //Indoor_aRatioBox_a13
0x6708, //Indoor_aRatioBox_a14
0x68fc, //Indoor_aRatioBox_a15
0x6903, //Indoor_aRatioBox_a16
0x6ae8, //Indoor_aRatioBox_a17
0x6b0a, //Indoor_aRatioBox_a18
0x6c8c, //Indoor_aRatioBox_a19
0x6d04, //Indoor_aRatioBox_a20
0x6eb0, //Indoor_aRatioBox_a21
0x6f07, //Indoor_aRatioBox_a22
0x706c, //Indoor_aRatioBox_a23
0x7104, //Indoor_aRatioBox_a24
0x72e2, //Indoor_aRatioBox_a25
0x730c, //Indoor_aRatioBox_a26
0x741c, //Indoor_aRatioBox_a27
0x7503, //Indoor_aRatioBox_a28
0x7684, //Indoor_aRatioBox_a29
0x7705, //Indoor_aRatioBox_a30
0x78dc, //Indoor_aRatioBox_a31
0x7905, //Indoor_aRatioBox_a32
0x7adc, //Indoor_aRatioBox_a33
0x7b0c, //Indoor_aRatioBox_a34
0x7ce4, //Indoor_aRatioBox_a35
0x7d01, //Indoor_aRatioBox_a36
0x7ef4, //Indoor_aRatioBox_a37
0x7f05, //Indoor_aRatioBox_a38
0x8000, //Indoor_aRatioBox_a39

0x8100, //Outdoor_wRgIntOfs
0x824a,//Outdoor_wRgIntOfs_n01
0x8301, //Outdoor_wBgIntOfs
0x8400, //Outdoor_wBgIntOfs_n01
0x8510, //Outdoor_bRgStep
0x8610, //Outdoor_bBgStep
0x8751, //Outdoor_aTgtWhtRgnBg
0x8852, //Outdoor_aTgtWhtRgnBg_a01
0x8953, //Outdoor_aTgtWhtRgnBg_a02
0x8a57, //Outdoor_aTgtWhtRgnBg_a03
0x8b5e, //Outdoor_aTgtWhtRgnBg_a04
0x8c64, //Outdoor_aTgtWhtRgnBg_a05
0x8d6A, //Outdoor_aTgtWhtRgnBg_a06
0x8e6F, //Outdoor_aTgtWhtRgnBg_a07
0x8f75, //Outdoor_aTgtWhtRgnBg_a08
0x907c, //Outdoor_aTgtWhtRgnBg_a09
0x9184, //Outdoor_aTgtWhtRgnBg_a10
0x925D, //Outdoor_aTgtWhtRgnRgLtLmt
0x9357, //Outdoor_aTgtWhtRgnRgLtLmt_a01
0x9451, //Outdoor_aTgtWhtRgnRgLtLmt_a02
0x9550, //Outdoor_aTgtWhtRgnRgLtLmt_a03
0x964e,//Outdoor_aTgtWhtRgnRgLtLmt_a04
0x974c,//Outdoor_aTgtWhtRgnRgLtLmt_a05
0x984b,//Outdoor_aTgtWhtRgnRgLtLmt_a06
0x9949,//Outdoor_aTgtWhtRgnRgLtLmt_a07
0x9a47, //Outdoor_aTgtWhtRgnRgLtLmt_a08
0x9b46,//Outdoor_aTgtWhtRgnRgLtLmt_a09
0x9c45,//Outdoor_aTgtWhtRgnRgLtLmt_a10
0x9d64, //Outdoor_aTgtWhtRgnRgRtLmt
0x9e63, //Outdoor_aTgtWhtRgnRgRtLmt_a01
0x9f62, //Outdoor_aTgtWhtRgnRgRtLmt_a02
0xa062, //Outdoor_aTgtWhtRgnRgRtLmt_a03
0xa161, //Outdoor_aTgtWhtRgnRgRtLmt_a04
0xa260, //Outdoor_aTgtWhtRgnRgRtLmt_a05
0xa35e, //Outdoor_aTgtWhtRgnRgRtLmt_a06
0xa45d,//Outdoor_aTgtWhtRgnRgRtLmt_a07
0xa55c,//Outdoor_aTgtWhtRgnRgRtLmt_a08
0xa65a, //Outdoor_aTgtWhtRgnRgRtLmt_a09
0xa757,//Outdoor_aTgtWhtRgnRgRtLmt_a10
0xa840, //Outdoor_aOptWhtRgnBg
0xa945, //Outdoor_aOptWhtRgnBg_a01
0xaa4b, //Outdoor_aOptWhtRgnBg_a02
0xab54, //Outdoor_aOptWhtRgnBg_a03
0xac60, //Outdoor_aOptWhtRgnBg_a04
0xad6c, //Outdoor_aOptWhtRgnBg_a05
0xae76, //Outdoor_aOptWhtRgnBg_a06
0xaf7f, //Outdoor_aOptWhtRgnBg_a07
0xb08c, //Outdoor_aOptWhtRgnBg_a08
0xb195, //Outdoor_aOptWhtRgnBg_a09
0xb2a0, //Outdoor_aOptWhtRgnBg_a10
0xb36a, //Outdoor_aOptWhtRgnRgLtLmt
0xb45b, //Outdoor_aOptWhtRgnRgLtLmt_a01
0xb553, //Outdoor_aOptWhtRgnRgLtLmt_a02
0xb64c, //Outdoor_aOptWhtRgnRgLtLmt_a03
0xb746, //Outdoor_aOptWhtRgnRgLtLmt_a04
0xb842, //Outdoor_aOptWhtRgnRgLtLmt_a05
0xb93e, //Outdoor_aOptWhtRgnRgLtLmt_a06
0xba3c, //Outdoor_aOptWhtRgnRgLtLmt_a07
0xbb3a, //Outdoor_aOptWhtRgnRgLtLmt_a08
0xbc39, //Outdoor_aOptWhtRgnRgLtLmt_a09
0xbd37, //Outdoor_aOptWhtRgnRgLtLmt_a10
0xbe7d, //Outdoor_aOptWhtRgnRgRtLmt
0xbf7c, //Outdoor_aOptWhtRgnRgRtLmt_a01
0xc079, //Outdoor_aOptWhtRgnRgRtLmt_a02
0xc176, //Outdoor_aOptWhtRgnRgRtLmt_a03
0xc26f, //Outdoor_aOptWhtRgnRgRtLmt_a04
0xc36a, //Outdoor_aOptWhtRgnRgRtLmt_a05
0xc466, //Outdoor_aOptWhtRgnRgRtLmt_a06
0xc563, //Outdoor_aOptWhtRgnRgRtLmt_a07
0xc65B, //Outdoor_aOptWhtRgnRgRtLmt_a08
0xc754, //Outdoor_aOptWhtRgnRgRtLmt_a09
0xc84a, //Outdoor_aOptWhtRgnRgRtLmt_a10

0xc942, //Outdoor_aCtmpWgtWdhTh
0xca4c,//Outdoor_aCtmpWgtWdhTh_a01
0xcb54,//Outdoor_aCtmpWgtWdhTh_a02
0xcc5c,//Outdoor_aCtmpWgtWdhTh_a03
0xcd64,//Outdoor_aCtmpWgtWdhTh_a04
0xce6c,//Outdoor_aCtmpWgtWdhTh_a05
0xcf74,//Outdoor_aCtmpWgtWdhTh_a06
0xd042, //Outdoor_aCtmpWgtHgtTh
0xd152, //Outdoor_aCtmpWgtHgtTh_a01
0xd258, //Outdoor_aCtmpWgtHgtTh_a02
0xd35e, //Outdoor_aCtmpWgtHgtTh_a03
0xd464, //Outdoor_aCtmpWgtHgtTh_a04
0xd56a, //Outdoor_aCtmpWgtHgtTh_a05
0xd672, //Outdoor_aCtmpWgtHgtTh_a06
0xd77a, //Outdoor_aCtmpWgtHgtTh_a07
0xd888, //Outdoor_aCtmpWgtHgtTh_a08
0xd911, //Outdoor_aCtmpWgt
0xda23,//Outdoor_aCtmpWgt_a01
0xdb22,//Outdoor_aCtmpWgt_a02
0xdc11, //Outdoor_aCtmpWgt_a03
0xdd22,//Outdoor_aCtmpWgt_a04
0xde22, //Outdoor_aCtmpWgt_a05
0xdf11, //Outdoor_aCtmpWgt_a06
0xe033, //Outdoor_aCtmpWgt_a07
0xe131,//Outdoor_aCtmpWgt_a08
0xe212, //Outdoor_aCtmpWgt_a09
0xe366,//Outdoor_aCtmpWgt_a10
0xe441,//Outdoor_aCtmpWgt_a11
0xe513, //Outdoor_aCtmpWgt_a12
0xe677,//Outdoor_aCtmpWgt_a13
0xe741,//Outdoor_aCtmpWgt_a14
0xe813,//Outdoor_aCtmpWgt_a15
0xe974, //Outdoor_aCtmpWgt_a16
0xea11, //Outdoor_aCtmpWgt_a17
0xeb23,//Outdoor_aCtmpWgt_a18
0xec53,//Outdoor_aCtmpWgt_a19
0xed11, //Outdoor_aCtmpWgt_a20
0xee43,//Outdoor_aCtmpWgt_a21
0xef31,//Outdoor_aCtmpWgt_a22
0xf011, //Outdoor_aCtmpWgt_a23
0xf111, //Outdoor_aCtmpWgt_a24

0xf212, //aYlvlWgt
0xf334, //aYlvlWgt_a01
0xf443, //aYlvlWgt_a02
0xf532, //aYlvlWgt_a03
0xf611,//aYlvlWgt_a04
0xf711, //aYlvlWgt_a05
0xf811, //aYlvlWgt_a06
0xf911, //aYlvlWgt_a07
0xfa11, //aYlvlWgt_a08
0xfb11, //aYlvlWgt_a09
0xfc11, //aYlvlWgt_a10
0xfd11, //aYlvlWgt_a11
0x0e00, // burst end

//Page ca
0x03ca,
0x0e01, // burst start

0x1011, //aYlvlWgt_a12
0x1122, //aYlvlWgt_a13
0x1222, //aYlvlWgt_a14
0x1311, //aYlvlWgt_a15

0x1464, //Outdoor_aTgtAngle
0x156b, //Outdoor_aTgtAngle_a01
0x1670, //Outdoor_aTgtAngle_a02
0x177a,//Outdoor_aTgtAngle_a03
0x1884,//Outdoor_aTgtAngle_a04
0x191a,//Outdoor_aRgTgtOfs
0x1a10,//Outdoor_aRgTgtOfs_a01
0x1b08,//Outdoor_aRgTgtOfs_a02
0x1c04,//Outdoor_aRgTgtOfs_a03
0x1d04,//Outdoor_aRgTgtOfs_a04
0x1e9e,//Outdoor_aBgTgtOfs
0x1f88, //Outdoor_aBgTgtOfs_a01
0x2084,//Outdoor_aBgTgtOfs_a02
0x2182,//Outdoor_aBgTgtOfs_a03
0x2282,//Outdoor_aBgTgtOfs_a04
0x2382,//Outdoor_bRgDefTgt
0x2488, //Outdoor_bBgDefTgt

0x251c, //Outdoor_aWhtPtTrcAglOfs
0x261a, //Outdoor_aWhtPtTrcAglOfs_a01
0x2718, //Outdoor_aWhtPtTrcAglOfs_a02
0x2816, //Outdoor_aWhtPtTrcAglOfs_a03
0x2914, //Outdoor_aWhtPtTrcAglOfs_a04
0x2a12, //Outdoor_aWhtPtTrcAglOfs_a05
0x2b10, //Outdoor_aWhtPtTrcAglOfs_a06
0x2c0f, //Outdoor_aWhtPtTrcAglOfs_a07
0x2d0e, //Outdoor_aWhtPtTrcAglOfs_a08
0x2e0e, //Outdoor_aWhtPtTrcAglOfs_a09
0x2f0a, //Outdoor_bWhtPtTrcCnt
0x3028, //Outdoor_aRtoDiffThNrBp
0x3148, //Outdoor_aRtoDiffThNrBp_a01
0x3228, //Outdoor_aAglDiffThTrWhtPt
0x3350, //Outdoor_aAglDiffThTrWhtPt_a01
0x34aa, //Outdoor_bWgtRatioTh1
0x35a0, //Outdoor_bWgtRatioTh2
0x360a, //Outdoor_bWgtOfsTh1
0x37a0, //Outdoor_bWgtOfsTh2
0x386d, //Outdoor_bWhtPtCorAglMin
0x3978, //Outdoor_bWhtPtCorAglMax
0x3a04, //Outdoor_bYlvlMin
0x3bf8, //Outdoor_bYlvlMax
0x3c28, //Outdoor_bPxlWgtLmtLoTh
0x3d78, //Outdoor_bPxlWgtLmtHiTh
0x3e00, //Outdoor_SplBldWgt_1
0x3f00, //Outdoor_SplBldWgt_2
0x4064, //Outdoor_SplBldWgt_3
0x4160, //Outdoor_TgtOff_StdHiTh
0x4230, //Outdoor_TgtOff_StdLoTh
0x4304,
0x44c0,
0x4507,
0x46c0,
0x4702, //Outdoor_aRatioBox
0x48b2, //Outdoor_aRatioBox_a01
0x4905, //Outdoor_aRatioBox_a02
0x4adc, //Outdoor_aRatioBox_a03
0x4b0a, //Outdoor_aRatioBox_a04
0x4c28, //Outdoor_aRatioBox_a05
0x4d0c, //Outdoor_aRatioBox_a06
0x4e1c, //Outdoor_aRatioBox_a07
0x4f02, //Outdoor_aRatioBox_a08
0x50ee, //Outdoor_aRatioBox_a09
0x5106, //Outdoor_aRatioBox_a10
0x5272, //Outdoor_aRatioBox_a11
0x5308, //Outdoor_aRatioBox_a12
0x5498, //Outdoor_aRatioBox_a13
0x550a, //Outdoor_aRatioBox_a14
0x56f0, //Outdoor_aRatioBox_a15
0x5703, //Outdoor_aRatioBox_a16
0x5820, //Outdoor_aRatioBox_a17
0x5907, //Outdoor_aRatioBox_a18
0x5a08, //Outdoor_aRatioBox_a19
0x5b07, //Outdoor_aRatioBox_a20
0x5c6c, //Outdoor_aRatioBox_a21
0x5d09, //Outdoor_aRatioBox_a22
0x5e60, //Outdoor_aRatioBox_a23
0x5f03, //Outdoor_aRatioBox_a24
0x6084, //Outdoor_aRatioBox_a25
0x6107, //Outdoor_aRatioBox_a26
0x62d0, //Outdoor_aRatioBox_a27
0x6306, //Outdoor_aRatioBox_a28
0x6440, //Outdoor_aRatioBox_a29
0x6508, //Outdoor_aRatioBox_a30
0x6634, //Outdoor_aRatioBox_a31
0x6703, //Outdoor_aRatioBox_a32
0x68e8, //Outdoor_aRatioBox_a33
0x6908, //Outdoor_aRatioBox_a34
0x6ad0, //Outdoor_aRatioBox_a35
0x6b04, //Outdoor_aRatioBox_a36
0x6c4c, //Outdoor_aRatioBox_a37
0x6d07, //Outdoor_aRatioBox_a38
0x6e08, //Outdoor_aRatioBox_a39

0x6f04,
0x7000,

0x7105, //Out2_Adt_RgainMin
0x7200, //Out2_Adt_RgainMin_n01
0x7305, //Out2_Adt_RgainMax
0x74c0,//Out2_Adt_RgainMax_n01
0x7504, //Out2_Adt_GgainMin
0x7600, //Out2_Adt_GgainMin_n01
0x7704, //Out2_Adt_GgainMax
0x7800, //Out2_Adt_GgainMax_n01
0x7905, //Out2_Adt_BgainMin
0x7ae0, //Out2_Adt_BgainMin_n01
0x7b06, //Out2_Adt_BgainMax
0x7ca0, //Out2_Adt_BgainMax_n01

0x7d05, //Out1_Adt_RgainMin
0x7e40,//Out1_Adt_RgainMin_n01
0x7f06, //Out1_Adt_RgainMax
0x8040,//Out1_Adt_RgainMax_n01
0x8104, //Out1_Adt_GgainMin
0x8200, //Out1_Adt_GgainMin_n01
0x8304, //Out1_Adt_GgainMax
0x8400, //Out1_Adt_GgainMax_n01
0x8505, //Out1_Adt_BgainMin
0x8680, //Out1_Adt_BgainMin_n01
0x8707, //Out1_Adt_BgainMax
0x8800, //Out1_Adt_BgainMax_n01

0x8904, //In_Adt_RgainMin
0x8a00, //In_Adt_RgainMin_n01
0x8b0d, //In_Adt_RgainMax
0x8c00, //In_Adt_RgainMax_n01
0x8d04, //In_Adt_GgainMin
0x8e00, //In_Adt_GgainMin_n01
0x8f05, //In_Adt_GgainMax
0x9080, //In_Adt_GgainMax_n01
0x9104, //In_Adt_BgainMin
0x9200, //In_Adt_BgainMin_n01
0x930d, //In_Adt_BgainMax
0x9480, //In_Adt_BgainMax_n01

0x9504, //Manual_Adt_RgainMin
0x9600, //Manual_Adt_RgainMin_n01
0x970d, //Manual_Adt_RgainMax
0x9800, //Manual_Adt_RgainMax_n01
0x9904, //Manual_Adt_GgainMin
0x9a00, //Manual_Adt_GgainMin_n01
0x9b04, //Manual_Adt_GgainMax
0x9c80, //Manual_Adt_GgainMax_n01
0x9d04, //Manual_Adt_BgainMin
0x9e00, //Manual_Adt_BgainMin_n01
0x9f0b, //Manual_Adt_BgainMax
0xa000, //Manual_Adt_BgainMax_n01

0x0e00, //burst end

0x03c8,
0x1700, //AWB Speed
0x1811,
0x2100,
0x2201,
0x11C3,	//AWB reset


/////////////////////////////////////////////////////////////////////////////////
// CD page(OTP control)
/////////////////////////////////////////////////////////////////////////////////
0x03CD,
0x1003,

0x2210,
//Manual Typical colo ratio write
0x271A, //Typical RG=0.685*1000 = 6850 = 1AC2
0x28C2,
0x2910, //Typical BG=0.430*1000 = 4300 = 10CC
0x2ACC,
0x2b0a,//+/-10 valid ratio check

///////////////////////////////////////////////////////////////////////////////
// Color ratio setting
/////////////////////////////////////////////////////////////////////////////////
0x03CE,
0x3098,	//Color ratio on
0x3101,
0x3304,	//R gain def
0x3400,
0x3504,	//G gain def
0x3600,
0x3704,	//B gain def
0x3800,

0x4500, //Outdoor In EvTh
0x4600,
0x4727,
0x4810,
0x4900, //Outdoor Out EvTh
0x4a00,
0x4b4e,
0x4c20,

0x553e, //Low In Th
0x564c, //Low Out Th
0x575c, //High Out Th
0x586c, //High In Th

0x6288, //Out weight
0x6386, //Indoor weight
0x6488, //Dark weight
0x65d8, //Low weight
0x6686, //High weight

///////////////////////////////////////////////////////////////////////////////
// D3 ~ D8 Page (Adaptive)
///////////////////////////////////////////////////////////////////////////////

0x03d3,	// Adaptive start

0x0e01, // burst start

0x1000,
0x1100,
0x1200,
0x1300,
0x1400,
0x1500,
0x1600,
0x1700,
0x1800,
0x1900,

0x1a00,	// Def_Yoffset
0x1b32,	// DYOFS_Ratio
0x1c00,	// DYOFS_Limit //10

0x1d00,	//EV Th OutEnd : 120fps AG 1x DG 1x
0x1e00,
0x1f20,
0x208d,

0x2100,	//EV Th OutStr : 80fps  Ag 1x Dg 1x
0x2200,
0x2330,
0x24d4,

0x2500,	//EV Th Dark1Str
0x2603,
0x2770,
0x28f0,

0x2900,	//EV Th Dark1End
0x2a05,
0x2b57,
0x2c30,

0x2d00,	//EV Th Dark2Str
0x2e06,
0x2f1a,
0x3080,

0x3100,	//EV Th Dark2End
0x3208,
0x3386,
0x34b6,

0x354b, //Ctmp LT End
0x3652, //Ctmp LT Str
0x3769,	//Ctmp HT Str
0x3873,	//Ctmp HT End

0x3900,// LSC_EvTh_OutEnd_4
0x3a00,// LSC_EvTh_OutEnd_3
0x3b13,// LSC_EvTh_OutEnd_2
0x3c88,// LSC_EvTh_OutEnd_1    def : 200fps  Ag 1x Dg 1x

0x3d00,// LSC_EvTh_OutStr_4
0x3e00,// LSC_EvTh_OutStr_3
0x3f30,// LSC_EvTh_OutStr_2
0x40d4,// LSC_EvTh_OutStr_1    def :  80fps  Ag 1x Dg 1x

0x4100,// LSC_EvTh_Dark1Str_4
0x4205,// LSC_EvTh_Dark1Str_3
0x43b8,// LSC_EvTh_Dark1Str_2
0x44d8,// LSC_EvTh_Dark1Str_1  def :  8fps  Ag 3x Dg 1x

0x4500,// LSC_EvTh_Dark1End_4
0x460b,// LSC_EvTh_Dark1End_3
0x4771,// LSC_EvTh_Dark1End_2
0x48b0,// LSC_EvTh_Dark1End_1  def :  8fps  Ag 6x Dg 1x

0x4900,// LSC_EvTh_Dark2Str_4
0x4a0f,// LSC_EvTh_Dark2Str_3
0x4b42,// LSC_EvTh_Dark2Str_2
0x4c40,// LSC_EvTh_Dark2Str_1  def :  8fps  Ag 8x Dg 1x

0x4d00,// LSC_EvTh_Dark2End_4
0x4e1e,// LSC_EvTh_Dark2End_3
0x4f84,// LSC_EvTh_Dark2End_2
0x5080,// LSC_EvTh_Dark2End_1  def :  4fps  Ag 8x Dg 1x

0x5155,//LSC Ctmp LTEnd Out
0x5264,	//LSC Ctmp LTStr Out
0x5378,//LSC Ctmp HTStr Out
0x5486,//LSC Ctmp HTEnd Out

0x5546,	//LSC Ctmp LTEnd In
0x5656,	//LSC Ctmp LTStr In
0x576e,	//LSC Ctmp HTStr In
0x5876,	//LSC Ctmp HTEnd In

0x5950,	// LSC_CTmpTh_LT_End_Dark
0x5a78,	// LSC_CTmpTh_LT_Str_Dark
0x5ba0,	// LSC_CTmpTh_HT_Str_Dark
0x5cb4,	// LSC_CTmpTh_HT_End_Dark

0x5d00,	// UniScn_EvMinTh_4
0x5e00,	// UniScn_EvMinTh_3
0x5f04,	// UniScn_EvMinTh_2
0x60e2,	// UniScn_EvMinTh_1    def : 600fps  Ag 1x Dg 1x

0x6100,	// UniScn_EvMaxTh_4
0x6205,	// UniScn_EvMaxTh_3
0x63b8,	// UniScn_EvMaxTh_2
0x64d8,	// UniScn_EvMaxTh_1     def :  8fps  Ag 3x Dg 1x

0x654e,	// UniScn_AglMinTh_1
0x6650,	// UniScn_AglMinTh_2
0x6773,	// UniScn_AglMaxTh_1
0x687d,	// UniScn_AglMaxTh_2

0x6903,	// UniScn_YstdMinTh
0x6a0a,// UniScn_YstdMaxTh
0x6b1e,	// UniScn_BPstdMinTh
0x6c34,// UniScn_BPstdMaxTh

0x6d64,	// Ytgt_ColWgt_Out
0x6e64,	// Ytgt_ColWgt_Dark
0x6f64,	// ColSat_ColWgt_Out
0x7064,	// ColSat_ColWgt_Dark
0x7164,	// CMC_ColWgt_Out
0x7264,	// CMC_ColWgt_Dark
0x7364,	// MCMC_ColWgt_Out
0x7464,	// MCMC_ColWgt_Dark
0x7564,	// CustomReg_CorWgt_Out
0x7664,	// CustomReg_CorWgt_Dark

0x7764,	// UniScn_Y_Ratio
0x7850,	// UniScn_Cb_Ratio
0x7950,	// UniScn_Cr_Ratio

0x7a00,	// Ytgt_offset
0x7b00,	// CbSat_offset
0x7c00,	// CrSat_offset

0x7d36,	// Y_target_Outdoor
0x7e3c,	// Y_target_Indoor
0x7f3c,	// Y_target_Dark1
0x803c,	// Y_target_Dark2
0x813c,	// Y_target_LowTemp
0x823c,	// Y_target_HighTemp

0x8380, // Cb_Outdoor
0x8495,	// Cb _Sat_Indoor
0x85a0,	// Cb _Sat_Dark1
0x8684,	// Cb _Sat_Dark2
0x8788,	// Cb _Sat_LowTemp
0x8892,	// Cb _Sat_HighTemp

0x8980,	// Cr _Sat_Outdoor
0x8a90,	// Cr _Sat_Indoor
0x8ba0,	// Cr _Sat_Dark1
0x8c80,	// Cr _Sat_Dark2
0x8d75,	// Cr _Sat_LowTemp
0x8e92,	// Cr _Sat_HighTemp

0x8f82,	// BLC_ofs_r_Outdoor
0x9081,	// BLC_ofs_b_Outdoor
0x9182,	// BLC_ofs_gr_Outdoor
0x9282,	// BLC_ofs_gb_Outdoor

0x9381,	// BLC_ofs_r_Indoor
0x9480,	// BLC_ofs_b_Indoor
0x9581,	// BLC_ofs_gr_Indoor
0x9681,	// BLC_ofs_gb_Indoor

0x9784,	// BLC_ofs_r_Dark1
0x9884,	// BLC_ofs_b_Dark1
0x9985,	// BLC_ofs_gr_Dark1
0x9a85,	// BLC_ofs_gb_Dark1

0x9b84,	// BLC_ofs_r_Dark2
0x9c84,	// BLC_ofs_b_Dark2
0x9d85,	// BLC_ofs_gr_Dark2
0x9e85,	// BLC_ofs_gb_Dark2

0x9f00,	//LSC Out_L ofs G
0xa000,	//LSC Out_L ofs B
0xa100,	//LSC Out_L ofs R
0xa280,	//LSC Out_L Gain G
0xa382,	//LSC Out_L Gain B
0xa484,//LSC Out_L Gain R

0xa500,	//LSC Out_M ofs G
0xa600,	//LSC Out_M ofs B
0xa700,	//LSC Out_M ofs R
0xa880,	//LSC Out_M Gain G
0xa982,//LSC Out_M Gain B
0xaa7e,	//LSC Out_M Gain R

0xab00,	//LSC Out_H ofs G
0xac00,	//LSC Out_H ofs B
0xad00,	//LSC Out_H ofs R
0xae80,	//LSC Out_H Gain G
0xaf84,//LSC Out_H Gain B
0xb078,//LSC Out_H Gain R

0xb100,	// LSC0_Ind_LowTmp        offset g
0xb200,	// LSC1_Ind_LowTmp        offset b
0xb300,	// LSC2_Ind_LowTmp        offset r
0xb480,	// LSC3_Ind_LowTmp        gain g
0xb580,	// LSC4_Ind_LowTmp        gain b
0xb688,	// LSC5_Ind_LowTmp        gain r

0xb700,	// LSC0_Ind_MiddleTmp     offset g
0xb800,	// LSC1_Ind_MiddleTmp     offset b
0xb900,	// LSC2_Ind_MiddleTmp     offset r
0xba80,	// LSC3_Ind_MiddleTmp     gain g
0xbb80,	// LSC4_Ind_MiddleTmp     gain b
0xbc7e,	// LSC5_Ind_MiddleTmp     gain r

0xbd00,	// LSC0_Ind_HighTmp       offset g
0xbe00,	// LSC1_Ind_HighTmp       offset b
0xbf00,	// LSC2_Ind_HighTmp       offset r
0xc080,	// LSC3_Ind_HighTmp       gain g
0xc180,	// LSC4_Ind_HighTmp       gain b
0xc27e,// LSC5_Ind_HighTmp       gain r

0xc300,	// LSC0_Dark1_LowTmp      offset g
0xc400,	// LSC1_Dark1_LowTmp      offset b
0xc500,	// LSC2_Dark1_LowTmp      offset r
0xc668,	// LSC3_Dark1_LowTmp      gain g
0xc768,	// LSC4_Dark1_LowTmp      gain b
0xc868,	// LSC5_Dark1_LowTmp      gain r

0xc900,	// LSC0_Dark1_MiddleTmp   offset g
0xca00,	// LSC1_Dark1_MiddleTmp   offset b
0xcb00,	// LSC2_Dark1_MiddleTmp   offset r
0xcc68,	// LSC3_Dark1_MiddleTmp   gain g
0xcd68,	// LSC4_Dark1_MiddleTmp   gain b
0xce68,	// LSC5_Dark1_MiddleTmp   gain r

0xcf00,	// LSC0_Dark1_HighTmp   offset g
0xd000,	// LSC1_Dark1_HighTmp   offset b
0xd100,	// LSC2_Dark1_HighTmp   offset r
0xd268,	// LSC3_Dark1_HighTmp   gain g
0xd368,	// LSC4_Dark1_HighTmp   gain b
0xd468,	// LSC5_Dark1_HighTmp   gain r

0xd500,	// LSC0_Dark2           offset g
0xd600,	// LSC1_Dark2           offset b
0xd700,	// LSC2_Dark2           offset r
0xd868,	// LSC3_Dark2           gain g
0xd968,	// LSC4_Dark2           gain b
0xda68,	// LSC5_Dark2           gain r

0xdb2f, //CMCSIGN_Out
0xdc55, //CMC_Out_00
0xdd1c, //CMC_Out_01
0xde07, //CMC_Out_02
0xdf0a, //CMC_Out_03
0xe051, //CMC_Out_04
0xe107, //CMC_Out_05
0xe201, //CMC_Out_06
0xe314, //CMC_Out_07
0xe455, //CMC_Out_08

0xe504,	// CMC_Out_LumTh1      CMC SP gain axis X(luminance)
0xe60a,	// CMC_Out_LumTh2
0xe710,	// CMC_Out_LumTh3
0xe818,	// CMC_Out_LumTh4
0xe920,	// CMC_Out_LumTh5
0xea28,	// CMC_Out_LumTh6
0xeb40,	// CMC_Out_LumTh7

0xec20,	// CMC_Out_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0xed20,	// CMC_Out_LumGain2_R
0xee20,	// CMC_Out_LumGain3_R
0xef20,	// CMC_Out_LumGain4_R
0xf020,	// CMC_Out_LumGain5_R
0xf120,	// CMC_Out_LumGain6_R
0xf220,	// CMC_Out_LumGain7_R
0xf320,	// CMC_Out_LumGain8_R    20 = x1.0

0xf420,	// CMC_Out_LumGain1_G  CMC SP G gain axis Y (gain):: max32
0xf520,	// CMC_Out_LumGain2_G
0xf620,	// CMC_Out_LumGain3_G
0xf720,	// CMC_Out_LumGain4_G
0xf820,	// CMC_Out_LumGain5_G
0xf920,	// CMC_Out_LumGain6_G
0xfa20,	// CMC_Out_LumGain7_G
0xfb20,	// CMC_Out_LumGain8_G    20 = x1.0

0xfc20,	// CMC_Out_LumGain1_B  CMC SP B gain axis Y (gain):: max32
0xfd20,	// CMC_Out_LumGain2_B
0x0e00, // burst end

0x03d4,	// page D4
0x0e01, // burst start

0x1020,	// CMC_Out_LumGain3_B
0x1120,	// CMC_Out_LumGain4_B
0x1220,	// CMC_Out_LumGain5_B
0x1320,	// CMC_Out_LumGain6_B
0x1420,	// CMC_Out_LumGain7_B
0x1520,	// CMC_Out_LumGain8_B    20 = x1.0

0x162f, //CMCSIGN_In_Mid
0x1753, //CMC_In_Mid_00
0x1816, //CMC_In_Mid_01
0x1903, //CMC_In_Mid_02
0x1a10, //CMC_In_Mid_03
0x1b53, //CMC_In_Mid_04
0x1c03, //CMC_In_Mid_05
0x1d04, //CMC_In_Mid_06
0x1e1d, //CMC_In_Mid_07
0x1f61, //CMC_In_Mid_08

0x2004,	// CMC_Ind_LumTh1     CMC SP gain axis X(luminance)
0x210a,	// CMC_Ind_LumTh2
0x2210,	// CMC_Ind_LumTh3
0x2318,	// CMC_Ind_LumTh4
0x2420,	// CMC_Ind_LumTh5
0x2528,	// CMC_Ind_LumTh6
0x2640,	// CMC_Ind_LumTh7

0x2708,	// CMC_Ind_LumGain1_R   CMC SP R gain axis Y (gain):: max32
0x2812,	// CMC_Ind_LumGain2_R
0x2918,	// CMC_Ind_LumGain3_R
0x2a1c,	// CMC_Ind_LumGain4_R
0x2b1e,	// CMC_Ind_LumGain5_R
0x2c20,	// CMC_Ind_LumGain6_R
0x2d20,	// CMC_Ind_LumGain7_R
0x2e20,	// CMC_Ind_LumGain8_R    20 = x1.0

0x2f08,	// CMC_Ind_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x3012,	// CMC_Ind_LumGain2_G
0x3118,	// CMC_Ind_LumGain3_G
0x321c,	// CMC_Ind_LumGain4_G
0x331e,	// CMC_Ind_LumGain5_G
0x3420,	// CMC_Ind_LumGain6_G
0x3520,	// CMC_Ind_LumGain7_G
0x3620,	// CMC_Ind_LumGain8_G    20 = x1.0

0x3708,	// CMC_Ind_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x3812,	// CMC_Ind_LumGain2_B
0x3918,	// CMC_Ind_LumGain3_B
0x3a1c,	// CMC_Ind_LumGain4_B
0x3b1e,	// CMC_Ind_LumGain5_B
0x3c20,	// CMC_Ind_LumGain6_B
0x3d20,	// CMC_Ind_LumGain7_B
0x3e20,	// CMC_Ind_LumGain8_B   20 = x1.0

0x3f2f, //CMCSIGN_Dark1
0x4053, //CMC_Dark1_00
0x411c, //CMC_Dark1_01
0x4209, //CMC_Dark1_02
0x430e, //CMC_Dark1_03
0x4453, //CMC_Dark1_04
0x4505, //CMC_Dark1_05
0x4603, //CMC_Dark1_06
0x4723, //CMC_Dark1_07
0x4866, //CMC_Dark1_08

0x4904,	// CMC_Dark1_LumTh1     CMC SP gain axis X(luminance)
0x4a0a,	// CMC_Dark1_LumTh2
0x4b10,	// CMC_Dark1_LumTh3
0x4c18,	// CMC_Dark1_LumTh4
0x4d20,	// CMC_Dark1_LumTh5
0x4e28,	// CMC_Dark1_LumTh6
0x4f40,	// CMC_Dark1_LumTh7

0x5008,	// CMC_Dark1_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0x5112,	// CMC_Dark1_LumGain2_R
0x5218,	// CMC_Dark1_LumGain3_R
0x531c,	// CMC_Dark1_LumGain4_R
0x541e,	// CMC_Dark1_LumGain5_R
0x5520,	// CMC_Dark1_LumGain6_R
0x5620,	// CMC_Dark1_LumGain7_R
0x5720,	// CMC_Dark1_LumGain8_R    20 = x1.0

0x5808,	// CMC_Dark1_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x5912,	// CMC_Dark1_LumGain2_G
0x5a18,	// CMC_Dark1_LumGain3_G
0x5b1c,	// CMC_Dark1_LumGain4_G
0x5c1e,	// CMC_Dark1_LumGain5_G
0x5d20,	// CMC_Dark1_LumGain6_G
0x5e20,	// CMC_Dark1_LumGain7_G
0x5f20,	// CMC_Dark1_LumGain8_G    20 = x1.0

0x6008,	// CMC_Dark1_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x6112,	// CMC_Dark1_LumGain2_B
0x6218,	// CMC_Dark1_LumGain3_B
0x631c,	// CMC_Dark1_LumGain4_B
0x641e,	// CMC_Dark1_LumGain5_B
0x6520,	// CMC_Dark1_LumGain6_B
0x6620,	// CMC_Dark1_LumGain7_B
0x6720,	// CMC_Dark1_LumGain8_B   20 = x1.0

0x682f, //CMCSIGN_Dark2
0x6953, //CMC_Dark2_00
0x6a1c, //CMC_Dark2_01
0x6b09, //CMC_Dark2_02
0x6c0e, //CMC_Dark2_03
0x6d53, //CMC_Dark2_04
0x6e05, //CMC_Dark2_05
0x6f03, //CMC_Dark2_06
0x7023, //CMC_Dark2_07
0x7166, //CMC_Dark2_08

0x7204,	// CMC_Dark2_LumTh1        CMC SP gain axis X(luminance)
0x730a,	// CMC_Dark2_LumTh2
0x7410,	// CMC_Dark2_LumTh3
0x7518,	// CMC_Dark2_LumTh4
0x7620,	// CMC_Dark2_LumTh5
0x7728,	// CMC_Dark2_LumTh6
0x7840,	// CMC_Dark2_LumTh7

0x7908,	// CMC_Dark2_LumGain1_R    CMC SP R gain
0x7a12,	// CMC_Dark2_LumGain2_R
0x7b18,	// CMC_Dark2_LumGain3_R
0x7c1c,	// CMC_Dark2_LumGain4_R
0x7d1e,	// CMC_Dark2_LumGain5_R
0x7e20,	// CMC_Dark2_LumGain6_R
0x7f20,	// CMC_Dark2_LumGain7_R
0x8020,	// CMC_Dark2_LumGain8_R    20 = x1.

0x8108,	// CMC_Dark2_LumGain1_G    CMC SP G gain
0x8212,	// CMC_Dark2_LumGain2_G
0x8318,	// CMC_Dark2_LumGain3_G
0x841c,	// CMC_Dark2_LumGain4_G
0x851e,	// CMC_Dark2_LumGain5_G
0x8620,	// CMC_Dark2_LumGain6_G
0x8720,	// CMC_Dark2_LumGain7_G
0x8820,	// CMC_Dark2_LumGain8_G    20 = x1.

0x8908,	// CMC_Dark2_LumGain1_B    CMC SP B gain
0x8a12,	// CMC_Dark2_LumGain2_B
0x8b18,	// CMC_Dark2_LumGain3_B
0x8c1c,	// CMC_Dark2_LumGain4_B
0x8d1e,	// CMC_Dark2_LumGain5_B
0x8e20,	// CMC_Dark2_LumGain6_B
0x8f20,	// CMC_Dark2_LumGain7_B
0x9020,	// CMC_Dark2_LumGain8_B    20 = x1.0

0x912f, // CMCSIGN_In_Low
0x9253, // CMC_In_Low_00
0x931e, //CMC_In_Low_01
0x940b, //CMC_In_Low_02
0x9518, //CMC_In_Low_03
0x9661, // CMC_In_Low_04
0x9709, //CMC_In_Low_05
0x9804, //CMC_In_Low_06
0x9914, //CMC_In_Low_07
0x9a58, // CMC_In_Low_08

0x9b04,	// CMC_LowTemp_LumTh1     CMC SP gain axis X(luminance)
0x9c0a,	// CMC_LowTemp_LumTh2
0x9d10,	// CMC_LowTemp_LumTh3
0x9e18,	// CMC_LowTemp_LumTh4
0x9f20,	// CMC_LowTemp_LumTh5
0xa028,	// CMC_LowTemp_LumTh6
0xa140,	// CMC_LowTemp_LumTh7

0xa220,	// CMC_LowTemp_LumGain1_R    CMC SP R gain
0xa320,	// CMC_LowTemp_LumGain2_R
0xa420,	// CMC_LowTemp_LumGain3_R
0xa520,	// CMC_LowTemp_LumGain4_R
0xa620,	// CMC_LowTemp_LumGain5_R
0xa720,	// CMC_LowTemp_LumGain6_R
0xa820,	// CMC_LowTemp_LumGain7_R
0xa920,	// CMC_LowTemp_LumGain8_R    20 = x1.0

0xaa20,	// CMC_LowTemp_LumGain1_G    CMC SP G gain
0xab20,	// CMC_LowTemp_LumGain2_G
0xac20,	// CMC_LowTemp_LumGain3_G
0xad20,	// CMC_LowTemp_LumGain4_G
0xae20,	// CMC_LowTemp_LumGain5_G
0xaf20,	// CMC_LowTemp_LumGain6_G
0xb020,	// CMC_LowTemp_LumGain7_G
0xb120,	// CMC_LowTemp_LumGain8_G    20 = x1.0

0xb220,	// CMC_LowTemp_LumGain1_B    CMC SP B gain
0xb320,	// CMC_LowTemp_LumGain2_B
0xb420,	// CMC_LowTemp_LumGain3_B
0xb520,	// CMC_LowTemp_LumGain4_B
0xb620,	// CMC_LowTemp_LumGain5_B
0xb720,	// CMC_LowTemp_LumGain6_B
0xb820,	// CMC_LowTemp_LumGain7_B
0xb920,	// CMC_LowTemp_LumGain8_B    20 = x1.0

0xba2d, //CMCSIGN_In_High
0xbb55, //CMC_In_High_00
0xbc21, //CMC_In_High_01
0xbd0c, //CMC_In_High_02
0xbe08, //CMC_In_High_03
0xbf55, //CMC_In_High_04
0xc00d, //CMC_In_High_05
0xc103, //CMC_In_High_06
0xc218, //CMC_In_High_07
0xc355, //CMC_In_High_08

0xc404,	// CMC_HighTemp_LumTh1       CMC SP gain axis X(luminance)
0xc50a,	// CMC_HighTemp_LumTh2
0xc610,	// CMC_HighTemp_LumTh3
0xc718,	// CMC_HighTemp_LumTh4
0xc820,	// CMC_HighTemp_LumTh5
0xc928,	// CMC_HighTemp_LumTh6
0xca40,	// CMC_HighTemp_LumTh7

0xcb20,	// CMC_HighTemp_LumGain1_R   CMC SP R gain
0xcc20,	// CMC_HighTemp_LumGain2_R
0xcd20,	// CMC_HighTemp_LumGain3_R
0xce20,	// CMC_HighTemp_LumGain4_R
0xcf20,	// CMC_HighTemp_LumGain5_R
0xd020,	// CMC_HighTemp_LumGain6_R
0xd120,	// CMC_HighTemp_LumGain7_R
0xd220,	// CMC_HighTemp_LumGain8_R    20 = x1.0

0xd320,	// CMC_HighTemp_LumGain1_G   CMC SP G gain
0xd420,	// CMC_HighTemp_LumGain2_G
0xd520,	// CMC_HighTemp_LumGain3_G
0xd620,	// CMC_HighTemp_LumGain4_G
0xd720,	// CMC_HighTemp_LumGain5_G
0xd820,	// CMC_HighTemp_LumGain6_G
0xd920,	// CMC_HighTemp_LumGain7_G
0xda20,	// CMC_HighTemp_LumGain8_G    20 = x1.

0xdb20,	// CMC_HighTemp_LumGain1_B   CMC SP B gain
0xdc20,	// CMC_HighTemp_LumGain2_B
0xdd20,	// CMC_HighTemp_LumGain3_B
0xde20,	// CMC_HighTemp_LumGain4_B
0xdf20,	// CMC_HighTemp_LumGain5_B
0xe020,	// CMC_HighTemp_LumGain6_B
0xe120,	// CMC_HighTemp_LumGain7_B
0xe220,	// CMC_HighTemp_LumGain8_B   20 = x1.0

////////////////////
// Adaptive Gamma //
////////////////////

0xe300,	//GMA_OUT
0xe400,
0xe503,
0xe60b,
0xe715,
0xe821,
0xe92f,
0xea3c,
0xeb49,
0xec54,
0xed5e,
0xee68,
0xef72,
0xf079,
0xf17f,
0xf284,
0xf38a,
0xf48e,
0xf594,
0xf698,
0xf79d,
0xf8a7,
0xf9b2,
0xfabb,
0xfbcb,
0xfcda,
0xfde9,
0x0e00, // burst end

0x03d5,	//Page d5

0x0e01, // burst start

0x10f7,
0x11ff,
0x12ff,
0x13ff,
0x14ff,
0x15ff,
0x16ff,

0x1700,	//GMA_IN
0x1805,
0x1909,
0x1a11,
0x1b19,
0x1c26,
0x1d33,
0x1e3f,
0x1f4a,
0x2055,
0x215e,
0x2267,
0x236f,
0x2477,
0x257e,
0x2684,
0x278b,
0x2891,
0x2996,
0x2a9b,
0x2ba0,
0x2ca8,
0x2db2,
0x2ebc,
0x2fcd,
0x30dd,
0x31ec,
0x32f9,
0x33ff,
0x34ff,
0x35ff,
0x36ff,
0x37ff,
0x38ff,

0x3900,	//GMA_D1
0x3a04,
0x3b08,
0x3c0f,
0x3d1b,
0x3e2e,
0x3f3e,
0x404e,
0x415b,
0x4265,
0x4371,
0x447b,
0x4585,
0x468d,
0x4795,
0x489b,
0x49a3,
0x4aa9,
0x4bb0,
0x4cb6,
0x4dbb,
0x4ec5,
0x4fcf,
0x50d7,
0x51e7,
0x52f3,
0x53fa,
0x54ff,
0x55ff,
0x56ff,
0x57ff,
0x58ff,
0x59ff,
0x5aff,

0x5b00,	//GMA_D2
0x5c04,
0x5d08,
0x5e0f,
0x5f1b,
0x602e,
0x613e,
0x624e,
0x635b,
0x6465,
0x6571,
0x667b,
0x6785,
0x688d,
0x6995,
0x6a9b,
0x6ba3,
0x6ca9,
0x6db0,
0x6eb6,
0x6fbb,
0x70c5,
0x71cf,
0x72d7,
0x73e7,
0x74f3,
0x75fa,
0x76ff,
0x77ff,
0x78ff,
0x79ff,
0x7aff,
0x7bff,
0x7cff,

///////////////////
// Adaptive MCMC //
///////////////////

// Outdoor MCMC
0x7d15, //Outdoor_delta1
0x7e19, //Outdoor_center1
0x7f0f, //Outdoor_delta2
0x8086, //Outdoor_center2
0x8117, //Outdoor_delta3
0x82bd, //Outdoor_center3
0x8317, //Outdoor_delta4
0x84ee, //Outdoor_center4
0x8593, //Outdoor_delta5
0x8625, //Outdoor_center5
0x8793, //Outdoor_delta6
0x8851, //Outdoor_center6

0x8940, //Outdoor_sat_gain1
0x8a40, //Outdoor_sat_gain2
0x8b40, //Outdoor_sat_gain3
0x8c40, //Outdoor_sat_gain4
0x8d40, //Outdoor_sat_gain5
0x8e40, //Outdoor_sat_gain6
0x8f94, //Outdoor_hue_angle1
0x9089, //Outdoor_hue_angle2
0x9110, //Outdoor_hue_angle3
0x9214, //Outdoor_hue_angle4
0x930b, //Outdoor_hue_angle5
0x9487, //Outdoor_hue_angle6

0x9500,	// MCMC24_Outdoor  mcmc_rgb_ofs_sign_r
0x9600,	// MCMC25_Outdoor  mcmc_rgb_ofs_sign_g
0x9700,	// MCMC26_Outdoor  mcmc_rgb_ofs_sign_b

0x9800,	// MCMC27_Outdoor  mcmc_rgb_ofs_r1 R
0x9900,	// MCMC28_Outdoor  mcmc_rgb_ofs_r1 G
0x9a00,	// MCMC29_Outdoor  mcmc_rgb_ofs_r1 B

0x9b00,	// MCMC30_Outdoor  mcmc_rgb_ofs_r2 R
0x9c00,	// MCMC31_Outdoor  mcmc_rgb_ofs_r2 G
0x9d00,	// MCMC32_Outdoor  mcmc_rgb_ofs_r2 B

0x9e00,	// MCMC33_Outdoor  mcmc_rgb_ofs_r3 R
0x9f00,	// MCMC34_Outdoor  mcmc_rgb_ofs_r3 G
0xa000,	// MCMC35_Outdoor  mcmc_rgb_ofs_r3 B

0xa100,	// MCMC36_Outdoor  mcmc_rgb_ofs_r4 R
0xa200,	// MCMC37_Outdoor  mcmc_rgb_ofs_r4 G
0xa300,	// MCMC38_Outdoor  mcmc_rgb_ofs_r4 B

0xa400,	// MCMC39_Outdoor  mcmc_rgb_ofs_r5 R
0xa500,	// MCMC40_Outdoor  mcmc_rgb_ofs_r5 G
0xa600,	// MCMC41_Outdoor  mcmc_rgb_ofs_r5 B

0xa700,	// MCMC42_Outdoor  mcmc_rgb_ofs_r6 R
0xa800,	// MCMC43_Outdoor  mcmc_rgb_ofs_r6 G
0xa900,	// MCMC44_Outdoor  mcmc_rgb_ofs_r6 B

0xaa00,	// MCMC45_Outdoor  mcmc_std_offset1
0xab00,	// MCMC46_Outdoor  mcmc_std_offset2
0xacff,	// MCMC47_Outdoor  mcmc_std_th_max
0xad00,	// MCMC48_Outdoor  mcmc_std_th_min

0xae3f,	// MCMC49_Outdoor  mcmc_lum_gain_wgt_th1 R1 magenta
0xaf3f,	// MCMC50_Outdoor  mcmc_lum_gain_wgt_th2 R1
0xb03f,	// MCMC51_Outdoor  mcmc_lum_gain_wgt_th3 R1
0xb13f,	// MCMC52_Outdoor  mcmc_lum_gain_wgt_th4 R1
0xb230,	// MCMC53_Outdoor  mcmc_rg1_lum_sp1      R1
0xb350,	// MCMC54_Outdoor  mcmc_rg1_lum_sp2      R1
0xb480,	// MCMC55_Outdoor  mcmc_rg1_lum_sp3      R1
0xb5b0,	// MCMC56_Outdoor  mcmc_rg1_lum_sp4      R1

0xb63f,	// MCMC57_Outdoor  mcmc_lum_gain_wgt_th1 R2 red
0xb73f,	// MCMC58_Outdoor  mcmc_lum_gain_wgt_th2 R2
0xb83f,	// MCMC59_Outdoor  mcmc_lum_gain_wgt_th3 R2
0xb93f,	// MCMC60_Outdoor  mcmc_lum_gain_wgt_th4 R2
0xba28,	// MCMC61_Outdoor  mcmc_rg2_lum_sp1      R2
0xbb50,	// MCMC62_Outdoor  mcmc_rg2_lum_sp2      R2
0xbc80,	// MCMC63_Outdoor  mcmc_rg2_lum_sp3      R2
0xbdb0,	// MCMC64_Outdoor  mcmc_rg2_lum_sp4      R2

0xbe3f,	// MCMC65_Outdoor  mcmc_lum_gain_wgt_th1 R3 yellow
0xbf3f,	// MCMC66_Outdoor  mcmc_lum_gain_wgt_th2 R3
0xc030,// MCMC67_Outdoor  mcmc_lum_gain_wgt_th3 R3
0xc12a,// MCMC68_Outdoor  mcmc_lum_gain_wgt_th4 R3
0xc220,// MCMC69_Outdoor  mcmc_rg3_lum_sp1      R3
0xc340,// MCMC70_Outdoor  mcmc_rg3_lum_sp2      R3
0xc470,// MCMC71_Outdoor  mcmc_rg3_lum_sp3      R3
0xc5b0,	// MCMC72_Outdoor  mcmc_rg3_lum_sp4      R3

0xc63f,	// MCMC73_Outdoor  mcmc_lum_gain_wgt_th1 R4 Green
0xc73f,	// MCMC74_Outdoor  mcmc_lum_gain_wgt_th2 R4
0xc83f,	// MCMC75_Outdoor  mcmc_lum_gain_wgt_th3 R4
0xc93f,	// MCMC76_Outdoor  mcmc_lum_gain_wgt_th4 R4
0xca10,	// MCMC77_Outdoor  mcmc_rg4_lum_sp1      R4
0xcb30,	// MCMC78_Outdoor  mcmc_rg4_lum_sp2      R4
0xcc60,	// MCMC79_Outdoor  mcmc_rg4_lum_sp3      R4
0xcd90,	// MCMC80_Outdoor  mcmc_rg4_lum_sp4      R4

0xce3f,	// MCMC81_Outdoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0xcf3f,	// MCMC82_Outdoor  mcmc_rg5_gain_wgt_th2 R5
0xd03f,	// MCMC83_Outdoor  mcmc_rg5_gain_wgt_th3 R5
0xd13f,	// MCMC84_Outdoor  mcmc_rg5_gain_wgt_th4 R5
0xd228,	// MCMC85_Outdoor  mcmc_rg5_lum_sp1      R5
0xd350,	// MCMC86_Outdoor  mcmc_rg5_lum_sp2      R5
0xd480,	// MCMC87_Outdoor  mcmc_rg5_lum_sp3      R5
0xd5b0,	// MCMC88_Outdoor  mcmc_rg5_lum_sp4      R5

0xd63f,	// MCMC89_Outdoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0xd73f,	// MCMC90_Outdoor  mcmc_rg6_gain_wgt_th2 R6
0xd83f,	// MCMC91_Outdoor  mcmc_rg6_gain_wgt_th3 R6
0xd93f,	// MCMC92_Outdoor  mcmc_rg6_gain_wgt_th4 R6
0xda28,	// MCMC93_Outdoor  mcmc_rg6_lum_sp1      R6
0xdb50,	// MCMC94_Outdoor  mcmc_rg6_lum_sp2      R6
0xdc80,	// MCMC95_Outdoor  mcmc_rg6_lum_sp3      R6
0xddb0,	// MCMC96_Outdoor  mcmc_rg6_lum_sp4      R6

0xde1e,	// MCMC97_Outdoor  mcmc2_allgain_x1
0xdf3c,	// MCMC98_Outdoor  mcmc2_allgain_x2
0xe03c,	// MCMC99_Outdoor  mcmc2_allgain_x4
0xe11e,	// MCMC100_Outdoor mcmc2_allgain_x5
0xe21e,	// MCMC101_Outdoor mcmc2_allgain_x7
0xe33c,	// MCMC102_Outdoor mcmc2_allgain_x8
0xe43c,	// MCMC103_Outdoor mcmc2_allgain_x10
0xe51e,	// MCMC104_Outdoor mcmc2_allgain_x11

0xe614, //Outdoor_allgain_y1
0xe714, //Outdoor_allgain_y2
0xe813, //Outdoor_allgain_y3
0xe912, //Outdoor_allgain_y4
0xea12, //Outdoor_allgain_y5
0xeb14, //Outdoor_allgain_y6
0xec16, //Outdoor_allgain_y7
0xed18, //Outdoor_allgain_y8
0xee1a, //Outdoor_allgain_y9
0xef17, //Outdoor_allgain_y10
0xf014, //Outdoor_allgain_y11
0xf113, //Outdoor_allgain_y12

// Indoor MCMC
0xf210, //Indoor_delta1
0xf31e, //Indoor_center1
0xf40b, //Indoor_delta2
0xf56f, //Indoor_center2
0xf610, //Indoor_delta3
0xf79c, // Indoor_center3
0xf807, // Indoor_delta4
0xf9b8, // Indoor_center4
0xfa90, //Indoor_delta5
0xfb2d, //Indoor_center5
0xfc92, //Indoor_delta6
0xfd4f, //Indoor_center6
0x0e00, // burst end

0x03d6,	// Page D6

0x0e01, // burst start

0x1040, //Indoor_sat_gain1
0x1140, //Indoor_sat_gain2
0x1240, //Indoor_sat_gain3
0x1340, //Indoor_sat_gain4
0x1440, //Indoor_sat_gain5
0x1540, //Indoor_sat_gain6

0x1600, //Indoor_hue_angle1
0x178a, //Indoor_hue_angle2
0x1800, //Indoor_hue_angle3
0x1900, //Indoor_hue_angle4
0x1a00, //Indoor_hue_angle5
0x1b02, //Indoor_hue_angle6

0x1c00,	// MCMC24_Indoor   mcmc_rgb_ofs_sign_r
0x1d00,	// MCMC25_Indoor   mcmc_rgb_ofs_sign_g
0x1e00,	// MCMC26_Indoor   mcmc_rgb_ofs_sign_b

0x1f00,	// MCMC27_Indoor   mcmc_rgb_ofs_r1 R
0x2000,	// MCMC28_Indoor   mcmc_rgb_ofs_r1 G
0x2100,	// MCMC29_Indoor   mcmc_rgb_ofs_r1 B

0x2200,	// MCMC30_Indoor   mcmc_rgb_ofs_r2 R
0x2300,	// MCMC31_Indoor   mcmc_rgb_ofs_r2 G
0x2400,	// MCMC32_Indoor   mcmc_rgb_ofs_r2 B

0x2500,	// MCMC33_Indoor   mcmc_rgb_ofs_r3 R
0x2600,	// MCMC34_Indoor   mcmc_rgb_ofs_r3 G
0x2700,	// MCMC35_Indoor   mcmc_rgb_ofs_r3 B

0x2800,	// MCMC36_Indoor   mcmc_rgb_ofs_r4 R
0x2900,	// MCMC37_Indoor   mcmc_rgb_ofs_r4 G
0x2a00,	// MCMC38_Indoor   mcmc_rgb_ofs_r4 B

0x2b00,	// MCMC39_Indoor   mcmc_rgb_ofs_r5 R
0x2c00,	// MCMC40_Indoor   mcmc_rgb_ofs_r5 G
0x2d00,	// MCMC41_Indoor   mcmc_rgb_ofs_r5 B

0x2e00,	// MCMC42_Indoor  mcmc_rgb_ofs_r6 R
0x2f00,	// MCMC43_Indoor  mcmc_rgb_ofs_r6 G
0x3000,	// MCMC44_Indoor  mcmc_rgb_ofs_r6 B

0x3100,	// MCMC45_Indoor  mcmc_std_offset1
0x3200,	// MCMC46_Indoor  mcmc_std_offset2
0x33ff,	// MCMC47_Indoor  mcmc_std_th_max
0x3400,	// MCMC48_Indoor  mcmc_std_th_min

0x3510,	// MCMC49_Indoor  mcmc_lum_gain_wgt_th1 R1 magenta
0x3621,	// MCMC50_Indoor  mcmc_lum_gain_wgt_th2 R1
0x3734,	// MCMC51_Indoor  mcmc_lum_gain_wgt_th3 R1
0x383f,	// MCMC52_Indoor  mcmc_lum_gain_wgt_th4 R1
0x3908,	// MCMC53_Indoor  mcmc_rg1_lum_sp1      R1
0x3a15,	// MCMC54_Indoor  mcmc_rg1_lum_sp2      R1
0x3b2f,	// MCMC55_Indoor  mcmc_rg1_lum_sp3      R1
0x3c51,	// MCMC56_Indoor  mcmc_rg1_lum_sp4      R1

0x3d3f,	// MCMC57_Indoor  mcmc_lum_gain_wgt_th1 R2 red
0x3e3f,	// MCMC58_Indoor  mcmc_lum_gain_wgt_th2 R2
0x3f3f,	// MCMC59_Indoor  mcmc_lum_gain_wgt_th3 R2
0x403f,	// MCMC60_Indoor  mcmc_lum_gain_wgt_th4 R2
0x4128,	// MCMC61_Indoor  mcmc_rg2_lum_sp1      R2
0x4250,	// MCMC62_Indoor  mcmc_rg2_lum_sp2      R2
0x4380,	// MCMC63_Indoor  mcmc_rg2_lum_sp3      R2
0x44b0,	// MCMC64_Indoor  mcmc_rg2_lum_sp4      R2

0x453f,	// MCMC65_Indoor  mcmc_lum_gain_wgt_th1 R3 yellow
0x463f,	// MCMC66_Indoor  mcmc_lum_gain_wgt_th2 R3
0x473f,	// MCMC67_Indoor  mcmc_lum_gain_wgt_th3 R3
0x483f,	// MCMC68_Indoor  mcmc_lum_gain_wgt_th4 R3
0x4928,	// MCMC69_Indoor  mcmc_rg3_lum_sp1      R3
0x4a50,	// MCMC70_Indoor  mcmc_rg3_lum_sp2      R3
0x4b80,	// MCMC71_Indoor  mcmc_rg3_lum_sp3      R3
0x4cb0,	// MCMC72_Indoor  mcmc_rg3_lum_sp4      R3

0x4d3f,	// MCMC73_Indoor  mcmc_lum_gain_wgt_th1 R4 Green
0x4e3f,	// MCMC74_Indoor  mcmc_lum_gain_wgt_th2 R4
0x4f3f,	// MCMC75_Indoor  mcmc_lum_gain_wgt_th3 R4
0x503f,	// MCMC76_Indoor  mcmc_lum_gain_wgt_th4 R4
0x5110,	// MCMC77_Indoor  mcmc_rg4_lum_sp1      R4
0x5230,	// MCMC78_Indoor  mcmc_rg4_lum_sp2      R4
0x5360,	// MCMC79_Indoor  mcmc_rg4_lum_sp3      R4
0x5490,	// MCMC80_Indoor  mcmc_rg4_lum_sp4      R4

0x553f,	// MCMC81_Indoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0x563f,	// MCMC82_Indoor  mcmc_rg5_gain_wgt_th2 R5
0x573f,	// MCMC83_Indoor  mcmc_rg5_gain_wgt_th3 R5
0x583f,	// MCMC84_Indoor  mcmc_rg5_gain_wgt_th4 R5
0x5928,	// MCMC85_Indoor  mcmc_rg5_lum_sp1      R5
0x5a50,	// MCMC86_Indoor  mcmc_rg5_lum_sp2      R5
0x5b80,	// MCMC87_Indoor  mcmc_rg5_lum_sp3      R5
0x5cb0,	// MCMC88_Indoor  mcmc_rg5_lum_sp4      R5

0x5d3f,	// MCMC89_Indoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0x5e3f,	// MCMC90_Indoor  mcmc_rg6_gain_wgt_th2 R6
0x5f3f,	// MCMC91_Indoor  mcmc_rg6_gain_wgt_th3 R6
0x603f,	// MCMC92_Indoor  mcmc_rg6_gain_wgt_th4 R6
0x6128,	// MCMC93_Indoor  mcmc_rg6_lum_sp1      R6
0x6250,	// MCMC94_Indoor  mcmc_rg6_lum_sp2      R6
0x6380,	// MCMC95_Indoor  mcmc_rg6_lum_sp3      R6
0x64b0,	// MCMC96_Indoor  mcmc_rg6_lum_sp4      R6

0x651d,	// MCMC97_Indoor  mcmc2_allgain_x1
0x663b,	// MCMC98_Indoor  mcmc2_allgain_x2
0x673b,	// MCMC99_Indoor  mcmc2_allgain_x4
0x681d,	// MCMC100_Indoor mcmc2_allgain_x5
0x691d,	// MCMC101_Indoor mcmc2_allgain_x7
0x6a3b,	// MCMC102_Indoor mcmc2_allgain_x8
0x6b3b,	// MCMC103_Indoor mcmc2_allgain_x10
0x6c1d,	// MCMC104_Indoor mcmc2_allgain_x11

0x6d0e,	// MCMC105_Indoor mcmc2_allgain_y0
0x6e0f,	// MCMC106_Indoor mcmc2_allgain_y1
0x6f0f,	// MCMC107_Indoor mcmc2_allgain_y2
0x700f,	// MCMC108_Indoor mcmc2_allgain_y3
0x710f,	// MCMC109_Indoor mcmc2_allgain_y4
0x7210,	// MCMC110_Indoor mcmc2_allgain_y5
0x7310,	// MCMC111_Indoor mcmc2_allgain_y6
0x7410,	// MCMC112_Indoor mcmc2_allgain_y7
0x7510,	// MCMC113_Indoor mcmc2_allgain_y8
0x760f,	// MCMC114_Indoor mcmc2_allgain_y9
0x770e,	// MCMC115_Indoor mcmc2_allgain_y10
0x780d,	// MCMC116_Indoor mcmc2_allgain_y11

// Dark1 MCMC
0x7917, //Dark1_delta1
0x7a56, //Dark1_center1
0x7b10, //Dark1_delta2
0x7c70, //Dark1_center2
0x7d10, //Dark1_delta3
0x7e9c, //Dark1_center3
0x7f18, //Dark1_delta4
0x80db, //Dark1_center4
0x8198, //Dark1_delta5
0x8226, //Dark1_center5
0x8399, //Dark1_delta6
0x845b, //Dark1_center6

0x8540, //Dark1_sat_gain1
0x8640, //Dark1_sat_gain2
0x8740, //Dark1_sat_gain3
0x8840, //Dark1_sat_gain4
0x8940, //Dark1_sat_gain5
0x8a40, //Dark1_sat_gain6
0x8b91, //Dark1_hue_angle1
0x8c00, //Dark1_hue_angle2
0x8d00, //Dark1_hue_angle3
0x8e0a, //Dark1_hue_angle4
0x8f05, //Dark1_hue_angle5
0x9086, //Dark1_hue_angle6

0x913f,	// MCMC24_Dark1   mcmc_rgb_ofs_sign
0x923f,	// MCMC25_Dark1   mcmc_rgb_ofs_sign
0x933f,	// MCMC26_Dark1   mcmc_rgb_ofs_sign

0x9400,	// MCMC27_Dark1   mcmc_rgb_ofs_r1 R
0x9500,	// MCMC28_Dark1   mcmc_rgb_ofs_r1 G
0x9600,	// MCMC29_Dark1   mcmc_rgb_ofs_r1 B

0x9700,	// MCMC30_Dark1   mcmc_rgb_ofs_r2 R
0x9800,	// MCMC31_Dark1   mcmc_rgb_ofs_r2 G
0x9900,	// MCMC32_Dark1   mcmc_rgb_ofs_r2 B

0x9a00,	// MCMC33_Dark1   mcmc_rgb_ofs_r3 R
0x9b00,	// MCMC34_Dark1   mcmc_rgb_ofs_r3 G
0x9c00,	// MCMC35_Dark1   mcmc_rgb_ofs_r3 B

0x9d00,	// MCMC36_Dark1   mcmc_rgb_ofs_r4 R
0x9e00,	// MCMC37_Dark1   mcmc_rgb_ofs_r4 G
0x9f00,	// MCMC38_Dark1   mcmc_rgb_ofs_r4 B

0xa000,	// MCMC39_Dark1   mcmc_rgb_ofs_r5 R
0xa100,	// MCMC40_Dark1   mcmc_rgb_ofs_r5 G
0xa200,	// MCMC41_Dark1   mcmc_rgb_ofs_r5 B

0xa300,	// MCMC42_Dark1  mcmc_rgb_ofs_r6 R
0xa400,	// MCMC43_Dark1  mcmc_rgb_ofs_r6 G
0xa500,	// MCMC44_Dark1  mcmc_rgb_ofs_r6 B

0xa600,	// MCMC45_Dark1  mcmc_std_offset1
0xa700,	// MCMC46_Dark1  mcmc_std_offset2
0xa8ff,	// MCMC47_Dark1  mcmc_std_th_max
0xa900,	// MCMC48_Dark1  mcmc_std_th_min

0xaa3f,	// MCMC49_Dark1  mcmc_lum_gain_wgt R1
0xab3f,	// MCMC50_Dark1  mcmc_lum_gain_wgt R1
0xac3f,	// MCMC51_Dark1  mcmc_lum_gain_wgt R1
0xad3f,	// MCMC52_Dark1  mcmc_lum_gain_wgt R1
0xae30,	// MCMC53_Dark1  mcmc_rg1_lum_sp1  R1
0xaf50,	// MCMC54_Dark1  mcmc_rg1_lum_sp2  R1
0xb080,	// MCMC55_Dark1  mcmc_rg1_lum_sp3  R1
0xb1b0,	// MCMC56_Dark1  mcmc_rg1_lum_sp4  R1

0xb23f,	// MCMC57_Dark1  mcmc_lum_gain_wgt R2
0xb33f,	// MCMC58_Dark1  mcmc_lum_gain_wgt R2
0xb43f,	// MCMC59_Dark1  mcmc_lum_gain_wgt R2
0xb53f,	// MCMC60_Dark1  mcmc_lum_gain_wgt R2
0xb628,	// MCMC61_Dark1  mcmc_rg2_lum_sp1  R2
0xb750,	// MCMC62_Dark1  mcmc_rg2_lum_sp2  R2
0xb880,	// MCMC63_Dark1  mcmc_rg2_lum_sp3  R2
0xb9b0,	// MCMC64_Dark1  mcmc_rg2_lum_sp4  R2

0xba3f,	// MCMC65_Dark1  mcmc_lum_gain_wgt R3
0xbb3f,	// MCMC66_Dark1  mcmc_lum_gain_wgt R3
0xbc3f,	// MCMC67_Dark1  mcmc_lum_gain_wgt R3
0xbd3f,	// MCMC68_Dark1  mcmc_lum_gain_wgt R3
0xbe28,	// MCMC69_Dark1  mcmc_rg3_lum_sp1  R3
0xbf50,	// MCMC70_Dark1  mcmc_rg3_lum_sp2  R3
0xc080,	// MCMC71_Dark1  mcmc_rg3_lum_sp3  R3
0xc1b0,	// MCMC72_Dark1  mcmc_rg3_lum_sp4  R3

0xc23f,	// MCMC73_Dark1  mcmc_lum_gain_wgt R4
0xc33f,	// MCMC74_Dark1  mcmc_lum_gain_wgt R4
0xc43f,	// MCMC75_Dark1  mcmc_lum_gain_wgt R4
0xc53f,	// MCMC76_Dark1  mcmc_lum_gain_wgt R4
0xc610,	// MCMC77_Dark1  mcmc_rg4_lum_sp1  R4
0xc730,	// MCMC78_Dark1  mcmc_rg4_lum_sp2  R4
0xc860,	// MCMC79_Dark1  mcmc_rg4_lum_sp3  R4
0xc990,	// MCMC80_Dark1  mcmc_rg4_lum_sp4  R4

0xca3f,	// MCMC81_Dark1  mcmc_rg5_gain_wgt R5
0xcb3f,	// MCMC82_Dark1  mcmc_rg5_gain_wgt R5
0xcc3f,	// MCMC83_Dark1  mcmc_rg5_gain_wgt R5
0xcd3f,	// MCMC84_Dark1  mcmc_rg5_gain_wgt R5
0xce28,	// MCMC85_Dark1  mcmc_rg5_lum_sp1  R5
0xcf50,	// MCMC86_Dark1  mcmc_rg5_lum_sp2  R5
0xd080,	// MCMC87_Dark1  mcmc_rg5_lum_sp3  R5
0xd1b0,	// MCMC88_Dark1  mcmc_rg5_lum_sp4  R5

0xd23f,	// MCMC89_Dark1  mcmc_rg6_gain_wgt R6
0xd33f,	// MCMC90_Dark1  mcmc_rg6_gain_wgt R6
0xd43f,	// MCMC91_Dark1  mcmc_rg6_gain_wgt R6
0xd53f,	// MCMC92_Dark1  mcmc_rg6_gain_wgt R6
0xd628,	// MCMC93_Dark1  mcmc_rg6_lum_sp1  R6
0xd750,	// MCMC94_Dark1  mcmc_rg6_lum_sp2  R6
0xd880,	// MCMC95_Dark1  mcmc_rg6_lum_sp3  R6
0xd9b0,	// MCMC96_Dark1  mcmc_rg6_lum_sp4  R6

0xda1c,	// MCMC97_Dark1  mcmc2_allgain_x1
0xdb3a,	// MCMC98_Dark1  mcmc2_allgain_x2
0xdc3a,	// MCMC99_Dark1  mcmc2_allgain_x4
0xdd1c,	// MCMC100_Dark1 mcmc2_allgain_x5
0xde1c,	// MCMC101_Dark1 mcmc2_allgain_x7
0xdf3a,	// MCMC102_Dark1 mcmc2_allgain_x8
0xe03a,	// MCMC103_Dark1 mcmc2_allgain_x10
0xe11c,	// MCMC104_Dark1 mcmc2_allgain_x11

0xe20f, //Dark1_allgain_y1
0xe310, //Dark1_allgain_y2
0xe410, //Dark1_allgain_y3
0xe511, //Dark1_allgain_y4
0xe610, //Dark1_allgain_y5
0xe713, //Dark1_allgain_y6
0xe812, //Dark1_allgain_y7
0xe912, //Dark1_allgain_y8
0xea12, //Dark1_allgain_y9
0xeb11, //Dark1_allgain_y10
0xec10, //Dark1_allgain_y11
0xed0f, //Dark1_allgain_y12

// Dark2 MCMC
0xee17,	// MCMC00_Dark2   mcmc_delta1
0xef56,	// MCMC01_Dark2   mcmc_center1
0xf010,	// MCMC02_Dark2   mcmc_delta2
0xf170,	// MCMC03_Dark2   mcmc_center2
0xf210,	// MCMC04_Dark2   mcmc_delta3
0xf39c,	// MCMC05_Dark2   mcmc_center3
0xf418,	// MCMC06_Dark2   mcmc_delta4
0xf5db,	// MCMC07_Dark2   mcmc_center4
0xf698,	// MCMC08_Dark2   mcmc_delta5
0xf726,	// MCMC09_Dark2   mcmc_center5
0xf899,	// MCMC10_Dark2   mcmc_delta6
0xf95b,	// MCMC11_Dark2   mcmc_center6

0xfa40,	// MCMC12_Dark2   mcmc_sat_gain1
0xfb40,	// MCMC13_Dark2   mcmc_sat_gain2
0xfc40,	// MCMC14_Dark2   mcmc_sat_gain3
0xfd40,	// MCMC15_Dark2   mcmc_sat_gain4
0x0e00, // burst end

0x03d7,// Page D7

0x0e01, // burst start

0x1040,	// MCMC16_Dark2   mcmc_sat_gain5
0x1140,	// MCMC17_Dark2   mcmc_sat_gain6
0x1291,	// MCMC18_Dark2   mcmc_hue_angle1
0x1300,	// MCMC19_Dark2   mcmc_hue_angle2
0x1400,	// MCMC20_Dark2   mcmc_hue_angle3
0x150a,	// MCMC21_Dark2   mcmc_hue_angle4
0x160f,	// MCMC22_Dark2   mcmc_hue_angle5
0x1786,	// MCMC23_Dark2   mcmc_hue_angle6

0x182f,	// MCMC24_Dark2   mcmc_rgb_ofs_sig
0x192f,	// MCMC25_Dark2   mcmc_rgb_ofs_sig
0x1a2f,	// MCMC26_Dark2   mcmc_rgb_ofs_sig

0x1b00,	// MCMC27_Dark2   mcmc_rgb_ofs_r1
0x1c00,	// MCMC28_Dark2   mcmc_rgb_ofs_r1
0x1d00,	// MCMC29_Dark2   mcmc_rgb_ofs_r1

0x1e00,	// MCMC30_Dark2   mcmc_rgb_ofs_r2
0x1f00,	// MCMC31_Dark2   mcmc_rgb_ofs_r2
0x2000,	// MCMC32_Dark2   mcmc_rgb_ofs_r2

0x2100,	// MCMC33_Dark2   mcmc_rgb_ofs_r3
0x2200,	// MCMC34_Dark2   mcmc_rgb_ofs_r3
0x2300,	// MCMC35_Dark2   mcmc_rgb_ofs_r3

0x2400,	// MCMC36_Dark2   mcmc_rgb_ofs_r4
0x2500,	// MCMC37_Dark2   mcmc_rgb_ofs_r4
0x2600,	// MCMC38_Dark2   mcmc_rgb_ofs_r4

0x2700,	// MCMC39_Dark2   mcmc_rgb_ofs_r5
0x2800,	// MCMC40_Dark2   mcmc_rgb_ofs_r5
0x2900,	// MCMC41_Dark2   mcmc_rgb_ofs_r5

0x2a00,	// MCMC42_Dark2  mcmc_rgb_ofs_r6 R
0x2b00,	// MCMC43_Dark2  mcmc_rgb_ofs_r6 G
0x2c00,	// MCMC44_Dark2  mcmc_rgb_ofs_r6 B

0x2d00,	// MCMC45_Dark2  mcmc_std_offset1
0x2e00,	// MCMC46_Dark2  mcmc_std_offset2
0x2fff,	// MCMC47_Dark2  mcmc_std_th_max
0x3000,	// MCMC48_Dark2  mcmc_std_th_min

0x313f,	// MCMC49_Dark2  mcmc_lum_gain_wgt R1
0x323f,	// MCMC50_Dark2  mcmc_lum_gain_wgt R1
0x333f,	// MCMC51_Dark2  mcmc_lum_gain_wgt R1
0x343f,	// MCMC52_Dark2  mcmc_lum_gain_wgt R1
0x3530,	// MCMC53_Dark2  mcmc_rg1_lum_sp1  R1
0x3650,	// MCMC54_Dark2  mcmc_rg1_lum_sp2  R1
0x3780,	// MCMC55_Dark2  mcmc_rg1_lum_sp3  R1
0x38b0,	// MCMC56_Dark2  mcmc_rg1_lum_sp4  R1

0x393f,	// MCMC57_Dark2  mcmc_lum_gain_wgt R2
0x3a3f,	// MCMC58_Dark2  mcmc_lum_gain_wgt R2
0x3b3f,	// MCMC59_Dark2  mcmc_lum_gain_wgt R2
0x3c3f,	// MCMC60_Dark2  mcmc_lum_gain_wgt R2
0x3d28,	// MCMC61_Dark2  mcmc_rg2_lum_sp1  R2
0x3e50,	// MCMC62_Dark2  mcmc_rg2_lum_sp2  R2
0x3f80,	// MCMC63_Dark2  mcmc_rg2_lum_sp3  R2
0x40b0,	// MCMC64_Dark2  mcmc_rg2_lum_sp4  R2

0x413f,	// MCMC65_Dark2  mcmc_lum_gain_wgt R3
0x423f,	// MCMC66_Dark2  mcmc_lum_gain_wgt R3
0x433f,	// MCMC67_Dark2  mcmc_lum_gain_wgt R3
0x443f,	// MCMC68_Dark2  mcmc_lum_gain_wgt R3
0x4528,	// MCMC69_Dark2  mcmc_rg3_lum_sp1  R3
0x4650,	// MCMC70_Dark2  mcmc_rg3_lum_sp2  R3
0x4780,	// MCMC71_Dark2  mcmc_rg3_lum_sp3  R3
0x48b0,	// MCMC72_Dark2  mcmc_rg3_lum_sp4  R3

0x491a,	// MCMC73_Dark2  mcmc_lum_gain_wgt R4
0x4a28,	// MCMC74_Dark2  mcmc_lum_gain_wgt R4
0x4b3f,	// MCMC75_Dark2  mcmc_lum_gain_wgt R4
0x4c3f,	// MCMC76_Dark2  mcmc_lum_gain_wgt R4
0x4d10,	// MCMC77_Dark2  mcmc_rg4_lum_sp1  R4
0x4e30,	// MCMC78_Dark2  mcmc_rg4_lum_sp2  R4
0x4f60,	// MCMC79_Dark2  mcmc_rg4_lum_sp3  R4
0x5090,	// MCMC80_Dark2  mcmc_rg4_lum_sp4  R4

0x511a,	// MCMC81_Dark2  mcmc_rg5_gain_wgt R5
0x5228,	// MCMC82_Dark2  mcmc_rg5_gain_wgt R5
0x533f,	// MCMC83_Dark2  mcmc_rg5_gain_wgt R5
0x543f,	// MCMC84_Dark2  mcmc_rg5_gain_wgt R5
0x5528,	// MCMC85_Dark2  mcmc_rg5_lum_sp1  R5
0x5650,	// MCMC86_Dark2  mcmc_rg5_lum_sp2  R5
0x5780,	// MCMC87_Dark2  mcmc_rg5_lum_sp3  R5
0x58b0,	// MCMC88_Dark2  mcmc_rg5_lum_sp4  R5

0x591a,	// MCMC89_Dark2  mcmc_rg6_gain_wgt R6
0x5a28,	// MCMC90_Dark2  mcmc_rg6_gain_wgt R6
0x5b3f,	// MCMC91_Dark2  mcmc_rg6_gain_wgt R6
0x5c3f,	// MCMC92_Dark2  mcmc_rg6_gain_wgt R6
0x5d28,	// MCMC93_Dark2  mcmc_rg6_lum_sp1  R6
0x5e50,	// MCMC94_Dark2  mcmc_rg6_lum_sp2  R6
0x5f80,	// MCMC95_Dark2  mcmc_rg6_lum_sp3  R6
0x60b0,	// MCMC96_Dark2  mcmc_rg6_lum_sp4  R6

0x611b,	// MCMC97_Dark2  mcmc2_allgain_x1
0x6239,	// MCMC98_Dark2  mcmc2_allgain_x2
0x6339,	// MCMC99_Dark2  mcmc2_allgain_x4
0x641b,	// MCMC100_Dark2 mcmc2_allgain_x5
0x651b,	// MCMC101_Dark2 mcmc2_allgain_x7
0x6639,	// MCMC102_Dark2 mcmc2_allgain_x8
0x6739,	// MCMC103_Dark2 mcmc2_allgain_x10
0x681b,	// MCMC104_Dark2 mcmc2_allgain_x11

0x690f,	// MCMC105_Dark2 mcmc2_allgain_y0
0x6a10,	// MCMC106_Dark2 mcmc2_allgain_y1
0x6b10,	// MCMC107_Dark2 mcmc2_allgain_y2
0x6c11,	// MCMC108_Dark2 mcmc2_allgain_y3
0x6d10,	// MCMC109_Dark2 mcmc2_allgain_y4
0x6e13,	// MCMC110_Dark2 mcmc2_allgain_y5
0x6f12,	// MCMC111_Dark2 mcmc2_allgain_y6
0x7012,	// MCMC112_Dark2 mcmc2_allgain_y7
0x7112,	// MCMC113_Dark2 mcmc2_allgain_y8
0x7211,	// MCMC114_Dark2 mcmc2_allgain_y9
0x7310,	// MCMC115_Dark2 mcmc2_allgain_y10
0x740f,	// MCMC116_Dark2 mcmc2_allgain_y11

// LowTemp MCMC
0x7510,	// MCMC00_LowTemp   mcmc_delta1
0x7639,	// MCMC01_LowTemp   mcmc_center1
0x7710,	// MCMC02_LowTemp   mcmc_delta2
0x7859,	// MCMC03_LowTemp   mcmc_center2
0x7912,	// MCMC04_LowTemp   mcmc_delta3
0x7a9d,	// MCMC05_LowTemp   mcmc_center3
0x7b12,	// MCMC06_LowTemp   mcmc_delta4
0x7cc1,	// MCMC07_LowTemp   mcmc_center4
0x7d18,	// MCMC08_LowTemp   mcmc_delta5
0x7eeb,	// MCMC09_LowTemp   mcmc_center5
0x7f99,	// MCMC10_LowTemp   mcmc_delta6
0x801c,	// MCMC11_LowTemp   mcmc_center6

0x8140,	// MCMC12_LowTemp   mcmc_sat_gain1
0x8240,	// MCMC13_LowTemp   mcmc_sat_gain2
0x8340,	// MCMC14_LowTemp   mcmc_sat_gain3
0x8440,	// MCMC15_LowTemp   mcmc_sat_gain4
0x8540,	// MCMC16_LowTemp   mcmc_sat_gain5
0x8640,	// MCMC17_LowTemp   mcmc_sat_gain6
0x8700,	// MCMC18_LowTemp   mcmc_hue_angle1
0x8800,	// MCMC19_LowTemp   mcmc_hue_angle2
0x8900,	// MCMC20_LowTemp   mcmc_hue_angle3
0x8a00,	// MCMC21_LowTemp   mcmc_hue_angle4
0x8b00,	// MCMC22_LowTemp   mcmc_hue_angle5
0x8c00,	// MCMC23_LowTemp   mcmc_hue_angle6

0x8d1f,	// MCMC24_LowTemp   mcmc_rgb_ofs_sig
0x8e1f,	// MCMC25_LowTemp   mcmc_rgb_ofs_sig
0x8f1f,	// MCMC26_LowTemp   mcmc_rgb_ofs_sig

0x9000,	// MCMC27_LowTemp   mcmc_rgb_ofs_r1
0x9100,	// MCMC28_LowTemp   mcmc_rgb_ofs_r1
0x9200,	// MCMC29_LowTemp   mcmc_rgb_ofs_r1

0x9300,	// MCMC30_LowTemp   mcmc_rgb_ofs_r2
0x9400,	// MCMC31_LowTemp   mcmc_rgb_ofs_r2
0x9500,	// MCMC32_LowTemp   mcmc_rgb_ofs_r2

0x9600,	// MCMC33_LowTemp   mcmc_rgb_ofs_r3
0x9700,	// MCMC34_LowTemp   mcmc_rgb_ofs_r3
0x9800,	// MCMC35_LowTemp   mcmc_rgb_ofs_r3

0x9900,	// MCMC36_LowTemp   mcmc_rgb_ofs_r4
0x9a00,	// MCMC37_LowTemp   mcmc_rgb_ofs_r4
0x9b00,	// MCMC38_LowTemp   mcmc_rgb_ofs_r4

0x9c00,	// MCMC39_LowTemp   mcmc_rgb_ofs_r5
0x9d00,	// MCMC40_LowTemp   mcmc_rgb_ofs_r5
0x9e00,	// MCMC41_LowTemp   mcmc_rgb_ofs_r5

0x9f00,	// MCMC42_LowTemp  mcmc_rgb_ofs_r6 R
0xa000,	// MCMC43_LowTemp  mcmc_rgb_ofs_r6 G
0xa100,	// MCMC44_LowTemp  mcmc_rgb_ofs_r6 B

0xa200,	// MCMC45_LowTemp  mcmc_std_offset1
0xa300,	// MCMC46_LowTemp  mcmc_std_offset2
0xa4ff,	// MCMC47_LowTemp  mcmc_std_th_max
0xa500,	// MCMC48_LowTemp  mcmc_std_th_min

0xa63f,	// MCMC49_LowTemp  mcmc_lum_gain_wgt R1
0xa73f,	// MCMC50_LowTemp  mcmc_lum_gain_wgt R1
0xa83f,	// MCMC51_LowTemp  mcmc_lum_gain_wgt R1
0xa93f,	// MCMC52_LowTemp  mcmc_lum_gain_wgt R1
0xaa30,	// MCMC53_LowTemp  mcmc_rg1_lum_sp1  R1
0xab50,	// MCMC54_LowTemp  mcmc_rg1_lum_sp2  R1
0xac80,	// MCMC55_LowTemp  mcmc_rg1_lum_sp3  R1
0xadb0,	// MCMC56_LowTemp  mcmc_rg1_lum_sp4  R1

0xae3f,	// MCMC57_LowTemp  mcmc_lum_gain_wgt R2
0xaf3f,	// MCMC58_LowTemp  mcmc_lum_gain_wgt R2
0xb03f,	// MCMC59_LowTemp  mcmc_lum_gain_wgt R2
0xb13f,	// MCMC60_LowTemp  mcmc_lum_gain_wgt R2
0xb228,	// MCMC61_LowTemp  mcmc_rg2_lum_sp1  R2
0xb350,	// MCMC62_LowTemp  mcmc_rg2_lum_sp2  R2
0xb480,	// MCMC63_LowTemp  mcmc_rg2_lum_sp3  R2
0xb5b0,	// MCMC64_LowTemp  mcmc_rg2_lum_sp4  R2

0xb63f,	// MCMC65_LowTemp  mcmc_lum_gain_wgt R3
0xb73f,	// MCMC66_LowTemp  mcmc_lum_gain_wgt R3
0xb83f,	// MCMC67_LowTemp  mcmc_lum_gain_wgt R3
0xb93f,	// MCMC68_LowTemp  mcmc_lum_gain_wgt R3
0xba28,	// MCMC69_LowTemp  mcmc_rg3_lum_sp1  R3
0xbb50,	// MCMC70_LowTemp  mcmc_rg3_lum_sp2  R3
0xbc80,	// MCMC71_LowTemp  mcmc_rg3_lum_sp3  R3
0xbdb0,	// MCMC72_LowTemp  mcmc_rg3_lum_sp4  R3

0xbe3f,	// MCMC73_LowTemp  mcmc_lum_gain_wgt R4
0xbf3f,	// MCMC74_LowTemp  mcmc_lum_gain_wgt R4
0xc03f,	// MCMC75_LowTemp  mcmc_lum_gain_wgt R4
0xc13f,	// MCMC76_LowTemp  mcmc_lum_gain_wgt R4
0xc210,	// MCMC77_LowTemp  mcmc_rg4_lum_sp1  R4
0xc330,	// MCMC78_LowTemp  mcmc_rg4_lum_sp2  R4
0xc460,	// MCMC79_LowTemp  mcmc_rg4_lum_sp3  R4
0xc590,	// MCMC80_LowTemp  mcmc_rg4_lum_sp4  R4

0xc63f,	// MCMC81_LowTemp  mcmc_rg5_gain_wgt R5
0xc73f,	// MCMC82_LowTemp  mcmc_rg5_gain_wgt R5
0xc83f,	// MCMC83_LowTemp  mcmc_rg5_gain_wgt R5
0xc93f,	// MCMC84_LowTemp  mcmc_rg5_gain_wgt R5
0xca28,	// MCMC85_LowTemp  mcmc_rg5_lum_sp1  R5
0xcb50,	// MCMC86_LowTemp  mcmc_rg5_lum_sp2  R5
0xcc80,	// MCMC87_LowTemp  mcmc_rg5_lum_sp3  R5
0xcdb0,	// MCMC88_LowTemp  mcmc_rg5_lum_sp4  R5

0xce3f,	// MCMC89_LowTemp  mcmc_rg6_gain_wgt R6
0xcf3f,	// MCMC90_LowTemp  mcmc_rg6_gain_wgt R6
0xd03f,	// MCMC91_LowTemp  mcmc_rg6_gain_wgt R6
0xd13f,	// MCMC92_LowTemp  mcmc_rg6_gain_wgt R6
0xd228,	// MCMC93_LowTemp  mcmc_rg6_lum_sp1  R6
0xd350,	// MCMC94_LowTemp  mcmc_rg6_lum_sp2  R6
0xd480,	// MCMC95_LowTemp  mcmc_rg6_lum_sp3  R6
0xd5b0,	// MCMC96_LowTemp  mcmc_rg6_lum_sp4  R6

0xd61a,	// MCMC97_LowTemp  mcmc2_allgain_x1
0xd738,	// MCMC98_LowTemp  mcmc2_allgain_x2
0xd838,	// MCMC99_LowTemp  mcmc2_allgain_x4
0xd91a,	// MCMC100_LowTemp mcmc2_allgain_x5
0xda1a,	// MCMC101_LowTemp mcmc2_allgain_x7
0xdb38,	// MCMC102_LowTemp mcmc2_allgain_x8
0xdc38,	// MCMC103_LowTemp mcmc2_allgain_x10
0xdd1a,	// MCMC104_LowTemp mcmc2_allgain_x11

0xde10,	// MCMC105_LowTemp mcmc2_allgain_y0
0xdf0f,	// MCMC106_LowTemp mcmc2_allgain_y1
0xe00e,	// MCMC107_LowTemp mcmc2_allgain_y2
0xe10e,	// MCMC108_LowTemp mcmc2_allgain_y3
0xe212,	// MCMC109_LowTemp mcmc2_allgain_y4
0xe316,	// MCMC110_LowTemp mcmc2_allgain_y5
0xe416,	// MCMC111_LowTemp mcmc2_allgain_y6
0xe514,	// MCMC112_LowTemp mcmc2_allgain_y
0xe612,	// MCMC113_LowTemp mcmc2_allgain_y8
0xe710,	// MCMC114_LowTemp mcmc2_allgain_y9
0xe810,	// MCMC115_LowTemp mcmc2_allgain_y10
0xe910,	// MCMC116_LowTemp mcmc2_allgain_y11
0x0e00, // burst end

// HighTemp MCMC
0x03d7, //Page d7
0xea10, //Hi-Temp_delta1
0xeb39, //Hi-Temp_center1
0xec10, //Hi-Temp_delta2
0xed59, //Hi-Temp_center2
0xee12, //Hi-Temp_delta3
0xef9d, //Hi-Temp_center3
0xf012, //Hi-Temp_delta4
0xf1bd, //Hi-Temp_center4
0xf21e, //Hi-Temp_delta5
0xf3f1, //Hi-Temp_center5
0xf49e, //Hi-Temp_delta6
0xf534, //Hi-Temp_center6
0xf640, //Hi-Temp_sat_gain1
0xf740, //Hi-Temp_sat_gain2
0xf840, //Hi-Temp_sat_gain3
0xf940, //Hi-Temp_sat_gain4
0xfa40, //Hi-Temp_sat_gain5
0xfb40, //Hi-Temp_sat_gain6
0xfc00, //Hi-Temp_hue_angle1
0xfd00, //Hi-Temp_hue_angle2

0x03d8, //Page d8
0x0e01, // burst start

0x1000, //Hi-Temp_hue_angle3
0x1100, //Hi-Temp_hue_angle4
0x1206, //Hi-Temp_hue_angle5
0x1300, //Hi-Temp_hue_angle6
0x1411, //Hi-Temp_rgb_ofs_sign_r
0x1511, //Hi-Temp_rgb_ofs_sign_g
0x1611, //Hi-Temp_rgb_ofs_sign_b
0x1700, //Hi-Temp_rgb_ofs_scl_r1
0x1800, //Hi-Temp_rgb_ofs_scl_g1
0x1900, //Hi-Temp_rgb_ofs_scl_b1
0x1a00, //Hi-Temp_rgb_ofs_scl_r2
0x1b00, //Hi-Temp_rgb_ofs_scl_g2
0x1c00, //Hi-Temp_rgb_ofs_scl_b2
0x1d00, //Hi-Temp_rgb_ofs_scl_r3
0x1e00, //Hi-Temp_rgb_ofs_scl_g3
0x1f00, //Hi-Temp_rgb_ofs_scl_b3
0x2000, //Hi-Temp_rgb_ofs_scl_r4
0x2100, //Hi-Temp_rgb_ofs_scl_g4
0x2200, //Hi-Temp_rgb_ofs_scl_b4
0x2300, //Hi-Temp_rgb_ofs_scl_r5
0x2400, //Hi-Temp_rgb_ofs_scl_g5
0x2500, //Hi-Temp_rgb_ofs_scl_b5
0x2600, //Hi-Temp_rgb_ofs_scl_r6
0x2700, //Hi-Temp_rgb_ofs_scl_g6
0x2800, //Hi-Temp_rgb_ofs_scl_b6
0x2900, //Hi-Temp_std_offset1
0x2a00, //Hi-Temp_std_offset2
0x2bff, //Hi-Temp_std_th_max
0x2c00, //Hi-Temp_std_th_min
0x2d3f, //Hi-Temp_rg1_lum_gain_wgt_th1
0x2e3f, //Hi-Temp_rg1_lum_gain_wgt_th2
0x2f3f, //Hi-Temp_rg1_lum_gain_wgt_th3
0x303f, //Hi-Temp_rg1_lum_gain_wgt_th4
0x3130, //Hi-Temp_rg1_lum_sp1
0x3250, //Hi-Temp_rg1_lum_sp2
0x3380, //Hi-Temp_rg1_lum_sp3
0x34b0, //Hi-Temp_rg1_lum_sp4
0x353f, //Hi-Temp_rg2_gain_wgt_th1
0x363f, //Hi-Temp_rg2_gain_wgt_th2
0x373f, //Hi-Temp_rg2_gain_wgt_th3
0x383f, //Hi-Temp_rg2_gain_wgt_th4
0x3928, //Hi-Temp_rg2_lum_sp1
0x3a50, //Hi-Temp_rg2_lum_sp2
0x3b80, //Hi-Temp_rg2_lum_sp3
0x3cb0, //Hi-Temp_rg2_lum_sp4
0x3d3f, //Hi-Temp_rg3_gain_wgt_th1
0x3e3f, //Hi-Temp_rg3_gain_wgt_th2
0x3f3f, //Hi-Temp_rg3_gain_wgt_th3
0x403f, //Hi-Temp_rg3_gain_wgt_th4
0x4128, //Hi-Temp_rg3_lum_sp1
0x4250, //Hi-Temp_rg3_lum_sp2
0x4380, //Hi-Temp_rg3_lum_sp3
0x44b0, //Hi-Temp_rg3_lum_sp4

0x453f, //Hi-Temp_rg4_gain_wgt_th1
0x463f, //Hi-Temp_rg4_gain_wgt_th2
0x473f, //Hi-Temp_rg4_gain_wgt_th3
0x483f, //Hi-Temp_rg4_gain_wgt_th4
0x4910, //Hi-Temp_rg4_lum_sp1
0x4a30, //Hi-Temp_rg4_lum_sp2
0x4b60, //Hi-Temp_rg4_lum_sp3
0x4c90, //Hi-Temp_rg4_lum_sp4

0x4d3f, //Hi-Temp_rg5_gain_wgt_th1
0x4e3f, //Hi-Temp_rg5_gain_wgt_th2
0x4f3f, //Hi-Temp_rg5_gain_wgt_th3
0x503f, //Hi-Temp_rg5_gain_wgt_th4
0x5128, //Hi-Temp_rg5_lum_sp1
0x5250, //Hi-Temp_rg5_lum_sp2
0x5380, //Hi-Temp_rg5_lum_sp3
0x54b0, //Hi-Temp_rg5_lum_sp4

0x553f, //Hi-Temp_rg6_gain_wgt_th1
0x563f, //Hi-Temp_rg6_gain_wgt_th2
0x573f, //Hi-Temp_rg6_gain_wgt_th3
0x583f, //Hi-Temp_rg6_gain_wgt_th4
0x5928, //Hi-Temp_rg6_lum_sp1
0x5a50, //Hi-Temp_rg6_lum_sp2
0x5b80, //Hi-Temp_rg6_lum_sp3
0x5cb0, //Hi-Temp_rg6_lum_sp4

0x5d19, //Hi-Temp_allgain_x1
0x5e37, //Hi-Temp_allgain_x2
0x5f37, //Hi-Temp_allgain_x3
0x6019, //Hi-Temp_allgain_x4
0x6119, //Hi-Temp_allgain_x5
0x6237, //Hi-Temp_allgain_x6
0x6337, //Hi-Temp_allgain_x7
0x6419, //Hi-Temp_allgain_x8

0x650e, //Hi-Temp_allgain_y0
0x660d, //Hi-Temp_allgain_y1
0x670e, //Hi-Temp_allgain_y2
0x680e, //Hi-Temp_allgain_y3
0x6910, //Hi-Temp_allgain_y4
0x6a10, //Hi-Temp_allgain_y5
0x6b13, //Hi-Temp_allgain_y6
0x6c12, //Hi-Temp_allgain_y7
0x6d13, //Hi-Temp_allgain_y8
0x6e12, //Hi-Temp_allgain_y9
0x6f0e, //Hi-Temp_allgain_y10
0x7011, //Hi-Temp_allgain_y11

0x0e00, // burst end

0x03D3,
0x11FE,	// Adaptive LSC on
0x108d,	// Adaptive on //B[1] EV with Y off for HD

///////////////////////////////////////////////////////////////////////////////
// DE ~ E0 Page (DMA Outdoor)
///////////////////////////////////////////////////////////////////////////////

0x03de, //DMA DE Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Outdoor 1111 add 720p 
0x1414,
0x1500, //Outdoor 1114 add 720p 
0x1615,
0x1781, //Outdoor 1115 add 720p 
0x1816,
0x1904, //Outdoor 1116 add 720p 
0x1a17,
0x1b58, //Outdoor 1117 add 720p 
0x1c18,
0x1d30, //Outdoor 1118 add 720p 
0x1e19,
0x1f12, //Outdoor 1119 add 720p 
0x2037,
0x2100, //Outdoor 1137
0x2238,
0x2300, //Outdoor 1138
0x2439,
0x2500, //Outdoor 1139
0x263a,
0x2700, //Outdoor 113a
0x283b,
0x2900, //Outdoor 113b
0x2a3c,
0x2b00, //Outdoor 113c
0x2c3d,
0x2d00, //Outdoor 113d
0x2e3e,
0x2f00, //Outdoor 113e
0x303f,
0x3100, //Outdoor 113f
0x3240,
0x3300, //Outdoor 1140
0x3441,
0x3500, //Outdoor 1141
0x3642,
0x3700, //Outdoor 1142
0x3843,
0x3900, //Outdoor 1143
0x3a49,
0x3b06, //Outdoor 1149 add 720p 
0x3c4a,
0x3d0a, //Outdoor 114a add 720p 
0x3e4b,
0x3f12, //Outdoor 114b add 720p 
0x404c,
0x411c, //Outdoor 114c add 720p 
0x424d,
0x4324, //Outdoor 114d add 720p 
0x444e,
0x4540, //Outdoor 114e add 720p 
0x464f,
0x4780, //Outdoor 114f add 720p 
0x4850,
0x491a, //Outdoor 1150
0x4a51,
0x4b23, //Outdoor 1151
0x4c52,
0x4d2c, //Outdoor 1152
0x4e53,
0x4f3f, //Outdoor 1153
0x5054,
0x513f, //Outdoor 1154
0x5255,
0x533e, //Outdoor 1155
0x5456,
0x553c, //Outdoor 1156
0x5657,
0x573a, //Outdoor 1157
0x5858,
0x593f, //Outdoor 1158
0x5a59,
0x5b3f, //Outdoor 1159
0x5c5a,
0x5d3e, //Outdoor 115a
0x5e5b,
0x5f3a, //Outdoor 115b
0x605c,
0x6137, //Outdoor 115c
0x625d,
0x6334, //Outdoor 115d
0x645e,
0x6532, //Outdoor 115e
0x665f,
0x6730, //Outdoor 115f
0x686e,
0x691c, //Outdoor 116e
0x6a6f,
0x6b18, //Outdoor 116f
0x6c77,
0x6d2b, //Outdoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f2a, //Outdoor 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Outdoor 1179 //Bayer SP Lum Pos3
0x727a,
0x731a, //Outdoor 117a //Bayer SP Lum Pos4
0x747b,
0x751c, //Outdoor 117b //Bayer SP Lum Pos5
0x767c,
0x771a, //Outdoor 117c //Bayer SP Lum Pos6
0x787d,
0x7919, //Outdoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b17, //Outdoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1e, //Outdoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1e, //Outdoor 1180 //Bayer SP Lum Neg2
0x8081,
0x811f, //Outdoor 1181 //Bayer SP Lum Neg3
0x8282,
0x831e, //Outdoor 1182 //Bayer SP Lum Neg4
0x8483,
0x851a, //Outdoor 1183 //Bayer SP Lum Neg5
0x8684,
0x871a, //Outdoor 1184 //Bayer SP Lum Neg6
0x8885,
0x891a, //Outdoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b1a, //Outdoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d1a, //Outdoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f16, //Outdoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9116, //Outdoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9315, //Outdoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9517, //Outdoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9717, //Outdoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9917, //Outdoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b16, //Outdoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Outdoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f20, //Outdoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa123, //Outdoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa321, //Outdoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa521, //Outdoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa720, //Outdoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91b, //Outdoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab18, //Outdoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad2b,//Outdoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf2b,//Outdoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb12b,//Outdoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb32b,//Outdoor 11aa //Bayer SP Edge4
0xb4ab,
0xb52b,//Outdoor 11ab //Bayer SP Edge5
0xb6ac,
0xb72c,//Outdoor 11ac //Bayer SP Edge6
0xb8ad,
0xb931,//Outdoor 11ad //Bayer SP Edge7
0xbaae,
0xbb35,//Outdoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Outdoor 11b7 add 720p 
0xbeb8,
0xbf22, //Outdoor 11b8 add 720p 
0xc0b9,
0xc121, //Outdoor 11b9 add 720p 
0xc2ba,
0xc31e, //Outdoor 11ba add 720p 
0xc4bb,
0xc51c, //Outdoor 11bb add 720p 
0xc6bc,
0xc71a, //Outdoor 11bc add 720p 
0xc8c7,
0xc930,//Outdoor 11c7 //Bayer SP STD1
0xcac8,
0xcb30,//Outdoor 11c8 //Bayer SP STD2
0xccc9,
0xcd30,//Outdoor 11c9 //Bayer SP STD3
0xceca,
0xcf30,//Outdoor 11ca //Bayer SP STD4
0xd0cb,
0xd130,//Outdoor 11cb //Bayer SP STD5
0xd2cc,
0xd330,//Outdoor 11cc //Bayer SP STD6
0xd4cd,
0xd52d,//Outdoor 11cd //Bayer SP STD7
0xd6ce,
0xd72a,//Outdoor 11ce //Bayer SP STD8
0xd8cf,
0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb15,//Outdoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Outdoor 11d1
0xded2,
0xdf40, //Outdoor 11d2
0xe0d3,
0xe1ff, //Outdoor 11d3
0xe2d4,
0xe301, //Outdoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe50a, //Outdoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Outdoor 11d6
0xe8d7,
0xe910,//Outdoor 11d7
0xead8,
0xeb01, //Outdoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed06, //Outdoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Outdoor 11da
0xf0db,
0xf107, //Outdoor 11db
0xf2df,
0xf355, //Outdoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf536, //Outdoor 11e0
0xf6e1,
0xf77a, //Outdoor 11e1
0xf8e2,
0xf935, //Outdoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfba0, //Outdoor 11e3
0xfce4,
0xfd01, //Outdoor 11e4
0x0e00, // burst end

0x03df, //DMA DF Page
0x0e01, // burst start

0x10e5,
0x1120,//Outdoor 11e5
0x12e6,
0x1301, //Outdoor 11e6
0x14e7,
0x151a,//Outdoor 11e7
0x16e8,
0x1701, //Outdoor 11e8
0x18e9,
0x1910, //Outdoor 11e9
0x1aea,
0x1b01, //Outdoor 11ea
0x1ceb,
0x1d12, //Outdoor 11eb
0x1eef,
0x1f33, //Outdoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Outdoor 1240 add 720p 
0x2470,
0x259f, //Outdoor 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Outdoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Outdoor 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Outdoor 1277
0x2c78,
0x2d2f, //Outdoor 1278
0x2e79,
0x2f09, //Outdoor 1279
0x307a,
0x3150, //Outdoor 127a
0x327b,
0x3310, //Outdoor 127b
0x347c,
0x3550, //Outdoor 127c //skin HPF gain
0x367d,
0x3710, //Outdoor 127d
0x387f,
0x3950, //Outdoor 127f
0x3a87,
0x3b3f, //Outdoor 1287 add 720p 
0x3c88,
0x3d3f, //Outdoor 1288 add 720p 
0x3e89,
0x3f3f, //Outdoor 1289 add 720p 
0x408a,
0x413f, //Outdoor 128a add 720p 
0x428b,
0x433f, //Outdoor 128b add 720p 
0x448c,
0x453f, //Outdoor 128c add 720p 
0x468d,
0x473f, //Outdoor 128d add 720p 
0x488e,
0x493f, //Outdoor 128e add 720p 
0x4a8f,
0x4b3f, //Outdoor 128f add 720p 
0x4c90,
0x4d3f, //Outdoor 1290 add 720p 
0x4e91,
0x4f3f, //Outdoor 1291 add 720p 
0x5092,
0x513f, //Outdoor 1292 add 720p 
0x5293,
0x533f, //Outdoor 1293 add 720p 
0x5494,
0x553f, //Outdoor 1294 add 720p 
0x5695,
0x573f, //Outdoor 1295 add 720p 
0x5896,
0x593f, //Outdoor 1296 add 720p 
0x5aae,
0x5b7f, //Outdoor 12ae
0x5caf,
0x5d63,//Outdoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Outdoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Outdoor 12c3 add 720p 
0x62c4,
0x631a, //Outdoor 12c4 add 720p 
0x64c5,
0x650c, //Outdoor 12c5 add 720p 
0x66c6,
0x6791, //Outdoor 12c6
0x68c7,
0x69a4, //Outdoor 12c7
0x6ac8,
0x6b3c, //Outdoor 12c8
0x6cd0,
0x6d08, //Outdoor 12d0 add 720p 
0x6ed1,
0x6f10, //Outdoor 12d1 add 720p 
0x70d2,
0x7118, //Outdoor 12d2 add 720p 
0x72d3,
0x7320, //Outdoor 12d3 add 720p 
0x74d4,
0x7530, //Outdoor 12d4 add 720p 
0x76d5,
0x7760, //Outdoor 12d5 add 720p 
0x78d6,
0x7980, //Outdoor 12d6 add 720p 
0x7ad7,
0x7b30,//Outdoor 12d7 //CI LPF NR offset
0x7cd8,
0x7d33,//Outdoor 12d8
0x7ed9,
0x7f35,//Outdoor 12d9
0x80da,
0x8135,//Outdoor 12da
0x82db,
0x8334,//Outdoor 12db
0x84dc,
0x8530,//Outdoor 12dc
0x86dd,
0x872a,//Outdoor 12dd
0x88de,
0x8926,//Outdoor 12de
0x8ae0,
0x8b49, //Outdoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Outdoor 12e1
0x8ee2,
0x8f02, //Outdoor 12e2
0x90e3,
0x9120, //Outdoor 12e3 //PS LN graph Y1
0x92e4,
0x9320, //Outdoor 12e4 //PS LN graph Y2
0x94e5,
0x9520, //Outdoor 12e5 //PS LN graph Y3
0x96e6,
0x9720, //Outdoor 12e6 //PS LN graph Y4
0x98e7,
0x9920, //Outdoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b20, //Outdoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d20, //Outdoor 12e9 //PS DY graph Y1
0x9eea,
0x9f20, //Outdoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Outdoor 12eb //PS DY graph Y3
0xa2ec,
0xa31e, //Outdoor 12ec //PS DY graph Y4
0xa4ed,
0xa51d, //Outdoor 12ed //PS DY graph Y5
0xa6ee,
0xa720, //Outdoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Outdoor 12f0
0xaaf1,
0xab2a, //Outdoor 12f1
0xacf2,
0xad32, //Outdoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb183, //Outdoor 1310 //Y-NR ENB add  720p
0xb230,
0xb33f, //Outdoor 1330
0xb431,
0xb53f, //Outdoor 1331
0xb632,
0xb73f, //Outdoor 1332
0xb833,
0xb93f, //Outdoor 1333
0xba34,
0xbb3f, //Outdoor 1334
0xbc35,
0xbd33, //Outdoor 1335
0xbe36,
0xbf2f, //Outdoor 1336
0xc037,
0xc12e, //Outdoor 1337
0xc238,
0xc302, //Outdoor 1338
0xc440,
0xc51e, //Outdoor 1340
0xc641,
0xc722, //Outdoor 1341
0xc842,
0xc962, //Outdoor 1342
0xca43,
0xcb63, //Outdoor 1343
0xcc44,
0xcdff, //Outdoor 1344
0xce45,
0xcf04, //Outdoor 1345
0xd046,
0xd136, //Outdoor 1346
0xd247,
0xd305, //Outdoor 1347
0xd448,
0xd520, //Outdoor 1348
0xd649,
0xd702, //Outdoor 1349
0xd84a,
0xd922, //Outdoor 134a
0xda4b,
0xdb06, //Outdoor 134b
0xdc4c,
0xdd20, //Outdoor 134c
0xde83,
0xdf08, //Outdoor 1383
0xe084,
0xe108, //Outdoor 1384
0xe2b7,
0xe3fd, //Outdoor 13b7
0xe4b8,
0xe5a7, //Outdoor 13b8
0xe6b9,
0xe7fe, //Outdoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Outdoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Outdoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Outdoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Outdoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Outdoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Outdoor 1410
0xf611,
0xf7d8, //Outdoor 1411
0xf812,
0xf910, //Outdoor 1412
0xfa13,
0xfb03, //Outdoor 1413
0xfc14,
0xfd0f, //Outdoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x1015,
0x117b, //Outdoor 1415 // Y Hi filter mask 1/16
0x1216,
0x131c, //Outdoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Outdoor 1417
0x1618,
0x170c, //Outdoor 1418
0x1819,
0x190c, //Outdoor 1419
0x1a1a,
0x1b18, //Outdoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1d, //Outdoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f26, //Outdoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2124, //Outdoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2323, //Outdoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x251a, //Outdoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Outdoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Outdoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b16, //Outdoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d16, //Outdoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f22, //Outdoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3121, //Outdoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3320, //Outdoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x351a, //Outdoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3719, //Outdoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3915, //Outdoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b18, //Outdoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d1b, //Outdoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f24, //Outdoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Outdoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4324, //Outdoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x451a, //Outdoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Outdoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4910, //Outdoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b18, //Outdoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d18,//Outdoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f18, //Outdoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Outdoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x531b, //Outdoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5519, //Outdoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5719, //Outdoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x591e, //Outdoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b25, //Outdoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d25, //Outdoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f24, //Outdoor 1467 //YC2D SP Edge Gain1
0x6068,
0x6124, //Outdoor 1468 //YC2D SP Edge Gain2
0x6269,
0x6328, //Outdoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652e,//Outdoor 146a //YC2D SP Edge Gain4
0x666b,
0x6730,//Outdoor 146b //YC2D SP Edge Gain5
0x686c,
0x6931,//Outdoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b30,//Outdoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d28,//Outdoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f27, //Outdoor 1487 //YC2D SP STD Gain1
0x7088,
0x7128, //Outdoor 1488 //YC2D SP STD Gain2
0x7289,
0x732d, //Outdoor 1489 //YC2D SP STD Gain3
0x748a,
0x752d,//Outdoor 148a //YC2D SP STD Gain4
0x768b,
0x772e,//Outdoor 148b //YC2D SP STD Gain5
0x788c,
0x792b,//Outdoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b28,//Outdoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d25,//Outdoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Outdoor 1497 add 720p 
0x8098,
0x813f, //Outdoor 1498 add 720p 
0x8299,
0x833f, //Outdoor 1499 add 720p 
0x849a,
0x853f, //Outdoor 149a add 720p 
0x869b,
0x873f, //Outdoor 149b add 720p 
0x88a0,
0x893f, //Outdoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Outdoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Outdoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Outdoor 14a3 add 720p 
0x90a4,
0x913f, //Outdoor 14a4 add 720p 
0x92c9,
0x9313, //Outdoor 14c9
0x94ca,
0x9520, //Outdoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Outdoor 1A10 add 720p 
0x9a18,
0x9b3f, //Outdoor 1A18
0x9c19,
0x9d3f, //Outdoor 1A19
0x9e1a,
0x9f3f, //Outdoor 1A1a
0xa01b,
0xa13f, //Outdoor 1A1b
0xa21c,
0xa33f, //Outdoor 1A1c
0xa41d,
0xa53c, //Outdoor 1A1d
0xa61e,
0xa738, //Outdoor 1A1e
0xa81f,
0xa935, //Outdoor 1A1f
0xaa20,
0xabe7, //Outdoor 1A20 add
0xac2f,
0xadf1, //Outdoor 1A2f add
0xae32,
0xaf87, //Outdoor 1A32 add
0xb034,
0xb1d2, //Outdoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31c, //Outdoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb506,//Outdoor 1A36
0xb637,
0xb740, //Outdoor 1A37
0xb838,
0xb9ff, //Outdoor 1A38
0xba39,
0xbb2e, //Outdoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Outdoor 1A3a
0xbe3b,
0xbf01, //Outdoor 1A3b
0xc03c,
0xc10c, //Outdoor 1A3c
0xc23d,
0xc301, //Outdoor 1A3d
0xc43e,
0xc507, //Outdoor 1A3e
0xc63f,
0xc701, //Outdoor 1A3f
0xc840,
0xc90c, //Outdoor 1A40
0xca41,
0xcb01, //Outdoor 1A41
0xcc42,
0xcd07, //Outdoor 1A42
0xce43,
0xcf2b, //Outdoor 1A43
0xd04d,
0xd10e, //Outdoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd30e, //Outdoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd50e, //Outdoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd713, //Outdoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd917, //Outdoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb17, //Outdoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd17, //Outdoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf17, //Outdoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe114, //Outdoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe311, //Outdoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe510, //Outdoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Outdoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Outdoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Outdoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Outdoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Outdoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf10f, //Outdoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf30f, //Outdoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf510, //Outdoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf717, //Outdoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf919, //Outdoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb17, //Outdoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd16, //Outdoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E0 page.
0xD027,
0xD142,

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x106c,
0x1116, //Outdoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1310, //Outdoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1516, //Outdoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1715, //Outdoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Outdoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Outdoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d13, //Outdoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f13, //Outdoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2113, //Outdoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2329, //Outdoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2529, //Outdoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2729, //Outdoor 1A7f //RGB SP Edge Gain3
0x2880,
0x292f,//Outdoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b2f,//Outdoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d2f,//Outdoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f27,//Outdoor 1A83 //RGB SP Edge Gain7
0x3084,
0x3125,//Outdoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3328, //Outdoor 1A9e //RGB SP STD Gain1
0x349f,
0x3528, //Outdoor 1A9f //RGB SP STD Gain2
0x36a0,
0x372f,//Outdoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x392e,//Outdoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b2d,//Outdoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d2d,//Outdoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f29,//Outdoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4125,//Outdoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4323,//Outdoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Outdoor 1Aa7 add
0x46a8,
0x473f, //Outdoor 1Aa8 add
0x48a9,
0x493f, //Outdoor 1Aa9 add
0x4aaa,
0x4b3f, //Outdoor 1Aaa add
0x4cab,
0x4d3f, //Outdoor 1Aab add
0x4eaf,
0x4f3f, //Outdoor 1Aaf add
0x50b0,
0x513f, //Outdoor 1Ab0 add
0x52b1,
0x533f, //Outdoor 1Ab1 add
0x54b2,
0x553f, //Outdoor 1Ab2 add
0x56b3,
0x573f, //Outdoor 1Ab3 add
0x58ca,
0x5900, //Outdoor 1Aca
0x5ae3,
0x5b13, //Outdoor 1Ae3 add
0x5ce4,
0x5d13, //Outdoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610f, //Outdoor 1070 Trans Func.   130108 Outdoor transFuc Flat graph
0x6271,
0x6300, //Outdoor 1071
0x6472,
0x6500, //Outdoor 1072
0x6673,
0x6700, //Outdoor 1073
0x6874,
0x6900, //Outdoor 1074
0x6a75,
0x6b00, //Outdoor 1075
0x6c76,
0x6d40,//Outdoor 1076
0x6e77,
0x6f40,//Outdoor 1077
0x7078,
0x7100,//Outdoor 1078
0x7279,
0x7340,//Outdoor 1079
0x747a,
0x7500,//Outdoor 107a
0x767b,
0x7740,//Outdoor 107b
0x787c,
0x7900,//Outdoor 107c
0x7a7d,
0x7b07, //Outdoor 107d
0x7c7e,
0x7d0f, //Outdoor 107e
0x7e7f,
0x7f1e, //Outdoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8330, //Outdoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Outdoor 031a (for sun-spot)
0x881b,
0x898c, //Outdoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Outdoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Outdoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9102, //Outdoor 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x9529, //Outdoor 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E1 ~ E3 Page (DMA Indoor)
///////////////////////////////////////////////////////////////////////////////

0x03e1, //DMA E1 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x137f, //Indoor 1111 add 720p 
0x1414,
0x1500, //Indoor 1114 add 720p 
0x1615,
0x1781, //Indoor 1115 add 720p 
0x1816,
0x1904, //Indoor 1116 add 720p
0x1a17,
0x1b58, //Indoor 1117 add 720p 
0x1c18,
0x1d30, //Indoor 1118 add 720p 
0x1e19,
0x1f12, //Indoor 1119 add 720p 
0x2037,
0x2101, //Indoor 1137 //Pre Flat rate
0x2238,
0x2300, //Indoor 1138
0x2439,
0x25ff, //Indoor 1139
0x263a,
0x2700, //Indoor 113a
0x283b,
0x2900, //Indoor 113b
0x2a3c,
0x2b00, //Indoor 113c
0x2c3d,
0x2d20, //Indoor 113d
0x2e3e,
0x2f00, //Indoor 113e
0x303f,
0x3100, //Indoor 113f
0x3240,
0x3300, //Indoor 1140
0x3441,
0x351e, //Indoor 1141
0x3642,
0x3700, //Indoor 1142
0x3843,
0x3900, //Indoor 1143
0x3a49,
0x3b06, //Indoor 1149 add 720p 
0x3c4a,
0x3d0a, //Indoor 114a add 720p 
0x3e4b,
0x3f12, //Indoor 114b add 720p 
0x404c,
0x411c, //Indoor 114c add 720p 
0x424d,
0x4324, //Indoor 114d add 720p 
0x444e,
0x4540, //Indoor 114e add 720p 
0x464f,
0x4780, //Indoor 114f add 720p 
0x4850,
0x493f, //Indoor 1150
0x4a51,
0x4b3f, //Indoor 1151
0x4c52,
0x4d3f, //Indoor 1152
0x4e53,
0x4f3d, //Indoor 1153
0x5054,
0x513c, //Indoor 1154
0x5255,
0x5338, //Indoor 1155
0x5456,
0x5536, //Indoor 1156
0x5657,
0x5734, //Indoor 1157
0x5858,
0x593f, //Indoor 1158
0x5a59,
0x5b3f, //Indoor 1159
0x5c5a,
0x5d3e, //Indoor 115a
0x5e5b,
0x5f38, //Indoor 115b
0x605c,
0x6133, //Indoor 115c
0x625d,
0x6331, //Indoor 115d
0x645e,
0x6530, //Indoor 115e
0x665f,
0x6730, //Indoor 115f
0x686e,
0x6920, //Indoor 116e
0x6a6f,
0x6b18, //Indoor 116f
0x6c77,
0x6d16, //Indoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f16, //Indoor 1178 //Bayer SP Lum Pos2
0x7079,
0x7115, //Indoor 1179 //Bayer SP Lum Pos3
0x727a,
0x7315, //Indoor 117a //Bayer SP Lum Pos4
0x747b,
0x7511, //Indoor 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Indoor 117c //Bayer SP Lum Pos6
0x787d,
0x7910, //Indoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b10, //Indoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d11, //Indoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f11, //Indoor 1180 //Bayer SP Lum Neg2
0x8081,
0x8111, //Indoor 1181 //Bayer SP Lum Neg3
0x8282,
0x8315, //Indoor 1182 //Bayer SP Lum Neg4
0x8483,
0x8516, //Indoor 1183 //Bayer SP Lum Neg5
0x8684,
0x8716, //Indoor 1184 //Bayer SP Lum Neg6
0x8885,
0x8916, //Indoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b16, //Indoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d15, //Indoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f15, //Indoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9113, //Indoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9313, //Indoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9513, //Indoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9713, //Indoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9913, //Indoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Indoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Indoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f16, //Indoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa117, //Indoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa317, //Indoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa51a, //Indoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa71a, //Indoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91a, //Indoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab19, //Indoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad26, //Indoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf26, //Indoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb125, //Indoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb325, //Indoor 11aa //Bayer SP Edge4
0xb4ab,
0xb525, //Indoor 11ab //Bayer SP Edge5
0xb6ac,
0xb725, //Indoor 11ac //Bayer SP Edge6
0xb8ad,
0xb925, //Indoor 11ad //Bayer SP Edge7
0xbaae,
0xbb24, //Indoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Indoor 11b7 add 720p 
0xbeb8,
0xbf22, //Indoor 11b8 add 720p 
0xc0b9,
0xc121, //Indoor 11b9 add 720p 
0xc2ba,
0xc31e, //Indoor 11ba add 720p 
0xc4bb,
0xc51c, //Indoor 11bb add 720p 
0xc6bc,
0xc71a, //Indoor 11bc add 720p 
0xc8c7,
0xc920, //Indoor 11c7 //Bayer SP STD1
0xcac8,
0xcb21, //Indoor 11c8 //Bayer SP STD2
0xccc9,
0xcd22, //Indoor 11c9 //Bayer SP STD3
0xceca,
0xcf24, //Indoor 11ca //Bayer SP STD4
0xd0cb,
0xd124, //Indoor 11cb //Bayer SP STD5
0xd2cc,
0xd324, //Indoor 11cc //Bayer SP STD6
0xd4cd,
0xd520, //Indoor 11cd //Bayer SP STD7
0xd6ce,
0xd71f, //Indoor 11ce //Bayer SP STD8
0xd8cf,
0xd965, //Indoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb18, //Indoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Indoor 11d1
0xded2,
0xdf40, //Indoor 11d2
0xe0d3,
0xe1ff, //Indoor 11d3
0xe2d4,
0xe302, //Indoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe51c, //Indoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Indoor 11d6
0xe8d7,
0xe910, //Indoor 11d7
0xead8,
0xeb01, //Indoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed0e, //Indoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Indoor 11da
0xf0db,
0xf107, //Indoor 11db
0xf2df,
0xf3cc, //Indoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Indoor 11e0
0xf6e1,
0xf77a, //Indoor 11e1
0xf8e2,
0xf900, //Indoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Indoor 11e3
0xfce4,
0xfd01, //Indoor 11e4 //Bayer Flat R3 StdL
0x0e00, // burst end

0x03e2, //DMA E2 Page
0x0e01, // burst start

0x10e5,
0x112a, //Indoor 11e5 //Bayer Flat R4 StdH
0x12e6,
0x1300, //Indoor 11e6
0x14e7,
0x1500, //Indoor 11e7
0x16e8,
0x1701, //Indoor 11e8
0x18e9,
0x191d, //Indoor 11e9
0x1aea,
0x1b00, //Indoor 11ea
0x1ceb,
0x1d00, //Indoor 11eb
0x1eef,
0x1fac, //Indoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Indoor 1240 add 720p 
0x2470,
0x259f, //Indoor 1270 // Bayer Sharpness ENB add
0x2671,
0x271a, //Indoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Indoor 1272 //Bayer LPF Gain
0x2a77,
0x2b26, //Indoor 1277 //20130412
0x2c78,
0x2d2f, //Indoor 1278
0x2e79,
0x2fff, //Indoor 1279
0x307a,
0x3150, //Indoor 127a
0x327b,
0x3310, //Indoor 127b
0x347c,
0x3564, //Indoor 127c //skin HPF gain
0x367d,
0x3720, //Indoor 127d
0x387f,
0x3950, //Indoor 127f
0x3a87,
0x3b3f, //Indoor 1287 add 720p 
0x3c88,
0x3d3f, //Indoor 1288 add 720p 
0x3e89,
0x3f3f, //Indoor 1289 add 720p 
0x408a,
0x413f, //Indoor 128a add 720p 
0x428b,
0x433f, //Indoor 128b add 720p 
0x448c,
0x453f, //Indoor 128c add 720p 
0x468d,
0x473f, //Indoor 128d add 720p 
0x488e,
0x493f, //Indoor 128e add 720p 
0x4a8f,
0x4b3f, //Indoor 128f add 720p 
0x4c90,
0x4d3f, //Indoor 1290 add 720p 
0x4e91,
0x4f3f, //Indoor 1291 add 720p 
0x5092,
0x513f, //Indoor 1292 add 720p 
0x5293,
0x533f, //Indoor 1293 add 720p 
0x5494,
0x553f, //Indoor 1294 add 720p 
0x5695,
0x573f, //Indoor 1295 add 720p 
0x5896,
0x593f, //Indoor 1296 add 720p 
0x5aae,
0x5b5f, //Indoor 12ae //Bayer Flat off
0x5caf,
0x5d80, //Indoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Indoor 12c0 // CI-LPF ENB add 720p
0x60c3,
0x613c, //Indoor 12c3 add 720p 
0x62c4,
0x631a, //Indoor 12c4 add 720p 
0x64c5,
0x650c, //Indoor 12c5 add 720p 
0x66c6,
0x6791, //Indoor 12c6
0x68c7,
0x69a4, //Indoor 12c7
0x6ac8,
0x6b3c, //Indoor 12c8
0x6cd0,
0x6d08, //Indoor 12d0 add 720p 
0x6ed1,
0x6f10, //Indoor 12d1 add 720p 
0x70d2,
0x7118, //Indoor 12d2 add 720p 
0x72d3,
0x7320, //Indoor 12d3 add 720p 
0x74d4,
0x7530, //Indoor 12d4 add 720p 
0x76d5,
0x7760, //Indoor 12d5 add 720p 
0x78d6,
0x7980, //Indoor 12d6 add 720p 
0x7ad7,
0x7b38, //Indoor 12d7
0x7cd8,
0x7d30, //Indoor 12d8
0x7ed9,
0x7f2a, //Indoor 12d9
0x80da,
0x812a, //Indoor 12da
0x82db,
0x8324, //Indoor 12db
0x84dc,
0x8520, //Indoor 12dc
0x86dd,
0x871a, //Indoor 12dd
0x88de,
0x8916, //Indoor 12de
0x8ae0,
0x8b63, //Indoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Indoor 12e1
0x8ee2,
0x8f02, //Indoor 12e2
0x90e3,
0x9110, //Indoor 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Indoor 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Indoor 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Indoor 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Indoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Indoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Indoor 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Indoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Indoor 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Indoor 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Indoor 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Indoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Indoor 12f0
0xaaf1,
0xab2a, //Indoor 12f1
0xacf2,
0xad32, //Indoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb183, //Indoor 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Indoor 1330
0xb431,
0xb520, //Indoor 1331
0xb632,
0xb720, //Indoor 1332
0xb833,
0xb920, //Indoor 1333
0xba34,
0xbb20, //Indoor 1334
0xbc35,
0xbd20, //Indoor 1335
0xbe36,
0xbf20, //Indoor 1336
0xc037,
0xc120, //Indoor 1337
0xc238,
0xc302, //Indoor 1338
0xc440,
0xc518, //Indoor 1340
0xc641,
0xc736, //Indoor 1341
0xc842,
0xc962, //Indoor 1342
0xca43,
0xcb63, //Indoor 1343
0xcc44,
0xcdff, //Indoor 1344
0xce45,
0xcf04, //Indoor 1345
0xd046,
0xd145, //Indoor 1346
0xd247,
0xd305, //Indoor 1347
0xd448,
0xd565, //Indoor 1348
0xd649,
0xd702, //Indoor 1349
0xd84a,
0xd922, //Indoor 134a
0xda4b,
0xdb06, //Indoor 134b
0xdc4c,
0xdd30, //Indoor 134c
0xde83,
0xdf08, //Indoor 1383 //add 20121210
0xe084,
0xe10a, //Indoor 1384 //add 20121210
0xe2b7,
0xe3fa, //Indoor 13b7
0xe4b8,
0xe577, //Indoor 13b8
0xe6b9,
0xe7fe, //Indoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Indoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Indoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Indoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Indoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Indoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Indoor 1410
0xf611,
0xf798, //Indoor 1411
0xf812,
0xf910, //Indoor 1412
0xfa13,
0xfb03, //Indoor 1413
0xfc14,
0xfd17, //Indoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x1015,
0x117b, //Indoor 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Indoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Indoor 1417
0x1618,
0x170c, //Indoor 1418
0x1819,
0x190c, //Indoor 1419
0x1a1a,
0x1b18, //Indoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d20, //Indoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f22, //Indoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2121, //Indoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x231c, //Indoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2515, //Indoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2711, //Indoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2910, //Indoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b11, //Indoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Indoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f1a, //Indoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3115, //Indoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3314, //Indoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3513, //Indoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Indoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Indoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b14, //Indoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d15, //Indoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f22, //Indoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Indoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4321, //Indoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4520, //Indoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x471d, //Indoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x491d, //Indoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b1d, //Indoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d1d, //Indoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f19, //Indoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Indoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5316, //Indoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5515, //Indoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5714, //Indoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Indoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b13, //Indoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Indoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f2a, //Indoor 1467 //YC2D SP Edge Gain1
0x6068,
0x612b, //Indoor 1468 //YC2D SP Edge Gain2
0x6269,
0x632c, //Indoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652d, //Indoor 146a //YC2D SP Edge Gain4
0x666b,
0x672d, //Indoor 146b //YC2D SP Edge Gain5
0x686c,
0x692b, //Indoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b2d, //Indoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d2e, //Indoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f1c, //Indoor 1487 //YC2D SP STD Gain1
0x7088,
0x7120, //Indoor 1488 //YC2D SP STD Gain2
0x7289,
0x7326, //Indoor 1489 //YC2D SP STD Gain3
0x748a,
0x7526, //Indoor 148a //YC2D SP STD Gain4
0x768b,
0x7722, //Indoor 148b //YC2D SP STD Gain5
0x788c,
0x7922, //Indoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1c, //Indoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d18, //Indoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Indoor 1497 add 720p 
0x8098,
0x813f, //Indoor 1498 add 720p 
0x8299,
0x833f, //Indoor 1499 add 720p 
0x849a,
0x853f, //Indoor 149a add 720p 
0x869b,
0x873f, //Indoor 149b add 720p 
0x88a0,
0x893f, //Indoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Indoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Indoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Indoor 14a3 add 720p 
0x90a4,
0x913f, //Indoor 14a4 add 720p 
0x92c9,
0x9313, //Indoor 14c9
0x94ca,
0x9527, //Indoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Indoor 1A10 add 720p 
0x9a18,
0x9b3f, //Indoor 1A18
0x9c19,
0x9d3f, //Indoor 1A19
0x9e1a,
0x9f2a, //Indoor 1A1a
0xa01b,
0xa127, //Indoor 1A1b
0xa21c,
0xa323, //Indoor 1A1c
0xa41d,
0xa523, //Indoor 1A1d
0xa61e,
0xa723, //Indoor 1A1e
0xa81f,
0xa923, //Indoor 1A1f
0xaa20,
0xabe7, //Indoor 1A20 add 720p 
0xac2f,
0xadf1, //Indoor 1A2f add 720p 
0xae32,
0xaf87, //Indoor 1A32 add 720p 
0xb034,
0xb1d0, //Indoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Indoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Indoor 1A36
0xb637,
0xb740, //Indoor 1A37
0xb838,
0xb9ff, //Indoor 1A38
0xba39,
0xbb1d, //Indoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Indoor 1A3a //RGB Flat R2_Lum H
0xbe3b,
0xbf00, //Indoor 1A3b
0xc03c,
0xc14c, //Indoor 1A3c
0xc23d,
0xc300, //Indoor 1A3d
0xc43e,
0xc513, //Indoor 1A3e
0xc63f,
0xc700, //Indoor 1A3f
0xc840,
0xc92a, //Indoor 1A40
0xca41,
0xcb00, //Indoor 1A41
0xcc42,
0xcd17, //Indoor 1A42
0xce43,
0xcf2c, //Indoor 1A43
0xd04d,
0xd112, //Indoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Indoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd511, //Indoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd710, //Indoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd910, //Indoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Indoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd10, //Indoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf10, //Indoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe113, //Indoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe313, //Indoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Indoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe710, //Indoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe910, //Indoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Indoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed10, //Indoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef10, //Indoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Indoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Indoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf513, //Indoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf714, //Indoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf914, //Indoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb14, //Indoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd15, //Indoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E3 page.
0xD62A,
0xD7FA,

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x106c,
0x1115, //Indoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Indoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Indoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Indoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1913, //Indoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Indoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d14, //Indoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Indoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2114, //Indoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2322, //Indoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2523, //Indoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2724, //Indoor 1A7f //RGB SP Edge Gain3
0x2880,
0x2925, //Indoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b26, //Indoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d27, //Indoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f29, //Indoor 1A83 //RGB SP Edge Gain7
0x3084,
0x312b, //Indoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Indoor 1A9e //RGB SP STD Gain1
0x349f,
0x3523, //Indoor 1A9f //RGB SP STD Gain2
0x36a0,
0x3726, //Indoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x3926, //Indoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b26, //Indoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d26, //Indoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f26, //Indoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4126, //Indoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4334, //Indoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Indoor 1Aa7 add 720p 
0x46a8,
0x473f, //Indoor 1Aa8 add 720p 
0x48a9,
0x493f, //Indoor 1Aa9 add 720p 
0x4aaa,
0x4b3f, //Indoor 1Aaa add 720p 
0x4cab,
0x4d3f, //Indoor 1Aab add 720p 
0x4eaf,
0x4f3f, //Indoor 1Aaf add 720p 
0x50b0,
0x513f, //Indoor 1Ab0 add 720p 
0x52b1,
0x533f, //Indoor 1Ab1 add 720p 
0x54b2,
0x553f, //Indoor 1Ab2 add 720p 
0x56b3,
0x573f, //Indoor 1Ab3 add 720p 
0x58ca,
0x5900, //Indoor 1Aca
0x5ae3,
0x5b13, //Indoor 1Ae3 add
0x5ce4,
0x5d13, //Indoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Indoor 1070 Trans Func.   130108 Indoor transFuc Flat graph
0x6271,
0x6301, //Indoor 1071
0x6472,
0x6589, //Indoor 1072
0x6673,
0x67d4, //Indoor 1073
0x6874,
0x6900, //Indoor 1074
0x6a75,
0x6b00, //Indoor 1075
0x6c76,
0x6d40, //Indoor 1076
0x6e77,
0x6f47, //Indoor 1077
0x7078,
0x71ae, //Indoor 1078
0x7279,
0x7350, //Indoor 1079
0x747a,
0x7500, //Indoor 107a
0x767b,
0x7750, //Indoor 107b
0x787c,
0x7900, //Indoor 107c
0x7a7d,
0x7b05, //Indoor 107d
0x7c7e,
0x7d0f, //Indoor 107e
0x7e7f,
0x7f2f, //Indoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x832a, //Indoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Indoor 031a (for sun-spot)
0x881b,
0x898c, //Indoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Indoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Indoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9103, //Indoor 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x9529, //Indoor 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E4 ~ E6 Page (DMA Dark1)
///////////////////////////////////////////////////////////////////////////////

0x03e4, //DMA E4 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x13ff, //Dark1 1111 add 720p 
0x1414,
0x1500, //Dark1 1114 add 720p 
0x1615,
0x1700, //Dark1 1115 add 720p 
0x1816,
0x1900, //Dark1 1116 add 720p 
0x1a17,
0x1b1e, //Dark1 1117 add 720p 
0x1c18,
0x1d10, //Dark1 1118 add 720p 
0x1e19,
0x1f06, //Dark1 1119 add 720p 
0x2037,
0x2101, //Dark1 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark1 1138 //Pre Flat R1 LumL
0x2439,
0x2503, //Dark1 1139 //Pre Flat R1 LumH
0x263a,
0x2703, //Dark1 113a
0x283b,
0x29ff, //Dark1 113b
0x2a3c,
0x2b00, //Dark1 113c
0x2c3d,
0x2d13, //Dark1 113d
0x2e3e,
0x2f00, //Dark1 113e
0x303f,
0x3110, //Dark1 113f
0x3240,
0x3300, //Dark1 1140
0x3441,
0x3510, //Dark1 1141
0x3642,
0x3700, //Dark1 1142
0x3843,
0x3918, //Dark1 1143
0x3a49,
0x3b02, //Dark1 1149 add 720p 
0x3c4a,
0x3d04, //Dark1 114a add 720p 
0x3e4b,
0x3f07, //Dark1 114b add 720p 
0x404c,
0x410c, //Dark1 114c add 720p 
0x424d,
0x4310, //Dark1 114d add 720p 
0x444e,
0x4518, //Dark1 114e add 720p 
0x464f,
0x4720, //Dark1 114f add 720p 
0x4850,
0x491a, //Dark1 1150
0x4a51,
0x4b1c, //Dark1 1151
0x4c52,
0x4d1e, //Dark1 1152
0x4e53,
0x4f24, //Dark1 1153
0x5054,
0x5128, //Dark1 1154
0x5255,
0x5326, //Dark1 1155
0x5456,
0x5522, //Dark1 1156
0x5657,
0x571e, //Dark1 1157
0x5858,
0x593f, //Dark1 1158
0x5a59,
0x5b3f, //Dark1 1159
0x5c5a,
0x5d3f, //Dark1 115a
0x5e5b,
0x5f3f, //Dark1 115b
0x605c,
0x613f, //Dark1 115c
0x625d,
0x633f, //Dark1 115d
0x645e,
0x653f, //Dark1 115e
0x665f,
0x673f, //Dark1 115f
0x686e,
0x6910, //Dark1 116e
0x6a6f,
0x6b10, //Dark1 116f
0x6c77,
0x6d20, //Dark1 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f1e, //Dark1 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Dark1 1179 //Bayer SP Lum Pos3
0x727a,
0x7318, //Dark1 117a //Bayer SP Lum Pos4
0x747b,
0x7514, //Dark1 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Dark1 117c //Bayer SP Lum Pos6
0x787d,
0x7908, //Dark1 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b08, //Dark1 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1c, //Dark1 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1c, //Dark1 1180 //Bayer SP Lum Neg2
0x8081,
0x811c, //Dark1 1181 //Bayer SP Lum Neg3
0x8282,
0x8318, //Dark1 1182 //Bayer SP Lum Neg4
0x8483,
0x8514, //Dark1 1183 //Bayer SP Lum Neg5
0x8684,
0x8710, //Dark1 1184 //Bayer SP Lum Neg6
0x8885,
0x8908, //Dark1 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b08, //Dark1 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d20, //Dark1 118f //Bayer SP Dy Pos1
0x8e90,
0x8f1e, //Dark1 1190 //Bayer SP Dy Pos2
0x9091,
0x911c, //Dark1 1191 //Bayer SP Dy Pos3
0x9292,
0x931a, //Dark1 1192 //Bayer SP Dy Pos4
0x9493,
0x9516, //Dark1 1193 //Bayer SP Dy Pos5
0x9694,
0x9714, //Dark1 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark1 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Dark1 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d1d, //Dark1 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f1d, //Dark1 1198 //Bayer SP Dy Neg2
0xa099,
0xa11c, //Dark1 1199 //Bayer SP Dy Neg3
0xa29a,
0xa31a, //Dark1 119a //Bayer SP Dy Neg4
0xa49b,
0xa516, //Dark1 119b //Bayer SP Dy Neg5
0xa69c,
0xa714, //Dark1 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark1 119d //Bayer SP Dy Neg7
0xaa9e,
0xab10, //Dark1 119e //Bayer SP Dy Neg8
0xaca7,
0xad18, //Dark1 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark1 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark1 11a9 //Bayer SP Edge3
0xb2aa,
0xb315, //Dark1 11aa //Bayer SP Edge4
0xb4ab,
0xb512, //Dark1 11ab //Bayer SP Edge5
0xb6ac,
0xb710, //Dark1 11ac //Bayer SP Edge6
0xb8ad,
0xb910, //Dark1 11ad //Bayer SP Edge7
0xbaae,
0xbb10, //Dark1 11ae //Bayer SP Edge8
0xbcb7,
0xbd18, //Dark1 11b7 add 720p 
0xbeb8,
0xbf10, //Dark1 11b8 add 720p 
0xc0b9,
0xc108, //Dark1 11b9 add 720p 
0xc2ba,
0xc308, //Dark1 11ba add 720p 
0xc4bb,
0xc508, //Dark1 11bb add 720p 
0xc6bc,
0xc708, //Dark1 11bc add 720p 
0xc8c7,
0xc91c, //Dark1 11c7 //Bayer SP STD1
0xcac8,
0xcb1c, //Dark1 11c8 //Bayer SP STD2
0xccc9,
0xcd1c, //Dark1 11c9 //Bayer SP STD3
0xceca,
0xcf1a, //Dark1 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark1 11cb //Bayer SP STD5
0xd2cc,
0xd316, //Dark1 11cc //Bayer SP STD6
0xd4cd,
0xd514, //Dark1 11cd //Bayer SP STD7
0xd6ce,
0xd712, //Dark1 11ce //Bayer SP STD8
0xd8cf,
0xd922, //Dark1 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark1 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd04, //Dark1 11d1
0xded2,
0xdf1a, //Dark1 11d2
0xe0d3,
0xe123, //Dark1 11d3
0xe2d4,
0xe300, //Dark1 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe516, //Dark1 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark1 11d6
0xe8d7,
0xe91c, //Dark1 11d7
0xead8,
0xeb00, //Dark1 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed08, //Dark1 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark1 11da
0xf0db,
0xf10e, //Dark1 11db
0xf2df,
0xf373, //Dark1 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf504, //Dark1 11e0
0xf6e1,
0xf71a, //Dark1 11e1
0xf8e2,
0xf900, //Dark1 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfbff, //Dark1 11e3
0xfce4,
0xfd00, //Dark1 11e4
0x0e00, // burst end

0x03e5, //DMA E5 Page
0x0e01, // burst start

0x10e5,
0x1118, //Dark1 11e5
0x12e6,
0x1300, //Dark1 11e6
0x14e7,
0x1528, //Dark1 11e7
0x16e8,
0x1700, //Dark1 11e8
0x18e9,
0x1909, //Dark1 11e9
0x1aea,
0x1b00, //Dark1 11ea
0x1ceb,
0x1d14, //Dark1 11eb
0x1eef,
0x1f33, //Dark1 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2336, //Dark1 1240 add 720p 
0x2470,
0x2581, //Dark1 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x2707, //Dark1 1271 //Bayer HPF Gain
0x2872,
0x2907, //Dark1 1272 //Bayer LPF Gain
0x2a77,
0x2b00, //Dark1 1277
0x2c78,
0x2d09, //Dark1 1278
0x2e79,
0x2f2e, //Dark1 1279
0x307a,
0x3150, //Dark1 127a
0x327b,
0x3310, //Dark1 127b
0x347c,
0x3550, //Dark1 127c //skin HPF gain
0x367d,
0x3710, //Dark1 127d
0x387f,
0x3950, //Dark1 127f
0x3a87,
0x3b08, //Dark1 1287 add 720p 
0x3c88,
0x3d08, //Dark1 1288 add 720p 
0x3e89,
0x3f08, //Dark1 1289 add 720p 
0x408a,
0x410c, //Dark1 128a add 720p 
0x428b,
0x4310, //Dark1 128b add 720p 
0x448c,
0x4514, //Dark1 128c add 720p 
0x468d,
0x4718, //Dark1 128d add 720p 
0x488e,
0x491a, //Dark1 128e add 720p 
0x4a8f,
0x4b08, //Dark1 128f add 720p 
0x4c90,
0x4d0a, //Dark1 1290 add 720p 
0x4e91,
0x4f0e, //Dark1 1291 add 720p 
0x5092,
0x5112, //Dark1 1292 add 720p 
0x5293,
0x5316, //Dark1 1293 add 720p 
0x5494,
0x551a, //Dark1 1294 add 720p 
0x5695,
0x5720, //Dark1 1295 add 720p 
0x5896,
0x5920, //Dark1 1296 add 720p 
0x5aae,
0x5b20, //Dark1 12ae
0x5caf,
0x5d33, //Dark1 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark1 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x6118, //Dark1 12c3 add 720p 
0x62c4,
0x630d, //Dark1 12c4 add 720p 
0x64c5,
0x6506, //Dark1 12c5 add 720p 
0x66c6,
0x6711, //Dark1 12c6
0x68c7,
0x6911, //Dark1 12c7
0x6ac8,
0x6b04, //Dark1 12c8
0x6cd0,
0x6d02, //Dark1 12d0 add 720p 
0x6ed1,
0x6f04, //Dark1 12d1 add 720p 
0x70d2,
0x7107, //Dark1 12d2 add 720p 
0x72d3,
0x730c, //Dark1 12d3 add 720p 
0x74d4,
0x7510, //Dark1 12d4 add 720p 
0x76d5,
0x7718, //Dark1 12d5 add 720p 
0x78d6,
0x7920, //Dark1 12d6 add 720p 
0x7ad7,
0x7b29, //Dark1 12d7 //CI LPF Lum offset start
0x7cd8,
0x7d2a, //Dark1 12d8
0x7ed9,
0x7f2c, //Dark1 12d9
0x80da,
0x812b, //Dark1 12da
0x82db,
0x832a, //Dark1 12db
0x84dc,
0x8528, //Dark1 12dc
0x86dd,
0x8727, //Dark1 12dd
0x88de,
0x8927, //Dark1 12de //CI LPF Lum offset end
0x8ae0,
0x8b63, //Dark1 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark1 12e1
0x8ee2,
0x8f02, //Dark1 12e2
0x90e3,
0x9110, //Dark1 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark1 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark1 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark1 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark1 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark1 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark1 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark1 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark1 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark1 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark1 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark1 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark1 12f0
0xaaf1,
0xab2a, //Dark1 12f1
0xacf2,
0xad32, //Dark1 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb180, //Dark1 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Dark1 1330
0xb431,
0xb520, //Dark1 1331
0xb632,
0xb720, //Dark1 1332
0xb833,
0xb920, //Dark1 1333
0xba34,
0xbb20, //Dark1 1334
0xbc35,
0xbd2d, //Dark1 1335
0xbe36,
0xbf20, //Dark1 1336
0xc037,
0xc120, //Dark1 1337
0xc238,
0xc302, //Dark1 1338
0xc440,
0xc500, //Dark1 1340
0xc641,
0xc713, //Dark1 1341
0xc842,
0xc962, //Dark1 1342
0xca43,
0xcb63, //Dark1 1343
0xcc44,
0xcd7e, //Dark1 1344
0xce45,
0xcf00, //Dark1 1345
0xd046,
0xd16b, //Dark1 1346
0xd247,
0xd300, //Dark1 1347
0xd448,
0xd54a, //Dark1 1348
0xd649,
0xd700, //Dark1 1349
0xd84a,
0xd943, //Dark1 134a
0xda4b,
0xdb00, //Dark1 134b
0xdc4c,
0xdd2e, //Dark1 134c
0xde83,
0xdf08, //Dark1 1383
0xe084,
0xe10a, //Dark1 1384
0xe2b7,
0xe3ff, //Dark1 13b7
0xe4b8,
0xe5ff, //Dark1 13b8
0xe6b9,
0xe7ff, //Dark1 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ff, //Dark1 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark1 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark1 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark1 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark1 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf501, //Dark1 1410
0xf611,
0xf7d8, //Dark1 1411
0xf812,
0xf910, //Dark1 1412
0xfa13,
0xfb05, //Dark1 1413
0xfc14,
0xfd14, //Dark1 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x1015,
0x117d, //Dark1 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Dark1 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark1 1417
0x1618,
0x170c, //Dark1 1418
0x1819,
0x190c, //Dark1 1419
0x1a1a,
0x1b1c, //Dark1 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Dark1 141b //YC2D Post STD gain Neg
0x1e27,
0x1f0f, //Dark1 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2110, //Dark1 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2311, //Dark1 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2512, //Dark1 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2713, //Dark1 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark1 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b13, //Dark1 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Dark1 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f0f, //Dark1 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3110, //Dark1 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3311, //Dark1 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark1 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3713, //Dark1 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3913, //Dark1 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark1 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d10, //Dark1 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f1c, //Dark1 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x411b, //Dark1 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x431a, //Dark1 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4518, //Dark1 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4716, //Dark1 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark1 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b12, //Dark1 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d10, //Dark1 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f1a, //Dark1 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5119, //Dark1 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5318, //Dark1 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5517, //Dark1 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5716, //Dark1 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Dark1 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark1 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d10, //Dark1 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f10, //Dark1 1467 //YC2D SP Edge Gain1
0x6068,
0x6113, //Dark1 1468 //YC2D SP Edge Gain2
0x6269,
0x6313, //Dark1 1469 //YC2D SP Edge Gain3
0x646a,
0x6514, //Dark1 146a //YC2D SP Edge Gain4
0x666b,
0x6716, //Dark1 146b //YC2D SP Edge Gain5
0x686c,
0x6916, //Dark1 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b15, //Dark1 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d13, //Dark1 146e //YC2D SP Edge Gain8
0x6e87,
0x6f19, //Dark1 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Dark1 1488 //YC2D SP STD Gain2
0x7289,
0x731c, //Dark1 1489 //YC2D SP STD Gain3
0x748a,
0x751b, //Dark1 148a //YC2D SP STD Gain4
0x768b,
0x771a, //Dark1 148b //YC2D SP STD Gain5
0x788c,
0x791c, //Dark1 148c //YC2D SP STD Gain6
0x7a8d,
0x7b25, //Dark1 148d //YC2D SP STD Gain7
0x7c8e,
0x7d29, //Dark1 148e //YC2D SP STD Gain8
0x7e97,
0x7f08, //Dark1 1497 add 720p 
0x8098,
0x810c, //Dark1 1498 add 720p 
0x8299,
0x8310, //Dark1 1499 add 720p 
0x849a,
0x8510, //Dark1 149a add 720p 
0x869b,
0x8710, //Dark1 149b add 720p 
0x88a0,
0x8908, //Dark1 14a0 add 720p 
0x8aa1,
0x8b10, //Dark1 14a1 add 720p 
0x8ca2,
0x8d14, //Dark1 14a2 add 720p 
0x8ea3,
0x8f1a, //Dark1 14a3 add 720p 
0x90a4,
0x911a, //Dark1 14a4 add 720p 
0x92c9,
0x9313, //Dark1 14c9
0x94ca,
0x9520, //Dark1 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9914, //Dark1 1A10 add 720p 
0x9a18,
0x9b1f, //Dark1 1A18
0x9c19,
0x9d15, //Dark1 1A19
0x9e1a,
0x9f0a, //Dark1 1A1a
0xa01b,
0xa107, //Dark1 1A1b
0xa21c,
0xa303, //Dark1 1A1c
0xa41d,
0xa503, //Dark1 1A1d
0xa61e,
0xa703, //Dark1 1A1e
0xa81f,
0xa903, //Dark1 1A1f
0xaa20,
0xab07, //Dark1 1A20 add 720p 
0xac2f,
0xadf6, //Dark1 1A2f add 720p 
0xae32,
0xaf07, //Dark1 1A32 add 720p 
0xb034,
0xb1df, //Dark1 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31b, //Dark1 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb5ef, //Dark1 1A36
0xb637,
0xb740, //Dark1 1A37
0xb838,
0xb9ff, //Dark1 1A38
0xba39,
0xbb2e, //Dark1 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark1 1A3a
0xbe3b,
0xbf01, //Dark1 1A3b
0xc03c,
0xc10c, //Dark1 1A3c
0xc23d,
0xc301, //Dark1 1A3d
0xc43e,
0xc507, //Dark1 1A3e
0xc63f,
0xc701, //Dark1 1A3f
0xc840,
0xc90c, //Dark1 1A40
0xca41,
0xcb01, //Dark1 1A41
0xcc42,
0xcd07, //Dark1 1A42
0xce43,
0xcf2b, //Dark1 1A43
0xd04d,
0xd115, //Dark1 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd314, //Dark1 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd513, //Dark1 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark1 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd911, //Dark1 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Dark1 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd0f, //Dark1 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf0e, //Dark1 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe115, //Dark1 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe314, //Dark1 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe513, //Dark1 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark1 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe911, //Dark1 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Dark1 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed0f, //Dark1 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef0e, //Dark1 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf11e, //Dark1 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf31d, //Dark1 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf51c, //Dark1 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf71a, //Dark1 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf918, //Dark1 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb16, //Dark1 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd14, //Dark1 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E6 page.
0xDC2E,
0xDDB2,

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark1 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x131e, //Dark1 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x151d, //Dark1 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x171c, //Dark1 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x191a, //Dark1 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b18, //Dark1 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d16, //Dark1 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Dark1 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark1 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark1 1A7d //RGB SP Edge Gain1
0x247e,
0x251f, //Dark1 1A7e //RGB SP Edge Gain2
0x267f,
0x271e, //Dark1 1A7f //RGB SP Edge Gain3
0x2880,
0x291c, //Dark1 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b1a, //Dark1 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d18, //Dark1 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f14, //Dark1 1A83 //RGB SP Edge Gain7
0x3084,
0x3110, //Dark1 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Dark1 1A9e //RGB SP STD Gain1
0x349f,
0x3520, //Dark1 1A9f //RGB SP STD Gain2
0x36a0,
0x371e, //Dark1 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391c, //Dark1 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark1 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d18, //Dark1 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f14, //Dark1 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4110, //Dark1 1Aa5 //RGB SP STD Gain8
0x42a6,
0x43aa, //Dark1 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x4504, //Dark1 1Aa7 add 720p
0x46a8,
0x4706, //Dark1 1Aa8 add 720p
0x48a9,
0x4908, //Dark1 1Aa9 add 720p
0x4aaa,
0x4b09, //Dark1 1Aaa add 720p
0x4cab,
0x4d0a, //Dark1 1Aab add 720p
0x4eaf,
0x4f04, //Dark1 1Aaf add 720p
0x50b0,
0x5106, //Dark1 1Ab0 add 720p
0x52b1,
0x5308, //Dark1 1Ab1 add 720p
0x54b2,
0x550a, //Dark1 1Ab2 add 720p
0x56b3,
0x570c, //Dark1 1Ab3 add 720p
0x58ca,
0x5900, //Dark1 1Aca
0x5ae3,
0x5b12, //Dark1 1Ae3 add 720p
0x5ce4,
0x5d12, //Dark1 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark1 1070 Trans Func.   130108 Dark1 transFuc Flat graph
0x6271,
0x6306, //Dark1 1071
0x6472,
0x65be, //Dark1 1072
0x6673,
0x6799, //Dark1 1073
0x6874,
0x6900, //Dark1 1074
0x6a75,
0x6b00, //Dark1 1075
0x6c76,
0x6d20, //Dark1 1076
0x6e77,
0x6f33, //Dark1 1077
0x7078,
0x7133, //Dark1 1078
0x7279,
0x7340, //Dark1 1079
0x747a,
0x7500, //Dark1 107a
0x767b,
0x7740, //Dark1 107b
0x787c,
0x7900, //Dark1 107c
0x7a7d,
0x7b07, //Dark1 107d
0x7c7e,
0x7d0f, //Dark1 107e
0x7e7f,
0x7f1e, //Dark1 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark1 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark1 031a (for sun-spot)
0x881b,
0x897c, //Dark1 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark1 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark1 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9104, //Dark1 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x95a9, //Dark1 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E7 ~ E9 Page (DMA Dark2)
///////////////////////////////////////////////////////////////////////////////

0x03e7, //DMA E7 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x13ff, //Dark2 1111 add 720p
0x1414,
0x1500, //Dark2 1114 add 720p
0x1615,
0x1700, //Dark2 1115 add 720p
0x1816,
0x1900, //Dark2 1116 add 720p
0x1a17,
0x1b1e, //Dark2 1117 add 720p
0x1c18,
0x1d10, //Dark2 1118 add 720p
0x1e19,
0x1f06, //Dark2 1119 add 720p
0x2037,
0x2101, //Dark2 1137 //Pre Flat rate B[4:1] //04
0x2238,
0x2300, //Dark2 1138 //Pre Flat R1 LumL
0x2439,
0x2503, //Dark2 1139
0x263a,
0x2703, //Dark2 113a
0x283b,
0x29ff, //Dark2 113b
0x2a3c,
0x2b00, //Dark2 113c
0x2c3d,
0x2d13, //Dark2 113d
0x2e3e,
0x2f00, //Dark2 113e
0x303f,
0x3110, //Dark2 113f
0x3240,
0x3300, //Dark2 1140
0x3441,
0x3510, //Dark2 1141
0x3642,
0x3700, //Dark2 1142
0x3843,
0x3918, //Dark2 1143
0x3a49,
0x3b02, //Dark2 1149
0x3c4a,
0x3d04, //Dark2 114a
0x3e4b,
0x3f07, //Dark2 114b
0x404c,
0x410c, //Dark2 114c
0x424d,
0x4310, //Dark2 114d
0x444e,
0x4518, //Dark2 114e
0x464f,
0x4720, //Dark2 114f
0x4850,
0x491a, //Dark2 1150
0x4a51,
0x4b1c, //Dark2 1151
0x4c52,
0x4d1e, //Dark2 1152
0x4e53,
0x4f24, //Dark2 1153
0x5054,
0x5128, //Dark2 1154
0x5255,
0x5326, //Dark2 1155
0x5456,
0x5522, //Dark2 1156
0x5657,
0x571e, //Dark2 1157
0x5858,
0x593f, //Dark2 1158
0x5a59,
0x5b3f, //Dark2 1159
0x5c5a,
0x5d3f, //Dark2 115a
0x5e5b,
0x5f3f, //Dark2 115b
0x605c,
0x613f, //Dark2 115c
0x625d,
0x633f, //Dark2 115d
0x645e,
0x653f, //Dark2 115e
0x665f,
0x673f, //Dark2 115f
0x686e,
0x6910, //Dark2 116e
0x6a6f,
0x6b10, //Dark2 116f
0x6c77,
0x6d20, //Dark2 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f1e, //Dark2 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Dark2 1179 //Bayer SP Lum Pos3
0x727a,
0x7318, //Dark2 117a //Bayer SP Lum Pos4
0x747b,
0x7514, //Dark2 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Dark2 117c //Bayer SP Lum Pos6
0x787d,
0x7908, //Dark2 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b08, //Dark2 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1c, //Dark2 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1c, //Dark2 1180 //Bayer SP Lum Neg2
0x8081,
0x811c, //Dark2 1181 //Bayer SP Lum Neg3
0x8282,
0x8318, //Dark2 1182 //Bayer SP Lum Neg4
0x8483,
0x8514, //Dark2 1183 //Bayer SP Lum Neg5
0x8684,
0x8710, //Dark2 1184 //Bayer SP Lum Neg6
0x8885,
0x8908, //Dark2 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b08, //Dark2 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d20, //Dark2 118f //Bayer SP Dy Pos1
0x8e90,
0x8f1e, //Dark2 1190 //Bayer SP Dy Pos2
0x9091,
0x911c, //Dark2 1191 //Bayer SP Dy Pos3
0x9292,
0x931a, //Dark2 1192 //Bayer SP Dy Pos4
0x9493,
0x9516, //Dark2 1193 //Bayer SP Dy Pos5
0x9694,
0x9714, //Dark2 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark2 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Dark2 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d1d, //Dark2 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f1d, //Dark2 1198 //Bayer SP Dy Neg2
0xa099,
0xa11c, //Dark2 1199 //Bayer SP Dy Neg3
0xa29a,
0xa31a, //Dark2 119a //Bayer SP Dy Neg4
0xa49b,
0xa516, //Dark2 119b //Bayer SP Dy Neg5
0xa69c,
0xa714, //Dark2 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark2 119d //Bayer SP Dy Neg7
0xaa9e,
0xab10, //Dark2 119e //Bayer SP Dy Neg8
0xaca7,
0xad18, //Dark2 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark2 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark2 11a9 //Bayer SP Edge3
0xb2aa,
0xb315, //Dark2 11aa //Bayer SP Edge4
0xb4ab,
0xb512, //Dark2 11ab //Bayer SP Edge5
0xb6ac,
0xb710, //Dark2 11ac //Bayer SP Edge6
0xb8ad,
0xb910, //Dark2 11ad //Bayer SP Edge7
0xbaae,
0xbb10, //Dark2 11ae //Bayer SP Edge8
0xbcb7,
0xbd18, //Dark2 11b7 add 720p
0xbeb8,
0xbf10, //Dark2 11b8 add 720p
0xc0b9,
0xc108, //Dark2 11b9 add 720p
0xc2ba,
0xc308, //Dark2 11ba add 720p
0xc4bb,
0xc508, //Dark2 11bb add 720p
0xc6bc,
0xc708, //Dark2 11bc add 720p
0xc8c7,
0xc91c, //Dark2 11c7 //Bayer SP STD1
0xcac8,
0xcb1c, //Dark2 11c8 //Bayer SP STD2
0xccc9,
0xcd1c, //Dark2 11c9 //Bayer SP STD3
0xceca,
0xcf1a, //Dark2 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark2 11cb //Bayer SP STD5
0xd2cc,
0xd316, //Dark2 11cc //Bayer SP STD6
0xd4cd,
0xd514, //Dark2 11cd //Bayer SP STD7
0xd6ce,
0xd712, //Dark2 11ce //Bayer SP STD8
0xd8cf,
0xd922, //Dark2 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark2 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd04, //Dark2 11d1
0xded2,
0xdf1a, //Dark2 11d2
0xe0d3,
0xe123, //Dark2 11d3
0xe2d4,
0xe300, //Dark2 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe516, //Dark2 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark2 11d6
0xe8d7,
0xe91c, //Dark2 11d7
0xead8,
0xeb00, //Dark2 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed08, //Dark2 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark2 11da
0xf0db,
0xf10e, //Dark2 11db
0xf2df,
0xf373, //Dark2 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf504, //Dark2 11e0
0xf6e1,
0xf71a, //Dark2 11e1
0xf8e2,
0xf900, //Dark2 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfbff, //Dark2 11e3
0xfce4,
0xfd00, //Dark2 11e4
0x0e00, // burst end

0x03e8, //DMA E8 Page
0x0e01, // burst start

0x10e5,
0x1118, //Dark2 11e5
0x12e6,
0x1300, //Dark2 11e6
0x14e7,
0x1528, //Dark2 11e7
0x16e8,
0x1700, //Dark2 11e8
0x18e9,
0x1909, //Dark2 11e9
0x1aea,
0x1b00, //Dark2 11ea
0x1ceb,
0x1d14, //Dark2 11eb
0x1eef,
0x1f33, //Dark2 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2336, //Dark2 1240 add 720p
0x2470,
0x2581, //Dark2 1270 // Bayer Sharpness ENB add 720p
0x2671,
0x2707, //Dark2 1271 //Bayer HPF Gain
0x2872,
0x2907, //Dark2 1272 //Bayer LPF Gain
0x2a77,
0x2b00, //Dark2 1277
0x2c78,
0x2d09, //Dark2 1278
0x2e79,
0x2f2e, //Dark2 1279
0x307a,
0x3150, //Dark2 127a
0x327b,
0x3310, //Dark2 127b
0x347c,
0x3550, //Dark2 127c //skin HPF gain
0x367d,
0x3710, //Dark2 127d
0x387f,
0x3950, //Dark2 127f
0x3a87,
0x3b08, //Dark2 1287 add 720p
0x3c88,
0x3d08, //Dark2 1288 add 720p
0x3e89,
0x3f08, //Dark2 1289 add 720p
0x408a,
0x410c, //Dark2 128a add 720p
0x428b,
0x4310, //Dark2 128b add 720p
0x448c,
0x4514, //Dark2 128c add 720p
0x468d,
0x4718, //Dark2 128d add 720p
0x488e,
0x491a, //Dark2 128e add 720p
0x4a8f,
0x4b08, //Dark2 128f add 720p
0x4c90,
0x4d0a, //Dark2 1290 add 720p
0x4e91,
0x4f0e, //Dark2 1291 add 720p
0x5092,
0x5112, //Dark2 1292 add 720p
0x5293,
0x5316, //Dark2 1293 add 720p
0x5494,
0x551a, //Dark2 1294 add 720p
0x5695,
0x5720, //Dark2 1295 add 720p
0x5896,
0x5920, //Dark2 1296 add 720p
0x5aae,
0x5b20, //Dark2 12ae
0x5caf,
0x5d33, //Dark2 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark2 12c0 // CI-LPF ENB add 720p
0x60c3,
0x6118, //Dark2 12c3 add 720p
0x62c4,
0x630d, //Dark2 12c4 add 720p
0x64c5,
0x6506, //Dark2 12c5 add 720p
0x66c6,
0x6711, //Dark2 12c6
0x68c7,
0x6911, //Dark2 12c7
0x6ac8,
0x6b04, //Dark2 12c8
0x6cd0,
0x6d02, //Dark2 12d0 add 720p
0x6ed1,
0x6f04, //Dark2 12d1 add 720p
0x70d2,
0x7107, //Dark2 12d2 add 720p
0x72d3,
0x730c, //Dark2 12d3 add 720p
0x74d4,
0x7510, //Dark2 12d4 add 720p
0x76d5,
0x7718, //Dark2 12d5 add 720p
0x78d6,
0x7920, //Dark2 12d6 add 720p
0x7ad7,
0x7b29, //Dark2 12d7 //CI LPF Lum offset start
0x7cd8,
0x7d2a, //Dark2 12d8
0x7ed9,
0x7f2c, //Dark2 12d9
0x80da,
0x812b, //Dark2 12da
0x82db,
0x832a, //Dark2 12db
0x84dc,
0x8528, //Dark2 12dc
0x86dd,
0x8727, //Dark2 12dd
0x88de,
0x8927, //Dark2 12de //CI LPF Lum offset end
0x8ae0,
0x8b63, //Dark2 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark2 12e1
0x8ee2,
0x8f02, //Dark2 12e2
0x90e3,
0x9110, //Dark2 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark2 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark2 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark2 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark2 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark2 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark2 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark2 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark2 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark2 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark2 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark2 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark2 12f0
0xaaf1,
0xab2a, //Dark2 12f1
0xacf2,
0xad32, //Dark2 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb180, //Dark2 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Dark2 1330
0xb431,
0xb520, //Dark2 1331
0xb632,
0xb720, //Dark2 1332
0xb833,
0xb920, //Dark2 1333
0xba34,
0xbb20, //Dark2 1334
0xbc35,
0xbd2d, //Dark2 1335
0xbe36,
0xbf20, //Dark2 1336
0xc037,
0xc120, //Dark2 1337
0xc238,
0xc302, //Dark2 1338
0xc440,
0xc500, //Dark2 1340
0xc641,
0xc713, //Dark2 1341
0xc842,
0xc962, //Dark2 1342
0xca43,
0xcb63, //Dark2 1343
0xcc44,
0xcd7e, //Dark2 1344
0xce45,
0xcf00, //Dark2 1345
0xd046,
0xd16b, //Dark2 1346
0xd247,
0xd300, //Dark2 1347
0xd448,
0xd54a, //Dark2 1348
0xd649,
0xd700, //Dark2 1349
0xd84a,
0xd943, //Dark2 134a
0xda4b,
0xdb00, //Dark2 134b
0xdc4c,
0xdd2e, //Dark2 134c
0xde83,
0xdf08, //Dark2 1383
0xe084,
0xe10a, //Dark2 1384
0xe2b7,
0xe3ff, //Dark2 13b7
0xe4b8,
0xe5ff, //Dark2 13b8
0xe6b9,
0xe7ff, //Dark2 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ff, //Dark2 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark2 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark2 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark2 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark2 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf501, //Dark2 1410
0xf611,
0xf7d8, //Dark2 1411
0xf812,
0xf910, //Dark2 1412
0xfa13,
0xfb05, //Dark2 1413
0xfc14,
0xfd14, //Dark2 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x1015,
0x117d, //Dark2 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Dark2 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark2 1417
0x1618,
0x170c, //Dark2 1418
0x1819,
0x190c, //Dark2 1419
0x1a1a,
0x1b1c, //Dark2 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Dark2 141b //YC2D Post STD gain Neg
0x1e27,
0x1f0f, //Dark2 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2110, //Dark2 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2311, //Dark2 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2512, //Dark2 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2713, //Dark2 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark2 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b13, //Dark2 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Dark2 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f0f, //Dark2 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3110, //Dark2 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3311, //Dark2 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark2 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3713, //Dark2 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3913, //Dark2 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark2 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d10, //Dark2 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f1c, //Dark2 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x411b, //Dark2 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x431a, //Dark2 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4518, //Dark2 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4716, //Dark2 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark2 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b12, //Dark2 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d10, //Dark2 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f1a, //Dark2 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5119, //Dark2 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5318, //Dark2 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5517, //Dark2 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5716, //Dark2 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Dark2 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark2 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d10, //Dark2 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f10, //Dark2 1467 //YC2D SP Edge Gain1
0x6068,
0x6123, //Dark2 1468 //YC2D SP Edge Gain2
0x6269,
0x6326, //Dark2 1469 //YC2D SP Edge Gain3
0x646a,
0x6524, //Dark2 146a //YC2D SP Edge Gain4
0x666b,
0x6713, //Dark2 146b //YC2D SP Edge Gain5
0x686c,
0x691a, //Dark2 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b12, //Dark2 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d12, //Dark2 146e //YC2D SP Edge Gain8
0x6e87,
0x6f19, //Dark2 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Dark2 1488 //YC2D SP STD Gain2
0x7289,
0x731c, //Dark2 1489 //YC2D SP STD Gain3
0x748a,
0x751b, //Dark2 148a //YC2D SP STD Gain4
0x768b,
0x771a, //Dark2 148b //YC2D SP STD Gain5
0x788c,
0x791c, //Dark2 148c //YC2D SP STD Gain6
0x7a8d,
0x7b25, //Dark2 148d //YC2D SP STD Gain7
0x7c8e,
0x7d29, //Dark2 148e //YC2D SP STD Gain8
0x7e97,
0x7f08, //Dark2 1497 add 720p
0x8098,
0x810c, //Dark2 1498 add 720p
0x8299,
0x8310, //Dark2 1499 add 720p
0x849a,
0x8510, //Dark2 149a add 720p
0x869b,
0x8710, //Dark2 149b add 720p
0x88a0,
0x8908, //Dark2 14a0 add 720p
0x8aa1,
0x8b10, //Dark2 14a1 add 720p
0x8ca2,
0x8d14, //Dark2 14a2 add 720p
0x8ea3,
0x8f1a, //Dark2 14a3 add 720p
0x90a4,
0x911a, //Dark2 14a4 add 720p
0x92c9,
0x9313, //Dark2 14c9
0x94ca,
0x9520, //Dark2 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9914, //Dark2 1A10 add 720p
0x9a18,
0x9b1f, //Dark2 1A18
0x9c19,
0x9d15, //Dark2 1A19
0x9e1a,
0x9f0a, //Dark2 1A1a
0xa01b,
0xa107, //Dark2 1A1b
0xa21c,
0xa303, //Dark2 1A1c
0xa41d,
0xa503, //Dark2 1A1d
0xa61e,
0xa703, //Dark2 1A1e
0xa81f,
0xa903, //Dark2 1A1f
0xaa20,
0xab07, //Dark2 1A20 add 720p
0xac2f,
0xadf6, //Dark2 1A2f add 720p
0xae32,
0xaf07, //Dark2 1A32 add 720p
0xb034,
0xb1df, //Dark2 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31b, //Dark2 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb5ef, //Dark2 1A36
0xb637,
0xb740, //Dark2 1A37
0xb838,
0xb9ff, //Dark2 1A38
0xba39,
0xbb2e, //Dark2 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark2 1A3a
0xbe3b,
0xbf01, //Dark2 1A3b
0xc03c,
0xc10c, //Dark2 1A3c
0xc23d,
0xc301, //Dark2 1A3d
0xc43e,
0xc507, //Dark2 1A3e
0xc63f,
0xc701, //Dark2 1A3f
0xc840,
0xc90c, //Dark2 1A40
0xca41,
0xcb01, //Dark2 1A41
0xcc42,
0xcd07, //Dark2 1A42
0xce43,
0xcf2b, //Dark2 1A43
0xd04d,
0xd115, //Dark2 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd314, //Dark2 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd513, //Dark2 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark2 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd911, //Dark2 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Dark2 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd0f, //Dark2 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf0e, //Dark2 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe115, //Dark2 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe314, //Dark2 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe513, //Dark2 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark2 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe911, //Dark2 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Dark2 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed0f, //Dark2 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef0e, //Dark2 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf11e, //Dark2 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf31d, //Dark2 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf51c, //Dark2 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf71a, //Dark2 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf918, //Dark2 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb16, //Dark2 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd14, //Dark2 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E9 page.
0xE232,
0xE36A,

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark2 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x131e, //Dark2 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x151d, //Dark2 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x171c, //Dark2 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x191a, //Dark2 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b18, //Dark2 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d16, //Dark2 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Dark2 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark2 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark2 1A7d //RGB SP Edge Gain1
0x247e,
0x251f, //Dark2 1A7e //RGB SP Edge Gain2
0x267f,
0x271e, //Dark2 1A7f //RGB SP Edge Gain3
0x2880,
0x291c, //Dark2 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b1a, //Dark2 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d18, //Dark2 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f14, //Dark2 1A83 //RGB SP Edge Gain7
0x3084,
0x3110, //Dark2 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Dark2 1A9e //RGB SP STD Gain1
0x349f,
0x3520, //Dark2 1A9f //RGB SP STD Gain2
0x36a0,
0x371e, //Dark2 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391c, //Dark2 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark2 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d18, //Dark2 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f14, //Dark2 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4110, //Dark2 1Aa5 //RGB SP STD Gain8
0x42a6,
0x43aa, //Dark2 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x4504, //Dark2 1Aa7 add 720p
0x46a8,
0x4706, //Dark2 1Aa8 add 720p
0x48a9,
0x4908, //Dark2 1Aa9 add 720p
0x4aaa,
0x4b09, //Dark2 1Aaa add 720p
0x4cab,
0x4d0a, //Dark2 1Aab add 720p
0x4eaf,
0x4f04, //Dark2 1Aaf add 720p
0x50b0,
0x5106, //Dark2 1Ab0 add 720p
0x52b1,
0x5308, //Dark2 1Ab1 add 720p
0x54b2,
0x550a, //Dark2 1Ab2 add 720p
0x56b3,
0x570c, //Dark2 1Ab3 add 720p
0x58ca,
0x5900, //Dark2 1Aca
0x5ae3,
0x5b12, //Dark2 1Ae3 add 720p
0x5ce4,
0x5d12, //Dark2 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark2 1070 Trans Func.   130108 Dark2 transFuc Flat graph
0x6271,
0x6306, //Dark2 1071
0x6472,
0x65be, //Dark2 1072
0x6673,
0x6799, //Dark2 1073
0x6874,
0x6900, //Dark2 1074
0x6a75,
0x6b00, //Dark2 1075
0x6c76,
0x6d20, //Dark2 1076
0x6e77,
0x6f33, //Dark2 1077
0x7078,
0x7133, //Dark2 1078
0x7279,
0x7340, //Dark2 1079
0x747a,
0x7500, //Dark2 107a
0x767b,
0x7740, //Dark2 107b
0x787c,
0x7900, //Dark2 107c
0x7a7d,
0x7b07, //Dark2 107d
0x7c7e,
0x7d0f, //Dark2 107e
0x7e7f,
0x7f1e, //Dark2 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark2 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark2 031a (for sun-spot)
0x881b,
0x897c, //Dark2 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark2 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark2 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9105, //Dark2 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x95a9, //Dark2 1211 (20130416 for defect)

0x0e00, // burst end

//--------------------------------------------------------------------------//
// MIPI TX Setting  //PCLK 86MHz
//--------------------------------------------------------------------------//
0x0305,  // Page05
0x1100, // lvds_ctl_2 //Phone set not continuous
0x1200,  // crc_ctl
0x1300,  // serial_ctl
0x1400,  // ser_out_ctl_1
0x1500,  // dphy_fifo_ctl
0x1602,  // lvds_inout_ctl1
0x1700,  // lvds_inout_ctl2
0x1880,  // lvds_inout_ctl3
0x1900,  // lvds_inout_ctl4
0x1af0,  // lvds_time_ctl
0x1c01,  // tlpx_time_l_dp
0x1d0d,  // tlpx_time_l_dn
0x1e0c,  // hs_zero_time
0x1f0c,  // hs_trail_time
0x21b8,  // hs_sync_code
0x2200,  // frame_start_id
0x2301,  // frame_end_id
0x241e,  // long_packet_id
0x2500,  // s_pkt_wc_h
0x2600,  // s_pkt_wc_l
0x2708,  // lvds_frame_end_cnt_h
0x2800,  // lvds_frame_end_cnt_l
0x2a06,  // lvds_image_width_h
0x2b40,  // lvds_image_width_l
0x2c04,  // lvds_image_height_h
0x2db0,  // lvds_image_height_l
0x300a,  // l_pkt_wc_h  // Full = 1280 * 2 (YUV)
0x3100,  // l_pkt_wc_l
0x321c,  // clk_zero_time
0x330e,  // clk_post_time
0x3405,  // clk_prepare_time
0x3508,  // clk_trail_time
0x3601,  // clk_tlpx_time_dp
0x3708,  // clk_tlpx_time_dn
0x3907,  // lvds_bias_ctl
0x3a00,  // lvds_test_tx
0x4200,  // mipi_test_width_l
0x4300,  // mipi_test_height_l
0x4400,  // mipi_test_size_h
0x4500,  // mipi_test_hsync_st
0x4600,  // mipi_test_hblank
0x4700,  // mipi_test_vsync_st
0x4800,  // mipi_test_vsync_end
0x49ff,  // ulps_size_opt1
0x4a0a,  // ulps_size_opt2
0x4b22,  // ulps_size_opt3
0x4c41,  // hs_wakeup_size_h
0x4d20,  // hs_wakeup_size_l
0x4e00,  // mipi_int_time_h
0x4fff,  // mipi_int_time_l
0x500A,  // cntns_clk_wait_h
0x5100,  // cntns_clk_wait_l
0x5740,  // mipi_dmy_reg
0x6000,  // mipi_frame_pkt_opt
0x6108,  // line_cnt_value_h
0x6200,  // line_cnt_value_l
0x101c,  // lvds_ctl_1

///////////////////////////////////////////////////////////////////////////////
// sleep off
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0x03c0,
0x7F80,	// DMA on
0x7E01,	// DMA set

0xff01,	//delay 10ms

///////////////////////////////////////////////////////////////////////////////
// end of HD set
///////////////////////////////////////////////////////////////////////////////

};
#if 1
static SensorConfigScript sr352_recording_50Hz_30fps[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 30fps @ OPCLK 54MHz(1Line = 2200)
0x3D1B,
0x3E75,
0x3FB0,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101,	//AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90 
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On

0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};
#endif
#if 0
static SensorConfigScript sr352_recording_50Hz_25fps[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 25fps @ OPCLK 54MHz(1Line = 2200)
0x3D20,
0x3Eee,
0x3F78,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On

0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms


};


static SensorConfigScript sr352_recording_50Hz_15fps[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 15fps @ OPCLK 54MHz(1Line = 2200)
0x3D36,
0x3Eeb,
0x3F60,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90 
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On


0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};


static SensorConfigScript sr352_recording_50Hz_modeOff[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x15c0,	//Patch Weight On B[6]
0x3608,	//Max 8fps
0x3708,	//Max 8fps

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1101,	//AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

0x03c7,
0x10f0,	//AE On 50hz f0, 60hz d0
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1190,	//Fixed Mode Off

0x0300,
0x1e01, // frame update
0x0100, //sleep off

0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};
#endif
#endif
/*=================================
 *CAMERA_BRIGHTNESS_1 (1/9) M4   *
 ==================================*/
static SensorConfigScript sr352_bright_m4[] =
{
0x0310,
0x1403,
0x5d40,
0x5ef0,
};

/*=================================
 *CAMERA_BRIGHTNESS_2 (2/9) M3  *
 ==================================*/

static SensorConfigScript sr352_bright_m3[] =
{
0x0310,
0x1403,
0x5d54,
0x5ef0,
};

/*=================================
  CAMERA_BRIGHTNESS_3 (3/9) M2
  ==================================*/
static SensorConfigScript sr352_bright_m2[] =
{
0x0310,
0x1403,
0x5d67,
0x5ef0,
};

/*=================================
  CAMERA_BRIGHTNESS_4 (4/9) M1
  ==================================*/

static SensorConfigScript sr352_bright_m1[] =
{
0x0310,
0x1403,
0x5d7b,
0x5ef0,
};

/*=================================
  CAMERA_BRIGHTNESS_5 (5/9) Default
  ==================================*/
static SensorConfigScript sr352_bright_default[] =
{
0x0310,
0x1402,
0x5d80,
0x5e00,
};

/*=================================
  CAMERA_BRIGHTNESS_6 (6/9) P1
  ==================================*/
static SensorConfigScript sr352_bright_p1[] =
{
0x0310,
0x1403,
0x5d86,
0x5e70,
};

/*=================================
  CAMERA_BRIGHTNESS_7 (7/9) P2
  ==================================*/
static SensorConfigScript sr352_bright_p2[] =
{
0x0310,
0x1403,
0x5d99,
0x5e70,
};

/*=================================
  CAMERA_BRIGHTNESS_8 (8/9) P3
  ==================================*/
static SensorConfigScript sr352_bright_p3[] =
{

0x0310,
0x1403,
0x5dae,
0x5e70,
};

/*=================================
  CAMERA_BRIGHTNESS_9 (9/9) P4
  ==================================*/
static SensorConfigScript sr352_bright_p4[] =
{
0x0310,
0x1403,
0x5dc0,
0x5e70,
};

static SensorConfigScript sr352_effect_none[] =
{
0x0310,
0x1103,
0x12f0,
0x4200,
0x4300,
0x4480,
0x4580,
0x0314,
0x8020,

//{SENSOR_CONF_SCRIPT_END, {0, 0}}
};

static SensorConfigScript sr352_effect_gray[] =
{
0x0310,
0x1103,
0x12f3,
0x4200,
0x4300,
0x4480,
0x4580,
0x0314,
0x8020,

//{SENSOR_CONF_SCRIPT_END, {0, 0}}
};

static SensorConfigScript sr352_effect_negative[] =
{
0x0310,
0x1103,
0x12f8,
0x4200,
0x4300,
0x4480,
0x4580,
0x0314,
0x8020,

//{SENSOR_CONF_SCRIPT_END, {0, 0}}
};

static SensorConfigScript sr352_effect_sepia[] =
{
0x0310,
0x1103,
0x12f3,
0x4200,
0x4300,
0x4460,
0x45a3,
0x0314,
0x8020,

//{SENSOR_CONF_SCRIPT_END, {0, 0}}
};

static SensorConfigScript sr352_wb_auto[] =
{
0x03c8,
0x1052, //AWB Off
0x12e0, //Adaptive e0, manual 20
0x15c4,
0x11c1, //AWB Reset
0x10d2, //AWB On

//{SENSOR_CONF_SCRIPT_END, {0, 0}}
};

static SensorConfigScript sr352_wb_cloudy[] =
{
	0x03c8,
	0x1052, //AWB Off
	0x1220, //Adaptive e0, manual 20
	0x1504,
	0x03ca, 
	0x9506, //R Min
	0x96b0,
	0x9706, //R Max
	0x98d0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d05, //B Min
	0x9ef0,
	0x9f06, //B Max
	0xa010,
	0x03c8,
	0x11c1, //AWB Reset
	0x10d2, //AWB On
};

static SensorConfigScript sr352_wb_fluorescent[] =
{
	0x03c8,
	0x1052, //AWB Off
	0x1220, //Adaptive e0, manual 20
	0x1504,
	0x03ca, 
	0x9504, //R Min
	0x96c0,
	0x9704, //R Max
	0x98e0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d08, //B Min
	0x9eb0,
	0x9f08, //B Max
	0xa0d0,
	0x03c8,
	0x11c1, //AWB Reset
	0x10d2, //AWB On
};

static SensorConfigScript sr352_wb_incandescent[] =
{
	0x03c8,
	0x1052, //AWB Off
	0x1220, //Adaptive e0, manual 20
	0x1504,
	0x03ca, 
	0x9504, //R Min
	0x9600,
	0x9704, //R Max
	0x9820,
	0x9904, //G Min
	0x9a20,
	0x9b04, //G Max
	0x9c80,
	0x9d0a, //B Min
	0x9e20,
	0x9f0a, //B Max
	0xa060,
	0x03c8,
	0x11c1, //AWB Reset
	0x10d2, //AWB On

};

static SensorConfigScript sr352_wb_sunny[] =
{
	0x03c8,
	0x1052, //AWB Off
	0x1220, //Adaptive e0, manual 20
	0x1504,	
	0x03ca, 
	0x9505, //R Min
	0x9620,
	0x9705, //R Max
	0x9840,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d06, //B Min
	0x9ea0,
	0x9f06, //B Max
	0xa0c0,
	0x03c8,
	0x11c1, //AWB Reset
	0x10d2, //AWB On

};

/*===========================================*/
/*CAMERA_SCENE_off                         */
/*===========================================*/
static SensorConfigScript sr352_SceneOff[] =
{
	//Scene Off (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3,
	0x108f, //EV option on
	0x11fe, //Function On
	
	0x0310,
	0x1210, //Y Ofs On
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 

	0x03c7,
	0x3608, //Band100 Max 8fps
	0x3708, //Band120 Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00, //cb offset
	0x7c00, //cr offset

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00, //SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320, //HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1403, //d gain on
	
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Landscape                   */
/*===========================================*/
static SensorConfigScript sr352_Landscape[] =
{
	//Scene Landscape (FPS Auto/ISO Auto/Maxtrix/Br0/AWB Auto/Sat1/Sharp+1)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_Matrix
0x03c6,
0x9E11,	//1 Line
0x9F11,
0xA011,
0xA111,
0xA211,
0xA311,

0xA411,	//2 Line
0xA511,
0xA611,
0xA711,
0xA811,
0xA911,

0xAA11,//3 Line
0xAB12,
0xAC22,
0xAD22,
0xAE21,
0xAF11,

0xB011,//4 Line
0xB112,
0xB222,
0xB322,
0xB421,
0xB511,

0xB611,//5 Line
0xB712,
0xB822,
0xB922,
0xBA21,
0xBB11,

0xBC11,//6 Line
0xBD12,
0xBE22,
0xBF22,
0xC021,
0xC111,

0xC211,//7 Line
0xC312,
0xC422,
0xC522,
0xC621,
0xC711,

0xC811,//8 Line
0xC911,
0xCA11,
0xCB11,
0xCC11,
0xCD11,

0xCE11,//9 Line
0xCF11,
0xD011,
0xD111,
0xD211,
0xD311,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b20,
	0x7c20,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness +1
	0x03de, //DE Page(Outdoor)
	0xd9cc, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd9cc, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd9cc,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd9cc,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,	
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_Party                       */
/*===========================================*/
static SensorConfigScript sr352_Party[] =
{
	/*Party/Indoor (FPS Auto/ISO 200/Center/Br0/AWB Auto/Sat1/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x5182, //pga_max_total
	0x525c, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,


	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b20,
	0x7c20,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90 
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_sunset                      */
/*===========================================*/
static SensorConfigScript sr352_Sunset[] =
{
	//Scene Sunset (FPS Auto/ISO Auto/Center/Br0/AWB Day/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cloudy
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca, 

	0x9506, //R Min
	0x96b0,
	0x9706, //R Max
	0x98d0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d05, //B Min
	0x9ef0,
	0x9f06, //B Max
	0xa010,
  
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
  
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_Dawn                        */
/*===========================================*/
static SensorConfigScript sr352_Dawn[] =
{
	//Scene sunrise Dawn (FPS Auto/ISO Auto/Center/Br0/AWB CWF/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cwf
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca,
	0x9504, //R Min
	0x96c0,
	0x9704, //R Max
	0x98e0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d08, //B Min
	0x9eb0,
	0x9f08, //B Max
	0xa0d0,
	
	
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
  
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Fall                        */
/*===========================================*/
static SensorConfigScript sr352_Fall[] =
{
	//Scene Fall (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat2/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b40,
	0x7c40,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};


/*===========================================*/
/*CAMERA_SCENE_Nightshot	          */
/*===========================================*/
static SensorConfigScript sr352_Nightshot[] =
{

	//Scene Night (FPS Night/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms
	
	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108f, //EV option on
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1210, //Y Ofs On

	0x0320,
	0x1260, //AE Digital gain On

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x71ba, //Digital gain max 
	
	0x03c7,
	0x3604, //Max 4fps
	0x3704, //Max 4fps

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//Capture Set
	0x03D9,
	0x8C60, //DG On
	0x25FF, //Deshutter AG Max
	0x2620, //Deshutter AG Min
	0x27ba, //Deshutter DG Max
	0x2880, //Deshutter DG Min
	0x2902, //Deshutter Max 2Fps
	0x1006, //Deshutter On

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};



/*===========================================*/
/*CAMERA_SCENE_Backlight                        */
/*===========================================*/
static SensorConfigScript sr352_Backlight[] =
{
	//Scene Against (FPS Auto/ISO Auto/Spot/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

	//SSD_Spot
	0x03c6,//1 Line
	0x9E00,
	0x9F00,
	0xA000,
	0xA100,
	0xA200,
	0xA300,
	0xA400,//2 Line
	0xA500,
	0xA600,
	0xA700,
	0xA800,
	0xA900,
	0xAA00,//3 Line
	0xAB01,
	0xAC11,
	0xAD11,
	0xAE10,
	0xAF00,
	0xB000,//4 Line
	0xB101,
	0xB2ff,
	0xB3ff,
	0xB410,
	0xB500,
	0xB600,//5 Line
	0xB701,
	0xB8ff,
	0xB9ff,
	0xBA10,
	0xBB00,
	0xBC00,//6 Line
	0xBD01,
	0xBEff,
	0xBFff,
	0xC010,
	0xC100,
	0xC200,//7 Line
	0xC301,
	0xC411,
	0xC511,
	0xC610,
	0xC700,
	0xC800,//8 Line
	0xC900,
	0xCA00,
	0xCB00,
	0xCC00,
	0xCD00,
	0xCE00,//9 Line
	0xCF00,
	0xD000,
	0xD100,
	0xD200,
	0xD300,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off

	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off	
};

/*===========================================*/
/*CAMERA_SCENE_Candle                      */
/*===========================================*/
static SensorConfigScript sr352_Candle[] =
{
	//Scene candle (FPS Auto/ISO Auto/Center/Br0/AWB Day/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cloudy
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca, 

	0x9506, //R Min
	0x96b0,
	0x9706, //R Max
	0x98d0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d05, //B Min
	0x9ef0,
	0x9f06, //B Max
	0xa010,

	
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10f0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};


/*===========================================*/
/*CAMERA_SCENE_Beach      */
/*===========================================*/
static SensorConfigScript sr352_Beach[] =
{
	//Beach(FPS Auto/ISO 50/Center/Br1/AWB Auto/Sat1/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10
	
	0x03D3, 
	0x108d, //EV option off
	0x11fe, //Y target On
	
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x5120, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x3608, //Max 8fps
0x3708, //Max 8fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a10, //target offset

//Saturation
0x03d3,
0x7b20,
0x7c20,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b8, 60hz 90 

0x0300,
0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Sports              */
/*===========================================*/
static SensorConfigScript sr352_Sports[] =
{
	/*Sports (FPS Sports/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms
	
	0x03c7,
	0x1030,	//AE Off (Band Off) 50hz 30, 60hz 10

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off

	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5248, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x361e, //Max 30fps
0x371e, //Max 30fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10b0,	//AE On (Band Off) 50hz b0, 60hz 90 

0x0300,
0x0100, //sleep off

};


/*===========================================*/
/*CAMERA_SCENE_Firework                      */
/*===========================================*/
static SensorConfigScript sr352_Firework[] =
{
	/*Firework (FPS Fire/ISO 50/Center/Br0/AWB Auto/Sat0/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x3604, //Max 4fps
0x3704, //Max 4fps

//Capture Set
0x03D9,
0x8C20, //DG Off
0x2520, //Deshutter AG Max
0x2620, //Deshutter AG Min
0x2780, //Deshutter DG Max
0x2880, //Deshutter DG Min
0x2901, //Deshutter Max 1Fps
0x1006, //Deshutter On

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10f0,	//AE On (Band Off) 50hz b0, 60hz 90 

0x0300,
0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Portrait              */
/*===========================================*/
static SensorConfigScript sr352_Portrait[] =
{
	//Scene Portrait (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp-1)
0x0300,
0x0110, //frame sleep on
0xff19, //delay 250ms

	0x03c7,
	0x1070,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	

0x03c7,
0x3608, //Max 8fps
0x3708, //Max 8fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

//Sharpness -1
	0x03de, //DE Page(Outdoor)
	0xd911, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd911, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd911,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd911,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10f0,	//AE On 50hz f0, 60hz d0

0x0300,
0x0100, //sleep off

};

/*=========================================================*/
/*METERING()                                               */
/*=========================================================*/
static SensorConfigScript sr352_metering_matrix[] = {

	//Matrix weight set________________
	0x03c6, //1 line
	0x9E00,
	0x9F00,
	0xA000,
	0xA100,
	0xA200,
	0xA300,

	0xA400, //2 line
	0xA500,
	0xA600,
	0xA700,
	0xA800,
	0xA900,

	0xAA11, //3 line
	0xAB11,
	0xAC11,
	0xAD11,
	0xAE11,
	0xAF11,

	0xB011, //4 line
	0xB111,
	0xB211,
	0xB311,
	0xB411,
	0xB511,

	0xB611, //5 line
	0xB711,
	0xB811,
	0xB911,
	0xBA11,
	0xBB11,

	0xBC11, //6 line
	0xBD11,
	0xBE11,
	0xBF11,
	0xC011,
	0xC111,

	0xC211, //7 line
	0xC311,
	0xC411,
	0xC511,
	0xC611,
	0xC711,

	0xC811, //8 line
	0xC911,
	0xCA11,
	0xCB11,
	0xCC11,
	0xCD11,

	0xCE11, //9 line
	0xCF11,
	0xD011,
	0xD111,
	0xD211,
	0xD311,
	

};

static SensorConfigScript sr352_metering_center[] = {

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,


};

static SensorConfigScript sr352_metering_spot[] = {

	
	//Spot weight set________________
	0x03c6, //1 line
	0x9E00,
	0x9F00,
	0xA000,
	0xA100,
	0xA200,
	0xA300,

	0xA400, //2 line
	0xA500,
	0xA600,
	0xA700,
	0xA800,
	0xA900,

	0xAA00, //3 line
	0xAB01,
	0xAC11,
	0xAD11,
	0xAE10,
	0xAF00,

	0xB000, //4 line
	0xB101,
	0xB2ff,
	0xB3ff,
	0xB410,
	0xB500,

	0xB600, //5 line
	0xB701,
	0xB8ff,
	0xB9ff,
	0xBA10,
	0xBB00,

	0xBC00, //6 line
	0xBD01,
	0xBEff,
	0xBFff,
	0xC010,
	0xC100,

	0xC200, //7 line
	0xC301,
	0xC411,
	0xC511,
	0xC610,
	0xC700,

	0xC800, //8 line
	0xC900,
	0xCA00,
	0xCB00,
	0xCC00,
	0xCD00,

	0xCE00, //9 line
	0xCF00,
	0xD000,
	0xD100,
	0xD200,
	0xD300,
	

};

#if 1
static SensorConfigScript sr352_stream_stop[] = {
	0x03c1,
	0x1006, // ssd tranfer disable
	0xff01, 

	0x0300,
	0x0101,	//Sleep of frame sync type | Mipi frame

	0x03c1,
	0x1007, // ssd tranfer enable

	0x0300,
};
#endif
#if 0
static SensorConfigScript sr352_Init_Reg_60hz[] = {

///////////////////////////////////////////////////////////////////////////////
// Sensor Initial Start
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// mcu clock enable for bus release
///////////////////////////////////////////////////////////////////////////////
0x0326,
0x1089,
0x1080,

///////////////////////////////////////////////////////////////////////////////
// reset
///////////////////////////////////////////////////////////////////////////////
0x0300,
0x0101,
0x0107,
0x0101,

0x0daa, // ESD Check Register
0x0faa, // ESD Check Register

///////////////////////////////////////////////////////////////////////////////
// pad drive / pll setting
///////////////////////////////////////////////////////////////////////////////

0x0300,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0207,		// pclk_drive = 000b, i2c_drive = 111b
0x0c07,		// d_pad_drive = 000b, gpio_pad_drive = 111b
//////////////////////////////////////////////////////////////////
0x0725, //mode_pll1  24mhz / (5+1) = 4mhz
0x086c, //mode_pll2  isp clk = 108Mhz;
0x0985, //mode_pll3  // MIPI 4x div 1/2 // isp clk div = 1/4 //Preview
0x07A5,
0x07A5,
0x07A5,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0A60, // mode_pll4 for mipi mode
0x0Ae0, // mode_pll4 for mipi mode

0x0326,
0x1B03,		// bus clk div = 1/4

///////////////////////////////////////////////////////////////////////////////
// 7 Page(memory configuration)
///////////////////////////////////////////////////////////////////////////////
0x0307,
0x2101,	// SSD sram clock inv on
0x3345,	// bit[6]:C-NR DC ���� ����

///////////////////////////////////////////////////////////////////////////////
// mcu reset
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x1080,		// mcu reset
0x1089,		// mcu clk enable
0x1108,		// xdata clear
0x1100,		// xdata clear
0xff01,   // delay 10ms

///////////////////////////////////////////////////////////////////////////////
// opt download
///////////////////////////////////////////////////////////////////////////////

0x030A,
0x1200,	// otp clock enable

// timing for 108mhz
0x403B,	// otp cfg 1
0x4155,	// otp cfg 2
0x423B,	// otp cfg 3
0x433B,	// otp cfg 4
0x443B,	// otp cfg 5
0x452B,	// otp cfg 6
0x4671,	// otp cfg 7
0x470B,	// otp cfg 8
0x4803,	// otp cfg 9
0x496A,	// otp cfg 10
0x4A3B,	// otp cfg 11
0x4B85,	// otp cfg 12
0x4C55,	// otp cfg 13

0xff01,	//delay 10ms

// downlaod otp - system data
0x2000,	// otp addr = Otp:0000h
0x2100,	// otp addr = Otp:0000h
0x2000,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2100,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1301,	// start download system data
0x1300,	// toggle start

0xff01,   // delay 10ms

// download otp - mcu data
0x2000,	// otp addr = Otp:0080h
0x2180,	// otp addr = Otp:0080h
0x2000,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2e01,	// otp download size = 0100
0x2f00,	// otp download size = 0100
0x1801,	// link xdata to otp
0x3010,	// otp mcu buffer addr = Xdata:105Dh
0x315D,	// otp mcu buffer addr = Xdata:105Dh
0x1302,	// start download mcu data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

// download otp - dpc data
0x2001,	// otp addr = Otp:0180h
0x2180,	// otp addr = Otp:0180h
0x2001,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1801,	// link xdata to otp
0x3033,	// otp mcu buffer addr = Xdata:3384h
0x3184,	// otp mcu buffer addr = Xdata:3384h
0x1304,	// start download dpc data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

0x030A,
0x1280,	// otp clock disable

///////////////////////////////////////////////////////////////////////////////
// TAP for capture function
///////////////////////////////////////////////////////////////////////////////

0x0326,

0x1600,		// set tap address (high)
0x1700,		// set tap address (low)
0x1801,		// use tap memory

0x4002,		// set auto increment mode
0x4400,		// select rom
0x4500,		// set high address
0x4600,		// set low address

// tap code download - start
// (caution : data length must be even)
0x4290,
0x4281,
0x42f0,
0x42e0,
0x4254,
0x4207,
0x42d3,
0x4294,
0x4202,
0x4240,
0x421e,
0x4290,
0x4206,
0x42af,
0x42e0,
0x42b4,
0x4201,
0x4207,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e1,
0x4210,
0x4290,
0x4206,
0x42af,
0x42e0,
0x42d3,
0x4294,
0x4202,
0x4250,
0x4213,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e1,
0x420c,
0x4290,
0x4206,
0x428c,
0x42e4,
0x42f0,
0x42a3,
0x42f0,
0x42a3,
0x42f0,
0x42a3,
0x42f0,
0x4222,
0x4290,
0x421f,
0x4221,
0x42e0,
0x4230,
0x42e1,
0x4255,
0x4290,
0x4290,
0x42b4,
0x42e0,
0x4230,
0x42e5,
0x424e,
0x42e0,
0x4230,
0x42e1,
0x424a,
0x4290,
0x421f,
0x422d,
0x4274,
0x4208,
0x42f0,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e4,
0x421d,
0x4290,
0x4200,
0x4288,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4200,
0x4289,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4200,
0x428a,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4200,
0x428b,
0x4280,
0x421b,
0x4290,
0x4200,
0x428c,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4200,
0x428d,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4200,
0x428e,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4200,
0x428f,
0x42e0,
0x4290,
0x4206,
0x428f,
0x42f0,
0x4222,
0x4290,
0x421f,
0x422d,
0x4274,
0x4204,
0x42f0,
0x4290,
0x4206,
0x4289,
0x42e0,
0x4230,
0x42e4,
0x421d,
0x4290,
0x4206,
0x4266,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4206,
0x4267,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4206,
0x4268,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4206,
0x4269,
0x4280,
0x421b,
0x4290,
0x4206,
0x426a,
0x42e0,
0x4290,
0x4206,
0x428c,
0x42f0,
0x4290,
0x4206,
0x426b,
0x42e0,
0x4290,
0x4206,
0x428d,
0x42f0,
0x4290,
0x4206,
0x426c,
0x42e0,
0x4290,
0x4206,
0x428e,
0x42f0,
0x4290,
0x4206,
0x426d,
0x42e0,
0x4290,
0x4206,
0x428f,
0x42f0,
0x4222,
0x420a,
// tap code download - end

0x4401,		// select ram

0x16f8,		// set tap address (high)
0x1700,		// set tap address (low)

///////////////////////////////////////////////////////////////////////////////
// 0 Page
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1041, //binning + prev1
0x1190, //Fixed mode off
0x1200,
0x1328,
0x1501,
0x1700, // Clock inversion off
0x1800,
0x1d05,	//Group_frame_update
0x1E01,	//Group_frame_update_reset
0x2000,
0x2100, // preview row start set
0x2200,
0x2300, // preview col start set
0x2406, // height = 1536
0x2500,
0x2608, // width = 2048
0x2700,

///////////////////////////////////////////////////////////////////////////////
//ONE LINE SETTING
0x0300,
0x4c08, // 1Line = 2200  : 054(HBLANK) + 2146(Active Pixel)
0x4d98,

///////////////////////////////////////////////////////////////////////////////
0x5200,	//Vsync H
0x5314,	//Vsync L
///////////////////////////////////////////////////////////////////////////////

//Pixel windowing
0x8000, // bayer y start
0x8100,
0x8206, // bayer height
0x8324,
0x8400,	//pixel_col_start
0x8500,
0x8608,	//pixel_width
0x8724,

///////////////////////////////////////////////////////////////////////////////
// 1 Page
///////////////////////////////////////////////////////////////////////////////

0x0301,
0x1062,	// BLC=ON, column BLC, col_OBP DPC
0x1111,   // BLC offset ENB + Adaptive BLC ENB B[4]
0x1200,
0x1339,	// BLC(Frame BLC ofs - Column ALC ofs)+FrameALC skip
0x1400,
0x238F,	// Frame BLC avg ���� for 8 frame
0x5004, // blc height = 4
0x5144,
0x6000,
0x6100,
0x6200,
0x6300,
0x787f,	// ramp_rst_offset = 128
0x7904,	// ramp offset
0x7b04,	// ramp offset
0x7e00,

///////////////////////////////////////////////////////////////////////////////
// 2 Page
///////////////////////////////////////////////////////////////////////////////

0x0302,
0x1b80,
0x1d40,
0x2310,
0x4008,
0x418a,	// 20130213 Rev BC ver. ADC input range @ 800mv
0x460a,	// + 3.3V, -0.9V
0x4717, // 20121129 2.9V
0x481a,
0x4913,
0x54c0,
0x5540,
0x5633,
0xa002,
0xa1a8,
0xa204,
0xa379,
0xa404,
0xa5dc,
0xa608,
0xa766,
0xa802,
0xa97b,
0xaa03,
0xab4f,
0xac03,
0xada0,
0xae05,
0xaf43,

///////////////////////////////////////////////////////////////////////////////
// 3 Page
///////////////////////////////////////////////////////////////////////////////

0x0303,
0x1a06, // cds_s1
0x1b7c,
0x1c02,
0x1d88,
0x1e06,
0x1f7c,
0x4200,
0x43b0,
0x4601,
0x4700,
0x4a00,
0x4bae,
0x4e00,
0x4fae,
0x5200,
0x53aa,
0x5600,
0x57aa,
0x5A00,
0x5baa,
0x6A00,
0x6Bf8,
0x7206, // s_addr_cut
0x7390,
0x7806, // rx half_rst
0x798b,
0x7A06,
0x7B95,
0x7C06,
0x7D8b,
0x7E06,
0x7F95,
0x8406, // tx half_rst
0x858b,
0x8606,
0x8795,
0x8806,
0x898b,
0x8A06,
0x8B95,
0x9206, // sx
0x9381,
0x9606,
0x9781,
0x9806, // sxb
0x9981,
0x9c06,
0x9d81,

0xb601, // --------------> s_hb_cnt_hold = 500
0xb7f4,
0xc000, // i_addr_mux_prev
0xc1b4,
0xc200,
0xc3f4,
0xc400,
0xc5b4,
0xc600,
0xc7f4,
0xc800, // i_addr_cut_prev
0xc9b8,
0xca00,
0xcbf0,
0xcc00,
0xcdb8,
0xce00,
0xcff0,
0xd000, // Rx_exp_prev
0xd1ba,
0xd200,
0xd3ee,
0xd400,
0xd5ba,
0xd600,
0xd7ee,
0xd800, // Tx_exp_prev
0xd9bc,
0xdA00,
0xdBec,
0xdC00,
0xdDbc,
0xdE00,
0xdFec,

0xe000,
0xe120,
0xfc06, // clamp_sig
0xfd78,

///////////////////////////////////////////////////////////////////////////////
// 4 Page
///////////////////////////////////////////////////////////////////////////////

0x0304,
0x1003,	//Ramp multiple sampling

0x5a06, // cds_pxl_smpl
0x5b78,
0x5e06,
0x5f78,
0x6206,
0x6378,

///////////////////////////////////////////////////////////////////////////////
// mcu start
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x6200,	// normal mode start
0x6500,	// watchdog disable
0x1009,	// mcu reset release
//Analog setting ���Ŀ� MCU�� reset ��Ŵ.

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1001, // otp_dpc_ctl
0x1111, //Preview1 0410
0x1202, //Preview1 0410

///////////////////////////////////////////////////////////////////////////////
// 15 Page (LSC)
///////////////////////////////////////////////////////////////////////////////

0x0315,
0x1000,	// LSC OFF
0x1100, //gap y disable

///////////////////////////////////////////////////////////////////////////////
// set lsc parameter
///////////////////////////////////////////////////////////////////////////////

0x030a,
0x1901,

0x1180, // B[7] LSC burst mode ENB

0x0326,
0x4002,	// auto increment enable
0x4401,
0x45a3,	// LSC bank0 start addr H
0x4600,	// LSC bank0 start addr L

//LSC G channel reg________________________ 20130422 LSC Blending DNP 90_CWF 5_TL84 5

0x0e01, //BURST_START

0x423c, //G Value
0x4213,
0x42af,
0x423d,
0x4263,
0x42b1,
0x4237,
0x42a3,
0x423d,
0x4230,
0x4202,
0x42cf,
0x422b,
0x4222,
0x42b0,
0x422c,
0x42d2,
0x42fe,
0x4233,
0x42f3,
0x427e,
0x423b,
0x4273,
0x42d7,
0x423a,
0x42d3,
0x42e1,
0x4237,
0x4293,
0x4283,
0x4238,
0x42d3,
0x4260,
0x4231,
0x42e2,
0x42d4,
0x4228,
0x42d2,
0x4257,
0x4223,
0x4292,
0x4237,
0x4225,
0x4262,
0x428f,
0x422d,
0x4283,
0x4224,
0x4236,
0x4293,
0x4296,
0x4239,
0x42b3,
0x42a0,
0x4236,
0x4293,
0x4257,
0x4234,
0x4253,
0x4210,
0x422c,
0x4212,
0x426b,
0x4221,
0x42a1,
0x42df,
0x421b,
0x42f1,
0x42bf,
0x421d,
0x42f2,
0x4220,
0x4227,
0x4222,
0x42cb,
0x4231,
0x42b3,
0x4255,
0x4238,
0x4293,
0x42bd,
0x4234,
0x42b3,
0x422a,
0x4230,
0x42a2,
0x42c8,
0x4226,
0x4272,
0x4201,
0x421a,
0x4261,
0x4264,
0x4214,
0x4221,
0x4244,
0x4216,
0x4281,
0x42ab,
0x4220,
0x42a2,
0x4274,
0x422d,
0x4283,
0x4222,
0x4236,
0x42f3,
0x42bd,
0x4233,
0x4253,
0x4209,
0x422d,
0x42d2,
0x428c,
0x4221,
0x4281,
0x42a5,
0x4214,
0x4230,
0x42fd,
0x420d,
0x42a0,
0x42dc,
0x4210,
0x4241,
0x424c,
0x421b,
0x4222,
0x4228,
0x4229,
0x42f2,
0x42fb,
0x4235,
0x4273,
0x42b3,
0x4231,
0x4262,
0x42e1,
0x422a,
0x42c2,
0x4249,
0x421c,
0x4291,
0x424d,
0x420e,
0x4270,
0x42a2,
0x4207,
0x42d0,
0x4280,
0x420a,
0x4270,
0x42f2,
0x4215,
0x42d1,
0x42dc,
0x4225,
0x42e2,
0x42cc,
0x4233,
0x4243,
0x429c,
0x4230,
0x4292,
0x42cb,
0x4228,
0x42c2,
0x421f,
0x4219,
0x4251,
0x4214,
0x420a,
0x42a0,
0x4263,
0x4204,
0x4200,
0x4243,
0x4206,
0x42b0,
0x42b7,
0x4212,
0x4241,
0x42a8,
0x4223,
0x4242,
0x42b0,
0x4231,
0x42f3,
0x428e,
0x422f,
0x42b2,
0x42b5,
0x4227,
0x4201,
0x42fa,
0x4216,
0x42b0,
0x42e7,
0x4207,
0x42c0,
0x4236,
0x4201,
0x4230,
0x4216,
0x4203,
0x42e0,
0x428a,
0x420f,
0x4271,
0x427f,
0x4220,
0x42f2,
0x4295,
0x4230,
0x42a3,
0x4280,
0x422f,
0x42a2,
0x42b2,
0x4226,
0x4291,
0x42ef,
0x4215,
0x42e0,
0x42d8,
0x4206,
0x42d0,
0x4226,
0x4200,
0x4240,
0x4206,
0x4202,
0x42f0,
0x427a,
0x420e,
0x4271,
0x4270,
0x4220,
0x4242,
0x428f,
0x4230,
0x4293,
0x4283,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422f,
0x4242,
0x42ad,
0x4226,
0x4261,
0x42ec,
0x4215,
0x42c0,
0x42d6,
0x4206,
0x42c0,
0x4224,
0x4200,
0x4230,
0x4205,
0x4202,
0x42e0,
0x4279,
0x420e,
0x4261,
0x426d,
0x4220,
0x4222,
0x428d,
0x4230,
0x4253,
0x427e,
0x4230,
0x4202,
0x42bd,
0x4227,
0x42a2,
0x4203,
0x4217,
0x4260,
0x42f0,
0x4208,
0x4260,
0x4240,
0x4201,
0x42d0,
0x4220,
0x4204,
0x4280,
0x4294,
0x4210,
0x4211,
0x4287,
0x4221,
0x4292,
0x42a0,
0x4231,
0x4293,
0x4291,
0x4230,
0x4292,
0x42ce,
0x4229,
0x4242,
0x4226,
0x4219,
0x42d1,
0x421b,
0x420b,
0x4210,
0x426a,
0x4204,
0x4280,
0x424c,
0x4207,
0x4230,
0x42bf,
0x4212,
0x42b1,
0x42ae,
0x4223,
0x4292,
0x42b8,
0x4232,
0x42a3,
0x429c,
0x4231,
0x42e2,
0x42ee,
0x422b,
0x42e2,
0x425d,
0x421d,
0x42e1,
0x4261,
0x420f,
0x4270,
0x42b1,
0x4208,
0x42f0,
0x4291,
0x420b,
0x42a1,
0x4206,
0x4217,
0x4211,
0x42ed,
0x4226,
0x42f2,
0x42e3,
0x4234,
0x42f3,
0x42bb,
0x4233,
0x4263,
0x420e,
0x422e,
0x4262,
0x4296,
0x4222,
0x4251,
0x42b1,
0x4214,
0x42e1,
0x4209,
0x420e,
0x4270,
0x42ea,
0x4211,
0x4211,
0x425b,
0x421b,
0x42f2,
0x4233,
0x422a,
0x4273,
0x420b,
0x4237,
0x4223,
0x42d9,
0x4235,
0x4213,
0x4235,
0x4231,
0x4292,
0x42da,
0x4227,
0x42b2,
0x4214,
0x421b,
0x4291,
0x4278,
0x4215,
0x4271,
0x425a,
0x4218,
0x4201,
0x42c5,
0x4222,
0x4212,
0x4287,
0x422e,
0x42a3,
0x4241,
0x4239,
0x42e3,
0x42fc,
0x4234,
0x4213,
0x423a,
0x4233,
0x4223,
0x4201,
0x422b,
0x4282,
0x4264,
0x4221,
0x4271,
0x42df,
0x421c,
0x4211,
0x42c4,
0x421e,
0x4272,
0x4223,
0x4227,
0x4212,
0x42c4,
0x4231,
0x4243,
0x425f,
0x423a,
0x4293,
0x42f3,
0x4231,
0x42e3,
0x423a,
0x4235,
0x4263,
0x4231,
0x422f,
0x4232,
0x42b1,
0x4227,
0x4222,
0x4242,
0x4222,
0x4282,
0x422b,
0x4224,
0x4292,
0x427e,
0x422c,
0x4213,
0x4204,
0x4234,
0x42a3,
0x428a,
0x423a,
0x42a3,
0x42c9,
0x4234,
0x4233,
0x423b,
0x4237,
0x42b3,
0x4261,
0x4232,
0x42e2,
0x42fe,
0x422c,
0x42c2,
0x42a5,
0x4229,
0x4202,
0x4291,
0x422a,
0x42b2,
0x42d9,
0x4231,
0x4203,
0x4244,
0x4237,
0x42f3,
0x42b5,
0x423a,
0x42a3,
0x42f4,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x425f, //R Value
0x42a5,
0x42d8,
0x4261,
0x4235,
0x42e5,
0x4258,
0x42e5,
0x4229,
0x424c,
0x4244,
0x4273,
0x4244,
0x42e4,
0x424d,
0x4248,
0x4244,
0x42d9,
0x4254,
0x4245,
0x42af,
0x4260,
0x42c6,
0x423c,
0x425f,
0x4276,
0x4235,
0x425b,
0x4235,
0x425c,
0x4256,
0x4295,
0x4222,
0x424b,
0x4254,
0x423a,
0x423c,
0x4283,
0x4272,
0x4234,
0x4283,
0x424a,
0x4238,
0x4223,
0x42df,
0x4245,
0x4294,
0x42db,
0x4255,
0x4205,
0x429b,
0x4259,
0x4285,
0x42f8,
0x425b,
0x4265,
0x423a,
0x4250,
0x42e4,
0x42b0,
0x4242,
0x42b3,
0x429b,
0x4231,
0x42c2,
0x42c1,
0x4229,
0x4212,
0x4297,
0x422d,
0x4203,
0x4234,
0x423b,
0x42e4,
0x4257,
0x424e,
0x4245,
0x424b,
0x4259,
0x4236,
0x422b,
0x4259,
0x4265,
0x4203,
0x424b,
0x42f4,
0x4243,
0x4239,
0x42f2,
0x42f7,
0x4226,
0x4272,
0x4204,
0x421d,
0x4231,
0x42d9,
0x4221,
0x4232,
0x4280,
0x4231,
0x42a3,
0x42d2,
0x4247,
0x42d5,
0x4205,
0x4257,
0x42a6,
0x423e,
0x4257,
0x42f4,
0x42d4,
0x4247,
0x4293,
0x42e3,
0x4232,
0x4202,
0x4266,
0x421c,
0x42e1,
0x4268,
0x4213,
0x4261,
0x423c,
0x4217,
0x4291,
0x42eb,
0x4228,
0x42d3,
0x4255,
0x4242,
0x4204,
0x42c9,
0x4255,
0x4246,
0x422f,
0x4255,
0x4204,
0x4298,
0x4242,
0x42f3,
0x427c,
0x422a,
0x4281,
0x42e4,
0x4214,
0x4290,
0x42e4,
0x420b,
0x4240,
0x42b9,
0x420f,
0x4251,
0x4267,
0x4220,
0x42e2,
0x42e0,
0x423b,
0x42e4,
0x4282,
0x4252,
0x4236,
0x4214,
0x4253,
0x42d4,
0x4275,
0x423f,
0x42c3,
0x4237,
0x4225,
0x4241,
0x4289,
0x420e,
0x42e0,
0x428b,
0x4205,
0x42b0,
0x4260,
0x4209,
0x42c1,
0x420d,
0x421b,
0x4252,
0x428e,
0x4237,
0x4294,
0x4253,
0x4250,
0x4256,
0x4206,
0x4252,
0x4264,
0x4254,
0x423d,
0x4223,
0x4201,
0x4221,
0x4291,
0x424c,
0x420b,
0x4210,
0x424c,
0x4201,
0x42b0,
0x4221,
0x4205,
0x42e0,
0x42ce,
0x4217,
0x4262,
0x424f,
0x4234,
0x4224,
0x4229,
0x424e,
0x4255,
0x42f1,
0x4252,
0x4254,
0x424e,
0x423c,
0x4272,
0x42ed,
0x4220,
0x4231,
0x4236,
0x4209,
0x4280,
0x4233,
0x4200,
0x4250,
0x4208,
0x4204,
0x4250,
0x42b7,
0x4215,
0x42e2,
0x4237,
0x4232,
0x42f4,
0x421d,
0x424d,
0x42e5,
0x42ef,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4252,
0x4244,
0x424d,
0x423c,
0x4262,
0x42ec,
0x4220,
0x4231,
0x4234,
0x4209,
0x4280,
0x4232,
0x4200,
0x4240,
0x4207,
0x4204,
0x4240,
0x42b5,
0x4215,
0x42b2,
0x4235,
0x4232,
0x42d4,
0x421c,
0x424d,
0x42f5,
0x42f2,
0x4253,
0x4234,
0x4262,
0x423e,
0x4203,
0x420d,
0x4222,
0x4261,
0x4258,
0x420b,
0x42b0,
0x4256,
0x4202,
0x4260,
0x422c,
0x4206,
0x4270,
0x42d8,
0x4217,
0x42f2,
0x4258,
0x4234,
0x42d4,
0x4235,
0x424f,
0x4246,
0x4203,
0x4254,
0x4234,
0x4280,
0x4240,
0x42e3,
0x4247,
0x4226,
0x4271,
0x4299,
0x420f,
0x42b0,
0x4296,
0x4206,
0x4270,
0x426c,
0x420a,
0x4281,
0x4217,
0x421b,
0x42e2,
0x4296,
0x4238,
0x4224,
0x425c,
0x4250,
0x42c6,
0x420c,
0x4256,
0x4254,
0x42b1,
0x4244,
0x42d3,
0x429c,
0x422c,
0x4292,
0x4203,
0x4216,
0x4220,
0x42fd,
0x420c,
0x42c0,
0x42d1,
0x4210,
0x42b1,
0x427f,
0x4222,
0x4252,
0x42f5,
0x423d,
0x4244,
0x429c,
0x4253,
0x42d6,
0x422f,
0x4258,
0x4204,
0x42e3,
0x4249,
0x4273,
0x42fe,
0x4233,
0x42d2,
0x4282,
0x421e,
0x42a1,
0x4282,
0x4215,
0x4201,
0x4256,
0x4219,
0x4212,
0x4203,
0x422a,
0x4253,
0x4267,
0x4243,
0x4234,
0x42e0,
0x4257,
0x4246,
0x4258,
0x425a,
0x4255,
0x421d,
0x424e,
0x4254,
0x426e,
0x423c,
0x42b3,
0x4222,
0x4229,
0x4222,
0x422f,
0x4220,
0x4202,
0x4206,
0x4223,
0x42e2,
0x42ae,
0x4234,
0x4263,
0x42f5,
0x424a,
0x4205,
0x4236,
0x425b,
0x4216,
0x427d,
0x4258,
0x42e5,
0x422a,
0x4251,
0x4254,
0x42be,
0x4244,
0x4203,
0x42b8,
0x4233,
0x42c2,
0x42e5,
0x422b,
0x4292,
0x42c0,
0x422f,
0x4263,
0x4257,
0x423d,
0x42c4,
0x426c,
0x424f,
0x4295,
0x426e,
0x425c,
0x4296,
0x4275,
0x4255,
0x42c5,
0x4226,
0x4255,
0x4245,
0x4213,
0x424b,
0x4234,
0x4249,
0x423e,
0x4213,
0x4294,
0x4237,
0x4203,
0x4277,
0x423a,
0x4294,
0x4201,
0x4247,
0x4224,
0x42ea,
0x4255,
0x42e5,
0x42b8,
0x425c,
0x4276,
0x423a,
0x4258,
0x4275,
0x427c,
0x425e,
0x4245,
0x42b9,
0x4257,
0x4255,
0x4229,
0x424d,
0x4254,
0x4293,
0x4247,
0x4274,
0x427e,
0x424a,
0x42c4,
0x42fb,
0x4255,
0x4295,
0x42b7,
0x4261,
0x4236,
0x4252,
0x4261,
0x42f6,
0x4270,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4240, //B Value
0x4263,
0x42cb,
0x423e,
0x4263,
0x42cc,
0x4239,
0x4283,
0x426e,
0x4233,
0x42c3,
0x4212,
0x422f,
0x42f2,
0x42f7,
0x4230,
0x42d3,
0x422a,
0x4235,
0x42b3,
0x428a,
0x423b,
0x42d3,
0x42e6,
0x423b,
0x42e4,
0x4218,
0x4239,
0x4253,
0x4285,
0x4237,
0x4253,
0x4252,
0x4231,
0x42a2,
0x42e1,
0x422a,
0x4272,
0x427a,
0x4226,
0x4232,
0x425d,
0x4227,
0x4232,
0x429a,
0x422d,
0x4243,
0x4210,
0x4234,
0x42a3,
0x4279,
0x4239,
0x4223,
0x42ab,
0x4237,
0x42b3,
0x423f,
0x4230,
0x4242,
0x42d9,
0x4229,
0x42d2,
0x4255,
0x4221,
0x4221,
0x42e2,
0x421c,
0x4271,
0x42c3,
0x421d,
0x42a2,
0x420a,
0x4224,
0x42d2,
0x4296,
0x422d,
0x4263,
0x420b,
0x4236,
0x4253,
0x42bf,
0x4235,
0x4243,
0x420c,
0x422c,
0x4242,
0x4290,
0x4224,
0x4211,
0x42eb,
0x421a,
0x4201,
0x4266,
0x4214,
0x42a1,
0x4249,
0x4216,
0x4231,
0x4298,
0x421e,
0x4262,
0x423c,
0x4229,
0x4212,
0x42d5,
0x4233,
0x42a3,
0x42a0,
0x4233,
0x4272,
0x42e5,
0x4229,
0x4242,
0x4255,
0x421f,
0x4271,
0x4294,
0x4214,
0x4201,
0x4203,
0x420e,
0x4250,
0x42e4,
0x4210,
0x4231,
0x423d,
0x4219,
0x4211,
0x42f3,
0x4225,
0x4262,
0x42aa,
0x4231,
0x4293,
0x4288,
0x4231,
0x4242,
0x42b8,
0x4225,
0x42c2,
0x420e,
0x421a,
0x4281,
0x423e,
0x420e,
0x4260,
0x42a8,
0x4208,
0x42a0,
0x428b,
0x420a,
0x4290,
0x42e7,
0x4214,
0x4201,
0x42aa,
0x4221,
0x4282,
0x427a,
0x422f,
0x4233,
0x426c,
0x422f,
0x4242,
0x4299,
0x4223,
0x42d1,
0x42e3,
0x4217,
0x4201,
0x4204,
0x420a,
0x4280,
0x426a,
0x4204,
0x42c0,
0x424d,
0x4206,
0x42e0,
0x42af,
0x4210,
0x42a1,
0x427a,
0x421e,
0x42d2,
0x425c,
0x422d,
0x4293,
0x4255,
0x422e,
0x4252,
0x4280,
0x4221,
0x42b1,
0x42bb,
0x4214,
0x4260,
0x42d4,
0x4207,
0x4290,
0x423a,
0x4201,
0x42c0,
0x421e,
0x4203,
0x42f0,
0x4280,
0x420d,
0x42f1,
0x4250,
0x421c,
0x42c2,
0x423f,
0x422c,
0x4213,
0x4243,
0x422d,
0x4272,
0x4272,
0x4220,
0x42d1,
0x42a9,
0x4213,
0x4220,
0x42c2,
0x4206,
0x4250,
0x4226,
0x4200,
0x4290,
0x420b,
0x4202,
0x42d0,
0x426f,
0x420c,
0x42d1,
0x4241,
0x421b,
0x42f2,
0x4238,
0x422b,
0x42d3,
0x4242,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422d,
0x4212,
0x426c,
0x4220,
0x4281,
0x42a5,
0x4212,
0x42c0,
0x42be,
0x4206,
0x4200,
0x4222,
0x4200,
0x4220,
0x4205,
0x4202,
0x4290,
0x426c,
0x420c,
0x42a1,
0x423e,
0x421b,
0x42d2,
0x4235,
0x422b,
0x4293,
0x423d,
0x422e,
0x4202,
0x427d,
0x4221,
0x42b1,
0x42ba,
0x4214,
0x4250,
0x42d5,
0x4207,
0x42a0,
0x423b,
0x4201,
0x42b0,
0x421f,
0x4204,
0x4210,
0x4285,
0x420e,
0x4251,
0x4258,
0x421d,
0x4252,
0x424c,
0x422c,
0x42c3,
0x424c,
0x422e,
0x4272,
0x428d,
0x4223,
0x4231,
0x42db,
0x4216,
0x4280,
0x42fb,
0x420a,
0x4200,
0x4263,
0x4204,
0x4250,
0x4248,
0x4206,
0x42b0,
0x42ad,
0x4210,
0x42a1,
0x427c,
0x421f,
0x4242,
0x4262,
0x422e,
0x4213,
0x425f,
0x4230,
0x4292,
0x42b3,
0x4225,
0x42d2,
0x420d,
0x421a,
0x4231,
0x423b,
0x420e,
0x4210,
0x42a3,
0x4208,
0x4270,
0x428a,
0x420a,
0x42d0,
0x42f0,
0x4214,
0x42b1,
0x42b7,
0x4222,
0x4282,
0x428b,
0x4230,
0x4273,
0x4282,
0x4231,
0x42d2,
0x42d1,
0x4228,
0x4262,
0x4240,
0x421e,
0x4221,
0x4281,
0x4212,
0x42d0,
0x42f2,
0x420d,
0x4250,
0x42db,
0x420f,
0x42c1,
0x423b,
0x4219,
0x4231,
0x42f7,
0x4225,
0x42d2,
0x42b3,
0x4232,
0x42a3,
0x42a0,
0x4234,
0x4222,
0x42fd,
0x422b,
0x4282,
0x4282,
0x4223,
0x4201,
0x42da,
0x4218,
0x42c1,
0x4256,
0x4213,
0x42c1,
0x4240,
0x4216,
0x4211,
0x429c,
0x421e,
0x42c2,
0x4245,
0x4229,
0x42e2,
0x42ec,
0x4235,
0x42d3,
0x42ce,
0x4234,
0x4203,
0x4208,
0x422d,
0x4202,
0x42a7,
0x4226,
0x4242,
0x421e,
0x421d,
0x42c1,
0x42ad,
0x4219,
0x4261,
0x429c,
0x421b,
0x4271,
0x42eb,
0x4223,
0x4202,
0x4279,
0x422c,
0x4213,
0x4204,
0x4236,
0x4273,
0x42cb,
0x4233,
0x42c3,
0x422f,
0x4232,
0x4222,
0x42ff,
0x422c,
0x4242,
0x428b,
0x4225,
0x4222,
0x422a,
0x4221,
0x4262,
0x421d,
0x4223,
0x4262,
0x4263,
0x4229,
0x42f2,
0x42da,
0x4231,
0x42c3,
0x4259,
0x4238,
0x42d3,
0x42c1,
0x4238,
0x42e3,
0x4256,
0x4237,
0x4233,
0x4257,
0x4232,
0x4232,
0x42f7,
0x422c,
0x4282,
0x42a7,
0x4229,
0x4262,
0x429e,
0x422b,
0x4252,
0x42db,
0x4230,
0x42e3,
0x423b,
0x4237,
0x4283,
0x42ae,
0x423b,
0x4224,
0x4216,

0x0e00, //BURST_END

0x030a,
0x1902,	// Bus Switch

0x0315,	// Shading FPGA(Hi-352)
0x1001,	// LSC ON
0x1100, //gap y off, gap x off

0x2780,	// LSC G
0x2880,	// LSC B
0x2980,	// LSC R

///////////////////////////////////
// 10 Page Saturation (H/W)
///////////////////////////////////
0x0310,
0x1001,
0x1210, //YOFS ENB
0x1700, //20121127 CSP option
0x1800, //20121127 CSP option
0x2000, //16_235 range scale down off
0x6003, //Sat ENB Transfer Function     //Transfunction on

///////////////////////////////////
// 11 Page D-LPF (H/W)
///////////////////////////////////
0x0311, //11 page
0x100f, //D-LPF ENB //DPC marker

0x1228, //20121120 character long line detection th
0x132c, //20121120 character short line detection th

0x1d12, // ORG_STD Ctrl
0x1e00,// 20130410_STD 03 -> 00
0x2178, // Color STD Gain
//Bayer Sharpness Gain Ctrl
0xb722, //SpColor_gain1
0xb822, //SpColor_gain2
0xb921, //SpColor_gain3
0xba1e, //SpColor_gain4
0xbb1c, //SpColor_gain5
0xbc1a, //SpColor_gain6

0xf280, //pga_dark1_hi //Enter Dark1
0xf376, //pga_dark_lo  //Escape Dark1
///////////////////////////////////
// 12 Page DPC,GBGR (H/W)//////////
///////////////////////////////////
0x0312, //12 page
0x1057, //DPC ON
0x1230,
0x2b08, //white_th
0x2c08, //middle_h_th
0x2d08, //middle_l_th
0x2e06, //dark_th
0x2f10, //20121127 _DPC TH
0x3010, //20121127 _DPC TH
0x3110, //20121127 _DPC TH
0x3210, //20121127 _DPC TH
0x4188, //GBGR Cut off //46

///////////////////////////////////
// 12 Page CI-LPF (H/W)////////////
///////////////////////////////////

0xEF01, //Interpol Color filter On/Off

///////////////////////////////////
// 13 Page YC-2D_Y-NR (H/W)/////////
///////////////////////////////////
0x0313,

0x802d, //YC-2D_C-NR ENB, C-Filter DC option on B[7] //DC on 8b //DC off 2d
0x81ff, // add 20121210
0x82fe, // add 20121210

0x8532,
0x8608, // add 20121210

//==========================================================================
// C-Filter PS Reducing (Mask-Size Adjustment)

0x8790,//C-mask near STD TH
0x8870,//C-mask middle STD TH
0x8950,//C-mask far STD TH

0x8a86, //color STD

0x970f, // C-filter Lum gain 1
0x980e,
0x990d,
0x9a0c,
0x9b0b,
0x9c0a,
0x9d09,
0x9e08,

0xa70f, // C-filter STD gain 1
0xa80e,
0xa90d,
0xaa0c,
0xab0b,
0xac0a,
0xad09,
0xae08,

//==========================================================================

///////////////////////////////////
// 14 Page YC-2D_Sharpness(H/W)
///////////////////////////////////
0x0314,
0x7720, //Yc2d_ee_color_gain1
0x7820, //Yc2d_ee_color_gain2
0x7920, //Yc2d_ee_color_gain3
0x7a1c, //Yc2d_ee_color_gain4
0x7b1b, //Yc2d_ee_color_gain5
0x7c1a, //Yc2d_ee_color_gain6
0x7d19, //Yc2d_ee_color_gain7
0x7e18, //Yc2d_ee_color_gain8

0xc000, //Yc2d_ee_lclip_gain_n1
0xc100, //Yc2d_ee_lclip_gain_n2
0xc200, //Yc2d_ee_lclip_gain_n3
0xc300, //Yc2d_ee_lclip_gain_n4
0xc401, //Yc2d_ee_lclip_gain_n5

///////////////////////////////////////////////////////////////////////////////
// 16 Page CMC / AWB Gain
///////////////////////////////////////////////////////////////////////////////

0x0316,
0x107f,	// CMC ENB	3f(spGrap off) 7f(spGrap on)
0x2052,// PS / LN

0xa003,	// WB gain on
0xa205,	// R_h (12bit = 8bit * 16)
0xa380,	// R_l
0xa407,	// B_h (12bit = 8bit * 16)
0xa580,	// B_l

0xD001,	//Bayer gain enable
///////////////////////////////////////////////////////////////////////////////
// 17 Page Gamma
///////////////////////////////////////////////////////////////////////////////

0x0317,
0x1007,	// GMA ENB //PS On
0x1252,// old:43 new:65

///////////////////////////////////////////////////////////////////////////////
// 18 Page MCMC
///////////////////////////////////////////////////////////////////////////////

0x0318,	// Page 18
0x1001,	// mcmc_ctl1
0x117f,	// mcmc_ctl2
0x5310,	// mcmc_ctl3

0x561b,	// mcmc_glb_sat_lvl_sp1
0x5739,	// mcmc_glb_sat_lvl_sp2
0x585a,	// mcmc_glb_sat_lvl_sp3
0x5980,	// mcmc_glb_sat_lvl_sp4
0x5aa6,	// mcmc_glb_sat_lvl_sp5
0x5bc1,	// mcmc_glb_sat_lvl_sp6
0x5ce8,	// mcmc_glb_sat_lvl_sp7
0x5d38,	// mcmc_glb_sat_gain_sp1
0x5e3a,	// mcmc_glb_sat_gain_sp2
0x5f3c,	// mcmc_glb_sat_gain_sp3
0x603f,	// mcmc_glb_sat_gain_sp4
0x613f,	// mcmc_glb_sat_gain_sp5
0x623f,	// mcmc_glb_sat_gain_sp6
0x633f,	// mcmc_glb_sat_gain_sp7
0x643f,	// mcmc_glb_sat_gain_sp8
0x6500,	// mcmc_std_ctl1
0x6600,	// mcmc_std_ctl2
0x6700,	// mcmc_std_ctl3

0x6cff,	// mcmc_lum_ctl1 sat hue offset
0x6d3f,	// mcmc_lum_ctl2 gain
0x6e00,	// mcmc_lum_ctl3 hue
0x6f00,	// mcmc_lum_ctl4 rgb offset
0x7000,	// mcmc_lum_ctl5 rgb scale

0xa100,
0xa201,	//star gain enb

///////////////////////////////////////////////////////////////////////////////
// 1A Page_RGB Y-NR, Y-Sharpness
///////////////////////////////////////////////////////////////////////////////

0x031a,
0x309f,	// RGB-Sharpness ENB // Flat-region RGB ENB B[1] //Green dis [7] On

0x8D20, //RGB-Color_Gain1
0x8E20, //RGB-Color_Gain2
0x8F20, //RGB-Color_Gain3
0x9020, //RGB-Color_Gain4
0x9120, //RGB-Color_Gain5

///////////////////////////////////////////////////////////////////////////////
// 20 Page (FZY)
///////////////////////////////////////////////////////////////////////////////

0x0320, //Page 20
0x1220,

0x1800,//Check Flicker Lock Off

0x3600, //EXP Unit 
0x3708,
0x3898,

0x51ff, //PGA Max
0x5220, //PGA Min x0.9

0x61FF,	// max ramp gain
0x6200,	// min ramp gain
0x60E0,	// ramp gain

0x803a, //Y Target
///////////////////////////////////////////////////////////////////////////////
// 23 Page (AFC)
///////////////////////////////////////////////////////////////////////////////

0x0323, //Page 23
0x147A, //Flicker Line 100
0x1566, //Flicker Line 120
0x1001, //Frame Interval

///////////////////////////////////////////////////////////////////////////////
// 2A Page (SSD)
///////////////////////////////////////////////////////////////////////////////

0x032A,
0x1011,
0x1101,
0x1650,	//SSD B gain int gain 1.5

0x0300,
0x0100,
0xff02,

0x0320,
0x10ad, //50hz bd, 60hz ad
0x2000, //Start ExpTime 120fps
0x2106,
0x22d9,
0x2320,

0x03c1,
0x1006, // ssd tranfer disable
0xff02, // 20ms

0x0300,
0x0101,	// Sleep on

0x03c1,
0x1007, // ssd tranfer enable
///////////////////////////////////////////////////////////////////////////////
//
// F/W setting start
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// C0 Page (SYSTEM)
///////////////////////////////////////////////////////////////////////////////
//OPCLK Setting
////54mhz = 337F980
0x03C0,
0x1403,
0x1537,
0x16F9,
0x1780,

///////////////////////////////////////////////////////////////////////////////
// C6 Page (SSD Y weight)
///////////////////////////////////////////////////////////////////////////////
//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

///////////////////////////////////////////////////////////////////////////////
// C7 Page (AE)
///////////////////////////////////////////////////////////////////////////////
//Shutter Setting
	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
0x1230, // Fast speed
0x15c0, // SSD Patch Weight Y Mean On

0x1e03, // Band1 Step
0x1f06, // Band2 Step
0x2008, // Band3 Step

0x2149, // Band1 Gain 30fps
0x2253, // Band2 Gain 15fps
0x2378,// Band3 Gain 12fps

0x3608, // Max 8fps
0x3708, // Max 8fps

0x3d22, // YTh Lock, Unlock0

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0x7002, // 50hz 82, 60hz 02
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x4400, //Start ExpTime 120fps
0x4506,	
0x46d9,	
0x4720,	

0xa748, //Start ExpTime 120fps float
0xa8db,	
0xa924,	
0xaa00,	

0x0320, //HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x10d0,	//AE On 50hz f0, 60hz d0

///////////////////////////////////////////////////////////////////////////////
// D9 Page (Capture/Preview)
///////////////////////////////////////////////////////////////////////////////
0x03d9,
0x7ce0, 
0x8c20,	//en_ramp_gain_auto

///////////////////////////////////////////////////////////////////////////////
// C8 ~ CC Page (AWB)
///////////////////////////////////////////////////////////////////////////////
0x03C8,
0x0e01, // burst start

0x10D2,
0x11c3,
0x12e0,
0x131a, //bCtl4
0x149f,
0x15c4,
0x1600,
0x1734,
0x1855,
0x1922,
0x1a22,
0x1b44,
0x1c44,
0x1d66,
0x1e66,
0x1f88,
0x2088,
0x2104,
0x2230,
0x2308,
0x2400,
0x251e,
0x2630, //init awb speed
0x276a,
0x2880,
0x2910,
0x2a04,
0x2b0a,
0x2c04,
0x2d0c,
0x2e1e,
0x2f00, //dwOutdoorCondInTh
0x3000, //dwOutdoorCondInTh_n01
0x3121, //dwOutdoorCondInTh_n02
0x3234, //dwOutdoorCondInTh_n03
0x3300, //dwOutdoorCondOutTh
0x3400, //dwOutdoorCondOutTh_n01
0x3527, //dwOutdoorCondOutTh_n02
0x3610, //dwOutdoorCondOutTh_n03
0x3700, //dwEvTh
0x3800, //dwEvTh_n01
0x3904, //dwEvTh_n02 //840fps
0x3a11, //dwEvTh_n03
0x3b00, //dwEvTh_a01
0x3c00, //dwEvTh_a01_n01
0x3d08, //dwEvTh_a01_n02
0x3e23, //dwEvTh_a01_n03 //480fps
0x3f00, //dwEvTh_a02
0x4000, //dwEvTh_a02_n01
0x4120,//dwEvTh_a02_n02
0x428D,//dwEvTh_a02_n03 //120fps
0x4300, //dwEvTh_a03
0x4400, //dwEvTh_a03_n01
0x4561, //dwEvTh_a03_n02
0x46a8,//dwEvTh_a03_n03 //40fps
0x4700, //dwEvTh_a04
0x4804, //dwEvTh_a04_n01
0x49c4, //dwEvTh_a04_n02
0x4ab4, //dwEvTh_a04_n03
0x4b00, //dwEvTh_a05
0x4c0b, //dwEvTh_a05_n01
0x4d71, //dwEvTh_a05_n02
0x4eb0, //dwEvTh_a05_n03
0x4f96,//aAglMaxMinLmt
0x504a,//aAglMaxMinLmt_a01
0x519C, //aAglMaxMinLmt_a02
0x5260, //aAglMaxMinLmt_a03
0x53a0, //aAglMaxMinLmt_a04
0x5420, //aAglMaxMinLmt_a05
0x55a0, //aAglMaxMinLmt_a06
0x5638, //aAglMaxMinLmt_a07
0x5776,//aTgtWhtRgnBgMaxMinLmt
0x5856,//aTgtWhtRgnBgMaxMinLmt_a01
0x5982, //aTgtWhtRgnBgMaxMinLmt_a02
0x5a52, //aTgtWhtRgnBgMaxMinLmt_a03
0x5b98, //aTgtWhtRgnBgMaxMinLmt_a04
0x5c20, //aTgtWhtRgnBgMaxMinLmt_a05
0x5d98, //aTgtWhtRgnBgMaxMinLmt_a06
0x5e42, //aTgtWhtRgnBgMaxMinLmt_a07

0x5f10, //bTgtWhtRgnBgStep
0x600a, //BpOption distance weight  def : 0a -> 04 -> 02

0x611e,
0x6234,
0x6380,
0x6410,
0x6501,
0x6604,
0x670e,
0x6800,
0x6932,
0x6a00,
0x6ba2,
0x6c02,
0x6d00,
0x6e00,
0x6f00,
0x7000,
0x7100,
0x7200,
0x7300,
0x7400,
0x7500,
0x7655,
0x7755,
0x7855,
0x7955,
0x7a55,
0x7b55,
0x7c55,
0x7d55,
0x7e55,
0x7f55,
0x8055,
0x8155,
0x8255,
0x8355,
0x8455,
0x8555,
0x8655,
0x8755,
0x8855,
0x8955,
0x8a55,
0x8b55,
0x8c55,
0x8d55,
0x8e55,
0x8f55,
0x9055,
0x9100,
0x9200,

0x9300, //Indoor_wRgIntOfs
0x94a0, //Indoor_wRgIntOfs_n01
0x9500, //Indoor_wBgIntOfs
0x96c0, //Indoor_wBgIntOfs_n01
0x9610, //Indoor_bRgStep
0x9710, //Indoor_bBgStep
0x9926, //Indoor_aTgtWhtRgnBg
0x9a29, //Indoor_aTgtWhtRgnBg_a01
0x9b2c, //Indoor_aTgtWhtRgnBg_a02
0x9c38, //Indoor_aTgtWhtRgnBg_a03
0x9d43, //Indoor_aTgtWhtRgnBg_a04
0x9e4d, //Indoor_aTgtWhtRgnBg_a05
0x9f59, //Indoor_aTgtWhtRgnBg_a06
0xa064, //Indoor_aTgtWhtRgnBg_a07
0xa16f, //Indoor_aTgtWhtRgnBg_a08
0xa27b, //Indoor_aTgtWhtRgnBg_a09
0xa38e,//Indoor_aTgtWhtRgnBg_a10
0xa4a0,//Indoor_aTgtWhtRgnRgLtLmt
0xa598,//Indoor_aTgtWhtRgnRgLtLmt_a01
0xa682,//Indoor_aTgtWhtRgnRgLtLmt_a02
0xa76d,//Indoor_aTgtWhtRgnRgLtLmt_a03
0xa860,//Indoor_aTgtWhtRgnRgLtLmt_a04
0xa956,//Indoor_aTgtWhtRgnRgLtLmt_a05
0xaa4c,//Indoor_aTgtWhtRgnRgLtLmt_a06
0xab45,//Indoor_aTgtWhtRgnRgLtLmt_a07
0xac40,//Indoor_aTgtWhtRgnRgLtLmt_a08
0xad3c,//Indoor_aTgtWhtRgnRgLtLmt_a09
0xae39,//Indoor_aTgtWhtRgnRgLtLmt_a10
0xafaa,//Indoor_aTgtWhtRgnRgRtLmt
0xb0a9,//Indoor_aTgtWhtRgnRgRtLmt_a01
0xb1a8,//Indoor_aTgtWhtRgnRgRtLmt_a02
0xb2a2,//Indoor_aTgtWhtRgnRgRtLmt_a03
0xb395,//Indoor_aTgtWhtRgnRgRtLmt_a04
0xb488,//Indoor_aTgtWhtRgnRgRtLmt_a05
0xb578,//Indoor_aTgtWhtRgnRgRtLmt_a06
0xb665,//Indoor_aTgtWhtRgnRgRtLmt_a07
0xb75a,//Indoor_aTgtWhtRgnRgRtLmt_a08
0xb850,//Indoor_aTgtWhtRgnRgRtLmt_a09
0xb94a,//Indoor_aTgtWhtRgnRgRtLmt_a10
0xba1b, //Indoor_aOptWhtRgnBg
0xbb1d, //Indoor_aOptWhtRgnBg_a01
0xbc1f, //Indoor_aOptWhtRgnBg_a02
0xbd2a, //Indoor_aOptWhtRgnBg_a03
0xbe38, //Indoor_aOptWhtRgnBg_a04
0xbf47, //Indoor_aOptWhtRgnBg_a05
0xc054, //Indoor_aOptWhtRgnBg_a06
0xc161, //Indoor_aOptWhtRgnBg_a07
0xc272, //Indoor_aOptWhtRgnBg_a08
0xc382, //Indoor_aOptWhtRgnBg_a09
0xc49a,//Indoor_aOptWhtRgnBg_a10
0xc5ad, //Indoor_aOptWhtRgnRgLtLmt
0xc698,//Indoor_aOptWhtRgnRgLtLmt_a01
0xc78a,//Indoor_aOptWhtRgnRgLtLmt_a02
0xc874,//Indoor_aOptWhtRgnRgLtLmt_a03
0xc95f,//Indoor_aOptWhtRgnRgLtLmt_a04
0xca50,//Indoor_aOptWhtRgnRgLtLmt_a05
0xcb46,//Indoor_aOptWhtRgnRgLtLmt_a06
0xcc40,//Indoor_aOptWhtRgnRgLtLmt_a07
0xcd39,//Indoor_aOptWhtRgnRgLtLmt_a08
0xce35,//Indoor_aOptWhtRgnRgLtLmt_a09
0xcf33,//Indoor_aOptWhtRgnRgLtLmt_a10
0xd0ba,//Indoor_aOptWhtRgnRgRtLmt
0xd1b9,//Indoor_aOptWhtRgnRgRtLmt_a01
0xd2b8,//Indoor_aOptWhtRgnRgRtLmt_a02
0xd3b5, //Indoor_aOptWhtRgnRgRtLmt_a03
0xd4ae, //Indoor_aOptWhtRgnRgRtLmt_a04
0xd5a1, //Indoor_aOptWhtRgnRgRtLmt_a05
0xd68c, //Indoor_aOptWhtRgnRgRtLmt_a06
0xd778,//Indoor_aOptWhtRgnRgRtLmt_a07
0xd860,//Indoor_aOptWhtRgnRgRtLmt_a08
0xd954,//Indoor_aOptWhtRgnRgRtLmt_a09
0xda4d,//Indoor_aOptWhtRgnRgRtLmt_a10

0xdb36, //Indoor_aCtmpWgtWdhTh
0xdc40, //Indoor_aCtmpWgtWdhTh_a01
0xdd4c, //Indoor_aCtmpWgtWdhTh_a02
0xde5c, //Indoor_aCtmpWgtWdhTh_a03
0xdf6e, //Indoor_aCtmpWgtWdhTh_a04
0xe07f, //Indoor_aCtmpWgtWdhTh_a05
0xe1a4, //Indoor_aCtmpWgtWdhTh_a06
0xe227, //Indoor_aCtmpWgtHgtTh
0xe332, //Indoor_aCtmpWgtHgtTh_a01
0xe43c, //Indoor_aCtmpWgtHgtTh_a02
0xe548, //Indoor_aCtmpWgtHgtTh_a03
0xe65c, //Indoor_aCtmpWgtHgtTh_a04
0xe770, //Indoor_aCtmpWgtHgtTh_a05
0xe87c, //Indoor_aCtmpWgtHgtTh_a06
0xe986, //Indoor_aCtmpWgtHgtTh_a07
0xea90, //Indoor_aCtmpWgtHgtTh_a08
0xeb11, //Indoor_aCtmpWgt
0xec11, //Indoor_aCtmpWgt_a01
0xed12, //Indoor_aCtmpWgt_a02
0xee11, //Indoor_aCtmpWgt_a03
0xef11, //Indoor_aCtmpWgt_a04
0xf033, //Indoor_aCtmpWgt_a05
0xf111, //Indoor_aCtmpWgt_a06
0xf214, //Indoor_aCtmpWgt_a07
0xf343, //Indoor_aCtmpWgt_a08
0xf411, //Indoor_aCtmpWgt_a09
0xf555, //Indoor_aCtmpWgt_a10
0xf641, //Indoor_aCtmpWgt_a11
0xf716, //Indoor_aCtmpWgt_a12
0xf865, //Indoor_aCtmpWgt_a13
0xf911, //Indoor_aCtmpWgt_a14
0xfa48, //Indoor_aCtmpWgt_a15
0xfb61, //Indoor_aCtmpWgt_a16
0xfc11, //Indoor_aCtmpWgt_a17
0xfd46, //Indoor_aCtmpWgt_a18
0x0e00, // burst end

0x03c9, //c9 page
0x0e01, // burst start

0x1011, //Indoor_aCtmpWgt_a19
0x1111, //Indoor_aCtmpWgt_a20
0x1223, //Indoor_aCtmpWgt_a21
0x1311, //Indoor_aCtmpWgt_a22
0x1411, //Indoor_aCtmpWgt_a23
0x1510, //Indoor_aCtmpWgt_a24

0x1611,//Indoor_aYlvlWgt
0x1711,//Indoor_aYlvlWgt_a01
0x1811,//Indoor_aYlvlWgt_a02
0x1911,//Indoor_aYlvlWgt_a03
0x1a11,//Indoor_aYlvlWgt_a04
0x1b11,//Indoor_aYlvlWgt_a05
0x1c11,//Indoor_aYlvlWgt_a06
0x1d11,//Indoor_aYlvlWgt_a07
0x1e11,//Indoor_aYlvlWgt_a08
0x1f11,//Indoor_aYlvlWgt_a09
0x2011,//Indoor_aYlvlWgt_a10
0x2122,//Indoor_aYlvlWgt_a11
0x2222,//Indoor_aYlvlWgt_a12
0x2334,//Indoor_aYlvlWgt_a13
0x2432,//Indoor_aYlvlWgt_a14
0x2521,//Indoor_aYlvlWgt_a15

0x2633, //Indoor_aTgtAngle
0x273f, //Indoor_aTgtAngle_a01
0x2843, //Indoor_aTgtAngle_a02
0x2950,//Indoor_aTgtAngle_a03
0x2a68,//Indoor_aTgtAngle_a04
0x2b28,//Indoor_aRgTgtOfs
0x2c14,//Indoor_aRgTgtOfs_a01
0x2d02,//Indoor_aRgTgtOfs_a02
0x2e00,//Indoor_aRgTgtOfs_a03
0x2f08,//Indoor_aRgTgtOfs_a04
0x30d4,//Indoor_aBgTgtOfs
0x31ca,//Indoor_aBgTgtOfs_a01
0x32aa,//Indoor_aBgTgtOfs_a02
0x33a4,//Indoor_aBgTgtOfs_a03
0x3488,//Indoor_aBgTgtOfs_a04

0x3500,//bRgDefTgt //indoor
0x3600,//bBgDefTgt //indoor

0x3720,//Indoor_aWhtPtTrcAglOfs
0x381e,//Indoor_aWhtPtTrcAglOfs_a01
0x391c,//Indoor_aWhtPtTrcAglOfs_a02
0x3a1a,//Indoor_aWhtPtTrcAglOfs_a03
0x3b18,//Indoor_aWhtPtTrcAglOfs_a04
0x3c16,//Indoor_aWhtPtTrcAglOfs_a05
0x3d14,//Indoor_aWhtPtTrcAglOfs_a06
0x3e14,//Indoor_aWhtPtTrcAglOfs_a07
0x3f13,//Indoor_aWhtPtTrcAglOfs_a08
0x4012,//Indoor_aWhtPtTrcAglOfs_a09
0x4104,//Indoor_bWhtPtTrcCnt
0x4214,//Indoor_aRtoDiffThNrBp
0x433c,//Indoor_aRtoDiffThNrBp_a01
0x4428,//Indoor_aAglDiffThTrWhtPt
0x4550,//Indoor_aAglDiffThTrWhtPt_a01
0x46aa,//Indoor_bWgtRatioTh1
0x47a0,//Indoor_bWgtRatioTh2
0x4844,//Indoor_bWgtOfsTh1
0x4940,//Indoor_bWgtOfsTh2
0x4a5a,//Indoor_bWhtPtCorAglMin
0x4b70,//Indoor_bWhtPtCorAglMax
0x4c04,//Indoor_bYlvlMin
0x4df8,//Indoor_bYlvlMax
0x4e28,//Indoor_bPxlWgtLmtLoTh
0x4f78,//Indoor_bPxlWgtLmtHiTh
0x5000,//Indoor_SplBldWgt_1
0x5100,//Indoor_SplBldWgt_2
0x5264,//Indoor_SplBldWgt_3
0x5360,//Indoor_TgtOff_StdHiTh
0x5430,//Indoor_TgtOff_StdLoTh
0x5505,//Indoor_wInitRg
0x56d0,//Indoor_wInitRg_n01
0x5706,//Indoor_wInitBg
0x5840,//Indoor_wInitBg_n01

0x5902, //Indoor_aRatioBox
0x5aee, //Indoor_aRatioBox_a01
0x5b06, //Indoor_aRatioBox_a02
0x5c40, //Indoor_aRatioBox_a03
0x5d08, //Indoor_aRatioBox_a04
0x5e34, //Indoor_aRatioBox_a05
0x5f0b,//Indoor_aRatioBox_a06
0x6054,//Indoor_aRatioBox_a07
0x6103, //Indoor_aRatioBox_a08
0x6252, //Indoor_aRatioBox_a09
0x6307, //Indoor_aRatioBox_a10
0x64d0, //Indoor_aRatioBox_a11
0x6506, //Indoor_aRatioBox_a12
0x66a4, //Indoor_aRatioBox_a13
0x6708, //Indoor_aRatioBox_a14
0x68fc, //Indoor_aRatioBox_a15
0x6903, //Indoor_aRatioBox_a16
0x6ae8, //Indoor_aRatioBox_a17
0x6b0a, //Indoor_aRatioBox_a18
0x6c8c, //Indoor_aRatioBox_a19
0x6d04, //Indoor_aRatioBox_a20
0x6eb0, //Indoor_aRatioBox_a21
0x6f07, //Indoor_aRatioBox_a22
0x706c, //Indoor_aRatioBox_a23
0x7104, //Indoor_aRatioBox_a24
0x72e2, //Indoor_aRatioBox_a25
0x730c, //Indoor_aRatioBox_a26
0x741c, //Indoor_aRatioBox_a27
0x7503, //Indoor_aRatioBox_a28
0x7684, //Indoor_aRatioBox_a29
0x7705, //Indoor_aRatioBox_a30
0x78dc, //Indoor_aRatioBox_a31
0x7905, //Indoor_aRatioBox_a32
0x7adc, //Indoor_aRatioBox_a33
0x7b0c, //Indoor_aRatioBox_a34
0x7ce4, //Indoor_aRatioBox_a35
0x7d01, //Indoor_aRatioBox_a36
0x7ef4, //Indoor_aRatioBox_a37
0x7f05, //Indoor_aRatioBox_a38
0x8000, //Indoor_aRatioBox_a39

0x8100, //Outdoor_wRgIntOfs
0x8240, //Outdoor_wRgIntOfs_n01
0x8301, //Outdoor_wBgIntOfs
0x8400, //Outdoor_wBgIntOfs_n01
0x8510, //Outdoor_bRgStep
0x8610, //Outdoor_bBgStep
0x8751, //Outdoor_aTgtWhtRgnBg
0x8852, //Outdoor_aTgtWhtRgnBg_a01
0x8953, //Outdoor_aTgtWhtRgnBg_a02
0x8a57, //Outdoor_aTgtWhtRgnBg_a03
0x8b5e, //Outdoor_aTgtWhtRgnBg_a04
0x8c64, //Outdoor_aTgtWhtRgnBg_a05
0x8d6A, //Outdoor_aTgtWhtRgnBg_a06
0x8e6F, //Outdoor_aTgtWhtRgnBg_a07
0x8f75, //Outdoor_aTgtWhtRgnBg_a08
0x907c, //Outdoor_aTgtWhtRgnBg_a09
0x9184, //Outdoor_aTgtWhtRgnBg_a10
0x925D, //Outdoor_aTgtWhtRgnRgLtLmt
0x9357, //Outdoor_aTgtWhtRgnRgLtLmt_a01
0x9451, //Outdoor_aTgtWhtRgnRgLtLmt_a02
0x9550, //Outdoor_aTgtWhtRgnRgLtLmt_a03
0x964e,//Outdoor_aTgtWhtRgnRgLtLmt_a04
0x974c,//Outdoor_aTgtWhtRgnRgLtLmt_a05
0x984b,//Outdoor_aTgtWhtRgnRgLtLmt_a06
0x9949,//Outdoor_aTgtWhtRgnRgLtLmt_a07
0x9a47, //Outdoor_aTgtWhtRgnRgLtLmt_a08
0x9b46,//Outdoor_aTgtWhtRgnRgLtLmt_a09
0x9c45,//Outdoor_aTgtWhtRgnRgLtLmt_a10
0x9d64, //Outdoor_aTgtWhtRgnRgRtLmt
0x9e63, //Outdoor_aTgtWhtRgnRgRtLmt_a01
0x9f62, //Outdoor_aTgtWhtRgnRgRtLmt_a02
0xa062, //Outdoor_aTgtWhtRgnRgRtLmt_a03
0xa161, //Outdoor_aTgtWhtRgnRgRtLmt_a04
0xa260, //Outdoor_aTgtWhtRgnRgRtLmt_a05
0xa35e, //Outdoor_aTgtWhtRgnRgRtLmt_a06
0xa45d,//Outdoor_aTgtWhtRgnRgRtLmt_a07
0xa55c,//Outdoor_aTgtWhtRgnRgRtLmt_a08
0xa65a, //Outdoor_aTgtWhtRgnRgRtLmt_a09
0xa757,//Outdoor_aTgtWhtRgnRgRtLmt_a10
0xa840, //Outdoor_aOptWhtRgnBg
0xa945, //Outdoor_aOptWhtRgnBg_a01
0xaa4b, //Outdoor_aOptWhtRgnBg_a02
0xab54, //Outdoor_aOptWhtRgnBg_a03
0xac60, //Outdoor_aOptWhtRgnBg_a04
0xad6c, //Outdoor_aOptWhtRgnBg_a05
0xae76, //Outdoor_aOptWhtRgnBg_a06
0xaf7f, //Outdoor_aOptWhtRgnBg_a07
0xb08c, //Outdoor_aOptWhtRgnBg_a08
0xb195, //Outdoor_aOptWhtRgnBg_a09
0xb2a0, //Outdoor_aOptWhtRgnBg_a10
0xb36a, //Outdoor_aOptWhtRgnRgLtLmt
0xb45b, //Outdoor_aOptWhtRgnRgLtLmt_a01
0xb553, //Outdoor_aOptWhtRgnRgLtLmt_a02
0xb64c, //Outdoor_aOptWhtRgnRgLtLmt_a03
0xb746, //Outdoor_aOptWhtRgnRgLtLmt_a04
0xb842, //Outdoor_aOptWhtRgnRgLtLmt_a05
0xb93e, //Outdoor_aOptWhtRgnRgLtLmt_a06
0xba3c, //Outdoor_aOptWhtRgnRgLtLmt_a07
0xbb3a, //Outdoor_aOptWhtRgnRgLtLmt_a08
0xbc39, //Outdoor_aOptWhtRgnRgLtLmt_a09
0xbd37, //Outdoor_aOptWhtRgnRgLtLmt_a10
0xbe7d, //Outdoor_aOptWhtRgnRgRtLmt
0xbf7c, //Outdoor_aOptWhtRgnRgRtLmt_a01
0xc079, //Outdoor_aOptWhtRgnRgRtLmt_a02
0xc176, //Outdoor_aOptWhtRgnRgRtLmt_a03
0xc26f, //Outdoor_aOptWhtRgnRgRtLmt_a04
0xc36a, //Outdoor_aOptWhtRgnRgRtLmt_a05
0xc466, //Outdoor_aOptWhtRgnRgRtLmt_a06
0xc563, //Outdoor_aOptWhtRgnRgRtLmt_a07
0xc65B, //Outdoor_aOptWhtRgnRgRtLmt_a08
0xc754, //Outdoor_aOptWhtRgnRgRtLmt_a09
0xc84a, //Outdoor_aOptWhtRgnRgRtLmt_a10

0xc942, //Outdoor_aCtmpWgtWdhTh
0xca4c,//Outdoor_aCtmpWgtWdhTh_a01
0xcb54,//Outdoor_aCtmpWgtWdhTh_a02
0xcc5c,//Outdoor_aCtmpWgtWdhTh_a03
0xcd64,//Outdoor_aCtmpWgtWdhTh_a04
0xce6c,//Outdoor_aCtmpWgtWdhTh_a05
0xcf74,//Outdoor_aCtmpWgtWdhTh_a06
0xd042, //Outdoor_aCtmpWgtHgtTh
0xd152, //Outdoor_aCtmpWgtHgtTh_a01
0xd258, //Outdoor_aCtmpWgtHgtTh_a02
0xd35e, //Outdoor_aCtmpWgtHgtTh_a03
0xd464, //Outdoor_aCtmpWgtHgtTh_a04
0xd56a, //Outdoor_aCtmpWgtHgtTh_a05
0xd672, //Outdoor_aCtmpWgtHgtTh_a06
0xd77a, //Outdoor_aCtmpWgtHgtTh_a07
0xd888, //Outdoor_aCtmpWgtHgtTh_a08
0xd911, //Outdoor_aCtmpWgt
0xda23,//Outdoor_aCtmpWgt_a01
0xdb22,//Outdoor_aCtmpWgt_a02
0xdc11, //Outdoor_aCtmpWgt_a03
0xdd22,//Outdoor_aCtmpWgt_a04
0xde22, //Outdoor_aCtmpWgt_a05
0xdf11, //Outdoor_aCtmpWgt_a06
0xe033, //Outdoor_aCtmpWgt_a07
0xe131,//Outdoor_aCtmpWgt_a08
0xe212, //Outdoor_aCtmpWgt_a09
0xe366,//Outdoor_aCtmpWgt_a10
0xe441,//Outdoor_aCtmpWgt_a11
0xe513, //Outdoor_aCtmpWgt_a12
0xe677,//Outdoor_aCtmpWgt_a13
0xe741,//Outdoor_aCtmpWgt_a14
0xe813,//Outdoor_aCtmpWgt_a15
0xe974, //Outdoor_aCtmpWgt_a16
0xea11, //Outdoor_aCtmpWgt_a17
0xeb23,//Outdoor_aCtmpWgt_a18
0xec53,//Outdoor_aCtmpWgt_a19
0xed11, //Outdoor_aCtmpWgt_a20
0xee43,//Outdoor_aCtmpWgt_a21
0xef31,//Outdoor_aCtmpWgt_a22
0xf011, //Outdoor_aCtmpWgt_a23
0xf111, //Outdoor_aCtmpWgt_a24

0xf212,//aYlvlWgt
0xf334,//aYlvlWgt_a01
0xf443,//aYlvlWgt_a02
0xf532,//aYlvlWgt_a03
0xf622,//aYlvlWgt_a04
0xf711, //aYlvlWgt_a05
0xf811, //aYlvlWgt_a06
0xf911, //aYlvlWgt_a07
0xfa11, //aYlvlWgt_a08
0xfb11, //aYlvlWgt_a09
0xfc11,//aYlvlWgt_a10
0xfd11, //aYlvlWgt_a11
0x0e00, // burst end

//Page ca
0x03ca,
0x0e01, // burst start

0x1011, //aYlvlWgt_a12
0x1122, //aYlvlWgt_a13
0x1222, //aYlvlWgt_a14
0x1311, //aYlvlWgt_a15

0x1464, //Outdoor_aTgtAngle
0x156b, //Outdoor_aTgtAngle_a01
0x1670, //Outdoor_aTgtAngle_a02
0x177a,//Outdoor_aTgtAngle_a03
0x1884,//Outdoor_aTgtAngle_a04
0x191a,//Outdoor_aRgTgtOfs
0x1a12,//Outdoor_aRgTgtOfs_a01
0x1b0a,//Outdoor_aRgTgtOfs_a02
0x1c04,//Outdoor_aRgTgtOfs_a03
0x1d04,//Outdoor_aRgTgtOfs_a04
0x1e9e,//Outdoor_aBgTgtOfs
0x1f88, //Outdoor_aBgTgtOfs_a01
0x2086,//Outdoor_aBgTgtOfs_a02
0x2182,//Outdoor_aBgTgtOfs_a03
0x2280,//Outdoor_aBgTgtOfs_a04
0x2384,//Outdoor_bRgDefTgt
0x2488, //Outdoor_bBgDefTgt

0x251c, //Outdoor_aWhtPtTrcAglOfs
0x261a, //Outdoor_aWhtPtTrcAglOfs_a01
0x2718, //Outdoor_aWhtPtTrcAglOfs_a02
0x2816, //Outdoor_aWhtPtTrcAglOfs_a03
0x2914, //Outdoor_aWhtPtTrcAglOfs_a04
0x2a12, //Outdoor_aWhtPtTrcAglOfs_a05
0x2b10, //Outdoor_aWhtPtTrcAglOfs_a06
0x2c0f, //Outdoor_aWhtPtTrcAglOfs_a07
0x2d0e, //Outdoor_aWhtPtTrcAglOfs_a08
0x2e0e, //Outdoor_aWhtPtTrcAglOfs_a09
0x2f0a, //Outdoor_bWhtPtTrcCnt
0x3028, //Outdoor_aRtoDiffThNrBp
0x3148, //Outdoor_aRtoDiffThNrBp_a01
0x3228, //Outdoor_aAglDiffThTrWhtPt
0x3350, //Outdoor_aAglDiffThTrWhtPt_a01
0x34aa, //Outdoor_bWgtRatioTh1
0x35a0, //Outdoor_bWgtRatioTh2
0x360a, //Outdoor_bWgtOfsTh1
0x37a0, //Outdoor_bWgtOfsTh2
0x386d, //Outdoor_bWhtPtCorAglMin
0x3978, //Outdoor_bWhtPtCorAglMax
0x3a04, //Outdoor_bYlvlMin
0x3bf8, //Outdoor_bYlvlMax
0x3c28, //Outdoor_bPxlWgtLmtLoTh
0x3d78, //Outdoor_bPxlWgtLmtHiTh
0x3e00, //Outdoor_SplBldWgt_1
0x3f00, //Outdoor_SplBldWgt_2
0x4064, //Outdoor_SplBldWgt_3
0x4160, //Outdoor_TgtOff_StdHiTh
0x4230, //Outdoor_TgtOff_StdLoTh
0x4304,
0x44c0,
0x4507,
0x46c0,
0x4702, //Outdoor_aRatioBox
0x48b2, //Outdoor_aRatioBox_a01
0x4905, //Outdoor_aRatioBox_a02
0x4adc, //Outdoor_aRatioBox_a03
0x4b0a, //Outdoor_aRatioBox_a04
0x4c28, //Outdoor_aRatioBox_a05
0x4d0c, //Outdoor_aRatioBox_a06
0x4e1c, //Outdoor_aRatioBox_a07
0x4f02, //Outdoor_aRatioBox_a08
0x50ee, //Outdoor_aRatioBox_a09
0x5106, //Outdoor_aRatioBox_a10
0x5272, //Outdoor_aRatioBox_a11
0x5308, //Outdoor_aRatioBox_a12
0x5498, //Outdoor_aRatioBox_a13
0x550a, //Outdoor_aRatioBox_a14
0x56f0, //Outdoor_aRatioBox_a15
0x5703, //Outdoor_aRatioBox_a16
0x5820, //Outdoor_aRatioBox_a17
0x5907, //Outdoor_aRatioBox_a18
0x5a08, //Outdoor_aRatioBox_a19
0x5b07, //Outdoor_aRatioBox_a20
0x5c6c, //Outdoor_aRatioBox_a21
0x5d09, //Outdoor_aRatioBox_a22
0x5e60, //Outdoor_aRatioBox_a23
0x5f03, //Outdoor_aRatioBox_a24
0x6084, //Outdoor_aRatioBox_a25
0x6107, //Outdoor_aRatioBox_a26
0x62d0, //Outdoor_aRatioBox_a27
0x6306, //Outdoor_aRatioBox_a28
0x6440, //Outdoor_aRatioBox_a29
0x6508, //Outdoor_aRatioBox_a30
0x6634, //Outdoor_aRatioBox_a31
0x6703, //Outdoor_aRatioBox_a32
0x68e8, //Outdoor_aRatioBox_a33
0x6908, //Outdoor_aRatioBox_a34
0x6ad0, //Outdoor_aRatioBox_a35
0x6b04, //Outdoor_aRatioBox_a36
0x6c4c, //Outdoor_aRatioBox_a37
0x6d07, //Outdoor_aRatioBox_a38
0x6e08, //Outdoor_aRatioBox_a39

0x6f04,
0x7000,

0x7105, //Out2_Adt_RgainMin
0x7200, //Out2_Adt_RgainMin_n01
0x7305, //Out2_Adt_RgainMax
0x74d0,//Out2_Adt_RgainMax_n01
0x7504, //Out2_Adt_GgainMin
0x7600, //Out2_Adt_GgainMin_n01
0x7704, //Out2_Adt_GgainMax
0x7800, //Out2_Adt_GgainMax_n01
0x7905, //Out2_Adt_BgainMin
0x7ae0, //Out2_Adt_BgainMin_n01
0x7b07, //Out2_Adt_BgainMax
0x7c00,//Out2_Adt_BgainMax_n01

0x7d05, //Out1_Adt_RgainMin
0x7e40,//Out1_Adt_RgainMin_n01
0x7f06, //Out1_Adt_RgainMax
0x8080, //Out1_Adt_RgainMax_n01
0x8104, //Out1_Adt_GgainMin
0x8200, //Out1_Adt_GgainMin_n01
0x8304, //Out1_Adt_GgainMax
0x8400, //Out1_Adt_GgainMax_n01
0x8505, //Out1_Adt_BgainMin
0x8680, //Out1_Adt_BgainMin_n01
0x8707, //Out1_Adt_BgainMax
0x88e0, //Out1_Adt_BgainMax_n01

0x8904, //In_Adt_RgainMin
0x8a00, //In_Adt_RgainMin_n01
0x8b0d, //In_Adt_RgainMax
0x8c00, //In_Adt_RgainMax_n01
0x8d04, //In_Adt_GgainMin
0x8e00, //In_Adt_GgainMin_n01
0x8f05, //In_Adt_GgainMax
0x9080, //In_Adt_GgainMax_n01
0x9104, //In_Adt_BgainMin
0x9200, //In_Adt_BgainMin_n01
0x930d, //In_Adt_BgainMax
0x9480, //In_Adt_BgainMax_n01

0x9504, //Manual_Adt_RgainMin
0x9600, //Manual_Adt_RgainMin_n01
0x970d, //Manual_Adt_RgainMax
0x9800, //Manual_Adt_RgainMax_n01
0x9904, //Manual_Adt_GgainMin
0x9a00, //Manual_Adt_GgainMin_n01
0x9b04, //Manual_Adt_GgainMax
0x9c80, //Manual_Adt_GgainMax_n01
0x9d04, //Manual_Adt_BgainMin
0x9e00, //Manual_Adt_BgainMin_n01
0x9f0b, //Manual_Adt_BgainMax
0xa000, //Manual_Adt_BgainMax_n01

0x0e00, //burst end

0x03C8,
0x11C3,	//AWB reset


/////////////////////////////////////////////////////////////////////////////////
// CD page(OTP control)
/////////////////////////////////////////////////////////////////////////////////
0x03CD,
0x1003,

0x2210,
//Manual Typical colo ratio write
0x271A, //Typical RG=0.685*1000 = 6850 = 1AC2
0x28C2,
0x2910, //Typical BG=0.430*1000 = 4300 = 10CC
0x2ACC,
0x2b0a,//+/-10 valid ratio check

///////////////////////////////////////////////////////////////////////////////
// Color ratio setting
/////////////////////////////////////////////////////////////////////////////////
0x03CE,
0x3098,	//Color ratio on
0x3101,
0x3304,	//R gain def
0x3400,
0x3504,	//G gain def
0x3600,
0x3704,	//B gain def
0x3800,

0x4500, //Outdoor In EvTh
0x4600,
0x4727,
0x4810,
0x4900, //Outdoor Out EvTh
0x4a00,
0x4b4e,
0x4c20,

0x553e, //Low In Th
0x564c, //Low Out Th
0x575c, //High Out Th
0x586c, //High In Th

0x6286, //Out weight
0x6386, //Indoor weight
0x6488, //Dark weight
0x65d8, //Low weight
0x6686, //High weight
///////////////////////////////////////////////////////////////////////////////
// D3 ~ D8 Page (Adaptive)
///////////////////////////////////////////////////////////////////////////////

0x03d3,	// Adaptive start

0x0e01, // burst start

0x1000,
0x1100,
0x1200,
0x1300,
0x1400,
0x1500,
0x1600,
0x1700,
0x1800,
0x1900,

0x1a00,	// Def_Yoffset
0x1b23,	// DYOFS_Ratio
0x1c04,	// DYOFS_Limit

0x1d00,//EV Th OutEnd
0x1e00,
0x1f20,
0x208d,

0x2100,//EV Th OutStr def :  80fps  Ag 1x Dg 1x
0x2200,
0x2330,
0x24d4,

0x2500,	//EV Th Dark1Str
0x260b,
0x2771,
0x28b0,

0x2900,	//EV Th Dark1End
0x2a0d,
0x2bbb,
0x2ca0,

0x2d00,	//EV Th Dark2Str
0x2e12,
0x2f76,
0x3090,

0x3100,	//EV Th Dark2End
0x321e,
0x3384,
0x3480,

0x354b, //Ctmp LT End
0x3652, //Ctmp LT Str
0x3769,	//Ctmp HT Str
0x3873,	//Ctmp HT End

0x3900,	// LSC_EvTh_OutEnd_4
0x3a00,	// LSC_EvTh_OutEnd_3
0x3b13,	// LSC_EvTh_OutEnd_2
0x3c88,	// LSC_EvTh_OutEnd_1    def : 200fps  Ag 1x Dg 1x

0x3d00,	// LSC_EvTh_OutStr_4
0x3e00,	// LSC_EvTh_OutStr_3
0x3f30,	// LSC_EvTh_OutStr_2
0x40d4,	// LSC_EvTh_OutStr_1    def :  80fps  Ag 1x Dg 1x

0x4100,	// LSC_EvTh_Dark1Str_4
0x4205,	// LSC_EvTh_Dark1Str_3
0x43b8,	// LSC_EvTh_Dark1Str_2
0x44d8,	// LSC_EvTh_Dark1Str_1  def :  8fps  Ag 3x Dg 1x

0x4500,	// LSC_EvTh_Dark1End_4
0x460b,	// LSC_EvTh_Dark1End_3
0x4771,	// LSC_EvTh_Dark1End_2
0x48b0,	// LSC_EvTh_Dark1End_1  def :  8fps  Ag 6x Dg 1x

0x4900,	// LSC_EvTh_Dark2Str_4
0x4a0f,	// LSC_EvTh_Dark2Str_3
0x4b42,	// LSC_EvTh_Dark2Str_2
0x4c40,	// LSC_EvTh_Dark2Str_1  def :  8fps  Ag 8x Dg 1x

0x4d00,	// LSC_EvTh_Dark2End_4
0x4e1e,	// LSC_EvTh_Dark2End_3
0x4f84,	// LSC_EvTh_Dark2End_2
0x5080,	// LSC_EvTh_Dark2End_1  def :  4fps  Ag 8x Dg 1x

0x5155,//LSC Ctmp LTEnd Out
0x5264,	//LSC Ctmp LTStr Out
0x5378,	//LSC Ctmp HTStr Out
0x5486,	//LSC Ctmp HTEnd Out

0x5546,	//LSC Ctmp LTEnd In
0x5656,	//LSC Ctmp LTStr In
0x576e,	//LSC Ctmp HTStr In
0x5876,	//LSC Ctmp HTEnd In

0x5950,	// LSC_CTmpTh_LT_End_Dark
0x5a78,	// LSC_CTmpTh_LT_Str_Dark
0x5ba0,	// LSC_CTmpTh_HT_Str_Dark
0x5cb4,	// LSC_CTmpTh_HT_End_Dark

0x5d00,	// UniScn_EvMinTh_4
0x5e00,	// UniScn_EvMinTh_3
0x5f04,	// UniScn_EvMinTh_2
0x60e2,	// UniScn_EvMinTh_1    def : 600fps  Ag 1x Dg 1x

0x6100,	// UniScn_EvMaxTh_4
0x6205,	// UniScn_EvMaxTh_3
0x63b8,	// UniScn_EvMaxTh_2
0x64d8,	// UniScn_EvMaxTh_1     def :  8fps  Ag 3x Dg 1x

0x654e,	// UniScn_AglMinTh_1
0x6650,	// UniScn_AglMinTh_2
0x6773,	// UniScn_AglMaxTh_1
0x687d,	// UniScn_AglMaxTh_2

0x6903,	// UniScn_YstdMinTh
0x6a0a,	// UniScn_YstdMaxTh
0x6b1e,	// UniScn_BPstdMinTh
0x6c34,	// UniScn_BPstdMaxTh

0x6d64,	// Ytgt_ColWgt_Out
0x6e64,	// Ytgt_ColWgt_Dark
0x6f64,	// ColSat_ColWgt_Out
0x7064,	// ColSat_ColWgt_Dark
0x7164,	// CMC_ColWgt_Out
0x7264,	// CMC_ColWgt_Dark
0x7364,	// MCMC_ColWgt_Out
0x7464,	// MCMC_ColWgt_Dark
0x7564,	// CustomReg_CorWgt_Out
0x7664,	// CustomReg_CorWgt_Dark

0x7764,	// UniScn_Y_Ratio
0x7850,	// UniScn_Cb_Ratio
0x7950,	// UniScn_Cr_Ratio

0x7a00,	// Ytgt_offset
0x7b00,	// CbSat_offset
0x7c00,	// CrSat_offset

0x7d34,	// Y_target_Outdoor
0x7e34,	// Y_target_Indoor
0x7f34,	// Y_target_Dark1
0x8034,	// Y_target_Dark2
0x8135,// Y_target_LowTemp
0x822f,// Y_target_HighTemp

0x8398, // Cb_Outdoor
0x8495,	// Cb _Sat_Indoor
0x85a0,	// Cb _Sat_Dark1
0x8684,	// Cb _Sat_Dark2
0x8788,	// Cb _Sat_LowTemp
0x8892,	// Cb _Sat_HighTemp

0x8988,	// Cr _Sat_Outdoor
0x8a90,	// Cr _Sat_Indoor
0x8ba0,	// Cr _Sat_Dark1
0x8c80,	// Cr _Sat_Dark2
0x8d75,	// Cr _Sat_LowTemp
0x8e92,	// Cr _Sat_HighTemp

0x8f82,	// BLC_ofs_r_Outdoor
0x9081,	// BLC_ofs_b_Outdoor
0x9182,	// BLC_ofs_gr_Outdoor
0x9282,	// BLC_ofs_gb_Outdoor

0x9381,	// BLC_ofs_r_Indoor
0x9480,	// BLC_ofs_b_Indoor
0x9581,	// BLC_ofs_gr_Indoor
0x9681,	// BLC_ofs_gb_Indoor

0x9782,	// BLC_ofs_r_Dark1
0x9882,	// BLC_ofs_b_Dark1
0x9982,	// BLC_ofs_gr_Dark1
0x9a82,	// BLC_ofs_gb_Dark1

0x9b82,	// BLC_ofs_r_Dark2
0x9c82,	// BLC_ofs_b_Dark2
0x9d82,	// BLC_ofs_gr_Dark2
0x9e82,	// BLC_ofs_gb_Dark2

0x9f00,	//LSC Out_L ofs G
0xa000,	//LSC Out_L ofs B
0xa100,	//LSC Out_L ofs R
0xa280,	//LSC Out_L Gain G
0xa382,	//LSC Out_L Gain B
0xa486,	//LSC Out_L Gain R

0xa500,	//LSC Out_M ofs G
0xa600,	//LSC Out_M ofs B
0xa700,	//LSC Out_M ofs R
0xa880,	//LSC Out_M Gain G
0xa982,	//LSC Out_M Gain B
0xaa80,	//LSC Out_M Gain R

0xab00,	//LSC Out_H ofs G
0xac00,	//LSC Out_H ofs B
0xad00,	//LSC Out_H ofs R
0xae80,	//LSC Out_H Gain G
0xaf84,	//LSC Out_H Gain B
0xb078,	//LSC Out_H Gain R

0xb100,	// LSC0_Ind_LowTmp        offset g
0xb200,	// LSC1_Ind_LowTmp        offset b
0xb300,	// LSC2_Ind_LowTmp        offset r
0xb480,	// LSC3_Ind_LowTmp        gain g
0xb580,	// LSC4_Ind_LowTmp        gain b
0xb688,	// LSC5_Ind_LowTmp        gain r

0xb700,	// LSC0_Ind_MiddleTmp     offset g
0xb800,	// LSC1_Ind_MiddleTmp     offset b
0xb900,	// LSC2_Ind_MiddleTmp     offset r
0xba80,	// LSC3_Ind_MiddleTmp     gain g
0xbb80,	// LSC4_Ind_MiddleTmp     gain b
0xbc7e,	// LSC5_Ind_MiddleTmp     gain r

0xbd00,	// LSC0_Ind_HighTmp       offset g
0xbe00,	// LSC1_Ind_HighTmp       offset b
0xbf00,	// LSC2_Ind_HighTmp       offset r
0xc080,	// LSC3_Ind_HighTmp       gain g
0xc180,	// LSC4_Ind_HighTmp       gain b
0xc27e,	// LSC5_Ind_HighTmp       gain r

0xc300,	// LSC0_Dark1_LowTmp      offset g
0xc400,	// LSC1_Dark1_LowTmp      offset b
0xc500,	// LSC2_Dark1_LowTmp      offset r
0xc668,	// LSC3_Dark1_LowTmp      gain g
0xc768,	// LSC4_Dark1_LowTmp      gain b
0xc868,	// LSC5_Dark1_LowTmp      gain r

0xc900,	// LSC0_Dark1_MiddleTmp   offset g
0xca00,	// LSC1_Dark1_MiddleTmp   offset b
0xcb00,	// LSC2_Dark1_MiddleTmp   offset r
0xcc68,	// LSC3_Dark1_MiddleTmp   gain g
0xcd68,	// LSC4_Dark1_MiddleTmp   gain b
0xce68,	// LSC5_Dark1_MiddleTmp   gain r

0xcf00,	// LSC0_Dark1_HighTmp   offset g
0xd000,	// LSC1_Dark1_HighTmp   offset b
0xd100,	// LSC2_Dark1_HighTmp   offset r
0xd268,	// LSC3_Dark1_HighTmp   gain g
0xd368,	// LSC4_Dark1_HighTmp   gain b
0xd468,	// LSC5_Dark1_HighTmp   gain r

0xd500,	// LSC0_Dark2           offset g
0xd600,	// LSC1_Dark2           offset b
0xd700,	// LSC2_Dark2           offset r
0xd868,	// LSC3_Dark2           gain g
0xd968,	// LSC4_Dark2           gain b
0xda68,	// LSC5_Dark2           gain r

0xdb2f, //CMCSIGN_Out
0xdc55, //CMC_Out_00
0xdd1c, //CMC_Out_01
0xde07, //CMC_Out_02
0xdf0a, //CMC_Out_03
0xe051, //CMC_Out_04
0xe107, //CMC_Out_05
0xe201, //CMC_Out_06
0xe314, //CMC_Out_07
0xe455, //CMC_Out_08

0xe504,	// CMC_Out_LumTh1      CMC SP gain axis X(luminance)
0xe60a,	// CMC_Out_LumTh2
0xe710,	// CMC_Out_LumTh3
0xe818,	// CMC_Out_LumTh4
0xe920,	// CMC_Out_LumTh5
0xea28,	// CMC_Out_LumTh6
0xeb40,	// CMC_Out_LumTh7

0xec20,	// CMC_Out_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0xed20,	// CMC_Out_LumGain2_R
0xee20,	// CMC_Out_LumGain3_R
0xef20,	// CMC_Out_LumGain4_R
0xf020,	// CMC_Out_LumGain5_R
0xf120,	// CMC_Out_LumGain6_R
0xf220,	// CMC_Out_LumGain7_R
0xf320,	// CMC_Out_LumGain8_R    20 = x1.0

0xf420,	// CMC_Out_LumGain1_G  CMC SP G gain axis Y (gain):: max32
0xf520,	// CMC_Out_LumGain2_G
0xf620,	// CMC_Out_LumGain3_G
0xf720,	// CMC_Out_LumGain4_G
0xf820,	// CMC_Out_LumGain5_G
0xf920,	// CMC_Out_LumGain6_G
0xfa20,	// CMC_Out_LumGain7_G
0xfb20,	// CMC_Out_LumGain8_G    20 = x1.0

0xfc20,	// CMC_Out_LumGain1_B  CMC SP B gain axis Y (gain):: max32
0xfd20,	// CMC_Out_LumGain2_B
0x0e00, // burst end

0x03d4,	// page D4
0x0e01, // burst start

0x1020,	// CMC_Out_LumGain3_B
0x1120,	// CMC_Out_LumGain4_B
0x1220,	// CMC_Out_LumGain5_B
0x1320,	// CMC_Out_LumGain6_B
0x1420,	// CMC_Out_LumGain7_B
0x1520,	// CMC_Out_LumGain8_B    20 = x1.0

0x162f, //CMCSIGN_In_Mid
0x1753, //CMC_In_Mid_00
0x1816, //CMC_In_Mid_01
0x1903, //CMC_In_Mid_02
0x1a10, //CMC_In_Mid_03
0x1b53, //CMC_In_Mid_04
0x1c03, //CMC_In_Mid_05
0x1d04, //CMC_In_Mid_06
0x1e1d, //CMC_In_Mid_07
0x1f61, //CMC_In_Mid_08

0x2004,	// CMC_Ind_LumTh1     CMC SP gain axis X(luminance)
0x210a,	// CMC_Ind_LumTh2
0x2210,	// CMC_Ind_LumTh3
0x2318,	// CMC_Ind_LumTh4
0x2420,	// CMC_Ind_LumTh5
0x2528,	// CMC_Ind_LumTh6
0x2640,	// CMC_Ind_LumTh7

0x2708,	// CMC_Ind_LumGain1_R   CMC SP R gain axis Y (gain):: max32
0x2812,	// CMC_Ind_LumGain2_R
0x2918,	// CMC_Ind_LumGain3_R
0x2a1c,	// CMC_Ind_LumGain4_R
0x2b1e,	// CMC_Ind_LumGain5_R
0x2c20,	// CMC_Ind_LumGain6_R
0x2d20,	// CMC_Ind_LumGain7_R
0x2e20,	// CMC_Ind_LumGain8_R    20 = x1.0

0x2f08,	// CMC_Ind_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x3012,	// CMC_Ind_LumGain2_G
0x3118,	// CMC_Ind_LumGain3_G
0x321c,	// CMC_Ind_LumGain4_G
0x331e,	// CMC_Ind_LumGain5_G
0x3420,	// CMC_Ind_LumGain6_G
0x3520,	// CMC_Ind_LumGain7_G
0x3620,	// CMC_Ind_LumGain8_G    20 = x1.0

0x3708,	// CMC_Ind_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x3812,	// CMC_Ind_LumGain2_B
0x3918,	// CMC_Ind_LumGain3_B
0x3a1c,	// CMC_Ind_LumGain4_B
0x3b1e,	// CMC_Ind_LumGain5_B
0x3c20,	// CMC_Ind_LumGain6_B
0x3d20,	// CMC_Ind_LumGain7_B
0x3e20,	// CMC_Ind_LumGain8_B   20 = x1.0

0x3f2f, //CMCSIGN_Dark1
0x4053, //CMC_Dark1_00
0x411c, //CMC_Dark1_01
0x4209, //CMC_Dark1_02
0x430e, //CMC_Dark1_03
0x4453, //CMC_Dark1_04
0x4505, //CMC_Dark1_05
0x4603, //CMC_Dark1_06
0x4723, //CMC_Dark1_07
0x4866, //CMC_Dark1_08

0x4904,	// CMC_Dark1_LumTh1     CMC SP gain axis X(luminance)
0x4a0a,	// CMC_Dark1_LumTh2
0x4b10,	// CMC_Dark1_LumTh3
0x4c18,	// CMC_Dark1_LumTh4
0x4d20,	// CMC_Dark1_LumTh5
0x4e28,	// CMC_Dark1_LumTh6
0x4f40,	// CMC_Dark1_LumTh7

0x5008,	// CMC_Dark1_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0x5112,	// CMC_Dark1_LumGain2_R
0x5218,	// CMC_Dark1_LumGain3_R
0x531c,	// CMC_Dark1_LumGain4_R
0x541e,	// CMC_Dark1_LumGain5_R
0x5520,	// CMC_Dark1_LumGain6_R
0x5620,	// CMC_Dark1_LumGain7_R
0x5720,	// CMC_Dark1_LumGain8_R    20 = x1.0

0x5808,	// CMC_Dark1_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x5912,	// CMC_Dark1_LumGain2_G
0x5a18,	// CMC_Dark1_LumGain3_G
0x5b1c,	// CMC_Dark1_LumGain4_G
0x5c1e,	// CMC_Dark1_LumGain5_G
0x5d20,	// CMC_Dark1_LumGain6_G
0x5e20,	// CMC_Dark1_LumGain7_G
0x5f20,	// CMC_Dark1_LumGain8_G    20 = x1.0

0x6008,	// CMC_Dark1_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x6112,	// CMC_Dark1_LumGain2_B
0x6218,	// CMC_Dark1_LumGain3_B
0x631c,	// CMC_Dark1_LumGain4_B
0x641e,	// CMC_Dark1_LumGain5_B
0x6520,	// CMC_Dark1_LumGain6_B
0x6620,	// CMC_Dark1_LumGain7_B
0x6720,	// CMC_Dark1_LumGain8_B   20 = x1.0

0x682f, //CMCSIGN_Dark2
0x6953, //CMC_Dark2_00
0x6a1c, //CMC_Dark2_01
0x6b09, //CMC_Dark2_02
0x6c0e, //CMC_Dark2_03
0x6d53, //CMC_Dark2_04
0x6e05, //CMC_Dark2_05
0x6f03, //CMC_Dark2_06
0x7023, //CMC_Dark2_07
0x7166, //CMC_Dark2_08

0x7204,	// CMC_Dark2_LumTh1        CMC SP gain axis X(luminance)
0x730a,	// CMC_Dark2_LumTh2
0x7410,	// CMC_Dark2_LumTh3
0x7518,	// CMC_Dark2_LumTh4
0x7620,	// CMC_Dark2_LumTh5
0x7728,	// CMC_Dark2_LumTh6
0x7840,	// CMC_Dark2_LumTh7

0x7908,	// CMC_Dark2_LumGain1_R    CMC SP R gain
0x7a12,	// CMC_Dark2_LumGain2_R
0x7b18,	// CMC_Dark2_LumGain3_R
0x7c1c,	// CMC_Dark2_LumGain4_R
0x7d1e,	// CMC_Dark2_LumGain5_R
0x7e20,	// CMC_Dark2_LumGain6_R
0x7f20,	// CMC_Dark2_LumGain7_R
0x8020,	// CMC_Dark2_LumGain8_R    20 = x1.

0x8108,	// CMC_Dark2_LumGain1_G    CMC SP G gain
0x8212,	// CMC_Dark2_LumGain2_G
0x8318,	// CMC_Dark2_LumGain3_G
0x841c,	// CMC_Dark2_LumGain4_G
0x851e,	// CMC_Dark2_LumGain5_G
0x8620,	// CMC_Dark2_LumGain6_G
0x8720,	// CMC_Dark2_LumGain7_G
0x8820,	// CMC_Dark2_LumGain8_G    20 = x1.

0x8908,	// CMC_Dark2_LumGain1_B    CMC SP B gain
0x8a12,	// CMC_Dark2_LumGain2_B
0x8b18,	// CMC_Dark2_LumGain3_B
0x8c1c,	// CMC_Dark2_LumGain4_B
0x8d1e,	// CMC_Dark2_LumGain5_B
0x8e20,	// CMC_Dark2_LumGain6_B
0x8f20,	// CMC_Dark2_LumGain7_B
0x9020,	// CMC_Dark2_LumGain8_B    20 = x1.0

0x912f, // CMCSIGN_In_Low
0x9253, // CMC_In_Low_00
0x931e, //CMC_In_Low_01
0x940b, //CMC_In_Low_02
0x9518, //CMC_In_Low_03
0x9661, // CMC_In_Low_04
0x9709, //CMC_In_Low_05
0x9804, //CMC_In_Low_06
0x9914, //CMC_In_Low_07
0x9a58, // CMC_In_Low_08

0x9b04,	// CMC_LowTemp_LumTh1     CMC SP gain axis X(luminance)
0x9c0a,	// CMC_LowTemp_LumTh2
0x9d10,	// CMC_LowTemp_LumTh3
0x9e18,	// CMC_LowTemp_LumTh4
0x9f20,	// CMC_LowTemp_LumTh5
0xa028,	// CMC_LowTemp_LumTh6
0xa140,	// CMC_LowTemp_LumTh7

0xa220,	// CMC_LowTemp_LumGain1_R    CMC SP R gain
0xa320,	// CMC_LowTemp_LumGain2_R
0xa420,	// CMC_LowTemp_LumGain3_R
0xa520,	// CMC_LowTemp_LumGain4_R
0xa620,	// CMC_LowTemp_LumGain5_R
0xa720,	// CMC_LowTemp_LumGain6_R
0xa820,	// CMC_LowTemp_LumGain7_R
0xa920,	// CMC_LowTemp_LumGain8_R    20 = x1.0

0xaa20,	// CMC_LowTemp_LumGain1_G    CMC SP G gain
0xab20,	// CMC_LowTemp_LumGain2_G
0xac20,	// CMC_LowTemp_LumGain3_G
0xad20,	// CMC_LowTemp_LumGain4_G
0xae20,	// CMC_LowTemp_LumGain5_G
0xaf20,	// CMC_LowTemp_LumGain6_G
0xb020,	// CMC_LowTemp_LumGain7_G
0xb120,	// CMC_LowTemp_LumGain8_G    20 = x1.0

0xb220,	// CMC_LowTemp_LumGain1_B    CMC SP B gain
0xb320,	// CMC_LowTemp_LumGain2_B
0xb420,	// CMC_LowTemp_LumGain3_B
0xb520,	// CMC_LowTemp_LumGain4_B
0xb620,	// CMC_LowTemp_LumGain5_B
0xb720,	// CMC_LowTemp_LumGain6_B
0xb820,	// CMC_LowTemp_LumGain7_B
0xb920,	// CMC_LowTemp_LumGain8_B    20 = x1.0

0xba2d, //CMCSIGN_In_High
0xbb55, //CMC_In_High_00
0xbc21, //CMC_In_High_01
0xbd0c, //CMC_In_High_02
0xbe08, //CMC_In_High_03
0xbf55, //CMC_In_High_04
0xc00d, //CMC_In_High_05
0xc103, //CMC_In_High_06
0xc218, //CMC_In_High_07
0xc355, //CMC_In_High_08

0xc404,	// CMC_HighTemp_LumTh1       CMC SP gain axis X(luminance)
0xc50a,	// CMC_HighTemp_LumTh2
0xc610,	// CMC_HighTemp_LumTh3
0xc718,	// CMC_HighTemp_LumTh4
0xc820,	// CMC_HighTemp_LumTh5
0xc928,	// CMC_HighTemp_LumTh6
0xca40,	// CMC_HighTemp_LumTh7

0xcb20,	// CMC_HighTemp_LumGain1_R   CMC SP R gain
0xcc20,	// CMC_HighTemp_LumGain2_R
0xcd20,	// CMC_HighTemp_LumGain3_R
0xce20,	// CMC_HighTemp_LumGain4_R
0xcf20,	// CMC_HighTemp_LumGain5_R
0xd020,	// CMC_HighTemp_LumGain6_R
0xd120,	// CMC_HighTemp_LumGain7_R
0xd220,	// CMC_HighTemp_LumGain8_R    20 = x1.0

0xd320,	// CMC_HighTemp_LumGain1_G   CMC SP G gain
0xd420,	// CMC_HighTemp_LumGain2_G
0xd520,	// CMC_HighTemp_LumGain3_G
0xd620,	// CMC_HighTemp_LumGain4_G
0xd720,	// CMC_HighTemp_LumGain5_G
0xd820,	// CMC_HighTemp_LumGain6_G
0xd920,	// CMC_HighTemp_LumGain7_G
0xda20,	// CMC_HighTemp_LumGain8_G    20 = x1.

0xdb20,	// CMC_HighTemp_LumGain1_B   CMC SP B gain
0xdc20,	// CMC_HighTemp_LumGain2_B
0xdd20,	// CMC_HighTemp_LumGain3_B
0xde20,	// CMC_HighTemp_LumGain4_B
0xdf20,	// CMC_HighTemp_LumGain5_B
0xe020,	// CMC_HighTemp_LumGain6_B
0xe120,	// CMC_HighTemp_LumGain7_B
0xe220,	// CMC_HighTemp_LumGain8_B   20 = x1.0

////////////////////
// Adaptive Gamma //
////////////////////

0xe300,	// GMA_OUT
0xe403,
0xe508,
0xe60f,
0xe715,
0xe820,
0xe92d,
0xea3b,
0xeb47,
0xec53,
0xed5c,
0xee65,
0xef6d,
0xf075,
0xf17c,
0xf282,
0xf388,
0xf48d,
0xf591,
0xf696,
0xf79a,
0xf8a1,
0xf9a9,
0xfab0,
0xfbbe,
0xfcc8,
0xfdd2,

0x0e00, // burst end

0x03d5,	// Page d5

0x0e01, // burst start

0x10db,
0x11e3,
0x12ea,
0x13f0,
0x14f5,
0x15fb,
0x16ff,

0x1700,	//GMA_IN
0x1803,
0x1908,
0x1a0f,
0x1b15,
0x1c20,
0x1d2d,
0x1e3b,
0x1f47,
0x2053,
0x215c,
0x2265,
0x236d,
0x2475,
0x257c,
0x2682,
0x2788,
0x288d,
0x2991,
0x2a96,
0x2b9a,
0x2ca1,
0x2da9,
0x2eb0,
0x2fbe,
0x30c8,
0x31d2,
0x32db,
0x33e3,
0x34ea,
0x35f0,
0x36f5,
0x37fb,
0x38ff,

0x3900,	// GMA_D1
0x3a06,
0x3b0e,
0x3c17,
0x3d1f,
0x3e2c,
0x3f3c,
0x404c,
0x4157,
0x4261,
0x436b,
0x4476,
0x457e,
0x4685,
0x478c,
0x4892,
0x4998,
0x4a9c,
0x4ba2,
0x4ca6,
0x4dab,
0x4eb4,
0x4fbc,
0x50c2,
0x51cb,
0x52d3,
0x53db,
0x54e1,
0x55e7,
0x56ed,
0x57f3,
0x58f8,
0x59fc,
0x5aff,

0x5b00,	// GMA_D2
0x5c06,
0x5d0e,
0x5e17,
0x5f1f,
0x602c,
0x613c,
0x624c,
0x6357,
0x6461,
0x656b,
0x6676,
0x677e,
0x6885,
0x698c,
0x6a92,
0x6b98,
0x6c9c,
0x6da2,
0x6ea6,
0x6fab,
0x70b4,
0x71bc,
0x72c2,
0x73cb,
0x74d3,
0x75db,
0x76e1,
0x77e7,
0x78ed,
0x79f3,
0x7af8,
0x7bfc,
0x7cff,

///////////////////
// Adaptive MCMC //
///////////////////

// Outdoor MCMC
0x7d15, //Outdoor_delta1
0x7e19, //Outdoor_center1
0x7f0f, //Outdoor_delta2
0x8086, //Outdoor_center2
0x8117, //Outdoor_delta3
0x82bd, //Outdoor_center3
0x8317, //Outdoor_delta4
0x84ee, //Outdoor_center4
0x8593, //Outdoor_delta5
0x8625, //Outdoor_center5
0x8793, //Outdoor_delta6
0x8851, //Outdoor_center6
                 
0x8940, // Outdoor_sat_gain1
0x8a40, // Outdoor_sat_gain2
0x8b40, // Outdoor_sat_gain3
0x8c40, // Outdoor_sat_gain4
0x8d40, // Outdoor_sat_gain5
0x8e40, // Outdoor_sat_gain6
0x8f94, // Outdoor_hue_angle1
0x9089, //Outdoor_hue_angle2
0x9110, //Outdoor_hue_angle3
0x9214, //Outdoor_hue_angle4
0x930b, //Outdoor_hue_angle5
0x9487, //Outdoor_hue_angle6

0x9500,	// MCMC24_Outdoor  mcmc_rgb_ofs_sign_r
0x9600,	// MCMC25_Outdoor  mcmc_rgb_ofs_sign_g
0x9700,	// MCMC26_Outdoor  mcmc_rgb_ofs_sign_b

0x9800,	// MCMC27_Outdoor  mcmc_rgb_ofs_r1 R
0x9900,	// MCMC28_Outdoor  mcmc_rgb_ofs_r1 G
0x9a00,	// MCMC29_Outdoor  mcmc_rgb_ofs_r1 B

0x9b00,	// MCMC30_Outdoor  mcmc_rgb_ofs_r2 R
0x9c00,	// MCMC31_Outdoor  mcmc_rgb_ofs_r2 G
0x9d00,	// MCMC32_Outdoor  mcmc_rgb_ofs_r2 B

0x9e00,	// MCMC33_Outdoor  mcmc_rgb_ofs_r3 R
0x9f00,	// MCMC34_Outdoor  mcmc_rgb_ofs_r3 G
0xa000,	// MCMC35_Outdoor  mcmc_rgb_ofs_r3 B

0xa100,	// MCMC36_Outdoor  mcmc_rgb_ofs_r4 R
0xa200,	// MCMC37_Outdoor  mcmc_rgb_ofs_r4 G
0xa300,	// MCMC38_Outdoor  mcmc_rgb_ofs_r4 B

0xa400,	// MCMC39_Outdoor  mcmc_rgb_ofs_r5 R
0xa500,	// MCMC40_Outdoor  mcmc_rgb_ofs_r5 G
0xa600,	// MCMC41_Outdoor  mcmc_rgb_ofs_r5 B

0xa700,	// MCMC42_Outdoor  mcmc_rgb_ofs_r6 R
0xa800,	// MCMC43_Outdoor  mcmc_rgb_ofs_r6 G
0xa900,	// MCMC44_Outdoor  mcmc_rgb_ofs_r6 B

0xaa00,	// MCMC45_Outdoor  mcmc_std_offset1
0xab00,	// MCMC46_Outdoor  mcmc_std_offset2
0xacff,	// MCMC47_Outdoor  mcmc_std_th_max
0xad00,	// MCMC48_Outdoor  mcmc_std_th_min

0xae3f,	// MCMC49_Outdoor  mcmc_lum_gain_wgt_th1 R1 magenta
0xaf3f,	// MCMC50_Outdoor  mcmc_lum_gain_wgt_th2 R1
0xb03f,	// MCMC51_Outdoor  mcmc_lum_gain_wgt_th3 R1
0xb13f,	// MCMC52_Outdoor  mcmc_lum_gain_wgt_th4 R1
0xb230,	// MCMC53_Outdoor  mcmc_rg1_lum_sp1      R1
0xb350,	// MCMC54_Outdoor  mcmc_rg1_lum_sp2      R1
0xb480,	// MCMC55_Outdoor  mcmc_rg1_lum_sp3      R1
0xb5b0,	// MCMC56_Outdoor  mcmc_rg1_lum_sp4      R1

0xb63f,	// MCMC57_Outdoor  mcmc_lum_gain_wgt_th1 R2 red
0xb73f,	// MCMC58_Outdoor  mcmc_lum_gain_wgt_th2 R2
0xb83f,	// MCMC59_Outdoor  mcmc_lum_gain_wgt_th3 R2
0xb93f,	// MCMC60_Outdoor  mcmc_lum_gain_wgt_th4 R2
0xba28,	// MCMC61_Outdoor  mcmc_rg2_lum_sp1      R2
0xbb50,	// MCMC62_Outdoor  mcmc_rg2_lum_sp2      R2
0xbc80,	// MCMC63_Outdoor  mcmc_rg2_lum_sp3      R2
0xbdb0,	// MCMC64_Outdoor  mcmc_rg2_lum_sp4      R2

0xbe3f,	// MCMC65_Outdoor  mcmc_lum_gain_wgt_th1 R3 yellow
0xbf3f,	// MCMC66_Outdoor  mcmc_lum_gain_wgt_th2 R3
0xc030,// MCMC67_Outdoor  mcmc_lum_gain_wgt_th3 R3
0xc12a,// MCMC68_Outdoor  mcmc_lum_gain_wgt_th4 R3
0xc220,// MCMC69_Outdoor  mcmc_rg3_lum_sp1      R3
0xc340,// MCMC70_Outdoor  mcmc_rg3_lum_sp2      R3
0xc470,// MCMC71_Outdoor  mcmc_rg3_lum_sp3      R3
0xc5b0,	// MCMC72_Outdoor  mcmc_rg3_lum_sp4      R3

0xc63f,	// MCMC73_Outdoor  mcmc_lum_gain_wgt_th1 R4 Green
0xc73f,	// MCMC74_Outdoor  mcmc_lum_gain_wgt_th2 R4
0xc83f,	// MCMC75_Outdoor  mcmc_lum_gain_wgt_th3 R4
0xc93f,	// MCMC76_Outdoor  mcmc_lum_gain_wgt_th4 R4
0xca10,	// MCMC77_Outdoor  mcmc_rg4_lum_sp1      R4
0xcb30,	// MCMC78_Outdoor  mcmc_rg4_lum_sp2      R4
0xcc60,	// MCMC79_Outdoor  mcmc_rg4_lum_sp3      R4
0xcd90,	// MCMC80_Outdoor  mcmc_rg4_lum_sp4      R4

0xce3f,	// MCMC81_Outdoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0xcf3f,	// MCMC82_Outdoor  mcmc_rg5_gain_wgt_th2 R5
0xd03f,	// MCMC83_Outdoor  mcmc_rg5_gain_wgt_th3 R5
0xd13f,	// MCMC84_Outdoor  mcmc_rg5_gain_wgt_th4 R5
0xd228,	// MCMC85_Outdoor  mcmc_rg5_lum_sp1      R5
0xd350,	// MCMC86_Outdoor  mcmc_rg5_lum_sp2      R5
0xd480,	// MCMC87_Outdoor  mcmc_rg5_lum_sp3      R5
0xd5b0,	// MCMC88_Outdoor  mcmc_rg5_lum_sp4      R5

0xd63f,	// MCMC89_Outdoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0xd73f,	// MCMC90_Outdoor  mcmc_rg6_gain_wgt_th2 R6
0xd83f,	// MCMC91_Outdoor  mcmc_rg6_gain_wgt_th3 R6
0xd93f,	// MCMC92_Outdoor  mcmc_rg6_gain_wgt_th4 R6
0xda28,	// MCMC93_Outdoor  mcmc_rg6_lum_sp1      R6
0xdb50,	// MCMC94_Outdoor  mcmc_rg6_lum_sp2      R6
0xdc80,	// MCMC95_Outdoor  mcmc_rg6_lum_sp3      R6
0xddb0,	// MCMC96_Outdoor  mcmc_rg6_lum_sp4      R6

0xde1e,	// MCMC97_Outdoor  mcmc2_allgain_x1
0xdf3c,	// MCMC98_Outdoor  mcmc2_allgain_x2
0xe03c,	// MCMC99_Outdoor  mcmc2_allgain_x4
0xe11e,	// MCMC100_Outdoor mcmc2_allgain_x5
0xe21e,	// MCMC101_Outdoor mcmc2_allgain_x7
0xe33c,	// MCMC102_Outdoor mcmc2_allgain_x8
0xe43c,	// MCMC103_Outdoor mcmc2_allgain_x10
0xe51e,	// MCMC104_Outdoor mcmc2_allgain_x11

0xe616, //Outdoor_allgain_y1
0xe716, //Outdoor_allgain_y2
0xe815, //Outdoor_allgain_y3
0xe914, //Outdoor_allgain_y4
0xea14, //Outdoor_allgain_y5
0xeb16, //Outdoor_allgain_y6
0xec18, //Outdoor_allgain_y7
0xed1a, //Outdoor_allgain_y8
0xee1c, //Outdoor_allgain_y9
0xef19, //Outdoor_allgain_y10
0xf014, //Outdoor_allgain_y11
0xf114, //Outdoor_allgain_y12

// Indoor MCMC
0xf210, // Indoor_delta1
0xf31e, // Indoor_center1
0xf40b, //Indoor_delta2
0xf56f, //Indoor_center2
0xf610, // Indoor_delta3
0xf79c, // Indoor_center3
0xf807, // Indoor_delta4
0xf9b8, // Indoor_center4
0xfa90, //Indoor_delta5
0xfb2d, //Indoor_center5
0xfc92, //Indoor_delta6
0xfd4f, //Indoor_center6
0x0e00, // burst end

0x03d6,	// Page D6

0x0e01, // burst start

0x1040, //Indoor_sat_gain1
0x1140, //Indoor_sat_gain2
0x1240, //Indoor_sat_gain3
0x1340, //Indoor_sat_gain4
0x1440, //Indoor_sat_gain5
0x1540, //Indoor_sat_gain6

0x1600, //Indoor_hue_angle1
0x178a, //Indoor_hue_angle2
0x1800, //Indoor_hue_angle3
0x1900, //Indoor_hue_angle4
0x1a00, //Indoor_hue_angle5
0x1b02, //Indoor_hue_angle6

0x1c00,	// MCMC24_Indoor   mcmc_rgb_ofs_sign_r
0x1d00,	// MCMC25_Indoor   mcmc_rgb_ofs_sign_g
0x1e00,	// MCMC26_Indoor   mcmc_rgb_ofs_sign_b

0x1f00,	// MCMC27_Indoor   mcmc_rgb_ofs_r1 R
0x2000,	// MCMC28_Indoor   mcmc_rgb_ofs_r1 G
0x2100,	// MCMC29_Indoor   mcmc_rgb_ofs_r1 B

0x2200,	// MCMC30_Indoor   mcmc_rgb_ofs_r2 R
0x2300,	// MCMC31_Indoor   mcmc_rgb_ofs_r2 G
0x2400,	// MCMC32_Indoor   mcmc_rgb_ofs_r2 B

0x2500,	// MCMC33_Indoor   mcmc_rgb_ofs_r3 R
0x2600,	// MCMC34_Indoor   mcmc_rgb_ofs_r3 G
0x2700,	// MCMC35_Indoor   mcmc_rgb_ofs_r3 B

0x2800,	// MCMC36_Indoor   mcmc_rgb_ofs_r4 R
0x2900,	// MCMC37_Indoor   mcmc_rgb_ofs_r4 G
0x2a00,	// MCMC38_Indoor   mcmc_rgb_ofs_r4 B

0x2b00,	// MCMC39_Indoor   mcmc_rgb_ofs_r5 R
0x2c00,	// MCMC40_Indoor   mcmc_rgb_ofs_r5 G
0x2d00,	// MCMC41_Indoor   mcmc_rgb_ofs_r5 B

0x2e00,	// MCMC42_Indoor  mcmc_rgb_ofs_r6 R
0x2f00,	// MCMC43_Indoor  mcmc_rgb_ofs_r6 G
0x3000,	// MCMC44_Indoor  mcmc_rgb_ofs_r6 B

0x3100,	// MCMC45_Indoor  mcmc_std_offset1
0x3200,	// MCMC46_Indoor  mcmc_std_offset2
0x33ff,	// MCMC47_Indoor  mcmc_std_th_max
0x3400,	// MCMC48_Indoor  mcmc_std_th_min

0x3510,	// MCMC49_Indoor  mcmc_lum_gain_wgt_th1 R1 magenta
0x3621,	// MCMC50_Indoor  mcmc_lum_gain_wgt_th2 R1
0x3734,	// MCMC51_Indoor  mcmc_lum_gain_wgt_th3 R1
0x383f,	// MCMC52_Indoor  mcmc_lum_gain_wgt_th4 R1
0x3908,	// MCMC53_Indoor  mcmc_rg1_lum_sp1      R1
0x3a15,	// MCMC54_Indoor  mcmc_rg1_lum_sp2      R1
0x3b2f,	// MCMC55_Indoor  mcmc_rg1_lum_sp3      R1
0x3c51,	// MCMC56_Indoor  mcmc_rg1_lum_sp4      R1

0x3d3f,	// MCMC57_Indoor  mcmc_lum_gain_wgt_th1 R2 red
0x3e3f,	// MCMC58_Indoor  mcmc_lum_gain_wgt_th2 R2
0x3f3f,	// MCMC59_Indoor  mcmc_lum_gain_wgt_th3 R2
0x403f,	// MCMC60_Indoor  mcmc_lum_gain_wgt_th4 R2
0x4128,	// MCMC61_Indoor  mcmc_rg2_lum_sp1      R2
0x4250,	// MCMC62_Indoor  mcmc_rg2_lum_sp2      R2
0x4380,	// MCMC63_Indoor  mcmc_rg2_lum_sp3      R2
0x44b0,	// MCMC64_Indoor  mcmc_rg2_lum_sp4      R2

0x453f,	// MCMC65_Indoor  mcmc_lum_gain_wgt_th1 R3 yellow
0x463f,	// MCMC66_Indoor  mcmc_lum_gain_wgt_th2 R3
0x473f,	// MCMC67_Indoor  mcmc_lum_gain_wgt_th3 R3
0x483f,	// MCMC68_Indoor  mcmc_lum_gain_wgt_th4 R3
0x4928,	// MCMC69_Indoor  mcmc_rg3_lum_sp1      R3
0x4a50,	// MCMC70_Indoor  mcmc_rg3_lum_sp2      R3
0x4b80,	// MCMC71_Indoor  mcmc_rg3_lum_sp3      R3
0x4cb0,	// MCMC72_Indoor  mcmc_rg3_lum_sp4      R3

0x4d3f,	// MCMC73_Indoor  mcmc_lum_gain_wgt_th1 R4 Green
0x4e3f,	// MCMC74_Indoor  mcmc_lum_gain_wgt_th2 R4
0x4f3f,	// MCMC75_Indoor  mcmc_lum_gain_wgt_th3 R4
0x503f,	// MCMC76_Indoor  mcmc_lum_gain_wgt_th4 R4
0x5110,	// MCMC77_Indoor  mcmc_rg4_lum_sp1      R4
0x5230,	// MCMC78_Indoor  mcmc_rg4_lum_sp2      R4
0x5360,	// MCMC79_Indoor  mcmc_rg4_lum_sp3      R4
0x5490,	// MCMC80_Indoor  mcmc_rg4_lum_sp4      R4

0x553f,	// MCMC81_Indoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0x563f,	// MCMC82_Indoor  mcmc_rg5_gain_wgt_th2 R5
0x573f,	// MCMC83_Indoor  mcmc_rg5_gain_wgt_th3 R5
0x583f,	// MCMC84_Indoor  mcmc_rg5_gain_wgt_th4 R5
0x5928,	// MCMC85_Indoor  mcmc_rg5_lum_sp1      R5
0x5a50,	// MCMC86_Indoor  mcmc_rg5_lum_sp2      R5
0x5b80,	// MCMC87_Indoor  mcmc_rg5_lum_sp3      R5
0x5cb0,	// MCMC88_Indoor  mcmc_rg5_lum_sp4      R5

0x5d3f,	// MCMC89_Indoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0x5e3f,	// MCMC90_Indoor  mcmc_rg6_gain_wgt_th2 R6
0x5f3f,	// MCMC91_Indoor  mcmc_rg6_gain_wgt_th3 R6
0x603f,	// MCMC92_Indoor  mcmc_rg6_gain_wgt_th4 R6
0x6128,	// MCMC93_Indoor  mcmc_rg6_lum_sp1      R6
0x6250,	// MCMC94_Indoor  mcmc_rg6_lum_sp2      R6
0x6380,	// MCMC95_Indoor  mcmc_rg6_lum_sp3      R6
0x64b0,	// MCMC96_Indoor  mcmc_rg6_lum_sp4      R6

0x651d,	// MCMC97_Indoor  mcmc2_allgain_x1
0x663b,	// MCMC98_Indoor  mcmc2_allgain_x2
0x673b,	// MCMC99_Indoor  mcmc2_allgain_x4
0x681d,	// MCMC100_Indoor mcmc2_allgain_x5
0x691d,	// MCMC101_Indoor mcmc2_allgain_x7
0x6a3b,	// MCMC102_Indoor mcmc2_allgain_x8
0x6b3b,	// MCMC103_Indoor mcmc2_allgain_x10
0x6c1d,	// MCMC104_Indoor mcmc2_allgain_x11

0x6d0e,	// MCMC105_Indoor mcmc2_allgain_y0
0x6e0f,	// MCMC106_Indoor mcmc2_allgain_y1
0x6f0f,	// MCMC107_Indoor mcmc2_allgain_y2
0x700f,	// MCMC108_Indoor mcmc2_allgain_y3
0x710f,	// MCMC109_Indoor mcmc2_allgain_y4
0x7210,	// MCMC110_Indoor mcmc2_allgain_y5
0x7310,	// MCMC111_Indoor mcmc2_allgain_y6
0x7410,	// MCMC112_Indoor mcmc2_allgain_y7
0x7510,	// MCMC113_Indoor mcmc2_allgain_y8
0x760f,	// MCMC114_Indoor mcmc2_allgain_y9
0x770e,	// MCMC115_Indoor mcmc2_allgain_y10
0x780d,	// MCMC116_Indoor mcmc2_allgain_y11

// Dark1 MCMC
0x7917, //Dark1_delta1
0x7a56, //Dark1_center1
0x7b10, //Dark1_delta2
0x7c70, //Dark1_center2
0x7d10, //Dark1_delta3
0x7e9c, //Dark1_center3
0x7f18, //Dark1_delta4
0x80db, //Dark1_center4
0x8198, //Dark1_delta5
0x8226, //Dark1_center5
0x8399, //Dark1_delta6
0x845b, //Dark1_center6

0x8540, //Dark1_sat_gain1
0x8640, //Dark1_sat_gain2
0x8740, //Dark1_sat_gain3
0x8840, //Dark1_sat_gain4
0x8940, //Dark1_sat_gain5
0x8a40, //Dark1_sat_gain6
0x8b91, //Dark1_hue_angle1
0x8c00, //Dark1_hue_angle2
0x8d00, //Dark1_hue_angle3
0x8e0a, //Dark1_hue_angle4
0x8f05, //Dark1_hue_angle5
0x9086, //Dark1_hue_angle6

0x913f,	// MCMC24_Dark1   mcmc_rgb_ofs_sign
0x923f,	// MCMC25_Dark1   mcmc_rgb_ofs_sign
0x933f,	// MCMC26_Dark1   mcmc_rgb_ofs_sign

0x9400,	// MCMC27_Dark1   mcmc_rgb_ofs_r1 R
0x9500,	// MCMC28_Dark1   mcmc_rgb_ofs_r1 G
0x9600,	// MCMC29_Dark1   mcmc_rgb_ofs_r1 B

0x9700,	// MCMC30_Dark1   mcmc_rgb_ofs_r2 R
0x9800,	// MCMC31_Dark1   mcmc_rgb_ofs_r2 G
0x9900,	// MCMC32_Dark1   mcmc_rgb_ofs_r2 B

0x9a00,	// MCMC33_Dark1   mcmc_rgb_ofs_r3 R
0x9b00,	// MCMC34_Dark1   mcmc_rgb_ofs_r3 G
0x9c00,	// MCMC35_Dark1   mcmc_rgb_ofs_r3 B

0x9d00,	// MCMC36_Dark1   mcmc_rgb_ofs_r4 R
0x9e00,	// MCMC37_Dark1   mcmc_rgb_ofs_r4 G
0x9f00,	// MCMC38_Dark1   mcmc_rgb_ofs_r4 B

0xa000,	// MCMC39_Dark1   mcmc_rgb_ofs_r5 R
0xa100,	// MCMC40_Dark1   mcmc_rgb_ofs_r5 G
0xa200,	// MCMC41_Dark1   mcmc_rgb_ofs_r5 B

0xa300,	// MCMC42_Dark1  mcmc_rgb_ofs_r6 R
0xa400,	// MCMC43_Dark1  mcmc_rgb_ofs_r6 G
0xa500,	// MCMC44_Dark1  mcmc_rgb_ofs_r6 B

0xa600,	// MCMC45_Dark1  mcmc_std_offset1
0xa700,	// MCMC46_Dark1  mcmc_std_offset2
0xa8ff,	// MCMC47_Dark1  mcmc_std_th_max
0xa900,	// MCMC48_Dark1  mcmc_std_th_min

0xaa3f,	// MCMC49_Dark1  mcmc_lum_gain_wgt R1
0xab3f,	// MCMC50_Dark1  mcmc_lum_gain_wgt R1
0xac3f,	// MCMC51_Dark1  mcmc_lum_gain_wgt R1
0xad3f,	// MCMC52_Dark1  mcmc_lum_gain_wgt R1
0xae30,	// MCMC53_Dark1  mcmc_rg1_lum_sp1  R1
0xaf50,	// MCMC54_Dark1  mcmc_rg1_lum_sp2  R1
0xb080,	// MCMC55_Dark1  mcmc_rg1_lum_sp3  R1
0xb1b0,	// MCMC56_Dark1  mcmc_rg1_lum_sp4  R1

0xb23f,	// MCMC57_Dark1  mcmc_lum_gain_wgt R2
0xb33f,	// MCMC58_Dark1  mcmc_lum_gain_wgt R2
0xb43f,	// MCMC59_Dark1  mcmc_lum_gain_wgt R2
0xb53f,	// MCMC60_Dark1  mcmc_lum_gain_wgt R2
0xb628,	// MCMC61_Dark1  mcmc_rg2_lum_sp1  R2
0xb750,	// MCMC62_Dark1  mcmc_rg2_lum_sp2  R2
0xb880,	// MCMC63_Dark1  mcmc_rg2_lum_sp3  R2
0xb9b0,	// MCMC64_Dark1  mcmc_rg2_lum_sp4  R2

0xba3f,	// MCMC65_Dark1  mcmc_lum_gain_wgt R3
0xbb3f,	// MCMC66_Dark1  mcmc_lum_gain_wgt R3
0xbc3f,	// MCMC67_Dark1  mcmc_lum_gain_wgt R3
0xbd3f,	// MCMC68_Dark1  mcmc_lum_gain_wgt R3
0xbe28,	// MCMC69_Dark1  mcmc_rg3_lum_sp1  R3
0xbf50,	// MCMC70_Dark1  mcmc_rg3_lum_sp2  R3
0xc080,	// MCMC71_Dark1  mcmc_rg3_lum_sp3  R3
0xc1b0,	// MCMC72_Dark1  mcmc_rg3_lum_sp4  R3

0xc23f,	// MCMC73_Dark1  mcmc_lum_gain_wgt R4
0xc33f,	// MCMC74_Dark1  mcmc_lum_gain_wgt R4
0xc43f,	// MCMC75_Dark1  mcmc_lum_gain_wgt R4
0xc53f,	// MCMC76_Dark1  mcmc_lum_gain_wgt R4
0xc610,	// MCMC77_Dark1  mcmc_rg4_lum_sp1  R4
0xc730,	// MCMC78_Dark1  mcmc_rg4_lum_sp2  R4
0xc860,	// MCMC79_Dark1  mcmc_rg4_lum_sp3  R4
0xc990,	// MCMC80_Dark1  mcmc_rg4_lum_sp4  R4

0xca3f,	// MCMC81_Dark1  mcmc_rg5_gain_wgt R5
0xcb3f,	// MCMC82_Dark1  mcmc_rg5_gain_wgt R5
0xcc3f,	// MCMC83_Dark1  mcmc_rg5_gain_wgt R5
0xcd3f,	// MCMC84_Dark1  mcmc_rg5_gain_wgt R5
0xce28,	// MCMC85_Dark1  mcmc_rg5_lum_sp1  R5
0xcf50,	// MCMC86_Dark1  mcmc_rg5_lum_sp2  R5
0xd080,	// MCMC87_Dark1  mcmc_rg5_lum_sp3  R5
0xd1b0,	// MCMC88_Dark1  mcmc_rg5_lum_sp4  R5

0xd23f,	// MCMC89_Dark1  mcmc_rg6_gain_wgt R6
0xd33f,	// MCMC90_Dark1  mcmc_rg6_gain_wgt R6
0xd43f,	// MCMC91_Dark1  mcmc_rg6_gain_wgt R6
0xd53f,	// MCMC92_Dark1  mcmc_rg6_gain_wgt R6
0xd628,	// MCMC93_Dark1  mcmc_rg6_lum_sp1  R6
0xd750,	// MCMC94_Dark1  mcmc_rg6_lum_sp2  R6
0xd880,	// MCMC95_Dark1  mcmc_rg6_lum_sp3  R6
0xd9b0,	// MCMC96_Dark1  mcmc_rg6_lum_sp4  R6

0xda1c,	// MCMC97_Dark1  mcmc2_allgain_x1
0xdb3a,	// MCMC98_Dark1  mcmc2_allgain_x2
0xdc3a,	// MCMC99_Dark1  mcmc2_allgain_x4
0xdd1c,	// MCMC100_Dark1 mcmc2_allgain_x5
0xde1c,	// MCMC101_Dark1 mcmc2_allgain_x7
0xdf3a,	// MCMC102_Dark1 mcmc2_allgain_x8
0xe03a,	// MCMC103_Dark1 mcmc2_allgain_x10
0xe11c,	// MCMC104_Dark1 mcmc2_allgain_x11

0xe20f, //Dark1_allgain_y1
0xe310, //Dark1_allgain_y2
0xe410, //Dark1_allgain_y3
0xe511, //Dark1_allgain_y4
0xe610, //Dark1_allgain_y5
0xe713, //Dark1_allgain_y6
0xe812, //Dark1_allgain_y7
0xe912, //Dark1_allgain_y8
0xea12, //Dark1_allgain_y9
0xeb11, //Dark1_allgain_y10
0xec10, //Dark1_allgain_y11
0xed0f, //Dark1_allgain_y12

// Dark2 MCMC
0xee17,	// MCMC00_Dark2   mcmc_delta1
0xef56,	// MCMC01_Dark2   mcmc_center1
0xf010,	// MCMC02_Dark2   mcmc_delta2
0xf170,	// MCMC03_Dark2   mcmc_center2
0xf210,	// MCMC04_Dark2   mcmc_delta3
0xf39c,	// MCMC05_Dark2   mcmc_center3
0xf418,	// MCMC06_Dark2   mcmc_delta4
0xf5db,	// MCMC07_Dark2   mcmc_center4
0xf698,	// MCMC08_Dark2   mcmc_delta5
0xf726,	// MCMC09_Dark2   mcmc_center5
0xf899,	// MCMC10_Dark2   mcmc_delta6
0xf95b,	// MCMC11_Dark2   mcmc_center6

0xfa40,	// MCMC12_Dark2   mcmc_sat_gain1
0xfb40,	// MCMC13_Dark2   mcmc_sat_gain2
0xfc40,	// MCMC14_Dark2   mcmc_sat_gain3
0xfd40,	// MCMC15_Dark2   mcmc_sat_gain4
0x0e00, // burst end

0x03d7,// Page D7

0x0e01, // burst start

0x1040,	// MCMC16_Dark2   mcmc_sat_gain5
0x1140,	// MCMC17_Dark2   mcmc_sat_gain6
0x1291,	// MCMC18_Dark2   mcmc_hue_angle1
0x1300,	// MCMC19_Dark2   mcmc_hue_angle2
0x1400,	// MCMC20_Dark2   mcmc_hue_angle3
0x150a,	// MCMC21_Dark2   mcmc_hue_angle4
0x160f,	// MCMC22_Dark2   mcmc_hue_angle5
0x1786,	// MCMC23_Dark2   mcmc_hue_angle6

0x182f,	// MCMC24_Dark2   mcmc_rgb_ofs_sig
0x192f,	// MCMC25_Dark2   mcmc_rgb_ofs_sig
0x1a2f,	// MCMC26_Dark2   mcmc_rgb_ofs_sig

0x1b00,	// MCMC27_Dark2   mcmc_rgb_ofs_r1
0x1c00,	// MCMC28_Dark2   mcmc_rgb_ofs_r1
0x1d00,	// MCMC29_Dark2   mcmc_rgb_ofs_r1

0x1e00,	// MCMC30_Dark2   mcmc_rgb_ofs_r2
0x1f00,	// MCMC31_Dark2   mcmc_rgb_ofs_r2
0x2000,	// MCMC32_Dark2   mcmc_rgb_ofs_r2

0x2100,	// MCMC33_Dark2   mcmc_rgb_ofs_r3
0x2200,	// MCMC34_Dark2   mcmc_rgb_ofs_r3
0x2300,	// MCMC35_Dark2   mcmc_rgb_ofs_r3

0x2400,	// MCMC36_Dark2   mcmc_rgb_ofs_r4
0x2500,	// MCMC37_Dark2   mcmc_rgb_ofs_r4
0x2600,	// MCMC38_Dark2   mcmc_rgb_ofs_r4

0x2700,	// MCMC39_Dark2   mcmc_rgb_ofs_r5
0x2800,	// MCMC40_Dark2   mcmc_rgb_ofs_r5
0x2900,	// MCMC41_Dark2   mcmc_rgb_ofs_r5

0x2a00,	// MCMC42_Dark2  mcmc_rgb_ofs_r6 R
0x2b00,	// MCMC43_Dark2  mcmc_rgb_ofs_r6 G
0x2c00,	// MCMC44_Dark2  mcmc_rgb_ofs_r6 B

0x2d00,	// MCMC45_Dark2  mcmc_std_offset1
0x2e00,	// MCMC46_Dark2  mcmc_std_offset2
0x2fff,	// MCMC47_Dark2  mcmc_std_th_max
0x3000,	// MCMC48_Dark2  mcmc_std_th_min

0x313f,	// MCMC49_Dark2  mcmc_lum_gain_wgt R1
0x323f,	// MCMC50_Dark2  mcmc_lum_gain_wgt R1
0x333f,	// MCMC51_Dark2  mcmc_lum_gain_wgt R1
0x343f,	// MCMC52_Dark2  mcmc_lum_gain_wgt R1
0x3530,	// MCMC53_Dark2  mcmc_rg1_lum_sp1  R1
0x3650,	// MCMC54_Dark2  mcmc_rg1_lum_sp2  R1
0x3780,	// MCMC55_Dark2  mcmc_rg1_lum_sp3  R1
0x38b0,	// MCMC56_Dark2  mcmc_rg1_lum_sp4  R1

0x393f,	// MCMC57_Dark2  mcmc_lum_gain_wgt R2
0x3a3f,	// MCMC58_Dark2  mcmc_lum_gain_wgt R2
0x3b3f,	// MCMC59_Dark2  mcmc_lum_gain_wgt R2
0x3c3f,	// MCMC60_Dark2  mcmc_lum_gain_wgt R2
0x3d28,	// MCMC61_Dark2  mcmc_rg2_lum_sp1  R2
0x3e50,	// MCMC62_Dark2  mcmc_rg2_lum_sp2  R2
0x3f80,	// MCMC63_Dark2  mcmc_rg2_lum_sp3  R2
0x40b0,	// MCMC64_Dark2  mcmc_rg2_lum_sp4  R2

0x413f,	// MCMC65_Dark2  mcmc_lum_gain_wgt R3
0x423f,	// MCMC66_Dark2  mcmc_lum_gain_wgt R3
0x433f,	// MCMC67_Dark2  mcmc_lum_gain_wgt R3
0x443f,	// MCMC68_Dark2  mcmc_lum_gain_wgt R3
0x4528,	// MCMC69_Dark2  mcmc_rg3_lum_sp1  R3
0x4650,	// MCMC70_Dark2  mcmc_rg3_lum_sp2  R3
0x4780,	// MCMC71_Dark2  mcmc_rg3_lum_sp3  R3
0x48b0,	// MCMC72_Dark2  mcmc_rg3_lum_sp4  R3

0x491a,	// MCMC73_Dark2  mcmc_lum_gain_wgt R4
0x4a28,	// MCMC74_Dark2  mcmc_lum_gain_wgt R4
0x4b3f,	// MCMC75_Dark2  mcmc_lum_gain_wgt R4
0x4c3f,	// MCMC76_Dark2  mcmc_lum_gain_wgt R4
0x4d10,	// MCMC77_Dark2  mcmc_rg4_lum_sp1  R4
0x4e30,	// MCMC78_Dark2  mcmc_rg4_lum_sp2  R4
0x4f60,	// MCMC79_Dark2  mcmc_rg4_lum_sp3  R4
0x5090,	// MCMC80_Dark2  mcmc_rg4_lum_sp4  R4

0x511a,	// MCMC81_Dark2  mcmc_rg5_gain_wgt R5
0x5228,	// MCMC82_Dark2  mcmc_rg5_gain_wgt R5
0x533f,	// MCMC83_Dark2  mcmc_rg5_gain_wgt R5
0x543f,	// MCMC84_Dark2  mcmc_rg5_gain_wgt R5
0x5528,	// MCMC85_Dark2  mcmc_rg5_lum_sp1  R5
0x5650,	// MCMC86_Dark2  mcmc_rg5_lum_sp2  R5
0x5780,	// MCMC87_Dark2  mcmc_rg5_lum_sp3  R5
0x58b0,	// MCMC88_Dark2  mcmc_rg5_lum_sp4  R5

0x591a,	// MCMC89_Dark2  mcmc_rg6_gain_wgt R6
0x5a28,	// MCMC90_Dark2  mcmc_rg6_gain_wgt R6
0x5b3f,	// MCMC91_Dark2  mcmc_rg6_gain_wgt R6
0x5c3f,	// MCMC92_Dark2  mcmc_rg6_gain_wgt R6
0x5d28,	// MCMC93_Dark2  mcmc_rg6_lum_sp1  R6
0x5e50,	// MCMC94_Dark2  mcmc_rg6_lum_sp2  R6
0x5f80,	// MCMC95_Dark2  mcmc_rg6_lum_sp3  R6
0x60b0,	// MCMC96_Dark2  mcmc_rg6_lum_sp4  R6

0x611b,	// MCMC97_Dark2  mcmc2_allgain_x1
0x6239,	// MCMC98_Dark2  mcmc2_allgain_x2
0x6339,	// MCMC99_Dark2  mcmc2_allgain_x4
0x641b,	// MCMC100_Dark2 mcmc2_allgain_x5
0x651b,	// MCMC101_Dark2 mcmc2_allgain_x7
0x6639,	// MCMC102_Dark2 mcmc2_allgain_x8
0x6739,	// MCMC103_Dark2 mcmc2_allgain_x10
0x681b,	// MCMC104_Dark2 mcmc2_allgain_x11

0x690f,	// MCMC105_Dark2 mcmc2_allgain_y0
0x6a10,	// MCMC106_Dark2 mcmc2_allgain_y1
0x6b10,	// MCMC107_Dark2 mcmc2_allgain_y2
0x6c11,	// MCMC108_Dark2 mcmc2_allgain_y3
0x6d10,	// MCMC109_Dark2 mcmc2_allgain_y4
0x6e13,	// MCMC110_Dark2 mcmc2_allgain_y5
0x6f12,	// MCMC111_Dark2 mcmc2_allgain_y6
0x7012,	// MCMC112_Dark2 mcmc2_allgain_y7
0x7112,	// MCMC113_Dark2 mcmc2_allgain_y8
0x7211,	// MCMC114_Dark2 mcmc2_allgain_y9
0x7310,	// MCMC115_Dark2 mcmc2_allgain_y10
0x740f,	// MCMC116_Dark2 mcmc2_allgain_y11

// LowTemp MCMC
0x7510,	// MCMC00_LowTemp   mcmc_delta1
0x7639,	// MCMC01_LowTemp   mcmc_center1
0x7710,	// MCMC02_LowTemp   mcmc_delta2
0x7859,	// MCMC03_LowTemp   mcmc_center2
0x7912,	// MCMC04_LowTemp   mcmc_delta3
0x7a9d,	// MCMC05_LowTemp   mcmc_center3
0x7b12,	// MCMC06_LowTemp   mcmc_delta4
0x7cc1,	// MCMC07_LowTemp   mcmc_center4
0x7d18,	// MCMC08_LowTemp   mcmc_delta5
0x7eeb,	// MCMC09_LowTemp   mcmc_center5
0x7f99,	// MCMC10_LowTemp   mcmc_delta6
0x801c,	// MCMC11_LowTemp   mcmc_center6

0x8140,	// MCMC12_LowTemp   mcmc_sat_gain1
0x8240,	// MCMC13_LowTemp   mcmc_sat_gain2
0x8340,	// MCMC14_LowTemp   mcmc_sat_gain3
0x8440,	// MCMC15_LowTemp   mcmc_sat_gain4
0x8540,	// MCMC16_LowTemp   mcmc_sat_gain5
0x8640,	// MCMC17_LowTemp   mcmc_sat_gain6
0x8700,	// MCMC18_LowTemp   mcmc_hue_angle1
0x8800,	// MCMC19_LowTemp   mcmc_hue_angle2
0x8900,	// MCMC20_LowTemp   mcmc_hue_angle3
0x8a00,	// MCMC21_LowTemp   mcmc_hue_angle4
0x8b00,	// MCMC22_LowTemp   mcmc_hue_angle5
0x8c00,	// MCMC23_LowTemp   mcmc_hue_angle6

0x8d1f,	// MCMC24_LowTemp   mcmc_rgb_ofs_sig
0x8e1f,	// MCMC25_LowTemp   mcmc_rgb_ofs_sig
0x8f1f,	// MCMC26_LowTemp   mcmc_rgb_ofs_sig

0x9000,	// MCMC27_LowTemp   mcmc_rgb_ofs_r1
0x9100,	// MCMC28_LowTemp   mcmc_rgb_ofs_r1
0x9200,	// MCMC29_LowTemp   mcmc_rgb_ofs_r1

0x9300,	// MCMC30_LowTemp   mcmc_rgb_ofs_r2
0x9400,	// MCMC31_LowTemp   mcmc_rgb_ofs_r2
0x9500,	// MCMC32_LowTemp   mcmc_rgb_ofs_r2

0x9600,	// MCMC33_LowTemp   mcmc_rgb_ofs_r3
0x9700,	// MCMC34_LowTemp   mcmc_rgb_ofs_r3
0x9800,	// MCMC35_LowTemp   mcmc_rgb_ofs_r3

0x9900,	// MCMC36_LowTemp   mcmc_rgb_ofs_r4
0x9a00,	// MCMC37_LowTemp   mcmc_rgb_ofs_r4
0x9b00,	// MCMC38_LowTemp   mcmc_rgb_ofs_r4

0x9c00,	// MCMC39_LowTemp   mcmc_rgb_ofs_r5
0x9d00,	// MCMC40_LowTemp   mcmc_rgb_ofs_r5
0x9e00,	// MCMC41_LowTemp   mcmc_rgb_ofs_r5

0x9f00,	// MCMC42_LowTemp  mcmc_rgb_ofs_r6 R
0xa000,	// MCMC43_LowTemp  mcmc_rgb_ofs_r6 G
0xa100,	// MCMC44_LowTemp  mcmc_rgb_ofs_r6 B

0xa200,	// MCMC45_LowTemp  mcmc_std_offset1
0xa300,	// MCMC46_LowTemp  mcmc_std_offset2
0xa4ff,	// MCMC47_LowTemp  mcmc_std_th_max
0xa500,	// MCMC48_LowTemp  mcmc_std_th_min

0xa63f,	// MCMC49_LowTemp  mcmc_lum_gain_wgt R1
0xa73f,	// MCMC50_LowTemp  mcmc_lum_gain_wgt R1
0xa83f,	// MCMC51_LowTemp  mcmc_lum_gain_wgt R1
0xa93f,	// MCMC52_LowTemp  mcmc_lum_gain_wgt R1
0xaa30,	// MCMC53_LowTemp  mcmc_rg1_lum_sp1  R1
0xab50,	// MCMC54_LowTemp  mcmc_rg1_lum_sp2  R1
0xac80,	// MCMC55_LowTemp  mcmc_rg1_lum_sp3  R1
0xadb0,	// MCMC56_LowTemp  mcmc_rg1_lum_sp4  R1

0xae3f,	// MCMC57_LowTemp  mcmc_lum_gain_wgt R2
0xaf3f,	// MCMC58_LowTemp  mcmc_lum_gain_wgt R2
0xb03f,	// MCMC59_LowTemp  mcmc_lum_gain_wgt R2
0xb13f,	// MCMC60_LowTemp  mcmc_lum_gain_wgt R2
0xb228,	// MCMC61_LowTemp  mcmc_rg2_lum_sp1  R2
0xb350,	// MCMC62_LowTemp  mcmc_rg2_lum_sp2  R2
0xb480,	// MCMC63_LowTemp  mcmc_rg2_lum_sp3  R2
0xb5b0,	// MCMC64_LowTemp  mcmc_rg2_lum_sp4  R2

0xb63f,	// MCMC65_LowTemp  mcmc_lum_gain_wgt R3
0xb73f,	// MCMC66_LowTemp  mcmc_lum_gain_wgt R3
0xb83f,	// MCMC67_LowTemp  mcmc_lum_gain_wgt R3
0xb93f,	// MCMC68_LowTemp  mcmc_lum_gain_wgt R3
0xba28,	// MCMC69_LowTemp  mcmc_rg3_lum_sp1  R3
0xbb50,	// MCMC70_LowTemp  mcmc_rg3_lum_sp2  R3
0xbc80,	// MCMC71_LowTemp  mcmc_rg3_lum_sp3  R3
0xbdb0,	// MCMC72_LowTemp  mcmc_rg3_lum_sp4  R3

0xbe3f,	// MCMC73_LowTemp  mcmc_lum_gain_wgt R4
0xbf3f,	// MCMC74_LowTemp  mcmc_lum_gain_wgt R4
0xc03f,	// MCMC75_LowTemp  mcmc_lum_gain_wgt R4
0xc13f,	// MCMC76_LowTemp  mcmc_lum_gain_wgt R4
0xc210,	// MCMC77_LowTemp  mcmc_rg4_lum_sp1  R4
0xc330,	// MCMC78_LowTemp  mcmc_rg4_lum_sp2  R4
0xc460,	// MCMC79_LowTemp  mcmc_rg4_lum_sp3  R4
0xc590,	// MCMC80_LowTemp  mcmc_rg4_lum_sp4  R4

0xc63f,	// MCMC81_LowTemp  mcmc_rg5_gain_wgt R5
0xc73f,	// MCMC82_LowTemp  mcmc_rg5_gain_wgt R5
0xc83f,	// MCMC83_LowTemp  mcmc_rg5_gain_wgt R5
0xc93f,	// MCMC84_LowTemp  mcmc_rg5_gain_wgt R5
0xca28,	// MCMC85_LowTemp  mcmc_rg5_lum_sp1  R5
0xcb50,	// MCMC86_LowTemp  mcmc_rg5_lum_sp2  R5
0xcc80,	// MCMC87_LowTemp  mcmc_rg5_lum_sp3  R5
0xcdb0,	// MCMC88_LowTemp  mcmc_rg5_lum_sp4  R5

0xce3f,	// MCMC89_LowTemp  mcmc_rg6_gain_wgt R6
0xcf3f,	// MCMC90_LowTemp  mcmc_rg6_gain_wgt R6
0xd03f,	// MCMC91_LowTemp  mcmc_rg6_gain_wgt R6
0xd13f,	// MCMC92_LowTemp  mcmc_rg6_gain_wgt R6
0xd228,	// MCMC93_LowTemp  mcmc_rg6_lum_sp1  R6
0xd350,	// MCMC94_LowTemp  mcmc_rg6_lum_sp2  R6
0xd480,	// MCMC95_LowTemp  mcmc_rg6_lum_sp3  R6
0xd5b0,	// MCMC96_LowTemp  mcmc_rg6_lum_sp4  R6

0xd61a,	// MCMC97_LowTemp  mcmc2_allgain_x1
0xd738,	// MCMC98_LowTemp  mcmc2_allgain_x2
0xd838,	// MCMC99_LowTemp  mcmc2_allgain_x4
0xd91a,	// MCMC100_LowTemp mcmc2_allgain_x5
0xda1a,	// MCMC101_LowTemp mcmc2_allgain_x7
0xdb38,	// MCMC102_LowTemp mcmc2_allgain_x8
0xdc38,	// MCMC103_LowTemp mcmc2_allgain_x10
0xdd1a,	// MCMC104_LowTemp mcmc2_allgain_x11

0xde10,	// MCMC105_LowTemp mcmc2_allgain_y0
0xdf0f,	// MCMC106_LowTemp mcmc2_allgain_y1
0xe00e,	// MCMC107_LowTemp mcmc2_allgain_y2
0xe10e,	// MCMC108_LowTemp mcmc2_allgain_y3
0xe212,	// MCMC109_LowTemp mcmc2_allgain_y4
0xe316,	// MCMC110_LowTemp mcmc2_allgain_y5
0xe416,	// MCMC111_LowTemp mcmc2_allgain_y6
0xe514,	// MCMC112_LowTemp mcmc2_allgain_y
0xe612,	// MCMC113_LowTemp mcmc2_allgain_y8
0xe710,	// MCMC114_LowTemp mcmc2_allgain_y9
0xe810,	// MCMC115_LowTemp mcmc2_allgain_y10
0xe910,	// MCMC116_LowTemp mcmc2_allgain_y11
0x0e00, // burst end

// HighTemp MCMC
0x03d7, //Page d7
0xea10, //Hi-Temp_delta1
0xeb39, //Hi-Temp_center1
0xec10, //Hi-Temp_delta2
0xed59, //Hi-Temp_center2
0xee12, //Hi-Temp_delta3
0xef9d, //Hi-Temp_center3
0xf012, //Hi-Temp_delta4
0xf1bd, //Hi-Temp_center4
0xf21e, //Hi-Temp_delta5
0xf3f1, //Hi-Temp_center5
0xf49e, //Hi-Temp_delta6
0xf534, //Hi-Temp_center6
0xf640, //Hi-Temp_sat_gain1
0xf740, //Hi-Temp_sat_gain2
0xf840, //Hi-Temp_sat_gain3
0xf940, //Hi-Temp_sat_gain4
0xfa40, //Hi-Temp_sat_gain5
0xfb40, //Hi-Temp_sat_gain6
0xfc00, //Hi-Temp_hue_angle1
0xfd00, //Hi-Temp_hue_angle2

0x03d8, //Page d8
0x0e01, // burst start

0x1000, //Hi-Temp_hue_angle3
0x1100, //Hi-Temp_hue_angle4
0x1206, //Hi-Temp_hue_angle5
0x1300, //Hi-Temp_hue_angle6
0x1411, //Hi-Temp_rgb_ofs_sign_r
0x1511, //Hi-Temp_rgb_ofs_sign_g
0x1611, //Hi-Temp_rgb_ofs_sign_b
0x1700, //Hi-Temp_rgb_ofs_scl_r1
0x1800, //Hi-Temp_rgb_ofs_scl_g1
0x1900, //Hi-Temp_rgb_ofs_scl_b1
0x1a00, //Hi-Temp_rgb_ofs_scl_r2
0x1b00, //Hi-Temp_rgb_ofs_scl_g2
0x1c00, //Hi-Temp_rgb_ofs_scl_b2
0x1d00, //Hi-Temp_rgb_ofs_scl_r3
0x1e00, //Hi-Temp_rgb_ofs_scl_g3
0x1f00, //Hi-Temp_rgb_ofs_scl_b3
0x2000, //Hi-Temp_rgb_ofs_scl_r4
0x2100, //Hi-Temp_rgb_ofs_scl_g4
0x2200, //Hi-Temp_rgb_ofs_scl_b4
0x2300, //Hi-Temp_rgb_ofs_scl_r5
0x2400, //Hi-Temp_rgb_ofs_scl_g5
0x2500, //Hi-Temp_rgb_ofs_scl_b5
0x2600, //Hi-Temp_rgb_ofs_scl_r6
0x2700, //Hi-Temp_rgb_ofs_scl_g6
0x2800, //Hi-Temp_rgb_ofs_scl_b6
0x2900, //Hi-Temp_std_offset1
0x2a00, //Hi-Temp_std_offset2
0x2bff, //Hi-Temp_std_th_max
0x2c00, //Hi-Temp_std_th_min
0x2d3f, //Hi-Temp_rg1_lum_gain_wgt_th1
0x2e3f, //Hi-Temp_rg1_lum_gain_wgt_th2
0x2f3f, //Hi-Temp_rg1_lum_gain_wgt_th3
0x303f, //Hi-Temp_rg1_lum_gain_wgt_th4
0x3130, //Hi-Temp_rg1_lum_sp1
0x3250, //Hi-Temp_rg1_lum_sp2
0x3380, //Hi-Temp_rg1_lum_sp3
0x34b0, //Hi-Temp_rg1_lum_sp4
0x353f, //Hi-Temp_rg2_gain_wgt_th1
0x363f, //Hi-Temp_rg2_gain_wgt_th2
0x373f, //Hi-Temp_rg2_gain_wgt_th3
0x383f, //Hi-Temp_rg2_gain_wgt_th4
0x3928, //Hi-Temp_rg2_lum_sp1
0x3a50, //Hi-Temp_rg2_lum_sp2
0x3b80, //Hi-Temp_rg2_lum_sp3
0x3cb0, //Hi-Temp_rg2_lum_sp4
0x3d3f, //Hi-Temp_rg3_gain_wgt_th1
0x3e3f, //Hi-Temp_rg3_gain_wgt_th2
0x3f3f, //Hi-Temp_rg3_gain_wgt_th3
0x403f, //Hi-Temp_rg3_gain_wgt_th4
0x4128, //Hi-Temp_rg3_lum_sp1
0x4250, //Hi-Temp_rg3_lum_sp2
0x4380, //Hi-Temp_rg3_lum_sp3
0x44b0, //Hi-Temp_rg3_lum_sp4

0x453f, //Hi-Temp_rg4_gain_wgt_th1
0x463f, //Hi-Temp_rg4_gain_wgt_th2
0x473f, //Hi-Temp_rg4_gain_wgt_th3
0x483f, //Hi-Temp_rg4_gain_wgt_th4
0x4910, //Hi-Temp_rg4_lum_sp1
0x4a30, //Hi-Temp_rg4_lum_sp2
0x4b60, //Hi-Temp_rg4_lum_sp3
0x4c90, //Hi-Temp_rg4_lum_sp4

0x4d3f, //Hi-Temp_rg5_gain_wgt_th1
0x4e3f, //Hi-Temp_rg5_gain_wgt_th2
0x4f3f, //Hi-Temp_rg5_gain_wgt_th3
0x503f, //Hi-Temp_rg5_gain_wgt_th4
0x5128, //Hi-Temp_rg5_lum_sp1
0x5250, //Hi-Temp_rg5_lum_sp2
0x5380, //Hi-Temp_rg5_lum_sp3
0x54b0, //Hi-Temp_rg5_lum_sp4

0x553f, //Hi-Temp_rg6_gain_wgt_th1
0x563f, //Hi-Temp_rg6_gain_wgt_th2
0x573f, //Hi-Temp_rg6_gain_wgt_th3
0x583f, //Hi-Temp_rg6_gain_wgt_th4
0x5928, //Hi-Temp_rg6_lum_sp1
0x5a50, //Hi-Temp_rg6_lum_sp2
0x5b80, //Hi-Temp_rg6_lum_sp3
0x5cb0, //Hi-Temp_rg6_lum_sp4

0x5d19, //Hi-Temp_allgain_x1
0x5e37, //Hi-Temp_allgain_x2
0x5f37, //Hi-Temp_allgain_x3
0x6019, //Hi-Temp_allgain_x4
0x6119, //Hi-Temp_allgain_x5
0x6237, //Hi-Temp_allgain_x6
0x6337, //Hi-Temp_allgain_x7
0x6419, //Hi-Temp_allgain_x8

0x650e, //Hi-Temp_allgain_y0
0x660d, //Hi-Temp_allgain_y1
0x670e, //Hi-Temp_allgain_y2
0x680e, //Hi-Temp_allgain_y3
0x6910, //Hi-Temp_allgain_y4
0x6a10, //Hi-Temp_allgain_y5
0x6b13, //Hi-Temp_allgain_y6
0x6c12, //Hi-Temp_allgain_y7
0x6d13, //Hi-Temp_allgain_y8
0x6e12, //Hi-Temp_allgain_y9
0x6f0e, //Hi-Temp_allgain_y10
0x7011, //Hi-Temp_allgain_y11

0x0e00, // burst end

0x03D3,
0x11FE,	// function block on
0x108F,	// Adaptive on

0x03d8,
0xcc34,
0x03dd,
0xbf34,

///////////////////////////////////////////////////////////////////////////////
// DE ~ E0 Page (DMA Outdoor)
///////////////////////////////////////////////////////////////////////////////

0x03de, //DMA DE Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Outdoor 1111 add 720p 
0x1414,
0x1500, //Outdoor 1114 add 720p 
0x1615,
0x1781, //Outdoor 1115 add 720p 
0x1816,
0x1904, //Outdoor 1116 add 720p 
0x1a17,
0x1b58, //Outdoor 1117 add 720p 
0x1c18,
0x1d30, //Outdoor 1118 add 720p 
0x1e19,
0x1f12, //Outdoor 1119 add 720p 
0x2037,
0x2100, //Outdoor 1137
0x2238,
0x2300, //Outdoor 1138
0x2439,
0x2500, //Outdoor 1139
0x263a,
0x2700, //Outdoor 113a
0x283b,
0x2900, //Outdoor 113b
0x2a3c,
0x2b00, //Outdoor 113c
0x2c3d,
0x2d00, //Outdoor 113d
0x2e3e,
0x2f00, //Outdoor 113e
0x303f,
0x3100, //Outdoor 113f
0x3240,
0x3300, //Outdoor 1140
0x3441,
0x3500, //Outdoor 1141
0x3642,
0x3700, //Outdoor 1142
0x3843,
0x3900, //Outdoor 1143
0x3a49,
0x3b06, //Outdoor 1149 add 720p 
0x3c4a,
0x3d0a, //Outdoor 114a add 720p 
0x3e4b,
0x3f12, //Outdoor 114b add 720p 
0x404c,
0x411c, //Outdoor 114c add 720p 
0x424d,
0x4324, //Outdoor 114d add 720p 
0x444e,
0x4540, //Outdoor 114e add 720p 
0x464f,
0x4780, //Outdoor 114f add 720p 
0x4850,
0x491a, //Outdoor 1150
0x4a51,
0x4b23, //Outdoor 1151
0x4c52,
0x4d2c, //Outdoor 1152
0x4e53,
0x4f3f, //Outdoor 1153
0x5054,
0x513f, //Outdoor 1154
0x5255,
0x533e, //Outdoor 1155
0x5456,
0x553c, //Outdoor 1156
0x5657,
0x573a, //Outdoor 1157
0x5858,
0x593f, //Outdoor 1158
0x5a59,
0x5b3f, //Outdoor 1159
0x5c5a,
0x5d3e, //Outdoor 115a
0x5e5b,
0x5f3a, //Outdoor 115b
0x605c,
0x6137, //Outdoor 115c
0x625d,
0x6334, //Outdoor 115d
0x645e,
0x6532, //Outdoor 115e
0x665f,
0x6730, //Outdoor 115f
0x686e,
0x691c, //Outdoor 116e
0x6a6f,
0x6b18, //Outdoor 116f
0x6c77,
0x6d2b, //Outdoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f2a, //Outdoor 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Outdoor 1179 //Bayer SP Lum Pos3
0x727a,
0x731a, //Outdoor 117a //Bayer SP Lum Pos4
0x747b,
0x751c, //Outdoor 117b //Bayer SP Lum Pos5
0x767c,
0x771a, //Outdoor 117c //Bayer SP Lum Pos6
0x787d,
0x7919, //Outdoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b17, //Outdoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1e, //Outdoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1e, //Outdoor 1180 //Bayer SP Lum Neg2
0x8081,
0x811f, //Outdoor 1181 //Bayer SP Lum Neg3
0x8282,
0x831e, //Outdoor 1182 //Bayer SP Lum Neg4
0x8483,
0x851a, //Outdoor 1183 //Bayer SP Lum Neg5
0x8684,
0x871a, //Outdoor 1184 //Bayer SP Lum Neg6
0x8885,
0x891a, //Outdoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b1a, //Outdoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d1a, //Outdoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f16, //Outdoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9116, //Outdoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9315, //Outdoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9517, //Outdoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9717, //Outdoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9917, //Outdoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b16, //Outdoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Outdoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f20, //Outdoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa123, //Outdoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa321, //Outdoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa521, //Outdoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa720, //Outdoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91b, //Outdoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab18, //Outdoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad2b, //Outdoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf2b, //Outdoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb12b, //Outdoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb32b, //Outdoor 11aa //Bayer SP Edge4
0xb4ab,
0xb52b, //Outdoor 11ab //Bayer SP Edge5
0xb6ac,
0xb72c, //Outdoor 11ac //Bayer SP Edge6
0xb8ad,
0xb931, //Outdoor 11ad //Bayer SP Edge7
0xbaae,
0xbb35, //Outdoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Outdoor 11b7 add 720p 
0xbeb8,
0xbf22, //Outdoor 11b8 add 720p 
0xc0b9,
0xc121, //Outdoor 11b9 add 720p 
0xc2ba,
0xc31e, //Outdoor 11ba add 720p 
0xc4bb,
0xc51c, //Outdoor 11bb add 720p 
0xc6bc,
0xc71a, //Outdoor 11bc add 720p 
0xc8c7,
0xc930, //Outdoor 11c7 //Bayer SP STD1
0xcac8,
0xcb30, //Outdoor 11c8 //Bayer SP STD2
0xccc9,
0xcd30, //Outdoor 11c9 //Bayer SP STD3
0xceca,
0xcf30, //Outdoor 11ca //Bayer SP STD4
0xd0cb,
0xd130, //Outdoor 11cb //Bayer SP STD5
0xd2cc,
0xd330, //Outdoor 11cc //Bayer SP STD6
0xd4cd,
0xd52d, //Outdoor 11cd //Bayer SP STD7
0xd6ce,
0xd72a, //Outdoor 11ce //Bayer SP STD8
0xd8cf,
0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb15,//Outdoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Outdoor 11d1
0xded2,
0xdf40, //Outdoor 11d2
0xe0d3,
0xe1ff, //Outdoor 11d3
0xe2d4,
0xe301, //Outdoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe50a, //Outdoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Outdoor 11d6
0xe8d7,
0xe910,//Outdoor 11d7
0xead8,
0xeb01, //Outdoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed06, //Outdoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Outdoor 11da
0xf0db,
0xf107, //Outdoor 11db
0xf2df,
0xf355, //Outdoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf536, //Outdoor 11e0
0xf6e1,
0xf77a, //Outdoor 11e1
0xf8e2,
0xf935, //Outdoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfba0, //Outdoor 11e3
0xfce4,
0xfd01, //Outdoor 11e4
0x0e00, // burst end

0x03df, //DMA DF Page
0x0e01, // burst start

0x10e5,
0x1120,//Outdoor 11e5
0x12e6,
0x1301, //Outdoor 11e6
0x14e7,
0x151a,//Outdoor 11e7
0x16e8,
0x1701, //Outdoor 11e8
0x18e9,
0x1910, //Outdoor 11e9
0x1aea,
0x1b01, //Outdoor 11ea
0x1ceb,
0x1d12, //Outdoor 11eb
0x1eef,
0x1f33, //Outdoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Outdoor 1240 add 720p 
0x2470,
0x259f, //Outdoor 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Outdoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Outdoor 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Outdoor 1277
0x2c78,
0x2d2f, //Outdoor 1278
0x2e79,
0x2f09, //Outdoor 1279
0x307a,
0x3150, //Outdoor 127a
0x327b,
0x3310, //Outdoor 127b
0x347c,
0x3550, //Outdoor 127c //skin HPF gain
0x367d,
0x3710, //Outdoor 127d
0x387f,
0x3950, //Outdoor 127f
0x3a87,
0x3b3f, //Outdoor 1287 add 720p 
0x3c88,
0x3d3f, //Outdoor 1288 add 720p 
0x3e89,
0x3f3f, //Outdoor 1289 add 720p 
0x408a,
0x413f, //Outdoor 128a add 720p 
0x428b,
0x433f, //Outdoor 128b add 720p 
0x448c,
0x453f, //Outdoor 128c add 720p 
0x468d,
0x473f, //Outdoor 128d add 720p 
0x488e,
0x493f, //Outdoor 128e add 720p 
0x4a8f,
0x4b3f, //Outdoor 128f add 720p 
0x4c90,
0x4d3f, //Outdoor 1290 add 720p 
0x4e91,
0x4f3f, //Outdoor 1291 add 720p 
0x5092,
0x513f, //Outdoor 1292 add 720p 
0x5293,
0x533f, //Outdoor 1293 add 720p 
0x5494,
0x553f, //Outdoor 1294 add 720p 
0x5695,
0x573f, //Outdoor 1295 add 720p 
0x5896,
0x593f, //Outdoor 1296 add 720p 
0x5aae,
0x5b7f, //Outdoor 12ae
0x5caf,
0x5d63,//Outdoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Outdoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Outdoor 12c3 add 720p 
0x62c4,
0x631a, //Outdoor 12c4 add 720p 
0x64c5,
0x650c, //Outdoor 12c5 add 720p 
0x66c6,
0x6791, //Outdoor 12c6
0x68c7,
0x69a4, //Outdoor 12c7
0x6ac8,
0x6b3c, //Outdoor 12c8
0x6cd0,
0x6d08, //Outdoor 12d0 add 720p 
0x6ed1,
0x6f10, //Outdoor 12d1 add 720p 
0x70d2,
0x7118, //Outdoor 12d2 add 720p 
0x72d3,
0x7320, //Outdoor 12d3 add 720p 
0x74d4,
0x7530, //Outdoor 12d4 add 720p 
0x76d5,
0x7760, //Outdoor 12d5 add 720p 
0x78d6,
0x7980, //Outdoor 12d6 add 720p 
0x7ad7,
0x7b30,//Outdoor 12d7
0x7cd8,
0x7d33,//Outdoor 12d8
0x7ed9,
0x7f35,//Outdoor 12d9
0x80da,
0x8135,//Outdoor 12da
0x82db,
0x8334,//Outdoor 12db
0x84dc,
0x8530,//Outdoor 12dc
0x86dd,
0x872a,//Outdoor 12dd
0x88de,
0x8926,//Outdoor 12de
0x8ae0,
0x8b49, //Outdoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Outdoor 12e1
0x8ee2,
0x8f02, //Outdoor 12e2
0x90e3,
0x9120, //Outdoor 12e3 //PS LN graph Y1
0x92e4,
0x9320, //Outdoor 12e4 //PS LN graph Y2
0x94e5,
0x9520, //Outdoor 12e5 //PS LN graph Y3
0x96e6,
0x9720, //Outdoor 12e6 //PS LN graph Y4
0x98e7,
0x9920, //Outdoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b20, //Outdoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d20, //Outdoor 12e9 //PS DY graph Y1
0x9eea,
0x9f20, //Outdoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Outdoor 12eb //PS DY graph Y3
0xa2ec,
0xa31e, //Outdoor 12ec //PS DY graph Y4
0xa4ed,
0xa51d, //Outdoor 12ed //PS DY graph Y5
0xa6ee,
0xa720, //Outdoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Outdoor 12f0
0xaaf1,
0xab2a, //Outdoor 12f1
0xacf2,
0xad32, //Outdoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Outdoor 1310 //Y-NR ENB add  720p 
0xb230,
0xb33f, //Outdoor 1330
0xb431,
0xb53f, //Outdoor 1331
0xb632,
0xb73f, //Outdoor 1332
0xb833,
0xb93f, //Outdoor 1333
0xba34,
0xbb3f, //Outdoor 1334
0xbc35,
0xbd33, //Outdoor 1335
0xbe36,
0xbf2f, //Outdoor 1336
0xc037,
0xc12e, //Outdoor 1337
0xc238,
0xc302, //Outdoor 1338
0xc440,
0xc51e, //Outdoor 1340
0xc641,
0xc722, //Outdoor 1341
0xc842,
0xc962, //Outdoor 1342
0xca43,
0xcb63, //Outdoor 1343
0xcc44,
0xcdff, //Outdoor 1344
0xce45,
0xcf04, //Outdoor 1345
0xd046,
0xd136, //Outdoor 1346
0xd247,
0xd305, //Outdoor 1347
0xd448,
0xd520, //Outdoor 1348
0xd649,
0xd702, //Outdoor 1349
0xd84a,
0xd922, //Outdoor 134a
0xda4b,
0xdb06, //Outdoor 134b
0xdc4c,
0xdd20, //Outdoor 134c
0xde83,
0xdf08, //Outdoor 1383
0xe084,
0xe108, //Outdoor 1384
0xe2b7,
0xe3fd, //Outdoor 13b7
0xe4b8,
0xe5a7, //Outdoor 13b8
0xe6b9,
0xe7fe, //Outdoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Outdoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Outdoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Outdoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Outdoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Outdoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Outdoor 1410
0xf611,
0xf7d8, //Outdoor 1411
0xf812,
0xf910, //Outdoor 1412
0xfa13,
0xfb03, //Outdoor 1413
0xfc14,
0xfd0f, //Outdoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x1015,
0x117b, //Outdoor 1415 // Y Hi filter mask 1/16
0x1216,
0x131c, //Outdoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Outdoor 1417
0x1618,
0x170c, //Outdoor 1418
0x1819,
0x190c, //Outdoor 1419
0x1a1a,
0x1b18, //Outdoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1d, //Outdoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f26, //Outdoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2124, //Outdoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2323, //Outdoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x251a, //Outdoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Outdoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Outdoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b16, //Outdoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d16, //Outdoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f22, //Outdoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3121, //Outdoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3320, //Outdoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x351a, //Outdoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3719, //Outdoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3915, //Outdoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b18, //Outdoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d1b, //Outdoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f24, //Outdoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Outdoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4324, //Outdoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x451a,//Outdoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714,//Outdoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4910,//Outdoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b18,//Outdoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d18,//Outdoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f18, //Outdoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Outdoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x531b, //Outdoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5519, //Outdoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5719, //Outdoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x591e, //Outdoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b25, //Outdoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d25, //Outdoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f24, //Outdoor 1467 //YC2D SP Edge Gain1
0x6068,
0x6124, //Outdoor 1468 //YC2D SP Edge Gain2
0x6269,
0x6328, //Outdoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652e, //Outdoor 146a //YC2D SP Edge Gain4
0x666b,
0x6730, //Outdoor 146b //YC2D SP Edge Gain5
0x686c,
0x6931, //Outdoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b30, //Outdoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d28, //Outdoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f27, //Outdoor 1487 //YC2D SP STD Gain1
0x7088,
0x7128, //Outdoor 1488 //YC2D SP STD Gain2
0x7289,
0x732d, //Outdoor 1489 //YC2D SP STD Gain3
0x748a,
0x752d, //Outdoor 148a //YC2D SP STD Gain4
0x768b,
0x772e, //Outdoor 148b //YC2D SP STD Gain5
0x788c,
0x792b, //Outdoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b28, //Outdoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d25, //Outdoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Outdoor 1497 add 720p 
0x8098,
0x813f, //Outdoor 1498 add 720p 
0x8299,
0x833f, //Outdoor 1499 add 720p 
0x849a,
0x853f, //Outdoor 149a add 720p 
0x869b,
0x873f, //Outdoor 149b add 720p 
0x88a0,
0x893f, //Outdoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Outdoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Outdoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Outdoor 14a3 add 720p 
0x90a4,
0x913f, //Outdoor 14a4 add 720p 
0x92c9,
0x9313, //Outdoor 14c9
0x94ca,
0x9520, //Outdoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Outdoor 1A10 add 720p 
0x9a18,
0x9b3f, //Outdoor 1A18 
0x9c19,
0x9d3f, //Outdoor 1A19
0x9e1a,
0x9f3f, //Outdoor 1A1a
0xa01b,
0xa13f, //Outdoor 1A1b
0xa21c,
0xa33f, //Outdoor 1A1c
0xa41d,
0xa53c, //Outdoor 1A1d
0xa61e,
0xa738, //Outdoor 1A1e
0xa81f,
0xa935, //Outdoor 1A1f
0xaa20,
0xabe7, //Outdoor 1A20 add
0xac2f,
0xadf1, //Outdoor 1A2f add
0xae32,
0xaf87, //Outdoor 1A32 add
0xb034,
0xb1d2, //Outdoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31c, //Outdoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb506,//Outdoor 1A36
0xb637,
0xb740, //Outdoor 1A37
0xb838,
0xb9ff, //Outdoor 1A38
0xba39,
0xbb2e, //Outdoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Outdoor 1A3a
0xbe3b,
0xbf01, //Outdoor 1A3b
0xc03c,
0xc10c, //Outdoor 1A3c
0xc23d,
0xc301, //Outdoor 1A3d
0xc43e,
0xc507, //Outdoor 1A3e
0xc63f,
0xc701, //Outdoor 1A3f
0xc840,
0xc90c, //Outdoor 1A40
0xca41,
0xcb01, //Outdoor 1A41
0xcc42,
0xcd07, //Outdoor 1A42
0xce43,
0xcf2b, //Outdoor 1A43
0xd04d,
0xd10e, //Outdoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd30e, //Outdoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd50e, //Outdoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd713, //Outdoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd917, //Outdoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb17, //Outdoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd17, //Outdoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf17, //Outdoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe114, //Outdoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe311, //Outdoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe510, //Outdoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Outdoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Outdoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Outdoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Outdoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Outdoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf10f, //Outdoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf30f, //Outdoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf510, //Outdoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf717, //Outdoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf919, //Outdoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb17, //Outdoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd16, //Outdoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E0 page.
0xD027,
0xD142,

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x106c,
0x1116, //Outdoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1310, //Outdoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1516, //Outdoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1715, //Outdoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Outdoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Outdoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d13, //Outdoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f13, //Outdoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2113, //Outdoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2329, //Outdoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2529, //Outdoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2729, //Outdoor 1A7f //RGB SP Edge Gain3
0x2880,
0x292f, //Outdoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b2f, //Outdoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d2f, //Outdoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f27, //Outdoor 1A83 //RGB SP Edge Gain7
0x3084,
0x3125, //Outdoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3328, //Outdoor 1A9e //RGB SP STD Gain1
0x349f,
0x3528, //Outdoor 1A9f //RGB SP STD Gain2
0x36a0,
0x372f, //Outdoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x392e, //Outdoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b2d, //Outdoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d2d, //Outdoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f29, //Outdoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4125, //Outdoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4323,//Outdoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Outdoor 1Aa7 add
0x46a8,
0x473f, //Outdoor 1Aa8 add
0x48a9,
0x493f, //Outdoor 1Aa9 add
0x4aaa,
0x4b3f, //Outdoor 1Aaa add
0x4cab,
0x4d3f, //Outdoor 1Aab add
0x4eaf,
0x4f3f, //Outdoor 1Aaf add
0x50b0,
0x513f, //Outdoor 1Ab0 add
0x52b1,
0x533f, //Outdoor 1Ab1 add
0x54b2,
0x553f, //Outdoor 1Ab2 add
0x56b3,
0x573f, //Outdoor 1Ab3 add
0x58ca,
0x5900, //Outdoor 1Aca
0x5ae3,
0x5b13, //Outdoor 1Ae3 add
0x5ce4,
0x5d13, //Outdoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610f, //Outdoor 1070 Trans Func.   130108 Outdoor transFuc Flat graph
0x6271,
0x6300, //Outdoor 1071
0x6472,
0x6500, //Outdoor 1072
0x6673,
0x6700, //Outdoor 1073
0x6874,
0x6900, //Outdoor 1074
0x6a75,
0x6b00, //Outdoor 1075
0x6c76,
0x6d40, //Outdoor 1076
0x6e77,
0x6f40, //Outdoor 1077
0x7078,
0x7100, //Outdoor 1078
0x7279,
0x7340, //Outdoor 1079
0x747a,
0x7500, //Outdoor 107a
0x767b,
0x7740, //Outdoor 107b
0x787c,
0x7900, //Outdoor 107c
0x7a7d,
0x7b07, //Outdoor 107d
0x7c7e,
0x7d0f, //Outdoor 107e
0x7e7f,
0x7f1e, //Outdoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8330, //Outdoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Outdoor 031a (for sun-spot)
0x881b,
0x898c, //Outdoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Outdoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Outdoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9102, //Outdoor 11f0 (for af bug)
0x9203,              
0x9312, // 12 page    
0x9412,              
0x9530, //Outdoor 1212

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E1 ~ E3 Page (DMA Indoor)
///////////////////////////////////////////////////////////////////////////////

0x03e1, //DMA E1 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Indoor 1111 add 720p 
0x1414,
0x1500, //Indoor 1114 add 720p 
0x1615,
0x1781, //Indoor 1115 add 720p 
0x1816,
0x1904, //Indoor 1116 add 720p 
0x1a17,
0x1b58, //Indoor 1117 add 720p 
0x1c18,
0x1d30, //Indoor 1118 add 720p 
0x1e19,
0x1f12, //Indoor 1119 add 720p 
0x2037,
0x2107, //Indoor 1137 
0x2238,
0x2300, //Indoor 1138 //Pre flat R1 LumL
0x2439,
0x25ff, //Indoor 1139 //Pre flat R1 LumH
0x263a,
0x2700, //Indoor 113a
0x283b,
0x2900, //Indoor 113b
0x2a3c,
0x2b00, //Indoor 113c
0x2c3d,
0x2d56, //Indoor 113d
0x2e3e,
0x2f00, //Indoor 113e
0x303f,
0x3100, //Indoor 113f
0x3240,
0x3300, //Indoor 1140
0x3441,
0x352a, //Indoor 1141
0x3642,
0x3700, //Indoor 1142
0x3843,
0x3900, //Indoor 1143
0x3a49,
0x3b06, //Indoor 1149 add 720p 
0x3c4a,
0x3d0a, //Indoor 114a add 720p
0x3e4b,
0x3f12, //Indoor 114b add 720p
0x404c,
0x411c, //Indoor 114c add 720p
0x424d,
0x4324, //Indoor 114d add 720p
0x444e,
0x4540, //Indoor 114e add 720p
0x464f,
0x4780, //Indoor 114f add 720p
0x4850,
0x493f, //Indoor 1150 
0x4a51,
0x4b3f, //Indoor 1151
0x4c52,
0x4d3f, //Indoor 1152
0x4e53,
0x4f3d, //Indoor 1153
0x5054,
0x513c, //Indoor 1154
0x5255,
0x5338, //Indoor 1155
0x5456,
0x5536, //Indoor 1156
0x5657,
0x5734, //Indoor 1157
0x5858,
0x593f, //Indoor 1158
0x5a59,
0x5b3f, //Indoor 1159
0x5c5a,
0x5d3e, //Indoor 115a
0x5e5b,
0x5f38, //Indoor 115b
0x605c,
0x6133, //Indoor 115c
0x625d,
0x6331, //Indoor 115d
0x645e,
0x6530, //Indoor 115e
0x665f,
0x6730, //Indoor 115f
0x686e,
0x6920, //Indoor 116e
0x6a6f,
0x6b18, //Indoor 116f
0x6c77,
0x6d16, //Indoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f16, //Indoor 1178 //Bayer SP Lum Pos2
0x7079,
0x7115, //Indoor 1179 //Bayer SP Lum Pos3
0x727a,
0x7315, //Indoor 117a //Bayer SP Lum Pos4
0x747b,
0x7511, //Indoor 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Indoor 117c //Bayer SP Lum Pos6
0x787d,
0x7910, //Indoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b10, //Indoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d11, //Indoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f11, //Indoor 1180 //Bayer SP Lum Neg2
0x8081,
0x8111, //Indoor 1181 //Bayer SP Lum Neg3
0x8282,
0x8315, //Indoor 1182 //Bayer SP Lum Neg4
0x8483,
0x8516, //Indoor 1183 //Bayer SP Lum Neg5
0x8684,
0x8716, //Indoor 1184 //Bayer SP Lum Neg6
0x8885,
0x8916, //Indoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b16, //Indoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d15, //Indoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f15, //Indoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9113, //Indoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9313, //Indoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9513, //Indoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9713, //Indoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9913, //Indoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Indoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Indoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f16, //Indoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa117, //Indoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa317, //Indoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa51a, //Indoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa71a, //Indoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91a, //Indoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab19, //Indoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad26, //Indoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf26, //Indoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb125, //Indoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb325, //Indoor 11aa //Bayer SP Edge4
0xb4ab,
0xb525, //Indoor 11ab //Bayer SP Edge5
0xb6ac,
0xb725, //Indoor 11ac //Bayer SP Edge6
0xb8ad,
0xb925, //Indoor 11ad //Bayer SP Edge7
0xbaae,
0xbb24, //Indoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Indoor 11b7 add 720p 
0xbeb8,
0xbf22, //Indoor 11b8 add 720p 
0xc0b9,
0xc121, //Indoor 11b9 add 720p 
0xc2ba,
0xc31e, //Indoor 11ba add 720p 
0xc4bb,
0xc51c, //Indoor 11bb add 720p 
0xc6bc,
0xc71a, //Indoor 11bc add 720p 
0xc8c7,
0xc920, //Indoor 11c7 //Bayer SP STD1
0xcac8,
0xcb21, //Indoor 11c8 //Bayer SP STD2
0xccc9,
0xcd22, //Indoor 11c9 //Bayer SP STD3
0xceca,
0xcf24, //Indoor 11ca //Bayer SP STD4
0xd0cb,
0xd124, //Indoor 11cb //Bayer SP STD5
0xd2cc,
0xd324, //Indoor 11cc //Bayer SP STD6
0xd4cd,
0xd520, //Indoor 11cd //Bayer SP STD7
0xd6ce,
0xd71f, //Indoor 11ce //Bayer SP STD8
0xd8cf,
0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb18, //Indoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Indoor 11d1
0xded2,
0xdf40, //Indoor 11d2
0xe0d3,
0xe1ff, //Indoor 11d3
0xe2d4,
0xe302, //Indoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe51c, //Indoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Indoor 11d6
0xe8d7,
0xe910, //Indoor 11d7
0xead8,
0xeb01, //Indoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed0e, //Indoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Indoor 11da
0xf0db,
0xf107, //Indoor 11db
0xf2df,
0xf3cc, //Indoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Indoor 11e0
0xf6e1,
0xf77a, //Indoor 11e1
0xf8e2,
0xf907,//Indoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb18,//Indoor 11e3
0xfce4,
0xfd01, //Indoor 11e4 //Bayer Flat R3 StdL
0x0e00, // burst end

0x03e2, //DMA E2 Page
0x0e01, // burst start

0x10e5,
0x1122,//Indoor 11e5 //Bayer Flat R3 StdH
0x12e6,
0x1300, //Indoor 11e6
0x14e7,
0x150f,//Indoor 11e7
0x16e8,
0x1701, //Indoor 11e8
0x18e9,
0x191d, //Indoor 11e9
0x1aea,
0x1b00, //Indoor 11ea
0x1ceb,
0x1d0a,//Indoor 11eb
0x1eef,
0x1fa0,//Indoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Indoor 1240 add 720p 
0x2470,
0x259f, //Indoor 1270 // Bayer Sharpness ENB add
0x2671,
0x271a, //Indoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Indoor 1272 //Bayer LPF Gain
0x2a77,
0x2b26, //Indoor 1277 //20130412
0x2c78,
0x2d2f, //Indoor 1278
0x2e79,
0x2fff, //Indoor 1279
0x307a,
0x3150, //Indoor 127a
0x327b,
0x3310, //Indoor 127b
0x347c,
0x3564, //Indoor 127c //skin HPF gain
0x367d,
0x3720, //Indoor 127d
0x387f,
0x3950, //Indoor 127f
0x3a87,
0x3b3f, //Indoor 1287 add 720p 
0x3c88,
0x3d3f, //Indoor 1288 add 720p 
0x3e89,
0x3f3f, //Indoor 1289 add 720p 
0x408a,
0x413f, //Indoor 128a add 720p 
0x428b,
0x433f, //Indoor 128b add 720p 
0x448c,
0x453f, //Indoor 128c add 720p 
0x468d,
0x473f, //Indoor 128d add 720p 
0x488e,
0x493f, //Indoor 128e add 720p 
0x4a8f,
0x4b3f, //Indoor 128f add 720p 
0x4c90,
0x4d3f, //Indoor 1290 add 720p 
0x4e91,
0x4f3f, //Indoor 1291 add 720p 
0x5092,
0x513f, //Indoor 1292 add 720p 
0x5293,
0x533f, //Indoor 1293 add 720p 
0x5494,
0x553f, //Indoor 1294 add 720p 
0x5695,
0x573f, //Indoor 1295 add 720p 
0x5896,
0x593f, //Indoor 1296 add 720p 
0x5aae,
0x5b7f, //Indoor 12ae
0x5caf,
0x5d80, //Indoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Indoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Indoor 12c3 add 720p 
0x62c4,
0x631a, //Indoor 12c4 add 720p 
0x64c5,
0x650c, //Indoor 12c5 add 720p 
0x66c6,
0x6791, //Indoor 12c6
0x68c7,
0x69a4, //Indoor 12c7
0x6ac8,
0x6b3c, //Indoor 12c8
0x6cd0,
0x6d08, //Indoor 12d0 add 720p 
0x6ed1,
0x6f10, //Indoor 12d1 add 720p 
0x70d2,
0x7118, //Indoor 12d2 add 720p 
0x72d3,
0x7320, //Indoor 12d3 add 720p 
0x74d4,
0x7530, //Indoor 12d4 add 720p 
0x76d5,
0x7760, //Indoor 12d5 add 720p 
0x78d6,
0x7980, //Indoor 12d6 add 720p 
0x7ad7,
0x7b38, //Indoor 12d7
0x7cd8,
0x7d30, //Indoor 12d8
0x7ed9,
0x7f2a, //Indoor 12d9
0x80da,
0x812a, //Indoor 12da
0x82db,
0x8324, //Indoor 12db
0x84dc,
0x8520, //Indoor 12dc
0x86dd,
0x871a, //Indoor 12dd
0x88de,
0x8916, //Indoor 12de
0x8ae0,
0x8b63, //Indoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Indoor 12e1
0x8ee2,
0x8f02, //Indoor 12e2
0x90e3,
0x9110, //Indoor 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Indoor 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Indoor 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Indoor 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Indoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Indoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Indoor 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Indoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Indoor 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Indoor 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Indoor 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Indoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Indoor 12f0
0xaaf1,
0xab2a, //Indoor 12f1
0xacf2,
0xad32, //Indoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Indoor 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Indoor 1330
0xb431,
0xb520, //Indoor 1331
0xb632,
0xb720, //Indoor 1332
0xb833,
0xb920, //Indoor 1333
0xba34,
0xbb20, //Indoor 1334
0xbc35,
0xbd20, //Indoor 1335
0xbe36,
0xbf20, //Indoor 1336
0xc037,
0xc120, //Indoor 1337
0xc238,
0xc302, //Indoor 1338
0xc440,
0xc518, //Indoor 1340
0xc641,
0xc736, //Indoor 1341
0xc842,
0xc962, //Indoor 1342
0xca43,
0xcb63, //Indoor 1343
0xcc44,
0xcdff, //Indoor 1344
0xce45,
0xcf04, //Indoor 1345
0xd046,
0xd145, //Indoor 1346
0xd247,
0xd305, //Indoor 1347
0xd448,
0xd565, //Indoor 1348
0xd649,
0xd702, //Indoor 1349
0xd84a,
0xd922, //Indoor 134a
0xda4b,
0xdb06, //Indoor 134b
0xdc4c,
0xdd30, //Indoor 134c
0xde83,
0xdf08, //Indoor 1383 //add 20121210
0xe084,
0xe10a, //Indoor 1384 //add 20121210
0xe2b7,
0xe3fa, //Indoor 13b7
0xe4b8,
0xe577, //Indoor 13b8
0xe6b9,
0xe7fe, //Indoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Indoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Indoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Indoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Indoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Indoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Indoor 1410
0xf611,
0xf798, //Indoor 1411
0xf812,
0xf910, //Indoor 1412
0xfa13,
0xfb03, //Indoor 1413
0xfc14,
0xfd10, //Indoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x1015,
0x117b, //Indoor 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Indoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Indoor 1417
0x1618,
0x170c, //Indoor 1418
0x1819,
0x190c, //Indoor 1419
0x1a1a,
0x1b18, //Indoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Indoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f22, //Indoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2121, //Indoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x231c, //Indoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2515, //Indoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2711, //Indoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2910, //Indoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b11, //Indoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Indoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f1a, //Indoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3115, //Indoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3314, //Indoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3513, //Indoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Indoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Indoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b14, //Indoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d15, //Indoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f22, //Indoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Indoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4321, //Indoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4520, //Indoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x471d, //Indoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x491d, //Indoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b1d, //Indoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d1d, //Indoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f19, //Indoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Indoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5316, //Indoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5515, //Indoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5714, //Indoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Indoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b13, //Indoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Indoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f2a, //Indoor 1467 //YC2D SP Edge Gain1
0x6068,
0x612b, //Indoor 1468 //YC2D SP Edge Gain2
0x6269,
0x632c, //Indoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652d, //Indoor 146a //YC2D SP Edge Gain4
0x666b,
0x672d, //Indoor 146b //YC2D SP Edge Gain5
0x686c,
0x692b, //Indoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b2d, //Indoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d2e, //Indoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f14, //Indoor 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Indoor 1488 //YC2D SP STD Gain2
0x7289,
0x7326, //Indoor 1489 //YC2D SP STD Gain3
0x748a,
0x7526, //Indoor 148a //YC2D SP STD Gain4
0x768b,
0x7720, //Indoor 148b //YC2D SP STD Gain5
0x788c,
0x7920, //Indoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Indoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d15, //Indoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Indoor 1497 add 720p 
0x8098,
0x813f, //Indoor 1498 add 720p 
0x8299,
0x833f, //Indoor 1499 add 720p 
0x849a,
0x853f, //Indoor 149a add 720p 
0x869b,
0x873f, //Indoor 149b add 720p 
0x88a0,
0x893f, //Indoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Indoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Indoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Indoor 14a3 add 720p 
0x90a4,
0x913f, //Indoor 14a4 add 720p 
0x92c9,
0x9313, //Indoor 14c9
0x94ca,
0x9527, //Indoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Indoor 1A10 add 720p
0x9a18,
0x9b3f, //Indoor 1A18
0x9c19,
0x9d3f, //Indoor 1A19
0x9e1a,
0x9f2a, //Indoor 1A1a
0xa01b,
0xa127, //Indoor 1A1b
0xa21c,
0xa323, //Indoor 1A1c
0xa41d,
0xa523, //Indoor 1A1d
0xa61e,
0xa723, //Indoor 1A1e
0xa81f,
0xa923, //Indoor 1A1f
0xaa20,
0xabe7, //Indoor 1A20 add 720p 
0xac2f,
0xadf1, //Indoor 1A2f add 720p 
0xae32,
0xaf87, //Indoor 1A32 add 720p 
0xb034,
0xb1d0, //Indoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Indoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Indoor 1A36
0xb637,
0xb740, //Indoor 1A37
0xb838,
0xb9ff, //Indoor 1A38
0xba39,
0xbb1d, //Indoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Indoor 1A3a //RGB Flat R2_Lum H
0xbe3b,
0xbf00, //Indoor 1A3b
0xc03c,
0xc14c, //Indoor 1A3c
0xc23d,
0xc300, //Indoor 1A3d
0xc43e,
0xc513, //Indoor 1A3e
0xc63f,
0xc700, //Indoor 1A3f
0xc840,
0xc92a, //Indoor 1A40
0xca41,
0xcb00, //Indoor 1A41
0xcc42,
0xcd17, //Indoor 1A42
0xce43,
0xcf2c, //Indoor 1A43
0xd04d,
0xd112, //Indoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Indoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd511, //Indoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd710, //Indoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd910, //Indoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Indoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd10, //Indoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf10, //Indoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe113, //Indoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe313, //Indoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Indoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe710, //Indoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe910, //Indoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Indoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed10, //Indoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef10, //Indoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Indoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Indoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf513, //Indoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf714, //Indoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf914, //Indoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb14, //Indoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd15, //Indoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E3 page.
0xD62A,
0xD7FA,

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x106c,
0x1115, //Indoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Indoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Indoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Indoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1913, //Indoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Indoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d14, //Indoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Indoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2114, //Indoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2322, //Indoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2523, //Indoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2724, //Indoor 1A7f //RGB SP Edge Gain3
0x2880,
0x2925, //Indoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b26, //Indoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d27, //Indoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f29, //Indoor 1A83 //RGB SP Edge Gain7
0x3084,
0x312b, //Indoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Indoor 1A9e //RGB SP STD Gain1
0x349f,
0x3523, //Indoor 1A9f //RGB SP STD Gain2
0x36a0,
0x3726, //Indoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x3926, //Indoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b26, //Indoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d26, //Indoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f26, //Indoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4126, //Indoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4334, //Indoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Indoor 1Aa7 add 720p 
0x46a8,
0x473f, //Indoor 1Aa8 add 720p 
0x48a9,
0x493f, //Indoor 1Aa9 add 720p 
0x4aaa,
0x4b3f, //Indoor 1Aaa add 720p 
0x4cab,
0x4d3f, //Indoor 1Aab add 720p 
0x4eaf,
0x4f3f, //Indoor 1Aaf add 720p 
0x50b0,
0x513f, //Indoor 1Ab0 add 720p 
0x52b1,
0x533f, //Indoor 1Ab1 add 720p 
0x54b2,
0x553f, //Indoor 1Ab2 add 720p 
0x56b3,
0x573f, //Indoor 1Ab3 add 720p 
0x58ca,
0x5900, //Indoor 1Aca
0x5ae3,
0x5b13, //Indoor 1Ae3 add
0x5ce4,
0x5d13, //Indoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Indoor 1070 Trans Func.   130108 Indoor transFuc Flat graph
0x6271,
0x6301, //Indoor 1071
0x6472,
0x6589, //Indoor 1072
0x6673,
0x67d4, //Indoor 1073
0x6874,
0x6900, //Indoor 1074
0x6a75,
0x6b00, //Indoor 1075
0x6c76,
0x6d40, //Indoor 1076
0x6e77,
0x6f47, //Indoor 1077
0x7078,
0x71ae, //Indoor 1078
0x7279,
0x7350, //Indoor 1079
0x747a,
0x7500, //Indoor 107a
0x767b,
0x7750, //Indoor 107b
0x787c,
0x7900, //Indoor 107c
0x7a7d,
0x7b05, //Indoor 107d
0x7c7e,
0x7d0f, //Indoor 107e
0x7e7f,
0x7f2f, //Indoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x832a, //Indoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Indoor 031a (for sun-spot)
0x881b,
0x898c, //Indoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Indoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Indoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9103, //Indoor 11f0 (for af bug)
0x9203,             
0x9312, // 12 page   
0x9412,             
0x9508, //Indoor 1212

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E4 ~ E6 Page (DMA Dark1)
///////////////////////////////////////////////////////////////////////////////

0x03e4, //DMA E4 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Dark1 1111 add 720p 
0x1414,
0x1500, //Dark1 1114 add 720p 
0x1615,
0x1781, //Dark1 1115 add 720p 
0x1816,
0x1904, //Dark1 1116 add 720p 
0x1a17,
0x1b58, //Dark1 1117 add 720p 
0x1c18,
0x1d30, //Dark1 1118 add 720p 
0x1e19,
0x1f12, //Dark1 1119 add 720p 
0x2037,
0x211f, //Dark1 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark1 1138 //Pre Flat R1 LumL
0x2439,
0x25ff, //Dark1 1139 //Pre Flat R1 LumH
0x263a,
0x2700, //Dark1 113a
0x283b,
0x2900, //Dark1 113b
0x2a3c,
0x2b00, //Dark1 113c
0x2c3d,
0x2d53, //Dark1 113d
0x2e3e,
0x2f00, //Dark1 113e
0x303f,
0x3100, //Dark1 113f
0x3240,
0x3300, //Dark1 1140
0x3441,
0x352d, //Dark1 1141
0x3642,
0x3700, //Dark1 1142
0x3843,
0x3900, //Dark1 1143
0x3a49,
0x3b06, //Dark1 1149 add 720p 
0x3c4a,
0x3d0a, //Dark1 114a add 720p 
0x3e4b,
0x3f12, //Dark1 114b add 720p 
0x404c,
0x411c, //Dark1 114c add 720p 
0x424d,
0x4324, //Dark1 114d add 720p 
0x444e,
0x4540, //Dark1 114e add 720p 
0x464f,
0x4780, //Dark1 114f add 720p 
0x4850,
0x493f, //Dark1 1150 
0x4a51,
0x4b3f, //Dark1 1151
0x4c52,
0x4d3f, //Dark1 1152
0x4e53,
0x4f3d, //Dark1 1153
0x5054,
0x513c, //Dark1 1154
0x5255,
0x5338, //Dark1 1155
0x5456,
0x5536, //Dark1 1156
0x5657,
0x5734, //Dark1 1157
0x5858,
0x593f, //Dark1 1158
0x5a59,
0x5b3f, //Dark1 1159
0x5c5a,
0x5d3e, //Dark1 115a
0x5e5b,
0x5f38, //Dark1 115b
0x605c,
0x6133, //Dark1 115c
0x625d,
0x6331, //Dark1 115d
0x645e,
0x6530, //Dark1 115e
0x665f,
0x6730, //Dark1 115f
0x686e,
0x6920, //Dark1 116e
0x6a6f,
0x6b18, //Dark1 116f
0x6c77,
0x6d12, //Dark1 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f0f, //Dark1 1178 //Bayer SP Lum Pos2
0x7079,
0x710f, //Dark1 1179 //Bayer SP Lum Pos3
0x727a,
0x7312, //Dark1 117a //Bayer SP Lum Pos4
0x747b,
0x7512, //Dark1 117b //Bayer SP Lum Pos5
0x767c,
0x7712, //Dark1 117c //Bayer SP Lum Pos6
0x787d,
0x7912, //Dark1 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b12, //Dark1 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d12, //Dark1 117f //Bayer SP Lum Neg1
0x7e80,
0x7f0f, //Dark1 1180 //Bayer SP Lum Neg2
0x8081,
0x810f, //Dark1 1181 //Bayer SP Lum Neg3
0x8282,
0x8312, //Dark1 1182 //Bayer SP Lum Neg4
0x8483,
0x8512, //Dark1 1183 //Bayer SP Lum Neg5
0x8684,
0x8712, //Dark1 1184 //Bayer SP Lum Neg6
0x8885,
0x8912, //Dark1 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b12, //Dark1 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d0f, //Dark1 118f //Bayer SP Dy Pos1
0x8e90,
0x8f0f, //Dark1 1190 //Bayer SP Dy Pos2
0x9091,
0x9112, //Dark1 1191 //Bayer SP Dy Pos3
0x9292,
0x9312, //Dark1 1192 //Bayer SP Dy Pos4
0x9493,
0x9512, //Dark1 1193 //Bayer SP Dy Pos5
0x9694,
0x9712, //Dark1 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark1 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b12, //Dark1 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d0f, //Dark1 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f0f, //Dark1 1198 //Bayer SP Dy Neg2
0xa099,
0xa112, //Dark1 1199 //Bayer SP Dy Neg3
0xa29a,
0xa312, //Dark1 119a //Bayer SP Dy Neg4
0xa49b,
0xa512, //Dark1 119b //Bayer SP Dy Neg5
0xa69c,
0xa712, //Dark1 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark1 119d //Bayer SP Dy Neg7
0xaa9e,
0xab12, //Dark1 119e //Bayer SP Dy Neg8
0xaca7,
0xad1c, //Dark1 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark1 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark1 11a9 //Bayer SP Edge3
0xb2aa,
0xb318, //Dark1 11aa //Bayer SP Edge4
0xb4ab,
0xb51d, //Dark1 11ab //Bayer SP Edge5
0xb6ac,
0xb720, //Dark1 11ac //Bayer SP Edge6
0xb8ad,
0xb920, //Dark1 11ad //Bayer SP Edge7
0xbaae,
0xbb20, //Dark1 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Dark1 11b7 add 720p 
0xbeb8,
0xbf22, //Dark1 11b8 add 720p 
0xc0b9,
0xc121, //Dark1 11b9 add 720p 
0xc2ba,
0xc31e, //Dark1 11ba add 720p 
0xc4bb,
0xc51c, //Dark1 11bb add 720p 
0xc6bc,
0xc71a, //Dark1 11bc add 720p 
0xc8c7,
0xc912, //Dark1 11c7 //Bayer SP STD1
0xcac8,
0xcb12, //Dark1 11c8 //Bayer SP STD2
0xccc9,
0xcd13, //Dark1 11c9 //Bayer SP STD3
0xceca,
0xcf18, //Dark1 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark1 11cb //Bayer SP STD5
0xd2cc,
0xd318, //Dark1 11cc //Bayer SP STD6
0xd4cd,
0xd518, //Dark1 11cd //Bayer SP STD7
0xd6ce,
0xd718, //Dark1 11ce //Bayer SP STD8
0xd8cf,
0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark1 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xddff, //Dark1 11d1
0xded2,
0xdf00, //Dark1 11d2
0xe0d3,
0xe1ff, //Dark1 11d3
0xe2d4,
0xe300, //Dark1 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe557,//Dark1 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark1 11d6
0xe8d7,
0xe92a, //Dark1 11d7
0xead8,
0xeb00, //Dark1 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed27, //Dark1 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark1 11da
0xf0db,
0xf120, //Dark1 11db
0xf2df,
0xf3ff, //Dark1 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Dark1 11e0
0xf6e1,
0xf77a, //Dark1 11e1
0xf8e2,
0xf900, //Dark1 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Dark1 11e3
0xfce4,
0xfd01, //Dark1 11e4
0x0e00, // burst end

0x03e5, //DMA E5 Page
0x0e01, // burst start

0x10e5,
0x1121, //Dark1 11e5
0x12e6,
0x1300, //Dark1 11e6
0x14e7,
0x1500, //Dark1 11e7
0x16e8,
0x1701, //Dark1 11e8
0x18e9,
0x191d, //Dark1 11e9
0x1aea,
0x1b00, //Dark1 11ea
0x1ceb,
0x1d00, //Dark1 11eb
0x1eef,
0x1fff, //Dark1 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Dark1 1240 add 720p 
0x2470,
0x259f, //Dark1 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Dark1 1271 //Bayer HPF Gain
0x2872,
0x2916, //Dark1 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Dark1 1277
0x2c78,
0x2d2f, //Dark1 1278
0x2e79,
0x2fff, //Dark1 1279
0x307a,
0x3150, //Dark1 127a
0x327b,
0x3310, //Dark1 127b
0x347c,
0x3564, //Dark1 127c //skin HPF gain
0x367d,
0x3720, //Dark1 127d
0x387f,
0x3950, //Dark1 127f
0x3a87,
0x3b3f, //Dark1 1287 add 720p 
0x3c88,
0x3d3f, //Dark1 1288 add 720p 
0x3e89,
0x3f3f, //Dark1 1289 add 720p 
0x408a,
0x413f, //Dark1 128a add 720p 
0x428b,
0x433f, //Dark1 128b add 720p 
0x448c,
0x453f, //Dark1 128c add 720p 
0x468d,
0x473f, //Dark1 128d add 720p 
0x488e,
0x493f, //Dark1 128e add 720p 
0x4a8f,
0x4b3f, //Dark1 128f add 720p 
0x4c90,
0x4d3f, //Dark1 1290 add 720p 
0x4e91,
0x4f3f, //Dark1 1291 add 720p 
0x5092,
0x513f, //Dark1 1292 add 720p 
0x5293,
0x533f, //Dark1 1293 add 720p 
0x5494,
0x553f, //Dark1 1294 add 720p 
0x5695,
0x573f, //Dark1 1295 add 720p 
0x5896,
0x593f, //Dark1 1296 add 720p 
0x5aae,
0x5b7f, //Dark1 12ae
0x5caf,
0x5d80, //Dark1 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark1 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Dark1 12c3 add 720p 
0x62c4,
0x631a, //Dark1 12c4 add 720p 
0x64c5,
0x650c, //Dark1 12c5 add 720p 
0x66c6,
0x6791, //Dark1 12c6
0x68c7,
0x69a4, //Dark1 12c7
0x6ac8,
0x6b3c, //Dark1 12c8
0x6cd0,
0x6d08, //Dark1 12d0 add 720p 
0x6ed1,
0x6f10, //Dark1 12d1 add 720p 
0x70d2,
0x7118, //Dark1 12d2 add 720p 
0x72d3,
0x7320, //Dark1 12d3 add 720p 
0x74d4,
0x7530, //Dark1 12d4 add 720p 
0x76d5,
0x7760, //Dark1 12d5 add 720p 
0x78d6,
0x7980, //Dark1 12d6 add 720p 
0x7ad7,
0x7b38, //Dark1 12d7
0x7cd8,
0x7d30, //Dark1 12d8
0x7ed9,
0x7f2a, //Dark1 12d9
0x80da,
0x812a, //Dark1 12da
0x82db,
0x8324, //Dark1 12db
0x84dc,
0x8520, //Dark1 12dc
0x86dd,
0x871a, //Dark1 12dd
0x88de,
0x8916, //Dark1 12de
0x8ae0,
0x8b63, //Dark1 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark1 12e1
0x8ee2,
0x8f02, //Dark1 12e2
0x90e3,
0x9110, //Dark1 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark1 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark1 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark1 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark1 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark1 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark1 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark1 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark1 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark1 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark1 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark1 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark1 12f0
0xaaf1,
0xab2a, //Dark1 12f1
0xacf2,
0xad32, //Dark1 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Dark1 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Dark1 1330
0xb431,
0xb520, //Dark1 1331
0xb632,
0xb720, //Dark1 1332
0xb833,
0xb920, //Dark1 1333
0xba34,
0xbb20, //Dark1 1334
0xbc35,
0xbd20, //Dark1 1335
0xbe36,
0xbf20, //Dark1 1336
0xc037,
0xc120, //Dark1 1337
0xc238,
0xc302, //Dark1 1338
0xc440,
0xc518, //Dark1 1340
0xc641,
0xc736, //Dark1 1341
0xc842,
0xc962, //Dark1 1342
0xca43,
0xcb63, //Dark1 1343
0xcc44,
0xcdff, //Dark1 1344
0xce45,
0xcf04, //Dark1 1345
0xd046,
0xd145, //Dark1 1346
0xd247,
0xd305, //Dark1 1347
0xd448,
0xd565, //Dark1 1348
0xd649,
0xd702, //Dark1 1349
0xd84a,
0xd922, //Dark1 134a
0xda4b,
0xdb06, //Dark1 134b
0xdc4c,
0xdd30, //Dark1 134c
0xde83,
0xdf08, //Dark1 1383
0xe084,
0xe10a, //Dark1 1384
0xe2b7,
0xe3fa, //Dark1 13b7
0xe4b8,
0xe577, //Dark1 13b8
0xe6b9,
0xe7fe, //Dark1 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Dark1 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark1 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark1 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark1 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark1 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Dark1 1410
0xf611,
0xf798, //Dark1 1411
0xf812,
0xf910, //Dark1 1412
0xfa13,
0xfb03, //Dark1 1413
0xfc14,
0xfd23,//Dark1 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x1015,
0x117b, //Dark1 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Dark1 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark1 1417
0x1618,
0x170c, //Dark1 1418
0x1819,
0x190c, //Dark1 1419
0x1a1a,
0x1b24,//Dark1 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d24,//Dark1 141b //YC2D Post STD gain Neg
0x1e27,
0x1f14, //Dark1 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2114, //Dark1 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2314, //Dark1 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2514, //Dark1 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Dark1 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark1 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b14, //Dark1 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d14, //Dark1 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f12, //Dark1 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3112, //Dark1 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3312, //Dark1 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark1 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Dark1 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Dark1 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark1 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d12, //Dark1 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f14, //Dark1 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4114, //Dark1 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4314, //Dark1 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4514, //Dark1 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Dark1 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark1 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b14, //Dark1 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d14, //Dark1 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f12, //Dark1 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5112, //Dark1 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5312, //Dark1 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5512, //Dark1 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5712, //Dark1 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5912, //Dark1 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark1 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Dark1 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f20, //Dark1 1467 //YC2D SP Edge Gain1
0x6068,
0x6120, //Dark1 1468 //YC2D SP Edge Gain2
0x6269,
0x6320, //Dark1 1469 //YC2D SP Edge Gain3
0x646a,
0x6520, //Dark1 146a //YC2D SP Edge Gain4
0x666b,
0x6720, //Dark1 146b //YC2D SP Edge Gain5
0x686c,
0x6920, //Dark1 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b20, //Dark1 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d20, //Dark1 146e //YC2D SP Edge Gain8
0x6e87,
0x6f2a,//Dark1 1487 //YC2D SP STD Gain1
0x7088,
0x712a,//Dark1 1488 //YC2D SP STD Gain2
0x7289,
0x732a,//Dark1 1489 //YC2D SP STD Gain3
0x748a,
0x752a,//Dark1 148a //YC2D SP STD Gain4
0x768b,
0x772a,//Dark1 148b //YC2D SP STD Gain5
0x788c,
0x791a, //Dark1 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Dark1 148d //YC2D SP STD Gain7
0x7c8e,
0x7d1a, //Dark1 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Dark1 1497 add 720p 
0x8098,
0x813f, //Dark1 1498 add 720p 
0x8299,
0x833f, //Dark1 1499 add 720p 
0x849a,
0x853f, //Dark1 149a add 720p 
0x869b,
0x873f, //Dark1 149b add 720p 
0x88a0,
0x893f, //Dark1 14a0 add 720p 
0x8aa1,
0x8b3f, //Dark1 14a1 add 720p 
0x8ca2,
0x8d3f, //Dark1 14a2 add 720p 
0x8ea3,
0x8f3f, //Dark1 14a3 add 720p 
0x90a4,
0x913f, //Dark1 14a4 add 720p 
0x92c9,
0x9313, //Dark1 14c9
0x94ca,
0x9527, //Dark1 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Dark1 1A10 add 720p 
0x9a18,
0x9b3f, //Dark1 1A18
0x9c19,
0x9d3f, //Dark1 1A19
0x9e1a,
0x9f2a, //Dark1 1A1a
0xa01b,
0xa127, //Dark1 1A1b
0xa21c,
0xa323, //Dark1 1A1c
0xa41d,
0xa523, //Dark1 1A1d
0xa61e,
0xa723, //Dark1 1A1e
0xa81f,
0xa923, //Dark1 1A1f
0xaa20,
0xabe7, //Dark1 1A20 add 720p 
0xac2f,
0xadf1, //Dark1 1A2f add 720p 
0xae32,
0xaf87, //Dark1 1A32 add 720p 
0xb034,
0xb1d0, //Dark1 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Dark1 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Dark1 1A36
0xb637,
0xb740, //Dark1 1A37
0xb838,
0xb9ff, //Dark1 1A38
0xba39,
0xbb11, //Dark1 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark1 1A3a
0xbe3b,
0xbf00, //Dark1 1A3b
0xc03c,
0xc14c, //Dark1 1A3c
0xc23d,
0xc300, //Dark1 1A3d
0xc43e,
0xc513, //Dark1 1A3e
0xc63f,
0xc700, //Dark1 1A3f
0xc840,
0xc92a, //Dark1 1A40
0xca41,
0xcb00, //Dark1 1A41
0xcc42,
0xcd17, //Dark1 1A42
0xce43,
0xcf2c, //Dark1 1A43
0xd04d,
0xd112, //Dark1 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Dark1 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd512, //Dark1 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark1 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd912, //Dark1 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb12, //Dark1 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd12, //Dark1 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf12, //Dark1 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe112, //Dark1 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe312, //Dark1 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Dark1 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark1 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Dark1 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Dark1 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Dark1 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Dark1 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Dark1 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Dark1 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf512, //Dark1 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf712, //Dark1 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf912, //Dark1 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb12, //Dark1 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd12, //Dark1 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E6 page.
0xDC2E,
0xDDB2,

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark1 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Dark1 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Dark1 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Dark1 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Dark1 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b12, //Dark1 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d12, //Dark1 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f12, //Dark1 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark1 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark1 1A7d //RGB SP Edge Gain1
0x247e,
0x2520, //Dark1 1A7e //RGB SP Edge Gain2
0x267f,
0x2720, //Dark1 1A7f //RGB SP Edge Gain3
0x2880,
0x2920, //Dark1 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b20, //Dark1 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d20, //Dark1 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f20, //Dark1 1A83 //RGB SP Edge Gain7
0x3084,
0x3120, //Dark1 1A84 //RGB SP Edge Gain8
0x329e,
0x331a, //Dark1 1A9e //RGB SP STD Gain1
0x349f,
0x351a, //Dark1 1A9f //RGB SP STD Gain2
0x36a0,
0x371a, //Dark1 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391a, //Dark1 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark1 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d1a, //Dark1 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f1a, //Dark1 1Aa4 //RGB SP STD Gain7
0x40a5,
0x411a, //Dark1 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4336,//Dark1 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Dark1 1Aa7 add 720p
0x46a8,
0x473f, //Dark1 1Aa8 add 720p
0x48a9,
0x493f, //Dark1 1Aa9 add 720p
0x4aaa,
0x4b3f, //Dark1 1Aaa add 720p
0x4cab,
0x4d3f, //Dark1 1Aab add 720p
0x4eaf,
0x4f3f, //Dark1 1Aaf add 720p
0x50b0,
0x513f, //Dark1 1Ab0 add 720p
0x52b1,
0x533f, //Dark1 1Ab1 add 720p
0x54b2,
0x553f, //Dark1 1Ab2 add 720p
0x56b3,
0x573f, //Dark1 1Ab3 add 720p
0x58ca,
0x5900, //Dark1 1Aca
0x5ae3,
0x5b13, //Dark1 1Ae3 add 720p
0x5ce4,
0x5d13, //Dark1 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark1 1070 Trans Func.   130108 Dark1 transFuc Flat graph
0x6271,
0x630a, //Dark1 1071
0x6472,
0x65be, //Dark1 1072
0x6673,
0x67cc, //Dark1 1073
0x6874,
0x6900, //Dark1 1074
0x6a75,
0x6b00, //Dark1 1075
0x6c76,
0x6d20, //Dark1 1076
0x6e77,
0x6f33, //Dark1 1077
0x7078,
0x7133, //Dark1 1078
0x7279,
0x7349, //Dark1 1079
0x747a,
0x7599, //Dark1 107a
0x767b,
0x7749, //Dark1 107b
0x787c,
0x7999, //Dark1 107c
0x7a7d,
0x7b07, //Dark1 107d
0x7c7e,
0x7d0f, //Dark1 107e
0x7e7f,
0x7f1e, //Dark1 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark1 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark1 031a (for sun-spot)
0x881b,
0x897c, //Dark1 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark1 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark1 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9104, //Dark1 11f0 (for af bug)
0x9203,            
0x9312, // 12 page  
0x9412,            
0x9503, //Dark1 1212
0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E7 ~ E9 Page (DMA Dark2)
///////////////////////////////////////////////////////////////////////////////

0x03e7, //DMA E7 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Dark2 1111 add 720p
0x1414,
0x1500, //Dark2 1114 add 720p
0x1615,
0x1781, //Dark2 1115 add 720p
0x1816,
0x1904, //Dark2 1116 add 720p
0x1a17,
0x1b58, //Dark2 1117 add 720p
0x1c18,
0x1d30, //Dark2 1118 add 720p
0x1e19,
0x1f12, //Dark2 1119 add 720p
0x2037,
0x211f, //Dark2 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark2 1138 //Pre Flat R1 LumL
0x2439,
0x25ff, //Dark2 1139 //Pre Flat R1 LumH
0x263a,
0x2700, //Dark2 113a
0x283b,
0x2900, //Dark2 113b
0x2a3c,
0x2b00, //Dark2 113c
0x2c3d,
0x2d53, //Dark2 113d
0x2e3e,
0x2f00, //Dark2 113e
0x303f,
0x3100, //Dark2 113f
0x3240,
0x3300, //Dark2 1140
0x3441,
0x352d, //Dark2 1141
0x3642,
0x3700, //Dark2 1142
0x3843,
0x3900, //Dark2 1143
0x3a49,
0x3b06, //Dark2 1149 add 720p
0x3c4a,
0x3d0a, //Dark2 114a add 720p
0x3e4b,
0x3f12, //Dark2 114b add 720p
0x404c,
0x411c, //Dark2 114c add 720p
0x424d,
0x4324, //Dark2 114d add 720p
0x444e,
0x4540, //Dark2 114e add 720p
0x464f,
0x4780, //Dark2 114f add 720p
0x4850,
0x493f, //Dark2 1150 
0x4a51,
0x4b3f, //Dark2 1151
0x4c52,
0x4d3f, //Dark2 1152
0x4e53,
0x4f3d, //Dark2 1153
0x5054,
0x513c, //Dark2 1154
0x5255,
0x5338, //Dark2 1155
0x5456,
0x5536, //Dark2 1156
0x5657,
0x5734, //Dark2 1157
0x5858,
0x593f, //Dark2 1158
0x5a59,
0x5b3f, //Dark2 1159
0x5c5a,
0x5d3e, //Dark2 115a
0x5e5b,
0x5f38, //Dark2 115b
0x605c,
0x6133, //Dark2 115c
0x625d,
0x6331, //Dark2 115d
0x645e,
0x6530, //Dark2 115e
0x665f,
0x6730, //Dark2 115f
0x686e,
0x6920, //Dark2 116e
0x6a6f,
0x6b18, //Dark2 116f
0x6c77,
0x6d12, //Dark2 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f0f, //Dark2 1178 //Bayer SP Lum Pos2
0x7079,
0x710f, //Dark2 1179 //Bayer SP Lum Pos3
0x727a,
0x7312, //Dark2 117a //Bayer SP Lum Pos4
0x747b,
0x7512, //Dark2 117b //Bayer SP Lum Pos5
0x767c,
0x7712, //Dark2 117c //Bayer SP Lum Pos6
0x787d,
0x7912, //Dark2 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b12, //Dark2 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d12, //Dark2 117f //Bayer SP Lum Neg1
0x7e80,
0x7f0f, //Dark2 1180 //Bayer SP Lum Neg2
0x8081,
0x810f, //Dark2 1181 //Bayer SP Lum Neg3
0x8282,
0x8312, //Dark2 1182 //Bayer SP Lum Neg4
0x8483,
0x8512, //Dark2 1183 //Bayer SP Lum Neg5
0x8684,
0x8712, //Dark2 1184 //Bayer SP Lum Neg6
0x8885,
0x8912, //Dark2 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b12, //Dark2 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d0f, //Dark2 118f //Bayer SP Dy Pos1
0x8e90,
0x8f0f, //Dark2 1190 //Bayer SP Dy Pos2
0x9091,
0x9112, //Dark2 1191 //Bayer SP Dy Pos3
0x9292,
0x9312, //Dark2 1192 //Bayer SP Dy Pos4
0x9493,
0x9512, //Dark2 1193 //Bayer SP Dy Pos5
0x9694,
0x9712, //Dark2 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark2 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b12, //Dark2 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d0f, //Dark2 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f0f, //Dark2 1198 //Bayer SP Dy Neg2
0xa099,
0xa112, //Dark2 1199 //Bayer SP Dy Neg3
0xa29a,
0xa312, //Dark2 119a //Bayer SP Dy Neg4
0xa49b,
0xa512, //Dark2 119b //Bayer SP Dy Neg5
0xa69c,
0xa712, //Dark2 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark2 119d //Bayer SP Dy Neg7
0xaa9e,
0xab12, //Dark2 119e //Bayer SP Dy Neg8
0xaca7,
0xad1c, //Dark2 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark2 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark2 11a9 //Bayer SP Edge3
0xb2aa,
0xb318, //Dark2 11aa //Bayer SP Edge4
0xb4ab,
0xb51d, //Dark2 11ab //Bayer SP Edge5
0xb6ac,
0xb720, //Dark2 11ac //Bayer SP Edge6
0xb8ad,
0xb920, //Dark2 11ad //Bayer SP Edge7
0xbaae,
0xbb20, //Dark2 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Dark2 11b7 add 720p
0xbeb8,
0xbf22, //Dark2 11b8 add 720p
0xc0b9,
0xc121, //Dark2 11b9 add 720p
0xc2ba,
0xc31e, //Dark2 11ba add 720p
0xc4bb,
0xc51c, //Dark2 11bb add 720p
0xc6bc,
0xc71a, //Dark2 11bc add 720p
0xc8c7,
0xc912, //Dark2 11c7 //Bayer SP STD1
0xcac8,
0xcb12, //Dark2 11c8 //Bayer SP STD2
0xccc9,
0xcd13, //Dark2 11c9 //Bayer SP STD3
0xceca,
0xcf18, //Dark2 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark2 11cb //Bayer SP STD5
0xd2cc,
0xd318, //Dark2 11cc //Bayer SP STD6
0xd4cd,
0xd518, //Dark2 11cd //Bayer SP STD7
0xd6ce,
0xd718, //Dark2 11ce //Bayer SP STD8
0xd8cf,
0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark2 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xddff, //Dark2 11d1
0xded2,
0xdf00, //Dark2 11d2
0xe0d3,
0xe1ff, //Dark2 11d3
0xe2d4,
0xe300, //Dark2 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe557,//Dark2 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark2 11d6
0xe8d7,
0xe92a, //Dark2 11d7
0xead8,
0xeb00, //Dark2 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed27, //Dark2 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark2 11da
0xf0db,
0xf120, //Dark2 11db
0xf2df,
0xf3ff, //Dark2 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Dark2 11e0
0xf6e1,
0xf77a, //Dark2 11e1
0xf8e2,
0xf900, //Dark2 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Dark2 11e3
0xfce4,
0xfd01, //Dark2 11e4
0x0e00, // burst end

0x03e8, //DMA E8 Page
0x0e01, // burst start

0x10e5,
0x1121, //Dark2 11e5
0x12e6,
0x1300, //Dark2 11e6
0x14e7,
0x1500, //Dark2 11e7
0x16e8,
0x1701, //Dark2 11e8
0x18e9,
0x191d, //Dark2 11e9
0x1aea,
0x1b00, //Dark2 11ea
0x1ceb,
0x1d00, //Dark2 11eb
0x1eef,
0x1fff, //Dark2 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Dark2 1240 add 720p
0x2470,
0x259f, //Dark2 1270 // Bayer Sharpness ENB add 720p
0x2671,
0x271a, //Dark2 1271 //Bayer HPF Gain
0x2872,
0x2916, //Dark2 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Dark2 1277
0x2c78,
0x2d2f, //Dark2 1278
0x2e79,
0x2fff, //Dark2 1279
0x307a,
0x3150, //Dark2 127a
0x327b,
0x3310, //Dark2 127b
0x347c,
0x3564, //Dark2 127c //skin HPF gain
0x367d,
0x3720, //Dark2 127d
0x387f,
0x3950, //Dark2 127f
0x3a87,
0x3b3f, //Dark2 1287 add 720p
0x3c88,
0x3d3f, //Dark2 1288 add 720p
0x3e89,
0x3f3f, //Dark2 1289 add 720p
0x408a,
0x413f, //Dark2 128a add 720p
0x428b,
0x433f, //Dark2 128b add 720p
0x448c,
0x453f, //Dark2 128c add 720p
0x468d,
0x473f, //Dark2 128d add 720p
0x488e,
0x493f, //Dark2 128e add 720p
0x4a8f,
0x4b3f, //Dark2 128f add 720p
0x4c90,
0x4d3f, //Dark2 1290 add 720p
0x4e91,
0x4f3f, //Dark2 1291 add 720p
0x5092,
0x513f, //Dark2 1292 add 720p
0x5293,
0x533f, //Dark2 1293 add 720p
0x5494,
0x553f, //Dark2 1294 add 720p
0x5695,
0x573f, //Dark2 1295 add 720p
0x5896,
0x593f, //Dark2 1296 add 720p
0x5aae,
0x5b7f, //Dark2 12ae
0x5caf,
0x5d80, //Dark2 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark2 12c0 // CI-LPF ENB add 720p
0x60c3,
0x613c, //Dark2 12c3 add 720p
0x62c4,
0x631a, //Dark2 12c4 add 720p
0x64c5,
0x650c, //Dark2 12c5 add 720p
0x66c6,
0x6791, //Dark2 12c6
0x68c7,
0x69a4, //Dark2 12c7
0x6ac8,
0x6b3c, //Dark2 12c8
0x6cd0,
0x6d08, //Dark2 12d0 add 720p
0x6ed1,
0x6f10, //Dark2 12d1 add 720p
0x70d2,
0x7118, //Dark2 12d2 add 720p
0x72d3,
0x7320, //Dark2 12d3 add 720p
0x74d4,
0x7530, //Dark2 12d4 add 720p
0x76d5,
0x7760, //Dark2 12d5 add 720p
0x78d6,
0x7980, //Dark2 12d6 add 720p
0x7ad7,
0x7b38, //Dark2 12d7
0x7cd8,
0x7d30, //Dark2 12d8
0x7ed9,
0x7f2a, //Dark2 12d9
0x80da,
0x812a, //Dark2 12da
0x82db,
0x8324, //Dark2 12db
0x84dc,
0x8520, //Dark2 12dc
0x86dd,
0x871a, //Dark2 12dd
0x88de,
0x8916, //Dark2 12de
0x8ae0,
0x8b63, //Dark2 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark2 12e1
0x8ee2,
0x8f02, //Dark2 12e2
0x90e3,
0x9110, //Dark2 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark2 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark2 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark2 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark2 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark2 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark2 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark2 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark2 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark2 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark2 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark2 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark2 12f0
0xaaf1,
0xab2a, //Dark2 12f1
0xacf2,
0xad32, //Dark2 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb181, //Dark2 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Dark2 1330
0xb431,
0xb520, //Dark2 1331
0xb632,
0xb720, //Dark2 1332
0xb833,
0xb920, //Dark2 1333
0xba34,
0xbb20, //Dark2 1334
0xbc35,
0xbd20, //Dark2 1335
0xbe36,
0xbf20, //Dark2 1336
0xc037,
0xc120, //Dark2 1337
0xc238,
0xc302, //Dark2 1338
0xc440,
0xc518, //Dark2 1340
0xc641,
0xc736, //Dark2 1341
0xc842,
0xc962, //Dark2 1342
0xca43,
0xcb63, //Dark2 1343
0xcc44,
0xcdff, //Dark2 1344
0xce45,
0xcf04, //Dark2 1345
0xd046,
0xd145, //Dark2 1346
0xd247,
0xd305, //Dark2 1347
0xd448,
0xd565, //Dark2 1348
0xd649,
0xd702, //Dark2 1349
0xd84a,
0xd922, //Dark2 134a
0xda4b,
0xdb06, //Dark2 134b
0xdc4c,
0xdd30, //Dark2 134c
0xde83,
0xdf08, //Dark2 1383
0xe084,
0xe10a, //Dark2 1384
0xe2b7,
0xe3fa, //Dark2 13b7
0xe4b8,
0xe577, //Dark2 13b8
0xe6b9,
0xe7fe, //Dark2 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Dark2 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark2 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark2 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark2 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark2 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Dark2 1410
0xf611,
0xf798, //Dark2 1411
0xf812,
0xf910, //Dark2 1412
0xfa13,
0xfb03, //Dark2 1413
0xfc14,
0xfd23,//Dark2 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x1015,
0x117b, //Dark2 1415 // Y Hi filter mask 1/16
0x1216,
0x1310, //Dark2 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark2 1417
0x1618,
0x170c, //Dark2 1418
0x1819,
0x190c, //Dark2 1419
0x1a1a,
0x1b24,//Dark2 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d24,//Dark2 141b //YC2D Post STD gain Neg
0x1e27,
0x1f14, //Dark2 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2114, //Dark2 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2314, //Dark2 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2514, //Dark2 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Dark2 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark2 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b14, //Dark2 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d14, //Dark2 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f12, //Dark2 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3112, //Dark2 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3312, //Dark2 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark2 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Dark2 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Dark2 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark2 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d12, //Dark2 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f14, //Dark2 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4114, //Dark2 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4314, //Dark2 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4514, //Dark2 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Dark2 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark2 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b14, //Dark2 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d14, //Dark2 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f12, //Dark2 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5112, //Dark2 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5312, //Dark2 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5512, //Dark2 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5712, //Dark2 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5912, //Dark2 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark2 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Dark2 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f20, //Dark2 1467 //YC2D SP Edge Gain1
0x6068,
0x6120, //Dark2 1468 //YC2D SP Edge Gain2
0x6269,
0x6320, //Dark2 1469 //YC2D SP Edge Gain3
0x646a,
0x6520, //Dark2 146a //YC2D SP Edge Gain4
0x666b,
0x6720, //Dark2 146b //YC2D SP Edge Gain5
0x686c,
0x6920, //Dark2 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b20, //Dark2 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d20, //Dark2 146e //YC2D SP Edge Gain8
0x6e87,
0x6f2a,//Dark2 1487 //YC2D SP STD Gain1
0x7088,
0x712a,//Dark2 1488 //YC2D SP STD Gain2
0x7289,
0x732a,//Dark2 1489 //YC2D SP STD Gain3
0x748a,
0x752a,//Dark2 148a //YC2D SP STD Gain4
0x768b,
0x772a,//Dark2 148b //YC2D SP STD Gain5
0x788c,
0x791a, //Dark2 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1a, //Dark2 148d //YC2D SP STD Gain7
0x7c8e,
0x7d1a, //Dark2 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Dark2 1497 add 720p
0x8098,
0x813f, //Dark2 1498 add 720p
0x8299,
0x833f, //Dark2 1499 add 720p
0x849a,
0x853f, //Dark2 149a add 720p
0x869b,
0x873f, //Dark2 149b add 720p
0x88a0,
0x893f, //Dark2 14a0 add 720p
0x8aa1,
0x8b3f, //Dark2 14a1 add 720p
0x8ca2,
0x8d3f, //Dark2 14a2 add 720p
0x8ea3,
0x8f3f, //Dark2 14a3 add 720p
0x90a4,
0x913f, //Dark2 14a4 add 720p
0x92c9,
0x9313, //Dark2 14c9
0x94ca,
0x9527, //Dark2 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Dark2 1A10 add 720p
0x9a18,
0x9b3f, //Dark2 1A18
0x9c19,
0x9d3f, //Dark2 1A19
0x9e1a,
0x9f2a, //Dark2 1A1a
0xa01b,
0xa127, //Dark2 1A1b
0xa21c,
0xa323, //Dark2 1A1c
0xa41d,
0xa523, //Dark2 1A1d
0xa61e,
0xa723, //Dark2 1A1e
0xa81f,
0xa923, //Dark2 1A1f
0xaa20,
0xabe7, //Dark2 1A20 add 720p
0xac2f,
0xadf1, //Dark2 1A2f add 720p
0xae32,
0xaf87, //Dark2 1A32 add 720p
0xb034,
0xb1d0, //Dark2 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Dark2 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Dark2 1A36
0xb637,
0xb740, //Dark2 1A37
0xb838,
0xb9ff, //Dark2 1A38
0xba39,
0xbb11, //Dark2 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark2 1A3a
0xbe3b,
0xbf00, //Dark2 1A3b
0xc03c,
0xc14c, //Dark2 1A3c
0xc23d,
0xc300, //Dark2 1A3d
0xc43e,
0xc513, //Dark2 1A3e
0xc63f,
0xc700, //Dark2 1A3f
0xc840,
0xc92a, //Dark2 1A40
0xca41,
0xcb00, //Dark2 1A41
0xcc42,
0xcd17, //Dark2 1A42
0xce43,
0xcf2c, //Dark2 1A43
0xd04d,
0xd112, //Dark2 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Dark2 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd512, //Dark2 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark2 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd912, //Dark2 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb12, //Dark2 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd12, //Dark2 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf12, //Dark2 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe112, //Dark2 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe312, //Dark2 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Dark2 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark2 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Dark2 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Dark2 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Dark2 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Dark2 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Dark2 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Dark2 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf512, //Dark2 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf712, //Dark2 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf912, //Dark2 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb12, //Dark2 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd12, //Dark2 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E9 page.
0xE232,
0xE36A,

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark2 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Dark2 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Dark2 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Dark2 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Dark2 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b12, //Dark2 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d12, //Dark2 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f12, //Dark2 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark2 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark2 1A7d //RGB SP Edge Gain1
0x247e,
0x2520, //Dark2 1A7e //RGB SP Edge Gain2
0x267f,
0x2720, //Dark2 1A7f //RGB SP Edge Gain3
0x2880,
0x2920, //Dark2 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b20, //Dark2 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d20, //Dark2 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f20, //Dark2 1A83 //RGB SP Edge Gain7
0x3084,
0x3120, //Dark2 1A84 //RGB SP Edge Gain8
0x329e,
0x331a, //Dark2 1A9e //RGB SP STD Gain1
0x349f,
0x351a, //Dark2 1A9f //RGB SP STD Gain2
0x36a0,
0x371a, //Dark2 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391a, //Dark2 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark2 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d1a, //Dark2 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f1a, //Dark2 1Aa4 //RGB SP STD Gain7
0x40a5,
0x411a, //Dark2 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4336,//Dark2 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Dark2 1Aa7 add 720p
0x46a8,
0x473f, //Dark2 1Aa8 add 720p
0x48a9,
0x493f, //Dark2 1Aa9 add 720p
0x4aaa,
0x4b3f, //Dark2 1Aaa add 720p
0x4cab,
0x4d3f, //Dark2 1Aab add 720p
0x4eaf,
0x4f3f, //Dark2 1Aaf add 720p
0x50b0,
0x513f, //Dark2 1Ab0 add 720p
0x52b1,
0x533f, //Dark2 1Ab1 add 720p
0x54b2,
0x553f, //Dark2 1Ab2 add 720p
0x56b3,
0x573f, //Dark2 1Ab3 add 720p
0x58ca,
0x5900, //Dark2 1Aca
0x5ae3,
0x5b13, //Dark2 1Ae3 add 720p
0x5ce4,
0x5d13, //Dark2 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark2 1070 Trans Func.   130108 Dark2 transFuc Flat graph
0x6271,
0x6306, //Dark2 1071
0x6472,
0x65be, //Dark2 1072
0x6673,
0x6799, //Dark2 1073
0x6874,
0x6900, //Dark2 1074
0x6a75,
0x6b00, //Dark2 1075
0x6c76,
0x6d20, //Dark2 1076
0x6e77,
0x6f33, //Dark2 1077
0x7078,
0x7133, //Dark2 1078
0x7279,
0x7340, //Dark2 1079
0x747a,
0x7500, //Dark2 107a
0x767b,
0x7740, //Dark2 107b
0x787c,
0x7900, //Dark2 107c
0x7a7d,
0x7b07, //Dark2 107d
0x7c7e,
0x7d0f, //Dark2 107e
0x7e7f,
0x7f1e, //Dark2 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark2 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark2 031a (for sun-spot)
0x881b,
0x897c, //Dark2 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark2 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark2 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9105, //Dark2 11f0 (for af bug)
0x9203,            
0x9312, // 12 page  
0x9412,            
0x9503, //Dark2 1212
0x0e00, // burst end

//--------------------------------------------------------------------------//
// MIPI TX Setting  //PCLK 54MHz
//--------------------------------------------------------------------------//
0x0305,  // Page05
0x1100,  // lvds_ctl_2 //Phone set not continuous           
0x1200,  // crc_ctl
0x1300,  // serial_ctl
0x1400,  // ser_out_ctl_1
0x1500,  // dphy_fifo_ctl
0x1602,  // lvds_inout_ctl1
0x1700,  // lvds_inout_ctl2
0x1880,  // lvds_inout_ctl3
0x1900,  // lvds_inout_ctl4
0x1af0,  // lvds_time_ctl
0x1c01,  // tlpx_time_l_dp
0x1d0f,  // tlpx_time_l_dn
0x1e08,  // hs_zero_time
0x1f0a,  // hs_trail_time
0x21b8,  // hs_sync_code
0x2200,  // frame_start_id
0x2301,  // frame_end_id
0x241e,  // long_packet_id
0x2500,  // s_pkt_wc_h
0x2600,  // s_pkt_wc_l
0x2708,  // lvds_frame_end_cnt_h
0x2800,  // lvds_frame_end_cnt_l
0x2a06,  // lvds_image_width_h
0x2b40,  // lvds_image_width_l
0x2c04,  // lvds_image_height_h
0x2db0,  // lvds_image_height_l
0x3008, // l_pkt_wc_h  // Pre = 1024 * 2 (YUV)
0x3100,  // l_pkt_wc_l
0x320f,  // clk_zero_time
0x330b,  // clk_post_time
0x3403,  // clk_prepare_time
0x3504,  // clk_trail_time
0x3601,  // clk_tlpx_time_dp
0x3706,  // clk_tlpx_time_dn
0x3907,  // lvds_bias_ctl
0x3a00,  // lvds_test_tx
0x4200,  // mipi_test_width_l
0x4300,  // mipi_test_height_l
0x4400,  // mipi_test_size_h
0x4500,  // mipi_test_hsync_st
0x4600,  // mipi_test_hblank
0x4700,  // mipi_test_vsync_st
0x4800,  // mipi_test_vsync_end
0x49ff,  // ulps_size_opt1
0x4a0a,  // ulps_size_opt2
0x4b22,  // ulps_size_opt3
0x4c41,  // hs_wakeup_size_h
0x4d20,  // hs_wakeup_size_l
0x4e00,  // mipi_int_time_h
0x4fff,  // mipi_int_time_l
0x500A,  // cntns_clk_wait_h
0x5100,  // cntns_clk_wait_l
0x5740,  // mipi_dmy_reg
0x6000,  // mipi_frame_pkt_opt
0x6108,  // line_cnt_value_h
0x6200,  // line_cnt_value_l
0x101c,  // lvds_ctl_1

};
 #endif
/*===========================================*/
/*CAMERA_RECORDING WITH 30fps  */
/*============================================*/
#if 0
static SensorConfigScript sr352_recording_50Hz_HD_60hz[] = {


///////////////////////////////////////////////////////////////////////////////
// HD Initial Start
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// mcu clock enable for bus release
///////////////////////////////////////////////////////////////////////////////
0x0326,
0x1089,
0x1080,

///////////////////////////////////////////////////////////////////////////////
// reset
///////////////////////////////////////////////////////////////////////////////
0x0300,
0x0101,
0x0107,
0x0101,

0x0daa, // ESD Check Register
0x0faa, // ESD Check Register

///////////////////////////////////////////////////////////////////////////////
// pad drive / pll setting
///////////////////////////////////////////////////////////////////////////////

0x0300,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0207,		// pclk_drive = 000b, i2c_drive = 111b
0x0c07,		// d_pad_drive = 000b, gpio_pad_drive = 111b
//////////////////////////////////////////////////////////////////
0x0725, //mode_pll1  24mhz / (5+1) = 4mhz
0x0856,	//mode_pll2  isp clk = 86Mhz;
0x0981, //mode_pll3  // MIPI 4x div 1/1 // isp clk div = 1/4
0x07A5,
0x07A5,
0x07A5,
//OUTPUT: MIPI interface /////////////////////////////////////////
0x0A60, //mode_pll4 for mipi mode
0x0Ae0, //mode_pll4 for mipi mode

0x0326,
0x1B03,		// bus clk div = 1/4

///////////////////////////////////////////////////////////////////////////////
// 7 Page(memory configuration)
///////////////////////////////////////////////////////////////////////////////
0x0307,
0x2101,	//SSD sram clock inv on
0x3345,	//bit[6]:C-NR DC ���� ����

///////////////////////////////////////////////////////////////////////////////
// mcu reset
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x1080,		// mcu reset
0x1089,		// mcu clk enable
0x1108,		// xdata clear
0x1100,		// xdata clear
0xff01,   // delay 10ms

///////////////////////////////////////////////////////////////////////////////
// opt download
///////////////////////////////////////////////////////////////////////////////

0x030A,
0x1200,	// otp clock enable

// timing for 86mhz
0x402f,	// otp cfg 1
0x4155,	// otp cfg 2
0x422f,	// otp cfg 3
0x432f,	// otp cfg 4
0x442f,	// otp cfg 5
0x4522,	// otp cfg 6
0x465a,	// otp cfg 7
0x4709,	// otp cfg 8
0x4802,	// otp cfg 9
0x49b8,	// otp cfg 10
0x4A2f,	// otp cfg 11
0x4B85,	// otp cfg 12
0x4C55,	// otp cfg 13

0xff01,	//delay 10ms

// downlaod otp - system data
0x2000,	// otp addr = Otp:0000h
0x2100,	// otp addr = Otp:0000h
0x2000,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2100,	// otp addr = Otp:0000h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1301,	// start download system data
0x1300,	// toggle start

0xff01,	//delay 10ms

// download otp - mcu data
0x2000,	// otp addr = Otp:0080h
0x2180,	// otp addr = Otp:0080h
0x2000,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0080h (otp addr must be set twice)
0x2e01,	// otp download size = 0100
0x2f00,	// otp download size = 0100
0x1801,	// link xdata to otp
0x3010,	// otp mcu buffer addr = Xdata:105Dh
0x315D,	// otp mcu buffer addr = Xdata:105Dh
0x1302,	// start download mcu data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

// download otp - dpc data
0x2001,	// otp addr = Otp:0180h
0x2180,	// otp addr = Otp:0180h
0x2001,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2180,	// otp addr = Otp:0180h (otp addr must be set twice)
0x2e00,	// otp download size = 0080
0x2f80,	// otp download size = 0080
0x1801,	// link xdata to otp
0x3033,	// otp mcu buffer addr = Xdata:3384h
0x3184,	// otp mcu buffer addr = Xdata:3384h
0x1304,	// start download dpc data
0x1300,	// toggle start

0xff01,	//delay 10ms

0x1800,	// link xdata to mcu

0x030A,
0x1280,	// otp clock disable

///////////////////////////////////////////////////////////////////////////////
// 0 Page
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1000,	// Full size
0x1190,	// Fixed mode off
0x1200,
0x1300,
0x1501,
0x1700,	// Clock inversion off
0x1800,
0x1D0D,	//Group_frame_update
0x1E01,	//Group_frame_update_reset
0x2000,
0x2100,	// preview row start set
0x2200,
0x2308,	// preview col start set
0x2402,
0x25d0,	// height 720
0x2605,
0x2700,	// width 1280

///////////////////////////////////////////////////////////////////////////////
//ONE LINE SETTING
0x0300,
0x4c07,	// one_line = 1850
0x4d3a,

///////////////////////////////////////////////////////////////////////////////
0x5200,	// Vsync H
0x5314,	// Vsync L
///////////////////////////////////////////////////////////////////////////////

//Pixel windowing
0x8001,	// pixel_row_start for 720p crop mode
0x819e,	// pwin_row_start = 414
0x8202,	// pwin_row_height = 738
0x83e2,
0x8401,	// pwin_col_start = 384
0x8580,
0x8605,	// pwin_col_width = 1312
0x8720,

///////////////////////////////////////////////////////////////////////////////
// 1 Page
///////////////////////////////////////////////////////////////////////////////

0x0301,
0x1062,	// BLC=ON, column BLC, col_OBP DPC
0x1111,   // BLC offset ENB + Adaptive BLC ENB B[4]
0x1200,
0x1339,	// BLC(Frame BLC ofs - Column ALC ofs)+FrameALC skip
0x1400,
0x238F,	// Frame BLC avg ���� for 8 frame
0x5004, // blc height = 4
0x5144,
0x6000,
0x6100,
0x6200,
0x6300,
0x787f,	// ramp_rst_offset = 128
0x7904,	// ramp offset
0x7b04,	// ramp offset
0x7e00,

///////////////////////////////////////////////////////////////////////////////
// 2 Page
///////////////////////////////////////////////////////////////////////////////

0x0302,
0x1b80,
0x1d40,
0x2310,
0x4008,
0x418a,	//20130213 Rev BC ver. ADC input range @ 800mv
0x460a,	// + 3.3V, -0.9V
0x4717, // 20121129 2.9V
0x481a,
0x4913,
0x54c0,
0x5540,
0x5633,
0xa001,
0xa17c,
0xa203,
0xa34d,
0xa403,
0xa5b0,
0xa606,
0xa7f2,
0xa801,
0xa94f,
0xaa02,
0xab23,
0xac02,
0xad74,
0xae04,
0xaf17,

///////////////////////////////////////////////////////////////////////////////
// 3 Page
///////////////////////////////////////////////////////////////////////////////

0x0303,
0x1a06, // cds_s1
0x1b7c,
0x1e06,
0x1f7c,
0x4200,
0x4346,
0x4600,
0x4774,
0x4a00,
0x4b44,
0x4e00,
0x4f44,
0x5200,
0x5320,
0x5600,
0x5720,
0x5A00,
0x5b20,
0x6A00,
0x6B6f,
0x7206, // s_addr_cut
0x7340,
0x7806, // rx half_rst
0x793b,
0x7A06,
0x7B45,
0x7C06,
0x7D3b,
0x7E06,
0x7F45,
0x8406, // tx half_rst
0x853b,
0x8606,
0x8745,
0x8806,
0x893b,
0x8A06,
0x8B45,
0x9206, // sx
0x9331,
0x9606,
0x9731,
0x9806, // sxb
0x9931,
0x9c06,
0x9d31,

0xe000,
0xe120,
0xfc06, // clamp_sig
0xfd38,

///////////////////////////////////////////////////////////////////////////////
// 4 Page
///////////////////////////////////////////////////////////////////////////////

0x0304,
0x1003,	//Ramp multiple sampling

0x5a06, // cds_pxl_smpl
0x5b78,
0x5e06,
0x5f78,
0x6206,
0x6378,

///////////////////////////////////////////////////////////////////////////////
// mcu start
///////////////////////////////////////////////////////////////////////////////

0x0326,
0x6200,	// normal mode start
0x6500,	// watchdog disable
0x1009,	// mcu reset release
//Analog setting ���Ŀ� MCU�� reset ��Ŵ.

///////////////////////////////////////////////////////////////////////////////
// b Page
///////////////////////////////////////////////////////////////////////////////
0x030b,
0x1001, // otp_dpc_ctl
0x1111, //HD 0415
0x1200, //HD 0415

///////////////////////////////////////////////////////////////////////////////
// 15 Page (LSC)
///////////////////////////////////////////////////////////////////////////////

0x0315,
0x1000,	// LSC OFF
0x1100, //gap y disable

///////////////////////////////////////////////////////////////////////////////
// set lsc parameter
///////////////////////////////////////////////////////////////////////////////

0x030a,
0x1901,

0x1180, // B[7] LSC burst mode ENB

0x0326,
0x4002,	// auto increment enable
0x4401,
0x45a3,	// LSC bank0 start addr H
0x4600,	// LSC bank0 start addr L

//LSC G channel reg________________________ 20130416 LSC Blending DNP 90_CWF 5_TL84 5

0x0e01, //BURST_START

0x423c, //G Value 
0x4213,
0x42af,
0x423d,
0x4263,
0x42b1,
0x4237,
0x42a3,
0x423d,
0x4230,
0x4202,
0x42cf,
0x422b,
0x4222,
0x42b0,
0x422c,
0x42d2,
0x42fe,
0x4233,
0x42f3,
0x427e,
0x423b,
0x4273,
0x42d7,
0x423a,
0x42d3,
0x42e1,
0x4237,
0x4293,
0x4283,
0x4238,
0x42d3,
0x4260,
0x4231,
0x42e2,
0x42d4,
0x4228,
0x42d2,
0x4257,
0x4223,
0x4292,
0x4237,
0x4225,
0x4262,
0x428f,
0x422d,
0x4283,
0x4224,
0x4236,
0x4293,
0x4296,
0x4239,
0x42b3,
0x42a0,
0x4236,
0x4293,
0x4257,
0x4234,
0x4253,
0x4210,
0x422c,
0x4212,
0x426b,
0x4221,
0x42a1,
0x42df,
0x421b,
0x42f1,
0x42bf,
0x421d,
0x42f2,
0x4220,
0x4227,
0x4222,
0x42cb,
0x4231,
0x42b3,
0x4255,
0x4238,
0x4293,
0x42bd,
0x4234,
0x42b3,
0x422a,
0x4230,
0x42a2,
0x42c8,
0x4226,
0x4272,
0x4201,
0x421a,
0x4261,
0x4264,
0x4214,
0x4221,
0x4244,
0x4216,
0x4281,
0x42ab,
0x4220,
0x42a2,
0x4274,
0x422d,
0x4283,
0x4222,
0x4236,
0x42f3,
0x42bd,
0x4233,
0x4253,
0x4209,
0x422d,
0x42d2,
0x428c,
0x4221,
0x4281,
0x42a5,
0x4214,
0x4230,
0x42fd,
0x420d,
0x42a0,
0x42dc,
0x4210,
0x4241,
0x424c,
0x421b,
0x4222,
0x4228,
0x4229,
0x42f2,
0x42fb,
0x4235,
0x4273,
0x42b3,
0x4231,
0x4262,
0x42e1,
0x422a,
0x42c2,
0x4249,
0x421c,
0x4291,
0x424d,
0x420e,
0x4270,
0x42a2,
0x4207,
0x42d0,
0x4280,
0x420a,
0x4270,
0x42f2,
0x4215,
0x42d1,
0x42dc,
0x4225,
0x42e2,
0x42cc,
0x4233,
0x4243,
0x429c,
0x4230,
0x4292,
0x42cb,
0x4228,
0x42c2,
0x421f,
0x4219,
0x4251,
0x4214,
0x420a,
0x42a0,
0x4263,
0x4204,
0x4200,
0x4243,
0x4206,
0x42b0,
0x42b7,
0x4212,
0x4241,
0x42a8,
0x4223,
0x4242,
0x42b0,
0x4231,
0x42f3,
0x428e,
0x422f,
0x42b2,
0x42b5,
0x4227,
0x4201,
0x42fa,
0x4216,
0x42b0,
0x42e7,
0x4207,
0x42c0,
0x4236,
0x4201,
0x4230,
0x4216,
0x4203,
0x42e0,
0x428a,
0x420f,
0x4271,
0x427f,
0x4220,
0x42f2,
0x4295,
0x4230,
0x42a3,
0x4280,
0x422f,
0x42a2,
0x42b2,
0x4226,
0x4291,
0x42ef,
0x4215,
0x42e0,
0x42d8,
0x4206,
0x42d0,
0x4226,
0x4200,
0x4240,
0x4206,
0x4202,
0x42f0,
0x427a,
0x420e,
0x4271,
0x4270,
0x4220,
0x4242,
0x428f,
0x4230,
0x4293,
0x4283,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422f,
0x4242,
0x42ad,
0x4226,
0x4261,
0x42ec,
0x4215,
0x42c0,
0x42d6,
0x4206,
0x42c0,
0x4224,
0x4200,
0x4230,
0x4205,
0x4202,
0x42e0,
0x4279,
0x420e,
0x4261,
0x426d,
0x4220,
0x4222,
0x428d,
0x4230,
0x4253,
0x427e,
0x4230,
0x4202,
0x42bd,
0x4227,
0x42a2,
0x4203,
0x4217,
0x4260,
0x42f0,
0x4208,
0x4260,
0x4240,
0x4201,
0x42d0,
0x4220,
0x4204,
0x4280,
0x4294,
0x4210,
0x4211,
0x4287,
0x4221,
0x4292,
0x42a0,
0x4231,
0x4293,
0x4291,
0x4230,
0x4292,
0x42ce,
0x4229,
0x4242,
0x4226,
0x4219,
0x42d1,
0x421b,
0x420b,
0x4210,
0x426a,
0x4204,
0x4280,
0x424c,
0x4207,
0x4230,
0x42bf,
0x4212,
0x42b1,
0x42ae,
0x4223,
0x4292,
0x42b8,
0x4232,
0x42a3,
0x429c,
0x4231,
0x42e2,
0x42ee,
0x422b,
0x42e2,
0x425d,
0x421d,
0x42e1,
0x4261,
0x420f,
0x4270,
0x42b1,
0x4208,
0x42f0,
0x4291,
0x420b,
0x42a1,
0x4206,
0x4217,
0x4211,
0x42ed,
0x4226,
0x42f2,
0x42e3,
0x4234,
0x42f3,
0x42bb,
0x4233,
0x4263,
0x420e,
0x422e,
0x4262,
0x4296,
0x4222,
0x4251,
0x42b1,
0x4214,
0x42e1,
0x4209,
0x420e,
0x4270,
0x42ea,
0x4211,
0x4211,
0x425b,
0x421b,
0x42f2,
0x4233,
0x422a,
0x4273,
0x420b,
0x4237,
0x4223,
0x42d9,
0x4235,
0x4213,
0x4235,
0x4231,
0x4292,
0x42da,
0x4227,
0x42b2,
0x4214,
0x421b,
0x4291,
0x4278,
0x4215,
0x4271,
0x425a,
0x4218,
0x4201,
0x42c5,
0x4222,
0x4212,
0x4287,
0x422e,
0x42a3,
0x4241,
0x4239,
0x42e3,
0x42fc,
0x4234,
0x4213,
0x423a,
0x4233,
0x4223,
0x4201,
0x422b,
0x4282,
0x4264,
0x4221,
0x4271,
0x42df,
0x421c,
0x4211,
0x42c4,
0x421e,
0x4272,
0x4223,
0x4227,
0x4212,
0x42c4,
0x4231,
0x4243,
0x425f,
0x423a,
0x4293,
0x42f3,
0x4231,
0x42e3,
0x423a,
0x4235,
0x4263,
0x4231,
0x422f,
0x4232,
0x42b1,
0x4227,
0x4222,
0x4242,
0x4222,
0x4282,
0x422b,
0x4224,
0x4292,
0x427e,
0x422c,
0x4213,
0x4204,
0x4234,
0x42a3,
0x428a,
0x423a,
0x42a3,
0x42c9,
0x4234,
0x4233,
0x423b,
0x4237,
0x42b3,
0x4261,
0x4232,
0x42e2,
0x42fe,
0x422c,
0x42c2,
0x42a5,
0x4229,
0x4202,
0x4291,
0x422a,
0x42b2,
0x42d9,
0x4231,
0x4203,
0x4244,
0x4237,
0x42f3,
0x42b5,
0x423a,
0x42a3,
0x42f4,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x425f, //R Value
0x42a5,
0x42d8,
0x4261,
0x4235,
0x42e5,
0x4258,
0x42e5,
0x4229,
0x424c,
0x4244,
0x4273,
0x4244,
0x42e4,
0x424d,
0x4248,
0x4244,
0x42d9,
0x4254,
0x4245,
0x42af,
0x4260,
0x42c6,
0x423c,
0x425f,
0x4276,
0x4235,
0x425b,
0x4235,
0x425c,
0x4256,
0x4295,
0x4222,
0x424b,
0x4254,
0x423a,
0x423c,
0x4283,
0x4272,
0x4234,
0x4283,
0x424a,
0x4238,
0x4223,
0x42df,
0x4245,
0x4294,
0x42db,
0x4255,
0x4205,
0x429b,
0x4259,
0x4285,
0x42f8,
0x425b,
0x4265,
0x423a,
0x4250,
0x42e4,
0x42b0,
0x4242,
0x42b3,
0x429b,
0x4231,
0x42c2,
0x42c1,
0x4229,
0x4212,
0x4297,
0x422d,
0x4203,
0x4234,
0x423b,
0x42e4,
0x4257,
0x424e,
0x4245,
0x424b,
0x4259,
0x4236,
0x422b,
0x4259,
0x4265,
0x4203,
0x424b,
0x42f4,
0x4243,
0x4239,
0x42f2,
0x42f7,
0x4226,
0x4272,
0x4204,
0x421d,
0x4231,
0x42d9,
0x4221,
0x4232,
0x4280,
0x4231,
0x42a3,
0x42d2,
0x4247,
0x42d5,
0x4205,
0x4257,
0x42a6,
0x423e,
0x4257,
0x42f4,
0x42d4,
0x4247,
0x4293,
0x42e3,
0x4232,
0x4202,
0x4266,
0x421c,
0x42e1,
0x4268,
0x4213,
0x4261,
0x423c,
0x4217,
0x4291,
0x42eb,
0x4228,
0x42d3,
0x4255,
0x4242,
0x4204,
0x42c9,
0x4255,
0x4246,
0x422f,
0x4255,
0x4204,
0x4298,
0x4242,
0x42f3,
0x427c,
0x422a,
0x4281,
0x42e4,
0x4214,
0x4290,
0x42e4,
0x420b,
0x4240,
0x42b9,
0x420f,
0x4251,
0x4267,
0x4220,
0x42e2,
0x42e0,
0x423b,
0x42e4,
0x4282,
0x4252,
0x4236,
0x4214,
0x4253,
0x42d4,
0x4275,
0x423f,
0x42c3,
0x4237,
0x4225,
0x4241,
0x4289,
0x420e,
0x42e0,
0x428b,
0x4205,
0x42b0,
0x4260,
0x4209,
0x42c1,
0x420d,
0x421b,
0x4252,
0x428e,
0x4237,
0x4294,
0x4253,
0x4250,
0x4256,
0x4206,
0x4252,
0x4264,
0x4254,
0x423d,
0x4223,
0x4201,
0x4221,
0x4291,
0x424c,
0x420b,
0x4210,
0x424c,
0x4201,
0x42b0,
0x4221,
0x4205,
0x42e0,
0x42ce,
0x4217,
0x4262,
0x424f,
0x4234,
0x4224,
0x4229,
0x424e,
0x4255,
0x42f1,
0x4252,
0x4254,
0x424e,
0x423c,
0x4272,
0x42ed,
0x4220,
0x4231,
0x4236,
0x4209,
0x4280,
0x4233,
0x4200,
0x4250,
0x4208,
0x4204,
0x4250,
0x42b7,
0x4215,
0x42e2,
0x4237,
0x4232,
0x42f4,
0x421d,
0x424d,
0x42e5,
0x42ef,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4252,
0x4244,
0x424d,
0x423c,
0x4262,
0x42ec,
0x4220,
0x4231,
0x4234,
0x4209,
0x4280,
0x4232,
0x4200,
0x4240,
0x4207,
0x4204,
0x4240,
0x42b5,
0x4215,
0x42b2,
0x4235,
0x4232,
0x42d4,
0x421c,
0x424d,
0x42f5,
0x42f2,
0x4253,
0x4234,
0x4262,
0x423e,
0x4203,
0x420d,
0x4222,
0x4261,
0x4258,
0x420b,
0x42b0,
0x4256,
0x4202,
0x4260,
0x422c,
0x4206,
0x4270,
0x42d8,
0x4217,
0x42f2,
0x4258,
0x4234,
0x42d4,
0x4235,
0x424f,
0x4246,
0x4203,
0x4254,
0x4234,
0x4280,
0x4240,
0x42e3,
0x4247,
0x4226,
0x4271,
0x4299,
0x420f,
0x42b0,
0x4296,
0x4206,
0x4270,
0x426c,
0x420a,
0x4281,
0x4217,
0x421b,
0x42e2,
0x4296,
0x4238,
0x4224,
0x425c,
0x4250,
0x42c6,
0x420c,
0x4256,
0x4254,
0x42b1,
0x4244,
0x42d3,
0x429c,
0x422c,
0x4292,
0x4203,
0x4216,
0x4220,
0x42fd,
0x420c,
0x42c0,
0x42d1,
0x4210,
0x42b1,
0x427f,
0x4222,
0x4252,
0x42f5,
0x423d,
0x4244,
0x429c,
0x4253,
0x42d6,
0x422f,
0x4258,
0x4204,
0x42e3,
0x4249,
0x4273,
0x42fe,
0x4233,
0x42d2,
0x4282,
0x421e,
0x42a1,
0x4282,
0x4215,
0x4201,
0x4256,
0x4219,
0x4212,
0x4203,
0x422a,
0x4253,
0x4267,
0x4243,
0x4234,
0x42e0,
0x4257,
0x4246,
0x4258,
0x425a,
0x4255,
0x421d,
0x424e,
0x4254,
0x426e,
0x423c,
0x42b3,
0x4222,
0x4229,
0x4222,
0x422f,
0x4220,
0x4202,
0x4206,
0x4223,
0x42e2,
0x42ae,
0x4234,
0x4263,
0x42f5,
0x424a,
0x4205,
0x4236,
0x425b,
0x4216,
0x427d,
0x4258,
0x42e5,
0x422a,
0x4251,
0x4254,
0x42be,
0x4244,
0x4203,
0x42b8,
0x4233,
0x42c2,
0x42e5,
0x422b,
0x4292,
0x42c0,
0x422f,
0x4263,
0x4257,
0x423d,
0x42c4,
0x426c,
0x424f,
0x4295,
0x426e,
0x425c,
0x4296,
0x4275,
0x4255,
0x42c5,
0x4226,
0x4255,
0x4245,
0x4213,
0x424b,
0x4234,
0x4249,
0x423e,
0x4213,
0x4294,
0x4237,
0x4203,
0x4277,
0x423a,
0x4294,
0x4201,
0x4247,
0x4224,
0x42ea,
0x4255,
0x42e5,
0x42b8,
0x425c,
0x4276,
0x423a,
0x4258,
0x4275,
0x427c,
0x425e,
0x4245,
0x42b9,
0x4257,
0x4255,
0x4229,
0x424d,
0x4254,
0x4293,
0x4247,
0x4274,
0x427e,
0x424a,
0x42c4,
0x42fb,
0x4255,
0x4295,
0x42b7,
0x4261,
0x4236,
0x4252,
0x4261,
0x42f6,
0x4270,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x4240, //B Value
0x4263,
0x42cb,
0x423e,
0x4263,
0x42cc,
0x4239,
0x4283,
0x426e,
0x4233,
0x42c3,
0x4212,
0x422f,
0x42f2,
0x42f7,
0x4230,
0x42d3,
0x422a,
0x4235,
0x42b3,
0x428a,
0x423b,
0x42d3,
0x42e6,
0x423b,
0x42e4,
0x4218,
0x4239,
0x4253,
0x4285,
0x4237,
0x4253,
0x4252,
0x4231,
0x42a2,
0x42e1,
0x422a,
0x4272,
0x427a,
0x4226,
0x4232,
0x425d,
0x4227,
0x4232,
0x429a,
0x422d,
0x4243,
0x4210,
0x4234,
0x42a3,
0x4279,
0x4239,
0x4223,
0x42ab,
0x4237,
0x42b3,
0x423f,
0x4230,
0x4242,
0x42d9,
0x4229,
0x42d2,
0x4255,
0x4221,
0x4221,
0x42e2,
0x421c,
0x4271,
0x42c3,
0x421d,
0x42a2,
0x420a,
0x4224,
0x42d2,
0x4296,
0x422d,
0x4263,
0x420b,
0x4236,
0x4253,
0x42bf,
0x4235,
0x4243,
0x420c,
0x422c,
0x4242,
0x4290,
0x4224,
0x4211,
0x42eb,
0x421a,
0x4201,
0x4266,
0x4214,
0x42a1,
0x4249,
0x4216,
0x4231,
0x4298,
0x421e,
0x4262,
0x423c,
0x4229,
0x4212,
0x42d5,
0x4233,
0x42a3,
0x42a0,
0x4233,
0x4272,
0x42e5,
0x4229,
0x4242,
0x4255,
0x421f,
0x4271,
0x4294,
0x4214,
0x4201,
0x4203,
0x420e,
0x4250,
0x42e4,
0x4210,
0x4231,
0x423d,
0x4219,
0x4211,
0x42f3,
0x4225,
0x4262,
0x42aa,
0x4231,
0x4293,
0x4288,
0x4231,
0x4242,
0x42b8,
0x4225,
0x42c2,
0x420e,
0x421a,
0x4281,
0x423e,
0x420e,
0x4260,
0x42a8,
0x4208,
0x42a0,
0x428b,
0x420a,
0x4290,
0x42e7,
0x4214,
0x4201,
0x42aa,
0x4221,
0x4282,
0x427a,
0x422f,
0x4233,
0x426c,
0x422f,
0x4242,
0x4299,
0x4223,
0x42d1,
0x42e3,
0x4217,
0x4201,
0x4204,
0x420a,
0x4280,
0x426a,
0x4204,
0x42c0,
0x424d,
0x4206,
0x42e0,
0x42af,
0x4210,
0x42a1,
0x427a,
0x421e,
0x42d2,
0x425c,
0x422d,
0x4293,
0x4255,
0x422e,
0x4252,
0x4280,
0x4221,
0x42b1,
0x42bb,
0x4214,
0x4260,
0x42d4,
0x4207,
0x4290,
0x423a,
0x4201,
0x42c0,
0x421e,
0x4203,
0x42f0,
0x4280,
0x420d,
0x42f1,
0x4250,
0x421c,
0x42c2,
0x423f,
0x422c,
0x4213,
0x4243,
0x422d,
0x4272,
0x4272,
0x4220,
0x42d1,
0x42a9,
0x4213,
0x4220,
0x42c2,
0x4206,
0x4250,
0x4226,
0x4200,
0x4290,
0x420b,
0x4202,
0x42d0,
0x426f,
0x420c,
0x42d1,
0x4241,
0x421b,
0x42f2,
0x4238,
0x422b,
0x42d3,
0x4242,

0x0e00, //BURST_END
0x0e01, //BURST_START

0x422d,
0x4212,
0x426c,
0x4220,
0x4281,
0x42a5,
0x4212,
0x42c0,
0x42be,
0x4206,
0x4200,
0x4222,
0x4200,
0x4220,
0x4205,
0x4202,
0x4290,
0x426c,
0x420c,
0x42a1,
0x423e,
0x421b,
0x42d2,
0x4235,
0x422b,
0x4293,
0x423d,
0x422e,
0x4202,
0x427d,
0x4221,
0x42b1,
0x42ba,
0x4214,
0x4250,
0x42d5,
0x4207,
0x42a0,
0x423b,
0x4201,
0x42b0,
0x421f,
0x4204,
0x4210,
0x4285,
0x420e,
0x4251,
0x4258,
0x421d,
0x4252,
0x424c,
0x422c,
0x42c3,
0x424c,
0x422e,
0x4272,
0x428d,
0x4223,
0x4231,
0x42db,
0x4216,
0x4280,
0x42fb,
0x420a,
0x4200,
0x4263,
0x4204,
0x4250,
0x4248,
0x4206,
0x42b0,
0x42ad,
0x4210,
0x42a1,
0x427c,
0x421f,
0x4242,
0x4262,
0x422e,
0x4213,
0x425f,
0x4230,
0x4292,
0x42b3,
0x4225,
0x42d2,
0x420d,
0x421a,
0x4231,
0x423b,
0x420e,
0x4210,
0x42a3,
0x4208,
0x4270,
0x428a,
0x420a,
0x42d0,
0x42f0,
0x4214,
0x42b1,
0x42b7,
0x4222,
0x4282,
0x428b,
0x4230,
0x4273,
0x4282,
0x4231,
0x42d2,
0x42d1,
0x4228,
0x4262,
0x4240,
0x421e,
0x4221,
0x4281,
0x4212,
0x42d0,
0x42f2,
0x420d,
0x4250,
0x42db,
0x420f,
0x42c1,
0x423b,
0x4219,
0x4231,
0x42f7,
0x4225,
0x42d2,
0x42b3,
0x4232,
0x42a3,
0x42a0,
0x4234,
0x4222,
0x42fd,
0x422b,
0x4282,
0x4282,
0x4223,
0x4201,
0x42da,
0x4218,
0x42c1,
0x4256,
0x4213,
0x42c1,
0x4240,
0x4216,
0x4211,
0x429c,
0x421e,
0x42c2,
0x4245,
0x4229,
0x42e2,
0x42ec,
0x4235,
0x42d3,
0x42ce,
0x4234,
0x4203,
0x4208,
0x422d,
0x4202,
0x42a7,
0x4226,
0x4242,
0x421e,
0x421d,
0x42c1,
0x42ad,
0x4219,
0x4261,
0x429c,
0x421b,
0x4271,
0x42eb,
0x4223,
0x4202,
0x4279,
0x422c,
0x4213,
0x4204,
0x4236,
0x4273,
0x42cb,
0x4233,
0x42c3,
0x422f,
0x4232,
0x4222,
0x42ff,
0x422c,
0x4242,
0x428b,
0x4225,
0x4222,
0x422a,
0x4221,
0x4262,
0x421d,
0x4223,
0x4262,
0x4263,
0x4229,
0x42f2,
0x42da,
0x4231,
0x42c3,
0x4259,
0x4238,
0x42d3,
0x42c1,
0x4238,
0x42e3,
0x4256,
0x4237,
0x4233,
0x4257,
0x4232,
0x4232,
0x42f7,
0x422c,
0x4282,
0x42a7,
0x4229,
0x4262,
0x429e,
0x422b,
0x4252,
0x42db,
0x4230,
0x42e3,
0x423b,
0x4237,
0x4283,
0x42ae,
0x423b,
0x4224,
0x4216,

0x0e00, //BURST_END

0x030a,
0x1902,	// Bus Switch

0x0315,	// Shading FPGA(Hi-352)
0x1001,	// LSC ON
0x1106, //gap y enb, gap x enb for 720p

0x2780,	// LSC G
0x2880,	// LSC B
0x2980,	// LSC R

0x308a, //gap x for 720p
0x3168, //gap y for 720p
0x366c, //lsc_win_x for 720p
0x3764, //lsc_win_y for 720p

///////////////////////////////////
// 10 Page Saturation (H/W)
///////////////////////////////////
0x0310,
0x1001,
0x1200, //Y OFS Disable
0x1700, //20121127 CSP option
0x1800, //20121127 CSP option
0x2004, //16_235 range scale down on
0x6003, //Sat ENB Transfer Function     //Transfunction on

///////////////////////////////////
// 11 Page D-LPF (H/W)
///////////////////////////////////
0x0311, //11 page
0x100f, //D-LPF ENB //DPC marker

0x1228, //20121120 character long line detection th
0x132c, //20121120 character short line detection th

0x1d12, // ORG_STD Ctrl
0x1e00,// 20130410_STD 03 -> 00
0x2178, // Color STD Gain
//Bayer Sharpness Gain Ctrl
0xb722, //SpColor_gain1
0xb822, //SpColor_gain2
0xb921, //SpColor_gain3
0xba1e, //SpColor_gain4
0xbb1c, //SpColor_gain5
0xbc1a, //SpColor_gain6

0xf2ff, //pga_dark1_hi
0xf3fc, //pga_dark_lo
///////////////////////////////////
// 12 Page DPC,GBGR (H/W)//////////
///////////////////////////////////
0x0312, //12 page
0x1057, //DPC ON
0x1230,
0x2b08, //white_th
0x2c08, //middle_h_th
0x2d08, //middle_l_th
0x2e06, //dark_th
0x2f10, //20121127 _DPC TH
0x3010, //20121127 _DPC TH
0x3110, //20121127 _DPC TH
0x3210, //20121127 _DPC TH
0x4188, //GBGR Cut off //46

///////////////////////////////////
// 12 Page CI-LPF (H/W)////////////
///////////////////////////////////

0xEF01, //Interpol Color filter On/Off

///////////////////////////////////
// 13 Page YC-2D_Y-NR (H/W)/////////
///////////////////////////////////
0x0313,

0x802d, //YC-2D_C-NR ENB, C-Filter DC option on B[7] //DC on 8b //DC off 2d
0x81ff, // add 20121210
0x82fe, // add 20121210

0x8532,
0x8608, // add 20121210

//==========================================================================
// C-Filter PS Reducing (Mask-Size Adjustment)

0x87ff, //C-mask near STD TH
0x88ff, //C-mask middle STD TH
0x89ff, //C-mask far STD TH

0x8a86, //color STD

0x970f, // C-filter Lum gain 1
0x980e,
0x990d,
0x9a0c,
0x9b0b,
0x9c0a,
0x9d09,
0x9e08,

0xa70f, // C-filter STD gain 1
0xa80e,
0xa90d,
0xaa0c,
0xab0b,
0xac0a,
0xad09,
0xae08,

//==========================================================================

///////////////////////////////////
// 14 Page YC-2D_Sharpness(H/W)
///////////////////////////////////
0x0314,
0x7720, //Yc2d_ee_color_gain1
0x7820, //Yc2d_ee_color_gain2
0x7920, //Yc2d_ee_color_gain3
0x7a1c, //Yc2d_ee_color_gain4
0x7b1b, //Yc2d_ee_color_gain5
0x7c1a, //Yc2d_ee_color_gain6
0x7d19, //Yc2d_ee_color_gain7
0x7e18, //Yc2d_ee_color_gain8

0xc000, //Yc2d_ee_lclip_gain_n1
0xc100, //Yc2d_ee_lclip_gain_n2
0xc200, //Yc2d_ee_lclip_gain_n3
0xc300, //Yc2d_ee_lclip_gain_n4
0xc401, //Yc2d_ee_lclip_gain_n5

///////////////////////////////////////////////////////////////////////////////
// 16 Page CMC / AWB Gain
///////////////////////////////////////////////////////////////////////////////

0x0316,
0x107f,	// CMC ENB	3f(spGrap off) 7f(spGrap on)
0x2052,// PS / LN

0xa003,	// WB gain on
0xa205,	// R_h (12bit = 8bit * 16)
0xa380,	// R_l
0xa407,	// B_h (12bit = 8bit * 16)
0xa580,	// B_l

0xD001,	//Bayer gain enable
///////////////////////////////////////////////////////////////////////////////
// 17 Page Gamma
///////////////////////////////////////////////////////////////////////////////

0x0317,
0x1007,	// GMA ENB //PS On
0x1252,// old:43 new:65

///////////////////////////////////////////////////////////////////////////////
// 18 Page MCMC
///////////////////////////////////////////////////////////////////////////////

0x0318,	// Page 18
0x1001,	// mcmc_ctl1
0x117f,	// mcmc_ctl2
0x5310,	// mcmc_ctl3

0x561b,	// mcmc_glb_sat_lvl_sp1
0x5739,	// mcmc_glb_sat_lvl_sp2
0x585a,	// mcmc_glb_sat_lvl_sp3
0x5980,	// mcmc_glb_sat_lvl_sp4
0x5aa6,	// mcmc_glb_sat_lvl_sp5
0x5bc1,	// mcmc_glb_sat_lvl_sp6
0x5ce8,	// mcmc_glb_sat_lvl_sp7
0x5d38,	// mcmc_glb_sat_gain_sp1
0x5e3a,	// mcmc_glb_sat_gain_sp2
0x5f3c,	// mcmc_glb_sat_gain_sp3
0x603f,	// mcmc_glb_sat_gain_sp4
0x613f,	// mcmc_glb_sat_gain_sp5
0x623f,	// mcmc_glb_sat_gain_sp6
0x633f,	// mcmc_glb_sat_gain_sp7
0x643f,	// mcmc_glb_sat_gain_sp8
0x6500,	// mcmc_std_ctl1
0x6600,	// mcmc_std_ctl2
0x6700,	// mcmc_std_ctl3

0x6cff,	// mcmc_lum_ctl1 sat hue offset
0x6d3f,	// mcmc_lum_ctl2 gain
0x6e00,	// mcmc_lum_ctl3 hue
0x6f00,	// mcmc_lum_ctl4 rgb offset
0x7000,	// mcmc_lum_ctl5 rgb scale

0xa100,
0xa201,	//star gain enb

///////////////////////////////////////////////////////////////////////////////
// 1A Page_RGB Y-NR, Y-Sharpness
///////////////////////////////////////////////////////////////////////////////

0x031a,
0x309f,	// RGB-Sharpness ENB // Flat-region RGB ENB B[1] //Green dis [7] On

0x8D20, //RGB-Color_Gain1
0x8E20, //RGB-Color_Gain2
0x8F20, //RGB-Color_Gain3
0x9020, //RGB-Color_Gain4
0x9120, //RGB-Color_Gain5

///////////////////////////////////////////////////////////////////////////////
// 20 Page (FZY)
///////////////////////////////////////////////////////////////////////////////

0x0320, //Page 20

0x10ad, //50hz bd, 60hz ad
0x12e0, //Dgain off 20 //Dgain enb_2x e0

0x3600, //EXP Unit 
0x3707,
0x383a,

0x51ff, //PGA Max
0x5220, //PGA Min x0.9

0x61FF,	// max ramp gain
0x6200,	// min ramp gain
0x60E0,	// ramp gain

0x7170, //DG MAX //D gain 1x = 80 = x 1 // D gain 2x = 40 = x 1
0x7240, //DG MIN

0x803a, //Y Target

///////////////////////////////////////////////////////////////////////////////
// 23 Page (AFC)
///////////////////////////////////////////////////////////////////////////////

0x0323, //Page 23
0x147A, //Flicker Line 100
0x1566, //Flicker Line 120
0x1001, //Frame Interval

///////////////////////////////////////////////////////////////////////////////
// 2A Page (SSD)
///////////////////////////////////////////////////////////////////////////////

0x032A,
0x1011,
0x1101,
0x1502,	//720p mode on
0x1650,	//SSD B gain int gain 1.5x
0x730C,	//start x in 720p
0x7408,	//start y in 720p
0x7564,	//width   in 720p
0x764A,	//height  in 720p

///////////////////////////////////////////////////////////////////////////////
//
// F/W setting start
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// C0 Page (SYSTEM)
///////////////////////////////////////////////////////////////////////////////
//OPCLK Setting
//43Mhz = 29020C0
0x03C0,
0x1402,
0x1590,
0x1620,
0x17C0,

///////////////////////////////////////////////////////////////////////////////
// C6 Page (SSD Y weight)
///////////////////////////////////////////////////////////////////////////////
//SSD_Matrix
0x03c6,
0x9E11,	//1 Line
0x9F11,
0xA011,
0xA111,
0xA211,
0xA311,

0xA411,	//2 Line
0xA511,
0xA611,
0xA711,
0xA811,
0xA911,

0xAA11,//3 Line
0xAB12,
0xAC22,
0xAD22,
0xAE21,
0xAF11,

0xB011,//4 Line
0xB112,
0xB222,
0xB322,
0xB421,
0xB511,

0xB611,//5 Line
0xB712,
0xB822,
0xB922,
0xBA21,
0xBB11,

0xBC11,//6 Line
0xBD12,
0xBE22,
0xBF22,
0xC021,
0xC111,

0xC211,//7 Line
0xC312,
0xC422,
0xC522,
0xC621,
0xC711,

0xC811,//8 Line
0xC911,
0xCA11,
0xCB11,
0xCC11,
0xCD11,

0xCE11,//9 Line
0xCF11,
0xD011,
0xD111,
0xD211,
0xD311,

///////////////////////////////////////////////////////////////////////////////
// C7 Page (AE)
///////////////////////////////////////////////////////////////////////////////
//Shutter Setting
0x03c7,
0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10
0x1203, // Slow AE
0x15c0, // SSD Patch Weight Y Mean On

0x3618, // Max 24fps
0x3718, // Max 24fps

0x3d22, // YTh Lock, Unlock0

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0x7002, // 50hz 82, 60hz 02
0xff01,
0x4c00, //SW ExpMin = 3700
0x4d00,
0x4e0e,
0x4f74,

0x4400, //Start ExpTime 120fps
0x4505,	
0x4672,	
0x47ba,	
0xa748, //Start ExpTime 120fps flaot
0xa8ae,	
0xa957,	
0xaa40,	

0x0320, //20 page
0x2000, //Start ExpTime 120fps
0x2105,
0x2272,
0x23ba,
0x2800, //HW ExpMin = 3700
0x290e,
0x2a74,

0x03c7,
0x1090,	//AE On (Band Off) 50hz b0, 60hz 90

///////////////////////////////////////////////////////////////////////////////
// D9 Page (Capture/Preview)
///////////////////////////////////////////////////////////////////////////////
0x03d9,
0x8c20,	//en_ramp_gain_auto

///////////////////////////////////////////////////////////////////////////////
// C8 ~ CC Page (AWB)
///////////////////////////////////////////////////////////////////////////////
0x03C8,
0x0e01, // burst start

0x10f6,
0x11c3,
0x12e0,
0x131a, //bCtl4
0x149f,
0x15c4,
0x1600,
0x1734,
0x1855,
0x1966,
0x1a66,
0x1b88,
0x1c88,
0x1d88,
0x1e88,
0x1faa,
0x20aa,
0x2100,
0x2201,
0x231e,
0x24ff,
0x251e,
0x2651, //init awb speed
0x27ff,
0x28f0,
0x2901,
0x2a00,
0x2b1e,
0x2c04,
0x2d0c,
0x2e1e,
0x2f00, //dwOutdoorCondInTh
0x3000, //dwOutdoorCondInTh_n01
0x3121, //dwOutdoorCondInTh_n02
0x3234, //dwOutdoorCondInTh_n03
0x3300, //dwOutdoorCondOutTh
0x3400, //dwOutdoorCondOutTh_n01
0x3527, //dwOutdoorCondOutTh_n02
0x3610, //dwOutdoorCondOutTh_n03
0x3700, //dwEvTh
0x3800, //dwEvTh_n01
0x3904,//dwEvTh_n02 //840fps
0x3aa6,//dwEvTh_n03
0x3b00, //dwEvTh_a01
0x3c00, //dwEvTh_a01_n01
0x3d08,//dwEvTh_a01_n02
0x3e23,//dwEvTh_a01_n03 //480fps
0x3f00, //dwEvTh_a02
0x4000, //dwEvTh_a02_n01
0x4120,//dwEvTh_a02_n02
0x428d,//dwEvTh_a02_n03 //120fps
0x4300, //dwEvTh_a03
0x4400, //dwEvTh_a03_n01
0x4561, //dwEvTh_a03_n02
0x46a8,//dwEvTh_a03_n03 //40fps
0x4700, //dwEvTh_a04
0x4804, //dwEvTh_a04_n01
0x49c4, //dwEvTh_a04_n02
0x4ab4, //dwEvTh_a04_n03
0x4b00, //dwEvTh_a05
0x4c0b, //dwEvTh_a05_n01
0x4d71, //dwEvTh_a05_n02
0x4eb0, //dwEvTh_a05_n03
0x4f96,//aAglMaxMinLmt
0x5058,//aAglMaxMinLmt_a01
0x519C,//aAglMaxMinLmt_a02
0x5260, //aAglMaxMinLmt_a03
0x53a0, //aAglMaxMinLmt_a04
0x5420, //aAglMaxMinLmt_a05
0x55a0, //aAglMaxMinLmt_a06
0x5638,//aAglMaxMinLmt_a07
0x5770,//aTgtWhtRgnBgMaxMinLmt      out2 max
0x585e,//aTgtWhtRgnBgMaxMinLmt_a01  out2 min
0x5974,//aTgtWhtRgnBgMaxMinLmt_a02  out1 max
0x5a5c,//aTgtWhtRgnBgMaxMinLmt_a03  out1 min
0x5b98,//aTgtWhtRgnBgMaxMinLmt_a04  in max
0x5c20,//aTgtWhtRgnBgMaxMinLmt_a05  in min
0x5d98,//aTgtWhtRgnBgMaxMinLmt_a06  dark max
0x5e42,//aTgtWhtRgnBgMaxMinLmt_a07  dark min

0x5f10, //bTgtWhtRgnBgStep
0x600a,//BpOption distance weight  def : 0a -> 04 -> 02

0x611e,
0x6234,
0x6380,
0x6410,
0x6501,
0x6604,
0x670e,
0x6800,
0x6932,
0x6a00,
0x6ba2,
0x6c02,
0x6d00,
0x6e00,
0x6f00,
0x7000,
0x7100,
0x7200,
0x7300,
0x7400,
0x7500,
0x7655,
0x7755,
0x7855,
0x7955,
0x7a55,
0x7b55,
0x7c55,
0x7d55,
0x7e55,
0x7f55,
0x8055,
0x8155,
0x8255,
0x8355,
0x8455,
0x8555,
0x8655,
0x8755,
0x8855,
0x8955,
0x8a55,
0x8b55,
0x8c55,
0x8d55,
0x8e55,
0x8f55,
0x9055,
0x9100,
0x9200,

0x9300, //Indoor_wRgIntOfs
0x94a0, //Indoor_wRgIntOfs_n01
0x9500, //Indoor_wBgIntOfs
0x96c0, //Indoor_wBgIntOfs_n01
0x9610, //Indoor_bRgStep
0x9710, //Indoor_bBgStep
0x9926, //Indoor_aTgtWhtRgnBg
0x9a29, //Indoor_aTgtWhtRgnBg_a01
0x9b2c, //Indoor_aTgtWhtRgnBg_a02
0x9c38, //Indoor_aTgtWhtRgnBg_a03
0x9d43, //Indoor_aTgtWhtRgnBg_a04
0x9e4d, //Indoor_aTgtWhtRgnBg_a05
0x9f59, //Indoor_aTgtWhtRgnBg_a06
0xa064, //Indoor_aTgtWhtRgnBg_a07
0xa16f, //Indoor_aTgtWhtRgnBg_a08
0xa27b, //Indoor_aTgtWhtRgnBg_a09
0xa38e,//Indoor_aTgtWhtRgnBg_a10
0xa4a0,//Indoor_aTgtWhtRgnRgLtLmt
0xa598,//Indoor_aTgtWhtRgnRgLtLmt_a01
0xa682,//Indoor_aTgtWhtRgnRgLtLmt_a02
0xa76d,//Indoor_aTgtWhtRgnRgLtLmt_a03
0xa860,//Indoor_aTgtWhtRgnRgLtLmt_a04
0xa956,//Indoor_aTgtWhtRgnRgLtLmt_a05
0xaa4c,//Indoor_aTgtWhtRgnRgLtLmt_a06
0xab45, //Indoor_aTgtWhtRgnRgLtLmt_a07
0xac40, //Indoor_aTgtWhtRgnRgLtLmt_a08
0xad3c, //Indoor_aTgtWhtRgnRgLtLmt_a09
0xae39,//Indoor_aTgtWhtRgnRgLtLmt_a10
0xafaa, //Indoor_aTgtWhtRgnRgRtLmt
0xb0a9, //Indoor_aTgtWhtRgnRgRtLmt_a01
0xb1a8,//Indoor_aTgtWhtRgnRgRtLmt_a02
0xb2a2,//Indoor_aTgtWhtRgnRgRtLmt_a03
0xb395,//Indoor_aTgtWhtRgnRgRtLmt_a04
0xb488,//Indoor_aTgtWhtRgnRgRtLmt_a05
0xb578,//Indoor_aTgtWhtRgnRgRtLmt_a06
0xb665,//Indoor_aTgtWhtRgnRgRtLmt_a07
0xb75a,//Indoor_aTgtWhtRgnRgRtLmt_a08
0xb850,//Indoor_aTgtWhtRgnRgRtLmt_a09
0xb94a,//Indoor_aTgtWhtRgnRgRtLmt_a10
0xba1b, //Indoor_aOptWhtRgnBg
0xbb1d, //Indoor_aOptWhtRgnBg_a01
0xbc1f, //Indoor_aOptWhtRgnBg_a02
0xbd2a, //Indoor_aOptWhtRgnBg_a03
0xbe38, //Indoor_aOptWhtRgnBg_a04
0xbf47, //Indoor_aOptWhtRgnBg_a05
0xc054, //Indoor_aOptWhtRgnBg_a06
0xc161, //Indoor_aOptWhtRgnBg_a07
0xc272, //Indoor_aOptWhtRgnBg_a08
0xc382, //Indoor_aOptWhtRgnBg_a09
0xc49a,//Indoor_aOptWhtRgnBg_a10
0xc5ad, //Indoor_aOptWhtRgnRgLtLmt
0xc698,//Indoor_aOptWhtRgnRgLtLmt_a01
0xc78a,//Indoor_aOptWhtRgnRgLtLmt_a02
0xc874,//Indoor_aOptWhtRgnRgLtLmt_a03
0xc95f,//Indoor_aOptWhtRgnRgLtLmt_a04
0xca50,//Indoor_aOptWhtRgnRgLtLmt_a05
0xcb46,//Indoor_aOptWhtRgnRgLtLmt_a06
0xcc40,//Indoor_aOptWhtRgnRgLtLmt_a07
0xcd39, //Indoor_aOptWhtRgnRgLtLmt_a08
0xce35, //Indoor_aOptWhtRgnRgLtLmt_a09
0xcf33, //Indoor_aOptWhtRgnRgLtLmt_a10
0xd0ba,//Indoor_aOptWhtRgnRgRtLmt
0xd1b9,//Indoor_aOptWhtRgnRgRtLmt_a01
0xd2b8,//Indoor_aOptWhtRgnRgRtLmt_a02
0xd3b5, //Indoor_aOptWhtRgnRgRtLmt_a03
0xd4ae, //Indoor_aOptWhtRgnRgRtLmt_a04
0xd5a1, //Indoor_aOptWhtRgnRgRtLmt_a05
0xd68c, //Indoor_aOptWhtRgnRgRtLmt_a06
0xd778,//Indoor_aOptWhtRgnRgRtLmt_a07
0xd860,//Indoor_aOptWhtRgnRgRtLmt_a08
0xd954,//Indoor_aOptWhtRgnRgRtLmt_a09
0xda4d,//Indoor_aOptWhtRgnRgRtLmt_a10

0xdb36, //Indoor_aCtmpWgtWdhTh
0xdc40, //Indoor_aCtmpWgtWdhTh_a01
0xdd4c, //Indoor_aCtmpWgtWdhTh_a02
0xde5c, //Indoor_aCtmpWgtWdhTh_a03
0xdf6e, //Indoor_aCtmpWgtWdhTh_a04
0xe07f, //Indoor_aCtmpWgtWdhTh_a05
0xe1a4, //Indoor_aCtmpWgtWdhTh_a06
0xe227, //Indoor_aCtmpWgtHgtTh
0xe332, //Indoor_aCtmpWgtHgtTh_a01
0xe43c, //Indoor_aCtmpWgtHgtTh_a02
0xe548,//Indoor_aCtmpWgtHgtTh_a03
0xe65c,//Indoor_aCtmpWgtHgtTh_a04
0xe770,//Indoor_aCtmpWgtHgtTh_a05
0xe87c,//Indoor_aCtmpWgtHgtTh_a06
0xe986,//Indoor_aCtmpWgtHgtTh_a07
0xea90, //Indoor_aCtmpWgtHgtTh_a08
0xeb11, //Indoor_aCtmpWgt
0xec11, //Indoor_aCtmpWgt_a01
0xed12, //Indoor_aCtmpWgt_a02
0xee11, //Indoor_aCtmpWgt_a03
0xef11, //Indoor_aCtmpWgt_a04
0xf033, //Indoor_aCtmpWgt_a05
0xf111, //Indoor_aCtmpWgt_a06
0xf214, //Indoor_aCtmpWgt_a07
0xf343, //Indoor_aCtmpWgt_a08
0xf411, //Indoor_aCtmpWgt_a09
0xf555, //Indoor_aCtmpWgt_a10
0xf641, //Indoor_aCtmpWgt_a11
0xf716, //Indoor_aCtmpWgt_a12
0xf865, //Indoor_aCtmpWgt_a13
0xf911, //Indoor_aCtmpWgt_a14
0xfa48, //Indoor_aCtmpWgt_a15
0xfb61, //Indoor_aCtmpWgt_a16
0xfc11, //Indoor_aCtmpWgt_a17
0xfd46, //Indoor_aCtmpWgt_a18
0x0e00, // burst end

0x03c9, //c9 page
0x0e01, // burst start

0x1011, //Indoor_aCtmpWgt_a19
0x1111, //Indoor_aCtmpWgt_a20
0x1223, //Indoor_aCtmpWgt_a21
0x1311, //Indoor_aCtmpWgt_a22
0x1411, //Indoor_aCtmpWgt_a23
0x1510, //Indoor_aCtmpWgt_a24

0x1611,//Indoor_aYlvlWgt
0x1711,//Indoor_aYlvlWgt_a01
0x1811,//Indoor_aYlvlWgt_a02
0x1911,//Indoor_aYlvlWgt_a03
0x1a11,//Indoor_aYlvlWgt_a04
0x1b11,//Indoor_aYlvlWgt_a05
0x1c11,//Indoor_aYlvlWgt_a06
0x1d11,//Indoor_aYlvlWgt_a07
0x1e11,//Indoor_aYlvlWgt_a08
0x1f11,//Indoor_aYlvlWgt_a09
0x2011,//Indoor_aYlvlWgt_a10
0x2122,//Indoor_aYlvlWgt_a11
0x2222,//Indoor_aYlvlWgt_a12
0x2334,//Indoor_aYlvlWgt_a13
0x2432,//Indoor_aYlvlWgt_a14
0x2521,//Indoor_aYlvlWgt_a15

0x2633, //Indoor_aTgtAngle
0x273f, //Indoor_aTgtAngle_a01
0x2843, //Indoor_aTgtAngle_a02
0x2950,//Indoor_aTgtAngle_a03
0x2a68,//Indoor_aTgtAngle_a04
0x2b28,//Indoor_aRgTgtOfs
0x2c14,//Indoor_aRgTgtOfs_a01
0x2d02,//Indoor_aRgTgtOfs_a02
0x2e00,//Indoor_aRgTgtOfs_a03
0x2f08,//Indoor_aRgTgtOfs_a04
0x30d4,//Indoor_aBgTgtOfs
0x31ca,//Indoor_aBgTgtOfs_a01
0x32aa,//Indoor_aBgTgtOfs_a02
0x33a4,//Indoor_aBgTgtOfs_a03
0x3488,//Indoor_aBgTgtOfs_a04

0x3500, //bRgDefTgt //indoor
0x3600, //bBgDefTgt //indoor

0x3720,//Indoor_aWhtPtTrcAglOfs
0x381e,//Indoor_aWhtPtTrcAglOfs_a01
0x391c,//Indoor_aWhtPtTrcAglOfs_a02
0x3a1a,//Indoor_aWhtPtTrcAglOfs_a03
0x3b18,//Indoor_aWhtPtTrcAglOfs_a04
0x3c16,//Indoor_aWhtPtTrcAglOfs_a05
0x3d14,//Indoor_aWhtPtTrcAglOfs_a06
0x3e14,//Indoor_aWhtPtTrcAglOfs_a07
0x3f13,//Indoor_aWhtPtTrcAglOfs_a08
0x4012,//Indoor_aWhtPtTrcAglOfs_a09
0x4104,//Indoor_bWhtPtTrcCnt
0x4214,//Indoor_aRtoDiffThNrBp
0x433c,//Indoor_aRtoDiffThNrBp_a01
0x4428,//Indoor_aAglDiffThTrWhtPt
0x4550,//Indoor_aAglDiffThTrWhtPt_a01
0x46aa,//Indoor_bWgtRatioTh1
0x47a0,//Indoor_bWgtRatioTh2
0x4844,//Indoor_bWgtOfsTh1
0x4940,//Indoor_bWgtOfsTh2
0x4a5a,//Indoor_bWhtPtCorAglMin
0x4b70,//Indoor_bWhtPtCorAglMax
0x4c04,//Indoor_bYlvlMin
0x4df8,//Indoor_bYlvlMax
0x4e28,//Indoor_bPxlWgtLmtLoTh
0x4f78,//Indoor_bPxlWgtLmtHiTh
0x5000,//Indoor_SplBldWgt_1
0x5100,//Indoor_SplBldWgt_2
0x5264,//Indoor_SplBldWgt_3
0x5360,//Indoor_TgtOff_StdHiTh
0x5430,//Indoor_TgtOff_StdLoTh
0x5505,//Indoor_wInitRg
0x56d0,//Indoor_wInitRg_n01
0x5706,//Indoor_wInitBg
0x5840,//Indoor_wInitBg_n01

0x5902, //Indoor_aRatioBox
0x5aee, //Indoor_aRatioBox_a01
0x5b06, //Indoor_aRatioBox_a02
0x5c40, //Indoor_aRatioBox_a03
0x5d08, //Indoor_aRatioBox_a04
0x5e34, //Indoor_aRatioBox_a05
0x5f0b,//Indoor_aRatioBox_a06
0x6054,//Indoor_aRatioBox_a07
0x6103, //Indoor_aRatioBox_a08
0x6252, //Indoor_aRatioBox_a09
0x6307, //Indoor_aRatioBox_a10
0x64d0, //Indoor_aRatioBox_a11
0x6506, //Indoor_aRatioBox_a12
0x66a4, //Indoor_aRatioBox_a13
0x6708, //Indoor_aRatioBox_a14
0x68fc, //Indoor_aRatioBox_a15
0x6903, //Indoor_aRatioBox_a16
0x6ae8, //Indoor_aRatioBox_a17
0x6b0a, //Indoor_aRatioBox_a18
0x6c8c, //Indoor_aRatioBox_a19
0x6d04, //Indoor_aRatioBox_a20
0x6eb0, //Indoor_aRatioBox_a21
0x6f07, //Indoor_aRatioBox_a22
0x706c, //Indoor_aRatioBox_a23
0x7104, //Indoor_aRatioBox_a24
0x72e2, //Indoor_aRatioBox_a25
0x730c, //Indoor_aRatioBox_a26
0x741c, //Indoor_aRatioBox_a27
0x7503, //Indoor_aRatioBox_a28
0x7684, //Indoor_aRatioBox_a29
0x7705, //Indoor_aRatioBox_a30
0x78dc, //Indoor_aRatioBox_a31
0x7905, //Indoor_aRatioBox_a32
0x7adc, //Indoor_aRatioBox_a33
0x7b0c, //Indoor_aRatioBox_a34
0x7ce4, //Indoor_aRatioBox_a35
0x7d01, //Indoor_aRatioBox_a36
0x7ef4, //Indoor_aRatioBox_a37
0x7f05, //Indoor_aRatioBox_a38
0x8000, //Indoor_aRatioBox_a39

0x8100, //Outdoor_wRgIntOfs
0x824a,//Outdoor_wRgIntOfs_n01
0x8301, //Outdoor_wBgIntOfs
0x8400, //Outdoor_wBgIntOfs_n01
0x8510, //Outdoor_bRgStep
0x8610, //Outdoor_bBgStep
0x8751, //Outdoor_aTgtWhtRgnBg
0x8852, //Outdoor_aTgtWhtRgnBg_a01
0x8953, //Outdoor_aTgtWhtRgnBg_a02
0x8a57, //Outdoor_aTgtWhtRgnBg_a03
0x8b5e, //Outdoor_aTgtWhtRgnBg_a04
0x8c64, //Outdoor_aTgtWhtRgnBg_a05
0x8d6A, //Outdoor_aTgtWhtRgnBg_a06
0x8e6F, //Outdoor_aTgtWhtRgnBg_a07
0x8f75, //Outdoor_aTgtWhtRgnBg_a08
0x907c, //Outdoor_aTgtWhtRgnBg_a09
0x9184, //Outdoor_aTgtWhtRgnBg_a10
0x925D, //Outdoor_aTgtWhtRgnRgLtLmt
0x9357, //Outdoor_aTgtWhtRgnRgLtLmt_a01
0x9451, //Outdoor_aTgtWhtRgnRgLtLmt_a02
0x9550, //Outdoor_aTgtWhtRgnRgLtLmt_a03
0x964e,//Outdoor_aTgtWhtRgnRgLtLmt_a04
0x974c,//Outdoor_aTgtWhtRgnRgLtLmt_a05
0x984b,//Outdoor_aTgtWhtRgnRgLtLmt_a06
0x9949,//Outdoor_aTgtWhtRgnRgLtLmt_a07
0x9a47, //Outdoor_aTgtWhtRgnRgLtLmt_a08
0x9b46,//Outdoor_aTgtWhtRgnRgLtLmt_a09
0x9c45,//Outdoor_aTgtWhtRgnRgLtLmt_a10
0x9d64, //Outdoor_aTgtWhtRgnRgRtLmt
0x9e63, //Outdoor_aTgtWhtRgnRgRtLmt_a01
0x9f62, //Outdoor_aTgtWhtRgnRgRtLmt_a02
0xa062, //Outdoor_aTgtWhtRgnRgRtLmt_a03
0xa161, //Outdoor_aTgtWhtRgnRgRtLmt_a04
0xa260, //Outdoor_aTgtWhtRgnRgRtLmt_a05
0xa35e, //Outdoor_aTgtWhtRgnRgRtLmt_a06
0xa45d,//Outdoor_aTgtWhtRgnRgRtLmt_a07
0xa55c,//Outdoor_aTgtWhtRgnRgRtLmt_a08
0xa65a, //Outdoor_aTgtWhtRgnRgRtLmt_a09
0xa757,//Outdoor_aTgtWhtRgnRgRtLmt_a10
0xa840, //Outdoor_aOptWhtRgnBg
0xa945, //Outdoor_aOptWhtRgnBg_a01
0xaa4b, //Outdoor_aOptWhtRgnBg_a02
0xab54, //Outdoor_aOptWhtRgnBg_a03
0xac60, //Outdoor_aOptWhtRgnBg_a04
0xad6c, //Outdoor_aOptWhtRgnBg_a05
0xae76, //Outdoor_aOptWhtRgnBg_a06
0xaf7f, //Outdoor_aOptWhtRgnBg_a07
0xb08c, //Outdoor_aOptWhtRgnBg_a08
0xb195, //Outdoor_aOptWhtRgnBg_a09
0xb2a0, //Outdoor_aOptWhtRgnBg_a10
0xb36a, //Outdoor_aOptWhtRgnRgLtLmt
0xb45b, //Outdoor_aOptWhtRgnRgLtLmt_a01
0xb553, //Outdoor_aOptWhtRgnRgLtLmt_a02
0xb64c, //Outdoor_aOptWhtRgnRgLtLmt_a03
0xb746, //Outdoor_aOptWhtRgnRgLtLmt_a04
0xb842, //Outdoor_aOptWhtRgnRgLtLmt_a05
0xb93e, //Outdoor_aOptWhtRgnRgLtLmt_a06
0xba3c, //Outdoor_aOptWhtRgnRgLtLmt_a07
0xbb3a, //Outdoor_aOptWhtRgnRgLtLmt_a08
0xbc39, //Outdoor_aOptWhtRgnRgLtLmt_a09
0xbd37, //Outdoor_aOptWhtRgnRgLtLmt_a10
0xbe7d, //Outdoor_aOptWhtRgnRgRtLmt
0xbf7c, //Outdoor_aOptWhtRgnRgRtLmt_a01
0xc079, //Outdoor_aOptWhtRgnRgRtLmt_a02
0xc176, //Outdoor_aOptWhtRgnRgRtLmt_a03
0xc26f, //Outdoor_aOptWhtRgnRgRtLmt_a04
0xc36a, //Outdoor_aOptWhtRgnRgRtLmt_a05
0xc466, //Outdoor_aOptWhtRgnRgRtLmt_a06
0xc563, //Outdoor_aOptWhtRgnRgRtLmt_a07
0xc65B, //Outdoor_aOptWhtRgnRgRtLmt_a08
0xc754, //Outdoor_aOptWhtRgnRgRtLmt_a09
0xc84a, //Outdoor_aOptWhtRgnRgRtLmt_a10

0xc942, //Outdoor_aCtmpWgtWdhTh
0xca4c,//Outdoor_aCtmpWgtWdhTh_a01
0xcb54,//Outdoor_aCtmpWgtWdhTh_a02
0xcc5c,//Outdoor_aCtmpWgtWdhTh_a03
0xcd64,//Outdoor_aCtmpWgtWdhTh_a04
0xce6c,//Outdoor_aCtmpWgtWdhTh_a05
0xcf74,//Outdoor_aCtmpWgtWdhTh_a06
0xd042, //Outdoor_aCtmpWgtHgtTh
0xd152, //Outdoor_aCtmpWgtHgtTh_a01
0xd258, //Outdoor_aCtmpWgtHgtTh_a02
0xd35e, //Outdoor_aCtmpWgtHgtTh_a03
0xd464, //Outdoor_aCtmpWgtHgtTh_a04
0xd56a, //Outdoor_aCtmpWgtHgtTh_a05
0xd672, //Outdoor_aCtmpWgtHgtTh_a06
0xd77a, //Outdoor_aCtmpWgtHgtTh_a07
0xd888, //Outdoor_aCtmpWgtHgtTh_a08
0xd911, //Outdoor_aCtmpWgt
0xda23,//Outdoor_aCtmpWgt_a01
0xdb22,//Outdoor_aCtmpWgt_a02
0xdc11, //Outdoor_aCtmpWgt_a03
0xdd22,//Outdoor_aCtmpWgt_a04
0xde22, //Outdoor_aCtmpWgt_a05
0xdf11, //Outdoor_aCtmpWgt_a06
0xe033, //Outdoor_aCtmpWgt_a07
0xe131,//Outdoor_aCtmpWgt_a08
0xe212, //Outdoor_aCtmpWgt_a09
0xe366,//Outdoor_aCtmpWgt_a10
0xe441,//Outdoor_aCtmpWgt_a11
0xe513, //Outdoor_aCtmpWgt_a12
0xe677,//Outdoor_aCtmpWgt_a13
0xe741,//Outdoor_aCtmpWgt_a14
0xe813,//Outdoor_aCtmpWgt_a15
0xe974, //Outdoor_aCtmpWgt_a16
0xea11, //Outdoor_aCtmpWgt_a17
0xeb23,//Outdoor_aCtmpWgt_a18
0xec53,//Outdoor_aCtmpWgt_a19
0xed11, //Outdoor_aCtmpWgt_a20
0xee43,//Outdoor_aCtmpWgt_a21
0xef31,//Outdoor_aCtmpWgt_a22
0xf011, //Outdoor_aCtmpWgt_a23
0xf111, //Outdoor_aCtmpWgt_a24

0xf212, //aYlvlWgt
0xf334, //aYlvlWgt_a01
0xf443, //aYlvlWgt_a02
0xf532, //aYlvlWgt_a03
0xf611,//aYlvlWgt_a04
0xf711, //aYlvlWgt_a05
0xf811, //aYlvlWgt_a06
0xf911, //aYlvlWgt_a07
0xfa11, //aYlvlWgt_a08
0xfb11, //aYlvlWgt_a09
0xfc11, //aYlvlWgt_a10
0xfd11, //aYlvlWgt_a11
0x0e00, // burst end

//Page ca
0x03ca,
0x0e01, // burst start

0x1011, //aYlvlWgt_a12
0x1122, //aYlvlWgt_a13
0x1222, //aYlvlWgt_a14
0x1311, //aYlvlWgt_a15

0x1464, //Outdoor_aTgtAngle
0x156b, //Outdoor_aTgtAngle_a01
0x1670, //Outdoor_aTgtAngle_a02
0x177a,//Outdoor_aTgtAngle_a03
0x1884,//Outdoor_aTgtAngle_a04
0x191a,//Outdoor_aRgTgtOfs
0x1a10,//Outdoor_aRgTgtOfs_a01
0x1b08,//Outdoor_aRgTgtOfs_a02
0x1c04,//Outdoor_aRgTgtOfs_a03
0x1d04,//Outdoor_aRgTgtOfs_a04
0x1e9e,//Outdoor_aBgTgtOfs
0x1f88, //Outdoor_aBgTgtOfs_a01
0x2084,//Outdoor_aBgTgtOfs_a02
0x2182,//Outdoor_aBgTgtOfs_a03
0x2282,//Outdoor_aBgTgtOfs_a04
0x2382,//Outdoor_bRgDefTgt
0x2488, //Outdoor_bBgDefTgt

0x251c, //Outdoor_aWhtPtTrcAglOfs
0x261a, //Outdoor_aWhtPtTrcAglOfs_a01
0x2718, //Outdoor_aWhtPtTrcAglOfs_a02
0x2816, //Outdoor_aWhtPtTrcAglOfs_a03
0x2914, //Outdoor_aWhtPtTrcAglOfs_a04
0x2a12, //Outdoor_aWhtPtTrcAglOfs_a05
0x2b10, //Outdoor_aWhtPtTrcAglOfs_a06
0x2c0f, //Outdoor_aWhtPtTrcAglOfs_a07
0x2d0e, //Outdoor_aWhtPtTrcAglOfs_a08
0x2e0e, //Outdoor_aWhtPtTrcAglOfs_a09
0x2f0a, //Outdoor_bWhtPtTrcCnt
0x3028, //Outdoor_aRtoDiffThNrBp
0x3148, //Outdoor_aRtoDiffThNrBp_a01
0x3228, //Outdoor_aAglDiffThTrWhtPt
0x3350, //Outdoor_aAglDiffThTrWhtPt_a01
0x34aa, //Outdoor_bWgtRatioTh1
0x35a0, //Outdoor_bWgtRatioTh2
0x360a, //Outdoor_bWgtOfsTh1
0x37a0, //Outdoor_bWgtOfsTh2
0x386d, //Outdoor_bWhtPtCorAglMin
0x3978, //Outdoor_bWhtPtCorAglMax
0x3a04, //Outdoor_bYlvlMin
0x3bf8, //Outdoor_bYlvlMax
0x3c28, //Outdoor_bPxlWgtLmtLoTh
0x3d78, //Outdoor_bPxlWgtLmtHiTh
0x3e00, //Outdoor_SplBldWgt_1
0x3f00, //Outdoor_SplBldWgt_2
0x4064, //Outdoor_SplBldWgt_3
0x4160, //Outdoor_TgtOff_StdHiTh
0x4230, //Outdoor_TgtOff_StdLoTh
0x4304,
0x44c0,
0x4507,
0x46c0,
0x4702, //Outdoor_aRatioBox
0x48b2, //Outdoor_aRatioBox_a01
0x4905, //Outdoor_aRatioBox_a02
0x4adc, //Outdoor_aRatioBox_a03
0x4b0a, //Outdoor_aRatioBox_a04
0x4c28, //Outdoor_aRatioBox_a05
0x4d0c, //Outdoor_aRatioBox_a06
0x4e1c, //Outdoor_aRatioBox_a07
0x4f02, //Outdoor_aRatioBox_a08
0x50ee, //Outdoor_aRatioBox_a09
0x5106, //Outdoor_aRatioBox_a10
0x5272, //Outdoor_aRatioBox_a11
0x5308, //Outdoor_aRatioBox_a12
0x5498, //Outdoor_aRatioBox_a13
0x550a, //Outdoor_aRatioBox_a14
0x56f0, //Outdoor_aRatioBox_a15
0x5703, //Outdoor_aRatioBox_a16
0x5820, //Outdoor_aRatioBox_a17
0x5907, //Outdoor_aRatioBox_a18
0x5a08, //Outdoor_aRatioBox_a19
0x5b07, //Outdoor_aRatioBox_a20
0x5c6c, //Outdoor_aRatioBox_a21
0x5d09, //Outdoor_aRatioBox_a22
0x5e60, //Outdoor_aRatioBox_a23
0x5f03, //Outdoor_aRatioBox_a24
0x6084, //Outdoor_aRatioBox_a25
0x6107, //Outdoor_aRatioBox_a26
0x62d0, //Outdoor_aRatioBox_a27
0x6306, //Outdoor_aRatioBox_a28
0x6440, //Outdoor_aRatioBox_a29
0x6508, //Outdoor_aRatioBox_a30
0x6634, //Outdoor_aRatioBox_a31
0x6703, //Outdoor_aRatioBox_a32
0x68e8, //Outdoor_aRatioBox_a33
0x6908, //Outdoor_aRatioBox_a34
0x6ad0, //Outdoor_aRatioBox_a35
0x6b04, //Outdoor_aRatioBox_a36
0x6c4c, //Outdoor_aRatioBox_a37
0x6d07, //Outdoor_aRatioBox_a38
0x6e08, //Outdoor_aRatioBox_a39

0x6f04,
0x7000,

0x7105, //Out2_Adt_RgainMin
0x7200, //Out2_Adt_RgainMin_n01
0x7305, //Out2_Adt_RgainMax
0x74c0,//Out2_Adt_RgainMax_n01
0x7504, //Out2_Adt_GgainMin
0x7600, //Out2_Adt_GgainMin_n01
0x7704, //Out2_Adt_GgainMax
0x7800, //Out2_Adt_GgainMax_n01
0x7905, //Out2_Adt_BgainMin
0x7ae0, //Out2_Adt_BgainMin_n01
0x7b06, //Out2_Adt_BgainMax
0x7ca0, //Out2_Adt_BgainMax_n01

0x7d05, //Out1_Adt_RgainMin
0x7e40,//Out1_Adt_RgainMin_n01
0x7f06, //Out1_Adt_RgainMax
0x8040,//Out1_Adt_RgainMax_n01
0x8104, //Out1_Adt_GgainMin
0x8200, //Out1_Adt_GgainMin_n01
0x8304, //Out1_Adt_GgainMax
0x8400, //Out1_Adt_GgainMax_n01
0x8505, //Out1_Adt_BgainMin
0x8680, //Out1_Adt_BgainMin_n01
0x8707, //Out1_Adt_BgainMax
0x8800, //Out1_Adt_BgainMax_n01

0x8904, //In_Adt_RgainMin
0x8a00, //In_Adt_RgainMin_n01
0x8b0d, //In_Adt_RgainMax
0x8c00, //In_Adt_RgainMax_n01
0x8d04, //In_Adt_GgainMin
0x8e00, //In_Adt_GgainMin_n01
0x8f05, //In_Adt_GgainMax
0x9080, //In_Adt_GgainMax_n01
0x9104, //In_Adt_BgainMin
0x9200, //In_Adt_BgainMin_n01
0x930d, //In_Adt_BgainMax
0x9480, //In_Adt_BgainMax_n01

0x9504, //Manual_Adt_RgainMin
0x9600, //Manual_Adt_RgainMin_n01
0x970d, //Manual_Adt_RgainMax
0x9800, //Manual_Adt_RgainMax_n01
0x9904, //Manual_Adt_GgainMin
0x9a00, //Manual_Adt_GgainMin_n01
0x9b04, //Manual_Adt_GgainMax
0x9c80, //Manual_Adt_GgainMax_n01
0x9d04, //Manual_Adt_BgainMin
0x9e00, //Manual_Adt_BgainMin_n01
0x9f0b, //Manual_Adt_BgainMax
0xa000, //Manual_Adt_BgainMax_n01

0x0e00, //burst end

0x03c8,
0x1700, //AWB Speed
0x1811,
0x2100,
0x2201,
0x11C3,	//AWB reset


/////////////////////////////////////////////////////////////////////////////////
// CD page(OTP control)
/////////////////////////////////////////////////////////////////////////////////
0x03CD,
0x1003,

0x2210,
//Manual Typical colo ratio write
0x271A, //Typical RG=0.685*1000 = 6850 = 1AC2
0x28C2,
0x2910, //Typical BG=0.430*1000 = 4300 = 10CC
0x2ACC,
0x2b0a,//+/-10 valid ratio check

///////////////////////////////////////////////////////////////////////////////
// Color ratio setting
/////////////////////////////////////////////////////////////////////////////////
0x03CE,
0x3098,	//Color ratio on
0x3101,
0x3304,	//R gain def
0x3400,
0x3504,	//G gain def
0x3600,
0x3704,	//B gain def
0x3800,

0x4500, //Outdoor In EvTh
0x4600,
0x4727,
0x4810,
0x4900, //Outdoor Out EvTh
0x4a00,
0x4b4e,
0x4c20,

0x553e, //Low In Th
0x564c, //Low Out Th
0x575c, //High Out Th
0x586c, //High In Th

0x6288, //Out weight
0x6386, //Indoor weight
0x6488, //Dark weight
0x65d8, //Low weight
0x6686, //High weight

///////////////////////////////////////////////////////////////////////////////
// D3 ~ D8 Page (Adaptive)
///////////////////////////////////////////////////////////////////////////////

0x03d3,	// Adaptive start

0x0e01, // burst start

0x1000,
0x1100,
0x1200,
0x1300,
0x1400,
0x1500,
0x1600,
0x1700,
0x1800,
0x1900,

0x1a00,	// Def_Yoffset
0x1b32,	// DYOFS_Ratio
0x1c00,	// DYOFS_Limit //10

0x1d00,	//EV Th OutEnd : 120fps AG 1x DG 1x
0x1e00,
0x1f20,
0x208d,

0x2100,	//EV Th OutStr : 80fps  Ag 1x Dg 1x
0x2200,
0x2330,
0x24d4,

0x2500,	//EV Th Dark1Str
0x2603,
0x2770,
0x28f0,

0x2900,	//EV Th Dark1End
0x2a05,
0x2b57,
0x2c30,

0x2d00,	//EV Th Dark2Str
0x2e06,
0x2f1a,
0x3080,

0x3100,	//EV Th Dark2End
0x3208,
0x3386,
0x34b6,

0x354b, //Ctmp LT End
0x3652, //Ctmp LT Str
0x3769,	//Ctmp HT Str
0x3873,	//Ctmp HT End

0x3900,// LSC_EvTh_OutEnd_4
0x3a00,// LSC_EvTh_OutEnd_3
0x3b13,// LSC_EvTh_OutEnd_2
0x3c88,// LSC_EvTh_OutEnd_1    def : 200fps  Ag 1x Dg 1x

0x3d00,// LSC_EvTh_OutStr_4
0x3e00,// LSC_EvTh_OutStr_3
0x3f30,// LSC_EvTh_OutStr_2
0x40d4,// LSC_EvTh_OutStr_1    def :  80fps  Ag 1x Dg 1x

0x4100,// LSC_EvTh_Dark1Str_4
0x4205,// LSC_EvTh_Dark1Str_3
0x43b8,// LSC_EvTh_Dark1Str_2
0x44d8,// LSC_EvTh_Dark1Str_1  def :  8fps  Ag 3x Dg 1x

0x4500,// LSC_EvTh_Dark1End_4
0x460b,// LSC_EvTh_Dark1End_3
0x4771,// LSC_EvTh_Dark1End_2
0x48b0,// LSC_EvTh_Dark1End_1  def :  8fps  Ag 6x Dg 1x

0x4900,// LSC_EvTh_Dark2Str_4
0x4a0f,// LSC_EvTh_Dark2Str_3
0x4b42,// LSC_EvTh_Dark2Str_2
0x4c40,// LSC_EvTh_Dark2Str_1  def :  8fps  Ag 8x Dg 1x

0x4d00,// LSC_EvTh_Dark2End_4
0x4e1e,// LSC_EvTh_Dark2End_3
0x4f84,// LSC_EvTh_Dark2End_2
0x5080,// LSC_EvTh_Dark2End_1  def :  4fps  Ag 8x Dg 1x

0x5155,//LSC Ctmp LTEnd Out
0x5264,	//LSC Ctmp LTStr Out
0x5378,//LSC Ctmp HTStr Out
0x5486,//LSC Ctmp HTEnd Out

0x5546,	//LSC Ctmp LTEnd In
0x5656,	//LSC Ctmp LTStr In
0x576e,	//LSC Ctmp HTStr In
0x5876,	//LSC Ctmp HTEnd In

0x5950,	// LSC_CTmpTh_LT_End_Dark
0x5a78,	// LSC_CTmpTh_LT_Str_Dark
0x5ba0,	// LSC_CTmpTh_HT_Str_Dark
0x5cb4,	// LSC_CTmpTh_HT_End_Dark

0x5d00,	// UniScn_EvMinTh_4
0x5e00,	// UniScn_EvMinTh_3
0x5f04,	// UniScn_EvMinTh_2
0x60e2,	// UniScn_EvMinTh_1    def : 600fps  Ag 1x Dg 1x

0x6100,	// UniScn_EvMaxTh_4
0x6205,	// UniScn_EvMaxTh_3
0x63b8,	// UniScn_EvMaxTh_2
0x64d8,	// UniScn_EvMaxTh_1     def :  8fps  Ag 3x Dg 1x

0x654e,	// UniScn_AglMinTh_1
0x6650,	// UniScn_AglMinTh_2
0x6773,	// UniScn_AglMaxTh_1
0x687d,	// UniScn_AglMaxTh_2

0x6903,	// UniScn_YstdMinTh
0x6a0a,// UniScn_YstdMaxTh
0x6b1e,	// UniScn_BPstdMinTh
0x6c34,// UniScn_BPstdMaxTh

0x6d64,	// Ytgt_ColWgt_Out
0x6e64,	// Ytgt_ColWgt_Dark
0x6f64,	// ColSat_ColWgt_Out
0x7064,	// ColSat_ColWgt_Dark
0x7164,	// CMC_ColWgt_Out
0x7264,	// CMC_ColWgt_Dark
0x7364,	// MCMC_ColWgt_Out
0x7464,	// MCMC_ColWgt_Dark
0x7564,	// CustomReg_CorWgt_Out
0x7664,	// CustomReg_CorWgt_Dark

0x7764,	// UniScn_Y_Ratio
0x7850,	// UniScn_Cb_Ratio
0x7950,	// UniScn_Cr_Ratio

0x7a00,	// Ytgt_offset
0x7b00,	// CbSat_offset
0x7c00,	// CrSat_offset

0x7d36,	// Y_target_Outdoor
0x7e3c,	// Y_target_Indoor
0x7f3c,	// Y_target_Dark1
0x803c,	// Y_target_Dark2
0x813c,	// Y_target_LowTemp
0x823c,	// Y_target_HighTemp

0x8380, // Cb_Outdoor
0x8495,	// Cb _Sat_Indoor
0x85a0,	// Cb _Sat_Dark1
0x8684,	// Cb _Sat_Dark2
0x8788,	// Cb _Sat_LowTemp
0x8892,	// Cb _Sat_HighTemp

0x8980,	// Cr _Sat_Outdoor
0x8a90,	// Cr _Sat_Indoor
0x8ba0,	// Cr _Sat_Dark1
0x8c80,	// Cr _Sat_Dark2
0x8d75,	// Cr _Sat_LowTemp
0x8e92,	// Cr _Sat_HighTemp

0x8f82,	// BLC_ofs_r_Outdoor
0x9081,	// BLC_ofs_b_Outdoor
0x9182,	// BLC_ofs_gr_Outdoor
0x9282,	// BLC_ofs_gb_Outdoor

0x9381,	// BLC_ofs_r_Indoor
0x9480,	// BLC_ofs_b_Indoor
0x9581,	// BLC_ofs_gr_Indoor
0x9681,	// BLC_ofs_gb_Indoor

0x9784,	// BLC_ofs_r_Dark1
0x9884,	// BLC_ofs_b_Dark1
0x9985,	// BLC_ofs_gr_Dark1
0x9a85,	// BLC_ofs_gb_Dark1

0x9b84,	// BLC_ofs_r_Dark2
0x9c84,	// BLC_ofs_b_Dark2
0x9d85,	// BLC_ofs_gr_Dark2
0x9e85,	// BLC_ofs_gb_Dark2

0x9f00,	//LSC Out_L ofs G
0xa000,	//LSC Out_L ofs B
0xa100,	//LSC Out_L ofs R
0xa280,	//LSC Out_L Gain G
0xa382,	//LSC Out_L Gain B
0xa484,//LSC Out_L Gain R

0xa500,	//LSC Out_M ofs G
0xa600,	//LSC Out_M ofs B
0xa700,	//LSC Out_M ofs R
0xa880,	//LSC Out_M Gain G
0xa982,//LSC Out_M Gain B
0xaa7e,	//LSC Out_M Gain R

0xab00,	//LSC Out_H ofs G
0xac00,	//LSC Out_H ofs B
0xad00,	//LSC Out_H ofs R
0xae80,	//LSC Out_H Gain G
0xaf84,//LSC Out_H Gain B
0xb078,//LSC Out_H Gain R

0xb100,	// LSC0_Ind_LowTmp        offset g
0xb200,	// LSC1_Ind_LowTmp        offset b
0xb300,	// LSC2_Ind_LowTmp        offset r
0xb480,	// LSC3_Ind_LowTmp        gain g
0xb580,	// LSC4_Ind_LowTmp        gain b
0xb688,	// LSC5_Ind_LowTmp        gain r

0xb700,	// LSC0_Ind_MiddleTmp     offset g
0xb800,	// LSC1_Ind_MiddleTmp     offset b
0xb900,	// LSC2_Ind_MiddleTmp     offset r
0xba80,	// LSC3_Ind_MiddleTmp     gain g
0xbb80,	// LSC4_Ind_MiddleTmp     gain b
0xbc7e,	// LSC5_Ind_MiddleTmp     gain r

0xbd00,	// LSC0_Ind_HighTmp       offset g
0xbe00,	// LSC1_Ind_HighTmp       offset b
0xbf00,	// LSC2_Ind_HighTmp       offset r
0xc080,	// LSC3_Ind_HighTmp       gain g
0xc180,	// LSC4_Ind_HighTmp       gain b
0xc27e,// LSC5_Ind_HighTmp       gain r

0xc300,	// LSC0_Dark1_LowTmp      offset g
0xc400,	// LSC1_Dark1_LowTmp      offset b
0xc500,	// LSC2_Dark1_LowTmp      offset r
0xc668,	// LSC3_Dark1_LowTmp      gain g
0xc768,	// LSC4_Dark1_LowTmp      gain b
0xc868,	// LSC5_Dark1_LowTmp      gain r

0xc900,	// LSC0_Dark1_MiddleTmp   offset g
0xca00,	// LSC1_Dark1_MiddleTmp   offset b
0xcb00,	// LSC2_Dark1_MiddleTmp   offset r
0xcc68,	// LSC3_Dark1_MiddleTmp   gain g
0xcd68,	// LSC4_Dark1_MiddleTmp   gain b
0xce68,	// LSC5_Dark1_MiddleTmp   gain r

0xcf00,	// LSC0_Dark1_HighTmp   offset g
0xd000,	// LSC1_Dark1_HighTmp   offset b
0xd100,	// LSC2_Dark1_HighTmp   offset r
0xd268,	// LSC3_Dark1_HighTmp   gain g
0xd368,	// LSC4_Dark1_HighTmp   gain b
0xd468,	// LSC5_Dark1_HighTmp   gain r

0xd500,	// LSC0_Dark2           offset g
0xd600,	// LSC1_Dark2           offset b
0xd700,	// LSC2_Dark2           offset r
0xd868,	// LSC3_Dark2           gain g
0xd968,	// LSC4_Dark2           gain b
0xda68,	// LSC5_Dark2           gain r

0xdb2f, //CMCSIGN_Out
0xdc55, //CMC_Out_00
0xdd1c, //CMC_Out_01
0xde07, //CMC_Out_02
0xdf0a, //CMC_Out_03
0xe051, //CMC_Out_04
0xe107, //CMC_Out_05
0xe201, //CMC_Out_06
0xe314, //CMC_Out_07
0xe455, //CMC_Out_08

0xe504,	// CMC_Out_LumTh1      CMC SP gain axis X(luminance)
0xe60a,	// CMC_Out_LumTh2
0xe710,	// CMC_Out_LumTh3
0xe818,	// CMC_Out_LumTh4
0xe920,	// CMC_Out_LumTh5
0xea28,	// CMC_Out_LumTh6
0xeb40,	// CMC_Out_LumTh7

0xec20,	// CMC_Out_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0xed20,	// CMC_Out_LumGain2_R
0xee20,	// CMC_Out_LumGain3_R
0xef20,	// CMC_Out_LumGain4_R
0xf020,	// CMC_Out_LumGain5_R
0xf120,	// CMC_Out_LumGain6_R
0xf220,	// CMC_Out_LumGain7_R
0xf320,	// CMC_Out_LumGain8_R    20 = x1.0

0xf420,	// CMC_Out_LumGain1_G  CMC SP G gain axis Y (gain):: max32
0xf520,	// CMC_Out_LumGain2_G
0xf620,	// CMC_Out_LumGain3_G
0xf720,	// CMC_Out_LumGain4_G
0xf820,	// CMC_Out_LumGain5_G
0xf920,	// CMC_Out_LumGain6_G
0xfa20,	// CMC_Out_LumGain7_G
0xfb20,	// CMC_Out_LumGain8_G    20 = x1.0

0xfc20,	// CMC_Out_LumGain1_B  CMC SP B gain axis Y (gain):: max32
0xfd20,	// CMC_Out_LumGain2_B
0x0e00, // burst end

0x03d4,	// page D4
0x0e01, // burst start

0x1020,	// CMC_Out_LumGain3_B
0x1120,	// CMC_Out_LumGain4_B
0x1220,	// CMC_Out_LumGain5_B
0x1320,	// CMC_Out_LumGain6_B
0x1420,	// CMC_Out_LumGain7_B
0x1520,	// CMC_Out_LumGain8_B    20 = x1.0

0x162f, //CMCSIGN_In_Mid
0x1753, //CMC_In_Mid_00
0x1816, //CMC_In_Mid_01
0x1903, //CMC_In_Mid_02
0x1a10, //CMC_In_Mid_03
0x1b53, //CMC_In_Mid_04
0x1c03, //CMC_In_Mid_05
0x1d04, //CMC_In_Mid_06
0x1e1d, //CMC_In_Mid_07
0x1f61, //CMC_In_Mid_08

0x2004,	// CMC_Ind_LumTh1     CMC SP gain axis X(luminance)
0x210a,	// CMC_Ind_LumTh2
0x2210,	// CMC_Ind_LumTh3
0x2318,	// CMC_Ind_LumTh4
0x2420,	// CMC_Ind_LumTh5
0x2528,	// CMC_Ind_LumTh6
0x2640,	// CMC_Ind_LumTh7

0x2708,	// CMC_Ind_LumGain1_R   CMC SP R gain axis Y (gain):: max32
0x2812,	// CMC_Ind_LumGain2_R
0x2918,	// CMC_Ind_LumGain3_R
0x2a1c,	// CMC_Ind_LumGain4_R
0x2b1e,	// CMC_Ind_LumGain5_R
0x2c20,	// CMC_Ind_LumGain6_R
0x2d20,	// CMC_Ind_LumGain7_R
0x2e20,	// CMC_Ind_LumGain8_R    20 = x1.0

0x2f08,	// CMC_Ind_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x3012,	// CMC_Ind_LumGain2_G
0x3118,	// CMC_Ind_LumGain3_G
0x321c,	// CMC_Ind_LumGain4_G
0x331e,	// CMC_Ind_LumGain5_G
0x3420,	// CMC_Ind_LumGain6_G
0x3520,	// CMC_Ind_LumGain7_G
0x3620,	// CMC_Ind_LumGain8_G    20 = x1.0

0x3708,	// CMC_Ind_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x3812,	// CMC_Ind_LumGain2_B
0x3918,	// CMC_Ind_LumGain3_B
0x3a1c,	// CMC_Ind_LumGain4_B
0x3b1e,	// CMC_Ind_LumGain5_B
0x3c20,	// CMC_Ind_LumGain6_B
0x3d20,	// CMC_Ind_LumGain7_B
0x3e20,	// CMC_Ind_LumGain8_B   20 = x1.0

0x3f2f, //CMCSIGN_Dark1
0x4053, //CMC_Dark1_00
0x411c, //CMC_Dark1_01
0x4209, //CMC_Dark1_02
0x430e, //CMC_Dark1_03
0x4453, //CMC_Dark1_04
0x4505, //CMC_Dark1_05
0x4603, //CMC_Dark1_06
0x4723, //CMC_Dark1_07
0x4866, //CMC_Dark1_08

0x4904,	// CMC_Dark1_LumTh1     CMC SP gain axis X(luminance)
0x4a0a,	// CMC_Dark1_LumTh2
0x4b10,	// CMC_Dark1_LumTh3
0x4c18,	// CMC_Dark1_LumTh4
0x4d20,	// CMC_Dark1_LumTh5
0x4e28,	// CMC_Dark1_LumTh6
0x4f40,	// CMC_Dark1_LumTh7

0x5008,	// CMC_Dark1_LumGain1_R  CMC SP R gain axis Y (gain):: max32
0x5112,	// CMC_Dark1_LumGain2_R
0x5218,	// CMC_Dark1_LumGain3_R
0x531c,	// CMC_Dark1_LumGain4_R
0x541e,	// CMC_Dark1_LumGain5_R
0x5520,	// CMC_Dark1_LumGain6_R
0x5620,	// CMC_Dark1_LumGain7_R
0x5720,	// CMC_Dark1_LumGain8_R    20 = x1.0

0x5808,	// CMC_Dark1_LumGain1_G   CMC SP G gain axis Y (gain):: max32
0x5912,	// CMC_Dark1_LumGain2_G
0x5a18,	// CMC_Dark1_LumGain3_G
0x5b1c,	// CMC_Dark1_LumGain4_G
0x5c1e,	// CMC_Dark1_LumGain5_G
0x5d20,	// CMC_Dark1_LumGain6_G
0x5e20,	// CMC_Dark1_LumGain7_G
0x5f20,	// CMC_Dark1_LumGain8_G    20 = x1.0

0x6008,	// CMC_Dark1_LumGain1_B   CMC SP B gain axis Y (gain):: max32
0x6112,	// CMC_Dark1_LumGain2_B
0x6218,	// CMC_Dark1_LumGain3_B
0x631c,	// CMC_Dark1_LumGain4_B
0x641e,	// CMC_Dark1_LumGain5_B
0x6520,	// CMC_Dark1_LumGain6_B
0x6620,	// CMC_Dark1_LumGain7_B
0x6720,	// CMC_Dark1_LumGain8_B   20 = x1.0

0x682f, //CMCSIGN_Dark2
0x6953, //CMC_Dark2_00
0x6a1c, //CMC_Dark2_01
0x6b09, //CMC_Dark2_02
0x6c0e, //CMC_Dark2_03
0x6d53, //CMC_Dark2_04
0x6e05, //CMC_Dark2_05
0x6f03, //CMC_Dark2_06
0x7023, //CMC_Dark2_07
0x7166, //CMC_Dark2_08

0x7204,	// CMC_Dark2_LumTh1        CMC SP gain axis X(luminance)
0x730a,	// CMC_Dark2_LumTh2
0x7410,	// CMC_Dark2_LumTh3
0x7518,	// CMC_Dark2_LumTh4
0x7620,	// CMC_Dark2_LumTh5
0x7728,	// CMC_Dark2_LumTh6
0x7840,	// CMC_Dark2_LumTh7

0x7908,	// CMC_Dark2_LumGain1_R    CMC SP R gain
0x7a12,	// CMC_Dark2_LumGain2_R
0x7b18,	// CMC_Dark2_LumGain3_R
0x7c1c,	// CMC_Dark2_LumGain4_R
0x7d1e,	// CMC_Dark2_LumGain5_R
0x7e20,	// CMC_Dark2_LumGain6_R
0x7f20,	// CMC_Dark2_LumGain7_R
0x8020,	// CMC_Dark2_LumGain8_R    20 = x1.

0x8108,	// CMC_Dark2_LumGain1_G    CMC SP G gain
0x8212,	// CMC_Dark2_LumGain2_G
0x8318,	// CMC_Dark2_LumGain3_G
0x841c,	// CMC_Dark2_LumGain4_G
0x851e,	// CMC_Dark2_LumGain5_G
0x8620,	// CMC_Dark2_LumGain6_G
0x8720,	// CMC_Dark2_LumGain7_G
0x8820,	// CMC_Dark2_LumGain8_G    20 = x1.

0x8908,	// CMC_Dark2_LumGain1_B    CMC SP B gain
0x8a12,	// CMC_Dark2_LumGain2_B
0x8b18,	// CMC_Dark2_LumGain3_B
0x8c1c,	// CMC_Dark2_LumGain4_B
0x8d1e,	// CMC_Dark2_LumGain5_B
0x8e20,	// CMC_Dark2_LumGain6_B
0x8f20,	// CMC_Dark2_LumGain7_B
0x9020,	// CMC_Dark2_LumGain8_B    20 = x1.0

0x912f, // CMCSIGN_In_Low
0x9253, // CMC_In_Low_00
0x931e, //CMC_In_Low_01
0x940b, //CMC_In_Low_02
0x9518, //CMC_In_Low_03
0x9661, // CMC_In_Low_04
0x9709, //CMC_In_Low_05
0x9804, //CMC_In_Low_06
0x9914, //CMC_In_Low_07
0x9a58, // CMC_In_Low_08

0x9b04,	// CMC_LowTemp_LumTh1     CMC SP gain axis X(luminance)
0x9c0a,	// CMC_LowTemp_LumTh2
0x9d10,	// CMC_LowTemp_LumTh3
0x9e18,	// CMC_LowTemp_LumTh4
0x9f20,	// CMC_LowTemp_LumTh5
0xa028,	// CMC_LowTemp_LumTh6
0xa140,	// CMC_LowTemp_LumTh7

0xa220,	// CMC_LowTemp_LumGain1_R    CMC SP R gain
0xa320,	// CMC_LowTemp_LumGain2_R
0xa420,	// CMC_LowTemp_LumGain3_R
0xa520,	// CMC_LowTemp_LumGain4_R
0xa620,	// CMC_LowTemp_LumGain5_R
0xa720,	// CMC_LowTemp_LumGain6_R
0xa820,	// CMC_LowTemp_LumGain7_R
0xa920,	// CMC_LowTemp_LumGain8_R    20 = x1.0

0xaa20,	// CMC_LowTemp_LumGain1_G    CMC SP G gain
0xab20,	// CMC_LowTemp_LumGain2_G
0xac20,	// CMC_LowTemp_LumGain3_G
0xad20,	// CMC_LowTemp_LumGain4_G
0xae20,	// CMC_LowTemp_LumGain5_G
0xaf20,	// CMC_LowTemp_LumGain6_G
0xb020,	// CMC_LowTemp_LumGain7_G
0xb120,	// CMC_LowTemp_LumGain8_G    20 = x1.0

0xb220,	// CMC_LowTemp_LumGain1_B    CMC SP B gain
0xb320,	// CMC_LowTemp_LumGain2_B
0xb420,	// CMC_LowTemp_LumGain3_B
0xb520,	// CMC_LowTemp_LumGain4_B
0xb620,	// CMC_LowTemp_LumGain5_B
0xb720,	// CMC_LowTemp_LumGain6_B
0xb820,	// CMC_LowTemp_LumGain7_B
0xb920,	// CMC_LowTemp_LumGain8_B    20 = x1.0

0xba2d, //CMCSIGN_In_High
0xbb55, //CMC_In_High_00
0xbc21, //CMC_In_High_01
0xbd0c, //CMC_In_High_02
0xbe08, //CMC_In_High_03
0xbf55, //CMC_In_High_04
0xc00d, //CMC_In_High_05
0xc103, //CMC_In_High_06
0xc218, //CMC_In_High_07
0xc355, //CMC_In_High_08

0xc404,	// CMC_HighTemp_LumTh1       CMC SP gain axis X(luminance)
0xc50a,	// CMC_HighTemp_LumTh2
0xc610,	// CMC_HighTemp_LumTh3
0xc718,	// CMC_HighTemp_LumTh4
0xc820,	// CMC_HighTemp_LumTh5
0xc928,	// CMC_HighTemp_LumTh6
0xca40,	// CMC_HighTemp_LumTh7

0xcb20,	// CMC_HighTemp_LumGain1_R   CMC SP R gain
0xcc20,	// CMC_HighTemp_LumGain2_R
0xcd20,	// CMC_HighTemp_LumGain3_R
0xce20,	// CMC_HighTemp_LumGain4_R
0xcf20,	// CMC_HighTemp_LumGain5_R
0xd020,	// CMC_HighTemp_LumGain6_R
0xd120,	// CMC_HighTemp_LumGain7_R
0xd220,	// CMC_HighTemp_LumGain8_R    20 = x1.0

0xd320,	// CMC_HighTemp_LumGain1_G   CMC SP G gain
0xd420,	// CMC_HighTemp_LumGain2_G
0xd520,	// CMC_HighTemp_LumGain3_G
0xd620,	// CMC_HighTemp_LumGain4_G
0xd720,	// CMC_HighTemp_LumGain5_G
0xd820,	// CMC_HighTemp_LumGain6_G
0xd920,	// CMC_HighTemp_LumGain7_G
0xda20,	// CMC_HighTemp_LumGain8_G    20 = x1.

0xdb20,	// CMC_HighTemp_LumGain1_B   CMC SP B gain
0xdc20,	// CMC_HighTemp_LumGain2_B
0xdd20,	// CMC_HighTemp_LumGain3_B
0xde20,	// CMC_HighTemp_LumGain4_B
0xdf20,	// CMC_HighTemp_LumGain5_B
0xe020,	// CMC_HighTemp_LumGain6_B
0xe120,	// CMC_HighTemp_LumGain7_B
0xe220,	// CMC_HighTemp_LumGain8_B   20 = x1.0

////////////////////
// Adaptive Gamma //
////////////////////

0xe300,	//GMA_OUT
0xe400,
0xe503,
0xe60b,
0xe715,
0xe821,
0xe92f,
0xea3c,
0xeb49,
0xec54,
0xed5e,
0xee68,
0xef72,
0xf079,
0xf17f,
0xf284,
0xf38a,
0xf48e,
0xf594,
0xf698,
0xf79d,
0xf8a7,
0xf9b2,
0xfabb,
0xfbcb,
0xfcda,
0xfde9,
0x0e00, // burst end

0x03d5,	//Page d5

0x0e01, // burst start

0x10f7,
0x11ff,
0x12ff,
0x13ff,
0x14ff,
0x15ff,
0x16ff,

0x1700,	//GMA_IN
0x1805,
0x1909,
0x1a11,
0x1b19,
0x1c26,
0x1d33,
0x1e3f,
0x1f4a,
0x2055,
0x215e,
0x2267,
0x236f,
0x2477,
0x257e,
0x2684,
0x278b,
0x2891,
0x2996,
0x2a9b,
0x2ba0,
0x2ca8,
0x2db2,
0x2ebc,
0x2fcd,
0x30dd,
0x31ec,
0x32f9,
0x33ff,
0x34ff,
0x35ff,
0x36ff,
0x37ff,
0x38ff,

0x3900,	//GMA_D1
0x3a04,
0x3b08,
0x3c0f,
0x3d1b,
0x3e2e,
0x3f3e,
0x404e,
0x415b,
0x4265,
0x4371,
0x447b,
0x4585,
0x468d,
0x4795,
0x489b,
0x49a3,
0x4aa9,
0x4bb0,
0x4cb6,
0x4dbb,
0x4ec5,
0x4fcf,
0x50d7,
0x51e7,
0x52f3,
0x53fa,
0x54ff,
0x55ff,
0x56ff,
0x57ff,
0x58ff,
0x59ff,
0x5aff,

0x5b00,	//GMA_D2
0x5c04,
0x5d08,
0x5e0f,
0x5f1b,
0x602e,
0x613e,
0x624e,
0x635b,
0x6465,
0x6571,
0x667b,
0x6785,
0x688d,
0x6995,
0x6a9b,
0x6ba3,
0x6ca9,
0x6db0,
0x6eb6,
0x6fbb,
0x70c5,
0x71cf,
0x72d7,
0x73e7,
0x74f3,
0x75fa,
0x76ff,
0x77ff,
0x78ff,
0x79ff,
0x7aff,
0x7bff,
0x7cff,

///////////////////
// Adaptive MCMC //
///////////////////

// Outdoor MCMC
0x7d15, //Outdoor_delta1
0x7e19, //Outdoor_center1
0x7f0f, //Outdoor_delta2
0x8086, //Outdoor_center2
0x8117, //Outdoor_delta3
0x82bd, //Outdoor_center3
0x8317, //Outdoor_delta4
0x84ee, //Outdoor_center4
0x8593, //Outdoor_delta5
0x8625, //Outdoor_center5
0x8793, //Outdoor_delta6
0x8851, //Outdoor_center6

0x8940, //Outdoor_sat_gain1
0x8a40, //Outdoor_sat_gain2
0x8b40, //Outdoor_sat_gain3
0x8c40, //Outdoor_sat_gain4
0x8d40, //Outdoor_sat_gain5
0x8e40, //Outdoor_sat_gain6
0x8f94, //Outdoor_hue_angle1
0x9089, //Outdoor_hue_angle2
0x9110, //Outdoor_hue_angle3
0x9214, //Outdoor_hue_angle4
0x930b, //Outdoor_hue_angle5
0x9487, //Outdoor_hue_angle6

0x9500,	// MCMC24_Outdoor  mcmc_rgb_ofs_sign_r
0x9600,	// MCMC25_Outdoor  mcmc_rgb_ofs_sign_g
0x9700,	// MCMC26_Outdoor  mcmc_rgb_ofs_sign_b

0x9800,	// MCMC27_Outdoor  mcmc_rgb_ofs_r1 R
0x9900,	// MCMC28_Outdoor  mcmc_rgb_ofs_r1 G
0x9a00,	// MCMC29_Outdoor  mcmc_rgb_ofs_r1 B

0x9b00,	// MCMC30_Outdoor  mcmc_rgb_ofs_r2 R
0x9c00,	// MCMC31_Outdoor  mcmc_rgb_ofs_r2 G
0x9d00,	// MCMC32_Outdoor  mcmc_rgb_ofs_r2 B

0x9e00,	// MCMC33_Outdoor  mcmc_rgb_ofs_r3 R
0x9f00,	// MCMC34_Outdoor  mcmc_rgb_ofs_r3 G
0xa000,	// MCMC35_Outdoor  mcmc_rgb_ofs_r3 B

0xa100,	// MCMC36_Outdoor  mcmc_rgb_ofs_r4 R
0xa200,	// MCMC37_Outdoor  mcmc_rgb_ofs_r4 G
0xa300,	// MCMC38_Outdoor  mcmc_rgb_ofs_r4 B

0xa400,	// MCMC39_Outdoor  mcmc_rgb_ofs_r5 R
0xa500,	// MCMC40_Outdoor  mcmc_rgb_ofs_r5 G
0xa600,	// MCMC41_Outdoor  mcmc_rgb_ofs_r5 B

0xa700,	// MCMC42_Outdoor  mcmc_rgb_ofs_r6 R
0xa800,	// MCMC43_Outdoor  mcmc_rgb_ofs_r6 G
0xa900,	// MCMC44_Outdoor  mcmc_rgb_ofs_r6 B

0xaa00,	// MCMC45_Outdoor  mcmc_std_offset1
0xab00,	// MCMC46_Outdoor  mcmc_std_offset2
0xacff,	// MCMC47_Outdoor  mcmc_std_th_max
0xad00,	// MCMC48_Outdoor  mcmc_std_th_min

0xae3f,	// MCMC49_Outdoor  mcmc_lum_gain_wgt_th1 R1 magenta
0xaf3f,	// MCMC50_Outdoor  mcmc_lum_gain_wgt_th2 R1
0xb03f,	// MCMC51_Outdoor  mcmc_lum_gain_wgt_th3 R1
0xb13f,	// MCMC52_Outdoor  mcmc_lum_gain_wgt_th4 R1
0xb230,	// MCMC53_Outdoor  mcmc_rg1_lum_sp1      R1
0xb350,	// MCMC54_Outdoor  mcmc_rg1_lum_sp2      R1
0xb480,	// MCMC55_Outdoor  mcmc_rg1_lum_sp3      R1
0xb5b0,	// MCMC56_Outdoor  mcmc_rg1_lum_sp4      R1

0xb63f,	// MCMC57_Outdoor  mcmc_lum_gain_wgt_th1 R2 red
0xb73f,	// MCMC58_Outdoor  mcmc_lum_gain_wgt_th2 R2
0xb83f,	// MCMC59_Outdoor  mcmc_lum_gain_wgt_th3 R2
0xb93f,	// MCMC60_Outdoor  mcmc_lum_gain_wgt_th4 R2
0xba28,	// MCMC61_Outdoor  mcmc_rg2_lum_sp1      R2
0xbb50,	// MCMC62_Outdoor  mcmc_rg2_lum_sp2      R2
0xbc80,	// MCMC63_Outdoor  mcmc_rg2_lum_sp3      R2
0xbdb0,	// MCMC64_Outdoor  mcmc_rg2_lum_sp4      R2

0xbe3f,	// MCMC65_Outdoor  mcmc_lum_gain_wgt_th1 R3 yellow
0xbf3f,	// MCMC66_Outdoor  mcmc_lum_gain_wgt_th2 R3
0xc030,// MCMC67_Outdoor  mcmc_lum_gain_wgt_th3 R3
0xc12a,// MCMC68_Outdoor  mcmc_lum_gain_wgt_th4 R3
0xc220,// MCMC69_Outdoor  mcmc_rg3_lum_sp1      R3
0xc340,// MCMC70_Outdoor  mcmc_rg3_lum_sp2      R3
0xc470,// MCMC71_Outdoor  mcmc_rg3_lum_sp3      R3
0xc5b0,	// MCMC72_Outdoor  mcmc_rg3_lum_sp4      R3

0xc63f,	// MCMC73_Outdoor  mcmc_lum_gain_wgt_th1 R4 Green
0xc73f,	// MCMC74_Outdoor  mcmc_lum_gain_wgt_th2 R4
0xc83f,	// MCMC75_Outdoor  mcmc_lum_gain_wgt_th3 R4
0xc93f,	// MCMC76_Outdoor  mcmc_lum_gain_wgt_th4 R4
0xca10,	// MCMC77_Outdoor  mcmc_rg4_lum_sp1      R4
0xcb30,	// MCMC78_Outdoor  mcmc_rg4_lum_sp2      R4
0xcc60,	// MCMC79_Outdoor  mcmc_rg4_lum_sp3      R4
0xcd90,	// MCMC80_Outdoor  mcmc_rg4_lum_sp4      R4

0xce3f,	// MCMC81_Outdoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0xcf3f,	// MCMC82_Outdoor  mcmc_rg5_gain_wgt_th2 R5
0xd03f,	// MCMC83_Outdoor  mcmc_rg5_gain_wgt_th3 R5
0xd13f,	// MCMC84_Outdoor  mcmc_rg5_gain_wgt_th4 R5
0xd228,	// MCMC85_Outdoor  mcmc_rg5_lum_sp1      R5
0xd350,	// MCMC86_Outdoor  mcmc_rg5_lum_sp2      R5
0xd480,	// MCMC87_Outdoor  mcmc_rg5_lum_sp3      R5
0xd5b0,	// MCMC88_Outdoor  mcmc_rg5_lum_sp4      R5

0xd63f,	// MCMC89_Outdoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0xd73f,	// MCMC90_Outdoor  mcmc_rg6_gain_wgt_th2 R6
0xd83f,	// MCMC91_Outdoor  mcmc_rg6_gain_wgt_th3 R6
0xd93f,	// MCMC92_Outdoor  mcmc_rg6_gain_wgt_th4 R6
0xda28,	// MCMC93_Outdoor  mcmc_rg6_lum_sp1      R6
0xdb50,	// MCMC94_Outdoor  mcmc_rg6_lum_sp2      R6
0xdc80,	// MCMC95_Outdoor  mcmc_rg6_lum_sp3      R6
0xddb0,	// MCMC96_Outdoor  mcmc_rg6_lum_sp4      R6

0xde1e,	// MCMC97_Outdoor  mcmc2_allgain_x1
0xdf3c,	// MCMC98_Outdoor  mcmc2_allgain_x2
0xe03c,	// MCMC99_Outdoor  mcmc2_allgain_x4
0xe11e,	// MCMC100_Outdoor mcmc2_allgain_x5
0xe21e,	// MCMC101_Outdoor mcmc2_allgain_x7
0xe33c,	// MCMC102_Outdoor mcmc2_allgain_x8
0xe43c,	// MCMC103_Outdoor mcmc2_allgain_x10
0xe51e,	// MCMC104_Outdoor mcmc2_allgain_x11

0xe614, //Outdoor_allgain_y1
0xe714, //Outdoor_allgain_y2
0xe813, //Outdoor_allgain_y3
0xe912, //Outdoor_allgain_y4
0xea12, //Outdoor_allgain_y5
0xeb14, //Outdoor_allgain_y6
0xec16, //Outdoor_allgain_y7
0xed18, //Outdoor_allgain_y8
0xee1a, //Outdoor_allgain_y9
0xef17, //Outdoor_allgain_y10
0xf014, //Outdoor_allgain_y11
0xf113, //Outdoor_allgain_y12

// Indoor MCMC
0xf210, //Indoor_delta1
0xf31e, //Indoor_center1
0xf40b, //Indoor_delta2
0xf56f, //Indoor_center2
0xf610, //Indoor_delta3
0xf79c, // Indoor_center3
0xf807, // Indoor_delta4
0xf9b8, // Indoor_center4
0xfa90, //Indoor_delta5
0xfb2d, //Indoor_center5
0xfc92, //Indoor_delta6
0xfd4f, //Indoor_center6
0x0e00, // burst end

0x03d6,	// Page D6

0x0e01, // burst start

0x1040, //Indoor_sat_gain1
0x1140, //Indoor_sat_gain2
0x1240, //Indoor_sat_gain3
0x1340, //Indoor_sat_gain4
0x1440, //Indoor_sat_gain5
0x1540, //Indoor_sat_gain6

0x1600, //Indoor_hue_angle1
0x178a, //Indoor_hue_angle2
0x1800, //Indoor_hue_angle3
0x1900, //Indoor_hue_angle4
0x1a00, //Indoor_hue_angle5
0x1b02, //Indoor_hue_angle6

0x1c00,	// MCMC24_Indoor   mcmc_rgb_ofs_sign_r
0x1d00,	// MCMC25_Indoor   mcmc_rgb_ofs_sign_g
0x1e00,	// MCMC26_Indoor   mcmc_rgb_ofs_sign_b

0x1f00,	// MCMC27_Indoor   mcmc_rgb_ofs_r1 R
0x2000,	// MCMC28_Indoor   mcmc_rgb_ofs_r1 G
0x2100,	// MCMC29_Indoor   mcmc_rgb_ofs_r1 B

0x2200,	// MCMC30_Indoor   mcmc_rgb_ofs_r2 R
0x2300,	// MCMC31_Indoor   mcmc_rgb_ofs_r2 G
0x2400,	// MCMC32_Indoor   mcmc_rgb_ofs_r2 B

0x2500,	// MCMC33_Indoor   mcmc_rgb_ofs_r3 R
0x2600,	// MCMC34_Indoor   mcmc_rgb_ofs_r3 G
0x2700,	// MCMC35_Indoor   mcmc_rgb_ofs_r3 B

0x2800,	// MCMC36_Indoor   mcmc_rgb_ofs_r4 R
0x2900,	// MCMC37_Indoor   mcmc_rgb_ofs_r4 G
0x2a00,	// MCMC38_Indoor   mcmc_rgb_ofs_r4 B

0x2b00,	// MCMC39_Indoor   mcmc_rgb_ofs_r5 R
0x2c00,	// MCMC40_Indoor   mcmc_rgb_ofs_r5 G
0x2d00,	// MCMC41_Indoor   mcmc_rgb_ofs_r5 B

0x2e00,	// MCMC42_Indoor  mcmc_rgb_ofs_r6 R
0x2f00,	// MCMC43_Indoor  mcmc_rgb_ofs_r6 G
0x3000,	// MCMC44_Indoor  mcmc_rgb_ofs_r6 B

0x3100,	// MCMC45_Indoor  mcmc_std_offset1
0x3200,	// MCMC46_Indoor  mcmc_std_offset2
0x33ff,	// MCMC47_Indoor  mcmc_std_th_max
0x3400,	// MCMC48_Indoor  mcmc_std_th_min

0x3510,	// MCMC49_Indoor  mcmc_lum_gain_wgt_th1 R1 magenta
0x3621,	// MCMC50_Indoor  mcmc_lum_gain_wgt_th2 R1
0x3734,	// MCMC51_Indoor  mcmc_lum_gain_wgt_th3 R1
0x383f,	// MCMC52_Indoor  mcmc_lum_gain_wgt_th4 R1
0x3908,	// MCMC53_Indoor  mcmc_rg1_lum_sp1      R1
0x3a15,	// MCMC54_Indoor  mcmc_rg1_lum_sp2      R1
0x3b2f,	// MCMC55_Indoor  mcmc_rg1_lum_sp3      R1
0x3c51,	// MCMC56_Indoor  mcmc_rg1_lum_sp4      R1

0x3d3f,	// MCMC57_Indoor  mcmc_lum_gain_wgt_th1 R2 red
0x3e3f,	// MCMC58_Indoor  mcmc_lum_gain_wgt_th2 R2
0x3f3f,	// MCMC59_Indoor  mcmc_lum_gain_wgt_th3 R2
0x403f,	// MCMC60_Indoor  mcmc_lum_gain_wgt_th4 R2
0x4128,	// MCMC61_Indoor  mcmc_rg2_lum_sp1      R2
0x4250,	// MCMC62_Indoor  mcmc_rg2_lum_sp2      R2
0x4380,	// MCMC63_Indoor  mcmc_rg2_lum_sp3      R2
0x44b0,	// MCMC64_Indoor  mcmc_rg2_lum_sp4      R2

0x453f,	// MCMC65_Indoor  mcmc_lum_gain_wgt_th1 R3 yellow
0x463f,	// MCMC66_Indoor  mcmc_lum_gain_wgt_th2 R3
0x473f,	// MCMC67_Indoor  mcmc_lum_gain_wgt_th3 R3
0x483f,	// MCMC68_Indoor  mcmc_lum_gain_wgt_th4 R3
0x4928,	// MCMC69_Indoor  mcmc_rg3_lum_sp1      R3
0x4a50,	// MCMC70_Indoor  mcmc_rg3_lum_sp2      R3
0x4b80,	// MCMC71_Indoor  mcmc_rg3_lum_sp3      R3
0x4cb0,	// MCMC72_Indoor  mcmc_rg3_lum_sp4      R3

0x4d3f,	// MCMC73_Indoor  mcmc_lum_gain_wgt_th1 R4 Green
0x4e3f,	// MCMC74_Indoor  mcmc_lum_gain_wgt_th2 R4
0x4f3f,	// MCMC75_Indoor  mcmc_lum_gain_wgt_th3 R4
0x503f,	// MCMC76_Indoor  mcmc_lum_gain_wgt_th4 R4
0x5110,	// MCMC77_Indoor  mcmc_rg4_lum_sp1      R4
0x5230,	// MCMC78_Indoor  mcmc_rg4_lum_sp2      R4
0x5360,	// MCMC79_Indoor  mcmc_rg4_lum_sp3      R4
0x5490,	// MCMC80_Indoor  mcmc_rg4_lum_sp4      R4

0x553f,	// MCMC81_Indoor  mcmc_rg5_gain_wgt_th1 R5 Cyan
0x563f,	// MCMC82_Indoor  mcmc_rg5_gain_wgt_th2 R5
0x573f,	// MCMC83_Indoor  mcmc_rg5_gain_wgt_th3 R5
0x583f,	// MCMC84_Indoor  mcmc_rg5_gain_wgt_th4 R5
0x5928,	// MCMC85_Indoor  mcmc_rg5_lum_sp1      R5
0x5a50,	// MCMC86_Indoor  mcmc_rg5_lum_sp2      R5
0x5b80,	// MCMC87_Indoor  mcmc_rg5_lum_sp3      R5
0x5cb0,	// MCMC88_Indoor  mcmc_rg5_lum_sp4      R5

0x5d3f,	// MCMC89_Indoor  mcmc_rg6_gain_wgt_th1 R6 Blue
0x5e3f,	// MCMC90_Indoor  mcmc_rg6_gain_wgt_th2 R6
0x5f3f,	// MCMC91_Indoor  mcmc_rg6_gain_wgt_th3 R6
0x603f,	// MCMC92_Indoor  mcmc_rg6_gain_wgt_th4 R6
0x6128,	// MCMC93_Indoor  mcmc_rg6_lum_sp1      R6
0x6250,	// MCMC94_Indoor  mcmc_rg6_lum_sp2      R6
0x6380,	// MCMC95_Indoor  mcmc_rg6_lum_sp3      R6
0x64b0,	// MCMC96_Indoor  mcmc_rg6_lum_sp4      R6

0x651d,	// MCMC97_Indoor  mcmc2_allgain_x1
0x663b,	// MCMC98_Indoor  mcmc2_allgain_x2
0x673b,	// MCMC99_Indoor  mcmc2_allgain_x4
0x681d,	// MCMC100_Indoor mcmc2_allgain_x5
0x691d,	// MCMC101_Indoor mcmc2_allgain_x7
0x6a3b,	// MCMC102_Indoor mcmc2_allgain_x8
0x6b3b,	// MCMC103_Indoor mcmc2_allgain_x10
0x6c1d,	// MCMC104_Indoor mcmc2_allgain_x11

0x6d0e,	// MCMC105_Indoor mcmc2_allgain_y0
0x6e0f,	// MCMC106_Indoor mcmc2_allgain_y1
0x6f0f,	// MCMC107_Indoor mcmc2_allgain_y2
0x700f,	// MCMC108_Indoor mcmc2_allgain_y3
0x710f,	// MCMC109_Indoor mcmc2_allgain_y4
0x7210,	// MCMC110_Indoor mcmc2_allgain_y5
0x7310,	// MCMC111_Indoor mcmc2_allgain_y6
0x7410,	// MCMC112_Indoor mcmc2_allgain_y7
0x7510,	// MCMC113_Indoor mcmc2_allgain_y8
0x760f,	// MCMC114_Indoor mcmc2_allgain_y9
0x770e,	// MCMC115_Indoor mcmc2_allgain_y10
0x780d,	// MCMC116_Indoor mcmc2_allgain_y11

// Dark1 MCMC
0x7917, //Dark1_delta1
0x7a56, //Dark1_center1
0x7b10, //Dark1_delta2
0x7c70, //Dark1_center2
0x7d10, //Dark1_delta3
0x7e9c, //Dark1_center3
0x7f18, //Dark1_delta4
0x80db, //Dark1_center4
0x8198, //Dark1_delta5
0x8226, //Dark1_center5
0x8399, //Dark1_delta6
0x845b, //Dark1_center6

0x8540, //Dark1_sat_gain1
0x8640, //Dark1_sat_gain2
0x8740, //Dark1_sat_gain3
0x8840, //Dark1_sat_gain4
0x8940, //Dark1_sat_gain5
0x8a40, //Dark1_sat_gain6
0x8b91, //Dark1_hue_angle1
0x8c00, //Dark1_hue_angle2
0x8d00, //Dark1_hue_angle3
0x8e0a, //Dark1_hue_angle4
0x8f05, //Dark1_hue_angle5
0x9086, //Dark1_hue_angle6

0x913f,	// MCMC24_Dark1   mcmc_rgb_ofs_sign
0x923f,	// MCMC25_Dark1   mcmc_rgb_ofs_sign
0x933f,	// MCMC26_Dark1   mcmc_rgb_ofs_sign

0x9400,	// MCMC27_Dark1   mcmc_rgb_ofs_r1 R
0x9500,	// MCMC28_Dark1   mcmc_rgb_ofs_r1 G
0x9600,	// MCMC29_Dark1   mcmc_rgb_ofs_r1 B

0x9700,	// MCMC30_Dark1   mcmc_rgb_ofs_r2 R
0x9800,	// MCMC31_Dark1   mcmc_rgb_ofs_r2 G
0x9900,	// MCMC32_Dark1   mcmc_rgb_ofs_r2 B

0x9a00,	// MCMC33_Dark1   mcmc_rgb_ofs_r3 R
0x9b00,	// MCMC34_Dark1   mcmc_rgb_ofs_r3 G
0x9c00,	// MCMC35_Dark1   mcmc_rgb_ofs_r3 B

0x9d00,	// MCMC36_Dark1   mcmc_rgb_ofs_r4 R
0x9e00,	// MCMC37_Dark1   mcmc_rgb_ofs_r4 G
0x9f00,	// MCMC38_Dark1   mcmc_rgb_ofs_r4 B

0xa000,	// MCMC39_Dark1   mcmc_rgb_ofs_r5 R
0xa100,	// MCMC40_Dark1   mcmc_rgb_ofs_r5 G
0xa200,	// MCMC41_Dark1   mcmc_rgb_ofs_r5 B

0xa300,	// MCMC42_Dark1  mcmc_rgb_ofs_r6 R
0xa400,	// MCMC43_Dark1  mcmc_rgb_ofs_r6 G
0xa500,	// MCMC44_Dark1  mcmc_rgb_ofs_r6 B

0xa600,	// MCMC45_Dark1  mcmc_std_offset1
0xa700,	// MCMC46_Dark1  mcmc_std_offset2
0xa8ff,	// MCMC47_Dark1  mcmc_std_th_max
0xa900,	// MCMC48_Dark1  mcmc_std_th_min

0xaa3f,	// MCMC49_Dark1  mcmc_lum_gain_wgt R1
0xab3f,	// MCMC50_Dark1  mcmc_lum_gain_wgt R1
0xac3f,	// MCMC51_Dark1  mcmc_lum_gain_wgt R1
0xad3f,	// MCMC52_Dark1  mcmc_lum_gain_wgt R1
0xae30,	// MCMC53_Dark1  mcmc_rg1_lum_sp1  R1
0xaf50,	// MCMC54_Dark1  mcmc_rg1_lum_sp2  R1
0xb080,	// MCMC55_Dark1  mcmc_rg1_lum_sp3  R1
0xb1b0,	// MCMC56_Dark1  mcmc_rg1_lum_sp4  R1

0xb23f,	// MCMC57_Dark1  mcmc_lum_gain_wgt R2
0xb33f,	// MCMC58_Dark1  mcmc_lum_gain_wgt R2
0xb43f,	// MCMC59_Dark1  mcmc_lum_gain_wgt R2
0xb53f,	// MCMC60_Dark1  mcmc_lum_gain_wgt R2
0xb628,	// MCMC61_Dark1  mcmc_rg2_lum_sp1  R2
0xb750,	// MCMC62_Dark1  mcmc_rg2_lum_sp2  R2
0xb880,	// MCMC63_Dark1  mcmc_rg2_lum_sp3  R2
0xb9b0,	// MCMC64_Dark1  mcmc_rg2_lum_sp4  R2

0xba3f,	// MCMC65_Dark1  mcmc_lum_gain_wgt R3
0xbb3f,	// MCMC66_Dark1  mcmc_lum_gain_wgt R3
0xbc3f,	// MCMC67_Dark1  mcmc_lum_gain_wgt R3
0xbd3f,	// MCMC68_Dark1  mcmc_lum_gain_wgt R3
0xbe28,	// MCMC69_Dark1  mcmc_rg3_lum_sp1  R3
0xbf50,	// MCMC70_Dark1  mcmc_rg3_lum_sp2  R3
0xc080,	// MCMC71_Dark1  mcmc_rg3_lum_sp3  R3
0xc1b0,	// MCMC72_Dark1  mcmc_rg3_lum_sp4  R3

0xc23f,	// MCMC73_Dark1  mcmc_lum_gain_wgt R4
0xc33f,	// MCMC74_Dark1  mcmc_lum_gain_wgt R4
0xc43f,	// MCMC75_Dark1  mcmc_lum_gain_wgt R4
0xc53f,	// MCMC76_Dark1  mcmc_lum_gain_wgt R4
0xc610,	// MCMC77_Dark1  mcmc_rg4_lum_sp1  R4
0xc730,	// MCMC78_Dark1  mcmc_rg4_lum_sp2  R4
0xc860,	// MCMC79_Dark1  mcmc_rg4_lum_sp3  R4
0xc990,	// MCMC80_Dark1  mcmc_rg4_lum_sp4  R4

0xca3f,	// MCMC81_Dark1  mcmc_rg5_gain_wgt R5
0xcb3f,	// MCMC82_Dark1  mcmc_rg5_gain_wgt R5
0xcc3f,	// MCMC83_Dark1  mcmc_rg5_gain_wgt R5
0xcd3f,	// MCMC84_Dark1  mcmc_rg5_gain_wgt R5
0xce28,	// MCMC85_Dark1  mcmc_rg5_lum_sp1  R5
0xcf50,	// MCMC86_Dark1  mcmc_rg5_lum_sp2  R5
0xd080,	// MCMC87_Dark1  mcmc_rg5_lum_sp3  R5
0xd1b0,	// MCMC88_Dark1  mcmc_rg5_lum_sp4  R5

0xd23f,	// MCMC89_Dark1  mcmc_rg6_gain_wgt R6
0xd33f,	// MCMC90_Dark1  mcmc_rg6_gain_wgt R6
0xd43f,	// MCMC91_Dark1  mcmc_rg6_gain_wgt R6
0xd53f,	// MCMC92_Dark1  mcmc_rg6_gain_wgt R6
0xd628,	// MCMC93_Dark1  mcmc_rg6_lum_sp1  R6
0xd750,	// MCMC94_Dark1  mcmc_rg6_lum_sp2  R6
0xd880,	// MCMC95_Dark1  mcmc_rg6_lum_sp3  R6
0xd9b0,	// MCMC96_Dark1  mcmc_rg6_lum_sp4  R6

0xda1c,	// MCMC97_Dark1  mcmc2_allgain_x1
0xdb3a,	// MCMC98_Dark1  mcmc2_allgain_x2
0xdc3a,	// MCMC99_Dark1  mcmc2_allgain_x4
0xdd1c,	// MCMC100_Dark1 mcmc2_allgain_x5
0xde1c,	// MCMC101_Dark1 mcmc2_allgain_x7
0xdf3a,	// MCMC102_Dark1 mcmc2_allgain_x8
0xe03a,	// MCMC103_Dark1 mcmc2_allgain_x10
0xe11c,	// MCMC104_Dark1 mcmc2_allgain_x11

0xe20f, //Dark1_allgain_y1
0xe310, //Dark1_allgain_y2
0xe410, //Dark1_allgain_y3
0xe511, //Dark1_allgain_y4
0xe610, //Dark1_allgain_y5
0xe713, //Dark1_allgain_y6
0xe812, //Dark1_allgain_y7
0xe912, //Dark1_allgain_y8
0xea12, //Dark1_allgain_y9
0xeb11, //Dark1_allgain_y10
0xec10, //Dark1_allgain_y11
0xed0f, //Dark1_allgain_y12

// Dark2 MCMC
0xee17,	// MCMC00_Dark2   mcmc_delta1
0xef56,	// MCMC01_Dark2   mcmc_center1
0xf010,	// MCMC02_Dark2   mcmc_delta2
0xf170,	// MCMC03_Dark2   mcmc_center2
0xf210,	// MCMC04_Dark2   mcmc_delta3
0xf39c,	// MCMC05_Dark2   mcmc_center3
0xf418,	// MCMC06_Dark2   mcmc_delta4
0xf5db,	// MCMC07_Dark2   mcmc_center4
0xf698,	// MCMC08_Dark2   mcmc_delta5
0xf726,	// MCMC09_Dark2   mcmc_center5
0xf899,	// MCMC10_Dark2   mcmc_delta6
0xf95b,	// MCMC11_Dark2   mcmc_center6

0xfa40,	// MCMC12_Dark2   mcmc_sat_gain1
0xfb40,	// MCMC13_Dark2   mcmc_sat_gain2
0xfc40,	// MCMC14_Dark2   mcmc_sat_gain3
0xfd40,	// MCMC15_Dark2   mcmc_sat_gain4
0x0e00, // burst end

0x03d7,// Page D7

0x0e01, // burst start

0x1040,	// MCMC16_Dark2   mcmc_sat_gain5
0x1140,	// MCMC17_Dark2   mcmc_sat_gain6
0x1291,	// MCMC18_Dark2   mcmc_hue_angle1
0x1300,	// MCMC19_Dark2   mcmc_hue_angle2
0x1400,	// MCMC20_Dark2   mcmc_hue_angle3
0x150a,	// MCMC21_Dark2   mcmc_hue_angle4
0x160f,	// MCMC22_Dark2   mcmc_hue_angle5
0x1786,	// MCMC23_Dark2   mcmc_hue_angle6

0x182f,	// MCMC24_Dark2   mcmc_rgb_ofs_sig
0x192f,	// MCMC25_Dark2   mcmc_rgb_ofs_sig
0x1a2f,	// MCMC26_Dark2   mcmc_rgb_ofs_sig

0x1b00,	// MCMC27_Dark2   mcmc_rgb_ofs_r1
0x1c00,	// MCMC28_Dark2   mcmc_rgb_ofs_r1
0x1d00,	// MCMC29_Dark2   mcmc_rgb_ofs_r1

0x1e00,	// MCMC30_Dark2   mcmc_rgb_ofs_r2
0x1f00,	// MCMC31_Dark2   mcmc_rgb_ofs_r2
0x2000,	// MCMC32_Dark2   mcmc_rgb_ofs_r2

0x2100,	// MCMC33_Dark2   mcmc_rgb_ofs_r3
0x2200,	// MCMC34_Dark2   mcmc_rgb_ofs_r3
0x2300,	// MCMC35_Dark2   mcmc_rgb_ofs_r3

0x2400,	// MCMC36_Dark2   mcmc_rgb_ofs_r4
0x2500,	// MCMC37_Dark2   mcmc_rgb_ofs_r4
0x2600,	// MCMC38_Dark2   mcmc_rgb_ofs_r4

0x2700,	// MCMC39_Dark2   mcmc_rgb_ofs_r5
0x2800,	// MCMC40_Dark2   mcmc_rgb_ofs_r5
0x2900,	// MCMC41_Dark2   mcmc_rgb_ofs_r5

0x2a00,	// MCMC42_Dark2  mcmc_rgb_ofs_r6 R
0x2b00,	// MCMC43_Dark2  mcmc_rgb_ofs_r6 G
0x2c00,	// MCMC44_Dark2  mcmc_rgb_ofs_r6 B

0x2d00,	// MCMC45_Dark2  mcmc_std_offset1
0x2e00,	// MCMC46_Dark2  mcmc_std_offset2
0x2fff,	// MCMC47_Dark2  mcmc_std_th_max
0x3000,	// MCMC48_Dark2  mcmc_std_th_min

0x313f,	// MCMC49_Dark2  mcmc_lum_gain_wgt R1
0x323f,	// MCMC50_Dark2  mcmc_lum_gain_wgt R1
0x333f,	// MCMC51_Dark2  mcmc_lum_gain_wgt R1
0x343f,	// MCMC52_Dark2  mcmc_lum_gain_wgt R1
0x3530,	// MCMC53_Dark2  mcmc_rg1_lum_sp1  R1
0x3650,	// MCMC54_Dark2  mcmc_rg1_lum_sp2  R1
0x3780,	// MCMC55_Dark2  mcmc_rg1_lum_sp3  R1
0x38b0,	// MCMC56_Dark2  mcmc_rg1_lum_sp4  R1

0x393f,	// MCMC57_Dark2  mcmc_lum_gain_wgt R2
0x3a3f,	// MCMC58_Dark2  mcmc_lum_gain_wgt R2
0x3b3f,	// MCMC59_Dark2  mcmc_lum_gain_wgt R2
0x3c3f,	// MCMC60_Dark2  mcmc_lum_gain_wgt R2
0x3d28,	// MCMC61_Dark2  mcmc_rg2_lum_sp1  R2
0x3e50,	// MCMC62_Dark2  mcmc_rg2_lum_sp2  R2
0x3f80,	// MCMC63_Dark2  mcmc_rg2_lum_sp3  R2
0x40b0,	// MCMC64_Dark2  mcmc_rg2_lum_sp4  R2

0x413f,	// MCMC65_Dark2  mcmc_lum_gain_wgt R3
0x423f,	// MCMC66_Dark2  mcmc_lum_gain_wgt R3
0x433f,	// MCMC67_Dark2  mcmc_lum_gain_wgt R3
0x443f,	// MCMC68_Dark2  mcmc_lum_gain_wgt R3
0x4528,	// MCMC69_Dark2  mcmc_rg3_lum_sp1  R3
0x4650,	// MCMC70_Dark2  mcmc_rg3_lum_sp2  R3
0x4780,	// MCMC71_Dark2  mcmc_rg3_lum_sp3  R3
0x48b0,	// MCMC72_Dark2  mcmc_rg3_lum_sp4  R3

0x491a,	// MCMC73_Dark2  mcmc_lum_gain_wgt R4
0x4a28,	// MCMC74_Dark2  mcmc_lum_gain_wgt R4
0x4b3f,	// MCMC75_Dark2  mcmc_lum_gain_wgt R4
0x4c3f,	// MCMC76_Dark2  mcmc_lum_gain_wgt R4
0x4d10,	// MCMC77_Dark2  mcmc_rg4_lum_sp1  R4
0x4e30,	// MCMC78_Dark2  mcmc_rg4_lum_sp2  R4
0x4f60,	// MCMC79_Dark2  mcmc_rg4_lum_sp3  R4
0x5090,	// MCMC80_Dark2  mcmc_rg4_lum_sp4  R4

0x511a,	// MCMC81_Dark2  mcmc_rg5_gain_wgt R5
0x5228,	// MCMC82_Dark2  mcmc_rg5_gain_wgt R5
0x533f,	// MCMC83_Dark2  mcmc_rg5_gain_wgt R5
0x543f,	// MCMC84_Dark2  mcmc_rg5_gain_wgt R5
0x5528,	// MCMC85_Dark2  mcmc_rg5_lum_sp1  R5
0x5650,	// MCMC86_Dark2  mcmc_rg5_lum_sp2  R5
0x5780,	// MCMC87_Dark2  mcmc_rg5_lum_sp3  R5
0x58b0,	// MCMC88_Dark2  mcmc_rg5_lum_sp4  R5

0x591a,	// MCMC89_Dark2  mcmc_rg6_gain_wgt R6
0x5a28,	// MCMC90_Dark2  mcmc_rg6_gain_wgt R6
0x5b3f,	// MCMC91_Dark2  mcmc_rg6_gain_wgt R6
0x5c3f,	// MCMC92_Dark2  mcmc_rg6_gain_wgt R6
0x5d28,	// MCMC93_Dark2  mcmc_rg6_lum_sp1  R6
0x5e50,	// MCMC94_Dark2  mcmc_rg6_lum_sp2  R6
0x5f80,	// MCMC95_Dark2  mcmc_rg6_lum_sp3  R6
0x60b0,	// MCMC96_Dark2  mcmc_rg6_lum_sp4  R6

0x611b,	// MCMC97_Dark2  mcmc2_allgain_x1
0x6239,	// MCMC98_Dark2  mcmc2_allgain_x2
0x6339,	// MCMC99_Dark2  mcmc2_allgain_x4
0x641b,	// MCMC100_Dark2 mcmc2_allgain_x5
0x651b,	// MCMC101_Dark2 mcmc2_allgain_x7
0x6639,	// MCMC102_Dark2 mcmc2_allgain_x8
0x6739,	// MCMC103_Dark2 mcmc2_allgain_x10
0x681b,	// MCMC104_Dark2 mcmc2_allgain_x11

0x690f,	// MCMC105_Dark2 mcmc2_allgain_y0
0x6a10,	// MCMC106_Dark2 mcmc2_allgain_y1
0x6b10,	// MCMC107_Dark2 mcmc2_allgain_y2
0x6c11,	// MCMC108_Dark2 mcmc2_allgain_y3
0x6d10,	// MCMC109_Dark2 mcmc2_allgain_y4
0x6e13,	// MCMC110_Dark2 mcmc2_allgain_y5
0x6f12,	// MCMC111_Dark2 mcmc2_allgain_y6
0x7012,	// MCMC112_Dark2 mcmc2_allgain_y7
0x7112,	// MCMC113_Dark2 mcmc2_allgain_y8
0x7211,	// MCMC114_Dark2 mcmc2_allgain_y9
0x7310,	// MCMC115_Dark2 mcmc2_allgain_y10
0x740f,	// MCMC116_Dark2 mcmc2_allgain_y11

// LowTemp MCMC
0x7510,	// MCMC00_LowTemp   mcmc_delta1
0x7639,	// MCMC01_LowTemp   mcmc_center1
0x7710,	// MCMC02_LowTemp   mcmc_delta2
0x7859,	// MCMC03_LowTemp   mcmc_center2
0x7912,	// MCMC04_LowTemp   mcmc_delta3
0x7a9d,	// MCMC05_LowTemp   mcmc_center3
0x7b12,	// MCMC06_LowTemp   mcmc_delta4
0x7cc1,	// MCMC07_LowTemp   mcmc_center4
0x7d18,	// MCMC08_LowTemp   mcmc_delta5
0x7eeb,	// MCMC09_LowTemp   mcmc_center5
0x7f99,	// MCMC10_LowTemp   mcmc_delta6
0x801c,	// MCMC11_LowTemp   mcmc_center6

0x8140,	// MCMC12_LowTemp   mcmc_sat_gain1
0x8240,	// MCMC13_LowTemp   mcmc_sat_gain2
0x8340,	// MCMC14_LowTemp   mcmc_sat_gain3
0x8440,	// MCMC15_LowTemp   mcmc_sat_gain4
0x8540,	// MCMC16_LowTemp   mcmc_sat_gain5
0x8640,	// MCMC17_LowTemp   mcmc_sat_gain6
0x8700,	// MCMC18_LowTemp   mcmc_hue_angle1
0x8800,	// MCMC19_LowTemp   mcmc_hue_angle2
0x8900,	// MCMC20_LowTemp   mcmc_hue_angle3
0x8a00,	// MCMC21_LowTemp   mcmc_hue_angle4
0x8b00,	// MCMC22_LowTemp   mcmc_hue_angle5
0x8c00,	// MCMC23_LowTemp   mcmc_hue_angle6

0x8d1f,	// MCMC24_LowTemp   mcmc_rgb_ofs_sig
0x8e1f,	// MCMC25_LowTemp   mcmc_rgb_ofs_sig
0x8f1f,	// MCMC26_LowTemp   mcmc_rgb_ofs_sig

0x9000,	// MCMC27_LowTemp   mcmc_rgb_ofs_r1
0x9100,	// MCMC28_LowTemp   mcmc_rgb_ofs_r1
0x9200,	// MCMC29_LowTemp   mcmc_rgb_ofs_r1

0x9300,	// MCMC30_LowTemp   mcmc_rgb_ofs_r2
0x9400,	// MCMC31_LowTemp   mcmc_rgb_ofs_r2
0x9500,	// MCMC32_LowTemp   mcmc_rgb_ofs_r2

0x9600,	// MCMC33_LowTemp   mcmc_rgb_ofs_r3
0x9700,	// MCMC34_LowTemp   mcmc_rgb_ofs_r3
0x9800,	// MCMC35_LowTemp   mcmc_rgb_ofs_r3

0x9900,	// MCMC36_LowTemp   mcmc_rgb_ofs_r4
0x9a00,	// MCMC37_LowTemp   mcmc_rgb_ofs_r4
0x9b00,	// MCMC38_LowTemp   mcmc_rgb_ofs_r4

0x9c00,	// MCMC39_LowTemp   mcmc_rgb_ofs_r5
0x9d00,	// MCMC40_LowTemp   mcmc_rgb_ofs_r5
0x9e00,	// MCMC41_LowTemp   mcmc_rgb_ofs_r5

0x9f00,	// MCMC42_LowTemp  mcmc_rgb_ofs_r6 R
0xa000,	// MCMC43_LowTemp  mcmc_rgb_ofs_r6 G
0xa100,	// MCMC44_LowTemp  mcmc_rgb_ofs_r6 B

0xa200,	// MCMC45_LowTemp  mcmc_std_offset1
0xa300,	// MCMC46_LowTemp  mcmc_std_offset2
0xa4ff,	// MCMC47_LowTemp  mcmc_std_th_max
0xa500,	// MCMC48_LowTemp  mcmc_std_th_min

0xa63f,	// MCMC49_LowTemp  mcmc_lum_gain_wgt R1
0xa73f,	// MCMC50_LowTemp  mcmc_lum_gain_wgt R1
0xa83f,	// MCMC51_LowTemp  mcmc_lum_gain_wgt R1
0xa93f,	// MCMC52_LowTemp  mcmc_lum_gain_wgt R1
0xaa30,	// MCMC53_LowTemp  mcmc_rg1_lum_sp1  R1
0xab50,	// MCMC54_LowTemp  mcmc_rg1_lum_sp2  R1
0xac80,	// MCMC55_LowTemp  mcmc_rg1_lum_sp3  R1
0xadb0,	// MCMC56_LowTemp  mcmc_rg1_lum_sp4  R1

0xae3f,	// MCMC57_LowTemp  mcmc_lum_gain_wgt R2
0xaf3f,	// MCMC58_LowTemp  mcmc_lum_gain_wgt R2
0xb03f,	// MCMC59_LowTemp  mcmc_lum_gain_wgt R2
0xb13f,	// MCMC60_LowTemp  mcmc_lum_gain_wgt R2
0xb228,	// MCMC61_LowTemp  mcmc_rg2_lum_sp1  R2
0xb350,	// MCMC62_LowTemp  mcmc_rg2_lum_sp2  R2
0xb480,	// MCMC63_LowTemp  mcmc_rg2_lum_sp3  R2
0xb5b0,	// MCMC64_LowTemp  mcmc_rg2_lum_sp4  R2

0xb63f,	// MCMC65_LowTemp  mcmc_lum_gain_wgt R3
0xb73f,	// MCMC66_LowTemp  mcmc_lum_gain_wgt R3
0xb83f,	// MCMC67_LowTemp  mcmc_lum_gain_wgt R3
0xb93f,	// MCMC68_LowTemp  mcmc_lum_gain_wgt R3
0xba28,	// MCMC69_LowTemp  mcmc_rg3_lum_sp1  R3
0xbb50,	// MCMC70_LowTemp  mcmc_rg3_lum_sp2  R3
0xbc80,	// MCMC71_LowTemp  mcmc_rg3_lum_sp3  R3
0xbdb0,	// MCMC72_LowTemp  mcmc_rg3_lum_sp4  R3

0xbe3f,	// MCMC73_LowTemp  mcmc_lum_gain_wgt R4
0xbf3f,	// MCMC74_LowTemp  mcmc_lum_gain_wgt R4
0xc03f,	// MCMC75_LowTemp  mcmc_lum_gain_wgt R4
0xc13f,	// MCMC76_LowTemp  mcmc_lum_gain_wgt R4
0xc210,	// MCMC77_LowTemp  mcmc_rg4_lum_sp1  R4
0xc330,	// MCMC78_LowTemp  mcmc_rg4_lum_sp2  R4
0xc460,	// MCMC79_LowTemp  mcmc_rg4_lum_sp3  R4
0xc590,	// MCMC80_LowTemp  mcmc_rg4_lum_sp4  R4

0xc63f,	// MCMC81_LowTemp  mcmc_rg5_gain_wgt R5
0xc73f,	// MCMC82_LowTemp  mcmc_rg5_gain_wgt R5
0xc83f,	// MCMC83_LowTemp  mcmc_rg5_gain_wgt R5
0xc93f,	// MCMC84_LowTemp  mcmc_rg5_gain_wgt R5
0xca28,	// MCMC85_LowTemp  mcmc_rg5_lum_sp1  R5
0xcb50,	// MCMC86_LowTemp  mcmc_rg5_lum_sp2  R5
0xcc80,	// MCMC87_LowTemp  mcmc_rg5_lum_sp3  R5
0xcdb0,	// MCMC88_LowTemp  mcmc_rg5_lum_sp4  R5

0xce3f,	// MCMC89_LowTemp  mcmc_rg6_gain_wgt R6
0xcf3f,	// MCMC90_LowTemp  mcmc_rg6_gain_wgt R6
0xd03f,	// MCMC91_LowTemp  mcmc_rg6_gain_wgt R6
0xd13f,	// MCMC92_LowTemp  mcmc_rg6_gain_wgt R6
0xd228,	// MCMC93_LowTemp  mcmc_rg6_lum_sp1  R6
0xd350,	// MCMC94_LowTemp  mcmc_rg6_lum_sp2  R6
0xd480,	// MCMC95_LowTemp  mcmc_rg6_lum_sp3  R6
0xd5b0,	// MCMC96_LowTemp  mcmc_rg6_lum_sp4  R6

0xd61a,	// MCMC97_LowTemp  mcmc2_allgain_x1
0xd738,	// MCMC98_LowTemp  mcmc2_allgain_x2
0xd838,	// MCMC99_LowTemp  mcmc2_allgain_x4
0xd91a,	// MCMC100_LowTemp mcmc2_allgain_x5
0xda1a,	// MCMC101_LowTemp mcmc2_allgain_x7
0xdb38,	// MCMC102_LowTemp mcmc2_allgain_x8
0xdc38,	// MCMC103_LowTemp mcmc2_allgain_x10
0xdd1a,	// MCMC104_LowTemp mcmc2_allgain_x11

0xde10,	// MCMC105_LowTemp mcmc2_allgain_y0
0xdf0f,	// MCMC106_LowTemp mcmc2_allgain_y1
0xe00e,	// MCMC107_LowTemp mcmc2_allgain_y2
0xe10e,	// MCMC108_LowTemp mcmc2_allgain_y3
0xe212,	// MCMC109_LowTemp mcmc2_allgain_y4
0xe316,	// MCMC110_LowTemp mcmc2_allgain_y5
0xe416,	// MCMC111_LowTemp mcmc2_allgain_y6
0xe514,	// MCMC112_LowTemp mcmc2_allgain_y
0xe612,	// MCMC113_LowTemp mcmc2_allgain_y8
0xe710,	// MCMC114_LowTemp mcmc2_allgain_y9
0xe810,	// MCMC115_LowTemp mcmc2_allgain_y10
0xe910,	// MCMC116_LowTemp mcmc2_allgain_y11
0x0e00, // burst end

// HighTemp MCMC
0x03d7, //Page d7
0xea10, //Hi-Temp_delta1
0xeb39, //Hi-Temp_center1
0xec10, //Hi-Temp_delta2
0xed59, //Hi-Temp_center2
0xee12, //Hi-Temp_delta3
0xef9d, //Hi-Temp_center3
0xf012, //Hi-Temp_delta4
0xf1bd, //Hi-Temp_center4
0xf21e, //Hi-Temp_delta5
0xf3f1, //Hi-Temp_center5
0xf49e, //Hi-Temp_delta6
0xf534, //Hi-Temp_center6
0xf640, //Hi-Temp_sat_gain1
0xf740, //Hi-Temp_sat_gain2
0xf840, //Hi-Temp_sat_gain3
0xf940, //Hi-Temp_sat_gain4
0xfa40, //Hi-Temp_sat_gain5
0xfb40, //Hi-Temp_sat_gain6
0xfc00, //Hi-Temp_hue_angle1
0xfd00, //Hi-Temp_hue_angle2

0x03d8, //Page d8
0x0e01, // burst start

0x1000, //Hi-Temp_hue_angle3
0x1100, //Hi-Temp_hue_angle4
0x1206, //Hi-Temp_hue_angle5
0x1300, //Hi-Temp_hue_angle6
0x1411, //Hi-Temp_rgb_ofs_sign_r
0x1511, //Hi-Temp_rgb_ofs_sign_g
0x1611, //Hi-Temp_rgb_ofs_sign_b
0x1700, //Hi-Temp_rgb_ofs_scl_r1
0x1800, //Hi-Temp_rgb_ofs_scl_g1
0x1900, //Hi-Temp_rgb_ofs_scl_b1
0x1a00, //Hi-Temp_rgb_ofs_scl_r2
0x1b00, //Hi-Temp_rgb_ofs_scl_g2
0x1c00, //Hi-Temp_rgb_ofs_scl_b2
0x1d00, //Hi-Temp_rgb_ofs_scl_r3
0x1e00, //Hi-Temp_rgb_ofs_scl_g3
0x1f00, //Hi-Temp_rgb_ofs_scl_b3
0x2000, //Hi-Temp_rgb_ofs_scl_r4
0x2100, //Hi-Temp_rgb_ofs_scl_g4
0x2200, //Hi-Temp_rgb_ofs_scl_b4
0x2300, //Hi-Temp_rgb_ofs_scl_r5
0x2400, //Hi-Temp_rgb_ofs_scl_g5
0x2500, //Hi-Temp_rgb_ofs_scl_b5
0x2600, //Hi-Temp_rgb_ofs_scl_r6
0x2700, //Hi-Temp_rgb_ofs_scl_g6
0x2800, //Hi-Temp_rgb_ofs_scl_b6
0x2900, //Hi-Temp_std_offset1
0x2a00, //Hi-Temp_std_offset2
0x2bff, //Hi-Temp_std_th_max
0x2c00, //Hi-Temp_std_th_min
0x2d3f, //Hi-Temp_rg1_lum_gain_wgt_th1
0x2e3f, //Hi-Temp_rg1_lum_gain_wgt_th2
0x2f3f, //Hi-Temp_rg1_lum_gain_wgt_th3
0x303f, //Hi-Temp_rg1_lum_gain_wgt_th4
0x3130, //Hi-Temp_rg1_lum_sp1
0x3250, //Hi-Temp_rg1_lum_sp2
0x3380, //Hi-Temp_rg1_lum_sp3
0x34b0, //Hi-Temp_rg1_lum_sp4
0x353f, //Hi-Temp_rg2_gain_wgt_th1
0x363f, //Hi-Temp_rg2_gain_wgt_th2
0x373f, //Hi-Temp_rg2_gain_wgt_th3
0x383f, //Hi-Temp_rg2_gain_wgt_th4
0x3928, //Hi-Temp_rg2_lum_sp1
0x3a50, //Hi-Temp_rg2_lum_sp2
0x3b80, //Hi-Temp_rg2_lum_sp3
0x3cb0, //Hi-Temp_rg2_lum_sp4
0x3d3f, //Hi-Temp_rg3_gain_wgt_th1
0x3e3f, //Hi-Temp_rg3_gain_wgt_th2
0x3f3f, //Hi-Temp_rg3_gain_wgt_th3
0x403f, //Hi-Temp_rg3_gain_wgt_th4
0x4128, //Hi-Temp_rg3_lum_sp1
0x4250, //Hi-Temp_rg3_lum_sp2
0x4380, //Hi-Temp_rg3_lum_sp3
0x44b0, //Hi-Temp_rg3_lum_sp4

0x453f, //Hi-Temp_rg4_gain_wgt_th1
0x463f, //Hi-Temp_rg4_gain_wgt_th2
0x473f, //Hi-Temp_rg4_gain_wgt_th3
0x483f, //Hi-Temp_rg4_gain_wgt_th4
0x4910, //Hi-Temp_rg4_lum_sp1
0x4a30, //Hi-Temp_rg4_lum_sp2
0x4b60, //Hi-Temp_rg4_lum_sp3
0x4c90, //Hi-Temp_rg4_lum_sp4

0x4d3f, //Hi-Temp_rg5_gain_wgt_th1
0x4e3f, //Hi-Temp_rg5_gain_wgt_th2
0x4f3f, //Hi-Temp_rg5_gain_wgt_th3
0x503f, //Hi-Temp_rg5_gain_wgt_th4
0x5128, //Hi-Temp_rg5_lum_sp1
0x5250, //Hi-Temp_rg5_lum_sp2
0x5380, //Hi-Temp_rg5_lum_sp3
0x54b0, //Hi-Temp_rg5_lum_sp4

0x553f, //Hi-Temp_rg6_gain_wgt_th1
0x563f, //Hi-Temp_rg6_gain_wgt_th2
0x573f, //Hi-Temp_rg6_gain_wgt_th3
0x583f, //Hi-Temp_rg6_gain_wgt_th4
0x5928, //Hi-Temp_rg6_lum_sp1
0x5a50, //Hi-Temp_rg6_lum_sp2
0x5b80, //Hi-Temp_rg6_lum_sp3
0x5cb0, //Hi-Temp_rg6_lum_sp4

0x5d19, //Hi-Temp_allgain_x1
0x5e37, //Hi-Temp_allgain_x2
0x5f37, //Hi-Temp_allgain_x3
0x6019, //Hi-Temp_allgain_x4
0x6119, //Hi-Temp_allgain_x5
0x6237, //Hi-Temp_allgain_x6
0x6337, //Hi-Temp_allgain_x7
0x6419, //Hi-Temp_allgain_x8

0x650e, //Hi-Temp_allgain_y0
0x660d, //Hi-Temp_allgain_y1
0x670e, //Hi-Temp_allgain_y2
0x680e, //Hi-Temp_allgain_y3
0x6910, //Hi-Temp_allgain_y4
0x6a10, //Hi-Temp_allgain_y5
0x6b13, //Hi-Temp_allgain_y6
0x6c12, //Hi-Temp_allgain_y7
0x6d13, //Hi-Temp_allgain_y8
0x6e12, //Hi-Temp_allgain_y9
0x6f0e, //Hi-Temp_allgain_y10
0x7011, //Hi-Temp_allgain_y11

0x0e00, // burst end

0x03D3,
0x11FE,	// Adaptive LSC on
0x108d,	// Adaptive on //B[1] EV with Y off for HD

///////////////////////////////////////////////////////////////////////////////
// DE ~ E0 Page (DMA Outdoor)
///////////////////////////////////////////////////////////////////////////////

0x03de, //DMA DE Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x1377, //Outdoor 1111 add 720p 
0x1414,
0x1500, //Outdoor 1114 add 720p 
0x1615,
0x1781, //Outdoor 1115 add 720p 
0x1816,
0x1904, //Outdoor 1116 add 720p 
0x1a17,
0x1b58, //Outdoor 1117 add 720p 
0x1c18,
0x1d30, //Outdoor 1118 add 720p 
0x1e19,
0x1f12, //Outdoor 1119 add 720p 
0x2037,
0x2100, //Outdoor 1137
0x2238,
0x2300, //Outdoor 1138
0x2439,
0x2500, //Outdoor 1139
0x263a,
0x2700, //Outdoor 113a
0x283b,
0x2900, //Outdoor 113b
0x2a3c,
0x2b00, //Outdoor 113c
0x2c3d,
0x2d00, //Outdoor 113d
0x2e3e,
0x2f00, //Outdoor 113e
0x303f,
0x3100, //Outdoor 113f
0x3240,
0x3300, //Outdoor 1140
0x3441,
0x3500, //Outdoor 1141
0x3642,
0x3700, //Outdoor 1142
0x3843,
0x3900, //Outdoor 1143
0x3a49,
0x3b06, //Outdoor 1149 add 720p 
0x3c4a,
0x3d0a, //Outdoor 114a add 720p 
0x3e4b,
0x3f12, //Outdoor 114b add 720p 
0x404c,
0x411c, //Outdoor 114c add 720p 
0x424d,
0x4324, //Outdoor 114d add 720p 
0x444e,
0x4540, //Outdoor 114e add 720p 
0x464f,
0x4780, //Outdoor 114f add 720p 
0x4850,
0x491a, //Outdoor 1150
0x4a51,
0x4b23, //Outdoor 1151
0x4c52,
0x4d2c, //Outdoor 1152
0x4e53,
0x4f3f, //Outdoor 1153
0x5054,
0x513f, //Outdoor 1154
0x5255,
0x533e, //Outdoor 1155
0x5456,
0x553c, //Outdoor 1156
0x5657,
0x573a, //Outdoor 1157
0x5858,
0x593f, //Outdoor 1158
0x5a59,
0x5b3f, //Outdoor 1159
0x5c5a,
0x5d3e, //Outdoor 115a
0x5e5b,
0x5f3a, //Outdoor 115b
0x605c,
0x6137, //Outdoor 115c
0x625d,
0x6334, //Outdoor 115d
0x645e,
0x6532, //Outdoor 115e
0x665f,
0x6730, //Outdoor 115f
0x686e,
0x691c, //Outdoor 116e
0x6a6f,
0x6b18, //Outdoor 116f
0x6c77,
0x6d2b, //Outdoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f2a, //Outdoor 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Outdoor 1179 //Bayer SP Lum Pos3
0x727a,
0x731a, //Outdoor 117a //Bayer SP Lum Pos4
0x747b,
0x751c, //Outdoor 117b //Bayer SP Lum Pos5
0x767c,
0x771a, //Outdoor 117c //Bayer SP Lum Pos6
0x787d,
0x7919, //Outdoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b17, //Outdoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1e, //Outdoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1e, //Outdoor 1180 //Bayer SP Lum Neg2
0x8081,
0x811f, //Outdoor 1181 //Bayer SP Lum Neg3
0x8282,
0x831e, //Outdoor 1182 //Bayer SP Lum Neg4
0x8483,
0x851a, //Outdoor 1183 //Bayer SP Lum Neg5
0x8684,
0x871a, //Outdoor 1184 //Bayer SP Lum Neg6
0x8885,
0x891a, //Outdoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b1a, //Outdoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d1a, //Outdoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f16, //Outdoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9116, //Outdoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9315, //Outdoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9517, //Outdoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9717, //Outdoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9917, //Outdoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b16, //Outdoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Outdoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f20, //Outdoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa123, //Outdoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa321, //Outdoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa521, //Outdoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa720, //Outdoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91b, //Outdoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab18, //Outdoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad2b,//Outdoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf2b,//Outdoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb12b,//Outdoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb32b,//Outdoor 11aa //Bayer SP Edge4
0xb4ab,
0xb52b,//Outdoor 11ab //Bayer SP Edge5
0xb6ac,
0xb72c,//Outdoor 11ac //Bayer SP Edge6
0xb8ad,
0xb931,//Outdoor 11ad //Bayer SP Edge7
0xbaae,
0xbb35,//Outdoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Outdoor 11b7 add 720p 
0xbeb8,
0xbf22, //Outdoor 11b8 add 720p 
0xc0b9,
0xc121, //Outdoor 11b9 add 720p 
0xc2ba,
0xc31e, //Outdoor 11ba add 720p 
0xc4bb,
0xc51c, //Outdoor 11bb add 720p 
0xc6bc,
0xc71a, //Outdoor 11bc add 720p 
0xc8c7,
0xc930,//Outdoor 11c7 //Bayer SP STD1
0xcac8,
0xcb30,//Outdoor 11c8 //Bayer SP STD2
0xccc9,
0xcd30,//Outdoor 11c9 //Bayer SP STD3
0xceca,
0xcf30,//Outdoor 11ca //Bayer SP STD4
0xd0cb,
0xd130,//Outdoor 11cb //Bayer SP STD5
0xd2cc,
0xd330,//Outdoor 11cc //Bayer SP STD6
0xd4cd,
0xd52d,//Outdoor 11cd //Bayer SP STD7
0xd6ce,
0xd72a,//Outdoor 11ce //Bayer SP STD8
0xd8cf,
0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb15,//Outdoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Outdoor 11d1
0xded2,
0xdf40, //Outdoor 11d2
0xe0d3,
0xe1ff, //Outdoor 11d3
0xe2d4,
0xe301, //Outdoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe50a, //Outdoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Outdoor 11d6
0xe8d7,
0xe910,//Outdoor 11d7
0xead8,
0xeb01, //Outdoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed06, //Outdoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Outdoor 11da
0xf0db,
0xf107, //Outdoor 11db
0xf2df,
0xf355, //Outdoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf536, //Outdoor 11e0
0xf6e1,
0xf77a, //Outdoor 11e1
0xf8e2,
0xf935, //Outdoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfba0, //Outdoor 11e3
0xfce4,
0xfd01, //Outdoor 11e4
0x0e00, // burst end

0x03df, //DMA DF Page
0x0e01, // burst start

0x10e5,
0x1120,//Outdoor 11e5
0x12e6,
0x1301, //Outdoor 11e6
0x14e7,
0x151a,//Outdoor 11e7
0x16e8,
0x1701, //Outdoor 11e8
0x18e9,
0x1910, //Outdoor 11e9
0x1aea,
0x1b01, //Outdoor 11ea
0x1ceb,
0x1d12, //Outdoor 11eb
0x1eef,
0x1f33, //Outdoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Outdoor 1240 add 720p 
0x2470,
0x259f, //Outdoor 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x271a, //Outdoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Outdoor 1272 //Bayer LPF Gain
0x2a77,
0x2b36, //Outdoor 1277
0x2c78,
0x2d2f, //Outdoor 1278
0x2e79,
0x2f09, //Outdoor 1279
0x307a,
0x3150, //Outdoor 127a
0x327b,
0x3310, //Outdoor 127b
0x347c,
0x3550, //Outdoor 127c //skin HPF gain
0x367d,
0x3710, //Outdoor 127d
0x387f,
0x3950, //Outdoor 127f
0x3a87,
0x3b3f, //Outdoor 1287 add 720p 
0x3c88,
0x3d3f, //Outdoor 1288 add 720p 
0x3e89,
0x3f3f, //Outdoor 1289 add 720p 
0x408a,
0x413f, //Outdoor 128a add 720p 
0x428b,
0x433f, //Outdoor 128b add 720p 
0x448c,
0x453f, //Outdoor 128c add 720p 
0x468d,
0x473f, //Outdoor 128d add 720p 
0x488e,
0x493f, //Outdoor 128e add 720p 
0x4a8f,
0x4b3f, //Outdoor 128f add 720p 
0x4c90,
0x4d3f, //Outdoor 1290 add 720p 
0x4e91,
0x4f3f, //Outdoor 1291 add 720p 
0x5092,
0x513f, //Outdoor 1292 add 720p 
0x5293,
0x533f, //Outdoor 1293 add 720p 
0x5494,
0x553f, //Outdoor 1294 add 720p 
0x5695,
0x573f, //Outdoor 1295 add 720p 
0x5896,
0x593f, //Outdoor 1296 add 720p 
0x5aae,
0x5b7f, //Outdoor 12ae
0x5caf,
0x5d63,//Outdoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Outdoor 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x613c, //Outdoor 12c3 add 720p 
0x62c4,
0x631a, //Outdoor 12c4 add 720p 
0x64c5,
0x650c, //Outdoor 12c5 add 720p 
0x66c6,
0x6791, //Outdoor 12c6
0x68c7,
0x69a4, //Outdoor 12c7
0x6ac8,
0x6b3c, //Outdoor 12c8
0x6cd0,
0x6d08, //Outdoor 12d0 add 720p 
0x6ed1,
0x6f10, //Outdoor 12d1 add 720p 
0x70d2,
0x7118, //Outdoor 12d2 add 720p 
0x72d3,
0x7320, //Outdoor 12d3 add 720p 
0x74d4,
0x7530, //Outdoor 12d4 add 720p 
0x76d5,
0x7760, //Outdoor 12d5 add 720p 
0x78d6,
0x7980, //Outdoor 12d6 add 720p 
0x7ad7,
0x7b30,//Outdoor 12d7 //CI LPF NR offset
0x7cd8,
0x7d33,//Outdoor 12d8
0x7ed9,
0x7f35,//Outdoor 12d9
0x80da,
0x8135,//Outdoor 12da
0x82db,
0x8334,//Outdoor 12db
0x84dc,
0x8530,//Outdoor 12dc
0x86dd,
0x872a,//Outdoor 12dd
0x88de,
0x8926,//Outdoor 12de
0x8ae0,
0x8b49, //Outdoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Outdoor 12e1
0x8ee2,
0x8f02, //Outdoor 12e2
0x90e3,
0x9120, //Outdoor 12e3 //PS LN graph Y1
0x92e4,
0x9320, //Outdoor 12e4 //PS LN graph Y2
0x94e5,
0x9520, //Outdoor 12e5 //PS LN graph Y3
0x96e6,
0x9720, //Outdoor 12e6 //PS LN graph Y4
0x98e7,
0x9920, //Outdoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b20, //Outdoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d20, //Outdoor 12e9 //PS DY graph Y1
0x9eea,
0x9f20, //Outdoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Outdoor 12eb //PS DY graph Y3
0xa2ec,
0xa31e, //Outdoor 12ec //PS DY graph Y4
0xa4ed,
0xa51d, //Outdoor 12ed //PS DY graph Y5
0xa6ee,
0xa720, //Outdoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Outdoor 12f0
0xaaf1,
0xab2a, //Outdoor 12f1
0xacf2,
0xad32, //Outdoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb183, //Outdoor 1310 //Y-NR ENB add  720p
0xb230,
0xb33f, //Outdoor 1330
0xb431,
0xb53f, //Outdoor 1331
0xb632,
0xb73f, //Outdoor 1332
0xb833,
0xb93f, //Outdoor 1333
0xba34,
0xbb3f, //Outdoor 1334
0xbc35,
0xbd33, //Outdoor 1335
0xbe36,
0xbf2f, //Outdoor 1336
0xc037,
0xc12e, //Outdoor 1337
0xc238,
0xc302, //Outdoor 1338
0xc440,
0xc51e, //Outdoor 1340
0xc641,
0xc722, //Outdoor 1341
0xc842,
0xc962, //Outdoor 1342
0xca43,
0xcb63, //Outdoor 1343
0xcc44,
0xcdff, //Outdoor 1344
0xce45,
0xcf04, //Outdoor 1345
0xd046,
0xd136, //Outdoor 1346
0xd247,
0xd305, //Outdoor 1347
0xd448,
0xd520, //Outdoor 1348
0xd649,
0xd702, //Outdoor 1349
0xd84a,
0xd922, //Outdoor 134a
0xda4b,
0xdb06, //Outdoor 134b
0xdc4c,
0xdd20, //Outdoor 134c
0xde83,
0xdf08, //Outdoor 1383
0xe084,
0xe108, //Outdoor 1384
0xe2b7,
0xe3fd, //Outdoor 13b7
0xe4b8,
0xe5a7, //Outdoor 13b8
0xe6b9,
0xe7fe, //Outdoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Outdoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Outdoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Outdoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Outdoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Outdoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Outdoor 1410
0xf611,
0xf7d8, //Outdoor 1411
0xf812,
0xf910, //Outdoor 1412
0xfa13,
0xfb03, //Outdoor 1413
0xfc14,
0xfd0f, //Outdoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x1015,
0x117b, //Outdoor 1415 // Y Hi filter mask 1/16
0x1216,
0x131c, //Outdoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Outdoor 1417
0x1618,
0x170c, //Outdoor 1418
0x1819,
0x190c, //Outdoor 1419
0x1a1a,
0x1b18, //Outdoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1d, //Outdoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f26, //Outdoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2124, //Outdoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2323, //Outdoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x251a, //Outdoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2714, //Outdoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Outdoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b16, //Outdoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d16, //Outdoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f22, //Outdoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3121, //Outdoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3320, //Outdoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x351a, //Outdoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3719, //Outdoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3915, //Outdoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b18, //Outdoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d1b, //Outdoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f24, //Outdoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Outdoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4324, //Outdoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x451a, //Outdoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4714, //Outdoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4910, //Outdoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b18, //Outdoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d18,//Outdoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f18, //Outdoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Outdoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x531b, //Outdoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5519, //Outdoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5719, //Outdoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x591e, //Outdoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b25, //Outdoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d25, //Outdoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f24, //Outdoor 1467 //YC2D SP Edge Gain1
0x6068,
0x6124, //Outdoor 1468 //YC2D SP Edge Gain2
0x6269,
0x6328, //Outdoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652e,//Outdoor 146a //YC2D SP Edge Gain4
0x666b,
0x6730,//Outdoor 146b //YC2D SP Edge Gain5
0x686c,
0x6931,//Outdoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b30,//Outdoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d28,//Outdoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f27, //Outdoor 1487 //YC2D SP STD Gain1
0x7088,
0x7128, //Outdoor 1488 //YC2D SP STD Gain2
0x7289,
0x732d, //Outdoor 1489 //YC2D SP STD Gain3
0x748a,
0x752d,//Outdoor 148a //YC2D SP STD Gain4
0x768b,
0x772e,//Outdoor 148b //YC2D SP STD Gain5
0x788c,
0x792b,//Outdoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b28,//Outdoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d25,//Outdoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Outdoor 1497 add 720p 
0x8098,
0x813f, //Outdoor 1498 add 720p 
0x8299,
0x833f, //Outdoor 1499 add 720p 
0x849a,
0x853f, //Outdoor 149a add 720p 
0x869b,
0x873f, //Outdoor 149b add 720p 
0x88a0,
0x893f, //Outdoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Outdoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Outdoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Outdoor 14a3 add 720p 
0x90a4,
0x913f, //Outdoor 14a4 add 720p 
0x92c9,
0x9313, //Outdoor 14c9
0x94ca,
0x9520, //Outdoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Outdoor 1A10 add 720p 
0x9a18,
0x9b3f, //Outdoor 1A18
0x9c19,
0x9d3f, //Outdoor 1A19
0x9e1a,
0x9f3f, //Outdoor 1A1a
0xa01b,
0xa13f, //Outdoor 1A1b
0xa21c,
0xa33f, //Outdoor 1A1c
0xa41d,
0xa53c, //Outdoor 1A1d
0xa61e,
0xa738, //Outdoor 1A1e
0xa81f,
0xa935, //Outdoor 1A1f
0xaa20,
0xabe7, //Outdoor 1A20 add
0xac2f,
0xadf1, //Outdoor 1A2f add
0xae32,
0xaf87, //Outdoor 1A32 add
0xb034,
0xb1d2, //Outdoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31c, //Outdoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb506,//Outdoor 1A36
0xb637,
0xb740, //Outdoor 1A37
0xb838,
0xb9ff, //Outdoor 1A38
0xba39,
0xbb2e, //Outdoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Outdoor 1A3a
0xbe3b,
0xbf01, //Outdoor 1A3b
0xc03c,
0xc10c, //Outdoor 1A3c
0xc23d,
0xc301, //Outdoor 1A3d
0xc43e,
0xc507, //Outdoor 1A3e
0xc63f,
0xc701, //Outdoor 1A3f
0xc840,
0xc90c, //Outdoor 1A40
0xca41,
0xcb01, //Outdoor 1A41
0xcc42,
0xcd07, //Outdoor 1A42
0xce43,
0xcf2b, //Outdoor 1A43
0xd04d,
0xd10e, //Outdoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd30e, //Outdoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd50e, //Outdoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd713, //Outdoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd917, //Outdoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb17, //Outdoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd17, //Outdoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf17, //Outdoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe114, //Outdoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe311, //Outdoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe510, //Outdoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Outdoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe912, //Outdoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb12, //Outdoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed12, //Outdoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef12, //Outdoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf10f, //Outdoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf30f, //Outdoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf510, //Outdoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf717, //Outdoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf919, //Outdoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb17, //Outdoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd16, //Outdoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E0 page.
0xD027,
0xD142,

0x03e0, //DMA E0 Page
0x0e01, // burst start

0x106c,
0x1116, //Outdoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1310, //Outdoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1516, //Outdoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1715, //Outdoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1912, //Outdoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Outdoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d13, //Outdoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f13, //Outdoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2113, //Outdoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2329, //Outdoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2529, //Outdoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2729, //Outdoor 1A7f //RGB SP Edge Gain3
0x2880,
0x292f,//Outdoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b2f,//Outdoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d2f,//Outdoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f27,//Outdoor 1A83 //RGB SP Edge Gain7
0x3084,
0x3125,//Outdoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3328, //Outdoor 1A9e //RGB SP STD Gain1
0x349f,
0x3528, //Outdoor 1A9f //RGB SP STD Gain2
0x36a0,
0x372f,//Outdoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x392e,//Outdoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b2d,//Outdoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d2d,//Outdoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f29,//Outdoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4125,//Outdoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4323,//Outdoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Outdoor 1Aa7 add
0x46a8,
0x473f, //Outdoor 1Aa8 add
0x48a9,
0x493f, //Outdoor 1Aa9 add
0x4aaa,
0x4b3f, //Outdoor 1Aaa add
0x4cab,
0x4d3f, //Outdoor 1Aab add
0x4eaf,
0x4f3f, //Outdoor 1Aaf add
0x50b0,
0x513f, //Outdoor 1Ab0 add
0x52b1,
0x533f, //Outdoor 1Ab1 add
0x54b2,
0x553f, //Outdoor 1Ab2 add
0x56b3,
0x573f, //Outdoor 1Ab3 add
0x58ca,
0x5900, //Outdoor 1Aca
0x5ae3,
0x5b13, //Outdoor 1Ae3 add
0x5ce4,
0x5d13, //Outdoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610f, //Outdoor 1070 Trans Func.   130108 Outdoor transFuc Flat graph
0x6271,
0x6300, //Outdoor 1071
0x6472,
0x6500, //Outdoor 1072
0x6673,
0x6700, //Outdoor 1073
0x6874,
0x6900, //Outdoor 1074
0x6a75,
0x6b00, //Outdoor 1075
0x6c76,
0x6d40,//Outdoor 1076
0x6e77,
0x6f40,//Outdoor 1077
0x7078,
0x7100,//Outdoor 1078
0x7279,
0x7340,//Outdoor 1079
0x747a,
0x7500,//Outdoor 107a
0x767b,
0x7740,//Outdoor 107b
0x787c,
0x7900,//Outdoor 107c
0x7a7d,
0x7b07, //Outdoor 107d
0x7c7e,
0x7d0f, //Outdoor 107e
0x7e7f,
0x7f1e, //Outdoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8330, //Outdoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Outdoor 031a (for sun-spot)
0x881b,
0x898c, //Outdoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Outdoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Outdoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9102, //Outdoor 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x9529, //Outdoor 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E1 ~ E3 Page (DMA Indoor)
///////////////////////////////////////////////////////////////////////////////

0x03e1, //DMA E1 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x137f, //Indoor 1111 add 720p 
0x1414,
0x1500, //Indoor 1114 add 720p 
0x1615,
0x1781, //Indoor 1115 add 720p 
0x1816,
0x1904, //Indoor 1116 add 720p
0x1a17,
0x1b58, //Indoor 1117 add 720p 
0x1c18,
0x1d30, //Indoor 1118 add 720p 
0x1e19,
0x1f12, //Indoor 1119 add 720p 
0x2037,
0x2101, //Indoor 1137 //Pre Flat rate
0x2238,
0x2300, //Indoor 1138
0x2439,
0x25ff, //Indoor 1139
0x263a,
0x2700, //Indoor 113a
0x283b,
0x2900, //Indoor 113b
0x2a3c,
0x2b00, //Indoor 113c
0x2c3d,
0x2d20, //Indoor 113d
0x2e3e,
0x2f00, //Indoor 113e
0x303f,
0x3100, //Indoor 113f
0x3240,
0x3300, //Indoor 1140
0x3441,
0x351e, //Indoor 1141
0x3642,
0x3700, //Indoor 1142
0x3843,
0x3900, //Indoor 1143
0x3a49,
0x3b06, //Indoor 1149 add 720p 
0x3c4a,
0x3d0a, //Indoor 114a add 720p 
0x3e4b,
0x3f12, //Indoor 114b add 720p 
0x404c,
0x411c, //Indoor 114c add 720p 
0x424d,
0x4324, //Indoor 114d add 720p 
0x444e,
0x4540, //Indoor 114e add 720p 
0x464f,
0x4780, //Indoor 114f add 720p 
0x4850,
0x493f, //Indoor 1150
0x4a51,
0x4b3f, //Indoor 1151
0x4c52,
0x4d3f, //Indoor 1152
0x4e53,
0x4f3d, //Indoor 1153
0x5054,
0x513c, //Indoor 1154
0x5255,
0x5338, //Indoor 1155
0x5456,
0x5536, //Indoor 1156
0x5657,
0x5734, //Indoor 1157
0x5858,
0x593f, //Indoor 1158
0x5a59,
0x5b3f, //Indoor 1159
0x5c5a,
0x5d3e, //Indoor 115a
0x5e5b,
0x5f38, //Indoor 115b
0x605c,
0x6133, //Indoor 115c
0x625d,
0x6331, //Indoor 115d
0x645e,
0x6530, //Indoor 115e
0x665f,
0x6730, //Indoor 115f
0x686e,
0x6920, //Indoor 116e
0x6a6f,
0x6b18, //Indoor 116f
0x6c77,
0x6d16, //Indoor 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f16, //Indoor 1178 //Bayer SP Lum Pos2
0x7079,
0x7115, //Indoor 1179 //Bayer SP Lum Pos3
0x727a,
0x7315, //Indoor 117a //Bayer SP Lum Pos4
0x747b,
0x7511, //Indoor 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Indoor 117c //Bayer SP Lum Pos6
0x787d,
0x7910, //Indoor 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b10, //Indoor 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d11, //Indoor 117f //Bayer SP Lum Neg1
0x7e80,
0x7f11, //Indoor 1180 //Bayer SP Lum Neg2
0x8081,
0x8111, //Indoor 1181 //Bayer SP Lum Neg3
0x8282,
0x8315, //Indoor 1182 //Bayer SP Lum Neg4
0x8483,
0x8516, //Indoor 1183 //Bayer SP Lum Neg5
0x8684,
0x8716, //Indoor 1184 //Bayer SP Lum Neg6
0x8885,
0x8916, //Indoor 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b16, //Indoor 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d15, //Indoor 118f //Bayer SP Dy Pos1
0x8e90,
0x8f15, //Indoor 1190 //Bayer SP Dy Pos2
0x9091,
0x9113, //Indoor 1191 //Bayer SP Dy Pos3
0x9292,
0x9313, //Indoor 1192 //Bayer SP Dy Pos4
0x9493,
0x9513, //Indoor 1193 //Bayer SP Dy Pos5
0x9694,
0x9713, //Indoor 1194 //Bayer SP Dy Pos6
0x9895,
0x9913, //Indoor 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Indoor 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d16, //Indoor 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f16, //Indoor 1198 //Bayer SP Dy Neg2
0xa099,
0xa117, //Indoor 1199 //Bayer SP Dy Neg3
0xa29a,
0xa317, //Indoor 119a //Bayer SP Dy Neg4
0xa49b,
0xa51a, //Indoor 119b //Bayer SP Dy Neg5
0xa69c,
0xa71a, //Indoor 119c //Bayer SP Dy Neg6
0xa89d,
0xa91a, //Indoor 119d //Bayer SP Dy Neg7
0xaa9e,
0xab19, //Indoor 119e //Bayer SP Dy Neg8
0xaca7,
0xad26, //Indoor 11a7 //Bayer SP Edge1
0xaea8,
0xaf26, //Indoor 11a8 //Bayer SP Edge2
0xb0a9,
0xb125, //Indoor 11a9 //Bayer SP Edge3
0xb2aa,
0xb325, //Indoor 11aa //Bayer SP Edge4
0xb4ab,
0xb525, //Indoor 11ab //Bayer SP Edge5
0xb6ac,
0xb725, //Indoor 11ac //Bayer SP Edge6
0xb8ad,
0xb925, //Indoor 11ad //Bayer SP Edge7
0xbaae,
0xbb24, //Indoor 11ae //Bayer SP Edge8
0xbcb7,
0xbd22, //Indoor 11b7 add 720p 
0xbeb8,
0xbf22, //Indoor 11b8 add 720p 
0xc0b9,
0xc121, //Indoor 11b9 add 720p 
0xc2ba,
0xc31e, //Indoor 11ba add 720p 
0xc4bb,
0xc51c, //Indoor 11bb add 720p 
0xc6bc,
0xc71a, //Indoor 11bc add 720p 
0xc8c7,
0xc920, //Indoor 11c7 //Bayer SP STD1
0xcac8,
0xcb21, //Indoor 11c8 //Bayer SP STD2
0xccc9,
0xcd22, //Indoor 11c9 //Bayer SP STD3
0xceca,
0xcf24, //Indoor 11ca //Bayer SP STD4
0xd0cb,
0xd124, //Indoor 11cb //Bayer SP STD5
0xd2cc,
0xd324, //Indoor 11cc //Bayer SP STD6
0xd4cd,
0xd520, //Indoor 11cd //Bayer SP STD7
0xd6ce,
0xd71f, //Indoor 11ce //Bayer SP STD8
0xd8cf,
0xd965, //Indoor 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb18, //Indoor 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd3f, //Indoor 11d1
0xded2,
0xdf40, //Indoor 11d2
0xe0d3,
0xe1ff, //Indoor 11d3
0xe2d4,
0xe302, //Indoor 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe51c, //Indoor 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe701, //Indoor 11d6
0xe8d7,
0xe910, //Indoor 11d7
0xead8,
0xeb01, //Indoor 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed0e, //Indoor 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef01, //Indoor 11da
0xf0db,
0xf107, //Indoor 11db
0xf2df,
0xf3cc, //Indoor 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf532, //Indoor 11e0
0xf6e1,
0xf77a, //Indoor 11e1
0xf8e2,
0xf900, //Indoor 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfb00, //Indoor 11e3
0xfce4,
0xfd01, //Indoor 11e4 //Bayer Flat R3 StdL
0x0e00, // burst end

0x03e2, //DMA E2 Page
0x0e01, // burst start

0x10e5,
0x112a, //Indoor 11e5 //Bayer Flat R4 StdH
0x12e6,
0x1300, //Indoor 11e6
0x14e7,
0x1500, //Indoor 11e7
0x16e8,
0x1701, //Indoor 11e8
0x18e9,
0x191d, //Indoor 11e9
0x1aea,
0x1b00, //Indoor 11ea
0x1ceb,
0x1d00, //Indoor 11eb
0x1eef,
0x1fac, //Indoor 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2337, //Indoor 1240 add 720p 
0x2470,
0x259f, //Indoor 1270 // Bayer Sharpness ENB add
0x2671,
0x271a, //Indoor 1271 //Bayer HPF Gain
0x2872,
0x2916, //Indoor 1272 //Bayer LPF Gain
0x2a77,
0x2b26, //Indoor 1277 //20130412
0x2c78,
0x2d2f, //Indoor 1278
0x2e79,
0x2fff, //Indoor 1279
0x307a,
0x3150, //Indoor 127a
0x327b,
0x3310, //Indoor 127b
0x347c,
0x3564, //Indoor 127c //skin HPF gain
0x367d,
0x3720, //Indoor 127d
0x387f,
0x3950, //Indoor 127f
0x3a87,
0x3b3f, //Indoor 1287 add 720p 
0x3c88,
0x3d3f, //Indoor 1288 add 720p 
0x3e89,
0x3f3f, //Indoor 1289 add 720p 
0x408a,
0x413f, //Indoor 128a add 720p 
0x428b,
0x433f, //Indoor 128b add 720p 
0x448c,
0x453f, //Indoor 128c add 720p 
0x468d,
0x473f, //Indoor 128d add 720p 
0x488e,
0x493f, //Indoor 128e add 720p 
0x4a8f,
0x4b3f, //Indoor 128f add 720p 
0x4c90,
0x4d3f, //Indoor 1290 add 720p 
0x4e91,
0x4f3f, //Indoor 1291 add 720p 
0x5092,
0x513f, //Indoor 1292 add 720p 
0x5293,
0x533f, //Indoor 1293 add 720p 
0x5494,
0x553f, //Indoor 1294 add 720p 
0x5695,
0x573f, //Indoor 1295 add 720p 
0x5896,
0x593f, //Indoor 1296 add 720p 
0x5aae,
0x5b5f, //Indoor 12ae //Bayer Flat off
0x5caf,
0x5d80, //Indoor 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Indoor 12c0 // CI-LPF ENB add 720p
0x60c3,
0x613c, //Indoor 12c3 add 720p 
0x62c4,
0x631a, //Indoor 12c4 add 720p 
0x64c5,
0x650c, //Indoor 12c5 add 720p 
0x66c6,
0x6791, //Indoor 12c6
0x68c7,
0x69a4, //Indoor 12c7
0x6ac8,
0x6b3c, //Indoor 12c8
0x6cd0,
0x6d08, //Indoor 12d0 add 720p 
0x6ed1,
0x6f10, //Indoor 12d1 add 720p 
0x70d2,
0x7118, //Indoor 12d2 add 720p 
0x72d3,
0x7320, //Indoor 12d3 add 720p 
0x74d4,
0x7530, //Indoor 12d4 add 720p 
0x76d5,
0x7760, //Indoor 12d5 add 720p 
0x78d6,
0x7980, //Indoor 12d6 add 720p 
0x7ad7,
0x7b38, //Indoor 12d7
0x7cd8,
0x7d30, //Indoor 12d8
0x7ed9,
0x7f2a, //Indoor 12d9
0x80da,
0x812a, //Indoor 12da
0x82db,
0x8324, //Indoor 12db
0x84dc,
0x8520, //Indoor 12dc
0x86dd,
0x871a, //Indoor 12dd
0x88de,
0x8916, //Indoor 12de
0x8ae0,
0x8b63, //Indoor 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Indoor 12e1
0x8ee2,
0x8f02, //Indoor 12e2
0x90e3,
0x9110, //Indoor 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Indoor 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Indoor 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Indoor 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Indoor 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Indoor 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Indoor 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Indoor 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Indoor 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Indoor 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Indoor 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Indoor 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Indoor 12f0
0xaaf1,
0xab2a, //Indoor 12f1
0xacf2,
0xad32, //Indoor 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb183, //Indoor 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Indoor 1330
0xb431,
0xb520, //Indoor 1331
0xb632,
0xb720, //Indoor 1332
0xb833,
0xb920, //Indoor 1333
0xba34,
0xbb20, //Indoor 1334
0xbc35,
0xbd20, //Indoor 1335
0xbe36,
0xbf20, //Indoor 1336
0xc037,
0xc120, //Indoor 1337
0xc238,
0xc302, //Indoor 1338
0xc440,
0xc518, //Indoor 1340
0xc641,
0xc736, //Indoor 1341
0xc842,
0xc962, //Indoor 1342
0xca43,
0xcb63, //Indoor 1343
0xcc44,
0xcdff, //Indoor 1344
0xce45,
0xcf04, //Indoor 1345
0xd046,
0xd145, //Indoor 1346
0xd247,
0xd305, //Indoor 1347
0xd448,
0xd565, //Indoor 1348
0xd649,
0xd702, //Indoor 1349
0xd84a,
0xd922, //Indoor 134a
0xda4b,
0xdb06, //Indoor 134b
0xdc4c,
0xdd30, //Indoor 134c
0xde83,
0xdf08, //Indoor 1383 //add 20121210
0xe084,
0xe10a, //Indoor 1384 //add 20121210
0xe2b7,
0xe3fa, //Indoor 13b7
0xe4b8,
0xe577, //Indoor 13b8
0xe6b9,
0xe7fe, //Indoor 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ca, //Indoor 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Indoor 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Indoor 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Indoor 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Indoor 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf5b3, //Indoor 1410
0xf611,
0xf798, //Indoor 1411
0xf812,
0xf910, //Indoor 1412
0xfa13,
0xfb03, //Indoor 1413
0xfc14,
0xfd17, //Indoor 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x1015,
0x117b, //Indoor 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Indoor 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Indoor 1417
0x1618,
0x170c, //Indoor 1418
0x1819,
0x190c, //Indoor 1419
0x1a1a,
0x1b18, //Indoor 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d20, //Indoor 141b //YC2D Post STD gain Neg
0x1e27,
0x1f22, //Indoor 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2121, //Indoor 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x231c, //Indoor 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2515, //Indoor 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2711, //Indoor 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2910, //Indoor 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b11, //Indoor 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Indoor 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f1a, //Indoor 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3115, //Indoor 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3314, //Indoor 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3513, //Indoor 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3712, //Indoor 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3912, //Indoor 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b14, //Indoor 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d15, //Indoor 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f22, //Indoor 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x4122, //Indoor 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x4321, //Indoor 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4520, //Indoor 144a //YC2D SP Dy Gain Pos4
0x464b,
0x471d, //Indoor 144b //YC2D SP Dy Gain Pos5
0x484c,
0x491d, //Indoor 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b1d, //Indoor 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d1d, //Indoor 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f19, //Indoor 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5118, //Indoor 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5316, //Indoor 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5515, //Indoor 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5714, //Indoor 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Indoor 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b13, //Indoor 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d12, //Indoor 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f2a, //Indoor 1467 //YC2D SP Edge Gain1
0x6068,
0x612b, //Indoor 1468 //YC2D SP Edge Gain2
0x6269,
0x632c, //Indoor 1469 //YC2D SP Edge Gain3
0x646a,
0x652d, //Indoor 146a //YC2D SP Edge Gain4
0x666b,
0x672d, //Indoor 146b //YC2D SP Edge Gain5
0x686c,
0x692b, //Indoor 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b2d, //Indoor 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d2e, //Indoor 146e //YC2D SP Edge Gain8
0x6e87,
0x6f1c, //Indoor 1487 //YC2D SP STD Gain1
0x7088,
0x7120, //Indoor 1488 //YC2D SP STD Gain2
0x7289,
0x7326, //Indoor 1489 //YC2D SP STD Gain3
0x748a,
0x7526, //Indoor 148a //YC2D SP STD Gain4
0x768b,
0x7722, //Indoor 148b //YC2D SP STD Gain5
0x788c,
0x7922, //Indoor 148c //YC2D SP STD Gain6
0x7a8d,
0x7b1c, //Indoor 148d //YC2D SP STD Gain7
0x7c8e,
0x7d18, //Indoor 148e //YC2D SP STD Gain8
0x7e97,
0x7f3f, //Indoor 1497 add 720p 
0x8098,
0x813f, //Indoor 1498 add 720p 
0x8299,
0x833f, //Indoor 1499 add 720p 
0x849a,
0x853f, //Indoor 149a add 720p 
0x869b,
0x873f, //Indoor 149b add 720p 
0x88a0,
0x893f, //Indoor 14a0 add 720p 
0x8aa1,
0x8b3f, //Indoor 14a1 add 720p 
0x8ca2,
0x8d3f, //Indoor 14a2 add 720p 
0x8ea3,
0x8f3f, //Indoor 14a3 add 720p 
0x90a4,
0x913f, //Indoor 14a4 add 720p 
0x92c9,
0x9313, //Indoor 14c9
0x94ca,
0x9527, //Indoor 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9915, //Indoor 1A10 add 720p 
0x9a18,
0x9b3f, //Indoor 1A18
0x9c19,
0x9d3f, //Indoor 1A19
0x9e1a,
0x9f2a, //Indoor 1A1a
0xa01b,
0xa127, //Indoor 1A1b
0xa21c,
0xa323, //Indoor 1A1c
0xa41d,
0xa523, //Indoor 1A1d
0xa61e,
0xa723, //Indoor 1A1e
0xa81f,
0xa923, //Indoor 1A1f
0xaa20,
0xabe7, //Indoor 1A20 add 720p 
0xac2f,
0xadf1, //Indoor 1A2f add 720p 
0xae32,
0xaf87, //Indoor 1A32 add 720p 
0xb034,
0xb1d0, //Indoor 1A34 //RGB High Gain B[5:0]
0xb235,
0xb311, //Indoor 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb500, //Indoor 1A36
0xb637,
0xb740, //Indoor 1A37
0xb838,
0xb9ff, //Indoor 1A38
0xba39,
0xbb1d, //Indoor 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Indoor 1A3a //RGB Flat R2_Lum H
0xbe3b,
0xbf00, //Indoor 1A3b
0xc03c,
0xc14c, //Indoor 1A3c
0xc23d,
0xc300, //Indoor 1A3d
0xc43e,
0xc513, //Indoor 1A3e
0xc63f,
0xc700, //Indoor 1A3f
0xc840,
0xc92a, //Indoor 1A40
0xca41,
0xcb00, //Indoor 1A41
0xcc42,
0xcd17, //Indoor 1A42
0xce43,
0xcf2c, //Indoor 1A43
0xd04d,
0xd112, //Indoor 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd312, //Indoor 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd511, //Indoor 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd710, //Indoor 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd910, //Indoor 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Indoor 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd10, //Indoor 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf10, //Indoor 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe113, //Indoor 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe313, //Indoor 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe512, //Indoor 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe710, //Indoor 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe910, //Indoor 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Indoor 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed10, //Indoor 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef10, //Indoor 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf112, //Indoor 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf312, //Indoor 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf513, //Indoor 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf714, //Indoor 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf914, //Indoor 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb14, //Indoor 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd15, //Indoor 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E3 page.
0xD62A,
0xD7FA,

0x03e3, //DMA E3 Page
0x0e01, // burst start

0x106c,
0x1115, //Indoor 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x1312, //Indoor 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x1512, //Indoor 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x1712, //Indoor 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x1913, //Indoor 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b13, //Indoor 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d14, //Indoor 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Indoor 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2114, //Indoor 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2322, //Indoor 1A7d //RGB SP Edge Gain1
0x247e,
0x2523, //Indoor 1A7e //RGB SP Edge Gain2
0x267f,
0x2724, //Indoor 1A7f //RGB SP Edge Gain3
0x2880,
0x2925, //Indoor 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b26, //Indoor 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d27, //Indoor 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f29, //Indoor 1A83 //RGB SP Edge Gain7
0x3084,
0x312b, //Indoor 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Indoor 1A9e //RGB SP STD Gain1
0x349f,
0x3523, //Indoor 1A9f //RGB SP STD Gain2
0x36a0,
0x3726, //Indoor 1Aa0 //RGB SP STD Gain3
0x38a1,
0x3926, //Indoor 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b26, //Indoor 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d26, //Indoor 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f26, //Indoor 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4126, //Indoor 1Aa5 //RGB SP STD Gain8
0x42a6,
0x4334, //Indoor 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x453f, //Indoor 1Aa7 add 720p 
0x46a8,
0x473f, //Indoor 1Aa8 add 720p 
0x48a9,
0x493f, //Indoor 1Aa9 add 720p 
0x4aaa,
0x4b3f, //Indoor 1Aaa add 720p 
0x4cab,
0x4d3f, //Indoor 1Aab add 720p 
0x4eaf,
0x4f3f, //Indoor 1Aaf add 720p 
0x50b0,
0x513f, //Indoor 1Ab0 add 720p 
0x52b1,
0x533f, //Indoor 1Ab1 add 720p 
0x54b2,
0x553f, //Indoor 1Ab2 add 720p 
0x56b3,
0x573f, //Indoor 1Ab3 add 720p 
0x58ca,
0x5900, //Indoor 1Aca
0x5ae3,
0x5b13, //Indoor 1Ae3 add
0x5ce4,
0x5d13, //Indoor 1Ae4 add
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Indoor 1070 Trans Func.   130108 Indoor transFuc Flat graph
0x6271,
0x6301, //Indoor 1071
0x6472,
0x6589, //Indoor 1072
0x6673,
0x67d4, //Indoor 1073
0x6874,
0x6900, //Indoor 1074
0x6a75,
0x6b00, //Indoor 1075
0x6c76,
0x6d40, //Indoor 1076
0x6e77,
0x6f47, //Indoor 1077
0x7078,
0x71ae, //Indoor 1078
0x7279,
0x7350, //Indoor 1079
0x747a,
0x7500, //Indoor 107a
0x767b,
0x7750, //Indoor 107b
0x787c,
0x7900, //Indoor 107c
0x7a7d,
0x7b05, //Indoor 107d
0x7c7e,
0x7d0f, //Indoor 107e
0x7e7f,
0x7f2f, //Indoor 107f
0x8003,
0x8102, // 2 page
0x8223,
0x832a, //Indoor 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8700, //Indoor 031a (for sun-spot)
0x881b,
0x898c, //Indoor 031b (for sun-spot)
0x8a1c,
0x8b02, //Indoor 031c (for sun-spot)
0x8c1d,
0x8d88, //Indoor 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9103, //Indoor 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x9529, //Indoor 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E4 ~ E6 Page (DMA Dark1)
///////////////////////////////////////////////////////////////////////////////

0x03e4, //DMA E4 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x13ff, //Dark1 1111 add 720p 
0x1414,
0x1500, //Dark1 1114 add 720p 
0x1615,
0x1700, //Dark1 1115 add 720p 
0x1816,
0x1900, //Dark1 1116 add 720p 
0x1a17,
0x1b1e, //Dark1 1117 add 720p 
0x1c18,
0x1d10, //Dark1 1118 add 720p 
0x1e19,
0x1f06, //Dark1 1119 add 720p 
0x2037,
0x2101, //Dark1 1137 //Pre Flat rate B[4:1]
0x2238,
0x2300, //Dark1 1138 //Pre Flat R1 LumL
0x2439,
0x2503, //Dark1 1139 //Pre Flat R1 LumH
0x263a,
0x2703, //Dark1 113a
0x283b,
0x29ff, //Dark1 113b
0x2a3c,
0x2b00, //Dark1 113c
0x2c3d,
0x2d13, //Dark1 113d
0x2e3e,
0x2f00, //Dark1 113e
0x303f,
0x3110, //Dark1 113f
0x3240,
0x3300, //Dark1 1140
0x3441,
0x3510, //Dark1 1141
0x3642,
0x3700, //Dark1 1142
0x3843,
0x3918, //Dark1 1143
0x3a49,
0x3b02, //Dark1 1149 add 720p 
0x3c4a,
0x3d04, //Dark1 114a add 720p 
0x3e4b,
0x3f07, //Dark1 114b add 720p 
0x404c,
0x410c, //Dark1 114c add 720p 
0x424d,
0x4310, //Dark1 114d add 720p 
0x444e,
0x4518, //Dark1 114e add 720p 
0x464f,
0x4720, //Dark1 114f add 720p 
0x4850,
0x491a, //Dark1 1150
0x4a51,
0x4b1c, //Dark1 1151
0x4c52,
0x4d1e, //Dark1 1152
0x4e53,
0x4f24, //Dark1 1153
0x5054,
0x5128, //Dark1 1154
0x5255,
0x5326, //Dark1 1155
0x5456,
0x5522, //Dark1 1156
0x5657,
0x571e, //Dark1 1157
0x5858,
0x593f, //Dark1 1158
0x5a59,
0x5b3f, //Dark1 1159
0x5c5a,
0x5d3f, //Dark1 115a
0x5e5b,
0x5f3f, //Dark1 115b
0x605c,
0x613f, //Dark1 115c
0x625d,
0x633f, //Dark1 115d
0x645e,
0x653f, //Dark1 115e
0x665f,
0x673f, //Dark1 115f
0x686e,
0x6910, //Dark1 116e
0x6a6f,
0x6b10, //Dark1 116f
0x6c77,
0x6d20, //Dark1 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f1e, //Dark1 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Dark1 1179 //Bayer SP Lum Pos3
0x727a,
0x7318, //Dark1 117a //Bayer SP Lum Pos4
0x747b,
0x7514, //Dark1 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Dark1 117c //Bayer SP Lum Pos6
0x787d,
0x7908, //Dark1 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b08, //Dark1 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1c, //Dark1 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1c, //Dark1 1180 //Bayer SP Lum Neg2
0x8081,
0x811c, //Dark1 1181 //Bayer SP Lum Neg3
0x8282,
0x8318, //Dark1 1182 //Bayer SP Lum Neg4
0x8483,
0x8514, //Dark1 1183 //Bayer SP Lum Neg5
0x8684,
0x8710, //Dark1 1184 //Bayer SP Lum Neg6
0x8885,
0x8908, //Dark1 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b08, //Dark1 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d20, //Dark1 118f //Bayer SP Dy Pos1
0x8e90,
0x8f1e, //Dark1 1190 //Bayer SP Dy Pos2
0x9091,
0x911c, //Dark1 1191 //Bayer SP Dy Pos3
0x9292,
0x931a, //Dark1 1192 //Bayer SP Dy Pos4
0x9493,
0x9516, //Dark1 1193 //Bayer SP Dy Pos5
0x9694,
0x9714, //Dark1 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark1 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Dark1 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d1d, //Dark1 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f1d, //Dark1 1198 //Bayer SP Dy Neg2
0xa099,
0xa11c, //Dark1 1199 //Bayer SP Dy Neg3
0xa29a,
0xa31a, //Dark1 119a //Bayer SP Dy Neg4
0xa49b,
0xa516, //Dark1 119b //Bayer SP Dy Neg5
0xa69c,
0xa714, //Dark1 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark1 119d //Bayer SP Dy Neg7
0xaa9e,
0xab10, //Dark1 119e //Bayer SP Dy Neg8
0xaca7,
0xad18, //Dark1 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark1 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark1 11a9 //Bayer SP Edge3
0xb2aa,
0xb315, //Dark1 11aa //Bayer SP Edge4
0xb4ab,
0xb512, //Dark1 11ab //Bayer SP Edge5
0xb6ac,
0xb710, //Dark1 11ac //Bayer SP Edge6
0xb8ad,
0xb910, //Dark1 11ad //Bayer SP Edge7
0xbaae,
0xbb10, //Dark1 11ae //Bayer SP Edge8
0xbcb7,
0xbd18, //Dark1 11b7 add 720p 
0xbeb8,
0xbf10, //Dark1 11b8 add 720p 
0xc0b9,
0xc108, //Dark1 11b9 add 720p 
0xc2ba,
0xc308, //Dark1 11ba add 720p 
0xc4bb,
0xc508, //Dark1 11bb add 720p 
0xc6bc,
0xc708, //Dark1 11bc add 720p 
0xc8c7,
0xc91c, //Dark1 11c7 //Bayer SP STD1
0xcac8,
0xcb1c, //Dark1 11c8 //Bayer SP STD2
0xccc9,
0xcd1c, //Dark1 11c9 //Bayer SP STD3
0xceca,
0xcf1a, //Dark1 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark1 11cb //Bayer SP STD5
0xd2cc,
0xd316, //Dark1 11cc //Bayer SP STD6
0xd4cd,
0xd514, //Dark1 11cd //Bayer SP STD7
0xd6ce,
0xd712, //Dark1 11ce //Bayer SP STD8
0xd8cf,
0xd922, //Dark1 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark1 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd04, //Dark1 11d1
0xded2,
0xdf1a, //Dark1 11d2
0xe0d3,
0xe123, //Dark1 11d3
0xe2d4,
0xe300, //Dark1 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe516, //Dark1 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark1 11d6
0xe8d7,
0xe91c, //Dark1 11d7
0xead8,
0xeb00, //Dark1 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed08, //Dark1 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark1 11da
0xf0db,
0xf10e, //Dark1 11db
0xf2df,
0xf373, //Dark1 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf504, //Dark1 11e0
0xf6e1,
0xf71a, //Dark1 11e1
0xf8e2,
0xf900, //Dark1 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfbff, //Dark1 11e3
0xfce4,
0xfd00, //Dark1 11e4
0x0e00, // burst end

0x03e5, //DMA E5 Page
0x0e01, // burst start

0x10e5,
0x1118, //Dark1 11e5
0x12e6,
0x1300, //Dark1 11e6
0x14e7,
0x1528, //Dark1 11e7
0x16e8,
0x1700, //Dark1 11e8
0x18e9,
0x1909, //Dark1 11e9
0x1aea,
0x1b00, //Dark1 11ea
0x1ceb,
0x1d14, //Dark1 11eb
0x1eef,
0x1f33, //Dark1 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2336, //Dark1 1240 add 720p 
0x2470,
0x2581, //Dark1 1270 // Bayer Sharpness ENB add 720p 
0x2671,
0x2707, //Dark1 1271 //Bayer HPF Gain
0x2872,
0x2907, //Dark1 1272 //Bayer LPF Gain
0x2a77,
0x2b00, //Dark1 1277
0x2c78,
0x2d09, //Dark1 1278
0x2e79,
0x2f2e, //Dark1 1279
0x307a,
0x3150, //Dark1 127a
0x327b,
0x3310, //Dark1 127b
0x347c,
0x3550, //Dark1 127c //skin HPF gain
0x367d,
0x3710, //Dark1 127d
0x387f,
0x3950, //Dark1 127f
0x3a87,
0x3b08, //Dark1 1287 add 720p 
0x3c88,
0x3d08, //Dark1 1288 add 720p 
0x3e89,
0x3f08, //Dark1 1289 add 720p 
0x408a,
0x410c, //Dark1 128a add 720p 
0x428b,
0x4310, //Dark1 128b add 720p 
0x448c,
0x4514, //Dark1 128c add 720p 
0x468d,
0x4718, //Dark1 128d add 720p 
0x488e,
0x491a, //Dark1 128e add 720p 
0x4a8f,
0x4b08, //Dark1 128f add 720p 
0x4c90,
0x4d0a, //Dark1 1290 add 720p 
0x4e91,
0x4f0e, //Dark1 1291 add 720p 
0x5092,
0x5112, //Dark1 1292 add 720p 
0x5293,
0x5316, //Dark1 1293 add 720p 
0x5494,
0x551a, //Dark1 1294 add 720p 
0x5695,
0x5720, //Dark1 1295 add 720p 
0x5896,
0x5920, //Dark1 1296 add 720p 
0x5aae,
0x5b20, //Dark1 12ae
0x5caf,
0x5d33, //Dark1 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark1 12c0 // CI-LPF ENB add 720p 
0x60c3,
0x6118, //Dark1 12c3 add 720p 
0x62c4,
0x630d, //Dark1 12c4 add 720p 
0x64c5,
0x6506, //Dark1 12c5 add 720p 
0x66c6,
0x6711, //Dark1 12c6
0x68c7,
0x6911, //Dark1 12c7
0x6ac8,
0x6b04, //Dark1 12c8
0x6cd0,
0x6d02, //Dark1 12d0 add 720p 
0x6ed1,
0x6f04, //Dark1 12d1 add 720p 
0x70d2,
0x7107, //Dark1 12d2 add 720p 
0x72d3,
0x730c, //Dark1 12d3 add 720p 
0x74d4,
0x7510, //Dark1 12d4 add 720p 
0x76d5,
0x7718, //Dark1 12d5 add 720p 
0x78d6,
0x7920, //Dark1 12d6 add 720p 
0x7ad7,
0x7b29, //Dark1 12d7 //CI LPF Lum offset start
0x7cd8,
0x7d2a, //Dark1 12d8
0x7ed9,
0x7f2c, //Dark1 12d9
0x80da,
0x812b, //Dark1 12da
0x82db,
0x832a, //Dark1 12db
0x84dc,
0x8528, //Dark1 12dc
0x86dd,
0x8727, //Dark1 12dd
0x88de,
0x8927, //Dark1 12de //CI LPF Lum offset end
0x8ae0,
0x8b63, //Dark1 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark1 12e1
0x8ee2,
0x8f02, //Dark1 12e2
0x90e3,
0x9110, //Dark1 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark1 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark1 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark1 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark1 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark1 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark1 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark1 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark1 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark1 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark1 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark1 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark1 12f0
0xaaf1,
0xab2a, //Dark1 12f1
0xacf2,
0xad32, //Dark1 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb180, //Dark1 1310 //Y-NR ENB add 720p 
0xb230,
0xb320, //Dark1 1330
0xb431,
0xb520, //Dark1 1331
0xb632,
0xb720, //Dark1 1332
0xb833,
0xb920, //Dark1 1333
0xba34,
0xbb20, //Dark1 1334
0xbc35,
0xbd2d, //Dark1 1335
0xbe36,
0xbf20, //Dark1 1336
0xc037,
0xc120, //Dark1 1337
0xc238,
0xc302, //Dark1 1338
0xc440,
0xc500, //Dark1 1340
0xc641,
0xc713, //Dark1 1341
0xc842,
0xc962, //Dark1 1342
0xca43,
0xcb63, //Dark1 1343
0xcc44,
0xcd7e, //Dark1 1344
0xce45,
0xcf00, //Dark1 1345
0xd046,
0xd16b, //Dark1 1346
0xd247,
0xd300, //Dark1 1347
0xd448,
0xd54a, //Dark1 1348
0xd649,
0xd700, //Dark1 1349
0xd84a,
0xd943, //Dark1 134a
0xda4b,
0xdb00, //Dark1 134b
0xdc4c,
0xdd2e, //Dark1 134c
0xde83,
0xdf08, //Dark1 1383
0xe084,
0xe10a, //Dark1 1384
0xe2b7,
0xe3ff, //Dark1 13b7
0xe4b8,
0xe5ff, //Dark1 13b8
0xe6b9,
0xe7ff, //Dark1 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ff, //Dark1 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark1 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark1 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark1 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark1 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf501, //Dark1 1410
0xf611,
0xf7d8, //Dark1 1411
0xf812,
0xf910, //Dark1 1412
0xfa13,
0xfb05, //Dark1 1413
0xfc14,
0xfd14, //Dark1 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x1015,
0x117d, //Dark1 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Dark1 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark1 1417
0x1618,
0x170c, //Dark1 1418
0x1819,
0x190c, //Dark1 1419
0x1a1a,
0x1b1c, //Dark1 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Dark1 141b //YC2D Post STD gain Neg
0x1e27,
0x1f0f, //Dark1 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2110, //Dark1 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2311, //Dark1 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2512, //Dark1 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2713, //Dark1 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark1 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b13, //Dark1 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Dark1 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f0f, //Dark1 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3110, //Dark1 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3311, //Dark1 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark1 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3713, //Dark1 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3913, //Dark1 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark1 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d10, //Dark1 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f1c, //Dark1 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x411b, //Dark1 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x431a, //Dark1 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4518, //Dark1 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4716, //Dark1 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark1 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b12, //Dark1 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d10, //Dark1 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f1a, //Dark1 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5119, //Dark1 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5318, //Dark1 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5517, //Dark1 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5716, //Dark1 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Dark1 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark1 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d10, //Dark1 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f10, //Dark1 1467 //YC2D SP Edge Gain1
0x6068,
0x6113, //Dark1 1468 //YC2D SP Edge Gain2
0x6269,
0x6313, //Dark1 1469 //YC2D SP Edge Gain3
0x646a,
0x6514, //Dark1 146a //YC2D SP Edge Gain4
0x666b,
0x6716, //Dark1 146b //YC2D SP Edge Gain5
0x686c,
0x6916, //Dark1 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b15, //Dark1 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d13, //Dark1 146e //YC2D SP Edge Gain8
0x6e87,
0x6f19, //Dark1 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Dark1 1488 //YC2D SP STD Gain2
0x7289,
0x731c, //Dark1 1489 //YC2D SP STD Gain3
0x748a,
0x751b, //Dark1 148a //YC2D SP STD Gain4
0x768b,
0x771a, //Dark1 148b //YC2D SP STD Gain5
0x788c,
0x791c, //Dark1 148c //YC2D SP STD Gain6
0x7a8d,
0x7b25, //Dark1 148d //YC2D SP STD Gain7
0x7c8e,
0x7d29, //Dark1 148e //YC2D SP STD Gain8
0x7e97,
0x7f08, //Dark1 1497 add 720p 
0x8098,
0x810c, //Dark1 1498 add 720p 
0x8299,
0x8310, //Dark1 1499 add 720p 
0x849a,
0x8510, //Dark1 149a add 720p 
0x869b,
0x8710, //Dark1 149b add 720p 
0x88a0,
0x8908, //Dark1 14a0 add 720p 
0x8aa1,
0x8b10, //Dark1 14a1 add 720p 
0x8ca2,
0x8d14, //Dark1 14a2 add 720p 
0x8ea3,
0x8f1a, //Dark1 14a3 add 720p 
0x90a4,
0x911a, //Dark1 14a4 add 720p 
0x92c9,
0x9313, //Dark1 14c9
0x94ca,
0x9520, //Dark1 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9914, //Dark1 1A10 add 720p 
0x9a18,
0x9b1f, //Dark1 1A18
0x9c19,
0x9d15, //Dark1 1A19
0x9e1a,
0x9f0a, //Dark1 1A1a
0xa01b,
0xa107, //Dark1 1A1b
0xa21c,
0xa303, //Dark1 1A1c
0xa41d,
0xa503, //Dark1 1A1d
0xa61e,
0xa703, //Dark1 1A1e
0xa81f,
0xa903, //Dark1 1A1f
0xaa20,
0xab07, //Dark1 1A20 add 720p 
0xac2f,
0xadf6, //Dark1 1A2f add 720p 
0xae32,
0xaf07, //Dark1 1A32 add 720p 
0xb034,
0xb1df, //Dark1 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31b, //Dark1 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb5ef, //Dark1 1A36
0xb637,
0xb740, //Dark1 1A37
0xb838,
0xb9ff, //Dark1 1A38
0xba39,
0xbb2e, //Dark1 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark1 1A3a
0xbe3b,
0xbf01, //Dark1 1A3b
0xc03c,
0xc10c, //Dark1 1A3c
0xc23d,
0xc301, //Dark1 1A3d
0xc43e,
0xc507, //Dark1 1A3e
0xc63f,
0xc701, //Dark1 1A3f
0xc840,
0xc90c, //Dark1 1A40
0xca41,
0xcb01, //Dark1 1A41
0xcc42,
0xcd07, //Dark1 1A42
0xce43,
0xcf2b, //Dark1 1A43
0xd04d,
0xd115, //Dark1 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd314, //Dark1 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd513, //Dark1 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark1 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd911, //Dark1 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Dark1 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd0f, //Dark1 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf0e, //Dark1 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe115, //Dark1 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe314, //Dark1 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe513, //Dark1 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark1 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe911, //Dark1 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Dark1 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed0f, //Dark1 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef0e, //Dark1 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf11e, //Dark1 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf31d, //Dark1 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf51c, //Dark1 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf71a, //Dark1 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf918, //Dark1 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb16, //Dark1 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd14, //Dark1 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E6 page.
0xDC2E,
0xDDB2,

0x03e6, //DMA E6 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark1 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x131e, //Dark1 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x151d, //Dark1 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x171c, //Dark1 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x191a, //Dark1 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b18, //Dark1 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d16, //Dark1 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Dark1 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark1 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark1 1A7d //RGB SP Edge Gain1
0x247e,
0x251f, //Dark1 1A7e //RGB SP Edge Gain2
0x267f,
0x271e, //Dark1 1A7f //RGB SP Edge Gain3
0x2880,
0x291c, //Dark1 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b1a, //Dark1 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d18, //Dark1 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f14, //Dark1 1A83 //RGB SP Edge Gain7
0x3084,
0x3110, //Dark1 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Dark1 1A9e //RGB SP STD Gain1
0x349f,
0x3520, //Dark1 1A9f //RGB SP STD Gain2
0x36a0,
0x371e, //Dark1 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391c, //Dark1 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark1 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d18, //Dark1 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f14, //Dark1 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4110, //Dark1 1Aa5 //RGB SP STD Gain8
0x42a6,
0x43aa, //Dark1 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x4504, //Dark1 1Aa7 add 720p
0x46a8,
0x4706, //Dark1 1Aa8 add 720p
0x48a9,
0x4908, //Dark1 1Aa9 add 720p
0x4aaa,
0x4b09, //Dark1 1Aaa add 720p
0x4cab,
0x4d0a, //Dark1 1Aab add 720p
0x4eaf,
0x4f04, //Dark1 1Aaf add 720p
0x50b0,
0x5106, //Dark1 1Ab0 add 720p
0x52b1,
0x5308, //Dark1 1Ab1 add 720p
0x54b2,
0x550a, //Dark1 1Ab2 add 720p
0x56b3,
0x570c, //Dark1 1Ab3 add 720p
0x58ca,
0x5900, //Dark1 1Aca
0x5ae3,
0x5b12, //Dark1 1Ae3 add 720p
0x5ce4,
0x5d12, //Dark1 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark1 1070 Trans Func.   130108 Dark1 transFuc Flat graph
0x6271,
0x6306, //Dark1 1071
0x6472,
0x65be, //Dark1 1072
0x6673,
0x6799, //Dark1 1073
0x6874,
0x6900, //Dark1 1074
0x6a75,
0x6b00, //Dark1 1075
0x6c76,
0x6d20, //Dark1 1076
0x6e77,
0x6f33, //Dark1 1077
0x7078,
0x7133, //Dark1 1078
0x7279,
0x7340, //Dark1 1079
0x747a,
0x7500, //Dark1 107a
0x767b,
0x7740, //Dark1 107b
0x787c,
0x7900, //Dark1 107c
0x7a7d,
0x7b07, //Dark1 107d
0x7c7e,
0x7d0f, //Dark1 107e
0x7e7f,
0x7f1e, //Dark1 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark1 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark1 031a (for sun-spot)
0x881b,
0x897c, //Dark1 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark1 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark1 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9104, //Dark1 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x95a9, //Dark1 1211 (20130416 for defect)

0x0e00, // burst end

///////////////////////////////////////////////////////////////////////////////
// E7 ~ E9 Page (DMA Dark2)
///////////////////////////////////////////////////////////////////////////////

0x03e7, //DMA E7 Page
0x0e01, // burst start

0x1003,
0x1111, //11 page
0x1211,
0x13ff, //Dark2 1111 add 720p
0x1414,
0x1500, //Dark2 1114 add 720p
0x1615,
0x1700, //Dark2 1115 add 720p
0x1816,
0x1900, //Dark2 1116 add 720p
0x1a17,
0x1b1e, //Dark2 1117 add 720p
0x1c18,
0x1d10, //Dark2 1118 add 720p
0x1e19,
0x1f06, //Dark2 1119 add 720p
0x2037,
0x2101, //Dark2 1137 //Pre Flat rate B[4:1] //04
0x2238,
0x2300, //Dark2 1138 //Pre Flat R1 LumL
0x2439,
0x2503, //Dark2 1139
0x263a,
0x2703, //Dark2 113a
0x283b,
0x29ff, //Dark2 113b
0x2a3c,
0x2b00, //Dark2 113c
0x2c3d,
0x2d13, //Dark2 113d
0x2e3e,
0x2f00, //Dark2 113e
0x303f,
0x3110, //Dark2 113f
0x3240,
0x3300, //Dark2 1140
0x3441,
0x3510, //Dark2 1141
0x3642,
0x3700, //Dark2 1142
0x3843,
0x3918, //Dark2 1143
0x3a49,
0x3b02, //Dark2 1149
0x3c4a,
0x3d04, //Dark2 114a
0x3e4b,
0x3f07, //Dark2 114b
0x404c,
0x410c, //Dark2 114c
0x424d,
0x4310, //Dark2 114d
0x444e,
0x4518, //Dark2 114e
0x464f,
0x4720, //Dark2 114f
0x4850,
0x491a, //Dark2 1150
0x4a51,
0x4b1c, //Dark2 1151
0x4c52,
0x4d1e, //Dark2 1152
0x4e53,
0x4f24, //Dark2 1153
0x5054,
0x5128, //Dark2 1154
0x5255,
0x5326, //Dark2 1155
0x5456,
0x5522, //Dark2 1156
0x5657,
0x571e, //Dark2 1157
0x5858,
0x593f, //Dark2 1158
0x5a59,
0x5b3f, //Dark2 1159
0x5c5a,
0x5d3f, //Dark2 115a
0x5e5b,
0x5f3f, //Dark2 115b
0x605c,
0x613f, //Dark2 115c
0x625d,
0x633f, //Dark2 115d
0x645e,
0x653f, //Dark2 115e
0x665f,
0x673f, //Dark2 115f
0x686e,
0x6910, //Dark2 116e
0x6a6f,
0x6b10, //Dark2 116f
0x6c77,
0x6d20, //Dark2 1177 //Bayer SP Lum Pos1
0x6e78,
0x6f1e, //Dark2 1178 //Bayer SP Lum Pos2
0x7079,
0x711c, //Dark2 1179 //Bayer SP Lum Pos3
0x727a,
0x7318, //Dark2 117a //Bayer SP Lum Pos4
0x747b,
0x7514, //Dark2 117b //Bayer SP Lum Pos5
0x767c,
0x7710, //Dark2 117c //Bayer SP Lum Pos6
0x787d,
0x7908, //Dark2 117d //Bayer SP Lum Pos7
0x7a7e,
0x7b08, //Dark2 117e //Bayer SP Lum Pos8
0x7c7f,
0x7d1c, //Dark2 117f //Bayer SP Lum Neg1
0x7e80,
0x7f1c, //Dark2 1180 //Bayer SP Lum Neg2
0x8081,
0x811c, //Dark2 1181 //Bayer SP Lum Neg3
0x8282,
0x8318, //Dark2 1182 //Bayer SP Lum Neg4
0x8483,
0x8514, //Dark2 1183 //Bayer SP Lum Neg5
0x8684,
0x8710, //Dark2 1184 //Bayer SP Lum Neg6
0x8885,
0x8908, //Dark2 1185 //Bayer SP Lum Neg7
0x8a86,
0x8b08, //Dark2 1186 //Bayer SP Lum Neg8
0x8c8f,
0x8d20, //Dark2 118f //Bayer SP Dy Pos1
0x8e90,
0x8f1e, //Dark2 1190 //Bayer SP Dy Pos2
0x9091,
0x911c, //Dark2 1191 //Bayer SP Dy Pos3
0x9292,
0x931a, //Dark2 1192 //Bayer SP Dy Pos4
0x9493,
0x9516, //Dark2 1193 //Bayer SP Dy Pos5
0x9694,
0x9714, //Dark2 1194 //Bayer SP Dy Pos6
0x9895,
0x9912, //Dark2 1195 //Bayer SP Dy Pos7
0x9a96,
0x9b10, //Dark2 1196 //Bayer SP Dy Pos8
0x9c97,
0x9d1d, //Dark2 1197 //Bayer SP Dy Neg1
0x9e98,
0x9f1d, //Dark2 1198 //Bayer SP Dy Neg2
0xa099,
0xa11c, //Dark2 1199 //Bayer SP Dy Neg3
0xa29a,
0xa31a, //Dark2 119a //Bayer SP Dy Neg4
0xa49b,
0xa516, //Dark2 119b //Bayer SP Dy Neg5
0xa69c,
0xa714, //Dark2 119c //Bayer SP Dy Neg6
0xa89d,
0xa912, //Dark2 119d //Bayer SP Dy Neg7
0xaa9e,
0xab10, //Dark2 119e //Bayer SP Dy Neg8
0xaca7,
0xad18, //Dark2 11a7 //Bayer SP Edge1
0xaea8,
0xaf18, //Dark2 11a8 //Bayer SP Edge2
0xb0a9,
0xb118, //Dark2 11a9 //Bayer SP Edge3
0xb2aa,
0xb315, //Dark2 11aa //Bayer SP Edge4
0xb4ab,
0xb512, //Dark2 11ab //Bayer SP Edge5
0xb6ac,
0xb710, //Dark2 11ac //Bayer SP Edge6
0xb8ad,
0xb910, //Dark2 11ad //Bayer SP Edge7
0xbaae,
0xbb10, //Dark2 11ae //Bayer SP Edge8
0xbcb7,
0xbd18, //Dark2 11b7 add 720p
0xbeb8,
0xbf10, //Dark2 11b8 add 720p
0xc0b9,
0xc108, //Dark2 11b9 add 720p
0xc2ba,
0xc308, //Dark2 11ba add 720p
0xc4bb,
0xc508, //Dark2 11bb add 720p
0xc6bc,
0xc708, //Dark2 11bc add 720p
0xc8c7,
0xc91c, //Dark2 11c7 //Bayer SP STD1
0xcac8,
0xcb1c, //Dark2 11c8 //Bayer SP STD2
0xccc9,
0xcd1c, //Dark2 11c9 //Bayer SP STD3
0xceca,
0xcf1a, //Dark2 11ca //Bayer SP STD4
0xd0cb,
0xd118, //Dark2 11cb //Bayer SP STD5
0xd2cc,
0xd316, //Dark2 11cc //Bayer SP STD6
0xd4cd,
0xd514, //Dark2 11cd //Bayer SP STD7
0xd6ce,
0xd712, //Dark2 11ce //Bayer SP STD8
0xd8cf,
0xd922, //Dark2 11cf //Bayer Post STD gain Neg/Pos
0xdad0,
0xdb00, //Dark2 11d0 //Bayer Flat R1 Lum L
0xdcd1,
0xdd04, //Dark2 11d1
0xded2,
0xdf1a, //Dark2 11d2
0xe0d3,
0xe123, //Dark2 11d3
0xe2d4,
0xe300, //Dark2 11d4 //Bayer Flat R1 STD L
0xe4d5,
0xe516, //Dark2 11d5 //Bayer Flat R1 STD H
0xe6d6,
0xe700, //Dark2 11d6
0xe8d7,
0xe91c, //Dark2 11d7
0xead8,
0xeb00, //Dark2 11d8 //Bayer Flat R1 DY L
0xecd9,
0xed08, //Dark2 11d9 //Bayer Flat R1 DY H
0xeeda,
0xef00, //Dark2 11da
0xf0db,
0xf10e, //Dark2 11db
0xf2df,
0xf373, //Dark2 11df //Bayer Flat R1/R2 rate
0xf4e0,
0xf504, //Dark2 11e0
0xf6e1,
0xf71a, //Dark2 11e1
0xf8e2,
0xf900, //Dark2 11e2 //Bayer Flat R4 LumL
0xfae3,
0xfbff, //Dark2 11e3
0xfce4,
0xfd00, //Dark2 11e4
0x0e00, // burst end

0x03e8, //DMA E8 Page
0x0e01, // burst start

0x10e5,
0x1118, //Dark2 11e5
0x12e6,
0x1300, //Dark2 11e6
0x14e7,
0x1528, //Dark2 11e7
0x16e8,
0x1700, //Dark2 11e8
0x18e9,
0x1909, //Dark2 11e9
0x1aea,
0x1b00, //Dark2 11ea
0x1ceb,
0x1d14, //Dark2 11eb
0x1eef,
0x1f33, //Dark2 11ef //Bayer Flat R3/R4 rate
0x2003,
0x2112, //12 Page
0x2240,
0x2336, //Dark2 1240 add 720p
0x2470,
0x2581, //Dark2 1270 // Bayer Sharpness ENB add 720p
0x2671,
0x2707, //Dark2 1271 //Bayer HPF Gain
0x2872,
0x2907, //Dark2 1272 //Bayer LPF Gain
0x2a77,
0x2b00, //Dark2 1277
0x2c78,
0x2d09, //Dark2 1278
0x2e79,
0x2f2e, //Dark2 1279
0x307a,
0x3150, //Dark2 127a
0x327b,
0x3310, //Dark2 127b
0x347c,
0x3550, //Dark2 127c //skin HPF gain
0x367d,
0x3710, //Dark2 127d
0x387f,
0x3950, //Dark2 127f
0x3a87,
0x3b08, //Dark2 1287 add 720p
0x3c88,
0x3d08, //Dark2 1288 add 720p
0x3e89,
0x3f08, //Dark2 1289 add 720p
0x408a,
0x410c, //Dark2 128a add 720p
0x428b,
0x4310, //Dark2 128b add 720p
0x448c,
0x4514, //Dark2 128c add 720p
0x468d,
0x4718, //Dark2 128d add 720p
0x488e,
0x491a, //Dark2 128e add 720p
0x4a8f,
0x4b08, //Dark2 128f add 720p
0x4c90,
0x4d0a, //Dark2 1290 add 720p
0x4e91,
0x4f0e, //Dark2 1291 add 720p
0x5092,
0x5112, //Dark2 1292 add 720p
0x5293,
0x5316, //Dark2 1293 add 720p
0x5494,
0x551a, //Dark2 1294 add 720p
0x5695,
0x5720, //Dark2 1295 add 720p
0x5896,
0x5920, //Dark2 1296 add 720p
0x5aae,
0x5b20, //Dark2 12ae
0x5caf,
0x5d33, //Dark2 12af // B[7:4]Blue/B[3:0]Skin
0x5ec0,
0x5f23, //Dark2 12c0 // CI-LPF ENB add 720p
0x60c3,
0x6118, //Dark2 12c3 add 720p
0x62c4,
0x630d, //Dark2 12c4 add 720p
0x64c5,
0x6506, //Dark2 12c5 add 720p
0x66c6,
0x6711, //Dark2 12c6
0x68c7,
0x6911, //Dark2 12c7
0x6ac8,
0x6b04, //Dark2 12c8
0x6cd0,
0x6d02, //Dark2 12d0 add 720p
0x6ed1,
0x6f04, //Dark2 12d1 add 720p
0x70d2,
0x7107, //Dark2 12d2 add 720p
0x72d3,
0x730c, //Dark2 12d3 add 720p
0x74d4,
0x7510, //Dark2 12d4 add 720p
0x76d5,
0x7718, //Dark2 12d5 add 720p
0x78d6,
0x7920, //Dark2 12d6 add 720p
0x7ad7,
0x7b29, //Dark2 12d7 //CI LPF Lum offset start
0x7cd8,
0x7d2a, //Dark2 12d8
0x7ed9,
0x7f2c, //Dark2 12d9
0x80da,
0x812b, //Dark2 12da
0x82db,
0x832a, //Dark2 12db
0x84dc,
0x8528, //Dark2 12dc
0x86dd,
0x8727, //Dark2 12dd
0x88de,
0x8927, //Dark2 12de //CI LPF Lum offset end
0x8ae0,
0x8b63, //Dark2 12e0 // 20121120 ln dy
0x8ce1,
0x8dfc, //Dark2 12e1
0x8ee2,
0x8f02, //Dark2 12e2
0x90e3,
0x9110, //Dark2 12e3 //PS LN graph Y1
0x92e4,
0x9312, //Dark2 12e4 //PS LN graph Y2
0x94e5,
0x951a, //Dark2 12e5 //PS LN graph Y3
0x96e6,
0x971d, //Dark2 12e6 //PS LN graph Y4
0x98e7,
0x991e, //Dark2 12e7 //PS LN graph Y5
0x9ae8,
0x9b1f, //Dark2 12e8 //PS LN graph Y6
0x9ce9,
0x9d10, //Dark2 12e9 //PS DY graph Y1
0x9eea,
0x9f12, //Dark2 12ea //PS DY graph Y2
0xa0eb,
0xa118, //Dark2 12eb //PS DY graph Y3
0xa2ec,
0xa31c, //Dark2 12ec //PS DY graph Y4
0xa4ed,
0xa51e, //Dark2 12ed //PS DY graph Y5
0xa6ee,
0xa71f, //Dark2 12ee //PS DY graph Y6
0xa8f0,
0xa900, //Dark2 12f0
0xaaf1,
0xab2a, //Dark2 12f1
0xacf2,
0xad32, //Dark2 12f2
0xae03,
0xaf13, //13 Page
0xb010,
0xb180, //Dark2 1310 //Y-NR ENB add 720p
0xb230,
0xb320, //Dark2 1330
0xb431,
0xb520, //Dark2 1331
0xb632,
0xb720, //Dark2 1332
0xb833,
0xb920, //Dark2 1333
0xba34,
0xbb20, //Dark2 1334
0xbc35,
0xbd2d, //Dark2 1335
0xbe36,
0xbf20, //Dark2 1336
0xc037,
0xc120, //Dark2 1337
0xc238,
0xc302, //Dark2 1338
0xc440,
0xc500, //Dark2 1340
0xc641,
0xc713, //Dark2 1341
0xc842,
0xc962, //Dark2 1342
0xca43,
0xcb63, //Dark2 1343
0xcc44,
0xcd7e, //Dark2 1344
0xce45,
0xcf00, //Dark2 1345
0xd046,
0xd16b, //Dark2 1346
0xd247,
0xd300, //Dark2 1347
0xd448,
0xd54a, //Dark2 1348
0xd649,
0xd700, //Dark2 1349
0xd84a,
0xd943, //Dark2 134a
0xda4b,
0xdb00, //Dark2 134b
0xdc4c,
0xdd2e, //Dark2 134c
0xde83,
0xdf08, //Dark2 1383
0xe084,
0xe10a, //Dark2 1384
0xe2b7,
0xe3ff, //Dark2 13b7
0xe4b8,
0xe5ff, //Dark2 13b8
0xe6b9,
0xe7ff, //Dark2 13b9 //20121217 DC R1,2 CR
0xe8ba,
0xe9ff, //Dark2 13ba //20121217 DC R3,4 CR
0xeabd,
0xeb78, //Dark2 13bd //20121121 c-filter LumHL DC rate
0xecc5,
0xed01, //Dark2 13c5 //20121121 c-filter DC_STD R1 R2 //20121217
0xeec6,
0xef22, //Dark2 13c6 //20121121 c-filter DC_STD R3 R4 //20121217
0xf0c7,
0xf133, //Dark2 13c7 //20121121 c-filter DC_STD R5 R6 //20121217
0xf203,
0xf314, //14 page
0xf410,
0xf501, //Dark2 1410
0xf611,
0xf7d8, //Dark2 1411
0xf812,
0xf910, //Dark2 1412
0xfa13,
0xfb05, //Dark2 1413
0xfc14,
0xfd14, //Dark2 1414 //YC2D Low Gain B[5:0]
0x0e00, // burst end

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x1015,
0x117d, //Dark2 1415 // Y Hi filter mask 1/16
0x1216,
0x1317, //Dark2 1416 //YC2D Hi Gain B[5:0]
0x1417,
0x1540, //Dark2 1417
0x1618,
0x170c, //Dark2 1418
0x1819,
0x190c, //Dark2 1419
0x1a1a,
0x1b1c, //Dark2 141a //YC2D Post STD gain Pos
0x1c1b,
0x1d1c, //Dark2 141b //YC2D Post STD gain Neg
0x1e27,
0x1f0f, //Dark2 1427 //YC2D SP Lum Gain Pos1
0x2028,
0x2110, //Dark2 1428 //YC2D SP Lum Gain Pos2
0x2229,
0x2311, //Dark2 1429 //YC2D SP Lum Gain Pos3
0x242a,
0x2512, //Dark2 142a //YC2D SP Lum Gain Pos4
0x262b,
0x2713, //Dark2 142b //YC2D SP Lum Gain Pos5
0x282c,
0x2914, //Dark2 142c //YC2D SP Lum Gain Pos6
0x2a2d,
0x2b13, //Dark2 142d //YC2D SP Lum Gain Pos7
0x2c2e,
0x2d10, //Dark2 142e //YC2D SP Lum Gain Pos8
0x2e30,
0x2f0f, //Dark2 1430 //YC2D SP Lum Gain Neg1
0x3031,
0x3110, //Dark2 1431 //YC2D SP Lum Gain Neg2
0x3232,
0x3311, //Dark2 1432 //YC2D SP Lum Gain Neg3
0x3433,
0x3512, //Dark2 1433 //YC2D SP Lum Gain Neg4
0x3634,
0x3713, //Dark2 1434 //YC2D SP Lum Gain Neg5
0x3835,
0x3913, //Dark2 1435 //YC2D SP Lum Gain Neg6
0x3a36,
0x3b12, //Dark2 1436 //YC2D SP Lum Gain Neg7
0x3c37,
0x3d10, //Dark2 1437 //YC2D SP Lum Gain Neg8
0x3e47,
0x3f1c, //Dark2 1447 //YC2D SP Dy Gain Pos1
0x4048,
0x411b, //Dark2 1448 //YC2D SP Dy Gain Pos2
0x4249,
0x431a, //Dark2 1449 //YC2D SP Dy Gain Pos3
0x444a,
0x4518, //Dark2 144a //YC2D SP Dy Gain Pos4
0x464b,
0x4716, //Dark2 144b //YC2D SP Dy Gain Pos5
0x484c,
0x4914, //Dark2 144c //YC2D SP Dy Gain Pos6
0x4a4d,
0x4b12, //Dark2 144d //YC2D SP Dy Gain Pos7
0x4c4e,
0x4d10, //Dark2 144e //YC2D SP Dy Gain Pos8
0x4e50,
0x4f1a, //Dark2 1450 //YC2D SP Dy Gain Neg1
0x5051,
0x5119, //Dark2 1451 //YC2D SP Dy Gain Neg2
0x5252,
0x5318, //Dark2 1452 //YC2D SP Dy Gain Neg3
0x5453,
0x5517, //Dark2 1453 //YC2D SP Dy Gain Neg4
0x5654,
0x5716, //Dark2 1454 //YC2D SP Dy Gain Neg5
0x5855,
0x5914, //Dark2 1455 //YC2D SP Dy Gain Neg6
0x5a56,
0x5b12, //Dark2 1456 //YC2D SP Dy Gain Neg7
0x5c57,
0x5d10, //Dark2 1457 //YC2D SP Dy Gain Neg8
0x5e67,
0x5f10, //Dark2 1467 //YC2D SP Edge Gain1
0x6068,
0x6123, //Dark2 1468 //YC2D SP Edge Gain2
0x6269,
0x6326, //Dark2 1469 //YC2D SP Edge Gain3
0x646a,
0x6524, //Dark2 146a //YC2D SP Edge Gain4
0x666b,
0x6713, //Dark2 146b //YC2D SP Edge Gain5
0x686c,
0x691a, //Dark2 146c //YC2D SP Edge Gain6
0x6a6d,
0x6b12, //Dark2 146d //YC2D SP Edge Gain7
0x6c6e,
0x6d12, //Dark2 146e //YC2D SP Edge Gain8
0x6e87,
0x6f19, //Dark2 1487 //YC2D SP STD Gain1
0x7088,
0x711a, //Dark2 1488 //YC2D SP STD Gain2
0x7289,
0x731c, //Dark2 1489 //YC2D SP STD Gain3
0x748a,
0x751b, //Dark2 148a //YC2D SP STD Gain4
0x768b,
0x771a, //Dark2 148b //YC2D SP STD Gain5
0x788c,
0x791c, //Dark2 148c //YC2D SP STD Gain6
0x7a8d,
0x7b25, //Dark2 148d //YC2D SP STD Gain7
0x7c8e,
0x7d29, //Dark2 148e //YC2D SP STD Gain8
0x7e97,
0x7f08, //Dark2 1497 add 720p
0x8098,
0x810c, //Dark2 1498 add 720p
0x8299,
0x8310, //Dark2 1499 add 720p
0x849a,
0x8510, //Dark2 149a add 720p
0x869b,
0x8710, //Dark2 149b add 720p
0x88a0,
0x8908, //Dark2 14a0 add 720p
0x8aa1,
0x8b10, //Dark2 14a1 add 720p
0x8ca2,
0x8d14, //Dark2 14a2 add 720p
0x8ea3,
0x8f1a, //Dark2 14a3 add 720p
0x90a4,
0x911a, //Dark2 14a4 add 720p
0x92c9,
0x9313, //Dark2 14c9
0x94ca,
0x9520, //Dark2 14ca
0x9603,
0x971a, //1A page
0x9810,
0x9914, //Dark2 1A10 add 720p
0x9a18,
0x9b1f, //Dark2 1A18
0x9c19,
0x9d15, //Dark2 1A19
0x9e1a,
0x9f0a, //Dark2 1A1a
0xa01b,
0xa107, //Dark2 1A1b
0xa21c,
0xa303, //Dark2 1A1c
0xa41d,
0xa503, //Dark2 1A1d
0xa61e,
0xa703, //Dark2 1A1e
0xa81f,
0xa903, //Dark2 1A1f
0xaa20,
0xab07, //Dark2 1A20 add 720p
0xac2f,
0xadf6, //Dark2 1A2f add 720p
0xae32,
0xaf07, //Dark2 1A32 add 720p
0xb034,
0xb1df, //Dark2 1A34 //RGB High Gain B[5:0]
0xb235,
0xb31b, //Dark2 1A35 //RGB Low Gain B[5:0]
0xb436,
0xb5ef, //Dark2 1A36
0xb637,
0xb740, //Dark2 1A37
0xb838,
0xb9ff, //Dark2 1A38
0xba39,
0xbb2e, //Dark2 1A39 //RGB Flat R2_Lum L
0xbc3a,
0xbd3f, //Dark2 1A3a
0xbe3b,
0xbf01, //Dark2 1A3b
0xc03c,
0xc10c, //Dark2 1A3c
0xc23d,
0xc301, //Dark2 1A3d
0xc43e,
0xc507, //Dark2 1A3e
0xc63f,
0xc701, //Dark2 1A3f
0xc840,
0xc90c, //Dark2 1A40
0xca41,
0xcb01, //Dark2 1A41
0xcc42,
0xcd07, //Dark2 1A42
0xce43,
0xcf2b, //Dark2 1A43
0xd04d,
0xd115, //Dark2 1A4d //RGB SP Lum Gain Neg1
0xd24e,
0xd314, //Dark2 1A4e //RGB SP Lum Gain Neg2
0xd44f,
0xd513, //Dark2 1A4f //RGB SP Lum Gain Neg3
0xd650,
0xd712, //Dark2 1A50 //RGB SP Lum Gain Neg4
0xd851,
0xd911, //Dark2 1A51 //RGB SP Lum Gain Neg5
0xda52,
0xdb10, //Dark2 1A52 //RGB SP Lum Gain Neg6
0xdc53,
0xdd0f, //Dark2 1A53 //RGB SP Lum Gain Neg7
0xde54,
0xdf0e, //Dark2 1A54 //RGB SP Lum Gain Neg8
0xe055,
0xe115, //Dark2 1A55 //RGB SP Lum Gain Pos1
0xe256,
0xe314, //Dark2 1A56 //RGB SP Lum Gain Pos2
0xe457,
0xe513, //Dark2 1A57 //RGB SP Lum Gain Pos3
0xe658,
0xe712, //Dark2 1A58 //RGB SP Lum Gain Pos4
0xe859,
0xe911, //Dark2 1A59 //RGB SP Lum Gain Pos5
0xea5a,
0xeb10, //Dark2 1A5a //RGB SP Lum Gain Pos6
0xec5b,
0xed0f, //Dark2 1A5b //RGB SP Lum Gain Pos7
0xee5c,
0xef0e, //Dark2 1A5c //RGB SP Lum Gain Pos8
0xf065,
0xf11e, //Dark2 1A65 //RGB SP Dy Gain Neg1
0xf266,
0xf31d, //Dark2 1A66 //RGB SP Dy Gain Neg2
0xf467,
0xf51c, //Dark2 1A67 //RGB SP Dy Gain Neg3
0xf668,
0xf71a, //Dark2 1A68 //RGB SP Dy Gain Neg4
0xf869,
0xf918, //Dark2 1A69 //RGB SP Dy Gain Neg5
0xfa6a,
0xfb16, //Dark2 1A6a //RGB SP Dy Gain Neg6
0xfc6b,
0xfd14, //Dark2 1A6b //RGB SP Dy Gain Neg7
0x0e00, // burst end

//I2CD set
0x0326,	//Xdata mapping for I2C direct E9 page.
0xE232,
0xE36A,

0x03e9, //DMA E9 Page
0x0e01, // burst start

0x106c,
0x1112, //Dark2 1A6c //RGB SP Dy Gain Neg8
0x126d,
0x131e, //Dark2 1A6d //RGB SP Dy Gain Pos1
0x146e,
0x151d, //Dark2 1A6e //RGB SP Dy Gain Pos2
0x166f,
0x171c, //Dark2 1A6f //RGB SP Dy Gain Pos3
0x1870,
0x191a, //Dark2 1A70 //RGB SP Dy Gain Pos4
0x1a71,
0x1b18, //Dark2 1A71 //RGB SP Dy Gain Pos5
0x1c72,
0x1d16, //Dark2 1A72 //RGB SP Dy Gain Pos6
0x1e73,
0x1f14, //Dark2 1A73 //RGB SP Dy Gain Pos7
0x2074,
0x2112, //Dark2 1A74 //RGB SP Dy Gain Pos8
0x227d,
0x2320, //Dark2 1A7d //RGB SP Edge Gain1
0x247e,
0x251f, //Dark2 1A7e //RGB SP Edge Gain2
0x267f,
0x271e, //Dark2 1A7f //RGB SP Edge Gain3
0x2880,
0x291c, //Dark2 1A80 //RGB SP Edge Gain4
0x2a81,
0x2b1a, //Dark2 1A81 //RGB SP Edge Gain5
0x2c82,
0x2d18, //Dark2 1A82 //RGB SP Edge Gain6
0x2e83,
0x2f14, //Dark2 1A83 //RGB SP Edge Gain7
0x3084,
0x3110, //Dark2 1A84 //RGB SP Edge Gain8
0x329e,
0x3322, //Dark2 1A9e //RGB SP STD Gain1
0x349f,
0x3520, //Dark2 1A9f //RGB SP STD Gain2
0x36a0,
0x371e, //Dark2 1Aa0 //RGB SP STD Gain3
0x38a1,
0x391c, //Dark2 1Aa1 //RGB SP STD Gain4
0x3aa2,
0x3b1a, //Dark2 1Aa2 //RGB SP STD Gain5
0x3ca3,
0x3d18, //Dark2 1Aa3 //RGB SP STD Gain6
0x3ea4,
0x3f14, //Dark2 1Aa4 //RGB SP STD Gain7
0x40a5,
0x4110, //Dark2 1Aa5 //RGB SP STD Gain8
0x42a6,
0x43aa, //Dark2 1Aa6 //RGB Post STD Gain Pos/Neg
0x44a7,
0x4504, //Dark2 1Aa7 add 720p
0x46a8,
0x4706, //Dark2 1Aa8 add 720p
0x48a9,
0x4908, //Dark2 1Aa9 add 720p
0x4aaa,
0x4b09, //Dark2 1Aaa add 720p
0x4cab,
0x4d0a, //Dark2 1Aab add 720p
0x4eaf,
0x4f04, //Dark2 1Aaf add 720p
0x50b0,
0x5106, //Dark2 1Ab0 add 720p
0x52b1,
0x5308, //Dark2 1Ab1 add 720p
0x54b2,
0x550a, //Dark2 1Ab2 add 720p
0x56b3,
0x570c, //Dark2 1Ab3 add 720p
0x58ca,
0x5900, //Dark2 1Aca
0x5ae3,
0x5b12, //Dark2 1Ae3 add 720p
0x5ce4,
0x5d12, //Dark2 1Ae4 add 720p
0x5e03,
0x5f10, //10 page
0x6070,
0x610c, //Dark2 1070 Trans Func.   130108 Dark2 transFuc Flat graph
0x6271,
0x6306, //Dark2 1071
0x6472,
0x65be, //Dark2 1072
0x6673,
0x6799, //Dark2 1073
0x6874,
0x6900, //Dark2 1074
0x6a75,
0x6b00, //Dark2 1075
0x6c76,
0x6d20, //Dark2 1076
0x6e77,
0x6f33, //Dark2 1077
0x7078,
0x7133, //Dark2 1078
0x7279,
0x7340, //Dark2 1079
0x747a,
0x7500, //Dark2 107a
0x767b,
0x7740, //Dark2 107b
0x787c,
0x7900, //Dark2 107c
0x7a7d,
0x7b07, //Dark2 107d
0x7c7e,
0x7d0f, //Dark2 107e
0x7e7f,
0x7f1e, //Dark2 107f
0x8003,
0x8102, // 2 page
0x8223,
0x8310, //Dark2 0223 (for sun-spot) // normal 3c
0x8403,
0x8503, // 3 page
0x861a,
0x8706, //Dark2 031a (for sun-spot)
0x881b,
0x897c, //Dark2 031b (for sun-spot)
0x8a1c,
0x8b00, //Dark2 031c (for sun-spot)
0x8c1d,
0x8d50, //Dark2 031d (for sun-spot)
0x8e03,
0x8f11, // 11 page
0x90f0,
0x9105, //Dark2 11f0 (for af bug)
0x9203, 
0x9312, //12 page
0x9411,
0x95a9, //Dark2 1211 (20130416 for defect)

0x0e00, // burst end

//--------------------------------------------------------------------------//
// MIPI TX Setting  //PCLK 86MHz
//--------------------------------------------------------------------------//
0x0305,  // Page05
0x1100, // lvds_ctl_2 //Phone set not continuous
0x1200,  // crc_ctl
0x1300,  // serial_ctl
0x1400,  // ser_out_ctl_1
0x1500,  // dphy_fifo_ctl
0x1602,  // lvds_inout_ctl1
0x1700,  // lvds_inout_ctl2
0x1880,  // lvds_inout_ctl3
0x1900,  // lvds_inout_ctl4
0x1af0,  // lvds_time_ctl
0x1c01,  // tlpx_time_l_dp
0x1d0d,  // tlpx_time_l_dn
0x1e0c,  // hs_zero_time
0x1f0c,  // hs_trail_time
0x21b8,  // hs_sync_code
0x2200,  // frame_start_id
0x2301,  // frame_end_id
0x241e,  // long_packet_id
0x2500,  // s_pkt_wc_h
0x2600,  // s_pkt_wc_l
0x2708,  // lvds_frame_end_cnt_h
0x2800,  // lvds_frame_end_cnt_l
0x2a06,  // lvds_image_width_h
0x2b40,  // lvds_image_width_l
0x2c04,  // lvds_image_height_h
0x2db0,  // lvds_image_height_l
0x300a,  // l_pkt_wc_h  // Full = 1280 * 2 (YUV)
0x3100,  // l_pkt_wc_l
0x321c,  // clk_zero_time
0x330e,  // clk_post_time
0x3405,  // clk_prepare_time
0x3508,  // clk_trail_time
0x3601,  // clk_tlpx_time_dp
0x3708,  // clk_tlpx_time_dn
0x3907,  // lvds_bias_ctl
0x3a00,  // lvds_test_tx
0x4200,  // mipi_test_width_l
0x4300,  // mipi_test_height_l
0x4400,  // mipi_test_size_h
0x4500,  // mipi_test_hsync_st
0x4600,  // mipi_test_hblank
0x4700,  // mipi_test_vsync_st
0x4800,  // mipi_test_vsync_end
0x49ff,  // ulps_size_opt1
0x4a0a,  // ulps_size_opt2
0x4b22,  // ulps_size_opt3
0x4c41,  // hs_wakeup_size_h
0x4d20,  // hs_wakeup_size_l
0x4e00,  // mipi_int_time_h
0x4fff,  // mipi_int_time_l
0x500A,  // cntns_clk_wait_h
0x5100,  // cntns_clk_wait_l
0x5740,  // mipi_dmy_reg
0x6000,  // mipi_frame_pkt_opt
0x6108,  // line_cnt_value_h
0x6200,  // line_cnt_value_l
0x101c,  // lvds_ctl_1

///////////////////////////////////////////////////////////////////////////////
// sleep off
///////////////////////////////////////////////////////////////////////////////

0x0300,
0x1e01, // frame update
0x0100,	// Sleep Off

0x03c0,
0x7F80,	// DMA on
0x7E01,	// DMA set

0xff01,	//delay 10ms

///////////////////////////////////////////////////////////////////////////////
// end of HD set
///////////////////////////////////////////////////////////////////////////////

};

static SensorConfigScript sr352_recording_50Hz_30fps_60hz[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 30fps @ OPCLK 54MHz(1Line = 2200)
0x3D1B,
0x3E75,
0x3FB0,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101,	//AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x1090,	//AE On (Band Off) 50hz b0, 60hz 90 
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On

0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};

static SensorConfigScript sr352_recording_50Hz_25fps_60hz[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 25fps @ OPCLK 54MHz(1Line = 2200)
0x3D20,
0x3Eee,
0x3F78,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x1090,	//AE On (Band Off) 50hz b0, 60hz 90
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On

0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms


};

static SensorConfigScript sr352_recording_50Hz_15fps_60hz[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set
0x03c7,
0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10

0x0316, // dark color 
0x103f,

//--------------------------------------------------------------------------//
//Fixed mode setting
//--------------------------------------------------------------------------//
///////////////////////////////////////////
// 20 Page(Fuzzy)
///////////////////////////////////////////
0x0320,
0x3C00,	//Fix 15fps @ OPCLK 54MHz(1Line = 2200)
0x3D36,
0x3Eeb,
0x3F60,

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1580,	//Patch Weight Off B[6]
0x361e,	//Max 30fps
0x371e,	//Max 30fps

0x1101, // B[1]Initial Speed Up, B[0]AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2A60,

0x03c7,
0x1090,	//AE On (Band Off) 50hz b0, 60hz 90 
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// C8 Page(AWB)
///////////////////////////////////////////

0x03c8,
0x148f,
0x1722, //AWB Speed
0x1844,
0x2220,
0x11C3,	//AWB reset

0x03d3,
0x108d,	// Adaptive on //B[1] EV with Y off

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1194,	//Fixed Mode On


0x0300,
0x1e01,  // frame update
0x0100,	// Sleep Off
0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};


static SensorConfigScript sr352_recording_50Hz_modeOff_60hz[] = {

0x03c1,
0x1006, // ssd tranfer disable
0xff01, 

0x0300,
0x0101,	//Sleep on

0x03c1,
0x1007, // ssd tranfer enable

0x03c0,
0x7f00,	// DMA off
0x7e01,	// DMA set

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x15c0,	//Patch Weight On B[6]
0x3608,	//Max 8fps
0x3708,	//Max 8fps

///////////////////////////////////////////
// C7 Page(AE)
///////////////////////////////////////////
0x03c7,
0x1101,	//AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

0x03c7,
0x10d0,	//AE On 50hz f0, 60hz d0
//--------------------------------------------------------------------------//

///////////////////////////////////////////
// 00 Page
///////////////////////////////////////////
0x0300,
0x1190,	//Fixed Mode Off

0x0300,
0x1e01, // frame update
0x0100, //sleep off

0x03c0,
0x7f80,	// DMA on
0x7e01,	// DMA set

0xff02, //delay 20ms

};
#endif
#if 0
/*===========================================*/
/*CAMERA_SCENE_off                         */
/*===========================================*/
static SensorConfigScript sr352_SceneOff_60hz[] =
{
	//Scene Off (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3,
	0x108f, //EV option on
	0x11fe, //Function On
	
	0x0310,
	0x1210, //Y Ofs On
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 

	0x03c7,
	0x3608, //Band100 Max 8fps
	0x3708, //Band120 Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00, //cb offset
	0x7c00, //cr offset

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00, //SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320, //HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1403, //d gain on
	
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Landscape                   */
/*===========================================*/
static SensorConfigScript sr352_Landscape_60hz[] =
{
	//Scene Landscape (FPS Auto/ISO Auto/Maxtrix/Br0/AWB Auto/Sat1/Sharp+1)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_Matrix
0x03c6,
0x9E11,	//1 Line
0x9F11,
0xA011,
0xA111,
0xA211,
0xA311,

0xA411,	//2 Line
0xA511,
0xA611,
0xA711,
0xA811,
0xA911,

0xAA11,//3 Line
0xAB12,
0xAC22,
0xAD22,
0xAE21,
0xAF11,

0xB011,//4 Line
0xB112,
0xB222,
0xB322,
0xB421,
0xB511,

0xB611,//5 Line
0xB712,
0xB822,
0xB922,
0xBA21,
0xBB11,

0xBC11,//6 Line
0xBD12,
0xBE22,
0xBF22,
0xC021,
0xC111,

0xC211,//7 Line
0xC312,
0xC422,
0xC522,
0xC621,
0xC711,

0xC811,//8 Line
0xC911,
0xCA11,
0xCB11,
0xCC11,
0xCD11,

0xCE11,//9 Line
0xCF11,
0xD011,
0xD111,
0xD211,
0xD311,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b20,
	0x7c20,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness +1
	0x03de, //DE Page(Outdoor)
	0xd9cc, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd9cc, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd9cc,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd9cc,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,	
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_Party                       */
/*===========================================*/
static SensorConfigScript sr352_Party_60hz[] =
{
	/*Party/Indoor (FPS Auto/ISO 200/Center/Br0/AWB Auto/Sat1/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x5182, //pga_max_total
	0x525c, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,


	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b20,
	0x7c20,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x1090,	//AE On (Band Off) 50hz b0, 60hz 90 
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_sunset                      */
/*===========================================*/
static SensorConfigScript sr352_Sunset_60hz[] =
{
	//Scene Sunset (FPS Auto/ISO Auto/Center/Br0/AWB Day/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cloudy
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca, 

	0x9506, //R Min
	0x96b0,
	0x9706, //R Max
	0x98d0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d05, //B Min
	0x9ef0,
	0x9f06, //B Max
	0xa010,
  
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
  
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};


/*===========================================*/
/*CAMERA_SCENE_Dawn                        */
/*===========================================*/
static SensorConfigScript sr352_Dawn_60hz[] =
{
	//Scene sunrise Dawn (FPS Auto/ISO Auto/Center/Br0/AWB CWF/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cwf
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca,
	0x9504, //R Min
	0x96c0,
	0x9704, //R Max
	0x98e0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d08, //B Min
	0x9eb0,
	0x9f08, //B Max
	0xa0d0,
	
	
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
  
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Fall                        */
/*===========================================*/
static SensorConfigScript sr352_Fall_60hz[] =
{
	//Scene Fall (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat2/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b40,
	0x7c40,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};
#endif
#if 0
/*===========================================*/
/*CAMERA_SCENE_Nightshot	          */
/*===========================================*/
static SensorConfigScript sr352_Nightshot_60hz[] =
{

	//Scene Night (FPS Night/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms
	
	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108f, //EV option on
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1210, //Y Ofs On

	0x0320,
	0x1260, //AE Digital gain On

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x71ba, //Digital gain max 
	
	0x03c7,
	0x3604, //Max 4fps
	0x3704, //Max 4fps

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//Capture Set
	0x03D9,
	0x8C60, //DG On
	0x25FF, //Deshutter AG Max
	0x2620, //Deshutter AG Min
	0x27ba, //Deshutter DG Max
	0x2880, //Deshutter DG Min
	0x2902, //Deshutter Max 2Fps
	0x1006, //Deshutter On

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off
};



/*===========================================*/
/*CAMERA_SCENE_Backlight                        */
/*===========================================*/
static SensorConfigScript sr352_Backlight_60hz[] =
{
	//Scene Against (FPS Auto/ISO Auto/Spot/Br0/AWB Auto/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

	//SSD_Spot
	0x03c6,//1 Line
	0x9E00,
	0x9F00,
	0xA000,
	0xA100,
	0xA200,
	0xA300,
	0xA400,//2 Line
	0xA500,
	0xA600,
	0xA700,
	0xA800,
	0xA900,
	0xAA00,//3 Line
	0xAB01,
	0xAC11,
	0xAD11,
	0xAE10,
	0xAF00,
	0xB000,//4 Line
	0xB101,
	0xB2ff,
	0xB3ff,
	0xB410,
	0xB500,
	0xB600,//5 Line
	0xB701,
	0xB8ff,
	0xB9ff,
	0xBA10,
	0xBB00,
	0xBC00,//6 Line
	0xBD01,
	0xBEff,
	0xBFff,
	0xC010,
	0xC100,
	0xC200,//7 Line
	0xC301,
	0xC411,
	0xC511,
	0xC610,
	0xC700,
	0xC800,//8 Line
	0xC900,
	0xCA00,
	0xCB00,
	0xCC00,
	0xCD00,
	0xCE00,//9 Line
	0xCF00,
	0xD000,
	0xD100,
	0xD200,
	0xD300,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB
	0x03c8,
	0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off

	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off	
};

/*===========================================*/
/*CAMERA_SCENE_Candle                      */
/*===========================================*/
static SensorConfigScript sr352_Candle_60hz[] =
{
	//Scene candle (FPS Auto/ISO Auto/Center/Br0/AWB Day/Sat0/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
	0x03c7,
	0x3608, //Max 8fps
	0x3708, //Max 8fps

	0x03D9,
	0x8C20, //DG Off
	0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

	//EV
	0x03d3,
	0x7a00, //target offset

	//Saturation
	0x03d3,
	0x7b00,
	0x7c00,

	//AWB cloudy
	0x03c8,
	0x12a0, //Adaptive e0, manual a0

	0x03ca, 

	0x9506, //R Min
	0x96b0,
	0x9706, //R Max
	0x98d0,
	0x9904, //G Min
	0x9a00,
	0x9b04, //G Max
	0x9c80,
	0x9d05, //B Min
	0x9ef0,
	0x9f06, //B Max
	0xa010,

	
	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

	0x03c7,
	0x1101, //AE Reset
	0xff01,
	0x4C00,//SW ExpMin	 = 8800
	0x4D00,	
	0x4E22,	
	0x4F60,	

	0x0320,//HW ExpMin  = 8800
	0x2800,
	0x2922,
	0x2a60,
	
	0x0310,
  0x1402, // d gain off
	
	0x03c7,
	0x10d0,	//AE On 50hz f0, 60hz d0
	
	0x0300,
	0x0100, //sleep off

};


/*===========================================*/
/*CAMERA_SCENE_Beach      */
/*===========================================*/
static SensorConfigScript sr352_Beach_60hz[] =
{
	//Beach(FPS Auto/ISO 50/Center/Br1/AWB Auto/Sat1/Sharp0)
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10
	
	0x03D3, 
	0x108d, //EV option off
	0x11fe, //Y target On
	
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x5120, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x3608, //Max 8fps
0x3708, //Max 8fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a10, //target offset

//Saturation
0x03d3,
0x7b20,
0x7c20,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x1090,	//AE On (Band Off) 50hz b8, 60hz 90 

0x0300,
0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Sports              */
/*===========================================*/
static SensorConfigScript sr352_Sports_60hz[] =
{
	/*Sports (FPS Sports/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms
	
	0x03c7,
	0x1010,	//AE Off (Band Off) 50hz 30, 60hz 10

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off

	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5248, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x361e, //Max 30fps
0x371e, //Max 30fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x1090,	//AE On (Band Off) 50hz b0, 60hz 90 

0x0300,
0x0100, //sleep off

};


/*===========================================*/
/*CAMERA_SCENE_Firework                      */
/*===========================================*/
static SensorConfigScript sr352_Firework_60hz[] =
{
	/*Firework (FPS Fire/ISO 50/Center/Br0/AWB Auto/Sat0/Sharp0)*/
	0x0300,
	0x0110, //frame sleep on
	0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50
	
	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	
0x03c7,
0x3604, //Max 4fps
0x3704, //Max 4fps

//Capture Set
0x03D9,
0x8C20, //DG Off
0x2520, //Deshutter AG Max
0x2620, //Deshutter AG Min
0x2780, //Deshutter DG Max
0x2880, //Deshutter DG Min
0x2901, //Deshutter Max 1Fps
0x1006, //Deshutter On

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

	//Sharpness 0
	0x03de, //DE Page(Outdoor)
	0xd954, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd976, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd998,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd998,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10d0,	//AE On (Band Off) 50hz b0, 60hz 90 

0x0300,
0x0100, //sleep off

};

/*===========================================*/
/*CAMERA_SCENE_Portrait              */
/*===========================================*/
static SensorConfigScript sr352_Portrait_60hz[] =
{
	//Scene Portrait (FPS Auto/ISO Auto/Center/Br0/AWB Auto/Sat0/Sharp-1)
0x0300,
0x0110, //frame sleep on
0xff19, //delay 250ms

	0x03c7,
	0x1050,	//AE Off (Band Off) 50hz 70, 60hz 50

	0x03D3, 
	0x108d, //EV option off
	0x117e, //Y target Off
	
	0x03d8,
	0xcc34,
	0x03dd,
	0xbf34,
	
	0x0310,
	0x1200, //Y Ofs Off
	
	0x0320,
	0x1220, //AE Digital gain Off

	0x51ff, //pga_max_total
	0x5220, //pga_min_total
	0x7180, //Digital gain max 
	

0x03c7,
0x3608, //Max 8fps
0x3708, //Max 8fps

0x03D9,
0x8C20, //DG Off
0x1000, //Deshutter Off

//SSD_CenterWeighted
0x03c6,
0x9E00,	//1 Line
0x9F00,
0xA000,
0xA100,
0xA200,
0xA300,

0xa422,//2 Line
0xa522,
0xa622,
0xa722,
0xa822,
0xa922,

0xaa44,//3 Line
0xab44,
0xac88,
0xad88,
0xae44,
0xaf44,

0xb044,//4 Line
0xb144,
0xb28c,
0xb3c8,
0xb444,
0xb544,

0xb644,//5 Line
0xb78c,
0xb8cc,
0xb9cc,
0xbac8,
0xbb44,

0xbc44,//6 Line
0xbd8c,
0xbecc,
0xbfcc,
0xc0c8,
0xc144,

0xc248,//7 Line
0xc3cc,
0xc4cc,
0xc5cc,
0xc6cc,
0xc784,

0xc844,//8 Line
0xc9aa,
0xcaaa,
0xcbaa,
0xccaa,
0xcd44,

0xce44,//9 Line
0xcf44,
0xd044,
0xd144,
0xd244,
0xd344,

//EV
0x03d3,
0x7a00, //target offset

//Saturation
0x03d3,
0x7b00,
0x7c00,

//AWB
0x03c8,
0x12e0, //Adaptive e0, manual a0

//Sharpness -1
	0x03de, //DE Page(Outdoor)
	0xd911, //Outdoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e1, //E1 Page(Indoor)
	0xd911, //Indoor 11cf //Bayer Post STD gain Neg/Pos

	0x03e4, //E4 Page(Dark1)
	0xd911,//Dark1 11cf //Bayer Post STD gain Neg/Pos

	0x03e7, //E7 Page(Dark2)
	0xd911,//Dark2 11cf //Bayer Post STD gain Neg/Pos

0x03c7,
0x1101, //AE Reset
0xff01,
0x4C00,//SW ExpMin	 = 8800
0x4D00,	
0x4E22,	
0x4F60,	

0x0320,//HW ExpMin  = 8800
0x2800,
0x2922,
0x2a60,

  0x0310,
  0x1402, // d gain off

0x03c7,
0x10d0,	//AE On 50hz f0, 60hz d0

0x0300,
0x0100, //sleep off

};
#endif
#endif /* _SR352_REGS_H_ */

