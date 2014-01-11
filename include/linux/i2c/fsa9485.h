/*
 * Copyright (C) 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Wonguk Jeong <wonguk.jeong@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef _FSA9485_H_
#define _FSA9485_H_

#include <linux/mfd/pm8xxx/pm8921-charger.h>

enum {
	FSA9485_DETACHED,
	FSA9485_ATTACHED,
	TSU6721_ATTACHED,
};

enum {
	DISABLE,
	ENABLE
};

enum {
	FSA9485_DETACHED_DOCK = 0,
	FSA9485_ATTACHED_DESK_DOCK,
	FSA9485_ATTACHED_CAR_DOCK,
};

enum {
	USB_CALL = 0,
	CDP_CALL,
	UART_CALL,
	CHARGER_CALL,
	OTG_CALL,
	JIG_CALL,
	DESKDOCK_CALL,
	CARDOCK_CALL,
	SMARTDOCK_CALL,
	AUDIODOCK_CALL,
	MHL_CALL,
	INCOMPATIBLE_CALL,
};

struct fsa9485_platform_data {
	void (*callback)(enum cable_type_t cable_type, int attached);
	void (*oxp_callback)(int state);
	void (*mhl_sel) (bool onoff);
	int	(*dock_init) (void);
};

enum {
	SWITCH_PORT_AUTO = 0,
	SWITCH_PORT_USB,
	SWITCH_PORT_AUDIO,
	SWITCH_PORT_UART,
	SWITCH_PORT_VAUDIO,
	SWITCH_PORT_USB_OPEN,
	SWITCH_PORT_ALL_OPEN,
};

extern void fsa9485_manual_switching(int path);
extern void fsa9485_otg_detach(void);
#if defined(CONFIG_VIDEO_MHL_V2)
extern int dock_det(void);
#endif
extern int check_jig_state(void);

extern int poweroff_charging;
extern struct class *sec_class;

#endif /* _FSA9485_H_ */
