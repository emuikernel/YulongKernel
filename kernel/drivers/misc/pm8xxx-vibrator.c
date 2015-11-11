/* Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mfd/pm8xxx/core.h>
#include <linux/mfd/pm8xxx/vibrator.h>

#include "../staging/android/timed_output.h"

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT      /* add by yanghao 2012/11/30 */
#include <yl_debug/management.h>
#endif

#define VIB_DRV			0x4A

#define VIB_DRV_SEL_MASK	0xf8
#define VIB_DRV_SEL_SHIFT	0x03
#define VIB_DRV_EN_MANUAL_MASK	0xfc
#define VIB_DRV_LOGIC_SHIFT	0x2

#define VIBRATOR_PWM_LEVEL_MIN	(0)
#define VIBRATOR_PWM_LEVEL_MAX	(100)
#define VIB_MAX_LEVEL_mV	3100
#define VIB_MIN_LEVEL_mV	1200
#define PWM_VIBRATOR_NAME	"vibrator"
static unsigned int pwm_level = 100 ;

struct pm8xxx_vib {
	struct hrtimer vib_timer;
	struct timed_output_dev timed_dev;
	struct mutex lock; /*yangliang mask and add1212 */
	struct device *dev;
	const struct pm8xxx_vibrator_platform_data *pdata;
	int state;
	int level;
	u8  reg_vib_drv;
	struct cdev vibrator_cdev; /* shezhi */
	unsigned int vibrator_major;
#ifdef CONFIG_YULONG_MODULE_MANAGEMENT /* add by yanghao 2012/11/30 */
	int power_state;
#endif
};

/*yangliang add for using workqueue independent1211 start*/
static struct workqueue_struct *vib_wq;
static struct work_struct vib_work;
static void pm8xxx_vib_update(struct work_struct *work);
/*yangliang add for using workqueue independent1211 end*/

static struct pm8xxx_vib *vib_dev;

int pm8xxx_vibrator_config(struct pm8xxx_vib_config *vib_config)
{
	u8 reg = 0;
	int rc;

	if (vib_dev == NULL) {
		pr_err("%s: vib_dev is NULL\n", __func__);
		return -EINVAL;
	}

	if (vib_config->drive_mV) {
		if ((vib_config->drive_mV < VIB_MIN_LEVEL_mV) ||
			(vib_config->drive_mV > VIB_MAX_LEVEL_mV)) {
			pr_err("Invalid vibrator drive strength\n");
			return -EINVAL;
		}
	}

	reg = (vib_config->drive_mV / 100) << VIB_DRV_SEL_SHIFT;

	reg |= (!!vib_config->active_low) << VIB_DRV_LOGIC_SHIFT;

	reg |= vib_config->enable_mode;

	rc = pm8xxx_writeb(vib_dev->dev->parent, VIB_DRV, reg);
	if (rc)
		pr_err("%s: pm8xxx write failed: rc=%d\n", __func__, rc);

	return rc;
}
EXPORT_SYMBOL(pm8xxx_vibrator_config);

/* REVISIT: just for debugging, will be removed in final working version */
static void __dump_vib_regs(struct pm8xxx_vib *vib, char *msg)
{
	u8 temp;

	dev_dbg(vib->dev, "%s\n", msg);

	pm8xxx_readb(vib->dev->parent, VIB_DRV, &temp);
	dev_dbg(vib->dev, "VIB_DRV - %X\n", temp);
}

static int pm8xxx_vib_read_u8(struct pm8xxx_vib *vib,
				 u8 *data, u16 reg)
{
	int rc;

	rc = pm8xxx_readb(vib->dev->parent, reg, data);
	if (rc < 0)
		dev_warn(vib->dev, "Error reading pm8xxx: %X - ret %X\n",
				reg, rc);

	return rc;
}

static int pm8xxx_vib_write_u8(struct pm8xxx_vib *vib,
				 u8 data, u16 reg)
{
	int rc;

	rc = pm8xxx_writeb(vib->dev->parent, reg, data);
	if (rc < 0)
		dev_warn(vib->dev, "Error writing pm8xxx: %X - ret %X\n",
				reg, rc);
	return rc;
}


static int vibrator_open(struct inode *inode, struct file *filp)
{
	if (inode == NULL || filp == NULL)
		return -EINVAL;

	filp->private_data = vib_dev;
	return 0;
}

static int vibrator_close(struct inode *inode, struct file *file)
{
	if (inode == NULL || file == NULL)
		return -EINVAL;

	return 0;
}

static ssize_t vibrator_read(struct file *filp, char __user *buf,
		size_t count, loff_t *ppos)
{
	if (filp == NULL || buf == NULL || ppos == NULL)
		return -EINVAL;

	return 1;
}

static ssize_t vibrator_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *ppos)
{
	char level;
	size_t count_temp = 0;

	if (filp == NULL || buf == NULL || ppos == NULL)
		return -EINVAL;

	count_temp = copy_from_user((&level), buf, count);
	if (level < VIBRATOR_PWM_LEVEL_MIN || level > VIBRATOR_PWM_LEVEL_MAX) {
		pr_err("arg is invalid, need between 0 < arg < 100\n");
		return 0;
	}

	pwm_level = level;
	return 1;
}

static int pm8xxx_vib_set(struct pm8xxx_vib *vib, int on)
{
	int rc;
	u8 val;

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT /* add by yanghao 2012/11/30 */
	if (on)
		vib->power_state = SET_MODULE_ON;
	else
		vib->power_state = SET_MODULE_OFF;
#endif

	/* huangqingjun add 2012/10/26 */
	if (85 < pwm_level && pwm_level <= 100)
		vib->level = VIB_MAX_LEVEL_mV/100;
	else if (70 < pwm_level && pwm_level <= 85)
		vib->level = ((VIB_MAX_LEVEL_mV-VIB_MIN_LEVEL_mV)*75/100+VIB_MIN_LEVEL_mV)/100;
	else if (55 < pwm_level && pwm_level <= 70)
		vib->level = (2500)/100;
	else if (30 < pwm_level && pwm_level <= 55)
		vib->level = (2300)/100;
	else if (0 < pwm_level && pwm_level <= 30)
		vib->level = ((VIB_MAX_LEVEL_mV-VIB_MIN_LEVEL_mV)*25/100+VIB_MIN_LEVEL_mV)/100;
	else
		vib->level = VIB_MAX_LEVEL_mV/100;

	pwm_level = 0;
	if (on) {
		val = vib->reg_vib_drv;
		val |= ((vib->level << VIB_DRV_SEL_SHIFT) & VIB_DRV_SEL_MASK);
		rc = pm8xxx_vib_write_u8(vib, val, VIB_DRV);
		if (rc < 0)
			return rc;
		vib->reg_vib_drv = val;
	} else {
		val = vib->reg_vib_drv;
		val &= ~VIB_DRV_SEL_MASK;
		rc = pm8xxx_vib_write_u8(vib, val, VIB_DRV);
		if (rc < 0)
			return rc;
		vib->reg_vib_drv = val;
	}
	__dump_vib_regs(vib, "vib_set_end");

	return rc;
}

static void pm8xxx_vib_enable(struct timed_output_dev *dev, int value)
{
	struct pm8xxx_vib *vib = container_of(dev, struct pm8xxx_vib,
					 timed_dev);


retry:
	mutex_lock(&vib->lock);
	if (hrtimer_try_to_cancel(&vib->vib_timer) < 0) {
		mutex_unlock(&vib->lock);
		cpu_relax();
		goto retry;
	}

	if (value == 0)
		vib->state = 0;
	else {
		value = (value > vib->pdata->max_timeout_ms ?
				 vib->pdata->max_timeout_ms : value);
		vib->state = 1;
		hrtimer_start(&vib->vib_timer,
			      ktime_set(value / 1000, (value % 1000) * 1000000),
			      HRTIMER_MODE_REL);
	}
	mutex_unlock(&vib->lock);
	queue_work(vib_wq, &vib_work);
}

static void pm8xxx_vib_update(struct work_struct *work)
{
	/* yangliang modify1211 vib change to vib_dev */
	pm8xxx_vib_set(vib_dev, vib_dev->state);
}

static int pm8xxx_vib_get_time(struct timed_output_dev *dev)
{
	struct pm8xxx_vib *vib = container_of(dev, struct pm8xxx_vib,
							timed_dev);

	if (hrtimer_active(&vib->vib_timer)) {
		ktime_t r = hrtimer_get_remaining(&vib->vib_timer);
		return (int)ktime_to_us(r);
	} else
		return 0;
}

static enum hrtimer_restart pm8xxx_vib_timer_func(struct hrtimer *timer)
{
	struct pm8xxx_vib *vib = container_of(timer, struct pm8xxx_vib,
							 vib_timer);

	vib->state = 0;

	queue_work(vib_wq, &vib_work);
	return HRTIMER_NORESTART;
}

#ifdef CONFIG_PM
static int pm8xxx_vib_suspend(struct device *dev)
{
	struct pm8xxx_vib *vib = dev_get_drvdata(dev);

	hrtimer_cancel(&vib->vib_timer);
	cancel_work_sync(&vib_work); /* yangliang modify1211 */
	/* turn-off vibrator */
	pm8xxx_vib_set(vib, 0);

	return 0;
}

static const struct dev_pm_ops pm8xxx_vib_pm_ops = {
	.suspend = pm8xxx_vib_suspend,
};
#endif

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT  /* add by yanghao 2012/11/30 */
static int vib_get(struct module_management *vib)
{
	return vib_dev->power_state;
}

static int vib_control(struct module_management *vib, unsigned int cmd)
{
	hrtimer_cancel(&vib_dev->vib_timer);
	cancel_work_sync(&vib_work);
	if (cmd == SET_MODULE_ON) {
		pm8xxx_vib_set(vib_dev, 1);
		pr_debug("vibrator cmd = SET_MODULE_ON\n");
	} else if (cmd == SET_MODULE_OFF) {
		/* turn-off vibrator */
		pm8xxx_vib_set(vib_dev, 0);
		pr_debug("vibrator cmd = SET_MODULE_OFF\n");
	}
	return 0;
}

static struct module_management vib_moudle_manager = {
	.name = PM8XXX_VIBRATOR_DEV_NAME,
	.get = vib_get,
	.control = vib_control,
};
#endif

static int __devinit pm8xxx_vib_probe(struct platform_device *pdev)

{
	const struct pm8xxx_vibrator_platform_data *pdata =
						pdev->dev.platform_data;
	struct pm8xxx_vib *vib;
	u8 val;
	int rc;

	if (!pdata)
		return -EINVAL;

	if (pdata->level_mV < VIB_MIN_LEVEL_mV ||
			 pdata->level_mV > VIB_MAX_LEVEL_mV)
		return -EINVAL;

	vib = kzalloc(sizeof(*vib), GFP_KERNEL);
	if (!vib)
		return -ENOMEM;

	vib->pdata	= pdata;
	vib->level	= pdata->level_mV / 100;
	vib->dev	= &pdev->dev;

	mutex_init(&vib->lock);
	INIT_WORK(&vib_work, pm8xxx_vib_update);
	vib_wq = create_workqueue("vibwq");
	if (vib_wq == NULL) {
		rc = -ENOMEM;
		goto err_create_wq;
	}

	hrtimer_init(&vib->vib_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	vib->vib_timer.function = pm8xxx_vib_timer_func;

	vib->timed_dev.name = "vibrator";
	vib->timed_dev.get_time = pm8xxx_vib_get_time;
	vib->timed_dev.enable = pm8xxx_vib_enable;

	__dump_vib_regs(vib, "boot_vib_default");

	/*
	 * Configure the vibrator, it operates in manual mode
	 * for timed_output framework.
	 */
	rc = pm8xxx_vib_read_u8(vib, &val, VIB_DRV);
	if (rc < 0)
		goto err_read_vib;
	val &= ~VIB_DRV_EN_MANUAL_MASK;
	rc = pm8xxx_vib_write_u8(vib, val, VIB_DRV);
	if (rc < 0)
		goto err_read_vib;

	vib->reg_vib_drv = val;

	rc = timed_output_dev_register(&vib->timed_dev);
	if (rc < 0)
		goto err_read_vib;

	vib_dev = vib;
	platform_set_drvdata(pdev, vib);



	return 0;

err_read_vib:
	kfree(vib);
err_create_wq:
	destroy_workqueue(vib_wq);
	vib_wq = NULL;
	vib_dev = NULL;
	return rc;
}

static int __devexit pm8xxx_vib_remove(struct platform_device *pdev)
{
	struct pm8xxx_vib *vib = platform_get_drvdata(pdev);

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
	yulong_unregister_module_management(&vib_moudle_manager);
#endif

	cancel_work_sync(&vib_work);
	hrtimer_cancel(&vib->vib_timer);
	timed_output_dev_unregister(&vib->timed_dev);
	destroy_workqueue(vib_wq);
	vib_wq = NULL;
	platform_set_drvdata(pdev, NULL);
	kfree(vib);

	return 0;
}

static struct platform_driver pm8xxx_vib_driver = {
	.probe		= pm8xxx_vib_probe,
	.remove		= __devexit_p(pm8xxx_vib_remove),
	.driver		= {
		.name	= PM8XXX_VIBRATOR_DEV_NAME,
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &pm8xxx_vib_pm_ops,
#endif
	},
};

static const struct file_operations vibrator_fops = {
	.owner       = THIS_MODULE,
	.open        = vibrator_open,
	.release     = vibrator_close,
	.write       = vibrator_write,
	.read        = vibrator_read,
};

static void vibrator_setup_cdev(struct pm8xxx_vib *dev, int index)
{
	unsigned int err;
	int dev_number = 0;

	dev_number = MKDEV(vib_dev->vibrator_major, index);

	if ((dev == NULL) || (&dev->vibrator_cdev == NULL)) {
		;
	}

	cdev_init(&dev->vibrator_cdev, &vibrator_fops);

	dev->vibrator_cdev.owner = THIS_MODULE;
	dev->vibrator_cdev.ops = &vibrator_fops;

	err = cdev_add(&dev->vibrator_cdev, dev_number, 1);

	if (err) {
		;
	}

	return;
}

static int __init pm8xxx_vib_init(void)
{
	static struct class *pvibrator_class_device;
	unsigned int result;
	dev_t dev_number;

	vib_dev = kzalloc(sizeof(*vib_dev), GFP_KERNEL);
	if (!vib_dev) {
		return -ENOMEM;
		goto error;
	}
	memset(vib_dev, 0, sizeof(*vib_dev));

	result = alloc_chrdev_region(&dev_number, 0, 1, PWM_VIBRATOR_NAME);
	if (!result)
		vib_dev->vibrator_major = MAJOR(dev_number);
	else
		return result;

	/* 0: the minor device number */
	vibrator_setup_cdev(vib_dev, 0);

	pvibrator_class_device =
		class_create(THIS_MODULE, "pwm-vibrator-class");
	if (IS_ERR(pvibrator_class_device))
		pr_debug("vibrator_init: vibrator register node fail!\r\n");

	device_create(pvibrator_class_device, NULL,
		MKDEV(vib_dev->vibrator_major, 0), NULL, PWM_VIBRATOR_NAME);

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT /* add by yanghao 2012/11/30 */
	yulong_register_module_management(&vib_moudle_manager);
	vib_dev->power_state = SET_MODULE_OFF;
#endif
	return platform_driver_register(&pm8xxx_vib_driver);
	return 0;

error:
	if (NULL != vib_dev) {
		kfree(vib_dev);
		vib_dev = NULL;
	}

	return -EINVAL;
}
module_init(pm8xxx_vib_init);

static void __exit pm8xxx_vib_exit(void)
{
	platform_driver_unregister(&pm8xxx_vib_driver);
}
module_exit(pm8xxx_vib_exit);

MODULE_ALIAS("platform:" PM8XXX_VIBRATOR_DEV_NAME);
MODULE_DESCRIPTION("pm8xxx vibrator driver");
MODULE_LICENSE("GPL v2");
