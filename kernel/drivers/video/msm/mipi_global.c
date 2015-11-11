/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_fb.h"
#include "mipi_dsi.h"

static struct  mipi_dsi_panel_platform_data *mipi_global_pdata;
static struct dsi_buf mipi_global_tx_buf;
static struct dsi_buf mipi_global_rx_buf;

static char hx8369_boyi_405_video13[2] = {
	0x51, 0x88,
};

static struct dsi_cmd_desc set_backlight[] = {
	{DTYPE_GEN_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video13), hx8369_boyi_405_video13},
};

#define NT35510_SLEEP_OFF_DELAY 120
#define NT35510_DISPLAY_ON_DELAY 120

/*ykl add*/
#include <linux/leds.h>
static int wled_trigger_initialized;
/*end*/

#define MIPI_GLOBAL_PARA
#define SYSFS_DEBUG_ENABLE
#define ORISE9806_YASHI_450_ID 2

#if defined(CONFIG_YULONG_LCD_ESD) || defined(CONFIG_READ_BACKLIGHT)
static unsigned char mipi_dsi_read_reg(struct msm_fb_data_type *mfd,
			struct dsi_cmd_desc dsi_cmd_desc)
{
	unsigned char lp;
	mutex_lock(&mfd->dma->ov_mutex);
	mipi_dsi_cmds_rx(mfd, &mipi_global_tx_buf, &mipi_global_rx_buf,
			&dsi_cmd_desc, 1);
	mutex_unlock(&mfd->dma->ov_mutex);
	lp = *mipi_global_rx_buf.data;
	return lp;
}
#endif

#ifdef CONFIG_YULONG_LCD_ESD
static char reg_val[2] = {0x0a, 0x00}; /* DTYPE_DCS_READ */

static struct dsi_cmd_desc novatek_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(reg_val), reg_val};

static int read_panel_reg(struct msm_fb_data_type *mfd)
{
	unsigned char lp;
	int ret = 0;
	reg_val[0] = 0x0a;
	lp = mipi_dsi_read_reg(mfd, novatek_manufacture_id_cmd);
	pr_debug("%s: reg=%d; val=%d\n", __func__, reg_val[0], lp);
	if (lp != 0x9c) {
		ret = 1;
		goto out;
	}
if (1) {
	reg_val[0] = 0x0b;
	lp = mipi_dsi_read_reg(mfd, novatek_manufacture_id_cmd);
	pr_debug("%s: reg=%d; val=%d\n", __func__, reg_val[0], lp);
	if (lp != 0x00) {
		ret = 1;
		goto out;
	}
	reg_val[0] = 0x0c;
	lp = mipi_dsi_read_reg(mfd, novatek_manufacture_id_cmd);
	pr_debug("%s: reg=%d; val=%d\n", __func__, reg_val[0], lp);
	if (lp != 0x07) {
		ret = 1;
		goto out;
	}
	reg_val[0] = 0x0d;
	lp = mipi_dsi_read_reg(mfd, novatek_manufacture_id_cmd);
	pr_debug("%s: reg=%d; val=%d\n", __func__, reg_val[0], lp);
	if (lp != 0x00) {
		ret = 1;
		goto out;
	}
}
out:
	return ret;
}
#endif

#ifdef CONFIG_READ_BACKLIGHT
static char reg_val1[2] = {0x52, 0x00};
static struct dsi_cmd_desc get_backlight_val = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(reg_val1), reg_val1};
static int read_backlight_val(struct msm_fb_data_type *mfd)
{
	unsigned char lp;
	int ret = 0;
	lp = mipi_dsi_read_reg(mfd, get_backlight_val);
	pr_debug("%s: read=%d\n", __func__, lp);
	return ret;
}
#endif

#if defined(CONFIG_YULONG_LCD_ESD) || defined(CONFIG_READ_BACKLIGHT)
uint8 monitor_reg_status(struct msm_fb_data_type *mfd, char mode)
{
	uint32 data = 0;
	uint16 cnt = 0, ret = 0;

	/* entry hs power mode */
	mipi_set_tx_power_mode(0);
	cnt = 0;
	while (cnt++ < 10000) {
		data = MIPI_INP(MIPI_DSI_BASE + 0x0004); /* DSI1_STATUS */
		if ((data & 0x0010) == 0) /* out of bta busy */
			break;
	}
	mipi_dsi_ack_err_status();
	mipi_dsi_cmd_mdp_busy();
	ret = MIPI_INP(MIPI_DSI_BASE + 0x0004); /* DSI_STATUS */
	if (!(ret & 0x08)) /* VIDEO_MODE_ENGINE_BUSY */
		usleep(4000);
	switch (mode) {
	case 0:
#ifdef CONFIG_YULONG_LCD_ESD
		ret = read_panel_reg(mfd);
#endif
		break;
	case 1:
#ifdef CONFIG_READ_BACKLIGHT
		ret = read_backlight_val(mfd);
#endif
		break;
	default:
		pr_err("%s: error mode\n", __func__);
		break;
	}

	/* entry high speed mode */
	mipi_set_tx_power_mode(1);

	return ret;
}
#endif

static bool first_bl;
static int mipi_global_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;

	if (mfd->key != MFD_KEY)
		return -EINVAL;
	mipi  = &mfd->panel_info.mipi;

	pr_debug("%s: enter\n", __func__);
	msleep(10);
	mipi_dsi_cmds_tx(&mipi_global_tx_buf, mipi->dsi_on_cmd,
		mipi->count_on_cmds);
	pr_debug("%s: count_on_cmds=%d\n", __func__, mipi->count_on_cmds);
	return 0;
}

int mipi_global_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	pr_debug("%s: enter\n", __func__);

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	mipi  = &mfd->panel_info.mipi;

	mipi_dsi_cmds_tx(&mipi_global_tx_buf, mipi->dsi_off_cmd,
		mipi->count_off_cmds);

	pr_debug("%s: count_off_cmds=%d\n", __func__,  mipi->count_off_cmds);
	return 0;
}

DEFINE_LED_TRIGGER(bkl_led_trigger);
static void mipi_global_set_backlight(struct msm_fb_data_type *mfd)
{
	if ((mipi_global_pdata->enable_wled_bl_ctrl)
	    && (wled_trigger_initialized)) {
		led_trigger_event(bkl_led_trigger, mfd->bl_level);
		return;
	}
	if (!first_bl && ((mfd->pdev->id >> 8) & 0x0f)
			== ORISE9806_YASHI_450_ID) {
		first_bl = true;
		msleep(50);
	}
#ifdef CONFIG_READ_BACKLIGHT
	mutex_lock(&mfd->set_backlight);
#endif
	mutex_lock(&mfd->dma->ov_mutex);
	hx8369_boyi_405_video13[1] = mfd->bl_level;
	pr_debug("%s: level = %d\n", __func__, hx8369_boyi_405_video13[1]);
	mipi_set_tx_power_mode(0);
	mipi_dsi_cmds_tx(&mipi_global_tx_buf, set_backlight, 1);
	mipi_set_tx_power_mode(1);
	mutex_unlock(&mfd->dma->ov_mutex);
#ifdef CONFIG_READ_BACKLIGHT
	msleep(5);
	monitor_reg_status(mfd, 1);
	mutex_unlock(&mfd->set_backlight);
#endif
}

static struct msm_fb_panel_data mipi_global_panel_data = {
	.on = mipi_global_lcd_on,
	.off = mipi_global_lcd_off,
	.set_backlight = mipi_global_set_backlight,
};

#ifdef SYSFS_DEBUG_ENABLE

static uint32 reg_read;
static uint32 reg_count;

static ssize_t windy_read_cmd(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	int i, j, valcount;
	struct dsi_cmd_desc *cmd_desc = NULL;
	struct msm_panel_info *pinfo;
	pinfo = &mipi_global_panel_data.panel_info;
	cmd_desc = pinfo->mipi.dsi_on_cmd;
	valcount = pinfo->mipi.count_on_cmds;
	buf[0] = 0;

	for (i = 0; i < reg_read; i++)
		cmd_desc++;

	ret += snprintf(buf+ret, 255-ret, "reg index%d=", reg_read);

	for (j = 0; j < cmd_desc->dlen; j++)
		ret += snprintf(buf+ret, 255-ret, " 0x%x",
					 cmd_desc->payload[j]);

	ret += snprintf(buf+ret, 255-ret, "\n");
	pr_debug("windy added log reg index=%d, rc=%d, dlen=%d, ret=%d\n",
		reg_read, reg_count, cmd_desc->dlen, ret);

	return ret;
}


static ssize_t windy_write_reg(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	uint32 reg, cnt;
	int rc = 0;

	rc = sscanf(buf, "%x %d", &reg, &cnt);
	if (rc == 2) {
		reg_read = reg;
		reg_count = cnt;
		pr_debug("%s: windy added log reg=%x count=%d\n",
			__func__, reg_read, reg_count);
		ret = rc;
	} else
		ret = -EINVAL;

	return ret;
}

static ssize_t windy_write_val(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 3;
	uint32 val[32];
	int rc = 0;
	struct dsi_cmd_desc *cmd_desc = NULL;
	int i, j, valcount = 1;
	struct msm_panel_info *pinfo;
	pinfo = &mipi_global_panel_data.panel_info;

	rc = sscanf(buf, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x "
		"%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
		&val[0], &val[1], &val[2], &val[3],
		&val[4], &val[5], &val[6], &val[7],
		&val[8], &val[9], &val[10], &val[11],
		&val[12], &val[13], &val[14], &val[15],
		&val[16], &val[17], &val[18], &val[19],
		&val[20], &val[21], &val[22], &val[23],
		&val[24], &val[25], &val[26], &val[27],
		&val[28], &val[29], &val[30], &val[31]);

	if (rc == reg_count) {
		ret = rc;
		pr_debug("%s: windy added log reg=0x%x count=%d rc=%d\n",
			__func__, reg_read, reg_count, rc);

		cmd_desc = pinfo->mipi.dsi_on_cmd;
		valcount = pinfo->mipi.count_on_cmds;

		for (i = 0; i < reg_read; i++)
			cmd_desc++;
		pr_debug("windy added log reg index=%d, rc=%d, dlen=%d\n",
			reg_read, reg_count, cmd_desc->dlen);
		if (reg_count > cmd_desc->dlen) {
			ret = -EINVAL;
			return ret;
		}

		for (j = 0; j < reg_count; j++)
			cmd_desc->payload[j] = val[j];
	} else
		ret = -EINVAL;
	return ret;
}

static ssize_t windy_write_porch(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 0;
	uint32 hbp, hfp, hpw, vbp, vfp, vpw;
	int rc = 0;
	struct msm_panel_info *pinfo;
	struct platform_device *pdev;
	struct msm_fb_data_type *mfd;
	struct fb_info *fbi;
	struct fb_var_screeninfo *var;

	pdev = container_of(dev, struct platform_device, dev);
	mfd = platform_get_drvdata(pdev);
	fbi = mfd->fbi;
	var = &fbi->var;
	pinfo = &mipi_global_panel_data.panel_info;

	rc = sscanf(buf, "%d %d %d %d %d %d",
		&hbp, &hfp, &hpw, &vbp, &vfp, &vpw);
	pr_debug("windy added log rc=%d, %d %d %d %d %d %d\n",
			rc, hbp, hfp, hpw, vbp, vfp, vpw);

	if (rc == 6) {
		var->left_margin = hbp;
		var->right_margin = hfp;
		var->upper_margin = vbp;
		var->lower_margin = vfp;
		var->hsync_len = hpw;
		var->vsync_len = vpw;
		pinfo->lcdc.h_back_porch = hbp;
		pinfo->lcdc.h_front_porch = hfp;
		pinfo->lcdc.h_pulse_width = hpw;
		pinfo->lcdc.v_back_porch =  vbp;
		pinfo->lcdc.v_front_porch = vfp;
		pinfo->lcdc.v_pulse_width = vpw;

		ret = rc;
	} else
		ret = -EINVAL;

	return ret;
}

static ssize_t windy_write_regulator(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 3;
	uint32 val[5];
	int rc = 0;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;
	int i;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->regulator;
	rc = sscanf(buf, "%x %x %x %x %x",
		&val[0], &val[1], &val[2], &val[3], &val[4]);

	if (rc == 5) {
		for (i = 0; i < 5; i++)
			*regulator++ = val[i];
	} else
		ret = -EINVAL;

	return ret;
}

static ssize_t windy_read_regulator(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	int i;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->regulator;
	buf[0] = 0;
	ret += snprintf(buf+ret, 255-ret, "regulator=");

	for (i = 0; i < 5; i++)
		ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[i]);
	ret += snprintf(buf+ret, 255-ret, "\n");

	return ret;
}

static ssize_t windy_write_timing(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 12;
	uint32 val[12];
	int rc = 0;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;
	int i;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->timing;
	rc = sscanf(buf, "%x %x %x %x %x %x %x %x %x %x %x %x",
		&val[0], &val[1], &val[2], &val[3], &val[4], &val[5], &val[6],
		&val[7], &val[8], &val[9], &val[10], &val[11]);

	if (rc == 12) {
		for (i = 0; i < 12; i++)
			*regulator++ = val[i];
	} else
		ret = -EINVAL;

	return ret;
}

static ssize_t windy_read_timing(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	int i;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->timing;
	buf[0] = 0;
	ret += snprintf(buf+ret, 255-ret, "regulator=");

	for (i = 0; i < 12; i++)
		ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[i]);
	ret += snprintf(buf+ret, 255-ret, "\n");

	return ret;
}

static ssize_t windy_write_pll(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	ssize_t ret = 3;
	uint32 val[21];
	int rc = 0;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->pll;
	rc = sscanf(buf, "%x %x %x %x %x %x",
		&val[0], &val[1], &val[2], &val[3], &val[4], &val[5]);

	if (rc == 6) {
		regulator[1] = val[0];
		regulator[2] = val[1];
		regulator[3] = val[2];
		regulator[8] = val[3];
		regulator[9] = val[4];
		regulator[10] = val[5];
	} else
		ret = -EINVAL;

	return ret;
}

static ssize_t windy_read_pll(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	unsigned int *regulator;
	struct msm_panel_info *pinfo;

	pinfo = &mipi_global_panel_data.panel_info;
	regulator = pinfo->mipi.dsi_phy_db->pll;
	buf[0] = 0;

	ret += snprintf(buf+ret, 255-ret, "regulator=");
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[1]);
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[2]);
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[3]);
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[8]);
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[9]);
	ret += snprintf(buf+ret, 255-ret, " 0x%x", regulator[10]);
	ret += snprintf(buf+ret, 255-ret, "\n");

	return ret;
}
static DEVICE_ATTR(morison_reg, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_cmd, windy_write_reg);
static DEVICE_ATTR(morison_val, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_cmd, windy_write_val);
static DEVICE_ATTR(morison_porch, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_cmd, windy_write_porch);
static DEVICE_ATTR(morison_regulator, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_regulator, windy_write_regulator);
static DEVICE_ATTR(morison_timing, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_timing, windy_write_timing);
static DEVICE_ATTR(morison_pll, S_IRUGO | S_IWUSR | S_IWGRP,
		windy_read_pll, windy_write_pll);

static struct attribute *fs_attrs[] = {
	&dev_attr_morison_reg.attr,
	&dev_attr_morison_val.attr,
	&dev_attr_morison_porch.attr,
	&dev_attr_morison_regulator.attr,
	&dev_attr_morison_timing.attr,
	&dev_attr_morison_pll.attr,
	NULL,
};

static struct attribute_group fs_attr_group = {
	.attrs = fs_attrs,
};
#endif

static int __devinit mipi_global_lcd_probe(struct platform_device *pdev)
{
#ifdef SYSFS_DEBUG_ENABLE
	struct platform_device *fb_dev;
	struct msm_fb_data_type *mfd;
	int rc;
#endif

	pr_info("%s: enter\n", __func__);

	if (pdev->id == 0) {
		mipi_global_pdata = pdev->dev.platform_data;
		return 0;
	}
#ifdef SYSFS_DEBUG_ENABLE
	fb_dev = msm_fb_add_device(pdev);
	mfd = platform_get_drvdata(fb_dev);
	rc = sysfs_create_group(&mfd->fbi->dev->kobj, &fs_attr_group);
	if (rc) {
		pr_err("%s: sysfs create group failed, rc=%d\n", __func__, rc);
		return rc;
	}

#else
	msm_fb_add_device(pdev);
#endif
	pr_info("%s: exit\n", __func__);
	return 0;
}

static struct platform_driver this_driver = {
	.probe = mipi_global_lcd_probe,
	.driver = {
		.name = "mipi_global",
	},
};
static int ch_used[3];

static int mipi_global_lcd_init(void)
{
	pr_debug("%s: enter\n", __func__);

	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_debug("%s: SUCCESS (WLED TRIGGER)\n", __func__);
	wled_trigger_initialized = 1;

	mipi_dsi_buf_alloc(&mipi_global_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&mipi_global_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

int mipi_global_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	pr_debug("%s: enter\n", __func__);

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_global_lcd_init();
	if (ret) {
		pr_err("mipi_global_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_global", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	mipi_global_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &mipi_global_panel_data,
				sizeof(mipi_global_panel_data));
	if (ret) {
		pr_debug("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_debug("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}
	pr_debug("%s: exit\n", __func__);

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}
