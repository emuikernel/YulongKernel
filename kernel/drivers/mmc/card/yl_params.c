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

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/scatterlist.h>

#include <linux/genhd.h>
#include <linux/reboot.h>
#include <linux/yl_params.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/slab.h>
/* I define transition phase, to make system/app groups' life easy.
 * During transition phase, system/app can mix use yl_params and yl_params1.
 * System/app can keep on using old V0 specification for YL param partition,
 * and still access yl_params, disregard V1 specification.
 * As Mr. Zhang urge them to conform V1 specification,
 * I hope system/app can transist to V1 as soon as possible.
 * When most people switched to V1 specification,
 * we will close transistion phase, and yl_params will dispear,
 * only left those V0 people struggle along.
 * So, KEEP your eyes on.
 * */
/* #define PARAMS_TRANSITION */


#define YL_PARAMS_NAME	"params"
#define YL_MISC_NAME	"misc"

/* on NVIDIA, partition name is 3-characters */
/*
#define YL_PARAMS_NAME	"PAR"
#define YL_MISC_NAME	"MSC"
*/

#define RECOVERY_FLAG  "boot-recovery"

#define YL_PARAMS_COUNT	16
#define TAG_LENGTH	16

#define PARAMS_TAG  "YL-params: "

#ifdef PARAMS_DEBUG
#define PARAMS_DBG(fmt, ...) \
	printk(PARAMS_TAG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define PARAMS_DBG(fmt, ...) do { \
	} while (0)
#endif

struct params_map {
	char tag[TAG_LENGTH];
	unsigned int block;
};

#define TAG_LENGTH_V0	10

#ifdef PARAMS_TRANSITION
struct params_map yl_params_map_obsolete[] = {
	{"DEVICE",       YL_OBSOLETE_DEVICE},
	{"WLAN",         YL_OBSOLETE_WLAN},
	{"ACCTW-CAL",    YL_OBSOLETE_ACCTW_CAL},
	{"DOWNFLAG",     YL_OBSOLETE_DOWNFLAG},
	{"GUARD",        YL_OBSOLETE_GUARD},
	{"odm_lckc",     YL_OBSOLETE_odm_lckc},
	{"odm_lckl",     YL_OBSOLETE_odm_lckl},
	{"ALM_TIME",     YL_OBSOLETE_ALM_TIME},
	{"mms_imsi",     YL_OBSOLETE_mms_imsi},
	{"CAM_CAL",      YL_OBSOLETE_CAM_CAL},
	{"AUTO_REG",     YL_OBSOLETE_AUTO_REG},
	{"VOL_TAB",      YL_OBSOLETE_VOL_TAB},
	{"PASSWRD",      YL_OBSOLETE_PASSWRD},
	{"MMS_VER",      YL_OBSOLETE_MMS_VER},
	{"ALM_POFF",     YL_OBSOLETE_ALM_POFF},
	{"RESERVED!",    YL_OBSOLETE_RESERVED},
};
#endif

struct params_map yl_params_map[] = {
	{"DEVICE",		YL_DEVICE},
	{"CONFIGURATION",	YL_CONFIGURATION},
	{"PRODUCTLINE",		YL_PRODUCTLINE},
	{"DYNAMIC",		YL_DYNAMIC},
	{"GUARD",		YL_GUARD},
	{"CMDLINE",		YL_CMDLINE},
	{"TOUCHSCRREN0",	YL_TOUCHSCRREN0},
	{"TOUCHSCRREN1",	YL_TOUCHSCRREN1},
	{"TOUCHSCRREN2",	YL_TOUCHSCRREN2},
	{"TOUCHSCRREN3",	YL_TOUCHSCRREN3},
	{"RESERVE0",		YL_RESERVE0},
	{"RESERVE1",		YL_RESERVE1},
	{"PROJECT0",		YL_PROJECT0},
	{"PROJECT1",		YL_PROJECT1},
	{"PROJECT2",		YL_PROJECT2},
	{"PROJECT3",		YL_PROJECT3},
};

#ifdef PARAMS_TRANSITION
static struct miscdevice yl_params_dev0;
#endif
static struct miscdevice yl_params_dev1;

struct yl_params_data {
	struct mmc_card	*card;
	struct hd_struct *part;
};

static struct yl_params_data yl_params_priv;
static struct yl_params_data yl_misc_priv;

#ifdef PARAMS_TRANSITION
static int yl_params_get_pageoffset_v0(const char *buf)
{
	unsigned int i;

	for (i = 0; i < YL_PARAMS_COUNT; i++) {
		if (!strncmp(yl_params_map_obsolete[i].tag, buf, TAG_LENGTH_V0)) {
			PARAMS_DBG("find tag name is [%s]\n", buf);
			return yl_params_map_obsolete[i].block;
		}
	}
	return -ERANGE;
}
#endif

static int yl_params_get_pageoffset(const char *buf)
{
	unsigned int i;

	for (i = 0; i < YL_PARAMS_COUNT; i++) {
		if (!strncmp(yl_params_map[i].tag, buf, TAG_LENGTH)) {
			PARAMS_DBG("find tag name is [%s]\n", buf);
			return yl_params_map[i].block;
		}
	}
	return -ERANGE;
}

/*
 * Fill in the mmc_request structure given a set of transfer parameters.
 */
static void yl_params_prepare_mrq(struct mmc_card *card,
	struct mmc_request *mrq, struct scatterlist *sg, unsigned sg_len,
	unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
{
	BUG_ON(!mrq || !mrq->cmd || !mrq->data || !mrq->stop);

	if (blocks > 1) {
		mrq->cmd->opcode = write ?
			MMC_WRITE_MULTIPLE_BLOCK : MMC_READ_MULTIPLE_BLOCK;
	} else {
		mrq->cmd->opcode = write ?
			MMC_WRITE_BLOCK : MMC_READ_SINGLE_BLOCK;
	}

	mrq->cmd->arg = dev_addr;
	if (!mmc_card_blockaddr(card))
		mrq->cmd->arg <<= 9;

	mrq->cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	if (blocks == 1)
		mrq->stop = NULL;
	else {
		mrq->stop->opcode = MMC_STOP_TRANSMISSION;
		mrq->stop->arg = 0;
		mrq->stop->flags = MMC_RSP_R1B | MMC_CMD_AC;
	}

	mrq->data->blksz = blksz;
	mrq->data->blocks = blocks;
	mrq->data->flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
	mrq->data->sg = sg;
	mrq->data->sg_len = sg_len;

	mmc_set_data_timeout(mrq->data, card);
}

/*
 * Wait for the card to finish the busy state
 */
static int yl_params_wait_busy(struct mmc_card *card)
{
	int ret, busy;
	struct mmc_command cmd;

	busy = 0;
	do {
		memset(&cmd, 0, sizeof(struct mmc_command));

		cmd.opcode = MMC_SEND_STATUS;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

		ret = mmc_wait_for_cmd(card->host, &cmd, 0);
		if (ret)
			break;

		if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) {
			busy = 1;
			pr_info("%s: Warning: Host did not "
					"wait for busy state to end.\n",
					mmc_hostname(card->host));
		}
	} while (!(cmd.resp[0] & R1_READY_FOR_DATA));

	return ret;
}

/*
 * Checks that a normal transfer didn't have any errors
 */
static int yl_params_check_res(struct mmc_request *mrq)
{
	int ret = 0;

	BUG_ON(!mrq || !mrq->cmd || !mrq->data);

	if (mrq->cmd->error)
		ret = mrq->cmd->error;
	else if (mrq->data->error)
		ret = mrq->data->error;
	else if (mrq->stop && mrq->stop->error)
		ret = mrq->stop->error;
	else if (mrq->data->bytes_xfered !=
			mrq->data->blocks * mrq->data->blksz)
		ret = -EIO;

	if (ret)
		pr_err("YLLOG:%s return ret=%d\n", __func__, ret);
	return ret;
}

static int yl_params_basic_transfer(struct yl_params_data *priv,
		sector_t offset, char *buf, int write)
{
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_command stop;
	struct mmc_data data;
	struct scatterlist sg;
	struct mmc_card *card;

	memset(&mrq, 0, sizeof(struct mmc_request));
	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	memset(&stop, 0, sizeof(struct mmc_command));

	mrq.cmd = &cmd;
	mrq.data = &data;
	mrq.stop = &stop;

	sg_init_one(&sg, buf, 512);

	card = priv->card;

	mmc_claim_host(card->host);
	yl_params_prepare_mrq(card, &mrq, &sg, 1,
			priv->part->start_sect + offset,
			1, 512, write);
	mmc_wait_for_req(card->host, &mrq);
	yl_params_wait_busy(card);
	mmc_release_host(card->host);

	if (yl_params_check_res(&mrq))
		return 0;
	else
		return 512;
}

#ifdef PARAMS_TRANSITION
static char aid_buf[512];
/* these three, hold for dynamic created V0 Device_info.
 * As read Device_info consist reading 4 blocks,
 * and some app such as Settings often read Device_info,
 * so I make one buffer to accelerate it.
 * */
DEFINE_MUTEX(device_mutex);
static char device_info_buf[512];
static int device_loaded;

static int yl_params_read_transistion(int block, char *buf, int ver)
{
	int len = 0;

	if (block != YL_OBSOLETE_DEVICE &&
	    block != YL_OBSOLETE_GUARD &&
	    block != YL_OBSOLETE_PASSWRD) {
		pr_err("YLLOG:%s caught unknown params block(%d) read!\n",
			__func__, block);
		return 0;
	}

	switch (block) {
	case YL_OBSOLETE_GUARD:
		len = yl_params_basic_transfer(&yl_params_priv,	YL_GUARD,
				buf, 0);
		if (len < 512)
			return 0;
		memmove(buf + TAG_LENGTH_V0,
			buf + TAG_LENGTH, 512 - TAG_LENGTH);
		break;
	case YL_OBSOLETE_PASSWRD:
		len = yl_params_basic_transfer(&yl_params_priv,	YL_DYNAMIC,
				aid_buf, 0);
		if (len < 512)
			return 0;
		memcpy(buf + TAG_LENGTH_V0,
			aid_buf + TAG_LENGTH + 32, TAG_LENGTH);
		break;
	case YL_OBSOLETE_DEVICE:
		{
		struct DeviceInfo_v0 *pDevice_v0;
		struct DeviceInfo *pDevice;
		struct ConfigurationInfo *pConf;
		struct ProductlineInfo *pProd;
		struct DynamicInfo *pDyn;

		mutex_lock(&device_mutex);
		if (device_loaded) {
			memcpy(buf, device_info_buf, 512);
			len = 512;
			mutex_unlock(&device_mutex);
			break;
		}
		pDevice_v0 = (struct DeviceInfo_v0 *)device_info_buf;
		strcpy(pDevice_v0->SyncByte,
			yl_params_map_obsolete[YL_OBSOLETE_DEVICE].tag);

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_DEVICE, aid_buf, 0);
		if (len < 512) {
			mutex_unlock(&device_mutex);
			return 0;
		}
		pDevice = (struct DeviceInfo *)aid_buf;
		strncpy(pDevice_v0->CommunicationModel1,
			pDevice->CommunicationModel1, 16);
		strncpy(pDevice_v0->CommunicationModel2,
			pDevice->CommunicationModel2, 16);

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_CONFIGURATION, aid_buf, 0);
		if (len < 512) {
			mutex_unlock(&device_mutex);
			return 0;
		}
		pConf = (struct ConfigurationInfo *)aid_buf;
		strncpy(pDevice_v0->DeviceName, pConf->ProductName, 16);
		pDevice_v0->HardwareVersionMajor =
			pConf->HardwareVersionMajor[1] - '0';

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_PRODUCTLINE, aid_buf, 0);
		if (len < 512) {
			mutex_unlock(&device_mutex);
			return 0;
		}
		pProd = (struct ProductlineInfo *)aid_buf;
		strncpy(pDevice_v0->SN, pProd->SN, 16);
		memcpy(pDevice_v0->IMEI1, pProd->IMEI1, 32);
		memcpy(pDevice_v0->IMEI2, pProd->IMEI2, 32);
		memcpy(&pDevice_v0->Module1CalStatus,
			&pProd->ModuleCalStatus1, 4);
		memcpy(&pDevice_v0->DMtag, &pProd->DMtag, 3);

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_DYNAMIC, aid_buf, 0);
		if (len < 512) {
			mutex_unlock(&device_mutex);
			return 0;
		}
		pDyn = (struct DynamicInfo *)aid_buf;
		memcpy(&pDevice_v0->BSP, &pDyn->BSP, 32);
		memcpy(pDevice_v0->SupperPassword, pDyn->SupperPassword, 8);

		memcpy(buf, device_info_buf, 512);
		len = 512;
		device_loaded = 1;
		mutex_unlock(&device_mutex);
		}
		break;
	}
	return len;
}
#endif

/* caller should only provide buffer of size 512 bytes exactly,
 * */
int yl_params_kernel_read(char *buf, size_t count, int ver)
{
	int block;
	size_t len;

	if (count != 512)
		return -EINVAL;

#ifdef PARAMS_TRANSITION
	if (ver == 0)
		block = yl_params_get_pageoffset_v0(buf);
	else
#endif
		block = yl_params_get_pageoffset(buf);
	if (block < 0) {
		pr_err("YLLOG:Can't find tag %s, line %d!\n", buf, __LINE__);
		return -ERANGE;
	}

#ifdef PARAMS_TRANSITION
	if (ver == 0)
		len = yl_params_read_transistion(block, buf, ver);
	else
#endif
		len = yl_params_basic_transfer(&yl_params_priv, block,
			buf, 0);
	PARAMS_DBG("block=%d,count=%d at %s func\n", block, count, __func__);
	return count < len ? count : len;
}
EXPORT_SYMBOL_GPL(yl_params_kernel_read);

#ifdef PARAMS_TRANSITION
static int yl_params_write_transistion(int block, char *buf, int ver)
{
	int len = 0;

	if (block != YL_OBSOLETE_DEVICE &&
	    block != YL_OBSOLETE_GUARD &&
	    block != YL_OBSOLETE_PASSWRD) {
		pr_err("YLLOG:%s caught unknown params block(%d) write!\n",
			__func__, block);
		return 0;
	}

	switch (block) {
	case YL_OBSOLETE_GUARD:
		memmove(buf + TAG_LENGTH,
			buf + TAG_LENGTH_V0, 512 - TAG_LENGTH);
		memset(buf + TAG_LENGTH_V0, 0, TAG_LENGTH - TAG_LENGTH_V0);
		len = yl_params_basic_transfer(&yl_params_priv,	YL_GUARD,
				buf, 1);
		if (len < 512)
			return 0;
		break;
	case YL_OBSOLETE_PASSWRD:
		len = yl_params_basic_transfer(&yl_params_priv,	YL_DYNAMIC,
				aid_buf, 0);
		if (len < 512)
			return 0;
		strncpy(aid_buf + TAG_LENGTH + 32, buf + TAG_LENGTH_V0, 16);
		len = yl_params_basic_transfer(&yl_params_priv,	YL_DYNAMIC,
				aid_buf, 1);
		if (len < 512)
			return 0;
		break;
	case YL_OBSOLETE_DEVICE:
		{
		struct DeviceInfo_v0 *pDevice_v0;
		struct ProductlineInfo *pProd;
		struct DynamicInfo *pDyn;

		mutex_lock(&device_mutex);
		pDevice_v0 = (struct DeviceInfo_v0 *)buf;

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_PRODUCTLINE, aid_buf, 0);
		if (len < 512) {
			device_loaded = 0;
			mutex_unlock(&device_mutex);
			return 0;
		}
		pProd = (struct ProductlineInfo *)aid_buf;
		strncpy(pProd->SN, pDevice_v0->SN, 16);
		memcpy(pProd->IMEI1, pDevice_v0->IMEI1, 32);
		memcpy(pProd->IMEI2, pDevice_v0->IMEI2, 32);
		pProd->ModuleCalStatus1 = pDevice_v0->Module1CalStatus;
		pProd->ModuleCalStatus2 = pDevice_v0->Module2CalStatus;
		pProd->ModuleRFTestStatus1 = pDevice_v0->Module1RFTestStatus;
		pProd->ModuleRFTestStatus2 = pDevice_v0->Module2RFTestStatus;
		pProd->DMtag = pDevice_v0->DMtag;
		pProd->CameraCal = pDevice_v0->sensor_cal_flag;
		pProd->RPtag = pDevice_v0->RPtag;

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_PRODUCTLINE, aid_buf, 1);
		if (len < 512) {
			device_loaded = 0;
			mutex_unlock(&device_mutex);
			return 0;
		}

		len = yl_params_basic_transfer(&yl_params_priv,
				YL_DYNAMIC, aid_buf, 0);
		if (len < 512) {
			device_loaded = 0;
			mutex_unlock(&device_mutex);
			return 0;
		}
		pDyn = (struct DynamicInfo *)aid_buf;
		memcpy(pDyn->SupperPassword, pDevice_v0->SupperPassword, 8);
		len = yl_params_basic_transfer(&yl_params_priv,
				YL_DYNAMIC, aid_buf, 1);
		if (len < 512) {
			device_loaded = 0;
			mutex_unlock(&device_mutex);
			return 0;
		}

		memcpy(device_info_buf, buf, 512);
		len = 512;
		device_loaded = 1;
		mutex_unlock(&device_mutex);
		}
		break;
	}
	return len;
}
#endif

/* caller should only provide buffer of size 512 bytes exactly,
 * as only read-modify-write is accept
 * */
int yl_params_kernel_write(char *buf, size_t count, int ver)
{
	int block;
	size_t len;

	if (count != 512)
		return -EINVAL;

#ifdef PARAMS_TRANSITION
	if (ver == 0)
		block = yl_params_get_pageoffset_v0(buf);
	else
#endif
		block = yl_params_get_pageoffset(buf);
	if (block < 0) {
		pr_err("YLLOG:Can't find tag %s, line %d!\n", buf, __LINE__);
		return -ERANGE;
	}

	PARAMS_DBG("block=%d,count=%d at %s\n", block, count, __func__);
#ifdef PARAMS_TRANSITION
	if (ver == 0)
		len = yl_params_write_transistion(block, buf, ver);
	else
#endif
		len = yl_params_basic_transfer(&yl_params_priv, block,
			buf, 1);
	return count < len ? count : len;
}
EXPORT_SYMBOL_GPL(yl_params_kernel_write);

static int yl_params_open(struct inode *inode, struct file *file)
{
	dev_t dev = inode->i_rdev;
	if (MINOR(dev) == yl_params_dev1.minor)
		file->private_data = (void *)1;
	else
		file->private_data = (void *)0;

	return 0;
}

static int yl_params_read(struct file *file, char __user *buf,
		size_t count, loff_t *ppos)
{
	int len;
	size_t count_temp;
	char kernel_buf[512];
	int ver = !!(int)file->private_data;


	if (count <= TAG_LENGTH_V0)
		pr_debug("%s read count %d\n", __func__, count);

	count_temp = copy_from_user(kernel_buf, buf,
			count < TAG_LENGTH ? count : TAG_LENGTH);
	if (count_temp != 0)
		pr_debug("%s copy left %d\n", __func__, count_temp);

	len = yl_params_kernel_read(kernel_buf, 512, ver);
	if (len <= 0)
		return len;

	count_temp = copy_to_user(buf, kernel_buf, len);
	return len - count_temp;
}

static int yl_params_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	char kernel_buf[512];
	int ver = !!(int)file->private_data;

	if (count <= TAG_LENGTH_V0)
		pr_debug("%s write count %d\n", __func__, count);

	/* We only support reading a maximum of a flash block */
	if (count > 512)
		count = 512;

	if (copy_from_user(kernel_buf, buf, count)) {
		pr_err("%s: copy failed\n", __func__);
		return  -EINVAL;
	}
	return yl_params_kernel_write(kernel_buf, 512, ver);
}

static const struct file_operations yl_params_fops = {
	.owner		= THIS_MODULE,
	.open		= yl_params_open,
	.read		= yl_params_read,
	.write		= yl_params_write,
};

#ifdef PARAMS_TRANSITION
static struct miscdevice yl_params_dev0 = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "yl_params",
	.fops = &yl_params_fops,
};
#endif

static struct miscdevice yl_params_dev1 = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "yl_params1",
	.fops = &yl_params_fops,
};

static int yl_reboot(struct notifier_block *nb, unsigned long event,
			void *unused)
{
	if (event == SYS_RESTART && unused && !strcmp(unused, "recovery")) {
		int ret;
		char kernel_buf[512] = RECOVERY_FLAG;

		ret = yl_params_basic_transfer(&yl_misc_priv, 0,
				kernel_buf, 1);
		PARAMS_DBG("yl_reboot write recover flag %s!\n",
				ret ? "success" : "failed");
	}
	printk(KERN_ERR  "YLLOG:system off event=%ld\n", event);
	return NOTIFY_DONE;
}

static struct notifier_block yl_reboot_fg_notifier = {
	.notifier_call = yl_reboot,
};

int yl_params_init(struct mmc_card *card)
{
	unsigned int res;

	if (yl_params_priv.card) {
		pr_debug("YLLOG:yl_params re-init denied, old card %p, new card %p!\n",
				yl_params_priv.card, card);
		return -EEXIST;
	}

	/* add ourselves to the reboot_notifier_list.
	 * We support adb reboot-recovery,
	 * system can't catch this command,
	 * so it's kernel duty to write boot-recovery flag.
	 * As we have a common reboot mechanism using scratch register,
	 * which also support recovery,
	 * why still is this flag needed?
	 * Enn, maybe we dare of power cut-off during reboot.
	 * */
	res = register_reboot_notifier(&yl_reboot_fg_notifier);
	if (res != 0)
		PARAMS_DBG("can't register reboot notifier\n");

	yl_params_priv.card = card;
	yl_misc_priv.card = card;

#ifdef PARAMS_TRANSITION
	res = misc_register(&yl_params_dev0);
	if (res)
		goto out;
#endif

	res = misc_register(&yl_params_dev1);
	/* If params v1 register fail, it doesn't matter */
	if (res) {
		PARAMS_DBG("regist yl_params1 failed %d\n", res);
#ifndef PARAMS_TRANSITION
		goto out;
#endif
	}

	return 0;

out:
	unregister_reboot_notifier(&yl_reboot_fg_notifier);
	pr_err("YLLOG:%s: Driver Initialisation failed\n", __FILE__);
	return res;
}
EXPORT_SYMBOL(yl_params_init);

void notify_ylparams(struct hd_struct *part)
{
	/* long long ago, it is
	 * char *pname = part->partition_name;
	 * */
	char *pname = part->info->volname;

	if (!strcmp(pname, YL_PARAMS_NAME))
		yl_params_priv.part = part;
	else if (!strcmp(pname, YL_MISC_NAME))
		yl_misc_priv.part = part;
	else
		return;
	PARAMS_DBG("find %s, partno=%d start_sect= %lu nr_sects= %lu!\n",
			pname,
			part->partno,
			part->start_sect,
			part->nr_sects);
#ifdef PARAMS_TRANSITION
#endif
}
EXPORT_SYMBOL(notify_ylparams);
