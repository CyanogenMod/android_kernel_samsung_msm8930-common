/*
* Simple driver for Texas Instruments LM48560 Speaker driver chip
* Copyright (C) 2013 Texas Instruments
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
*/
#define DEBUG
#define _DEBUG

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/regmap.h>
#include <linux/i2c/lm48560.h>


#define REG_SHDN        0x00
#define REG_NCLIP_CTL   0x01
#define REG_GAIN_CTL    0x02
#define REG_MAX          REG_GAIN_CTL

#define MASK_ENABLE     0x01

struct lm48560_chip_data {
    struct device *dev;
    struct lm48560_platform_data *pdata;
    struct regmap *regmap;
    struct mutex iolock;
    struct i2c_client *i2c;
};

static struct lm48560_chip_data *pchip;

static bool init_done = false;
int lm48560_power_enable(enum lm48560_enable en)
{
    return regmap_update_bits(pchip->regmap, REG_SHDN, MASK_ENABLE, en);
}
EXPORT_SYMBOL(lm48560_power_enable);

/* initialize chip */

int lm48560_write_reg(struct i2c_client *i2c, u8 reg, u8 value)
{
    
    struct lm48560_chip_data *pchip = i2c_get_clientdata(i2c);
    
    
    int ret;
    //int read_value;
    
    
    mutex_lock(&pchip->iolock);
    
    
    ret = i2c_smbus_write_byte_data(i2c, reg, value);
    //read_value = i2c_smbus_read_byte_data(i2c, reg);
    
    
    if (ret < 0)
    {
        pr_info("fail %s, reg(0x%x), ret(%d), %s\n", __func__, reg, ret, i2c->name);
    }
    else
    {
        pr_info("OK %s, reg(0x%x), ret(%d), %s, write %x\n", __func__, reg, ret, i2c->name, value);
        //pr_info("OK %s, reg(0x%x), ret(%d), %s, write %x, read %x\n", __func__, reg, ret, i2c->name, value, read_value);
    }
    mutex_unlock(&pchip->iolock);
    
    
    return ret;
    
    
}

static int lm48560_chip_init(struct lm48560_chip_data *pchip)
{
    int ret;
    unsigned int reg_val;
    struct lm48560_platform_data *pdata = pchip->pdata;

    reg_val = pdata->en | pdata->boost | pdata->insel | pdata->ontime;
    ret = lm48560_write_reg(pchip->i2c, REG_SHDN, reg_val);
    if (ret < 0)
goto out;

reg_val = pdata->rtime | pdata->atime | pdata->outlimit;
    ret = lm48560_write_reg(pchip->i2c, REG_NCLIP_CTL, reg_val);
    if (ret < 0)
goto out;

    ret = lm48560_write_reg(pchip->i2c, REG_GAIN_CTL, pdata->gain);
    if (ret < 0)
goto out;

    return ret;

out:
    dev_err(pchip->dev, "i2c failed to access register\n");
    return ret;
}

static const struct regmap_config lm48560_regmap = {
        .reg_bits = 8,
        .val_bits = 8,
        .max_register = REG_MAX,
};

int lm48560_chip_enable(enum lm48560_enable en)
{
    int ret;
    unsigned int reg_val;
    struct lm48560_platform_data *pdata = pchip->pdata;

    if(init_done == false)
    {
        pr_info("%s\n device init failed", __func__);
        return -1;
    }
    pr_info("%s enable %d boost %x, insel %x, ontime %x", __func__, en, pdata->boost, pdata->insel , pdata->ontime );
    pdata->boost = LM48560_BOOST_EN;
    pdata->insel = LM48560_INSEL2;
    pdata->ontime = LM48560_TWU_15MS;
     if(en)
       pdata->en = LM48560_EN;
     else
       pdata->en = LM48560_SHDN;
     
     reg_val = pdata->en | pdata->boost | pdata->insel | pdata->ontime;
     ret = lm48560_write_reg(pchip->i2c, REG_SHDN, reg_val);
     if (ret < 0)
       pr_info( "i2c failed to access register\n");
     return ret;
}
EXPORT_SYMBOL(lm48560_chip_enable);

static int lm48560_probe(struct i2c_client *client,
                                                                 const struct i2c_device_id *id)
{
    struct lm48560_platform_data *pdata = client->dev.platform_data;
    //struct lm48560_chip_data *pchip;
    int ret;
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
                    dev_err(&client->dev, "fail : i2c functionality check...\n");
        return -EOPNOTSUPP;
    }

    if (pdata == NULL) {
        dev_err(&client->dev, "fail : no platform data.\n");
        return -ENODATA;
    }

    pchip = devm_kzalloc(&client->dev, sizeof(struct lm48560_chip_data),
                                        GFP_KERNEL);
    if (!pchip) {
                    return -ENOMEM;
    }
    pchip->pdata = pdata;
    pchip->dev = &client->dev;
    pchip->i2c = client;

    mutex_init(&pchip->iolock);

    pchip->regmap = devm_regmap_init_i2c(client, &lm48560_regmap);
    if (IS_ERR(pchip->regmap)) {
        ret = PTR_ERR(pchip->regmap);
        dev_err(&client->dev, "fail : allocate register map: %d\n",
                        ret);
        return ret;
    }
    i2c_set_clientdata(client, pchip);

    /* chip initialize */
    ret = lm48560_chip_init(pchip);
    if (ret < 0) {
                    dev_err(&client->dev, "fail : init chip\n");
                    goto err_chip_init;
    }
    dev_info(&client->dev, "LM48560 Init Done.\n");
    init_done = true;
    return 0;

err_chip_init:
    return ret;
}

static int lm48560_remove(struct i2c_client *client)
{
    int ret;
    struct lm48560_chip_data *pchip = i2c_get_clientdata(client);

    ret = lm48560_write_reg(pchip->i2c, REG_SHDN, 0x00);
    if (ret < 0)
                    dev_err(pchip->dev, "i2c failed to access register\n");

    return 0;
}

static const struct i2c_device_id lm48560_id[] = {
                {LM48560_NAME, 0},
                {}
};

MODULE_DEVICE_TABLE(i2c, lm48560_id);

static struct i2c_driver lm48560_i2c_driver = {
    .driver = {
                      .name = LM48560_NAME,
                      },
    .probe = lm48560_probe,
    .remove = lm48560_remove,
    .id_table = lm48560_id,
};

module_i2c_driver(lm48560_i2c_driver);

MODULE_DESCRIPTION("Texas Instruments Backlight driver for LM48560");
MODULE_AUTHOR("Billy Yun <billy.yun@ti.com>");
MODULE_LICENSE("GPL v2");
