/*
 * ks02-gpio.h
 *
 * header file supporting gpio functions for Samsung device
 *
 * COPYRIGHT(C) Samsung Electronics Co., Ltd. 2006-2011 All Right Reserved.
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
#include <linux/mfd/pm8xxx/pm8038.h>
#include <linux/mfd/pm8xxx/pm8921.h>

#define PM8038_GPIO_BASE		NR_GPIO_IRQS
#define PM8038_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8038_GPIO_BASE)
#define PM8917_GPIO_PM_TO_SYS(pm_gpio)	PM8038_GPIO_PM_TO_SYS(pm_gpio)

/* MSM8930 GPIO */
#define	GPIO_CAM_IO_EN			82
#define	GPIO_MSM_FLASH_NOW		-1
#define	GPIO_SUB_CAM_MCLK		4
#define	GPIO_MAIN_CAM_MCLK		5
#define	GPIO_CAM_CORE_EN			6
#define	GPIO_VT_STBY				99
#define	GPIO_CAM_A_EN				47
//#define	GPIO_MSM_FLASH_CNTL_EN	64
//#define	GPIO_CAM_AF_EN			66
#define	GPIO_CAM2_RST_N			76
#define	GPIO_CAM1_RST_N			120
#define GPIO_I2C_DATA_CAM			20
#define GPIO_I2C_CLK_CAM			21
#ifdef CONFIG_MACH_KS02
#define GPIO_CAM_FLASH_SET			54
#define GPIO_CAM_FLASH_EN			91
#define GPIO_I2C_DATA_AF			42
#define GPIO_I2C_CLK_AF			43
#define CAM_IO_EN     PM8917_GPIO_PM_TO_SYS(33)
#else
#define GPIO_I2C_DATA_AF			20
#define GPIO_I2C_CLK_AF			21
#endif

#define GPIO_TSP_SEL		0
#define GPIO_TOUCH_IRQ		97
#define GPIO_TOUCH_SDA		16
#define GPIO_TOUCH_SCL		17
#define GPIO_UART_RXD			23
#define GPIO_HOLD_KEY		90
#define GPIO_DATA_KEY		46
#define GPIO_VOLUME_UP		50
#define GPIO_TKEY_LED			51
//#define GPIO_FLASH_LED_UNLOCK   64

#define GPIO_TKEY_INT		94
#define GPIO_SD_CARD_DET_N	93
#define GPIO_VIB_PWM			70
#define GPIO_VIB_EN			75
//#define GPIO_S_LED_I2C_SDA		71
#define GPIO_S_LED_I2C_SCL		72
#define GPIO_USB_I2C_SDA		34 
#define GPIO_USB_I2C_SCL        35
#define GPIO_TSP_D_EN			18
#define GPIO_TSP_A_EN			80
#define GPIO_VOLUME_DOWN	81

/* AUDIO */
#define GPIO_CODEC_INT			69
#define GPIO_CODEC_RESET		PM8917_GPIO_PM_TO_SYS(29)
#define GPIO_CODEC_SDA			36
#define GPIO_CODEC_SCK			37

#define GPIO_SPKR_I2S_TX_SCK		55
#define GPIO_SPKR_I2S_TX_WS		56
#define GPIO_SPKR_I2S_TX_DIN		57
#define GPIO_AUDIO_MCLK_REV10			53 //the GPIO_AUDIO_MCLK pin was not changed in ks02 as H/W rev
#define GPIO_AUDIO_MCLK			53
#define GPIO_SPKR_I2S_RX_SCK		60
#define GPIO_SPKR_I2S_RX_DOUT		61
#define GPIO_SPKR_I2S_RX_WS		62

#define GPIO_VPS_AMP_EN		PM8038_GPIO_PM_TO_SYS(18)
#define GPIO_SPK_AMP_EN		PM8038_GPIO_PM_TO_SYS(21)
#define GPIO_EAR_MIC_BIAS_EN		151
#define GPIO_SUB_MIC_BIAS_EN		89 

#ifndef CONFIG_MUIC_DET_JACK
#define GPIO_SHORT_SENDEND		111 // 94 is used for TKEY_INT, fix later
#define GPIO_EAR_DET			46
#endif

#define GPIO_TOUCHKEY_I2C_SDA		24
#define GPIO_TOUCHKEY_I2C_SCL		25
#define GPIO_2MIC_PW_DN			52
#define GPIO_RCV_SEL			79

/* BATTERY */
#define GPIO_BATT_INT                   7
#ifdef CONFIG_MFD_MAX77693
#define GPIO_FUELGAUGE_I2C_SDA          2
#define GPIO_FUELGAUGE_I2C_SCL          3
#define GPIO_FUEL_INT                   98
#define GPIO_IF_PMIC_SDA                34
#define GPIO_IF_PMIC_SCL                35
#define GPIO_IF_PMIC_IRQ                14
#endif

#ifdef CONFIG_LM48560_RCV
#define GPIO_LM48560_SDA        51
#define GPIO_LM48560_SCL         52
#endif
/* FPGA */


/* BT */
#define GPIO_BT_EN PM8917_GPIO_PM_TO_SYS(31)
#define GPIO_BT_WAKE  PM8917_GPIO_PM_TO_SYS(32)

/* SENSORS */
#define GPIO_NFC_SDA_1_8V		95
#define GPIO_NFC_SCL_1_8V		96
#define GPIO_NFC_IRQ			106
#define GPIO_NFC_FIRMWARE		92
#define GPIO_NFC_EN			80

#define GPIO_SENSOR_ALS_SDA		12
#define GPIO_SENSOR_ALS_SCL		13
#define GPIO_LEDA_EN			89
#define GPIO_PROX_INT			49
/* MHL/HDMI */
#define GPIO_MHL_RST			1
#define GPIO_MHL_EN				-1
#define GPIO_MHL_SDA			8
#define GPIO_MHL_SCL			9
#define GPIO_MHL_WAKE_UP		77

#define GPIO_MHL_INT			78 /* 55 for Rev00 */
#define GPIO_MHL_SEL			-1

/* PMIC8038 GPIO */
/* AUDIO */
#define PMIC_GPIO_2MIC_RST		11
/*I2C BUS ID*/
#define I2C_LEDS_BUS_ID			21
//#define GPIO_MAIN_MIC_BIAS_EN   66 
/* OTG */
#define GPIO_OVP_CTRL		PM8038_GPIO_PM_TO_SYS(15)


/* LCD */
#define DISP_RST_GPIO 58
#define DISP_SWITCH_GPIO 67

/* SUB PMIC */
#define GPIO_SUBPMIC_EN			47 // EMUL : 47, REV00 : 2
#define GPIO_SUBPMIC_SDA		12
#define GPIO_SUBPMIC_SCL		13

#define GPIO_HALL_SENSOR_INT 33

/* FM RADIO */
#define GPIO_FM_SDA_1_8V_REV01	28
#define GPIO_FM_SCL_1_8V_REV01	29
#define GPIO_FM_RST_REV01	26

#define GPIO_FM_SDA_1_8V	73
#define GPIO_FM_SCL_1_8V	74
#define GPIO_FM_RST	71
#define GPIO_FM_INT	107

#if defined(CONFIG_WCD9304_CLK_9600)
#define CLK_REVISION 0
#endif
/* gpio for changed list */
enum {
	BOARD_REV00,
	BOARD_REV01,
	BOARD_REV02,
	BOARD_REV03,
	BOARD_REV04,
	BOARD_REV05,
	BOARD_REV06,
	BOARD_REV07,
	BOARD_REV08,
	BOARD_REV09,
	BOARD_REV10,
	BOARD_REV11,
	BOARD_REV12,
	BOARD_REV13,
	BOARD_REV14,
	BOARD_REV15,
	GPIO_REV_MAX,
};
