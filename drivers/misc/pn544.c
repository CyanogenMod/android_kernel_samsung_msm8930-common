/*
 * Copyright (C) 2010 Trusted Logic S.A.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>
#include <linux/pn544.h>
#include <linux/wakelock.h>

#ifdef CONFIG_NFC_PN547
#include <mach/msm_xo.h>
#include <linux/workqueue.h>
#endif

#define MAX_BUFFER_SIZE		512

#define NFC_DEBUG 0
#define MAX_TRY_I2C_READ	10
#define I2C_ADDR_READ_L		0x51
#define I2C_ADDR_READ_H		0x57

struct pn544_dev {
	wait_queue_head_t read_wq;
	struct mutex read_mutex;
	struct i2c_client *client;
	struct miscdevice pn544_device;
	void (*conf_gpio) (void);
	unsigned int ven_gpio;
	unsigned int firm_gpio;
	unsigned int irq_gpio;

	atomic_t irq_enabled;
	atomic_t read_flag;
	bool cancel_read;
	struct wake_lock nfc_wake_lock;

#ifdef CONFIG_NFC_PN547
	unsigned int clk_req_gpio;
	unsigned int clk_req_irq;
	struct msm_xo_voter *nfc_clock;
	struct work_struct work_nfc_clock;
	struct workqueue_struct *wq_clock;
	bool clock_state;
#endif
};

static irqreturn_t pn544_dev_irq_handler(int irq, void *dev_id)
{
	struct pn544_dev *pn544_dev = dev_id;

	if (!gpio_get_value(pn544_dev->irq_gpio)) {
#if NFC_DEBUG
		pr_err("%s, irq_gpio = %d\n", __func__,
			gpio_get_value(pn544_dev->irq_gpio));
#endif
		return IRQ_HANDLED;
	}

	/* Wake up waiting readers */
	atomic_set(&pn544_dev->read_flag, 1);
	wake_up(&pn544_dev->read_wq);


#if NFC_DEBUG
	pr_info("pn544 : call\n");
#endif
	wake_lock_timeout(&pn544_dev->nfc_wake_lock, 2*HZ);
	return IRQ_HANDLED;
}

#ifdef CONFIG_NFC_PN547
static void nfc_work_func_clock(struct work_struct *work)
{
	struct pn544_dev *pn544_dev = container_of(work, struct pn544_dev,
					      work_nfc_clock);
	int ret = 0;

	if (gpio_get_value(pn544_dev->clk_req_gpio)) {
		if (pn544_dev->clock_state == false) {
			ret = msm_xo_mode_vote(pn544_dev->nfc_clock,
						MSM_XO_MODE_ON);
			if (ret < 0) {
				pr_err("%s:  Failed to vote for TCX0_A1 ON (%d)\n",
						__func__, ret);
			}
			pn544_dev->clock_state = true;
		}
	} else {
		if (pn544_dev->clock_state == true) {
			ret = msm_xo_mode_vote(pn544_dev->nfc_clock,
						MSM_XO_MODE_OFF);
			if (ret < 0) {
				pr_err("%s:  Failed to vote for TCX0_A1 OFF (%d)\n",
						__func__, ret);
			}
			pn544_dev->clock_state = false;
		}
	}
}

static irqreturn_t pn544_dev_clk_req_irq_handler(int irq, void *dev_id)
{
	struct pn544_dev *pn544_dev = dev_id;
	queue_work(pn544_dev->wq_clock, &pn544_dev->work_nfc_clock);
	return IRQ_HANDLED;
}
#endif

static ssize_t pn544_dev_read(struct file *filp, char __user *buf,
			      size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev = filp->private_data;
	char tmp[MAX_BUFFER_SIZE] = {0, };
	int ret = 0;
#ifdef CONFIG_NFC_PN544
	int readingWatchdog = 0;
#endif
	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	pr_debug("%s : reading %zu bytes. irq=%s\n", __func__, count,
		 gpio_get_value(pn544_dev->irq_gpio) ? "1" : "0");

#if NFC_DEBUG
	pr_info("pn544 : + r\n");
#endif

	mutex_lock(&pn544_dev->read_mutex);

#ifdef CONFIG_NFC_PN544
wait_irq:
#endif

	if (!gpio_get_value(pn544_dev->irq_gpio)) {
		atomic_set(&pn544_dev->read_flag, 0);
		if (filp->f_flags & O_NONBLOCK) {
			pr_info("%s : O_NONBLOCK\n", __func__);
			ret = -EAGAIN;
			goto fail;
		}

#if NFC_DEBUG
		pr_info("pn544: wait_event_interruptible : in\n");
#endif

		ret = wait_event_interruptible(pn544_dev->read_wq,
			atomic_read(&pn544_dev->read_flag));

#if NFC_DEBUG
		pr_info("pn544 :   h\n");
#endif

		if (pn544_dev->cancel_read) {
			pn544_dev->cancel_read = false;
			ret = -1;
			goto fail;
		}

		if (ret)
			goto fail;

	}

	/* Read data */
	ret = i2c_master_recv(pn544_dev->client, tmp, count);

#ifdef CONFIG_NFC_PN544
	/* If bad frame(from 0x51 to 0x57) is received from pn65n,
	* we need to read again after waiting that IRQ is down.
	* if data is not ready, pn65n will send from 0x51 to 0x57. */
	if ((I2C_ADDR_READ_L <= tmp[0] && tmp[0] <= I2C_ADDR_READ_H)
		&& readingWatchdog < MAX_TRY_I2C_READ) {
		pr_warn("%s: data is not ready yet.data = 0x%x, cnt=%d\n",
			__func__, tmp[0], readingWatchdog);
		usleep_range(2000, 2000); /* sleep 2ms to wait for IRQ */
		readingWatchdog++;
		goto wait_irq;
	}
#endif

#if NFC_DEBUG
	pr_info("pn544: i2c_master_recv\n");
#endif
	mutex_unlock(&pn544_dev->read_mutex);
	if (ret < 0) {
		pr_err("%s: i2c_master_recv returned %d\n", __func__,
				ret);
		return ret;
	}

	if (ret > count) {
		pr_err("%s: received too many bytes from i2c (%d)\n",
				__func__, ret);
		return -EIO;
	}

	if (copy_to_user(buf, tmp, ret)) {
		pr_err("%s : failed to copy to user space\n", __func__);
		return -EFAULT;
	}
	return ret;

fail:
	mutex_unlock(&pn544_dev->read_mutex);
	return ret;
}

static ssize_t pn544_dev_write(struct file *filp, const char __user *buf,
			       size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev;
	char tmp[MAX_BUFFER_SIZE] = {0, };
	int ret = 0, retry = 2;

	pn544_dev = filp->private_data;

#if NFC_DEBUG
	pr_info("pn544 : + w\n");
#endif

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	if (copy_from_user(tmp, buf, count)) {
		pr_err("%s : failed to copy from user space\n", __func__);
		return -EFAULT;
	}

	pr_debug("%s : writing %zu bytes.\n", __func__, count);
	/* Write data */
	do {
		retry--;
	ret = i2c_master_send(pn544_dev->client, tmp, count);
		if (ret == count)
			break;
		usleep_range(6000, 10000); /* Retry, chip was in standby */
#if NFC_DEBUG
		pr_debug("%s : retry = %d\n", __func__, retry);
#endif
	} while (retry);

#if NFC_DEBUG
	pr_info("pn544 : - w\n");
#endif

	if (ret != count) {
		pr_err("%s : i2c_master_send returned %d\n", __func__, ret);
		ret = -EIO;
	}

	return ret;
}

static int pn544_dev_open(struct inode *inode, struct file *filp)
{
	struct pn544_dev *pn544_dev = container_of(filp->private_data,
						   struct pn544_dev,
						   pn544_device);
	filp->private_data = pn544_dev;

	pr_debug("%s : %d,%d\n", __func__, imajor(inode), iminor(inode));

	return 0;
}

static long pn544_dev_ioctl(struct file *filp,
			   unsigned int cmd, unsigned long arg)
{
	struct pn544_dev *pn544_dev = filp->private_data;

	switch (cmd) {
	case PN544_SET_PWR:
		if (arg == 2) {
			/* power on with firmware download (requires hw reset)
			 */
			gpio_set_value_cansleep(pn544_dev->ven_gpio, 1);
			gpio_set_value(pn544_dev->firm_gpio, 1);
			usleep_range(10000, 10050);
			gpio_set_value_cansleep(pn544_dev->ven_gpio, 0);
			usleep_range(10000, 10050);
			gpio_set_value_cansleep(pn544_dev->ven_gpio, 1);
			usleep_range(10000, 10050);
			if (atomic_read(&pn544_dev->irq_enabled) == 0) {
				atomic_set(&pn544_dev->irq_enabled, 1);
				enable_irq(pn544_dev->client->irq);
				enable_irq_wake(pn544_dev->client->irq);
			}
			pr_info("%s power on with firmware, irq=%d\n", __func__,
				atomic_read(&pn544_dev->irq_enabled));
		} else if (arg == 1) {
			/* power on */
			if (pn544_dev->conf_gpio)
				pn544_dev->conf_gpio();
			gpio_set_value(pn544_dev->firm_gpio, 0);
			gpio_set_value_cansleep(pn544_dev->ven_gpio, 1);
			usleep_range(10000, 10050);
			if (atomic_read(&pn544_dev->irq_enabled) == 0) {
				atomic_set(&pn544_dev->irq_enabled, 1);
				enable_irq(pn544_dev->client->irq);
				enable_irq_wake(pn544_dev->client->irq);
			}
			pr_info("%s power on, irq=%d\n", __func__,
				atomic_read(&pn544_dev->irq_enabled));
		} else if (arg == 0) {
			/* power off */
			if (atomic_read(&pn544_dev->irq_enabled) == 1) {
				atomic_set(&pn544_dev->irq_enabled, 0);
				disable_irq_wake(pn544_dev->client->irq);
				disable_irq_nosync(pn544_dev->client->irq);
			}
			pr_info("%s power off, irq=%d\n", __func__,
				atomic_read(&pn544_dev->irq_enabled));
			gpio_set_value(pn544_dev->firm_gpio, 0);
			gpio_set_value_cansleep(pn544_dev->ven_gpio, 0);
			usleep_range(10000, 10050);
		} else if (arg == 3) {
			pr_info("%s Read Cancel\n", __func__);
			pn544_dev->cancel_read = true;
			atomic_set(&pn544_dev->read_flag, 1);
			wake_up(&pn544_dev->read_wq);
		} else {
			pr_err("%s bad arg %lu\n", __func__, arg);
			return -EINVAL;
		}
		break;
	default:
		pr_err("%s bad ioctl %u\n", __func__, cmd);
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations pn544_dev_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.read = pn544_dev_read,
	.write = pn544_dev_write,
	.open = pn544_dev_open,
	.unlocked_ioctl = pn544_dev_ioctl,
};

static int pn544_probe(struct i2c_client *client,
		       const struct i2c_device_id *id)
{
	int ret;
	struct pn544_i2c_platform_data *platform_data;
	struct pn544_dev *pn544_dev;

	if (client->dev.platform_data == NULL) {
		pr_err("%s : nfc probe fail\n", __func__);
		return -ENODEV;
	}
	platform_data = client->dev.platform_data;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s : need I2C_FUNC_I2C\n", __func__);
		return -ENODEV;
	}

	ret = gpio_request(platform_data->irq_gpio, "nfc_int");
	if (ret)
		return -ENODEV;
	ret = gpio_request(platform_data->ven_gpio, "nfc_ven");
	if (ret)
		goto err_ven;
	ret = gpio_request(platform_data->firm_gpio, "nfc_firm");
	if (ret)
		goto err_firm;
#ifdef CONFIG_NFC_PN547
	ret = gpio_request(platform_data->clk_req_gpio, "nfc_clk_req");
	if (ret)
		goto err_clk_req;
#endif
	pn544_dev = kzalloc(sizeof(*pn544_dev), GFP_KERNEL);
	if (pn544_dev == NULL) {
		dev_err(&client->dev,
			"failed to allocate memory for module data\n");
		ret = -ENOMEM;
		goto err_exit;
	}
#ifdef CONFIG_NFC_PN547
	pn544_dev->nfc_clock = msm_xo_get(MSM_XO_TCXO_A1, "nfc");
	if (IS_ERR(pn544_dev->nfc_clock)) {
		ret = PTR_ERR(pn544_dev->nfc_clock);
		printk(KERN_ERR "%s: Couldn't get TCXO_A1 vote for NFC (%d)\n",
					__func__, ret);
		ret = -ENODEV;
		goto err_get_clock;
	}
	pn544_dev->clock_state = false;
#endif
	pr_info("%s : IRQ num %d\n", __func__, client->irq);

	pn544_dev->irq_gpio = platform_data->irq_gpio;
	pn544_dev->ven_gpio = platform_data->ven_gpio;
	pn544_dev->firm_gpio = platform_data->firm_gpio;
	pn544_dev->conf_gpio = platform_data->conf_gpio;
#ifdef CONFIG_NFC_PN547
	pn544_dev->clk_req_gpio = platform_data->clk_req_gpio;
	pn544_dev->clk_req_irq = platform_data->clk_req_irq;
#endif
	pn544_dev->client = client;

	/* init mutex and queues */
	init_waitqueue_head(&pn544_dev->read_wq);
	mutex_init(&pn544_dev->read_mutex);

	pn544_dev->pn544_device.minor = MISC_DYNAMIC_MINOR;
	pn544_dev->pn544_device.name = "pn544";
	pn544_dev->pn544_device.fops = &pn544_dev_fops;

	ret = misc_register(&pn544_dev->pn544_device);
	if (ret) {
		pr_err("%s : misc_register failed\n", __FILE__);
		goto err_misc_register;
	}

	/* request irq.  the irq is set whenever the chip has data available
	 * for reading.  it is cleared when all data has been read.
	 */
	pr_info("%s : requesting IRQ %d\n", __func__, client->irq);
	gpio_direction_input(pn544_dev->irq_gpio);
	gpio_direction_output(pn544_dev->ven_gpio, 0);
	gpio_direction_output(pn544_dev->firm_gpio, 0);
#ifdef CONFIG_NFC_PN547
	gpio_direction_input(pn544_dev->clk_req_gpio);
#endif

	i2c_set_clientdata(client, pn544_dev);
	wake_lock_init(&pn544_dev->nfc_wake_lock,
			WAKE_LOCK_SUSPEND, "nfc_wake_lock");
#ifdef CONFIG_NFC_PN547
	pn544_dev->wq_clock = create_singlethread_workqueue("nfc_wq");
	if (!pn544_dev->wq_clock) {
		ret = -ENOMEM;
		pr_err("%s: could not create workqueue\n", __func__);
		goto err_create_workqueue;
	}
	INIT_WORK(&pn544_dev->work_nfc_clock, nfc_work_func_clock);
#endif
	ret = request_irq(client->irq, pn544_dev_irq_handler,
			  IRQF_TRIGGER_RISING, "pn544", pn544_dev);
	if (ret) {
		dev_err(&client->dev, "request_irq failed\n");
		goto err_request_irq_failed;
	}
	disable_irq_nosync(pn544_dev->client->irq);
	atomic_set(&pn544_dev->irq_enabled, 0);

#ifdef CONFIG_NFC_PN547
	ret = request_irq(pn544_dev->clk_req_irq, pn544_dev_clk_req_irq_handler,
		IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING
			, "pn544_clk_req", pn544_dev);
	if (ret) {
		dev_err(&client->dev, "request_irq(clk_req) failed\n");
		goto err_request_irq_failed;
	}

	enable_irq_wake(pn544_dev->clk_req_irq);
#endif
	return 0;

err_request_irq_failed:
#ifdef CONFIG_NFC_PN547
err_create_workqueue:
#endif
	misc_deregister(&pn544_dev->pn544_device);
	wake_lock_destroy(&pn544_dev->nfc_wake_lock);
err_misc_register:
	mutex_destroy(&pn544_dev->read_mutex);
#ifdef CONFIG_NFC_PN547
	msm_xo_put(pn544_dev->nfc_clock);
err_get_clock:
#endif
	kfree(pn544_dev);
err_exit:
#ifdef CONFIG_NFC_PN547
	gpio_free(platform_data->clk_req_gpio);
err_clk_req:
#endif
	gpio_free(platform_data->firm_gpio);
err_firm:
	gpio_free(platform_data->ven_gpio);
err_ven:
	gpio_free(platform_data->irq_gpio);
	pr_err("[PN544] pn544_probe fail!\n");
	return ret;
}

static int pn544_remove(struct i2c_client *client)
{
	struct pn544_dev *pn544_dev;

	pn544_dev = i2c_get_clientdata(client);
	wake_lock_destroy(&pn544_dev->nfc_wake_lock);
	free_irq(client->irq, pn544_dev);
	misc_deregister(&pn544_dev->pn544_device);
	mutex_destroy(&pn544_dev->read_mutex);
	gpio_free(pn544_dev->irq_gpio);
	gpio_free(pn544_dev->ven_gpio);
	gpio_free(pn544_dev->firm_gpio);
#ifdef CONFIG_NFC_PN547
	gpio_free(pn544_dev->clk_req_gpio);
	msm_xo_put(pn544_dev->nfc_clock);
#endif
	kfree(pn544_dev);

	return 0;
}

static const struct i2c_device_id pn544_id[] = {
	{"pn544", 0},
	{}
};

static struct i2c_driver pn544_driver = {
	.id_table = pn544_id,
	.probe = pn544_probe,
	.remove = pn544_remove,
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "pn544",
		   },
};

/*
 * module load/unload record keeping
 */

static int __init pn544_dev_init(void)
{
	pr_info("Loading pn544 driver\n");
	return i2c_add_driver(&pn544_driver);
}

module_init(pn544_dev_init);

static void __exit pn544_dev_exit(void)
{
	pr_info("Unloading pn544 driver\n");
	i2c_del_driver(&pn544_driver);
}

module_exit(pn544_dev_exit);

MODULE_AUTHOR("Sylvain Fonteneau");
MODULE_DESCRIPTION("NFC PN544 driver");
MODULE_LICENSE("GPL");
