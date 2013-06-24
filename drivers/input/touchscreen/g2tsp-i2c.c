#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/g2tsp_platform.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>

#include <asm/system.h>
#include <asm/uaccess.h>

#include "g2tsp.h"

#if (G2TSP_NEW_IC == 0)
#include "g2touch_i2c.h"
#endif

struct g2tsp_i2c {
    struct g2tsp_bus_ops ops;
    struct i2c_client *client;
    void *g2tsp_handle;
};

static s32 g2tsp_i2c_read_reg(void *handle, u8 addr, u8 *value)
{
    int retval = 0;
    struct g2tsp_i2c *ts = container_of(handle, struct g2tsp_i2c, ops);

    retval = i2c_master_send(ts->client, &addr, 1);
    if (retval < 0) {
		printk("%s() send Fail !! \n", __func__);	
        return retval;
    }
    retval = i2c_master_recv(ts->client, value, 1);

    return (retval < 0) ? retval : 0;
}

static s32 g2tsp_i2c_write_reg(void *handle, u8 addr, u8 value)
{
    u8 data[I2C_SMBUS_BLOCK_MAX+1];
    int retval;
    struct g2tsp_i2c *ts = container_of(handle, struct g2tsp_i2c, ops);

//	g2debug2("%s\n", __func__);
    data[0] = addr;
	data[1] = value;
    retval = i2c_master_send(ts->client, data, 2);
    if (retval < 0) {
		printk("%s() Fail !! \n", __func__);	
        return retval;
    }

    return 0;
}

static s32 g2tsp_i2c_read_block_data(void *handle, u8 addr,
    u8 length, void *values)
{
    int retval = 0;
    struct g2tsp_i2c *ts = container_of(handle, struct g2tsp_i2c, ops);

	g2debug1("%s\n", __func__);

    retval = i2c_master_send(ts->client, &addr, 1);
    if (retval < 0){
		printk("%s() send Fail !! \n", __func__);	
        return retval;
    }
    retval = i2c_master_recv(ts->client, values, length);

    return (retval < 0) ? retval : 0;
}

static s32 g2tsp_i2c_write_block_data(void *handle, u8 addr,
    u8 length, const void *values)
{
    u8 data[I2C_SMBUS_BLOCK_MAX+1];
    int num_bytes, count;
    int retval;
    struct g2tsp_i2c *ts = container_of(handle, struct g2tsp_i2c, ops);

	g2debug2("%s\n", __func__);
    num_bytes = length;
    data[0] = addr;
    count = (num_bytes > I2C_SMBUS_BLOCK_MAX) ?
        I2C_SMBUS_BLOCK_MAX : num_bytes;
    memcpy(&data[1], values, count+1);
    num_bytes -= count;
    retval = i2c_master_send(ts->client, data, count+1);
    if (retval < 0) {
		printk("%s() send fail !! \n", __func__);	
        return retval;
    }
    while (num_bytes > 0) {
        count = (num_bytes > I2C_SMBUS_BLOCK_MAX) ?
            I2C_SMBUS_BLOCK_MAX : num_bytes;
        memcpy(&data[0], values, count);
        num_bytes -= count;
        retval = i2c_master_send(ts->client, data, count);
        if (retval < 0)
            return retval;
    }

    return 0;
}


static int __devinit g2tsp_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct g2tsp_i2c *ts;
    int retval;
	
	g2debug1("%s\n", __func__);
	
    /* allocate and clear memory */
    ts = kzalloc(sizeof(*ts), GFP_KERNEL);
    if (ts == NULL) {
        printk(KERN_ERR "%s: Error, kzalloc.\n", __func__);
        retval = -ENOMEM;
        goto error_alloc_data_failed;
    }

    /* register driver_data */
#if (G2TSP_NEW_IC == 1)		
    ts->client = client;
    i2c_set_clientdata(client, ts);

    ts->ops.write_blk = g2tsp_i2c_write_block_data;
    ts->ops.read_blk = g2tsp_i2c_read_block_data;
    ts->ops.write = g2tsp_i2c_write_reg;
    ts->ops.read = g2tsp_i2c_read_reg;
#else
	ts->ops.write_blk = g2touch_i2c_write_block_data;
	ts->ops.read_blk = g2touch_i2c_read_block_data;
	ts->ops.write = g2touch_i2c_write_reg;
	ts->ops.read = g2touch_i2c_read_reg;
	g2touch_port_init();
#endif

    ts->g2tsp_handle = g2tsp_init(&ts->ops, &client->dev);
    if (!ts->g2tsp_handle)
        goto tsp_err;

    return 0;

tsp_err:
    kfree(ts);
error_alloc_data_failed:
    return retval;
}

static int __devexit g2tsp_i2c_remove(struct i2c_client *client)
{
    struct g2tsp_i2c *ts;

	g2debug("%s\n", __func__);
    ts = i2c_get_clientdata(client);
    g2tsp_release(ts->g2tsp_handle);

    kfree(ts);

    return 0;
}

static const struct i2c_device_id g2tsp_i2c_id[] = {
    { G2TSP_I2C_NAME, 0 },  { }
};

static struct i2c_driver g2tsp_i2c_driver = {
    .driver = {
        .name = G2TSP_I2C_NAME,
        .owner = THIS_MODULE,
    },
    .probe = g2tsp_i2c_probe,
    .remove = __devexit_p(g2tsp_i2c_remove),
    .id_table = g2tsp_i2c_id,
};

static int g2tsp_i2c_init(void)
{
	g2debug1("%s\n", __func__);
    return i2c_add_driver(&g2tsp_i2c_driver);
}

static void g2tsp_i2c_exit(void)
{
	g2debug1("%s\n", __func__);
    return i2c_del_driver(&g2tsp_i2c_driver);
}

module_init(g2tsp_i2c_init);
module_exit(g2tsp_i2c_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("G2TOUCH I2C");
MODULE_AUTHOR("");
MODULE_DEVICE_TABLE(i2c, g2tsp_i2c_id);




