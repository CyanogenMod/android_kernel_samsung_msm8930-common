
/* linux/driver/input/touchscreen/g2tsp.c
 *
 * Copyright (c) 2009-2012 G2Touch Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 *	# history # 
 *	- 2013-05-24(ver 1.2.0);
 *		: add DAC, ADC, AREA value read	
 *		: add watch-dog timer.
 *    - 2013-04-19
 *           : release event modify.(use timer)
 *    - 2013.04.16( Device_ver 1.0.5)
 *		: Add I2C_READ_FRAME(ioctl)
 *		: add 0xF3 tsp event --> for S/W request form tsp f/w
 *		: modify IOCTL Flash Write 
 *		: insert reset at the end of the firmware write function.
 *	- 2012.11.09, by jhkim
 *		: add Auto firmware download.
 *       
*/

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/byteorder/generic.h>
#include <linux/bitops.h>
#include <linux/earlysuspend.h>
#include <linux/g2tsp_platform.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/firmware.h>

#include <asm/system.h>
#include <asm/uaccess.h>

#include "g2tsp.h"

#include "g2tsp_misc.h"

#if (G2TSP_NEW_IC == 0)
#include "g2touch_i2c.h"
#endif


#ifdef CONFIG_LEDS_CLASS
#include <linux/regulator/consumer.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#endif


#define G2_DRIVER_VERSION	0x010200

#define DEF_TRACKING_ID	8	

#define G2TSP_FW_NAME		"g2tsp_fw.bin"
#define G2TSP_X_CH_NUM		15
#define G2TSP_Y_CH_NUM		20

#define FW_RECOVER_DELAY	msecs_to_jiffies(300)

#define TSP_DAC_FLAG		1
#define TSP_ADC_FLAG		2
#define TSP_AREA_FLAG		4
#define TSP_GET_FRAME_FLAG	0x80


struct g2tsp_trk {
	unsigned touch_id_num:4;
	unsigned sfk_id_num:4;
	
	unsigned fr_num:4;
	unsigned af1:1;
	unsigned rnd:1;
	unsigned sfkrnd:1;
	unsigned padding:1;

	unsigned y_h:3;
	unsigned x_h:3;
	unsigned area_h:2;

	u8 x_l;
	u8 y_l;
	u8 area_l;
}  __attribute((packed)); 

struct g2tsp_reg_data {
	unsigned tfnum:4;	
	unsigned sfknum:4; 		//special function key
	struct g2tsp_trk trk[10];	
} __attribute((packed));


struct g2tsp *current_ts;
const struct firmware *fw_info;
static u16 tsp_dac[390];
static u16 tsp_adc[390];
static u16 tsp_area[390];

static int watchdoc_enable = 1;

static unsigned char g2tsp_init_reg[5][2] = {
	//{0x00, 0x01},
	{0x00, 0x10},
	{0x30, 0x18},
	{0xce, 0x00},
	{0xc6, 0x00},
	{0x00, 0x00},
};

static unsigned char g2tsp_suspend_reg[1][2] = {
 {0x00, 0x01}
};

static unsigned char g2tsp_resume_reg[1][2] = {
	 {0x00, 0x00}
};
#define G2TSP_INIT_REGS (sizeof(g2tsp_init_reg) / sizeof(g2tsp_init_reg[0]))
#define G2TSP_SUSPEND_REGS (sizeof(g2tsp_suspend_reg) / sizeof(g2tsp_suspend_reg[0]))
#define G2TSP_RESUME_REGS (sizeof(g2tsp_resume_reg) / sizeof(g2tsp_resume_reg[0]))

void dbgMsgToCheese(const char *fmt, ...);
static void g2tsp_watchdog_timer_evnet(unsigned long data);
void g2tsp_watchdog_enable(struct g2tsp *ts, u32 msec);


#ifdef CONFIG_LEDS_CLASS
static void msm_tkey_led_vdd_on(bool onoff)
{
	int ret;
	static struct regulator *reg_l36;

	if (!reg_l36) {
		reg_l36 = regulator_get(NULL, "8917_l36");
		if (IS_ERR(reg_l36)) {
			pr_err("could not get 8917_l36, rc = %ld=n",
				PTR_ERR(reg_l36));
			return;
		}
		ret = regulator_set_voltage(reg_l36, 3300000, 3300000);
		if (ret) {
			pr_err("%s: unable to set ldo36 voltage to 3.3V\n",
				__func__);
			return;
		}
	}

	if (onoff) {
		if (!regulator_is_enabled(reg_l36)) {
			ret = regulator_enable(reg_l36);
			if (ret) {
				pr_err("enable l36 failed, rc=%d\n", ret);
				return;
			}
			pr_info("keyled 3.3V on is finished.\n");
		} else
			pr_info("keyled 3.3V is already on.\n");
	} else {
		if (regulator_is_enabled(reg_l36)) {
			ret = regulator_disable(reg_l36);
		if (ret) {
			pr_err("disable l36 failed, rc=%d\n", ret);
			return;
		}
		pr_info("keyled 3.3V off is finished.\n");
		} else
			pr_info("keyled 3.3V is already off.\n");
	}
}

static void msm_tkey_led_set(struct led_classdev *led_cdev,
	enum led_brightness value)
{
	bool tkey_led_on;

	if (value)
		tkey_led_on = true;
	else
		tkey_led_on = false;

	msm_tkey_led_vdd_on(tkey_led_on);
}
#endif
#if SEC_TSP_FACTORY_TEST

enum {
	BUILT_IN = 0,
	UMS,
	REQ_FW,
};



extern struct class *sec_class;
struct device *sec_touchscreen_dev;

#define TSP_CMD(name, func) .cmd_name = name, .cmd_func = func

struct tsp_cmd {
	struct list_head	list;
	const char	*cmd_name;
	void	(*cmd_func)(void *device_data);
};

static void fw_update(void *device_data);
static void get_fw_ver_bin(void *device_data);
static void get_fw_ver_ic(void *device_data);
static void get_config_ver(void *device_data);
static void get_x_num(void *device_data);
static void get_y_num(void *device_data);
static void get_chip_vendor(void *device_data);
static void get_chip_name(void *device_data);
static void get_threshold(void *device_data);
static void get_key_threshold(void *device_data);
static void get_reference(void *device_data);
static void get_normal(void *device_data);
static void get_delta(void *device_data);
static void run_reference_read(void *device_data);
static void run_normal_read(void *device_data);
static void run_delta_read(void *device_data);
static void not_support_cmd(void *device_data);

static void run_dac_read(void *device_data);
static void run_adc_read(void *device_data);
static void run_dac_read(void *device_data);

static void get_refDac(void *device_data);
static void get_dac(void *device_data);
static void get_adc(void *device_data);
static void get_area(void *device_data);
static void run_factory_cal(void *device_data);
static void get_diff_dac(void *device_data);
static void run_area_read(void *device_data);



static struct tsp_cmd tsp_cmds[] = {
	{TSP_CMD("fw_update", fw_update),},
	{TSP_CMD("get_fw_ver_bin", get_fw_ver_bin),},
	{TSP_CMD("get_fw_ver_ic", get_fw_ver_ic),},
	{TSP_CMD("get_config_ver", get_config_ver),},
	{TSP_CMD("get_x_num", get_x_num),},
	{TSP_CMD("get_y_num", get_y_num),},
	{TSP_CMD("get_chip_vendor", get_chip_vendor),},
	{TSP_CMD("get_chip_name", get_chip_name),},	
	{TSP_CMD("get_threshold", get_threshold),},
	{TSP_CMD("get_key_threshold", get_key_threshold),},
	{TSP_CMD("module_off_master", not_support_cmd),},
	{TSP_CMD("module_on_master", not_support_cmd),},
	{TSP_CMD("module_off_slave", not_support_cmd),},
	{TSP_CMD("module_on_slave", not_support_cmd),},	
	{TSP_CMD("run_reference_read", run_reference_read),},
	{TSP_CMD("run_normal_read", run_normal_read),},
	{TSP_CMD("run_delta_read", run_delta_read),},	
	{TSP_CMD("get_reference", get_reference),},
	{TSP_CMD("get_normal", get_normal),},
	{TSP_CMD("get_delta", get_delta),},	
	{TSP_CMD("not_support_cmd", not_support_cmd),},
	
	/* add G2TSP */
	{TSP_CMD("run_factory_cal", run_factory_cal),},
	{TSP_CMD("run_dac_read", run_dac_read),},
	{TSP_CMD("run_area_read", run_area_read),},	// size of touch.
	{TSP_CMD("run_adc_read", run_adc_read),},
	{TSP_CMD("get_dac", get_dac),},
	{TSP_CMD("get_area", get_area),},
	{TSP_CMD("get_adc", get_adc),},
	{TSP_CMD("get_adc", get_refDac),},
	{TSP_CMD("get_diff_dac", get_diff_dac),},
	
};

#endif

static void g2tsp_release_all(struct g2tsp *ts);
static void g2tsp_init_register(struct g2tsp *ts);

int g2tsp_FactoryCal(struct g2tsp *ts)
{
	int ret=-1;
	int time_out = 0;

	g2debug("%s() Enter + \r\n", __func__);
	dbgMsgToCheese("%s() Enter + \r\n", __func__);
	
	ts->workMode		= eMode_Factory_Cal;
	ts->factory_flag	= 0;
	ts->calfinish		= 0;

	ts->bus_ops->write(ts->bus_ops,0x00, 0x10);
	ts->bus_ops->write(ts->bus_ops,0xce, 0x05);
	ts->bus_ops->write(ts->bus_ops,0x30, 0x18); 
	ts->bus_ops->write(ts->bus_ops,0xc6, 0x00);
	udelay(10);
	ts->bus_ops->write(ts->bus_ops,0x00, 0x00);

	while(1)
	{
		msleep(10); //wait 10ms

		if(ts->factory_flag) {
			g2tsp_dac_write(ts, (u8*)ts->dacBuf);
			ret = 0;
			break;
		}
		
		if(time_out++ > 1000) { //wait 10sec
			g2debug("Factory Cal Time Out !! \r\n");
			dbgMsgToCheese("Factory Cal Time Out !! \r\n");
			break;
		}
	}

	ts->workMode		= eMode_Normal;
	ts->bus_ops->write(ts->bus_ops,0x00, 0x10);
	ts->bus_ops->write(ts->bus_ops,0xce, 0x00);
	ts->bus_ops->write(ts->bus_ops,0xc6, 0x00);
	ts->bus_ops->write(ts->bus_ops,0x30, 0x18);
	udelay(10);
	ts->bus_ops->write(ts->bus_ops,0x00, 0x00);

	return ret;

}


/*
		
*/
static int g2tsp_GetFrameData(struct g2tsp *ts, u8 type)
{
	int ret=-1;
	int time_out = 0;

	g2debug("%s(%d) Enter+ \r\n", __func__, type);
	dbgMsgToCheese("%s(%d) Enter+ \r\n", __func__, type);
	
	ts->workMode		= eMode_FactoryTest;
	ts->calfinish		= 0;
	ts->factory_flag	= type & 0x7f;
	
	ts->bus_ops->write(ts->bus_ops,0x00, 0x10);
	ts->bus_ops->write(ts->bus_ops,0xce, 0x05);
	ts->bus_ops->write(ts->bus_ops,0x30, 0x08);	
	udelay(10);
	ts->bus_ops->write(ts->bus_ops,0x00, 0x00);
	
	while(1)
	{
		msleep(10);	//wait 10ms
		if(ts->factory_flag & TSP_GET_FRAME_FLAG) 
		{
			int i;
			
			g2debug("Get Frame Data type(%d)!! \r\n",type);
			
			if(type == TSP_DAC_FLAG){
				memcpy(tsp_dac, ts->dacBuf, sizeof(tsp_dac));
			} else if (type == TSP_ADC_FLAG) {
				memcpy(tsp_adc, ts->dacBuf, sizeof(tsp_dac));
			} else if (type == TSP_AREA_FLAG) {
				memcpy(tsp_area, ts->dacBuf, sizeof(tsp_dac));
			}
			
			for(i=0; i<390; i++) {	
				printk("[%04d]", ts->dacBuf[i]);
				if((i%15)== 14 )	printk("\r\n");
			}

			ret = 0;
			break;
		}

		if(time_out++ > 500) { //wait 5sec
			g2debug("Get Frame Data Time Out type(%d)!! \r\n", type);
			dbgMsgToCheese("Get Frame Data Time Out type(%d)!! \r\n", type);
			break;
		}
		
	}

	ts->workMode		= eMode_Normal;
	ts->bus_ops->write(ts->bus_ops,0x00, 0x10);
	ts->bus_ops->write(ts->bus_ops,0xce, 0x00);
	ts->bus_ops->write(ts->bus_ops,0xc6, 0x00);
	ts->bus_ops->write(ts->bus_ops,0x30, 0x08);
	udelay(10);
	ts->bus_ops->write(ts->bus_ops,0x00, 0x00);

	return ret;	
}


static void check_firmware_version(struct g2tsp *ts, char *buf)
{
    g2debug("Firmware Version !! \r\n");
    
    ts->current_firmware_version[0] = (buf[1]<<16) |(buf[2]<<8) |  buf[3];
    ts->current_firmware_version[1] = (buf[4]<<16) |(buf[5]<<8) |  buf[6];
    
    g2debug("fw_ver[0] = %06x, fw_ver[1] = %06x \r\n",  ts->current_firmware_version[0],ts->current_firmware_version[1]);
	dbgMsgToCheese("fw_ver[0] = %06x, fw_ver[1] = %06x \r\n",  ts->current_firmware_version[0],ts->current_firmware_version[1]);
    if((ts->current_firmware_version[0] == 0) && (ts->current_firmware_version[1] == 0))
    {
    	// recheck at fw_recovery_work
    	g2debug("error firmware ver(0.0.0), at fw_recovery_work()!! \r\n");
        return;
    }

	if((ts->platform_data->fw_version[0] != ts->current_firmware_version[0]) 
		|| (ts->platform_data->fw_version[1] != ts->current_firmware_version[1]))
	{
		g2debug("version mismatched Re-Check at fw_recovery_work()!! \r\n");
		g2debug("Curent device version = 0x%06x.%06x, latest version = 0x%06x.%06x \r\n",
					ts->current_firmware_version[0],ts->current_firmware_version[1], ts->platform_data->fw_version[0],ts->platform_data->fw_version[1]);

		if(ts->firmware_download == 1) {
			ts->current_firmware_version[0] = 0;
			ts->current_firmware_version[1] = 0;
		}
		
	}
	else
	{
		g2debug("Version is matched!! -> Ready To Run!! \r\n");
		cancel_delayed_work(&ts->fw_work);
	}

	if(ts->workMode == eMode_Normal) 
		g2tsp_watchdog_enable(ts, 10000);	
	
}


//#define SWAP16(x)	(((x>>8)& 0x00ff) | ((x<<8)& 0xff00))

static void g2tsp_EventModeProcess(struct g2tsp *ts, u8 event, u16 len, u8 *data)
{
	static u8 frame_cnt = 0;
	
	switch(ts->workMode)
	{
	case eMode_Normal:
		if(event == 0xf5) { 	
			g2debug("Cal Finished !! \r\n");
			g2tsp_watchdog_enable(ts, 3000);	
		}
		break;
	case eMode_FW_Update:
		break;
	case eMode_Factory_Cal:
		if(event == 0xf5) {
			ts->calfinish = 1;
			frame_cnt = 0;
			ts->bus_ops->write(ts->bus_ops,0x02, 0x31);
			ts->bus_ops->write(ts->bus_ops,0xc6, 0x04);	//	
		} else if((event == 0xf7) && (ts->calfinish == 1)) {
		
			if(++frame_cnt > 1) {
				TSPFrameCopytoBuf_swap16(ts->dacBuf, &data[5+42]);
				ts->calfinish = 0;
				frame_cnt = 0;
				ts->factory_flag = 1;			
			}
		}
		break;
	case eMode_FactoryTest:
		if(event == 0xf5) {
			ts->calfinish = 1;
			g2debug("Factory Test Cal Finished !! \r\n");
			if(ts->factory_flag &  TSP_DAC_FLAG) {
				ts->bus_ops->write(ts->bus_ops,0x02, 0x31);
				ts->bus_ops->write(ts->bus_ops,0xc6, 0x04);
			} else if (ts->factory_flag &  TSP_ADC_FLAG) {
				ts->bus_ops->write(ts->bus_ops,0x02, 0x31);
				ts->bus_ops->write(ts->bus_ops,0xc6, 0x08);
			} else if	(ts->factory_flag &  TSP_AREA_FLAG) {
				ts->bus_ops->write(ts->bus_ops,0x02, 0x21);
				ts->bus_ops->write(ts->bus_ops,0xc6, 0x08);
			}
			
		} else if (event == 0xf7) {
			if(ts->calfinish == 1) 
			{
				g2debug("frame_cnt = %d \r\n", frame_cnt);
				if(++frame_cnt > 1) {
					
					ts->calfinish = 0;
					frame_cnt = 0;
					TSPFrameCopytoBuf(ts->dacBuf, &data[5+42]);
					ts->factory_flag |= TSP_GET_FRAME_FLAG;
				}
			}		
		}
		break;
	}
}

void g2tsp_dbg_event(struct g2tsp *ts, u8 event, u16 len, u8 *data)
{

	switch(event)
	{
	case 0xc0:		// Jig ID
		memcpy(ts->jigId, &data[5], 0x0F);					
		g2debug1("JIG ID = 0x%02x%02x%02x%02x \r\n", ts->jigId[0],ts->jigId[1],ts->jigId[2],ts->jigId[3]);	
		break;
	default:
		g2tsp_EventModeProcess(ts, event, len, data);
		break;
	}

}


struct g2tsp_info {
	u8 vender;
	u16 model;
	
	u8 fw_ver[3];
	u8 ic_name;
	u8 ic_rev;
	u8 tsp_type[3];
	u8 ch_x;
	u8 ch_y;
	u16 inch;
	u16 manufacture;
}  __attribute((packed)); 


static void g2tsp_i2c_event_process(struct g2tsp *ts, u8 type)
{
    u8 buf[60];
	static u8 	frameBuf[1024];	
	
	struct g2tsp_info tspInfo;
    
    switch(type)
    {
    case 0xF1:
        //memset(ts->tsp_info,0,30);
    	if(ts->bus_ops->read_blk(ts->bus_ops, G2_REG_TOUCH_BASE,18, ts->tsp_info) >= 0) {      
			memcpy((u8*)&tspInfo, &ts->tsp_info[1],17);
	    	check_firmware_version(ts,ts->tsp_info);
				
			g2debug2("ver(%02x.%02x.%02x),IC Name(%d), IC Rev(%d), TSP Type(%02x:%02x:%02x),ChInfo(%02x:%02x), Inch(%04x), Manuf(%04x) \r\n"
				,tspInfo.fw_ver[0],tspInfo.fw_ver[1],tspInfo.fw_ver[2],tspInfo.ic_name, tspInfo.ic_rev, tspInfo.tsp_type[0],tspInfo.tsp_type[1],tspInfo.tsp_type[2], tspInfo.ch_x,tspInfo.ch_y,tspInfo.inch,tspInfo.manufacture);

			dbgMsgToCheese("G2Touch Device Version = %06x",G2_DRIVER_VERSION);
			
        }
		ts->bus_ops->write(ts->bus_ops,G2_REG_AUX1, ts->auxreg1 | 0x01);
        break;
        
    case 0xF2:
        if(ReadTSPDbgData(ts,type, frameBuf) == true) {
			g2tsp_dbg_event(ts, frameBuf[2], (frameBuf[3]<<8)|(frameBuf[4]), frameBuf);
        }
        break;
        
    case 0xF3: 	// request s/w reset from TSP F/W
        if(ts->bus_ops->read(ts->bus_ops, G2_REG_TOUCH_BASE+1,buf) >= 0) {      
	    	g2debug(" Request Reset CMD buf[0] = 0x%02x \r\n",buf[0]);
			ts->platform_data->reset();
			mdelay(10);
			g2tsp_init_register(ts);
        }       
        
		
        break;
    default:
        
        break;
    }
    
}


static void g2tsp_input_worker(struct g2tsp *ts)
{
	struct g2tsp_reg_data rd;
	u8	touchNum,touchID, touchUpDown;
	u8 	sfkNum,sfkID,sfkUpDn;
    u16 x, y, area;
	int ret;
	int i, keycnt;

    memset((u8 *)&rd, 0, 61);

	
	ret = ts->bus_ops->read(ts->bus_ops,G2_REG_TOUCH_BASE,(u8 *)&rd);
	//dbgMsgToCheese("%s() , addr_60 = %02x ",__func__,*((u8*)&rd));

	if(ret < 0) {
		dbgMsgToCheese("Device I2C FAIL !! ");
		goto end_worker;
	}

    if(rd.sfknum == 0xf)
    {
        g2tsp_i2c_event_process(ts, *((u8*)&rd));
        goto end_worker;
    }
   
    
	if((rd.tfnum != 0) || (rd.sfknum != 0))
	{

		keycnt = (rd.tfnum)? rd.tfnum : rd.sfknum ;
#if 0
		if((rd.tfnum != 0) && (rd.sfknum != 0))
			goto end_worker;
#endif		
		if(keycnt > DEF_TRACKING_ID)
			goto end_worker;
		
		if(keycnt)
		{
		    u8  *rdData;
		    u8 checkSum=0,tspCheckSum;
            
			ret = ts->bus_ops->read_blk(ts->bus_ops, G2_REG_TOUCH_BASE,(keycnt*6)+1, (u8 *)&rd); 

            if (ret < 0){
				dbgMsgToCheese("Device I2C FAIL !! ");
                goto end_worker;
            }

            rdData = (u8 *)&rd;
            
            for(i=0; i< ((keycnt*6) + 1); i++) {
                checkSum += *rdData++;
            }

			ts->bus_ops->read(ts->bus_ops,G2_REG_CHECKSUM, &tspCheckSum);

            if(checkSum != tspCheckSum) {
                g2debug("checkSum Err!! deviceCheckSum = %02x, tspCheckSum = %02x \r\n", checkSum, tspCheckSum);
				dbgMsgToCheese("checkSum Err!! deviceCheckSum = %02x, tspCheckSum = %02x \r\n", checkSum, tspCheckSum);
                goto end_worker;
            }
            
		}
	}
	else
	{
		goto end_worker;
	}

	
	if (rd.tfnum)
	{
		//touchCnt;
		touchNum = rd.tfnum;

		for (i=0;i< touchNum;i++)
		{
			touchID = rd.trk[i].touch_id_num;
			x = (rd.trk[i].x_h << 8) | rd.trk[i].x_l;
			y = (rd.trk[i].y_h << 8) | rd.trk[i].y_l;
			area = (rd.trk[i].area_h << 8) | rd.trk[i].area_l;
			touchUpDown = rd.trk[i].rnd;
			g2debug1("%s %d ID:%d\t x:%d\t y:%d\t area:%d\t updn:%d\n", __func__, __LINE__, touchID, x, y, area, touchUpDown);
			//g2debug("%s %d ID:%d\t x:%d\t y:%d\t area:%d\t updn:%d\n", __func__, __LINE__, touchID, x, y, area, touchUpDown);
			
			if (ts->x_rev == 1)	
				x = ts->platform_data->res_x - x;

			if (ts->y_rev == 1)
				y = ts->platform_data->res_y - y;

			if((x > ts->platform_data->res_x) || (y > ts->platform_data->res_y))
            {         
				goto end_worker;
            }

			if(touchID >= DEF_TRACKING_ID) 
				goto end_worker;
				
			input_mt_slot(ts->input, touchID );		// event touch ID
			
			if (touchUpDown)
			{
				if(ts->prev_Area[touchID] > 0)
					area = (ts->prev_Area[touchID]*19+ area)/20;

				if (area < 1) 
					area = 1;
				
				ts->prev_Area[touchID] = area;

				input_report_abs(ts->input, ABS_MT_TRACKING_ID, touchID);
                input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, area);   // press       
                input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, area);
                input_report_abs(ts->input, ABS_MT_POSITION_X, x);
                input_report_abs(ts->input, ABS_MT_POSITION_Y, y);
                input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, true);
			} else {
				ts->prev_Area[touchID] = 0;
				input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
                input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
			}	
			
				
		}

		mod_timer(&ts->timer, jiffies + msecs_to_jiffies(200));
		//printk("isr occur \r\n");
		g2tsp_watchdog_enable(ts, 1000);
		
	}

	// touch key
	if (rd.sfknum)
	{
		sfkNum = rd.sfknum;
		for (i=0;i<sfkNum;i++)
		{
			sfkID = rd.trk[i].sfk_id_num; 
			sfkUpDn= rd.trk[i].sfkrnd; 

			for(keycnt = 0; keycnt < ts->platform_data->nkeys; keycnt++)
			{
				struct g2tsp_keys_button *keys = &ts->platform_data->keys[keycnt];
				
				if (sfkID == keys->glue_id)	{
					input_report_key(ts->input, keys->code, sfkUpDn);	
				}
			}
		}
		g2tsp_watchdog_enable(ts, 1000);
	}
 
    input_sync(ts->input);

end_worker:
	return;
}



static void g2tsp_timer_evnet(unsigned long data)
{	
	struct g2tsp *ts = (struct g2tsp *)data;	
	int i;

	g2debug1("%s( Enter+ ) \r\n",__func__);

	// search didn't release event id
	for(i=0; i<DEF_TRACKING_ID; i++)
	{
		if(ts->prev_Area[i] != 0)
			break;
	}

	if(i != DEF_TRACKING_ID)
	{
		// release event id
		for(i=0; i<DEF_TRACKING_ID; i++)
		{
			if(ts->prev_Area[i] == 0) 
				continue;
			
			g2debug2("event not released ID(%d) \r\n",i);
			dbgMsgToCheese("### event not released ID(%d) ### ",i);
			ts->prev_Area[i] = 0;
			input_mt_slot(ts->input, i);		
			input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
			input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
		}

		input_sync(ts->input);
	}
	
}


void g2tsp_watchdog_enable(struct g2tsp *ts, u32 msec)
{
	//g2debug(" Run WatchDog Timer !! \r\n");
	del_timer(&ts->watchdog_timer);
	setup_timer(&ts->watchdog_timer, g2tsp_watchdog_timer_evnet, (unsigned long)ts);
	mod_timer(&ts->watchdog_timer, jiffies + msecs_to_jiffies(msec));	
}


static void g2tsp_watchdog_timer_evnet(unsigned long data)
{	
	struct g2tsp *ts = (struct g2tsp *)data;	
	
	g2debug2("%s( Enter+ )\r\n",__func__);

	queue_work(ts->watchdog_work_queue, &ts->watchdog_work);
}




static void g2tsp_watchdog_work(struct work_struct *work)
{
	struct g2tsp 	*ts = container_of(work, struct g2tsp, watchdog_work);
	u8 reg_c5;
	static u8 errCnt=0,last_c5 = 0x7f;

	g2debug1(" %s(Enter +) \r\n", __func__);
	disable_irq_nosync(ts->irq);
	
	ts->bus_ops->read(ts->bus_ops, 0xC5, &reg_c5);	

	if((reg_c5 == last_c5) && (watchdoc_enable == 1))
	{
		errCnt++;

		if(errCnt >= 2)
		{
			g2debug("TSP sync err !!, reg_c5 = %02x\r\n", reg_c5);
			dbgMsgToCheese("TSP sync err !!, reg_c5 = %02x\r\n", reg_c5);

			errCnt = 0;
			
			ts->platform_data->reset();
			mdelay(10);
			ts->bus_ops->write(ts->bus_ops, 0x00, 0x10);	
			ts->bus_ops->write(ts->bus_ops, 0xC5, 0x80);	
			ts->bus_ops->write(ts->bus_ops, 0x30, 0x18);	
			udelay(10);
			ts->bus_ops->write(ts->bus_ops, 0x00, 0x00);	
			g2tsp_watchdog_enable(ts,5000);
	
		}
		else 
			mod_timer(&ts->watchdog_timer, jiffies + msecs_to_jiffies(100));	
	}
	else 
	{
		errCnt = 0;
		last_c5 = reg_c5;

		mod_timer(&ts->watchdog_timer, jiffies + msecs_to_jiffies(1000));	
	}
	
	
	
	enable_irq(ts->irq);
}


static void g2tsp_interrupt_work(struct work_struct *work)
{
    struct g2tsp 	*ts = container_of(work, struct g2tsp, irq_work);
    
    g2tsp_input_worker(ts);
    enable_irq(ts->irq);
}

static void g2tsp_fw_recovery_work(struct work_struct *work)
{	
	
    struct g2tsp 	*ts = container_of(work, struct g2tsp, fw_work.work);
    g2debug("%s() Enter + %06x, %06x  \r\n", __func__,ts->current_firmware_version[0],ts->current_firmware_version[1]);
    
    if((ts->current_firmware_version[0] == 0) && (ts->current_firmware_version[1] == 0))
    {
        g2debug("TSP Reset !! \r\n");
        ts->bus_ops->write(ts->bus_ops, 0x0, 0x10);
        udelay(5);
        ts->bus_ops->write(ts->bus_ops, 0x0, 0x00);
        mdelay(150);
    }
    
	if (ts->firmware_download)
	{
		g2debug("[G2TSP] : Emergency Firmware Update !! \r\n");

		request_firmware_nowait(THIS_MODULE,
		                      FW_ACTION_HOTPLUG,
	                    	  G2TSP_FW_NAME,
	                	      ts->pdev,
	            	          GFP_KERNEL,
	        	              ts,
	    	                  firmware_request_handler);
		
		ts->firmware_download = 0;
		ts->workMode = eMode_FW_Update;
	}
	else
	{
		g2debug("%s()-> Run WatchDog Timer !! \r\n",__func__);
		g2tsp_watchdog_enable(ts, 5000);
	}
	cancel_delayed_work(&ts->fw_work);
}



static irqreturn_t g2tsp_irq_func(int irq, void *handle)
{
//	int gpio;
    struct g2tsp *ts = (struct g2tsp *)handle;
	disable_irq_nosync(irq);

    if(ts->platform_data->irq_mode == IRQ_MODE_THREAD)
    {
        g2tsp_input_worker(ts);
        enable_irq(irq);
    }
    else
    {
        queue_work(ts->work_queue, &ts->irq_work);
    }

    return IRQ_HANDLED;
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void g2tsp_suspend_register(struct g2tsp *ts)
{
	int ret;
    int i;
	u8 data;

    printk("[G2TSP] : %s(Enter) \r\n", __func__);
	g2debug("%s\n", __func__);

    g2tsp_release_all(ts);
    
    for (i=0;i<G2TSP_SUSPEND_REGS;i++)
    {
		ret = ts->bus_ops->write(ts->bus_ops, g2tsp_suspend_reg[i][0], g2tsp_suspend_reg[i][1]);
        g2debug("%s : write [0x%02x]  0x%02x\n", __func__, 
				g2tsp_suspend_reg[i][0], 
				g2tsp_suspend_reg[i][1]);
    }

	mdelay(10);
	ts->bus_ops->read(ts->bus_ops, 0x00,&data);
	g2debug("suspend reg_00 = %02x \r\n", data);
	
	if(data != g2tsp_suspend_reg[0][1]) {
		g2debug("didn't enter suspend -> re-send suspend cmd !! \r\n");
		ts->bus_ops->write(ts->bus_ops, g2tsp_suspend_reg[0][0], g2tsp_suspend_reg[0][1]);
	}	
}


static void g2tsp_resume_register(struct g2tsp *ts)
{
	int ret;
    int i;
	u8 data;

	g2debug("%s\n", __func__);
    g2debug(" %s(Enter) \r\n", __func__);
		
    for (i=0;i<G2TSP_RESUME_REGS;i++)
    {
		ret = ts->bus_ops->write(ts->bus_ops, g2tsp_resume_reg[i][0], g2tsp_resume_reg[i][1]);
        g2debug("%s : write [0x%02x]  0x%02x\n", __func__, 
				g2tsp_resume_reg[i][0], 
				g2tsp_resume_reg[i][1]);	
		udelay(100);
    }

	mdelay(100);
	ts->bus_ops->read(ts->bus_ops, 0x00,&data);
	if(data != 0) {
		g2debug("didn't wakeup -> re init resume !! \r\n");
		ts->bus_ops->write(ts->bus_ops, g2tsp_resume_reg[0][0], g2tsp_resume_reg[0][1]);
	}
	
}



static void g2tsp_early_suspend(struct early_suspend *h)
{
    struct g2tsp *ts = container_of(h, struct g2tsp, early_suspend);

	g2debug("%s\n", __func__);
    printk("[G2TSP] : %s(Enter) \r\n", __func__);
    mutex_lock(&ts->mutex);

    disable_irq_nosync(ts->irq);
	ts->platform_data->suspend();
	g2tsp_suspend_register(ts);
    ts->suspend = 1;
    cancel_work_sync(&ts->irq_work);
	del_timer(&ts->watchdog_timer);

    mutex_unlock(&ts->mutex);
}


static void g2tsp_late_resume(struct early_suspend *h)
{
    struct g2tsp *ts = container_of(h, struct g2tsp, early_suspend);

	g2debug("%s\n", __func__);
    printk("[G2TSP] : %s(Enter) \r\n", __func__);
    
    mutex_lock(&ts->mutex);

	ts->platform_data->wakeup();
	g2tsp_resume_register(ts);
    ts->suspend = 0;

	g2tsp_watchdog_enable(ts, 2000);
    enable_irq(ts->irq);


    mutex_unlock(&ts->mutex);
}
#endif


static void g2tsp_init_register(struct g2tsp *ts)
{
	int ret;
	int i;

	g2debug("%s\n", __func__);
	
    ts->bus_ops->write(ts->bus_ops, g2tsp_init_reg[0][0], g2tsp_init_reg[0][1]);
    udelay(10);
	
    for (i=1;i<G2TSP_INIT_REGS;i++)
    {
		ret = ts->bus_ops->write(ts->bus_ops, g2tsp_init_reg[i][0], g2tsp_init_reg[i][1]);
		g2debug("%s : write [0x%02x]  0x%02x\n", __func__, 
				g2tsp_init_reg[i][0], 
				g2tsp_init_reg[i][1]);
    }
    msleep(1);
	
	
}


static void g2tsp_release_all(struct g2tsp *ts)
{
    int i;

    for(i=0; i<DEF_TRACKING_ID; i++)
    {
        input_mt_slot(ts->input, i+1);		// event touch ID   
    	input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
        input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);        
    }

    input_sync(ts->input);
}

/*****************************
	G2TSP IOCTL sysfs
******************************/
#define I2C_READ_WORD       0x9010
#define I2C_WRITE_WORD      0x9020
#define I2C_READ_FRAME      0x9040
#define I2C_APK_CMD			0x9060
#define I2C_FLASH_ERASE     0x9080
#define I2C_CHIP_INIT       0x9089

struct reg_data{
    unsigned int addr;
    unsigned int data;
};

struct reg_data_packet {
    unsigned int len;
    unsigned char buf[1024];
};

static u8 ioctl_buf[1024]; 
static struct reg_data_packet packet_data;


long g2tsp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)

{
    struct reg_data ioctl_data;

    
	u8 addr,val;
    u32 len;

    switch (cmd)
    {
    case I2C_READ_FRAME:
        if (copy_from_user(&packet_data, (void *)arg, sizeof(packet_data)))
            return -EFAULT;
		
        len = PopDbgPacket(current_ts, ioctl_buf);
        packet_data.len = len;
        memcpy(packet_data.buf, ioctl_buf, len);

        if (copy_to_user((struct packet_data *)arg, &packet_data, sizeof(packet_data)))
            return -EFAULT;

        break;

	case I2C_APK_CMD:
		g2tsp_watchdog_enable(current_ts,12000);
        if (copy_from_user(&ioctl_data, (void *)arg, sizeof(ioctl_data)))
            return -EFAULT;

		switch(ioctl_data.addr)
		{
		case 1:	//S/W Reset
			current_ts->bus_ops->write(current_ts->bus_ops, 0x00, 0x10);
			udelay(10);
			current_ts->bus_ops->write(current_ts->bus_ops, 0x00, 0x00);
			break;
		case 2: //H/W Reset
			current_ts->platform_data->reset();
			mdelay(10);
			g2tsp_init_register(current_ts);
			break;
		case 3:	//factory cal
			g2tsp_FactoryCal(current_ts);
			break;
		case 4: 
		case 5: 
		case 6: 	
			{
				u8 type = 0;
				
		        if (copy_from_user(&packet_data, (void *)arg, sizeof(packet_data)))
		            return -EFAULT;

				type = (1<<(ioctl_data.addr-4));

				if(g2tsp_GetFrameData(current_ts, type) < 0)
					return -EFAULT;

				memcpy(packet_data.buf, current_ts->dacBuf, sizeof(current_ts->dacBuf));
		        if (copy_to_user((struct packet_data *)arg, &packet_data, sizeof(packet_data)))
		            return -EFAULT;
			}
			break;
		case 0x10:
			watchdoc_enable  = 1;
			dbgMsgToCheese("Watch Dog Enable !!");
			break;
		case 0x11:
			watchdoc_enable  = 0;
			dbgMsgToCheese("Watch Dog Disable !!");
			break;

		}
		
		g2debug1("I2C_APK_CMD (cmd:%02x) \r\n", ioctl_data.addr);

        break;


		
    case I2C_READ_WORD:
        if (copy_from_user(&ioctl_data, (void *)arg, sizeof(ioctl_data)))
            return -EFAULT;

		addr = (u8)ioctl_data.addr;
		current_ts->bus_ops->read(current_ts->bus_ops, addr, &val);
        ioctl_data.data = (unsigned int)val;

        if (copy_to_user((struct reg_data *)arg, &ioctl_data, sizeof(ioctl_data)))
            return -EFAULT;

    	g2debug1("I2C_READ_WORD done.\n");
        break;
        
    case I2C_WRITE_WORD:
		g2tsp_watchdog_enable(current_ts,10000);
        if (copy_from_user(&ioctl_data, (void *)arg, sizeof(ioctl_data)))
            return -EFAULT;
	
		addr = (u8)ioctl_data.addr;
		val =  (u8)ioctl_data.data;	
		current_ts->bus_ops->write(current_ts->bus_ops, addr, val);
    	g2debug1("I2C_WRITE_WORD done.\n");
        break;
		
    case I2C_FLASH_ERASE:
		g2tsp_watchdog_enable(current_ts,10000);
        g2tsp_flash_eraseall(current_ts, 0x80);
    	g2debug("I2C_FLASH_ERASE done.\n");
        break;
		
    case I2C_CHIP_INIT:
		// remark 2012.11.05
		current_ts->workMode = eMode_Normal;
		current_ts->bus_ops->write(current_ts->bus_ops, 0x01, 0x00);	//Flash Enable	
//		current_ts->bus_ops->write(current_ts->bus_ops, 0xC0, 0x9f);
		current_ts->platform_data->reset();
		
		msleep(50);
		g2tsp_init_register(current_ts);
		
		g2debug("I2C_CHIP_INIT done.\n");

        break;
		
    default:
    	g2debug("unknown ioctl: %x\n", cmd);
        return -ENOIOCTLCMD;
		
    }
    return 0;
}


static const struct file_operations g2tsp_fileops = {
    .owner   = THIS_MODULE,
    .unlocked_ioctl   = g2tsp_ioctl,
};


static struct miscdevice g2tsp_misc_device = {
    .minor      = MISC_DYNAMIC_MINOR,
    .name       = "s3c-g2touch",
    .fops       = &g2tsp_fileops,
};


#if SEC_TSP_FACTORY_TEST

#define CHIP_INFO		"G2"
#define G2_DEVICE_NAME "GT-I9128"
#define FW_RELEASE_DATE "0514"

static void set_cmd_result(struct g2tsp *info, char *buff, int len)
{
	strncat(info->cmd_result, buff, len);
}

static void set_default_result(struct g2tsp *info)
{
	char delim = ':';
	memset(info->cmd_result, 0x00, ARRAY_SIZE(info->cmd_result));
	memcpy(info->cmd_result, info->cmd, strlen(info->cmd));
	strncat(info->cmd_result, &delim, 1);
}

static void not_support_cmd(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	
	set_default_result(info);
	sprintf(buff, "%s", "NA");
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 4;
	g2debug( "%s: \"%s(%d)\"\n", __func__,
				buff, strnlen(buff, sizeof(buff)));
	return;

}

static void get_fw_ver_bin(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};

	set_default_result(info);

//	snprintf(buff, sizeof(buff), "%s%06x%06x",  CHIP_INFO,info->platform_data->fw_version[0], info->platform_data->fw_version[1]);
	snprintf(buff, sizeof(buff), "%s%06x",  CHIP_INFO, info->platform_data->fw_version[1]);

	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	
}

static void get_fw_ver_ic(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};	

	set_default_result(info);
	
//	snprintf(buff, sizeof(buff), "%s%06x%06x", CHIP_INFO,  info->current_firmware_version[0], info->current_firmware_version[1]);
	snprintf(buff, sizeof(buff), "%s%06x", CHIP_INFO,   info->current_firmware_version[1]);

	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

}

static void get_config_ver(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[20] = {0};

	set_default_result(info);
	snprintf(buff, sizeof(buff), "%s_%s_%s",
	    G2_DEVICE_NAME, CHIP_INFO, FW_RELEASE_DATE);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

	return;
}

static void get_x_num(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	
	set_default_result(info);

	snprintf(buff, sizeof(buff), "%u", G2TSP_X_CH_NUM);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_y_num(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	
	set_default_result(info);

	snprintf(buff, sizeof(buff), "%u", G2TSP_Y_CH_NUM);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

}


static void get_chip_vendor(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	
	set_default_result(info);

	snprintf(buff, sizeof(buff), "%s", CHIP_INFO);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}
	

static void get_chip_name(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	
	set_default_result(info);

	snprintf(buff, sizeof(buff), "%s", CHIP_INFO);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_threshold(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
//	int ret;
//	u16 threshold;	
	char buff[16] = {0};
	
	set_default_result(info);

/*	
	ret = ts_read_data(info->client, ZINITIX_SENSITIVITY, (u8*)&threshold, 2);

	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "fail");
		set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
		info->cmd_state = 3;
		return;
	}
*/	
	snprintf(buff, sizeof(buff), "%d", 80);

	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

static void get_key_threshold(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
//	int ret;
//	u16 threshold;	
	char buff[16] = {0};
	
	set_default_result(info);
/*
	ret = ts_read_data(current_ts->client, ZINITIX_BUTTON_SENSITIVITY, (u8*)&threshold, 2);

	if (ret < 0) {
		snprintf(buff, sizeof(buff), "%s", "fail");
		set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
		info->cmd_state = 3;
		return;
	}
*/	
	snprintf(buff, sizeof(buff), "%u", 20);

	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));
}

/*
static inline void wait_raw_data(struct g2tsp *info, int need_skip_cnt)
{
	int skip;

	for(skip=0; skip < need_skip_cnt; ) {
		if (info->update == 1) {
			info->update = 0;
			skip++;
		}		
	}		
}
*/



static void run_reference_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);

	if(g2tsp_FactoryCal(info)< 0)
		return;

	info->cmd_state = 2;

	
}



static void run_dac_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);

	if(g2tsp_GetFrameData(info,TSP_DAC_FLAG)<0)
		return;

	info->cmd_state = 2;
}

static void run_area_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);

	if(g2tsp_GetFrameData(info,TSP_AREA_FLAG)<0)
		return;


	info->cmd_state = 2;
}


static void run_adc_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);
	
	if(g2tsp_GetFrameData(info,TSP_ADC_FLAG) < 0)
		return;


	info->cmd_state = 2;

}



static void get_diff_dac(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	int val;
	int x,y;
	
	set_default_result(info);
	x = info->cmd_param[0];
	y = info->cmd_param[1];

	val = Refence_dac[(y*15)+x] - tsp_dac[(y*15)+x];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));

	info->cmd_state = 2;

	g2debug("%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	

}



static void get_refDac(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	unsigned int val;
	int x,y;
	
	set_default_result(info);
	x = info->cmd_param[0];
	y = info->cmd_param[1];

	val = Refence_dac[(y*15)+x];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));

	
	info->cmd_state = 2;

	g2debug("%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	

}


static void get_dac(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	unsigned int val;
	int x,y;
	
	set_default_result(info);
	x = info->cmd_param[0];
	y = info->cmd_param[1];

	val = tsp_dac[(y*15)+x];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));

	
	info->cmd_state = 2;

	g2debug("%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	


}


static void get_area(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	unsigned int val;
	int x,y;
	
	set_default_result(info);
	x = info->cmd_param[0];
	y = info->cmd_param[1];

	val = tsp_area[(y*15)+x];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));

	
	info->cmd_state = 2;

	g2debug("%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	

}


static void get_adc(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	unsigned int val;
	int x,y;
	
	set_default_result(info);
	x = info->cmd_param[0];
	y = info->cmd_param[1];

	val = tsp_adc[(y*15)+x];
	snprintf(buff, sizeof(buff), "%u", val);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));

	
	info->cmd_state = 2;

	g2debug("%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));	

}


static void run_factory_cal(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);

	g2tsp_FactoryCal(info);
	
	info->cmd_state = 2;
}

static void run_normal_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);
/*
	
	ts_set_touchmode(TOUCH_NORMAL_MODE);
	wait_raw_data(info, 5);
	down(&info->raw_data_lock);
	memcpy((u8 *)info->normal_data, (u8 *)info->cur_data, info->cap_info.total_node_num*2);
	up(&info->raw_data_lock);	
	ts_set_touchmode(TOUCH_POINT_MODE);		
*/

	info->cmd_state = 2;
}

static void run_delta_read(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;

	set_default_result(info);
/*
		
	ts_set_touchmode(TOUCH_REF_MODE);
	wait_raw_data(info, 5);
	down(&info->raw_data_lock);
	memcpy((u8 *)info->delta_data, (u8 *)info->cur_data, info->cap_info.total_node_num*2);
	up(&info->raw_data_lock);	
	ts_set_touchmode(TOUCH_POINT_MODE);		
*/

	info->cmd_state = 2;
}

static void get_reference(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
/*	
	unsigned int val;
	int x_node, y_node;
	int node_num;
*/
	set_default_result(info);
/*
	x_node = info->cmd_param[0];
	y_node = info->cmd_param[1];
	
	if (x_node < 0 || x_node > info->cap_info.x_node_num ||
		y_node < 0 || y_node > info->cap_info.x_node_num) {		
		snprintf(buff, sizeof(buff), "%s", "abnormal");
		set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
		info->cmd_state = 3;
		return;

	}
	node_num = x_node*info->cap_info.x_node_num + y_node;
	
	val = info->ref_data[node_num];
	snprintf(buff, sizeof(buff), "%u", val);
	*/
	snprintf(buff, sizeof(buff), "%u", 1);
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;

	g2debug( "%s: %s(%d)\n", __func__,
			buff, strnlen(buff, sizeof(buff)));

}
static void get_normal(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
	/*	

	unsigned int val;
	int x_node, y_node;
	int node_num;
*/	
	set_default_result(info);
/*
	
	x_node = info->cmd_param[0];
	y_node = info->cmd_param[1];
	if (x_node < 0 || x_node > info->cap_info.x_node_num ||
		y_node < 0 || y_node > info->cap_info.x_node_num) { 	
		snprintf(buff, sizeof(buff), "%s", "abnormal");
		set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
		info->cmd_state = 3;
		return;
	}

	node_num = x_node*info->cap_info.x_node_num + y_node;
		
	val = info->normal_data[node_num];
	snprintf(buff, sizeof(buff), "%u", val);
	*/
	snprintf(buff, sizeof(buff), "%u", 1);

	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	
	g2debug( "%s: %s(%d)\n", __func__,
				buff, strnlen(buff, sizeof(buff)));

}
static void get_delta(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char buff[16] = {0};
/*
	unsigned int val;
	int x_node, y_node;
	int node_num;
*/	
	set_default_result(info);
/*	
	x_node = info->cmd_param[0];
	y_node = info->cmd_param[1];
	
	if (x_node < 0 || x_node > info->cap_info.x_node_num ||
		y_node < 0 || y_node > info->cap_info.x_node_num) { 	
		snprintf(buff, sizeof(buff), "%s", "abnormal");
		set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
		info->cmd_state = 3;
		return;
	}

	node_num = x_node*info->cap_info.x_node_num + y_node;
		
	val = info->delta_data[node_num];
	snprintf(buff, sizeof(buff), "%u", val);
	*/
	snprintf(buff, sizeof(buff), "%u", 0);
	
	set_cmd_result(info, buff, strnlen(buff, sizeof(buff)));
	info->cmd_state = 2;
	
	g2debug( "%s: %s(%d)\n", __func__,
				buff, strnlen(buff, sizeof(buff)));

}


static void fw_update(void *device_data)
{
	struct g2tsp *info = (struct g2tsp *)device_data;
	char result[16] = {0};

	switch (info->cmd_param[0]) {
	case BUILT_IN:
		info->firmware_download=1;
		info->factory_cal = 1;
		schedule_delayed_work(&info->fw_work, FW_RECOVER_DELAY);
	break;

	case UMS:
		break;
		}
	info->cmd_state = 2;
	snprintf(result, sizeof(result) , "%s", "OK");
	set_cmd_result(info, result,
				strnlen(result, sizeof(result)));
	return;
}


static ssize_t store_cmd(struct device *dev, struct device_attribute
				  *devattr, const char *buf, size_t count)
{
	struct g2tsp *info = current_ts;

	char *cur, *start, *end;
	char buff[TSP_CMD_STR_LEN] = {0};
	int len, i;
	struct tsp_cmd *tsp_cmd_ptr = NULL;
	char delim = ',';
	bool cmd_found = false;
	int param_cnt = 0;
	int ret;

	if (info->cmd_is_running == true) {
		g2debug( "tsp_cmd: other cmd is running.\n");
		goto err_out;
	}


	/* check lock  */
	mutex_lock(&info->cmd_lock);
	info->cmd_is_running = true;
	mutex_unlock(&info->cmd_lock);

	info->cmd_state = 1;

	for (i = 0; i < ARRAY_SIZE(info->cmd_param); i++)
		info->cmd_param[i] = 0;

	len = (int)count;
	if (*(buf + len - 1) == '\n')
		len--;
	memset(info->cmd, 0x00, ARRAY_SIZE(info->cmd));
	memcpy(info->cmd, buf, len);

	cur = strchr(buf, (int)delim);
	if (cur)
		memcpy(buff, buf, cur - buf);
	else
		memcpy(buff, buf, len);

	/* find command */
	list_for_each_entry(tsp_cmd_ptr, &info->cmd_list_head, list) {
		if (!strcmp(buff, tsp_cmd_ptr->cmd_name)) {
			cmd_found = true;
			break;
		}
	}

	/* set not_support_cmd */
	if (!cmd_found) {
		list_for_each_entry(tsp_cmd_ptr, &info->cmd_list_head, list) {
			if (!strcmp("not_support_cmd", tsp_cmd_ptr->cmd_name))
				break;
		}
	}

	/* parsing parameters */
	if (cur && cmd_found) {
		cur++;
		start = cur;
		memset(buff, 0x00, ARRAY_SIZE(buff));
		do {
			if (*cur == delim || cur - buf == len) {
				end = cur;
				memcpy(buff, start, end - start);
				*(buff + strlen(buff)) = '\0';
				ret = kstrtoint(buff, 10,\
						info->cmd_param + param_cnt);
				start = cur + 1;
				memset(buff, 0x00, ARRAY_SIZE(buff));
				param_cnt++;
			}
			cur++;
		} while (cur - buf <= len);
	}

	g2debug( "cmd = %s\n", tsp_cmd_ptr->cmd_name);
	for (i = 0; i < param_cnt; i++)
		g2debug("cmd param %d= %d\n", i,
							info->cmd_param[i]);

	tsp_cmd_ptr->cmd_func(info);


err_out:
	return count;
}

static ssize_t show_cmd_status(struct device *dev,
		struct device_attribute *devattr, char *buf)
{
	struct g2tsp *info = current_ts;
	//struct g2tsp *info = dev_get_drvdata(dev);
	char buff[16] = {0};

	g2debug( "tsp cmd: status:%d\n",
			info->cmd_state);

	if (info->cmd_state == 0)
		snprintf(buff, sizeof(buff), "WAITING");

	else if (info->cmd_state == 1)
		snprintf(buff, sizeof(buff), "RUNNING");

	else if (info->cmd_state == 2)
		snprintf(buff, sizeof(buff), "OK");

	else if (info->cmd_state == 3)
		snprintf(buff, sizeof(buff), "FAIL");

	else if (info->cmd_state == 4)
		snprintf(buff, sizeof(buff), "NOT_APPLICABLE");

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", buff);
}

static ssize_t show_cmd_result(struct device *dev, struct device_attribute
				    *devattr, char *buf)
{
	struct g2tsp *info = current_ts;
	//struct g2tsp *info = dev_get_drvdata(dev);

	g2debug("tsp cmd: result: %s\n", info->cmd_result);

	mutex_lock(&info->cmd_lock);
	info->cmd_is_running = false;
	mutex_unlock(&info->cmd_lock);

	info->cmd_state = 0;

	return snprintf(buf, TSP_BUF_SIZE, "%s\n", info->cmd_result);
}


static DEVICE_ATTR(cmd, S_IWUSR | S_IWGRP, NULL, store_cmd);
static DEVICE_ATTR(cmd_status, S_IRUGO, show_cmd_status, NULL);
static DEVICE_ATTR(cmd_result, S_IRUGO, show_cmd_result, NULL);

static struct attribute *sec_touch_attributes[] = {
	&dev_attr_cmd.attr,
	&dev_attr_cmd_status.attr,
	&dev_attr_cmd_result.attr,
	NULL,
};

static struct attribute_group sec_touch_attributes_group = {
	.attrs = sec_touch_attributes,
};
#endif


void *g2tsp_init(struct g2tsp_bus_ops *bus_ops, struct device *pdev)
{
    struct input_dev *input_device;
    struct g2tsp *ts;
    int retval = 0;
	int keycnt,i;

	g2debug("%s (Ver : %02d.%02d.%02d) \n", __func__,						
											(G2_DRIVER_VERSION>>16) & 0xFF, 
											(G2_DRIVER_VERSION>>8) & 0xFF,	
											(G2_DRIVER_VERSION) & 0xFF);

    ts = kzalloc(sizeof(*ts), GFP_KERNEL);
    if (ts == NULL) {
        printk(KERN_ERR "%s: Error, kzalloc\n", __func__);
        goto error_alloc_data_failed;
    }

    mutex_init(&ts->mutex);
    ts->pdev = pdev;
    ts->platform_data = pdev->platform_data;
    ts->bus_ops = bus_ops;

    if (ts->platform_data->power)
        retval = ts->platform_data->power(1);
    if (retval) {
        printk(KERN_ERR "%s: platform power control failed! \n", __func__);
        goto error_init;
    }

    /* Create the input device and register it. */
    input_device = input_allocate_device();
    if (!input_device) {
        retval = -ENOMEM;
        printk(KERN_ERR "%s: Error, failed to allocate input device\n",
            __func__);
        goto error_input_allocate_device;
    }

    ts->input = input_device;
    input_device->name = ts->platform_data->name;
    input_device->dev.parent = ts->pdev;

    input_device->phys    = "g2touch-ts/input0";

    input_device->id.bustype  = BUS_HOST;
    input_device->id.vendor   = 0x16B4;
    input_device->id.product  = 0x0702;
    input_device->id.version  = 0x0001;

    /* enable interrupts */
    ts->irq = gpio_to_irq(ts->platform_data->irq_gpio);
	


    if (retval < 0) {
        printk(KERN_ERR "%s: IRQ request failed r=%d\n",
            __func__, retval);
        goto error_set_irq;
    }

    set_bit(EV_SYN, input_device->evbit);
    set_bit(EV_KEY, input_device->evbit);
    set_bit(EV_ABS, input_device->evbit);


#ifdef CONFIG_LEDS_CLASS    
	set_bit(EV_LED, input_device->evbit);    
	set_bit(LED_MISC, input_device->ledbit);
#endif
    set_bit(EV_ABS, input_device->evbit);
    set_bit(INPUT_PROP_DIRECT, input_device->propbit);

	for(keycnt = 0; keycnt < ts->platform_data->nkeys; keycnt++)
	{
		struct g2tsp_keys_button *keys = &ts->platform_data->keys[keycnt];
        set_bit(keys->code & KEY_MAX, input_device->keybit);
	}

	memset(ts->prev_Area, 0, sizeof(ts->prev_Area));

    input_set_abs_params(input_device, ABS_MT_POSITION_X, 0, ts->platform_data->res_x, 0, 0);
    input_set_abs_params(input_device, ABS_MT_POSITION_Y, 0, ts->platform_data->res_y, 0, 0);
    input_set_abs_params(input_device, ABS_MT_TOUCH_MAJOR, 1, 255, 0, 0);
    input_set_abs_params(input_device, ABS_MT_WIDTH_MAJOR, 1, 255, 0, 0);
    input_set_abs_params(input_device, ABS_MT_TRACKING_ID, 0, DEF_TRACKING_ID, 0, 0);
    input_mt_init_slots(input_device, DEF_TRACKING_ID);

    //init dbgPacket
    ts->dbgPacket.head = 0;
    ts->dbgPacket.tail = 0;
	memset(ts->jigId, 0, sizeof(ts->jigId));
	
	// options check
	ts->x_rev 				= 0;
	ts->y_rev 				= 0;
 	ts->firmware_download 	= 0;
	ts->calfinish 			= 0;
	ts->factory_cal 		= 1;
	ts->workMode			= eMode_Normal;
	ts->factory_flag		= 0;
	
    g2debug(" Init flag = 0x%08x \r\n", ts->platform_data->options);
	if (ts->platform_data->options & G2_XREV)
		ts->x_rev = 1;
	
	if (ts->platform_data->options & G2_YREV)
		ts->y_rev = 1;

	if (ts->platform_data->options & G2_FWDOWN)
		ts->firmware_download = 1;

    retval = input_register_device(input_device);
    if (retval) {
        printk(KERN_ERR "%s: Error, failed to register input device\n", __func__);
        goto error_input_register_device;
    }

#ifdef CONFIG_HAS_EARLYSUSPEND
    ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ts->early_suspend.suspend = g2tsp_early_suspend;
    ts->early_suspend.resume = g2tsp_late_resume;
    register_early_suspend(&ts->early_suspend);
#endif

    dev_set_drvdata(pdev, ts);

    ts->bus_ops->read(ts->bus_ops, G2_REG_AUX1, &ts->auxreg1);

    // initialize version info.
	ts->current_firmware_version[0] = 0;
    ts->current_firmware_version[1] = 0;
    
	current_ts = ts;

    INIT_WORK(&ts->irq_work, g2tsp_interrupt_work);
    ts->work_queue = create_singlethread_workqueue("isr_work_queue");

    INIT_WORK(&ts->watchdog_work, g2tsp_watchdog_work);
    ts->watchdog_work_queue = create_singlethread_workqueue("watchdog_work_queue");


	INIT_DELAYED_WORK_DEFERRABLE(&ts->fw_work, g2tsp_fw_recovery_work);
	schedule_delayed_work(&ts->fw_work, FW_RECOVER_DELAY);

//	INIT_DELAYED_WORK_DEFERRABLE(&ts->watchdog_work, g2tsp_watchdog_work);

	setup_timer(&ts->timer, g2tsp_timer_evnet, (unsigned long)ts);
	setup_timer(&ts->watchdog_timer, g2tsp_watchdog_timer_evnet, (unsigned long)ts);
	

    if(ts->platform_data->irq_mode == IRQ_MODE_THREAD)
    {
        retval = request_threaded_irq(ts->irq, NULL, g2tsp_irq_func, ts->platform_data->irq_flag,  ts->input->name, ts);
    }
    else
    {
        retval = request_irq(ts->irq,  g2tsp_irq_func, ts->platform_data->irq_flag, ts->input->name, ts);
    }

	g2tsp_init_register(ts);
    
	retval = misc_register(&g2tsp_misc_device);
    if (retval) {
        printk(KERN_ERR "%s: s3c-g2touch misc err\n", __func__);
		goto error_misc_register_device;
    }

#if SEC_TSP_FACTORY_TEST	
	INIT_LIST_HEAD(&ts->cmd_list_head);
	for (i = 0; i < ARRAY_SIZE(tsp_cmds); i++)
		list_add_tail(&tsp_cmds[i].list, &ts->cmd_list_head);
	
	mutex_init(&ts->cmd_lock);
	ts->cmd_is_running = false;

	//sys/class/misc/touch_misc_fops/....
	sec_touchscreen_dev = device_create(sec_class, NULL, 0, NULL, "tsp");
	//sec_touchscreen_dev = device_create(sec_class, NULL, 0, NULL, "sec_touchscreen");

	if (IS_ERR(sec_touchscreen_dev))
		g2debug( "Failed to create device for the sysfs\n");

	
	retval = sysfs_create_group(&sec_touchscreen_dev->kobj, &sec_touch_attributes_group);
	if (retval)
		g2debug( "Failed to create sysfs .\n");	

#endif

#ifdef CONFIG_LEDS_CLASS
	ts->leds.name = TOUCHKEY_BACKLIGHT;
	ts->leds.brightness = LED_FULL;
	ts->leds.max_brightness = LED_FULL;
	ts->leds.brightness_set = msm_tkey_led_set;

	retval = led_classdev_register(pdev, &ts->leds);
	if (retval) {
		printk(KERN_ERR "%s: Failed to register led\n", __func__);
		goto fail_led_reg;
	}
#endif
	g2debug("G2TSP : %s() Exit !! \r\n", __func__);
    return ts;

#ifdef CONFIG_LEDS_CLASS
fail_led_reg:
	led_classdev_unregister(&ts->leds);
#endif

error_misc_register_device:
	
error_input_register_device:
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&ts->early_suspend);
#endif

    input_unregister_device(input_device);

    if (ts->irq >= 0)
        free_irq(ts->irq, ts);

error_set_irq:
  input_free_device(input_device);
error_input_allocate_device:
    if (ts->platform_data->power)
        ts->platform_data->power(0);

	del_timer_sync(&ts->timer);
	del_timer_sync(&ts->watchdog_timer);
error_init:
    kfree(ts);
error_alloc_data_failed:
    return NULL;
}


void g2tsp_release(void *handle)
{
    struct g2tsp *ts = handle;

	g2debug("%s\n", __func__);
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&ts->early_suspend);
#endif

    cancel_work_sync(&ts->irq_work);
    free_irq(ts->irq, ts);
    input_unregister_device(ts->input);
    input_free_device(ts->input);

    if (ts->platform_data->power)
        ts->platform_data->power(0);

    kfree(ts);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("G2touch touchscreen device driver");
MODULE_AUTHOR("G2TOUCH");

