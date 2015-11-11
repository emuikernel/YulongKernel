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
#include <linux/device.h>
#include <linux/kdev_t.h>
#include "touchscreen.h"

touchscreen_ops_tpye *touchscreen_ops[2];

#define TOUCH_IN_ACTIVE(num) (touchscreen_ops[num] && touchscreen_ops[num]->active && touchscreen_ops[num]->active())

static DEFINE_MUTEX(touchscreen_mutex);

int touchscreen_set_ops(touchscreen_ops_tpye *ops)
{
	if (ops == NULL || ops->touch_id > 1) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:ops error!\n");
		return -EBUSY;
	}

	mutex_lock(&touchscreen_mutex);
	if (touchscreen_ops[ops->touch_id] != NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:ops has been used!\n");
		mutex_unlock(&touchscreen_mutex);
		return -EBUSY;
	}
	touchscreen_ops[ops->touch_id] = ops;
	mutex_unlock(&touchscreen_mutex);

	pr_debug("BJ_BSP_Driver:CP_Touchscreen:ops add success!\n");
	return 0;
}

static ssize_t  touchscreen_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0))
		ret = touchscreen_ops[0]->touch_type;
	else if (TOUCH_IN_ACTIVE(1))
		ret = touchscreen_ops[1]->touch_type;
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%d\n", ret);

}

static ssize_t  touchscreen_active_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret = 0;
	int ret1 = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (touchscreen_ops[0] && touchscreen_ops[0]->active)
		ret = touchscreen_ops[0]->active();

	if (touchscreen_ops[1] && touchscreen_ops[1]->active)
		ret1 = touchscreen_ops[1]->active();

	mutex_unlock(&touchscreen_mutex);

	pr_debug("BJ_BSP_Driver:CP_Touchscreen:%d,%d in %s\n",
			ret, ret1, __FUNCTION__);
	return sprintf(buf, "%d,%d\n", ret, ret1);
}

static ssize_t touchscreen_firmware_update_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->firmware_need_update)
			ret = touchscreen_ops[0]->firmware_need_update();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->firmware_need_update)
			ret = touchscreen_ops[1]->firmware_need_update();
	}
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%d\n", ret);
}

static ssize_t  touchscreen_firmware_update_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "update", count-1)) {
		pr_info("BJ_BSP_Driver:CP_Touchscreen:string is %s,count=%d not update!\n", buf, count);
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->firmware_need_update && touchscreen_ops[0]->firmware_need_update() && touchscreen_ops[0]->firmware_do_update) {
			ret = touchscreen_ops[0]->firmware_do_update();
		}
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->firmware_need_update && touchscreen_ops[1]->firmware_need_update() && touchscreen_ops[1]->firmware_do_update) {
			ret = touchscreen_ops[1]->firmware_do_update();
		}
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;
}

static ssize_t touchscreen_calibrate_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->need_calibrate)
			ret = touchscreen_ops[0]->need_calibrate();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->need_calibrate)
			ret = touchscreen_ops[1]->need_calibrate();
	}
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%d\n", ret);
}

static ssize_t  touchscreen_calibrate_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "calibrate", count-1)) {
		pr_info("BJ_BSP_Driver:CP_Touchscreen:string is %s,not calibrate!\n", buf);
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->calibrate)
			ret = touchscreen_ops[0]->calibrate();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->calibrate)
			ret = touchscreen_ops[1]->calibrate();
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;

}

static ssize_t  touchscreen_firmware_version_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char version[32] = {0};

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->get_firmware_version)
			touchscreen_ops[0]->get_firmware_version(version);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->get_firmware_version)
			touchscreen_ops[1]->get_firmware_version(version);
	}
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%s\n", version);
}

static ssize_t  touchscreen_reset_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "reset", count-1)) {
		pr_info("BJ_BSP_Driver:CP_Touchscreen:string is %s,not reset!\n", buf);
		return -EINVAL;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->reset_touchscreen)
			ret = touchscreen_ops[0]->reset_touchscreen();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->reset_touchscreen)
			ret = touchscreen_ops[1]->reset_touchscreen();
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;

}

static ssize_t  touchscreen_mode_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->get_mode)
			ret = touchscreen_ops[0]->get_mode();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->get_mode)
			ret = touchscreen_ops[1]->get_mode();
	}
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%d\n", ret);
}

static ssize_t  touchscreen_mode_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	int mode = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "normal", count-1) == 0)
		mode = MODE_NORMAL;
	else if (strncmp(buf, "handwrite", count-1) == 0)
		mode = MODE_HANDWRITE;
	else {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:Don't support %s mode!\n",
				buf);
		return -EINVAL;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->set_mode)
			ret = touchscreen_ops[0]->set_mode(mode);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->set_mode)
			ret = touchscreen_ops[1]->set_mode(mode);
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;
}

static ssize_t  touchscreen_oreitation_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->get_oreitation)
			ret = touchscreen_ops[0]->get_oreitation();
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->get_oreitation)
			ret = touchscreen_ops[1]->get_oreitation();
	}
	mutex_unlock(&touchscreen_mutex);

	return sprintf(buf, "%d\n", ret);
}

static ssize_t  touchscreen_oreitation_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	int oreitation = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "oreitation", count-2)) {
		pr_info("BJ_BSP_Driver:CP_Touchscreen:string is %s,not oreitation\n", buf);
		return -EINVAL;
	}

	oreitation = buf[count-2] - '0';
	pr_debug("BJ_BSP_Driver:CP_Touchscreen:oreitation=%d", oreitation);
	if (oreitation < 0 || oreitation > 3) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:oreitation[%d] is invalid\n", oreitation);
		return -EINVAL;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->set_oreitation)
			ret = touchscreen_ops[0]->set_oreitation(oreitation);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->set_oreitation)
			ret = touchscreen_ops[1]->set_oreitation(oreitation);
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;

}

static ssize_t  touchscreen_regs_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->read_regs)
			ret = touchscreen_ops[0]->read_regs(buf);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->read_regs)
			ret = touchscreen_ops[1]->read_regs(buf);
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;
}

static ssize_t  touchscreen_regs_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->write_regs)
			ret = touchscreen_ops[0]->write_regs(buf);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->write_regs)
			ret = touchscreen_ops[1]->write_regs(buf);
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;
}

static ssize_t  touchscreen_debug_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	int on = 0;

	if (buf == NULL) {
		pr_err("BJ_BSP_Driver:CP_Touchscreen:buf is NULL!\n");
		return -ENOMEM;
	}

	if (strncmp(buf, "on", count-1) == 0)
		on = 1;

	mutex_lock(&touchscreen_mutex);
	if (TOUCH_IN_ACTIVE(0)) {
		if (touchscreen_ops[0]->debug)
			ret = touchscreen_ops[0]->debug(on);
	} else if (TOUCH_IN_ACTIVE(1)) {
		if (touchscreen_ops[1]->debug)
			ret = touchscreen_ops[1]->debug(on);
	}
	mutex_unlock(&touchscreen_mutex);

	return ret;
}

static DEVICE_ATTR(type, 0444, touchscreen_type_show, NULL);
static DEVICE_ATTR(active, 0444, touchscreen_active_show, NULL);
/*randy change for firmware update 20121016*/
static DEVICE_ATTR(firmware_update, 0660,
	touchscreen_firmware_update_show, touchscreen_firmware_update_store);
static DEVICE_ATTR(calibrate, 0644,
	touchscreen_calibrate_show, touchscreen_calibrate_store);
static DEVICE_ATTR(firmware_version, 0444,
	touchscreen_firmware_version_show, NULL);
static DEVICE_ATTR(reset, 0200, NULL, touchscreen_reset_store);
static DEVICE_ATTR(mode, 0644, touchscreen_mode_show, touchscreen_mode_store);
static DEVICE_ATTR(oreitation, 0644,
	touchscreen_oreitation_show, touchscreen_oreitation_store);
static DEVICE_ATTR(regs, 0644, touchscreen_regs_show, touchscreen_regs_store);
static DEVICE_ATTR(debug, 0200, NULL, touchscreen_debug_store);

static const struct attribute *touchscreen_attrs[] = {
	&dev_attr_type.attr,
	&dev_attr_active.attr,
	&dev_attr_firmware_update.attr,
	&dev_attr_calibrate.attr,
	&dev_attr_firmware_version.attr,
	&dev_attr_reset.attr,
	&dev_attr_mode.attr,
	&dev_attr_oreitation.attr,
	&dev_attr_regs.attr,
	&dev_attr_debug.attr,
	NULL,
};

static const struct attribute_group touchscreen_attr_group = {
	.attrs = (struct attribute **) touchscreen_attrs,
};

static ssize_t export_store(struct class *class, struct class_attribute *attr,
		const char *buf, size_t len)
{
	return 1;
}

static ssize_t unexport_store(struct class *class, struct class_attribute *attr,
		const char *buf, size_t len)
{
	return 1;
}

static struct class_attribute uart_class_attrs[] = {
	__ATTR(export, 0200, NULL, export_store),
	__ATTR(unexport, 0200, NULL, unexport_store),
	__ATTR_NULL,
};

static struct class touchscreen_class = {
	.name =		"touchscreen",
	.owner =	THIS_MODULE,

	.class_attrs =	uart_class_attrs,
};

static int touchscreen_export(void)
{
	int	status = 0;
	struct device	*dev = NULL;

	dev = device_create(&touchscreen_class, NULL, MKDEV(0, 0),
			NULL, "touchscreen_dev");
	if (dev) {
		status = sysfs_create_group(&dev->kobj,
				&touchscreen_attr_group);
	} else {
		printk(KERN_ERR "BJ_BSP_Driver:CP_Touchscreen:uart switch sysfs_create_group fail\r\n");
		status = -ENODEV;
	}

	return status;
}

static int __init touchscreen_sysfs_init(void)
{
	int	status = 0;
	touchscreen_ops[0] = NULL;
	touchscreen_ops[1] = NULL;
	status = class_register(&touchscreen_class);
	if (status < 0) {
		printk(KERN_ERR"BJ_BSP_Driver:CP_Touchscreen:uart switch class_register fail\r\n");
		return status;
	}

	status = touchscreen_export();

	return status;
}

arch_initcall(touchscreen_sysfs_init);

