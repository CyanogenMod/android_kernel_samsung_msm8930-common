#ifndef _GPIO_KEYS_H
#define _GPIO_KEYS_H

struct device;

#if defined(CONFIG_KEYBOARD_GPIO_EXTENDED_RESUME_EVENT)
enum KEY_SUPPORT {
	NOT_SUPPORT_RESUME_KEY_EVENT = 0,
	SUPPORT_RESUME_KEY_EVENT
};
enum KEY_FORCE_REPORT {
	FORCE_KEY_REPORT_OFF = 0,
	FORCE_KEY_REPORT_ON
};
#endif

struct gpio_keys_button {
	/* Configuration parameters */
	unsigned int code;	/* input event code (KEY_*, SW_*) */
	int gpio;		/* -1 if this key does not support gpio */
	int active_low;
	const char *desc;
	unsigned int type;	/* input event type (EV_KEY, EV_SW, EV_ABS) */
	int wakeup;		/* configure the button as a wake-up source */
	int debounce_interval;	/* debounce ticks interval in msecs */
	bool can_disable;
	int value;		/* axis value for EV_ABS */
	unsigned int irq;	/* Irq number in case of interrupt keys */
#if defined(CONFIG_KEYBOARD_GPIO_EXTENDED_RESUME_EVENT)
	int support_evt;	/* enable to support resume gpio events */
#endif
};

struct gpio_keys_platform_data {
	struct gpio_keys_button *buttons;
	int nbuttons;
	unsigned int poll_interval;	/* polling interval in msecs -
					   for polling driver only */
	unsigned int rep:1;		/* enable input subsystem auto repeat */
	int (*enable)(struct device *dev);
	void (*disable)(struct device *dev);
	const char *name;		/* input device name */
#ifdef CONFIG_SENSORS_HALL
	int gpio_flip_cover;
#endif
};

#endif
