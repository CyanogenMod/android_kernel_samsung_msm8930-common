#ifndef __G2TSP_H__
#define __G2TSP_H__

#include <linux/kernel.h>
#ifdef CONFIG_LEDS_CLASS
#include <linux/leds.h>
#define TOUCHKEY_BACKLIGHT	"button-backlight"
#endif

#define G2TSP_I2C_NAME  "g2tsp_i2c_adapter"

struct g2tsp_bus_ops {
    s32 (*write_blk)(void *handle, u8 addr, u8 length, const void *values);
    s32 (*read_blk)(void *handle, u8 addr, u8 length, void *values);
    s32 (*write)(void *handle, u8 addr, u8 value);
    s32 (*read)(void *handle, u8 addr, u8 *value);
};

void *g2tsp_init(struct g2tsp_bus_ops *bus_ops, struct device *pdev);
void g2tsp_release(void *handle);





#define SEC_TSP_FACTORY_TEST	1	//for samsung
#define	SEC_DND_N_COUNT		10
#define	SEC_DND_FREQUENCY	110		//300khz

#define MAX_RAW_DATA_SZ		576	/*32x18*/
#define MAX_SUPPORTED_FINGER_NUM 5
#define MAX_TRAW_DATA_SZ	\
	(MAX_RAW_DATA_SZ + 4*MAX_SUPPORTED_FINGER_NUM + 2)
#define TSP_CMD_STR_LEN 32
#define TSP_CMD_RESULT_STR_LEN 512
#define TSP_CMD_PARAM_NUM 8
#define TSP_BUF_SIZE 1024
	
/* preriod raw data interval*/

#define G2TSP_NEW_IC	1


#define G2_DEBUG_PRINT
//#define G2_DEBUG_PRINT1
#define G2_DEBUG_PRINT2

#ifdef G2_DEBUG_PRINT
	#define g2debug(fmt, ...)                   \
    	do {                                    \
			printk("[G2TSP]");     				\
            printk(fmt, ##__VA_ARGS__);     	\
    	} while(0) 
#else
	#define g2debug(fmt, ...) do { } while(0)
#endif

#ifdef G2_DEBUG_PRINT1
	#define g2debug1(fmt, ...)                  \
    	do {                                    \
            printk("[G2TSP1]");                 \
            printk(fmt, ##__VA_ARGS__);         \
    	} while(0) 
#else
	#define g2debug1(fmt, ...) do { } while(0)
#endif

#ifdef G2_DEBUG_PRINT2
	#define g2debug2(fmt, ...)                  \
    	do {                                    \
            printk("[G2TSP2]");                 \
            printk(fmt, ##__VA_ARGS__);         \
    	} while(0) 
#else
	#define g2debug2(fmt, ...) do { } while(0)
#endif


#define DBG_PACKET_SIZE 8 * 1024   //10KByte

struct tDbgPacket
{
	u16	head;
	u16	tail;
	u8	buf[DBG_PACKET_SIZE];
} __attribute((packed));


struct g2tsp {
	struct device *pdev;
    int irq;

	// options
	int x_rev;
	int y_rev;
    u8 auxreg1;
	int firmware_download;
	u8 jigId[16];
	int	current_firmware_version[2];

    struct input_dev *input;
    struct workqueue_struct	*work_queue;
    struct work_struct irq_work;
    struct delayed_work fw_work;
	struct timer_list	timer;
    struct mutex mutex;
    struct early_suspend early_suspend;
    struct g2tsp_platform_data *platform_data;
    struct g2tsp_bus_ops *bus_ops;

    struct tDbgPacket dbgPacket;
	u8 suspend;
	int prev_Area[10];
#ifdef CONFIG_LEDS_CLASS
	struct led_classdev leds;
	bool tkey_led_reserved;
#endif

#if SEC_TSP_FACTORY_TEST
	s16 ref_data[MAX_RAW_DATA_SZ];
	s16 normal_data[MAX_RAW_DATA_SZ];
	s16 delta_data[MAX_RAW_DATA_SZ];	

	struct list_head	cmd_list_head;
	u8	cmd_state;
	char	cmd[TSP_CMD_STR_LEN];
	int		cmd_param[TSP_CMD_PARAM_NUM];
	char	cmd_result[TSP_CMD_RESULT_STR_LEN];
	struct mutex	cmd_lock;
	bool	cmd_is_running;
#endif		

	u8 tsp_info[30];
};


#endif 
