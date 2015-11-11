/****************************************************************************/
/*                                                                          */
/*             Copyright (c) 2011-2012  YULONG Company                      */
/*                                                                          */
/* PROPRIETARY RIGHTS of YULONG Company are involved in the                 */
/* subject matter of this material.  All manufacturing, reproduction, use,  */
/* and sales rights pertaining to this subject matter are governed by the   */
/* license agreement.  The recipient of this software implicitly accepts    */
/* the terms of the license.                                                */
/*                                                                          */
/****************************************************************************/

/*
 * MP Communication module
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <linux/poll.h>
#include <mach/gpio.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "android_debug.h"

#define USB_SEL		100
#define SEL_UART	132

static int android_debug_major;
struct ANDROID_DEBUG_DEV *g_pandroid_debug_dev;
static struct class *sg_pandroid_debug_class;

static int android_debug_release(struct inode *inode, struct file *filp);
static int android_debug_open(struct inode *inode, struct file *filp);
static long android_debug_iocontrol(struct file *filp,
			unsigned int cmd, unsigned long arg);
static int android_debug_read(struct file *filp, char __user *buf,
			size_t count, loff_t *ppos);
static int android_debug_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *ppos);
static loff_t android_debug_seek(struct file *filp, loff_t offset, int orig);

static const struct file_operations android_debug_fops = {
	.owner       = THIS_MODULE,
	.open        = android_debug_open,
	.release     = android_debug_release,
	.write       = android_debug_write,
	.read        = android_debug_read,
	.llseek      = android_debug_seek,
	.unlocked_ioctl       = android_debug_iocontrol,
};

static int android_debug_resume(struct platform_device *dev)
{
	return 0;
}

static int android_debug_suspend(struct platform_device *dev,
			pm_message_t state)
{
	return 0;
}

static int android_debug_probe(struct platform_device *pdev)
{
	pr_debug("++android_debug_probe()!\n");
	if (NULL == pdev) {
		pr_err("pdev = NULL\n");
		return -ENOMEM;
	}
	pr_debug("--android_debug_probe()!\n");

	return 0;
}

static int android_debug_remove(struct platform_device *dev)
{
	if (NULL == dev) {
		pr_err("dev = NULL\n");
		return -ENOMEM;
	}

	if (NULL != g_pandroid_debug_dev) {
		kfree(g_pandroid_debug_dev);
		g_pandroid_debug_dev = NULL;
	}
	cdev_del(&g_pandroid_debug_dev->android_debug_cdev);
	unregister_chrdev_region(MKDEV(android_debug_major, 0), 1);
	return 0;
}

static struct platform_driver ANDROID_DEBUG_DRIVER = {
	.probe		= android_debug_probe,
	.remove		= android_debug_remove,
	.suspend	= android_debug_suspend,
	.resume		= android_debug_resume,
	.driver		= {
		.name	= "ANDROID_DEBUG",
		.owner	= THIS_MODULE,
	},
};


static void android_debug_setup_cdev(struct ANDROID_DEBUG_DEV *dev,
			int dev_index)
{
	int err = 0;
	int dev_number = 0;
	int maxdevicenum = 1;

	pr_debug("++%s!\n", __func__);
	if (dev == NULL) {
		pr_err("%s: dev = NULL\n", __func__);
		return;
	}

	dev_number = MKDEV(android_debug_major, dev_index);
	cdev_init(&dev->android_debug_cdev, &android_debug_fops);

	dev->android_debug_cdev.owner = THIS_MODULE;
	dev->android_debug_cdev.ops = &android_debug_fops;

	err = cdev_add(&dev->android_debug_cdev, dev_number, maxdevicenum);
	if (err)
		pr_err("%s: Error %d adding android_debug_cdev\n",
			__func__, err);

	pr_debug("--%s!\n", __func__);
}

static void switch_tail_to_wifi_log(void)
{

}

static void switch_tail_to_wifi_rf(void)
{

}

static void switch_tail_to_ap_usb(void)
{
	gpio_direction_output(USB_SEL, 0);
	gpio_direction_output(SEL_UART, 0);
}

static void switch_tail_to_ap_uart(void)
{
	gpio_direction_output(USB_SEL, 1);
	gpio_direction_output(SEL_UART, 0);
}

static void switch_tail_to_modem(void)
{
	gpio_direction_output(USB_SEL, 1);
	gpio_direction_output(SEL_UART, 1);
}

static void uart_switch(int uartSwitch)
{
	switch (uartSwitch) {
	case IOCTL_UART_SWITCH_AP:
		pr_debug("uart switch ap\n");
		switch_tail_to_ap_uart();
		break;
	case IOCTL_UART_SWITCH_WIFI_LOG:
		pr_debug("uart switch wifi log\n");
		switch_tail_to_wifi_log();
		break;
	case IOCTL_UART_SWITCH_MODEM:
		pr_debug("uart switch modem\n");
		switch_tail_to_modem();
		break;
	case IOCTL_UART_SWITCH_WIFI_RF_TEST:
		pr_debug("uart switch wifi rf test\n");
		switch_tail_to_wifi_rf();
		pr_debug("uart switch successful\n");
		break;
	default:
		pr_debug("other uart switch\n");
		break;
	}
}

static void usb_switch(int usbSwitch)
{
	switch (usbSwitch) {
	case IOCTL_USB_SWITCH_MODEM:
		break;
	case IOCTL_USB_SWITCH_AP:
		switch_tail_to_ap_usb();
		break;
	default:
		pr_warning("Don't support usb switch\n");
		break;
	}
	return;
}

static int __init android_debug_init(void)
{
	int dwRet = 0;
	dev_t dev_number = 0;
	int base_number = 0;
	int max_device_number = 1;

	pr_debug("++android_debug_init()!\n");

	dwRet = alloc_chrdev_region(&dev_number, base_number,
			max_device_number, "android-debug-device");
	if (!dwRet) {
		android_debug_major = MAJOR(dev_number);
		pr_debug("%s: android_debug major number is %d!\n",
			__func__, android_debug_major);
	} else {
		pr_err("%s: request android_debug major number fail!\n",
			__func__);
		goto out;
	}

	g_pandroid_debug_dev =
		kmalloc(sizeof(struct ANDROID_DEBUG_DEV), GFP_KERNEL);
	if (!g_pandroid_debug_dev) {
		pr_err("%s: kmalloc g_pandroid_debug_dev fail!\n", __func__);
		dwRet = -ENOMEM;
		goto erro_unchr;
	}
	memset(g_pandroid_debug_dev, 0, sizeof(struct ANDROID_DEBUG_DEV));

	android_debug_setup_cdev(g_pandroid_debug_dev, base_number);

	sg_pandroid_debug_class =
		class_create(THIS_MODULE, "android-debug-class");
	if (IS_ERR(sg_pandroid_debug_class))
		pr_err("%s: android_debug_device register node fail!\n",
			__func__);

	device_create(sg_pandroid_debug_class, NULL, MKDEV(android_debug_major,
				base_number), NULL, "yl_android_device");

	dwRet = platform_driver_register(&ANDROID_DEBUG_DRIVER);
	if (dwRet != 0)  {
		pr_err("%s: platform_driver_register is failure!\n", __func__);
		goto erro_undrv;
	}

	pr_debug("--%s!\n", __func__);
	return dwRet;

erro_undrv:
	if (NULL != g_pandroid_debug_dev) {
		kfree(g_pandroid_debug_dev);
		g_pandroid_debug_dev = NULL;
	}

erro_unchr:
	unregister_chrdev_region(dev_number, max_device_number);

out:
	pr_err("--android_debug_init Fail!\n");
	return dwRet;
}

static void __exit android_debug_deinit(void)
{
	return;
}

static int android_debug_open(struct inode *inode, struct file *filp)
{
	pr_debug("android_debug_open: +-\n");
	return 0;
}

static int android_debug_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static long android_debug_iocontrol(struct file *filp, unsigned int cmd,
			unsigned long arg)
{
	pr_debug("android_debug_iocontrol: ++\n");

	switch (cmd) {
	case IOCTL_UART_SWITCH_AP:
		pr_debug("%s: IOCTL_UART_SWITCH_AP\n", __func__);
		uart_switch(IOCTL_UART_SWITCH_AP);
		break;
	case IOCTL_UART_SWITCH_WIFI_LOG:
		pr_debug("%s: IOCTL_UART_SWITCH_WIFI_LOG\n", __func__);
		uart_switch(IOCTL_UART_SWITCH_WIFI_LOG);
		break;
	case IOCTL_UART_SWITCH_MODEM:
		pr_debug("%s: IOCTL_UART_SWITCH_MODEM\n", __func__);
		uart_switch(IOCTL_UART_SWITCH_MODEM);
		break;
	case IOCTL_UART_SWITCH_WIFI_RF_TEST:
		pr_debug("%s: IOCTL_UART_SWITCH_WIFI_RF_TEST\n", __func__);
		uart_switch(IOCTL_UART_SWITCH_WIFI_RF_TEST);
		break;
	case IOCTL_USB_SWITCH_MODEM:
		pr_debug("%s: IOCTL_USB_SWITCH_MODEM\n", __func__);
		usb_switch(IOCTL_USB_SWITCH_MODEM);
		break;
	case IOCTL_USB_SWITCH_AP:
		pr_debug("%s: IOCTL_USB_SWITCH_AP\n", __func__);
		usb_switch(IOCTL_USB_SWITCH_AP);
		break;
	default:
		pr_warning("%s: No supporting cmd\n", __func__);
		return -EINVAL;
	}

	pr_debug("android_debug_iocontrol: --\n");
	return 0;
}

static int android_debug_read(struct file *filp, char __user *buf,
			size_t count, loff_t *ppos)
{
	return 0;
}

static int android_debug_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *ppos)
{
	char arg[32] = {0};
	int ret = 0;

	ret = copy_from_user(arg, buf, sizeof(arg)-1);
	pr_debug("%s:arg=%s\n", __func__, arg);
	if (!strncmp(arg, "usb", strlen("usb"))) {
		pr_debug("%s:tail switch to ap usb\n", __func__);
		switch_tail_to_ap_usb();
	} else if (!strncmp(arg, "uart", strlen("uart"))) {
		pr_debug("%s:tail switch to ap uart\n", __func__);
		switch_tail_to_ap_uart();
	}
	return 0;
}

static loff_t android_debug_seek(struct file *filp, loff_t offset, int orig)
{
	return 0;
}

module_init(android_debug_init);
module_exit(android_debug_deinit);
MODULE_AUTHOR("Huang Jiefeng");
MODULE_LICENSE("GPL");
