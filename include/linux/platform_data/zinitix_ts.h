/*
 * zinitix_ts.h - Platform data for Zinitix touch driver
 *
 * Copyright (C) 2011 Google Inc.
 * Author: Dima Zavin <dima@android.com>
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _LINUX_ZINITIX_TOUCH_H
#define _LINUX_ZINITIX_TOUCH_H

#define ZINITIX_NAME	"zinitix_touch"

extern struct tsp_callbacks *charger_callbacks;
struct tsp_callbacks {
	void (*inform_charger)(struct tsp_callbacks *tsp_cb, bool mode);
};

struct zinitix_ts_platform_data {
	bool	invert_x;
	bool	invert_y;

	int	gpio_sda;
	int	gpio_scl;
	int	gpio_int;
	int	gpio_lcd_type;
	int	(*mux_fw_flash)(bool);
	void (*vdd_on)(bool);
	int (*is_vdd_on)(void);
	void	(*register_cb)(struct tsp_callbacks *);
	const char	*fw_name;
	const u8	*touchkey_keycode;
	int	check_module_type;
	void (*tkey_led_vdd_on)(bool);
	const char *tsp_ic_name;
};
extern struct class *sec_class;
extern int touch_is_pressed;
extern int system_rev;

extern int poweroff_charging;
/*
extern unsigned char LCD_Get_Value(void);
*/
#endif /* _LINUX_ZINITIX_TOUCH_H */
