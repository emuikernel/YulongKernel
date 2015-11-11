#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <mach/subsystem_restart.h>

#include <linux/suspend.h>

#include <mach/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>

#define   SET_GPIO_VALUE                  2

int test_value = -1;
static int test_suspend_wakeup = -1;

/* lock protects against unexport_gpio() being called while
 * sysfs files are active.
 */
static DEFINE_MUTEX(uart_switch_sysfs_lock);

static void get_gpio_from_console(u8 *buf, u8 *gpio, u8 *value)
{
	u8 data = 0;
	u8 i = 0;
	u8 *source = NULL;

	source = buf;
	if (NULL == source || NULL == gpio || NULL == value)
		return;
	/* Lookfor first char */
	while (*source == ' ' && (*source))
		source++;
	/* Ignore contrl char*/
	while (source[i] != ' ' && (*source))
		source++;

	while (*source == ' ' && (*source))
		source++;

	/* 取第一个数：GPIO */
	while (source[i] != ' ' && (*source) != '\n' && (*source)) {
		data = (data * 10) + (*source - '0');
		source++;
	}
	*gpio = data;
	printk(KERN_DEBUG "%s: gpio = %d\n", __func__, data);
	data = 0;

	/* 取第二个数: 写进GPIO寄存器的值 */
	while (*source == ' ' && (*source))
		source++;

	while ((*source) != '\n' && (*source)) {
		data = (data * 10) + (*source - '0');
		source++;
	}
	*value = data;
	printk(KERN_DEBUG "%s: data = %d\n", __func__, data);
	return;
}

static int get_data_from_console(u8 *buf, bool control_character)
{
	u8 data = 0;
	u8 i = 0;
	u8 *source = NULL;

	source = buf;
	if (NULL == source)
		return data;

	/* Lookfor first char */
	while (*source == ' ' && (*source))
		source++;

	if (control_character) {
		/* Ignore contrl char */
		while (source[i] != ' ' && (*source))
			source++;

		while (*source == ' ' && (*source))
			source++;
	}
	while (source[i] != '\n' && source[i]) {
		data = (data * 10) + (*source - '0');
		source++;
	}
	printk(KERN_DEBUG "%s: data = %d\n", __func__, data);
	return data;
}


static ssize_t uart_switch_set(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t status = 0;
	u8 value;

	mutex_lock(&uart_switch_sysfs_lock);
	value = get_data_from_console((u8 *)buf, 0);

	status = size;

	mutex_unlock(&uart_switch_sysfs_lock);
	return status;
}

static ssize_t uart_switch_get(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	printk(KERN_DEBUG "%s\n", __func__);
	return 0;
}

static ssize_t gpio_ctrl_set(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	u8 gpio = 0;
	u8 data = 0;
	ssize_t status = 0;

	mutex_lock(&uart_switch_sysfs_lock);

	printk(KERN_DEBUG "%s:SET_GPIO_VALUE\n", __func__);
	get_gpio_from_console((u8 *)buf, &gpio, &data);
	gpio_direction_output(gpio, data);

	status = size;

	mutex_unlock(&uart_switch_sysfs_lock);
	return status;
}

static ssize_t gpio_ctrl_get(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	printk(KERN_DEBUG "%s\n", __func__);
	return 0;
}

static DEFINE_SPINLOCK(test_lock);

/*added by shchj beginning 20130302*/
struct test_null_point {
	int a;
};
static struct test_null_point *null_point;
/*added by shchj ending 20130302*/

/* added for test omap clock */
static ssize_t test_value_set(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t status = 0;
	struct task_struct *tsk;
	unsigned long flags;

	printk(KERN_DEBUG "%s: +-\r\n", __func__);
	mutex_lock(&uart_switch_sysfs_lock);

	test_value = get_data_from_console((u8 *)buf, 0);

	status = size;

	mutex_unlock(&uart_switch_sysfs_lock);
	if (test_value == 201) {
		printk(KERN_INFO "Stack trace dump:\n");
		for_each_process(tsk) {
			printk(KERN_INFO "\nPID: %d, Name: %s\n",
				tsk->pid, tsk->comm);
			show_stack(tsk, NULL);
		}
	} else if (test_value == 202) {
		panic("fengchunsong test!");
	} else if (test_value == 203) {
		spin_lock_irqsave(&test_lock, flags);
		while (1);
		spin_unlock_irqrestore(&test_lock, flags);
	} else if (test_value == 204) {
		subsystem_restart("lpass");
	} else if (test_value == 205) {
		printk(KERN_INFO "regulatorstatusprint_yl ++\n");
		regulatorstatusprint_yl(1);
	} else if (test_value == 206) {
		printk(KERN_INFO "suspend gpiolib_show_yl ++\n");
		gpiolib_show_yl(1);
	} else if (test_value == 207) {
		printk(KERN_INFO "suspend clock_show_yl ++\n");
		clock_print_yl(1);
	} else if (test_value == 208) {
		printk(KERN_ERR "null exepsion\n");
		printk(KERN_ERR "null_point->a = %d\n", null_point->a);
	}

	return status;
}

static ssize_t test_value_get(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	printk(KERN_DEBUG "%s: %d\n", __func__, test_value);
	return 0;
}

static ssize_t set_suspend_wakeup(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t status = 0;

	printk(KERN_DEBUG "%s: +-\n", __func__);
	mutex_lock(&uart_switch_sysfs_lock);

	test_suspend_wakeup = get_data_from_console((u8 *)buf, 0);

	status = size;

	mutex_unlock(&uart_switch_sysfs_lock);
	return status;
}

static ssize_t get_suspend_wakeup(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	printk(KERN_DEBUG "%s: %d\n", __func__, test_suspend_wakeup);
	return 0;
}

static DEVICE_ATTR(uart_switch, 0774, uart_switch_get, uart_switch_set);
static DEVICE_ATTR(gpio_ctrl, 0774, gpio_ctrl_get, gpio_ctrl_set);
static DEVICE_ATTR(test_value, 0774, test_value_get, test_value_set);
static DEVICE_ATTR(suspend_wakeup, 0774,
			get_suspend_wakeup, set_suspend_wakeup);

static const struct attribute *uart_switch_attrs[] = {
	&dev_attr_uart_switch.attr,
	&dev_attr_gpio_ctrl.attr,
	&dev_attr_test_value.attr,
	&dev_attr_suspend_wakeup.attr,
	NULL,
};

static const struct attribute_group uart_switch_attr_group = {
	.attrs = (struct attribute **) uart_switch_attrs,
};

static struct class_attribute uart_class_attrs[] = {
	__ATTR_NULL,
};

static struct class uart_switch_class = {
	.name =		"uart_switch",
	.owner =	THIS_MODULE,

	.class_attrs =	uart_class_attrs,
};

static int uart_switch_export(void)
{
	int	status;
	struct device	*dev;

	mutex_lock(&uart_switch_sysfs_lock);
	dev = device_create(&uart_switch_class, NULL,
			MKDEV(133, 0), NULL, "uartswitchdev");
	if (dev) {
		status = sysfs_create_group(&dev->kobj,
					&uart_switch_attr_group);
	} else {
		printk(KERN_ERR"uart switch sysfs_create_group fail\r\n");
		status = -ENODEV;
	}
	mutex_unlock(&uart_switch_sysfs_lock);

	return status;
}

static void uart_switch_unexport(void)
{
	return;
}

static void uart_switch_remove(void)
{
	uart_switch_unexport();
	return;
}
EXPORT_SYMBOL_GPL(uart_switch_remove);

static int __init uart_switch_sysfs_init(void)
{
	int status;

	status = class_register(&uart_switch_class);
	if (status < 0) {
		printk(KERN_ERR "uart switch class_register fail\n");
		return status;
	}
	status = uart_switch_export();
	return status;
}
arch_initcall(uart_switch_sysfs_init);
