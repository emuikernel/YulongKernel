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

//ALERT:relocate pn544.c under .\kernel\drivers\misc

/*
* Makefile//TODO:Here is makefile reference
* obj-$(CONFIG_PN544)+= pn544.o
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
#include <linux/nfc/pn544.h>
#include <mach/msm_xo.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define MAX_BUFFER_SIZE	512

struct pn544_dev {
	wait_queue_head_t	read_wq;
	struct mutex		read_mutex;
	struct i2c_client	*client;
	struct miscdevice	pn544_device;
	unsigned int		ven_gpio;
	unsigned int		firm_gpio;
	unsigned int		irq_gpio;
	bool			irq_enabled;
	struct msm_xo_voter	*xo_handle_d1;
	spinlock_t			irq_enabled_lock;

};

static void pn544_disable_irq(struct pn544_dev *pn544_dev)
{
	unsigned long flags;

	spin_lock_irqsave(&pn544_dev->irq_enabled_lock, flags);
	if (pn544_dev->irq_enabled) {
		disable_irq_nosync(pn544_dev->client->irq);
		pn544_dev->irq_enabled = false;
	}
	spin_unlock_irqrestore(&pn544_dev->irq_enabled_lock, flags);
}

static irqreturn_t pn544_dev_irq_handler(int irq, void *dev_id)
{
	struct pn544_dev *pn544_dev = dev_id;

	pr_debug("%s\n", __func__);
	if (!gpio_get_value(pn544_dev->irq_gpio))
		return IRQ_HANDLED;

	pn544_disable_irq(pn544_dev);

	/* Wake up waiting readers */
	wake_up(&pn544_dev->read_wq);

	return IRQ_HANDLED;
}

static ssize_t pn544_dev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev = filp->private_data;
	char tmp[MAX_BUFFER_SIZE];
	int ret, i;

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	pr_debug("%s : reading %zu bytes.\n", __func__, count);

	mutex_lock(&pn544_dev->read_mutex);

	if (!gpio_get_value(pn544_dev->irq_gpio)) {
		if (filp->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto fail;
		}

		pn544_dev->irq_enabled = true;
		enable_irq(pn544_dev->client->irq);
		ret = wait_event_interruptible(pn544_dev->read_wq,
				gpio_get_value(pn544_dev->irq_gpio));

		pn544_disable_irq(pn544_dev);

		if (ret)
			goto fail;
	}

	/* Read data */
	ret = i2c_master_recv(pn544_dev->client, tmp, count);
	mutex_unlock(&pn544_dev->read_mutex);

	if (ret < 0) {
		pr_err("%s: i2c_master_recv returned %d\n", __func__, ret);
		return ret;
	}
	if (ret > count) {
		pr_err("%s: received too many bytes from i2c (%d)\n",
			__func__, ret);
		return -EIO;
	}
	if (copy_to_user(buf, tmp, ret)) {
		pr_warning("%s : failed to copy to user space\n", __func__);
		return -EFAULT;
	}

	pr_debug("IFD->PC:");
	for (i = 0; i < ret; i++)
		pr_debug(" %02X", tmp[i]);
	pr_debug("\n");

	return ret;

fail:
	mutex_unlock(&pn544_dev->read_mutex);
	return ret;
}

static ssize_t pn544_dev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offset)
{
	struct pn544_dev *pn544_dev;
	char tmp[MAX_BUFFER_SIZE];
	int ret, i;

	pr_debug("%s\n", __func__);
	pn544_dev = filp->private_data;

	if (count > MAX_BUFFER_SIZE)
		count = MAX_BUFFER_SIZE;

	if (copy_from_user(tmp, buf, count)) {
		pr_err("%s : failed to copy from user space\n", __func__);
		return -EFAULT;
	}

	pr_debug("%s : writing %zu bytes.\n", __func__, count);
	/* Write data */
	ret = i2c_master_send(pn544_dev->client, tmp, count);
	if (ret != count) {
		pr_err("%s : i2c_master_send returned %d\n", __func__, ret);
		ret = -EIO;
	}
	pr_debug("PC->IFD:");
	for (i = 0; i < count; i++)
		pr_debug(" %02X", tmp[i]);
	pr_debug("\n");

	return ret;
}

static int pn544_dev_open(struct inode *inode, struct file *filp)
{

	struct pn544_dev *pn544_dev = container_of(filp->private_data,
			struct pn544_dev, pn544_device);

	filp->private_data = pn544_dev;
	pr_debug("%s : %d,%d\n", __func__, imajor(inode), iminor(inode));

	return 0;
}

static long pn544_dev_ioctl(struct file *filp, unsigned int cmd,
			unsigned long arg)
{
	struct pn544_dev *pn544_dev = filp->private_data;

	switch (cmd) {
	case PN544_SET_PWR:
		if (arg == 2) {
			/* power on with firmware download (requires hw reset)
			 */
			pr_debug("%s power on with firmware\n", __func__);
			gpio_set_value(pn544_dev->ven_gpio, 1);
			gpio_set_value(pn544_dev->firm_gpio, 1);
			msleep(10);
			gpio_set_value(pn544_dev->ven_gpio, 0);
			msleep(50);
			gpio_set_value(pn544_dev->ven_gpio, 1);
			msleep(10);
		} else if (arg == 1) {
			/* power on */
			pr_debug("%s power on ven gpio is %d\n",
				__func__, pn544_dev->ven_gpio);
			gpio_set_value(pn544_dev->firm_gpio, 0);
			gpio_set_value(pn544_dev->ven_gpio, 1);
			msm_xo_mode_vote(pn544_dev->xo_handle_d1,
						 MSM_XO_MODE_ON);
			msleep(10);
		} else  if (arg == 0) {
			/* power off */
			pr_debug("%s power off ven gpio is %d\n",
				__func__, pn544_dev->ven_gpio);
			gpio_set_value(pn544_dev->firm_gpio, 0);
			gpio_set_value(pn544_dev->ven_gpio, 0);
			msm_xo_mode_vote(pn544_dev->xo_handle_d1,
						 MSM_XO_MODE_OFF);
			msleep(50);
		} else {
			pr_err("%s bad arg\n", __func__);
			return -EINVAL;
		}
		break;
	default:
		pr_err("%s bad ioctl %u\n", __func__, cmd);
		return -EINVAL;
	}

	return 0;
}

static ssize_t is_pn544_exist(struct i2c_client *client,
				struct pn544_dev *pn544_dev)
{
	int ret;
	unsigned char send_reset[] = {0x20, 0x00, 0x01, 0x01};/*RSET Frame*/

	gpio_set_value(pn544_dev->firm_gpio, 0);
	gpio_set_value(pn544_dev->ven_gpio, 1);
	msleep(20);

	gpio_set_value(pn544_dev->firm_gpio, 0);
	gpio_set_value(pn544_dev->ven_gpio, 0);
	msleep(50);

	gpio_set_value(pn544_dev->firm_gpio, 0);
	gpio_set_value(pn544_dev->ven_gpio, 1);
	msleep(20);

	ret = i2c_master_send(client, send_reset, sizeof(send_reset));

	if (ret != sizeof(send_reset)) {
		pr_err("%s : i2c_master_send returned %d\n", __func__, ret);
		return -EIO;
	}

	return 0;
}
static int pn544_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct pn544_dev *pn544_dev = i2c_get_clientdata(client);

	return msm_xo_mode_vote(pn544_dev->xo_handle_d1, MSM_XO_MODE_OFF);
}

static int pn544_resume(struct i2c_client *client)
{
	struct pn544_dev *pn544_dev = i2c_get_clientdata(client);
	return msm_xo_mode_vote(pn544_dev->xo_handle_d1, MSM_XO_MODE_ON);
}


static const struct file_operations pn544_dev_fops = {
	.owner	= THIS_MODULE,
	.llseek	= no_llseek,
	.read	= pn544_dev_read,
	.write	= pn544_dev_write,
	.open	= pn544_dev_open,
	.unlocked_ioctl = pn544_dev_ioctl,
};

static int pn544_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret;
	struct pn544_i2c_platform_data *platform_data;
	struct pn544_dev *pn544_dev;

	platform_data = client->dev.platform_data;
	pr_err("%s : nfc pn544 probe\n", __func__);

	if (platform_data == NULL) {
		pr_err("%s : nfc probe fail\n", __func__);
		return  -ENODEV;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s : need I2C_FUNC_I2C\n", __func__);
		return  -ENODEV;
	}

	/* IRQ */
	ret = gpio_request(platform_data->irq_gpio, "nfc_int");
	if (ret) {
		pr_err("gpio_nfc_int request error\n");
		return  -ENODEV;
	}

	ret = gpio_direction_input(platform_data->irq_gpio);
	if (ret) {
		pr_err("irq gpio gpio_direction_input error\n");
		return  -ENODEV;
	}

	/* VEN */
	ret = gpio_request(platform_data->ven_gpio, "nfc_ven");
	if (ret) {
		pr_err("gpio_nfc_ven request error\n");
		return  -ENODEV;
	}
	ret = gpio_direction_output(platform_data->ven_gpio, 1);
	if (ret) {
		pr_err("ven gpio gpio_direction_output error\n");
		return  -ENODEV;
	}

	/* FIRM */
	ret = gpio_request(platform_data->firm_gpio, "nfc_firm");
	if (ret) {
		pr_err("gpio_nfc_firm request error\n");
		return  -ENODEV;
	}
	ret = gpio_direction_output(platform_data->firm_gpio, 1);
	if (ret) {
		pr_err("firm gpio gpio_direction_output error\n");
		return  -ENODEV;
	}

	pn544_dev = kzalloc(sizeof(*pn544_dev), GFP_KERNEL);
	if (pn544_dev == NULL) {
		dev_err(&client->dev,
				"failed to allocate memory for module data\n");
		ret = -ENOMEM;
		goto err_exit;
	}

	pn544_dev->irq_gpio = platform_data->irq_gpio;
	pn544_dev->ven_gpio = platform_data->ven_gpio;
	pn544_dev->firm_gpio = platform_data->firm_gpio;
	pn544_dev->client  = client;

	ret = is_pn544_exist(client, pn544_dev);
	if (ret) {
		pr_err("NFC chipset is not existing.\n");
		ret = -ENODEV;
		goto err_exit;
	}

	pn544_dev->xo_handle_d1 = msm_xo_get(MSM_XO_TCXO_D1, "pn544");
	if (IS_ERR(pn544_dev->xo_handle_d1)) {
		ret = PTR_ERR(pn544_dev->xo_handle_d1);
		pr_err("%s: failed to get the handle for D1(%d)\n",
				__func__, ret);
		goto err_exit;
	}

	/* init mutex and queues */
	init_waitqueue_head(&pn544_dev->read_wq);
	mutex_init(&pn544_dev->read_mutex);
	spin_lock_init(&pn544_dev->irq_enabled_lock);

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
	pn544_dev->irq_enabled = true;

	ret = request_irq(client->irq, pn544_dev_irq_handler,
			IRQF_TRIGGER_HIGH, client->name, pn544_dev);
	if (ret) {
		dev_err(&client->dev, "request_irq failed\n");
		goto err_request_irq_failed;
	}

	pn544_disable_irq(pn544_dev);
	i2c_set_clientdata(client, pn544_dev);

	return 0;

err_request_irq_failed:
	misc_deregister(&pn544_dev->pn544_device);
err_misc_register:
	mutex_destroy(&pn544_dev->read_mutex);
	kfree(pn544_dev);
err_exit:
	gpio_free(pn544_dev->irq_gpio);
	gpio_free(pn544_dev->ven_gpio);
	gpio_free(pn544_dev->firm_gpio);
	return ret;
}

static int pn544_remove(struct i2c_client *client)
{
	struct pn544_dev *pn544_dev;

	pn544_dev = i2c_get_clientdata(client);
	free_irq(client->irq, pn544_dev);
	misc_deregister(&pn544_dev->pn544_device);
	mutex_destroy(&pn544_dev->read_mutex);
	gpio_free(pn544_dev->irq_gpio);
	gpio_free(pn544_dev->ven_gpio);
	gpio_free(pn544_dev->firm_gpio);
	kfree(pn544_dev);

	return 0;
}

static const struct i2c_device_id pn544_id[] = {
	{ "pn544", 0 },
	{ }
};

static struct i2c_driver pn544_driver = {
	.id_table	= pn544_id,
	.probe		= pn544_probe,
	.remove		= pn544_remove,
	.resume		= pn544_resume,
	.suspend	= pn544_suspend,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "pn544",
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
