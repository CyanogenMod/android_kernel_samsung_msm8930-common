#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/workqueue.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

//#include <plat/gpio-cfg.h>

#include <mach/gpio.h>

extern struct class *sec_class;
struct device *slot_switch_dev;

struct slot_switch_wq {
	struct delayed_work work_q;
//	struct fsa9480_info *sdata;
	struct list_head entry;
};

//#define GPIO_SIM_SEL 52
static ssize_t get_slot_switch(struct device *dev, struct device_attribute *attr, char *buf)
{
	int value;

	//return '0' slot path is '||', return '1' slot path is 'X'
	value = gpio_get_value(GPIO_SIM_SEL);
	printk("Current Slot is %x\n", value);

	return sprintf(buf, "%d\n", value);
}

// 1 : BCOM
// 0 : SPRD
static ssize_t set_slot_switch(struct device *dev, struct device_attribute *attr,   const char *buf, size_t size)
{
	int value;
	int fd;
	char buffer[2]={0};

	sscanf(buf, "%d", &value);

	mm_segment_t fs = get_fs();
	set_fs(get_ds());
	
	if ((fd = sys_open("/efs/slot_witch.bin", O_CREAT|O_WRONLY, 0)) < 0)
	{
		printk("%s :: open failed %s ,fd=0x%x\n", __func__, "/efs/slot_witch.bin", fd);
	} else {
		printk("%s :: open success %s ,fd=0x%x\n", __func__, "/efs/slot_witch.bin", fd);
	}

	printk("%s : value = %s \n", __func__,value);

	switch(value) {
		case 0:
			gpio_set_value(GPIO_SIM_SEL, 0);
			sprintf(buffer, "%s", "0");
			printk("set slot switch to %x\n", gpio_get_value(GPIO_SIM_SEL));
			break;
		case 1:
			gpio_set_value(GPIO_SIM_SEL, 1);
			sprintf(buffer, "1");			
			printk("set slot switch to %x\n", gpio_get_value(GPIO_SIM_SEL));
			break;
		default:
			printk("Enter 0 or 1!!\n");
	}

	sys_write(fd, buffer, strlen(buffer));

	sys_close(fd); 
	set_fs(fs);

	return size;
}

static DEVICE_ATTR(slot_sel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, get_slot_switch, set_slot_switch);

static void slot_switch_init_work(struct work_struct *work)
{
	struct delayed_work *dw = container_of(work, struct delayed_work, work);
	struct slot_switch_wq *wq = container_of(dw, struct slot_switch_wq, work_q);
//	struct fsa9480_info *usbsw = wq->sdata;
	int fd;
	int ret;
	char buffer[2]={0};

	printk("[slot switch]: %s :: \n",__func__);	

	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	if ((fd = sys_open("/efs/slot_switch.bin", O_CREAT|O_RDWR  ,0664)) < 0)
	{ 
		schedule_delayed_work(&wq->work_q, msecs_to_jiffies(2000));
		printk("[slot switch]: %s :: open failed %s ,fd=0x%x\n",__func__,"/efs/slot_switch.bin",fd);

		if(fd < 0){
			sys_close(fd);
			set_fs(fs);
	
		}
	} else {
		cancel_delayed_work(&wq->work_q);
		printk("[slot switch]: %s :: open success %s ,fd=0x%x\n",__func__,"/efs/slot_switch.bin",fd);
	}	
		
	ret = sys_read(fd, buffer, 1);
	if(ret < 0) {
		printk("slot_switch READ FAIL!\n");
		sys_close(fd);
		set_fs(fs);		
		return 0;
	}

	printk("slot switch buffer : %s\n", buffer);

	if (!strcmp(buffer, "0"))//SPRD
	{
		gpio_set_value(GPIO_SIM_SEL, 0);
		printk("set slot switch to %x\n", gpio_get_value(GPIO_SIM_SEL));
	} else 	if(!strcmp(buffer, "1")){//BCOM
		gpio_set_value(GPIO_SIM_SEL, 1);
		printk("set slot switch to %x\n", gpio_get_value(GPIO_SIM_SEL));
	}

	sys_close(fd);
	set_fs(fs);

	return;
}

static int __init slot_switch_manager_init(void)
{
	int ret = 0;
	int err = 0;
	int gpio = 0;
	struct slot_switch_wq *wq;

	printk("slot_switch_manager_init\n");

    //initailize uim_sim_switch gpio
   	printk("%s start \n",__func__);
	gpio = GPIO_SIM_SEL;
	err = gpio_request(gpio, "SIM_SEL");
	if (err) {	
		pr_err("fail to request gpio %s, gpio %d, errno %d\n",
					"PDA_ACTIVE", GPIO_SIM_SEL, err);
	} else {
		gpio_direction_output(gpio, 0);
		gpio_export(gpio, 0);
		printk("%s end \n",__func__);
	}

	//initailize slot switch device
	slot_switch_dev = device_create(sec_class,
                                    NULL, 0, NULL, "slot_switch");
	if (IS_ERR(slot_switch_dev))
		pr_err("Failed to create device(switch)!\n");

	if (device_create_file(slot_switch_dev, &dev_attr_slot_sel) < 0)
		pr_err("Failed to create device file(%s)!\n",
					dev_attr_slot_sel.attr.name);

	wq = kmalloc(sizeof(struct slot_switch_wq), GFP_ATOMIC);
	if (wq) {
//		wq->sdata = usbsw;
		INIT_DELAYED_WORK(&wq->work_q, slot_switch_init_work);
		schedule_delayed_work(&wq->work_q, msecs_to_jiffies(100));
	} else
		return -ENOMEM;

	return ret;
}

static void __exit slot_switch_manager_exit(void)
{
}

module_init(slot_switch_manager_init);
module_exit(slot_switch_manager_exit);

MODULE_AUTHOR("SAMSUNG ELECTRONICS CO., LTD");
MODULE_DESCRIPTION("Slot Switch");
MODULE_LICENSE("GPL");
