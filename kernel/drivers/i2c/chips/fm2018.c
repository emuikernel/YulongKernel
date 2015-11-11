#include <asm/mach-types.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <asm/gpio.h>
#include <linux/jiffies.h>
#include <linux/times.h>
#include <linux/fm2018.h>
#include <linux/debugfs.h>

static struct fm2018_platform_data *pdata = NULL;

static struct i2c_client *this_client = NULL;

static int fm2018_hw_init(void)
{
	int rc = 0;

	gpio_request(pdata->gpio_fm2018_sda, "fm2018_sda");
	gpio_request(pdata->gpio_fm2018_scl, "fm2018_scl");

	rc = gpio_request(pdata->gpio_fm2018_rst, "fm2018_rst");
	if (rc < 0) {
		printk("%s: gpio request reset pin failed\n", __func__);
		goto err_request_rst_failed;
	}
	gpio_direction_output(pdata->gpio_fm2018_rst, 1);

	rc = gpio_request(pdata->gpio_fm2018_irq_ana, "fm2018_irq_ana");
	if (rc < 0) {
		printk("%s: gpio request irq pin failed\n", __func__);
		goto err_request_irq_failed;
	}
	gpio_direction_output(pdata->gpio_fm2018_irq_ana, 0);

	return 0;

err_request_irq_failed:
	gpio_free(pdata->gpio_fm2018_rst);
err_request_rst_failed:

	return rc;
}

static int fm2018_opened = 0;

static int fm2018_open(struct inode *inode, struct file *file)
{
	int rc = 0;

	if (fm2018_opened) {
		printk("%s: busy\n", __func__);
		rc = -EBUSY;
		goto done;
	}

	fm2018_opened = 1;

done:
	return rc;
}

static int fm2018_release(struct inode *inode, struct file *file)
{
	fm2018_opened = 0;

	return 0;
}

static int fm2018_on = 0;

static int fm2018_power_on(void)
{
	int rc = 0;

	rc = pdata->fm2018_power(1);
	if (rc) {
		printk("%s: power on failed\n", __func__);
		goto power_1_err;
	}
	mdelay(30);
	gpio_set_value(pdata->gpio_fm2018_rst, 1);
	mdelay(30);

	fm2018_on = 1;
power_1_err:
	return rc;
}

static int fm2018_power_off(void)
{
	int rc = 0;

	gpio_set_value(pdata->gpio_fm2018_rst, 0);

	rc = pdata->fm2018_power(0);
	if (rc) {
		printk("%s: power off failed\n", __func__);
		return rc;
	}

	gpio_set_value(pdata->gpio_fm2018_irq_ana, 0);

	fm2018_on = 0;

	return rc;
}

#define CONFIG_DEBUG_FS 1

#ifdef CONFIG_DEBUG_FS

enum fm2018_state{
	FM2018_IDLE,
	FM2018_BYPASS,
	FM2018_CDMA_HANDSET,
	FM2018_CDMA_HANDFREE,
	FM2018_LOCAL_RECORD,
	FM2018_GSM_HANDSET,
	FM2018_GSM_HANDFREE,
};

static enum fm2018_state cur_state = FM2018_IDLE;

#endif

#define RETRIES 3


static int fm2018_reg_read(uint16_t reg, uint16_t *value)
{
	unsigned char data[10];
	u16 read_value[2];
	struct i2c_msg msg[1];
	int err = 0;

	if (value == NULL) {
		printk("value is NULL in %s func\n", __FUNCTION__);
		return -ENODEV;
	}

	memset(data, 0, sizeof(data));
	memset(read_value, 0, sizeof(read_value));

	/* 1.写要读取的寄存器的地址 */
	msg->addr = this_client->addr;
	msg->flags = 0;
	msg->len = 5;
	msg->buf = data;

	data[0] = 0xfc;	// 发送同步信号
	data[1] = 0xf3;	// 发送高同步位0xf3
	data[2] = 0x37;	// 发送寄存器写操作指令0x37

	/* Write addr - high bytes goes out first */
	data[3] = (u8)(reg >> 8);
	data[4] = (u8)(reg & 0x00FF);

	err = i2c_transfer(this_client->adapter, msg, 1);
	if (err < 0) {
		printk(KERN_ERR "i2c_transfer error %i, location 1\n", err);
		return err;
	}

	/* 2.读取的寄存器的低8位数据 */
	msg->len = 4;
	msg->flags = 0;

	data[0] = 0xfc;	// 发送同步信号
	data[1] = 0xf3;	// 发送高同步位0xf3
	data[2] = 0x60;	// 发送寄存器读操作指令0x60
	data[3] = 0x25;	// 寄存器低字节地址0x25

	err = i2c_transfer(this_client->adapter, msg, 1);
	if (err < 0) {
		printk(KERN_ERR "i2c_transfer error %i, location 2\n", err);
		return err;
	}

	msg->len = 1;
	msg->flags = I2C_M_RD;
	err = i2c_transfer(this_client->adapter, msg, 1);
	if (err < 0) {
		printk(KERN_ERR "i2c_transfer error %i, location 3\n", err);
		return err;
	}
	read_value[0] = data[0];

	/* 3.读取的寄存器的高8位数据 */
	msg->len = 4;
	msg->flags = 0;

	data[0] = 0xfc;	// 发送同步信号
	data[1] = 0xf3;	// 发送高同步位0xf3
	data[2] = 0x60;	// 发送寄存器读操作指令0x60
	data[3] = 0x26;	// 寄存器高字节地址0x26

	err = i2c_transfer(this_client->adapter, msg, 1);
	if (err < 0) {
		printk(KERN_ERR "i2c_transfer error %i, location 4\n", err);
		return err;
	}

	msg->len = 1;
	msg->flags = I2C_M_RD;
	err = i2c_transfer(this_client->adapter, msg, 1);
	if (err < 0) {
		printk(KERN_ERR "i2c_transfer error %i, location 5\n", err);
		return err;
	}
	read_value[1] = data[0];

	*value = (read_value[1] << 8) | read_value[0];

	return err;
}

static int fm2018_reg_write(uint16_t addr, uint16_t data)
{
	int i = 0;
	uint8_t buf[7];
	struct i2c_msg msg[] = {
		{
			.addr = this_client->addr,
			.flags = 0,
			.len = 7,
			.buf = buf,
		}
	};

	memset(buf, 0, sizeof(buf));
	buf[0] = 0xFC;
	buf[1] = 0xF3;
	buf[2] = 0x3B;
	buf[3] = (uint8_t)(addr >> 8);
	buf[4] = (uint8_t)(addr & 0x00FF);
	buf[5] = (uint8_t)(data >> 8);
	buf[6] = (uint8_t)(data & 0x00FF);

	for(i = 0; i < RETRIES; ++i) {
		if (1 == i2c_transfer(this_client->adapter, msg, 1)) {
			printk(KERN_INFO "successed to write to reg 0x%x\n", addr);
			return 0;
		}
		mdelay(10);
	}
	printk("%s: reg write failed\n", __func__);
	return -EIO;
}
struct fm2018_reg_val{
	uint16_t RegAddr;
	uint16_t RegData;
};

static struct fm2018_reg_val fm2018_bypass_regs[] = {
	{0x1E41, 0x0101},
	{0x1E4A, 0x0009},
	{0x1E58, 0x0019},
	{0x1E60, 0x0000},
	{0x1E70, 0x05C0},
	{0x1E4C, 0x001D},
	{0x1E3A, 0x0000},
};

#define FM2018_REG_VAL_SIZE (sizeof(struct fm2018_reg_val))

#define FM2018_BYPASS_REGS_NUM (sizeof(fm2018_bypass_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_bypass(void)
{
	unsigned int i = 0;
	int rc = 0;
	uint16_t data = 0;

	if (!fm2018_on) {
		rc = fm2018_power_on();
		if (rc) {
			printk("%s: fm2018 power on error!\n", __func__);
			goto power_on_err;
		}
	}

	for(i = 0; i < FM2018_BYPASS_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_bypass_regs[i].RegAddr, fm2018_bypass_regs[i].RegData);
		if (rc) {
			printk("%s: reg write failed\n", __func__);
			goto reg_write_err;
		}
	}
	mdelay(110);

	gpio_direction_output(pdata->gpio_fm2018_irq_ana, 1);

	for(i = 0; i < (FM2018_BYPASS_REGS_NUM - 1); ++i) {
		rc = fm2018_reg_read(fm2018_bypass_regs[i].RegAddr, &data);
		if (rc < 0) {
			printk("%s: reg read error!\n", __func__);
			goto reg_read_err;
		}

		if (data != fm2018_bypass_regs[i].RegData) {
			printk("%s: the data written is wrong!\n", __func__);
			rc = -EINVAL;
			goto data_err;
		}
	}

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_BYPASS;
#endif

data_err:
reg_read_err:
reg_write_err:
power_on_err:
	return rc;
}

#if 0
static struct fm2018_reg_val fm2018_cdma_handset_regs[] = {
	{0x1E34, 0x00A9},	//confirmed by qiuhui, 2010-10-26
	{0x1E3D, 0x0100},
	{0x1E45, 0x00CF},
	{0x1E46, 0x0014},
	{0x1E47, 0x2800},
	{0x1E48, 0x0B00},
	{0x1E49, 0x0800},
	{0x1E4A, 0x0009},
	{0x1E4C, 0x001D},
	{0x1E4D, 0x0001},
	{0x1E52, 0x0013},
	{0x1E57, 0x7FFF},
	{0x1E58, 0x0019},
	{0x1E5C, 0x0001},
	{0x1E60, 0x0000},
	{0x1E70, 0x05C0},
	{0x1EA1, 0x2000},
	{0x1EB3, 0x1200},
	{0x1EB4, 0x1700},
	{0x1EB5, 0x1100},
	{0x1EB6, 0x0100},
	{0x1EBC, 0x5000},
	{0x1EBD, 0x1800},
	{0x1EBE, 0x2800},
	{0x1EBF, 0x7FFF},
	{0x1ED5, 0x4000},
	{0x1ED6, 0x4000},
	{0x1ED7, 0x4000},
	{0x1ED8, 0x3800},
	{0x1ED9, 0x2E00},
	{0x1EDA, 0x5400},
	{0x1F00, 0x2E00},
	{0x1F01, 0x3800},
	{0x1F0B, 0x0060},
	{0x1F0C, 0x0600},
	{0x1F0D, 0x1200},
	{0x1E3A, 0x0000},
	{0x1E3A, 0x0000},
	{0x1E3A, 0x0000},
};

#define FM2018_CDMA_HANDSET_REGS_NUM (sizeof(fm2018_cdma_handset_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_cdma_handset(void)
{
	unsigned int i = 0;
	int rc = 0;

	if (!fm2018_on) {
		pr_info("%s: fm2018 is off!", __func__);
		fm2018_power_on();
	}
	pr_info("%s: fm2018 is on!", __func__);

	for(i = 0; i < FM2018_CDMA_HANDSET_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_cdma_handset_regs[i].RegAddr, fm2018_cdma_handset_regs[i].RegData);
		if (rc < 0) {
			printk("%s: reg write failed\n", __func__);
			return rc;
		}
	}
	mdelay(30);

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_CDMA_HANDSET;
#endif

	return rc;
}

static struct fm2018_reg_val fm2018_cdma_handfree_regs[] = {
	{0x1E34, 0x0009},
	{0x1E3D, 0x0100},
	{0x1E41, 0x0101},
	{0x1E45, 0x030F},
	{0x1E46, 0x0011},
	{0x1E47, 0x2800},
	{0x1E48, 0x0800},
	{0x1E49, 0x0800},
	{0x1E4D, 0x0120},
	{0x1E58, 0x0019},
	{0x1E60, 0x0000},
	{0x1E63, 0x0001},
	{0x1E70, 0x05C0},
	{0x1E86, 0x0009},
	{0x1E87, 0x0004},
	{0x1E88, 0x6000},
	{0x1E89, 0x0100},
	{0x1E8B, 0x0280},
	{0x1E8C, 0x0020},
	{0x1E91, 0x0002},
	{0x1E92, 0x1800},
	{0x1E93, 0x1000},
	{0x1EC8, 0x4800},
	{0x1EF8, 0x2000},
	{0x1EF9, 0x0800},
	{0x1EFF, 0x4800},
	{0x1E3A, 0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A, 0x0000},  	 //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A, 0x0000},    //ÅäÖÃÍê³ÉÖžÁî
};

#define FM2018_CDMA_HANDFREE_REGS_NUM (sizeof(fm2018_cdma_handfree_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_cdma_handfree(void)
{
	unsigned int i = 0;
	int rc = 0;

	if (!fm2018_on) {
		fm2018_power_on();
	}

	for(i = 0; i < FM2018_CDMA_HANDFREE_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_cdma_handfree_regs[i].RegAddr, fm2018_cdma_handfree_regs[i].RegData);
		if (rc < 0) {
			printk("%s: reg write failed\n", __func__);
			return rc;
		}
	}
	mdelay(30);

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_CDMA_HANDFREE;
#endif

	return rc;
}

static struct fm2018_reg_val fm2018_local_record_regs[] = {
#if 0
	{0x1E34, 0x00BA},
	{0x1E3A, 0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3D,  0x0100},        //for main mic, confirmed 2010-07-05, by FAE chenyuejian
	{0x1E41,  0x0101},
	{0x1E45,  0x001E},
	{0x1E47,  0x4800},
	{0x1E48,  0x0600},
	{0x1E58,  0x0019},
	{0x1E60,  0x0000},
	{0x1E70,  0x05C0},
	{0x1EF8,  0x0200},
	{0x1EF9,  0x0100},
	{0x1EFF,  0x0100},
   	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
#else
//	{0x1E34,  0x000F},
	{0x1E34,  0x0007},
//	{0x1E3D,  0x0300},
	{0x1E3D,  0x0100},
	{0x1E41,  0x0101},
	{0x1E46,  0x0014},
	{0x1E47,  0x2800},
	{0x1E48,  0x0800},
	{0x1E49,  0x0800},
	{0x1E58,  0x0019},
	{0x1E5C,  0x0001},
	{0x1E60,  0x0000},
	{0x1E70,  0x05C0},
//	{0x1EDB,  0x0800},
	{0x1EDB,  0x2000},
	{0x1EF8,  0x0600},
	{0x1EF9,  0x0200},
	{0x1EFF,  0x1000},
	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
	{0x1E3A,  0x0000},    //ÅäÖÃÍê³ÉÖžÁî
#endif
};

#define FM2018_LOCAL_RECORD_REGS_NUM (sizeof(fm2018_local_record_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_local_record(void)
{
	unsigned int i = 0;
	int rc = 0;

	if (!fm2018_on) {
		fm2018_power_on();
	}

	for(i = 0; i < FM2018_LOCAL_RECORD_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_local_record_regs[i].RegAddr, fm2018_local_record_regs[i].RegData);
		if (rc < 0) {
			printk("%s: reg write failed\n", __func__);
			return rc;
		}
	}
	mdelay(30);

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_LOCAL_RECORD;
#endif

	return rc;
}

static struct fm2018_reg_val fm2018_gsm_handset_regs[] = {
	{0x1E34, 0x00A9},	//confirmed by qiuhui, 2010-10-26
	{0x1E3D, 0x0100},
	{0x1E45, 0x00CF},
	{0x1E46, 0x0014},
	{0x1E47, 0x2800},
	{0x1E48, 0x0B00},
	{0x1E49, 0x0800},
	{0x1E4A, 0x0009},
	{0x1E4C, 0x001D},
	{0x1E4D, 0x0001},
	{0x1E52, 0x0013},
	{0x1E57, 0x7FFF},
	{0x1E58, 0x0019},
	{0x1E5C, 0x0001},
	{0x1E60, 0x0000},
	{0x1E70, 0x05C0},
	{0x1EA1, 0x2000},
	{0x1EB3, 0x1200},
	{0x1EB4, 0x1700},
	{0x1EB5, 0x1100},
	{0x1EB6, 0x0100},
	{0x1EBC, 0x5000},
	{0x1EBD, 0x1800},
	{0x1EBE, 0x2800},
	{0x1EBF, 0x7FFF},
	{0x1ED5, 0x4000},
	{0x1ED6, 0x4000},
	{0x1ED7, 0x4000},
	{0x1ED8, 0x3800},
	{0x1ED9, 0x2E00},
	{0x1EDA, 0x5400},
	{0x1F00, 0x2E00},
	{0x1F01, 0x3800},
	{0x1F0B, 0x0060},
	{0x1F0C, 0x0600},
	{0x1F0D, 0x1200},
	{0x1E3A, 0x0000},
	{0x1E3A, 0x0000},
	{0x1E3A, 0x0000},
};

#define FM2018_GSM_HANDSET_REGS_NUM (sizeof(fm2018_gsm_handset_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_gsm_handset(void)
{
	unsigned int i = 0;
	int rc = 0;

	if (!fm2018_on) {
		fm2018_power_on();
	}

	for(i = 0; i < FM2018_GSM_HANDSET_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_gsm_handset_regs[i].RegAddr, fm2018_gsm_handset_regs[i].RegData);
		if (rc < 0) {
			printk("%s: reg write failed\n", __func__);
			return rc;
		}
	}
	mdelay(30);

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_GSM_HANDSET;
#endif

	return rc;
}
#endif

static struct fm2018_reg_val fm2018_gsm_handfree_regs[] = {
	{0x1E34, 0x0009},
	{0x1E3D, 0x0100},
	{0x1E41, 0x0101},
	{0x1E45, 0x030F},
	{0x1E46, 0x0011},
	{0x1E47, 0x2800},
	{0x1E48, 0x0800},
	{0x1E49, 0x0800},
	{0x1E4D, 0x0120},
	{0x1E58, 0x0019},
	{0x1E60, 0x0000},
	{0x1E63, 0x0001},
	{0x1E70, 0x05C0},
	{0x1E86, 0x0009},
	{0x1E87, 0x0004},
	{0x1E88, 0x6000},
	{0x1E89, 0x0100},
	{0x1E8B, 0x0280},
	{0x1E8C, 0x0020},
	{0x1E91, 0x0002},
	{0x1E92, 0x1800},
	{0x1E93, 0x1000},
	{0x1EC8, 0x4800},
	{0x1EF8, 0x2000},
	{0x1EF9, 0x0800},
	{0x1EFF, 0x4800},
	{0x1E3A, 0x0000},
};

#define FM2018_GSM_HANDFREE_REGS_NUM (sizeof(fm2018_gsm_handfree_regs) / FM2018_REG_VAL_SIZE)

static int fm2018_gsm_handfree(void)
{
	unsigned int i = 0;
	int rc = 0;
	uint16_t data = 0;

	if (!fm2018_on) {
		rc = fm2018_power_on();
		if (rc) {
			printk("%s: fm2018 power on error!\n", __func__);
			goto power_on_err;
		}
	}

	for(i = 0; i < FM2018_GSM_HANDFREE_REGS_NUM; ++i) {
		rc = fm2018_reg_write(fm2018_gsm_handfree_regs[i].RegAddr, fm2018_gsm_handfree_regs[i].RegData);
		if (rc < 0) {
			printk("%s: reg write failed\n", __func__);
			goto reg_write_err;
		}
	}
	mdelay(30);

	for(i = 0; i < (FM2018_GSM_HANDFREE_REGS_NUM - 1); ++i) {
		rc = fm2018_reg_read(fm2018_gsm_handfree_regs[i].RegAddr, &data);
		if (rc < 0) {
			printk("%s: reg read error!\n", __func__);
			goto reg_read_err;
		}

		if (data != fm2018_gsm_handfree_regs[i].RegData) {
			printk("%s: the data written is wrong!\n", __func__);
			rc = -EINVAL;
			goto data_err;
		}
	}

#ifdef CONFIG_DEBUG_FS
	cur_state = FM2018_GSM_HANDFREE;
#endif

data_err:
reg_read_err:
reg_write_err:
power_on_err:
	return rc;
}

static long fm2018_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int rc = 0;

	switch (cmd) {
	case FM2018_IOCTL_SET_BYPASS_MODE:
	case FM2018_IOCTL_CDMA_HANDSET:
	case FM2018_IOCTL_CDMA_HANDFREE:
	case FM2018_IOCTL_LOCAL_RECORD:
	case FM2018_IOCTL_GSM_HANDSET:
		rc = fm2018_bypass();
		if (rc < 0) {
			printk("%s: bypass error!\n", __func__);
		}
		break;
	case FM2018_IOCTL_POWER_OFF:
		rc = fm2018_power_off();
		if (rc) {
			printk("%s: power off error!\n", __func__);
		}
		break;
//	case FM2018_IOCTL_CDMA_HANDSET:
//		rc = fm2018_cdma_handset();
//		break;
//	case FM2018_IOCTL_CDMA_HANDFREE:
//		rc = fm2018_cdma_handfree();
//		break;
//	case FM2018_IOCTL_LOCAL_RECORD:
//		rc = fm2018_local_record();
//		break;
//	case FM2018_IOCTL_GSM_HANDSET:
//		rc = fm2018_gsm_handset();
//		break;
	case FM2018_IOCTL_GSM_HANDFREE:
		rc = fm2018_gsm_handfree();
		if (rc < 0) {
			printk("%s: gsm handfree error!\n", __func__);
		}
		break;
	default:
		printk("%s: invalid mode\n", __func__);
		break;
	}

	return rc;
}

static const struct file_operations fm2018_fops = {
	.owner = THIS_MODULE,
	.open = fm2018_open,
	.release = fm2018_release,
	.unlocked_ioctl = fm2018_ioctl,
};

static struct miscdevice fm2018_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "echo_canceller",
	.fops = &fm2018_fops,
};

#ifdef CONFIG_DEBUG_FS

static struct dentry *debugfs_root = NULL;

static struct dentry *debugfs_peek = NULL;

static struct dentry *debugfs_poke = NULL;

static struct dentry *debugfs_power = NULL;

static struct dentry *debugfs_dump = NULL;

static int fm2018_debug_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static int get_parameters(char *buf, long int *param, int num)
{
	char *token = NULL;
	int base = 0;
	int cnt = 0;

	token = strsep(&buf, " ");

	for(cnt = 0; cnt < num; ++cnt) {
		if (token != NULL) {
			if (('x' == token[1]) || ('X' == token[1])) {
				base = 16;
			} else {
				base = 10;
			}

			if (strict_strtoul(token, base, &param[cnt]) != 0) {
				printk("%s: strict_strtoul failed!", __func__);
				return -EINVAL;
			}

			token = strsep(&buf, " ");
		} else {
			printk("%s: token is NULL!", __func__);
			return -EINVAL;
		}
	}

	return 0;
}

static 	u16 read_data = 0;

static ssize_t fm2018_debug_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
	char *access_str = filp->private_data;
	char lbuf[64];
	int rc = 0;
	long int param[5];

	if (cnt > sizeof(lbuf) - 1) {
		printk("%s: cnt is too big!", __func__);
		return -EINVAL;
	}

	rc = copy_from_user(lbuf, ubuf, cnt);
	if (rc) {
		printk("%s: copy from user failed!", __func__);
		return -EFAULT;
	}

	lbuf[cnt] = '\0';

	if (!strcmp(access_str, "power")) {
		if (get_parameters(lbuf, param, 1) == 0) {
			switch (param[0]) {
			case 1:
				switch (cur_state) {
				case FM2018_IDLE:
					rc = fm2018_power_on();
					if (rc) {
						printk("%s: fm2018 power on error!\n", __func__);
					}
					break;
				case FM2018_BYPASS:
				case FM2018_CDMA_HANDSET:
				case FM2018_CDMA_HANDFREE:
				case FM2018_LOCAL_RECORD:
				case FM2018_GSM_HANDSET:
					rc = fm2018_bypass();
					if (rc < 0) {
						printk("%s: bypass error!\n", __func__);
					}
					break;
//				case FM2018_CDMA_HANDSET:
//					fm2018_cdma_handset();
//					break;
//				case FM2018_CDMA_HANDFREE:
//					fm2018_cdma_handfree();
//					break;
//				case FM2018_LOCAL_RECORD:
//					fm2018_local_record();
//					break;
//				case FM2018_GSM_HANDSET:
//					fm2018_gsm_handset();
//					break;
				case FM2018_GSM_HANDFREE:
					rc = fm2018_gsm_handfree();
					if (rc < 0) {
						printk("%s: gsm handfree error!\n", __func__);
					}
					break;
				default:
					break;
				}
				break;
			case 0:
				rc = fm2018_power_off();
				if (rc) {
					printk("%s: power off error!\n", __func__);
				}
				break;
			default:
				printk("%s: Invalid value for power!", __func__);
				rc = -EINVAL;
				break;
			}
		} else {
			printk("%s: get parameters failed!", __func__);
			rc = -EINVAL;
		}
	} else if (!strcmp(access_str, "poke")) {
		/* write */
		rc = get_parameters(lbuf, param, 2);
		if ((param[0] >= 0x1E00) && (param[0] <= 0x1F20) && (param[1] <= 0xFFFF) && (0 == rc)) {
			rc = fm2018_reg_write(param[0], param[1]);
			if (rc) {
				printk("%s: reg write error!\n", __func__);
			}
		} else {
			printk("%s: Invalid parameters to write!", __func__);
			rc = -EINVAL;
		}
	} else if (!strcmp(access_str, "peek")) {
		/* read */
		rc = get_parameters(lbuf, param, 1);
		if ((param[0] >= 0x1E00) && (param[0] <= 0x1F20) && (0 == rc)) {
			rc = fm2018_reg_read(param[0], &read_data);
			if (rc < 0) {
				printk("%s: reg read error!\n", __func__);
			}
		} else {
			printk("%s: Invalid parameters to read!", __func__);
			rc = -EINVAL;
		}
	}

	if (rc == 0) {
		rc = cnt;
	} else {
		printk("%s: rc = %d\n", __func__, rc);
	}

	return rc;
}

#define DUMP_BUF_SIZE 10240

static char dump_buf[DUMP_BUF_SIZE];

static unsigned short fm2018_reg_addr[] = {

	//Parameters associated with device enable/format
	0x1E30, 0x1E32, 0x1E33, 0x1E39,
	0x1E3A, 0x1E41, 0x3FD9,

	//Parameters associated with HW gain
	0x1E34, 0x1E35, 0x1E36, 0x1E4A,
	0x1E4B, 0x1E4C, 0x3FC0, 0x3FC1,
	0x3FC6,

	//Parameters associated with SW process on/off
	0x1E44, 0x1E45, 0x1E46, 0x1E4F,
	0x1E51, 0x1E52, 0x1E58, 0x1E60,
	0x1E70, 0x1F30,

	//Parameters associated with SW gain
	0x1E3D, 0x1E3E, 0x1E40, 0x1E59,
	0x1EA1,

	//Parameters associated with microphone auto calibration
	0x1F00, 0x1F01,

	//Parameters associated with mic-in AGC enable: bit 15 of 0x1E45
	0x1EA7, 0x1EA8, 0x1EAB,

	//Parameter associated with noise paste back enable: bit 0 of 0x1E46
	0x1EA0,

	//Parameters associated with Line_out Equalizer
	0x1ED0, 0x1ED1, 0x1ED2, 0x1ED3, 0x1ED4,

	//Parameters associated with Speaker-out Equalizer
	0x1F06, 0x1F07, 0x1F08, 0x1F09,

	//Parameter associated with Speaker Dynamic Range Control (DRC) enable: bit[4] of 0x1E44
	0x1F28, 0x1F29, 0x1F2A, 0x1F2B,

	//Parameters associated with acoustic echo cancellation
	0x1E4D, 0x1E63, 0x1E86, 0x1E87,
	0x1E88, 0x1E89, 0x1E8B, 0x1E8C,
	0x1E90, 0x1E91, 0x1E92, 0x1E93,
	0x1EC5, 0x1EC6, 0x1EC7, 0x1EC8,
	0x1EC9, 0x1ECA, 0x1ECB, 0x1ECC,
	0x1EE0,

	//Parameters associated with half duplex control
	0x1E57, 0x1E5C,

	//Parameters associated with noise suppression
	0x1E47, 0x1E48, 0x1E49, 0x1EB3,
	0x1EB4, 0x1EB5, 0x1EBB, 0x1EBC,
	0x1EBD, 0x1EBE, 0x1EBF, 0x1EC0,
	0x1EC1, 0x1EDA,

	//Parameter associated with VAD (2-mic only)
	0x1E80, 0x1EF8, 0x1EF9, 0x1EFF,
	0x1F0A, 0x1F0B, 0x1F0C, 0x1F0D,

	//Parameter associated with BVE
	0x1EE2, 0x1EE3, 0x1EE4,

	//Read only parameter for status
	0x1E5E, 0x1E65, 0x1E66, 0x1E67,
	0x091B, 0x01E8, 0x3FE1,

};

#define FM2018_REG_ADDR_SIZE (sizeof(fm2018_reg_addr) / sizeof(unsigned short))

static ssize_t fm2018_debug_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	char lbuf[17];
	char *dump_off = dump_buf;
	ssize_t rc = 0;
	int i = 0;
	int err = 0;
	u16 reg_val = 0;

	if (!strcmp(file->private_data, "dump")) {
		memset(dump_buf, 0, DUMP_BUF_SIZE);

		for(i = 0; (i <= FM2018_REG_ADDR_SIZE) && fm2018_opened; ++i) {
			err = fm2018_reg_read(fm2018_reg_addr[i], &reg_val);
			if (err < 0) {
				printk("failed to read fm2018 register\n");
				break;
			}

			dump_off += sprintf(dump_off, "reg 0x%04x: 0x%04x\n", fm2018_reg_addr[i], reg_val);
		}

		rc = simple_read_from_buffer(ubuf, count, ppos, dump_buf, dump_off - dump_buf);
	} else {
		snprintf(lbuf, sizeof(lbuf) - 1, "0x%x\n", read_data);
		lbuf[16] = '\0';

		rc = simple_read_from_buffer(ubuf, count, ppos, lbuf, strlen(lbuf));
	}

	return rc;
}

static const struct file_operations fm2018_debug_ops = {
	.open = fm2018_debug_open,
	.write = fm2018_debug_write,
	.read = fm2018_debug_read
};

#endif

static int fm2018_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;

	pdata = client->dev.platform_data;

	if (!pdata) {
		pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			rc = -ENOMEM;
			printk("%s: platform data is NULL\n", __func__);
			goto err_alloc_data_failed;
		}
	}

	this_client = client;

	rc = fm2018_hw_init();
	if (rc) {
		printk("%s: hardware init failed\n", __func__);
		goto err_hw_init;
	}

	rc = misc_register(&fm2018_device);
	if (rc) {
		printk("%s: fm2018_device register failed\n", __func__);
		goto err_misc_register;
	}

#ifdef CONFIG_DEBUG_FS
	debugfs_root = debugfs_create_dir("fm2018", 0);
	if (!IS_ERR(debugfs_root)) {
		debugfs_peek = debugfs_create_file("peek", 0777, debugfs_root, (void *)"peek", &fm2018_debug_ops);
		debugfs_poke = debugfs_create_file("poke", 0777, debugfs_root, (void *)"poke", &fm2018_debug_ops);
		debugfs_power = debugfs_create_file("power", 0777, debugfs_root, (void *)"power", &fm2018_debug_ops);
		debugfs_dump = debugfs_create_file("dump", 0777, debugfs_root, (void *)"dump", &fm2018_debug_ops);
	}
#endif
	//fm2018_bypass();
	return 0;

err_misc_register:
err_hw_init:
	if (pdata) {
		kfree(pdata);
		pdata = NULL;
	}
err_alloc_data_failed:
	return rc;
}

static int fm2018_remove(struct i2c_client *client)
{
	kfree(i2c_get_clientdata(client));
	return 0;
}

static int fm2018_suspend(struct i2c_client *client, pm_message_t mesg)
{
	gpio_free(pdata->gpio_fm2018_sda);
	gpio_free(pdata->gpio_fm2018_scl);

	return 0;
}

static int fm2018_resume(struct i2c_client *client)
{
	gpio_request(pdata->gpio_fm2018_sda, "fm2018_sda");
	gpio_request(pdata->gpio_fm2018_scl, "fm2018_scl");

	return 0;
}

static const struct i2c_device_id fm2018_id[] = {
	{"fm2018", 0},
	{}
};

static struct i2c_driver fm2018_driver = {
	.probe = fm2018_probe,
	.remove = fm2018_remove,
	.suspend = fm2018_suspend,
	.resume = fm2018_resume,
	.id_table = fm2018_id,
	.driver = {
		.name = "fm2018",
		.owner = THIS_MODULE,
	},
};

static int __init fm2018_init(void)
{

	return i2c_add_driver(&fm2018_driver);
}

static void __exit fm2018_exit(void)
{
#ifdef CONFIG_DEBUG_FS
	debugfs_remove(debugfs_peek);
	debugfs_remove(debugfs_poke);
	debugfs_remove(debugfs_power);
	debugfs_remove(debugfs_dump);
	debugfs_remove(debugfs_root);
#endif
	i2c_del_driver(&fm2018_driver);
}

module_init(fm2018_init);
module_exit(fm2018_exit);

