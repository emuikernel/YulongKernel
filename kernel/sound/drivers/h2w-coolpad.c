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

#include <linux/module.h>
#include <linux/sysdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/switch.h>
#include <linux/input.h>
#include <linux/debugfs.h>
#include <linux/gpio.h>
#include <mach/board.h>
#include <mach/vreg.h>
#include <asm/mach-types.h>
#include <linux/slab.h>
#include <sound/jack.h>
#include <sound/soc.h>

#define yl_debug(fmt, arg...) \
	printk(KERN_ERR "[HEADSET] %s " fmt "\n", __func__, ## arg)

enum {
	NO_DEVICE = 0,
	HEADSET   = 1,
};

struct h2w_info {
	struct switch_dev sdev;
	struct input_dev *input;

	atomic_t btn_state;
	int ignore_btn;

	unsigned int irq;
	unsigned int irq_btn;

	struct hrtimer timer;
	ktime_t debounce_time;

	struct hrtimer btn_timer;
	ktime_t btn_debounce_time;

	struct hrtimer btn_chkerr_timer;
	ktime_t btn_chkerr_debounce_time;
};
static struct h2w_info *hi;

static ssize_t h2w_print_name(struct switch_dev *sdev, char *buf)
{
	switch (switch_get_state(&hi->sdev)) {
	case NO_DEVICE:
		return sprintf(buf, "No Device\n");
	case HEADSET:
		return sprintf(buf, "Headset\n");
	}
	return -EINVAL;
}

static void button_pressed(void)
{
	yl_debug("");
	input_report_key(hi->input, KEY_MEDIA, 1);
	input_sync(hi->input);
}

static void button_released(void)
{
	yl_debug("");
	input_report_key(hi->input, KEY_MEDIA, 0);
	input_sync(hi->input);
}

static void insert_headset(void)
{
	yl_debug("");
	switch_set_state(&hi->sdev, HEADSET);
}

static void remove_headset(void)
{

	yl_debug("");
	switch_set_state(&hi->sdev, NO_DEVICE);
}

void simulate_hs_report(struct snd_soc_jack *jack, int status, int mask)
{
	struct snd_soc_codec *codec;
	int oldstatus;

	if (!jack)
		return;
	pr_info("%s: id=[%s]; name=[%s]\n",
		__func__, jack->jack->id, jack->jack->name);
	codec = jack->codec;
	/* delet by youjin to avaid deadlock 2012-11-19
	mutex_lock(&codec->mutex);
	*/

	oldstatus = jack->status;
	jack->status &= ~mask;
	jack->status |= status & mask;
	if (mask && (jack->status == oldstatus))
		return;

	if (!strncmp("Headset Jack", jack->jack->id, sizeof("Headset Jack"))) {
		pr_debug("[%s] oldstatus:%d, newstatus:%d\n",
			jack->jack->id, oldstatus, jack->status);
		if (jack->status)
			insert_headset();
		else
			remove_headset();
	} else if (!strncmp("Button Jack",
			jack->jack->id, sizeof("Button Jack"))) {
		pr_debug("[%s] oldstatus:%d, newstatus:%d\n",
			jack->jack->id, oldstatus, jack->status);
		if (jack->status)
			button_pressed();
		else
			button_released();
	}
/*delet by youjin to avaid deadlock 2012-11-19
out:
	mutex_unlock(&codec->mutex);
*/
}
EXPORT_SYMBOL_GPL(simulate_hs_report);

static int h2w_probe(struct platform_device *pdev)
{
	int ret = 0;
	pr_debug("Registering H2W (headset) driver\n");
	hi = kzalloc(sizeof(struct h2w_info), GFP_KERNEL);
	if (!hi)
		return -ENOMEM;
	hi->ignore_btn = 0;
	hi->sdev.name = "h2w";
	hi->sdev.print_name = h2w_print_name;
	/* register a switch device */
	ret = switch_dev_register(&hi->sdev);
	if (ret < 0)
		goto err_switch_dev_register;
	/* register a input */
	hi->input = input_allocate_device();
	if (!hi->input) {
		ret = -ENOMEM;
		goto err_request_input_dev;
	}

	hi->input->name = "8930_handset";
	hi->input->evbit[0] = BIT_MASK(EV_KEY);
	hi->input->keybit[BIT_WORD(KEY_MEDIA)] = BIT_MASK(KEY_MEDIA);
	ret = input_register_device(hi->input);
	if (ret < 0)
		goto err_register_input_dev;

	return 0;

err_register_input_dev:
	input_free_device(hi->input);
err_request_input_dev:

err_switch_dev_register:
	printk(KERN_ERR "Failed to register H2W (headset) driver\n");
	kfree(hi);
	return ret;
}

static int h2w_remove(struct platform_device *pdev)
{
	/* yl_debug(""); */
	if (switch_get_state(&hi->sdev))
		remove_headset();
	input_unregister_device(hi->input);
	switch_dev_unregister(&hi->sdev);
	kfree(hi);

	return 0;
}

static struct platform_device h2w_device = {
	.name		= "h2w-coolpad",
};

static struct platform_driver h2w_driver = {
	.probe		= h2w_probe,
	.remove		= h2w_remove,
	.driver		= {
		.name		= "h2w-coolpad",
		.owner		= THIS_MODULE,
	},
};

static int __init h2w_init(void)
{
	int ret = 0;
	ret = platform_driver_register(&h2w_driver);
	if (ret)
		return ret;
	return platform_device_register(&h2w_device);
}

static void __exit h2w_exit(void)
{
	platform_device_unregister(&h2w_device);
	platform_driver_unregister(&h2w_driver);
}
late_initcall(h2w_init);
module_exit(h2w_exit);

MODULE_AUTHOR("youjin <youjin@yulong.com>");
MODULE_DESCRIPTION("simulate yulong to user space");
MODULE_LICENSE("GPL");
