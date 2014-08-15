/*
 * express-gpio.h
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

/* MSM8960 GPIO */
#define GPIO_MDP_VSYNC			0
#define GPIO_MHL_RST			1
#define GPIO_MSM_FLASH_CNTL_EN	2
#define GPIO_CAM_MCLK2			2	/* >= REV03 */
#define GPIO_MSM_FLASH_NOW		3
#define GPIO_MAIN_CAM_STBY		4
#define GPIO_CAM_MCLK				5
#define GPIO_CAM_CORE_EN			6
#define GPIO_CODEC_I2C_SDA              8
#define GPIO_CODEC_I2C_SCL              9
#define GPIO_LCD_22V_EN			10
#define GPIO_MXT_TS_IRQ			11
#define GPIO_ALS_SDA			12
#define GPIO_ALS_SCL			13
#define GPIO_FPGA_CS			14
#define GPIO_MHL_WAKE_UP		15
#define GPIO_VT_STBY				18

#define GPIO_MHL_EN				19
#define GPIO_I2C_DATA_CAM				20
#define GPIO_I2C_CLK_CAM				21
#define GPIO_FUELGAUGE_I2C_SDA		24
#define GPIO_FUELGAUGE_I2C_SCL		25

#define GPIO_NFC_SDA			32
#define GPIO_NFC_SCL			33
#ifdef CONFIG_VP_A2220
#define GPIO_A2220_I2C_SDA		36
#define GPIO_A2220_I2C_SCL		37
#endif
#define GPIO_KEY_HOME			40

#define GPIO_ALS_INT			-1 /*42*/
#define GPIO_SENSOR_SNS_SDA		44
#define GPIO_SENSOR_SNS_SCL		45
#define GPIO_CAM_A_EN			46
#define GPIO_HAPTIC_PWR_EN              47

#define GPIO_VOLUME_UP			50
#define GPIO_MXT_TS_LDO_EN		-1
#define GPIO_TKEY_LED			51
#define GPIO_MXT_TS_RESET		-1
#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH
#define	GPIO_TOUCH_KEY_INT_02	52
#define	GPIO_TOUCH_KEY_INT		54
#endif

#define GPIO_SENSOR_ALS_SDA		63
#define GPIO_SENSOR_ALS_SCL		64
#define GPIO_MAG_RST			66
#define GPIO_FUEL_INT			67
#ifdef CONFIG_MPU_SENSORS_MPU6050B1_411
#define GPIO_MSENSE_RST                 68
#define GPIO_MPU3050_INT                69
#endif
#define GPIO_VIB_PWM                    70

#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH
#define GPIO_TOUCHKEY_SDA		71
#define GPIO_TOUCHKEY_SCL		72
#endif
#define GPIO_USB_I2C_SDA		73
#define GPIO_USB_I2C_SCL		74
#define GPIO_CAM2_RST_N				76
#define GPIO_MHL_SEL			82

#define GPIO_VOLUME_DOWN		81
#define GPIO_KS8851_RST			89
#define GPIO_KS8851_IRQ			90

#define GPIO_NFC_FIRMWARE		92
#ifdef CONFIG_KEYBOARD_CYPRESS_TOUCH
#define	GPIO_TOUCH_KEY_EN		94
#endif
#define GPIO_MHL_SDA			95
#define GPIO_MHL_SCL			96
#define GPIO_MHL_INT			99

#define GPIO_NFC_IRQ			106
#define GPIO_CAM1_RST_N				107

/* MSM8960 NC GPIOs */
#define GPIO_NC_7				7
#define GPIO_NC_42				42
#define GPIO_NC_68				68

/* ES305B GPIO */
#define MSM_AUD_A2220_WAKEUP		35
#define MSM_AUD_A2220_RESET		75

/* PMIC8921 MPP */
#define PMIC_MPP_FLASH_LED_UNLOCK       4

/* PMIC8921 GPIO */
#define PMIC_GPIO_CHG_EN		2
#define PMIC_GPIO_VIB_ON		4
#define PMIC_GPIO_LINEOUT_EN		5
#define PMIC_MSM_FLASH_CNTL_EN		5
#define PMIC_GPIO_ALS_INT               6
#define PMIC_GPIO_CHG_STAT		17
#define PMIC_GPIO_SPK_EN		18
#define PMIC_GPIO_VPS_EN		19 /* NC */
#define PMIC_GPIO_NFC_EN		21
#define PMIC_GPIO_OTG_EN		22
#define PMIC_GPIO_BATT_INT		37
#define PMIC_GPIO_CODEC_RST		38
#define PMIC_GPIO_OTG_POWER		42
#define PMIC_GPIO_LCD_RST		43

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

enum {
	MDP_VSYNC,
	PS_INT,
};
