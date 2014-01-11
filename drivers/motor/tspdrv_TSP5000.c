/*
** =========================================================================
** File:
**     tspdrv_TSP5000.c
**
** Description:
**     TouchSense Kernel Module main entry-point.
**
** Portions Copyright (c) 2008-2010 Immersion Corporation. All Rights Reserved.
**
** This file contains Original Code and/or Modifications of Original Code
** as defined in and that are subject to the GNU Public License v2 -
** (the 'License'). You may not use this file except in compliance with the
** License. You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or contact
** TouchSenseSales@immersion.com.
**
** The Original Code and all software distributed under the License are
** distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
** EXPRESS OR IMPLIED, AND IMMERSION HEREBY DISCLAIMS ALL SUCH WARRANTIES,
** INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. Please see
** the License for the specific language governing rights and limitations
** under the License.
** =========================================================================
*/

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/wakelock.h>
#include <linux/io.h>
#include "../staging/android/timed_output.h"

#include "tspdrv_TSP5000.h"
#include <asm/atomic.h>
#include <linux/vibrator.h>
static int g_nTimerPeriodMs = 5; /* 5ms timer by default. This variable could be used by the SPI.*/
static int8_t g_bNackDetected =  false;

#ifdef CONFIG_MOTOR_DRV_TSP5000
#include "immvibespi_TSP5000.c"
#else
#include "immvibespi.c"
#endif
#if defined(VIBE_DEBUG) && defined(VIBE_RECORD)
#include <tspdrvRecorder.c>
#endif

/* Device name and version information */
/* DO NOT CHANGE - this is auto-generated */

#define VERSION_STR " v3.6.12.1\n"			/* DO NOT CHANGE - this is auto-generated */

/* account extra space for future extra digits in version number */
#define VERSION_STR_LEN 16
/* initialized in tspdrv_probe */
static char g_szDeviceName[  (VIBE_MAX_DEVICE_NAME_LENGTH
							+ VERSION_STR_LEN)
							* NUM_ACTUATORS];
static size_t g_cchDeviceName;				/* initialized in init_module */

static struct wake_lock vib_wake_lock;

/* Flag indicating whether the driver is in use */
static char g_bIsPlaying = false;

/* Flag indicating whether the debug level*/
static atomic_t g_nDebugLevel;
/* Buffer to store data sent to SPI */
#define MAX_SPI_BUFFER_SIZE (NUM_ACTUATORS * (VIBE_OUTPUT_SAMPLE_SIZE + SPI_HEADER_SIZE))

static char g_cWriteBuffer[MAX_SPI_BUFFER_SIZE];

#define VIBE_TUNING
/* #define VIBE_ENABLE_SYSTEM_TIMER */

#if ((LINUX_VERSION_CODE & 0xFFFF00) < KERNEL_VERSION(2,6,0))
#error Unsupported Kernel version
#endif

#ifndef HAVE_UNLOCKED_IOCTL
#define HAVE_UNLOCKED_IOCTL 0
#endif

#ifdef IMPLEMENT_AS_CHAR_DRIVER
static int g_nMajor = 0;
#endif

/* Needs to be included after the global variables because it uses them */
#include "tspdrvOutputDataHandler.c"
	#include "VibeOSKernelLinuxHRTime_TSP5000.c"

asmlinkage void _DbgOut(int level, const char *fmt,...)
{
	int nDbgLevel = atomic_read(&g_nDebugLevel);

	if (0 <= level && level <= nDbgLevel) {
		static char printk_buf[MAX_DEBUG_BUFFER_LENGTH];
		static char prefix[6][4] =
			{" * ", " ! ", " ? ", " I ", " V", " O "};
		va_list args;
		int ret;
		size_t size = sizeof(printk_buf);

		va_start(args, fmt);

		ret = scnprintf(printk_buf, size, KERN_EMERG "%s:%s %s",
		MODULE_NAME, prefix[level], fmt);
		if (ret >= size) return;

		vprintk(printk_buf, args);
		va_end(args);
	}
}


/* timed_output */
static void _set_vibetonz_work(struct work_struct *unused);

static DECLARE_WORK(vibetonz_work, _set_vibetonz_work);


static struct hrtimer timer;
static int max_timeout = 10000;

static int vibrator_value = -1;
static int vibrator_work;

#define TEST_MODE_TIME 10000

struct vibrator_platform_data vibrator_drvdata;

static int set_vibetonz(int timeout)
{
	int8_t strength;
	if (!timeout) {
		if (vibrator_drvdata.vib_model == HAPTIC_PWM) {
			strength = 0;
			ImmVibeSPI_ForceOut_SetSamples(0, 8, 1, &strength);
		} else { /* HAPTIC_MOTOR */
			ImmVibeSPI_ForceOut_AmpDisable(0);
		}
	} else {
		DbgOut((KERN_INFO "tspdrv: ENABLE\n"));
		if (vibrator_drvdata.vib_model == HAPTIC_PWM) {
			strength = 120;
			/* 90% duty cycle */
			ImmVibeSPI_ForceOut_SetSamples(0, 8, 1, &strength);
		} else { /* HAPTIC_MOTOR */
			ImmVibeSPI_ForceOut_AmpEnable(0);
		}
	}

	vibrator_value = timeout;
	return 0;
}

static void _set_vibetonz_work(struct work_struct *unused)
{
	set_vibetonz(vibrator_work);

	return;
}

static enum hrtimer_restart vibetonz_timer_func(struct hrtimer *timer)
{
	/* set_vibetonz(0); */
	vibrator_work = 0;
	schedule_work(&vibetonz_work);

	return HRTIMER_NORESTART;
}

static int get_time_for_vibetonz(struct timed_output_dev *dev)
{
	int remaining;

	if (hrtimer_active(&timer))	{
		ktime_t r = hrtimer_get_remaining(&timer);
		remaining = ktime_to_ms(r);/*returning time in ms*/
	} else {
		remaining = 0;
	}

	if (vibrator_value == -1)
		remaining = -1;

	return remaining;

}

static void enable_vibetonz_from_user(struct timed_output_dev *dev, int value)
{
	printk(KERN_DEBUG "tspdrv: Enable time = %d msec\n", value);
	hrtimer_cancel(&timer);

	/* set_vibetonz(value); */
	vibrator_work = value;
	schedule_work(&vibetonz_work);

	if (value > 0) {
		if (value > max_timeout)
			value = max_timeout;

		hrtimer_start(&timer,
			ktime_set(value / 1000, (value % 1000) * 1000000),
			HRTIMER_MODE_REL);
		vibrator_value = 0;
	}
}

static struct timed_output_dev timed_output_vt = {
	.name     = "vibrator",
	.get_time = get_time_for_vibetonz,
	.enable   = enable_vibetonz_from_user,
};

static void vibetonz_start(void)
{
	int ret = 0;

	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer.function = vibetonz_timer_func;

	ret = timed_output_dev_register(&timed_output_vt);

	if (ret)
		DbgOut((KERN_ERR
		"tspdrv: timed_output_dev_register is fail\n"));
}

/* File IO */
static int open(struct inode *inode, struct file *file);
static int release(struct inode *inode, struct file *file);
static ssize_t read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t write(struct file *file, const char *buf, size_t count,
					 loff_t *ppos);
#if HAVE_UNLOCKED_IOCTL
static long ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif
static const struct file_operations fops = {
	.owner =    THIS_MODULE,
	.read =     read,
	.write =    write,
	.unlocked_ioctl =    ioctl,
	.open =     open,
	.release =  release ,
	.llseek =   default_llseek
};

#ifndef IMPLEMENT_AS_CHAR_DRIVER
static struct miscdevice miscdev = {
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     MODULE_NAME,
	.fops =     &fops
};
#endif

#ifdef VIBE_ENABLE_SYSTEM_TIMER
int vibetonz_clk_on(struct device *dev)
{
	struct clk *vibetonz_clk = NULL;

	vibetonz_clk = clk_get(dev, "timers");
	if (IS_ERR(vibetonz_clk)) {
		DbgOut((KERN_ERR "tspdrv: failed to get clock for vibetonz\n"));
		goto err_clk0;
	}
	clk_enable(vibetonz_clk);
	clk_put(vibetonz_clk);

	return 0;

err_clk0:
	clk_put(vibetonz_clk);
	return -EINVAL;
}

int vibetonz_clk_off(struct device *dev)
{
	struct clk *vibetonz_clk = NULL;

	vibetonz_clk = clk_get(dev, "timers");
	if (IS_ERR(vibetonz_clk)) {
		DbgOut((KERN_ERR "tspdrv: failed to get clock for vibetonz\n"));
		goto err_clk0;
	}
	clk_disable(vibetonz_clk);
	clk_put(vibetonz_clk);

	return 0;

err_clk0:
	clk_put(vibetonz_clk);

	return -EINVAL;
}
#else
int vibetonz_clk_on(struct device *dev)
{
	return -EINVAL;
}

int vibetonz_clk_off(struct device *dev)
{
	return -EINVAL;
}
#endif	/* VIBE_ENABLE_SYSTEM_TIMER */

static __devinit int tspdrv_probe(struct platform_device *pdev)
{
	struct vibrator_platform_data *pdata;
	int ret = 0, i;   /* initialized below */
	int nRet=0;

	DbgOut((KERN_INFO "tspdrv: tspdrv_probe.\n"));
    atomic_set(&g_nDebugLevel, DBL_ERROR);
	DbgOut((KERN_INFO  "tspdrv: init_module.\n"));
	pdata = pdev->dev.platform_data;
	if (!pdata) {
		DbgOut((KERN_ERR "tspdrv: no platfrom_data\n"));
		goto fail;
	}

	vibrator_drvdata.vib_model = pdata->vib_model;
	vibrator_drvdata.power_onoff = pdata->power_onoff;
	vibrator_drvdata.is_pmic_haptic_pwr_en = \
					pdata->is_pmic_haptic_pwr_en;
	vibrator_drvdata.is_no_haptic_pwr = pdata->is_no_haptic_pwr;
	if (!vibrator_drvdata.is_no_haptic_pwr) {
		if (pdata->is_pmic_haptic_pwr_en)
			vibrator_drvdata.haptic_pwr_en_gpio = \
			PM8921_GPIO_PM_TO_SYS(pdata->haptic_pwr_en_gpio);
		else
			vibrator_drvdata.haptic_pwr_en_gpio = \
				pdata->haptic_pwr_en_gpio;
	}

	if (pdata->vib_model == HAPTIC_PWM) {
		vibrator_drvdata.vib_pwm_gpio = pdata->vib_pwm_gpio;
		vibrator_drvdata.is_pmic_vib_en = \
			pdata->is_pmic_vib_en;
		if (pdata->is_pmic_vib_en)
			vibrator_drvdata.vib_en_gpio = \
			PM8921_GPIO_PM_TO_SYS(pdata->vib_en_gpio);
		else
			vibrator_drvdata.vib_en_gpio = \
				pdata->vib_en_gpio;
	}

#ifdef IMPLEMENT_AS_CHAR_DRIVER
    g_nMajor = register_chrdev(0, MODULE_NAME, &fops);
    if (g_nMajor < 0)
	{
        DbgOut((KERN_ERR "tspdrv: can't get major number.\n"));
        return g_nMajor;
    }
#else
    nRet = misc_register(&miscdev);
	if (nRet)
	{
        DbgOut((KERN_ERR "tspdrv: misc_register failed.\n"));
		return nRet;
	}
#endif

	DbgRecorderInit(());

	vibetonz_clk_on(&pdev->dev);

	ImmVibeSPI_ForceOut_Initialize();
	VibeOSKernelLinuxInitTimer();
    ResetOutputData();

	/* Get and concatenate device name and initialize data buffer */
	g_cchDeviceName = 0;
	for (i = 0; i < NUM_ACTUATORS; i++) {
		char *szName = g_szDeviceName + g_cchDeviceName;
		ImmVibeSPI_Device_GetName(i,
			szName, VIBE_MAX_DEVICE_NAME_LENGTH);

		/* Append version information and get buffer length */
		strlcat(szName, VERSION_STR, sizeof(VERSION_STR));
		g_cchDeviceName += strnlen(szName, sizeof(szName));

	}
	wake_lock_init(&vib_wake_lock, WAKE_LOCK_SUSPEND, "vib_present");

	vibetonz_start();

	return 0;
#if 0

register_err:
#ifdef IMPLEMENT_AS_CHAR_DRIVER
	unregister_chrdev(g_nmajor, MODULE_NAME);
#else
	misc_deregister(&miscdev);
#endif
#endif
fail:
	return ret;
}

static int __devexit tspdrv_remove(struct platform_device *pdev)
{
	DbgOut((KERN_INFO "tspdrv: tspdrv_remove.\n"));

	DbgRecorderTerminate(());

	VibeOSKernelLinuxTerminateTimer();
	ImmVibeSPI_ForceOut_Terminate();

	wake_lock_destroy(&vib_wake_lock);

	return 0;
}

static int open(struct inode *inode, struct file *file)
{
	DbgOut((KERN_INFO "tspdrv: open.\n"));

	if (!try_module_get(THIS_MODULE))
		return -ENODEV;

	return 0;
}

static int release(struct inode *inode, struct file *file)
{
	DbgOut((KERN_INFO "tspdrv: release.\n"));

	/*
	** Reset force and stop timer when the driver is closed, to make sure
	** no dangling semaphore remains in the system, especially when the
	** driver is run outside of immvibed for testing purposes.
	*/
	VibeOSKernelLinuxStopTimer();

	/*
	** Clear the variable used to store the magic number to prevent
	** unauthorized caller to write data. TouchSense service is the only
	** valid caller.
	*/
	file->private_data = (void *)NULL;

	module_put(THIS_MODULE);

	return 0;
}

static ssize_t read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	const size_t nbufsize =
			(g_cchDeviceName > (size_t)(*ppos)) ?
			min(count, g_cchDeviceName - (size_t)(*ppos)) : 0;

	/* End of buffer, exit */
	if (0 == nbufsize)
		return 0;

	if (0 != copy_to_user(buf, g_szDeviceName + (*ppos), nbufsize)) {
		/* Failed to copy all the data, exit */
		DbgOut((KERN_ERR "tspdrv: copy_to_user failed.\n"));
		return 0;
	}

	/* Update file position and return copied buffer size */
	*ppos += nbufsize;
	return nbufsize;
}

static ssize_t write(struct file *file, const char *buf, size_t count,
					 loff_t *ppos)
{
	*ppos = 0;  /* file position not used, always set to 0 */

	/*
	** Prevent unauthorized caller to write data.
	** TouchSense service is the only valid caller.
	*/
	if (file->private_data != (void*)TSPDRV_MAGIC_NUMBER)
	{
		DbgOut((KERN_ERR "tspdrv: unauthorized write.\n"));
		return 0;
	}

	/*
	** Ignore packets that have size smaller than SPI_HEADER_SIZE or bigger than MAX_SPI_BUFFER_SIZE.
	** Please note that the daemon may send an empty buffer (count == SPI_HEADER_SIZE)
	** during quiet time between effects while playing a Timeline effect in order to maintain
	** correct timing: if "count" is equal to SPI_HEADER_SIZE, the call to VibeOSKernelLinuxStartTimer()
	** will just wait for the next timer tick.
	*/
	if ((count < SPI_HEADER_SIZE) || (count > MAX_SPI_BUFFER_SIZE))
	{
		DbgOut((KERN_ERR "tspdrv: invalid buffer size.\n"));
		return 0;
	}

	/* Copy immediately the input buffer */
	if (0 != copy_from_user(g_cWriteBuffer, buf, count))
	{
		/* Failed to copy all the data, exit */
		DbgOut((KERN_ERR "tspdrv: copy_from_user failed.\n"));
		return 0;
	}

	/* Extract force output samples and save them in an internal buffer */
	if (!SaveOutputData(g_cWriteBuffer, count))
	{
		DbgOut((KERN_ERR "tspdrv: SaveOutputData failed.\n"));
		return 0;
	}

	/* Start the timer after receiving new output force */
	g_bIsPlaying = true;

	VibeOSKernelLinuxStartTimer();

	return count;
}

static long ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	/* DbgOut(KERN_INFO "tspdrv: ioctl cmd[0x%x].\n", cmd); */
	switch (cmd) {
		case TSPDRV_SET_MAGIC_NUMBER:
			file->private_data = (void*)TSPDRV_MAGIC_NUMBER;
			break;

	case TSPDRV_ENABLE_AMP:
		wake_lock(&vib_wake_lock);
		ImmVibeSPI_ForceOut_AmpEnable(arg);
		DbgRecorderReset((arg));
		DbgRecord((arg, ";------- TSPDRV_ENABLE_AMP ---------\n"));
		break;

	case TSPDRV_DISABLE_AMP:
		ImmVibeSPI_ForceOut_AmpDisable(arg);
		wake_unlock(&vib_wake_lock);
		break;

	case TSPDRV_GET_NUM_ACTUATORS:
		return NUM_ACTUATORS;
		case TSPDRV_SET_DBG_LEVEL:
			{
				long nDbgLevel;
				if (0 != copy_from_user((void *)&nDbgLevel, (const void __user *)arg, sizeof(long))) {
					/* Error copying the data */
					DbgOut((KERN_ERR "copy_from_user failed to copy debug level data.\n"));
					return -1;
				}

				if (DBL_TEMP <= nDbgLevel &&  nDbgLevel <= DBL_OVERKILL) {
					atomic_set(&g_nDebugLevel, nDbgLevel);
				} else {
					DbgOut((KERN_DEBUG "Invalid debug level requested, ignored."));
				}

				break;
			}

		case TSPDRV_GET_DBG_LEVEL:
			return atomic_read(&g_nDebugLevel);

		case TSPDRV_SET_DEVICE_PARAMETER:
			{
				device_parameter deviceParam;

				if (0 != copy_from_user((void *)&deviceParam, (const void __user *)arg, sizeof(deviceParam)))
				{
					/* Error copying the data */
					DbgOut((KERN_ERR "tspdrv: copy_from_user failed to copy kernel parameter data.\n"));
					return -1;
				}

				switch (deviceParam.nDeviceParamID)
				{
					case VIBE_KP_CFG_UPDATE_RATE_MS:
						/* Update the timer period */
						g_nTimerPeriodMs = deviceParam.nDeviceParamValue;



#ifdef CONFIG_HIGH_RES_TIMERS
						/* For devices using high resolution timer we need to update the ktime period value */
						g_ktTimerPeriod = ktime_set(0, g_nTimerPeriodMs * 950000/*1000000*/);
#endif
						break;

					case VIBE_KP_CFG_FREQUENCY_PARAM1:
					case VIBE_KP_CFG_FREQUENCY_PARAM2:
					case VIBE_KP_CFG_FREQUENCY_PARAM3:
					case VIBE_KP_CFG_FREQUENCY_PARAM4:
					case VIBE_KP_CFG_FREQUENCY_PARAM5:
					case VIBE_KP_CFG_FREQUENCY_PARAM6:
						if (0 > ImmVibeSPI_ForceOut_SetFrequency(deviceParam.nDeviceIndex, deviceParam.nDeviceParamID, deviceParam.nDeviceParamValue))
						{
							DbgOut((KERN_ERR "tspdrv: cannot set device frequency parameter.\n"));
							return -1;
						}
						break;
				}
			}
		}
	return 0;
}

static int suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret;

	if (g_bIsPlaying)
	{
		ret = -EBUSY;
	} else {
		/* Disable system timers */
		vibetonz_clk_off(&pdev->dev);

		ret = 0;
	}
	DbgOut(KERN_DEBUG "tspdrv: %s (%d).\n", __func__, ret);
	return ret;
}

static int resume(struct platform_device *pdev)
{
	/* Restart system timers */
	DbgOut(KERN_DEBUG "tspdrv: %s.\n", __func__);
	return 0;
}

static struct platform_driver tspdrv_driver = {
	.probe = tspdrv_probe,
	.remove = __devexit_p(tspdrv_remove),
	.suspend = suspend,
	.resume = resume,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init tspdrv_init(void)
{
	return platform_driver_register(&tspdrv_driver);
}

static void __exit tspdrv_exit(void)
{
	platform_driver_unregister(&tspdrv_driver);
}

late_initcall(tspdrv_init);
module_exit(tspdrv_exit);

/* Module info */
MODULE_AUTHOR("Immersion Corporation");
MODULE_DESCRIPTION("TouchSense Kernel Module");
MODULE_LICENSE("GPL v2");
