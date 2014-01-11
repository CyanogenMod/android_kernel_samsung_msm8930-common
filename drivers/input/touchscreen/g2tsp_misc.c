/*
		G2TOUCH.INC
*/
#include <linux/workqueue.h>
#include <linux/firmware.h>
#include <linux/g2tsp_platform.h>
#include <linux/earlysuspend.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>


#include "g2tsp.h"
#include "g2tsp_misc.h"

extern struct g2tsp *current_ts;

extern void g2tsp_watchdog_enable(struct g2tsp *ts, u32 msec);
extern void g2tsp_dbg_event(struct g2tsp *ts, u8 event, u16 len, u8 *data);
extern int g2tsp_FactoryCal(struct g2tsp *ts);



/*
	referece dac --> it can be changed
*/
const u16 Refence_dac[390] = {
	479,429,448,448,455,439,429,412,402,374,376,364,358,341,325,
	413,423,440,454,442,432,414,405,390,378,366,359,347,337,320, 
	417,428,453,453,453,434,425,408,399,377,375,362,357,340,330, 
	423,443,458,477,461,450,432,423,406,395,381,375,362,353,338, 
	438,453,476,483,479,459,448,431,422,398,395,382,376,361,350, 
	450,474,488,509,490,477,458,448,432,419,405,398,385,377,361, 
	468,486,509,516,510,488,477,459,450,424,421,407,401,388,374, 
	481,507,522,543,521,508,487,477,459,448,431,424,411,404,386, 
	471,522,514,551,526,520,502,488,471,455,449,434,427,414,400, 
	516,545,562,581,557,543,520,510,490,480,460,452,438,432,415, 
	540,560,589,591,584,556,544,523,513,488,479,464,456,441,430, 
	509,588,572,625,579,582,552,547,515,515,492,484,467,462,447, 
	585,607,639,637,628,598,586,562,550,525,516,496,489,473,464, 
	604,641,659,676,645,629,600,589,564,556,531,521,502,497,484, 
	583,660,657,691,657,647,626,607,582,568,559,535,528,511,503, 
	658,701,720,735,699,684,648,639,610,602,576,564,543,538,526, 
	698,723,766,753,739,703,687,656,644,617,607,577,570,550,545, 
	654,766,740,800,729,742,696,692,644,652,620,605,579,576,565, 
	757,784,833,815,796,752,737,700,688,659,648,613,606,583,582, 
	835,886,925,922,881,849,816,788,760,742,714,691,669,655,669, 
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
	  0,  0,  0,346,  0,  0,  0,  0,  0,263,  0,  0,  0,  0,  0 
};


void g2tsp_flash_eraseall(struct g2tsp *ts, u8 cmd)
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
    ts->bus_ops->write(ts->bus_ops, 0xcc, cmd);   //partial Erase
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
	
//    ts->platform_data->i2c_to_gpio(1);							// switch from I2C to GPIO

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
#if G2TSP_NEW_IC
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

	disable_irq_nosync(ts->irq);
	del_timer(&ts->watchdog_timer);
	
	latest_version[0] = ts->platform_data->fw_version[0];
	latest_version[1] = ts->platform_data->fw_version[1];

	g2debug("Curent device version = 0x%06x.%06x, latest version = 0x%06x.%06x \r\n",
					ts->current_firmware_version[0],ts->current_firmware_version[1], latest_version[0],latest_version[1]);

	dbgMsgToCheese("Curent device version = 0x%06x.%06x, latest version = 0x%06x.%06x \r\n",
					ts->current_firmware_version[0],ts->current_firmware_version[1], latest_version[0],latest_version[1]);
	
	if((ts->current_firmware_version[0] != latest_version[0])||(ts->current_firmware_version[1] != latest_version[1]))
	{

		g2debug("G2TOUCH: Firmware Update Start !!\r\n");
		dbgMsgToCheese("G2TOUCH: Firmware Update Start !!\r\n");
			
		// erase flash
		g2tsp_flash_eraseall(ts, 0x80);
		
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

		msleep(10);
		ts->bus_ops->write(ts->bus_ops, 0x00, 0x10);	//soft reset On	
		udelay(10);
		ts->bus_ops->write(ts->bus_ops, 0x01, 0x00);	//Flash Enable	
		ts->bus_ops->write(ts->bus_ops, 0x30, 0x18);	//PowerUp Calibration.	

		ts->workMode = eMode_Normal;

		ts->bus_ops->write(ts->bus_ops, 0x00, 0x00);	//soft reset Off	
		msleep(10);

		g2debug("written 0x%x Bytes finished. \r\n", i);
		g2debug("Firmware Download Completed  \r\n");

		dbgMsgToCheese("Firmware Download Completed  \r\n");

		if(ts->factory_cal == 0) {
			g2debug("%s()-> Run WatchDog Timer !! \r\n", __func__);
			g2tsp_watchdog_enable(ts,5000);
		}

	}
	else
	{
		g2debug("current version is the latest version !! \r\n");
	}

	enable_irq(ts->irq);
	//ts->firmware_download = 0;
	return;
}




#define SWAP16(x)	(((x>>8)& 0x00ff) | ((x<<8)& 0xff00))


void g2tsp_dac_write(struct g2tsp *ts, u8 *data)
{
	u32 addr = 0x013000;
	int i=0,j=0;
	
	// erase flash (dac area)
	g2debug("%s: Enter + \r\n", __func__);
	dbgMsgToCheese("%s: Enter + ", __func__);

	for(i=0; i<390; i++)
	{
		printk("[%04d]", (data[(i*2)]<<8) | data[(i*2)+1]);
		if((i%15)== 14 )	printk("\r\n");
	}
	
	g2tsp_flash_eraseall(ts, 0xb3);	

	// Flash Dac address write
	ts->bus_ops->write(ts->bus_ops, 0xA1, (addr>>16) & 0xff);
	ts->bus_ops->write(ts->bus_ops, 0xA2, (addr>>8) & 0xff);
	ts->bus_ops->write(ts->bus_ops, 0xA3, addr & 0xff);
	ts->bus_ops->write(ts->bus_ops, 0xA0, 0x01);

	// Dac data write
	for(j=0; j<26; j++)
    {
    	for(i=0; i<30; i++)
    	{
    		
			ts->bus_ops->write(ts->bus_ops, 0xA4, data[(j*30)+i]);		
			ts->bus_ops->write(ts->bus_ops, 0xA5, 0x01);		
			ts->bus_ops->write(ts->bus_ops, 0xA5, 0x00);		

			// Write Dummy Data 
			if(i == 29)
			{
				ts->bus_ops->write(ts->bus_ops, 0xA4, 0);		
				ts->bus_ops->write(ts->bus_ops, 0xA5, 0x01);		
				ts->bus_ops->write(ts->bus_ops, 0xA5, 0x00);		
				ts->bus_ops->write(ts->bus_ops, 0xA4, 0);		
				ts->bus_ops->write(ts->bus_ops, 0xA5, 0x01);		
				ts->bus_ops->write(ts->bus_ops, 0xA5, 0x00);		
			}
    	}
    }

	// Dac Write finish
	ts->bus_ops->write(ts->bus_ops, 0xA4, 0x00);
	ts->bus_ops->write(ts->bus_ops, 0xA5, 0x00);
	ts->bus_ops->write(ts->bus_ops, 0xA0, 0x00);
	ts->bus_ops->write(ts->bus_ops, 0xA5, 0x00);
	
	ts->bus_ops->write(ts->bus_ops, 0x01, 0x00);	//
	ts->bus_ops->write(ts->bus_ops, 0x00, 0x10);
	udelay(10);
	ts->bus_ops->write(ts->bus_ops, 0xce, 0x00);	// disable debug mode
	ts->bus_ops->write(ts->bus_ops, 0xc6, 0x00);	// debug reset
	ts->bus_ops->write(ts->bus_ops, 0x30, 0x18);	// powerup-cal enable
	ts->bus_ops->write(ts->bus_ops, 0x00, 0x00);	// standby off--> tsp run

	ts->workMode = eMode_Normal;
	
	g2debug("Factory Cal Data Write Finished !! \r\n");
	dbgMsgToCheese("Factory Cal Data Write Finished !! \r\n");

	
	g2debug(" %s()->Run WatchDog Timer !! \r\n",__func__);
	g2tsp_watchdog_enable(ts,5000);
	
	
}


void firmware_request_handler(const struct firmware *fw, void *context)
{
	struct g2tsp 	*ts;
	ts = (struct g2tsp*) context;

	if(fw != NULL){
		g2tsp_firmware_load(ts, fw->data, fw->size);
		release_firmware(fw);

		if(ts->factory_cal == 1) {
			g2tsp_FactoryCal(ts);
		}
		
	} else {
        printk(KERN_ERR"failed to load G2Touch firmware will not working\n");
    }
}



void PushDbgPacket(struct g2tsp *ts, u8 *src, u16 len)
{
    int i,pos;
    
    u16 head;
    
    head = ts->dbgPacket.head;

    for(i=0; i<len; i++) {
        pos = (head + i) & (DBG_PACKET_SIZE-1);
        ts->dbgPacket.buf[pos]= src[i];
    }
    
    ts->dbgPacket.head = (head + len) & (DBG_PACKET_SIZE-1);  
	g2debug1("%s: head = %d, cmd=0x%02x, len=%d \r\n", __func__, head, src[2], len);
}


int PopDbgPacket(struct g2tsp *ts, u8 *src)
{
    int len = 0;
    
    u16 tail,head;
    u16 ntail;

    tail = ts->dbgPacket.tail;
    head = ts->dbgPacket.head;

    if(tail == head) 
    	return 0;

	//search STX --> 0x02, 0xA3
    while(1)	    
    {
       if(tail == head) 
            break;

        ntail = (tail +1) & (DBG_PACKET_SIZE-1);
        if((ts->dbgPacket.buf[tail] == 0x02) && (ts->dbgPacket.buf[ntail] == 0xA3))    //Get STX Word
        {   
            break;
        }
        
        tail = (tail+1) & (DBG_PACKET_SIZE-1);
    }

    //Search ETX -->0x03,0xB3
    while(1)
    {
        if((tail == head) || (len >= 1204)){
			len = 0;
            break;
        }

        src[len++] = ts->dbgPacket.buf[tail];
        ntail = (tail +1) & (DBG_PACKET_SIZE-1);
		
        if((ts->dbgPacket.buf[tail] == 0x03) && (ts->dbgPacket.buf[ntail] == 0xB3)) {
           src[len++] = 0xB3;
		   tail = ntail;
		   tail = (tail+1) & (DBG_PACKET_SIZE-1);
           break;
        }

        tail = (tail+1) & (DBG_PACKET_SIZE-1);
    }
    
    ts->dbgPacket.tail = tail;
    g2debug1("%s: tail = %d, cmd=0x%02x, len=%d \r\n", __func__, tail, src[2], len);

    
    return len;
}


void MakeI2cDebugBuffer(u8 *dst, u8 *src, u8 fcnt)
{
	u16 i,pos=0;

    pos = (fcnt * 49);

    for(i=0; i<10; i++)
    {
        if(i != 1) {
			dst[pos++] = src[(i*6)];
        }
		
        memcpy(&dst[pos],&src[(i*6)+2],4);
		pos += 4;		
    }
}



bool ReadTSPDbgData(struct g2tsp *ts, u8 type, u8 *frameBuf) 
{
	bool ret = false;
	u8 buf[61];
	u8 maxFrameCnt, frameCnt = 0;
    
    ts->bus_ops->read_blk(ts->bus_ops, G2_REG_TOUCH_BASE+1,60, buf); 
    
	maxFrameCnt = buf[1]& 0x1F; 		//frameCnt (0 ~ 12)
    frameCnt    = buf[7]& 0x1F;

	if(frameCnt > maxFrameCnt)  {
        ts->bus_ops->write(ts->bus_ops,G2_REG_AUX1, ts->auxreg1 | 0x01);
		return ret;
	}
  
    if(type == 0xF2)
    {
        MakeI2cDebugBuffer(frameBuf,buf,frameCnt);
        
        if(frameCnt == (maxFrameCnt-1)) 
        {
            u16 len, checkSum=0, i, bufCheckSum;
            len = (frameBuf[3]<<8) | (frameBuf[4]);

            for(i=0; i<(len+7); i++)  {
                checkSum += frameBuf[i];
            }
            bufCheckSum = (frameBuf[len+7] << 8) | (frameBuf[len+8]);
            
            if(bufCheckSum != checkSum) {
                g2debug("Packet Err: len=%d, checkSum = 0x%04x, bufCheckSum = 0x%02x%02x \r\n",len+7, checkSum, frameBuf[len+7],frameBuf[len+8]);
				ret = false;
            } else {
                PushDbgPacket(ts, frameBuf,len+7);
				ret = true;
            }
		}
    }
	
    ts->bus_ops->write(ts->bus_ops,G2_REG_AUX1, ts->auxreg1 | 0x01);

	return ret;
}


/*
	send prink to Cheese
*/
void dbgMsgToCheese(const char *fmt, ...)
{
	static u8 print_buf[128];
	u8 i=0,len=0;
	va_list va;
	u16 checkSum=0;
	
	print_buf[0] = 0x02;
	print_buf[1] = 0xA3;
		
	print_buf[2] = 0x50;
	print_buf[5] = 0;		// data
	print_buf[6] = 0;		// data
	print_buf[7] = 1;		// hex display
	
	va_start(va, fmt);
	len = vsprintf(&print_buf[8], fmt, va);
	va_end(va);

	print_buf[3] = (len+3)>>8;
	print_buf[4] = (len+3) & 0xFF;
	
	print_buf[len+8] = 0x03;
	print_buf[len+9] = 0xB3;

	for(i=0; i<len+10; i++)
		checkSum += print_buf[i];
	
	print_buf[len+10] = (checkSum>>8) & 0xff;
	print_buf[len+11] = checkSum & 0xff;


	PushDbgPacket(current_ts, print_buf,len+12);
	//	SendDataToI2C(0xF2, print_buf,len+10);
	
}


void TSPFrameCopytoBuf(u16 *dacBuf, u8* data)
{
	int i;
	
	for(i=0; i<390; i++) {
		dacBuf[i] = (data[(i*2)]<<8) | data[(i*2)+1];
	}	
}

void TSPFrameCopytoBuf_swap16(u16 *dacBuf, u8* data)
{
	int i;
	
	for(i=0; i<390; i++) {
		dacBuf[i] = (data[(i*2)+1]<<8) | data[(i*2)];
	}	
}



