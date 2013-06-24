/*
 * Simple driver for Texas Instruments
 *    LM48560 High Voltage Class H Ceramic Speaker Driver
 * Copyright (C) 2013 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __LINUX_LM48560_H
#define __LINUX_LM48560_H

#define LM48560_NAME "lm48560"

enum lm48560_enable {
    LM48560_SHDN = 0x00,
    LM48560_EN   = 0x01,
};


enum lm48560_boost {
    LM48560_BOOST_OFF  = 0x00,
    LM48560_BOOST_EN   = 0x02,
};

enum lm48560_insel {
    LM48560_INSEL1 = 0x00,
    LM48560_INSEL2 = 0x04,
};

enum lm48560_ton {
    LM48560_TWU_15MS = 0x00,
    LM48560_TWU_5MS  = 0x08,
};

enum lm48560_rlt {
    LM48560_RLT_500MS = 0x00,
    LM48560_RLT_380MS = 0x20,
    LM48560_RLT_210MS = 0x40,
    LM48560_RLT_170MS = 0x60,
};

enum lm48560_atk {
    LM48560_ATK_0P83MS = 0x00,
    LM48560_ATK_1P20MS = 0x08,
    LM48560_ATK_1P50MS = 0x10,
    LM48560_ATK_2P20MS = 0x18,
};


enum lm48560_plev {
    LM48560_PLEV_VLIM_DISABLE = 0,
    LM48560_PLEV_14VPP,
    LM48560_PLEV_17VPP,
    LM48560_PLEV_20VPP,
    LM48560_PLEV_22VPP,
    LM48560_PLEV_25VPP,
    LM48560_PLEV_28VPP,
};

enum lm48560_gain {
    LM48560_GAIN_00DB_B21DB = 0,
    LM48560_GAIN_06DB_B24DB ,
    LM48560_GAIN_12DB_B27DB ,
    LM48560_GAIN_18DB_B30DB ,
};

/* @en      : device enable
 * @boost   : boost enable
 * @insel   : input select
 * @ontime  : turn on time
 * @rtime   : release time
 * @atime   : attack time
 * @outlimit: output limit
 * @gain    : gain
 */

struct lm48560_platform_data {
                /* initial values */
    enum lm48560_enable en;
    enum lm48560_boost  boost;
    enum lm48560_insel  insel;
    enum lm48560_ton    ontime;
    enum lm48560_rlt    rtime;
    enum lm48560_atk    atime;
    enum lm48560_plev   outlimit;
    enum lm48560_gain   gain;
};

int lm48560_chip_enable(enum lm48560_enable en);

#endif /* __LINUX_LM48560_H */
