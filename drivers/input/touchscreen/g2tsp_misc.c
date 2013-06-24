/*
		G2TOUCH.INC
*/
#include <linux/workqueue.h>
#include <linux/firmware.h>
#include <linux/g2tsp_platform.h>
#include <linux/earlysuspend.h>
#include <linux/delay.h>
#include <linux/gpio.h>


#include "g2tsp.h"
#include "g2tsp_misc.h"


//#define G2TSP_USE_OLD_CHIP	0

void g2tsp_flash_eraseall(struct g2tsp *ts)
{
    int i;
	int err;

	g2debug("%s(Enter) %d\n", __func__, __LINE__);

	// reset
	ts->platform_data->reset();	

	ts->bus_ops->write(ts->bus_ops, 0x00, 0x10);
    ts->bus_ops->write(ts->bus_ops, 0x01, 0x01); 
	ts->bus_ops->write(ts->bus_ops, 0x00, 0x00);
	mdelay(50);
    ts->bus_ops->write(ts->bus_ops, 0xcc, 0x80);   //partial Erase
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0x98);

    udelay(1);

    for(i = 0; i < 1000; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc8);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd8);
    }

    for(i = 0; i < 8; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xcc);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xdc);
    }

    // BIST Command Start
    for(i = 0; i < 7; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc5);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd5);
    }

    for(i = 0; i < 40; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc4);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd4);
    }

    for(i = 0; i < 4; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc5);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd5);
    }

    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc4);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd4);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc5);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd5);

    for(i = 0; i < 6; i++)
    {
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xc4);
        ts->bus_ops->write(ts->bus_ops, 0xc7, 0xd4);
    }

    // BIST Command End

    // Internal TCK set to SCL
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xcc);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xdc);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0xfc);

 	g2debug("%s(Change I2C to GPIO) %d\n", __func__, __LINE__);
#if (G2TSP_NEW_IC == 1)		
		
    //ts->platform_data->i2c_to_gpio(1);							// switch from I2C to GPIO

		err = gpio_request(ts->platform_data->gpio_scl, "SCL_");
		if (err)
			printk(KERN_ERR "#### failed to request SCL_ ####\n");
		
		err = gpio_request(ts->platform_data->gpio_sda, "SDA_");
		if (err)
			printk(KERN_ERR "#### failed to request SDA_ ####\n");

    ts->platform_data->i2c_to_gpio(1);							// switch from I2C to GPIO
			
#endif

		gpio_direction_output(ts->platform_data->gpio_scl, 0);
		gpio_direction_output(ts->platform_data->gpio_sda, 0);

		udelay(1);
		gpio_set_value(ts->platform_data->gpio_sda, 0);
	    udelay(1);
		gpio_set_value(ts->platform_data->gpio_scl, 0);
	    udelay(1);
		gpio_set_value(ts->platform_data->gpio_scl, 1);
		udelay(1);
	
    // Internal Flash Erase Operation Start
    for(i = 0; i < 1620000; i++)
    {
		gpio_set_value(ts->platform_data->gpio_scl, 0);
		gpio_set_value(ts->platform_data->gpio_scl, 1);			
    }

 	g2debug("%s(Change GPIO to I2C) %d\n", __func__, __LINE__);
    // Internal Flash Erase Operation End
#if (G2TSP_NEW_IC == 1)
	{
		gpio_set_value(ts->platform_data->gpio_scl, 1);	
		gpio_free(ts->platform_data->gpio_scl);
		gpio_free(ts->platform_data->gpio_sda);
        ts->platform_data->i2c_to_gpio(0);                          // switch from GPIO to I2C   	
	}
#else
	{
		gpio_set_value(ts->platform_data->gpio_scl, 1);
	}
#endif


    // SCL is returned to I2C, Mode Desable.
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0x88);
    ts->bus_ops->write(ts->bus_ops, 0xc7, 0x00);

	// remark 2012.11.05
	udelay(10);
    ts->bus_ops->write(ts->bus_ops, 0xcc, 0x00);

		
    mdelay(50);

	g2debug(" FLASH ERASE Complete !!\n");

}

				   
void g2tsp_firmware_load(struct g2tsp 	*ts, const unsigned char *data, size_t size)
{
	int	i;
	int latest_version[0];
	
	latest_version[0] = ts->platform_data->fw_version[0];
	latest_version[1] = ts->platform_data->fw_version[1];

	g2debug("Curent device version = 0x%06x.%06x, latest version = 0x%06x.%06x \r\n",
					ts->current_firmware_version[0],ts->current_firmware_version[1], latest_version[0],latest_version[1]);

	if((ts->current_firmware_version[0] != latest_version[0])||(ts->current_firmware_version[1] != latest_version[1]))
	{

		g2debug("G2TOUCH: Firmware Update Start !!\r\n");

		// erase flash
		g2tsp_flash_eraseall(ts);

	 	// write binary
		ts->bus_ops->write(ts->bus_ops, 0xa1, 0x00);
		ts->bus_ops->write(ts->bus_ops, 0xa2, 0x00);
		ts->bus_ops->write(ts->bus_ops, 0xa3, 0x00);
		ts->bus_ops->write(ts->bus_ops, 0xa0, 0x01);

		for(i=0;i<size;i++)	
		{
			if((i >= 0xF0) && (i <= 0xF3)) 
				ts->bus_ops->write(ts->bus_ops, 0xa4, (ts->jigId[i&0x0f] & 0xff));
			else 
			ts->bus_ops->write(ts->bus_ops, 0xa4, (data[i] & 0xff));
			
			
			ts->bus_ops->write(ts->bus_ops, 0xa5, 0x01);
			ts->bus_ops->write(ts->bus_ops, 0xa5, 0x00);
		}

		ts->bus_ops->write(ts->bus_ops, 0xa4, 0x00);
		ts->bus_ops->write(ts->bus_ops, 0xa5, 0x01);
		ts->bus_ops->write(ts->bus_ops, 0xa0, 0x00);
		ts->bus_ops->write(ts->bus_ops, 0xa5, 0x00);

		// remark 2012.11.05
		ts->bus_ops->write(ts->bus_ops, 0x01, 0x00);	//Flash Enable	
		ts->platform_data->reset();
		
		mdelay(10);
		ts->bus_ops->write(ts->bus_ops, 0x00, 0x10);	//soft reset	
		mdelay(1);
		ts->bus_ops->write(ts->bus_ops, 0x30, 0x18);	//soft reset	
		ts->bus_ops->write(ts->bus_ops, 0x00, 0x00);	//soft reset	
		mdelay(100);

		g2debug("written 0x%x Bytes finished. \r\n", i);
		g2debug("Firmware Download Completed  \r\n");
	}
	else
	{
		g2debug("current version is the latest version !! \r\n");
	}
	
	//ts->firmware_download = 0;
	return;
}


void firmware_request_handler(const struct firmware *fw, void *context)
{
	struct g2tsp 	*ts;
	ts = (struct g2tsp*) context;

	printk("[G2TSP] : %s() %d \n", __func__,__LINE__);

	if(fw != NULL){
		printk("[G2TSP] : %s() %d \n", __func__,__LINE__);
		g2tsp_firmware_load(ts, fw->data, fw->size);
		printk("[G2TSP] : %s() %d \n", __func__,__LINE__);
		release_firmware(fw);
		printk("[G2TSP] : %s() %d \n", __func__,__LINE__);
	} else {
	printk("[G2TSP] : %s() %d \n", __func__,__LINE__);
        printk(KERN_ERR"failed to load G2TOuch firmware will not working\n");
    	}
}







