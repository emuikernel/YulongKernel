/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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

#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include "devices.h"
#include "board-8930.h"

/*===================================================
module: HDMI
function:gpio config
software version:17330
hardware version:P1
author:chenzhijun
day:20130321
location:start
===================================================*/
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct gpiomux_setting hdmi_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting hdmi_active_1_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting hdmi_active_2_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting hdmi_active_3_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting hdmi_active_4_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting hdmi_active_5_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_OUT_HIGH,
};
#endif
/*================location end=====================*/


/*===================================================
module:lcd
function:gpio config
software version:1230
hardware version:P0
author:yankelong
day:20121123
location:start
===================================================*/
static struct gpiomux_setting lcd_rst_n_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting lcd_rst_n_suspend = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.dir = GPIOMUX_OUT_HIGH,
};
#ifdef CONFIG_BOARD_QC1
static struct gpiomux_setting mdp_vsync_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting mdp_vsync_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};
#endif
static struct msm_gpiomux_config msm8960_lcd_config[] __initdata = {
	{
		.gpio	= 58,		/* slimbus data */
		.settings = {
			[GPIOMUX_SUSPENDED] = &lcd_rst_n_suspend,
			[GPIOMUX_ACTIVE] = &lcd_rst_n_active,
		},
	},
#ifdef CONFIG_BOARD_QC1
	{
		.gpio = 0, /*Fmark*/
		.settings = {
			[GPIOMUX_ACTIVE]    = &mdp_vsync_active_cfg,
			[GPIOMUX_SUSPENDED] = &mdp_vsync_suspend_cfg,
		},
	},
#endif
};
/*================location end=====================*/


/*===================================================
module:sensor
function:gpio config
software version:1230
hardware version:P0
author:huangfujie
day:20121123
location:start
===================================================*/
static struct gpiomux_setting sensor_irq_suspended = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	/* modify by Jay.HF 2012-12-04 from GPIOMUX_PULL_DOWN*/
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting sensor_irq_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

#ifdef CONFIG_NFC_PN544
static struct gpiomux_setting nfc_suspended = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting nfc_active = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};
#endif

static struct gpiomux_setting gsbi12_suspended_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_KEEPER,
};
static struct gpiomux_setting gsbi12_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm8930_sensor_irq_config[] __initdata = {
	{
		.gpio = 49,	/* ALS_PS_INT_N */
		.settings = {
			[GPIOMUX_SUSPENDED] = &sensor_irq_suspended,
			[GPIOMUX_ACTIVE] = &sensor_irq_active,
		},
	},
#ifdef CONFIG_NFC_PN544
	{
		.gpio = 6,
		.settings = {
			[GPIOMUX_SUSPENDED] = &nfc_suspended,
			[GPIOMUX_ACTIVE] = &nfc_active,
		},
	},
	{
		.gpio = 8,
		.settings = {
			[GPIOMUX_SUSPENDED] = &nfc_suspended,
			[GPIOMUX_ACTIVE] = &nfc_active,
		},
	},
	{
		.gpio = 35,
		.settings = {
			[GPIOMUX_SUSPENDED] = &nfc_suspended,
			[GPIOMUX_ACTIVE] = &nfc_active,
		},
	},
#endif
};

static struct msm_gpiomux_config msm8960_gsbi12_configs[] __initdata = {

	{
		.gpio      = 44,	/* GSBI12 I2C QUP SDA */
		.settings = {
			/* sensor i2c bus */
			[GPIOMUX_SUSPENDED] = &gsbi12_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi12_active_cfg,
		},
	},
	{
		.gpio      = 45,	/* GSBI12 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi12_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi12_active_cfg,
		},
	},
};
/*================location end=====================*/


/*===================================================
module:touch
function:gpio config
software version:1230
hardware version:P0
author:panhuangduan
day:20121123
location:start
===================================================*/
static struct gpiomux_setting atmel_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting atmel_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting atmel_resout_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting atmel_resout_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting gsbi3_suspended_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting gsbi3_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm8960_atmel_configs[] __initdata = {
	{	/* TS INTERRUPT */
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = &atmel_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &atmel_int_sus_cfg,
		},
	},
	{	/* TS RESOUT */
		.gpio = 52,
		.settings = {
			[GPIOMUX_ACTIVE]    = &atmel_resout_act_cfg,
			[GPIOMUX_SUSPENDED] = &atmel_resout_sus_cfg,
		},
	},
};

static struct msm_gpiomux_config msm8960_gsbi3_configs[] __initdata = {
	{
		.gpio      = 16,	/* GSBI3 I2C QUP SDA */
		.settings = {           /* touchscreen i2c bus */
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
	{
		.gpio      = 17,	/* GSBI3 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
};
/*================location end=====================*/


/*===================================================
module:audio
function:gpio config
software version:1230
hardware version:P0
author:youjin
day:20121123
location:start
===================================================*/

static struct gpiomux_setting cdc_mclk = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting audio_mbhc = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting slimbus = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting sitar_reset = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

static struct msm_gpiomux_config msm8960_audio_mbhc_configs[] __initdata = {
	{
		.gpio = 37,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_mbhc,
		},
	}
};

static struct msm_gpiomux_config msm8960_audio_codec_configs[] __initdata = {
	{
		.gpio = 59,
		.settings = {
			[GPIOMUX_SUSPENDED] = &cdc_mclk,
		},
	}
};

static struct msm_gpiomux_config msm_sitar_config[] __initdata = {
	{
		.gpio = 42,           /* SYS_RST_N */
		.settings = {
			[GPIOMUX_SUSPENDED] = &sitar_reset,
		},
	}
};

static struct msm_gpiomux_config msm8960_slimbus_config[] __initdata = {
	{
		.gpio	= 60,		/* slimbus data */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
	{
		.gpio	= 61,		/* slimbus clk */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
};
/*================location end=====================*/


/*===================================================
module:wcnss
function:gpio config
software version:1230
hardware version:P0
author:zhangxiaobin
day:20121123
location:start
===================================================*/
static struct gpiomux_setting wcnss_5wire_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting wcnss_5wire_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv  = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config wcnss_5wire_interface[] = {

	{
		.gpio = 84,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 85,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 86,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 87,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 88,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
};
/*================location end=====================*/


/*===================================================
module:uart serial
function:gpio config
software version:1230
hardware version:P0
author:fengchunsong
day:20121123
location:start
===================================================*/
static struct gpiomux_setting gsbi_uart = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};
#ifdef CONFIG_NFC_PN544
static struct gpiomux_setting gsbi5_nfc_suspended_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_KEEPER,
};
static struct gpiomux_setting gsbi5_nfc_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};
#endif
static struct msm_gpiomux_config msm8960_gsbi_configs[] __initdata = {
	{
		.gpio      = 22,	/* GSBI5 UART2 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi_uart,
		},
	},
	{
		.gpio      = 23,	/* GSBI5 UART2 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi_uart
		},
	},
#ifdef CONFIG_NFC_PN544
	{
		.gpio      = 24,        /* GSBI5 I2C QUP SDA */
		.settings = {
			/*the same config with sersor i2c bus */
			[GPIOMUX_SUSPENDED] = &gsbi5_nfc_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi5_nfc_active_cfg,
		},
	},
	{
		.gpio      = 25,        /* GSBI5 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi5_nfc_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi5_nfc_active_cfg,
		},
	},
#endif

};
/*================location end=====================*/


/*===================================================
module:camera
function:gpio config
software version:1230
hardware version:P0
author:pengyong
day:20121123
location:start
===================================================*/
/*
 * All of gpios configuration are put in board-8930-camera.c
 * gpios as follows:
 * gpio_9  gpio_10 gpio_20 gpio_21
 * gpio_54 gpio_76 gpio_107
 */
/*================location end=====================*/


/*===================================================
module:sdcard
function:gpio config
software version:1230
hardware version:P0
author:TigerHuang
day:20121220
location:start
===================================================*/
static struct gpiomux_setting sd_det_line = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
#if defined(CONFIG_BOARD_MTR3) || defined(CONFIG_BOARD_QC1)
	.pull = GPIOMUX_PULL_DOWN,
#else
	.pull = GPIOMUX_PULL_UP,
#endif
};

static struct msm_gpiomux_config msm8960_sd_det_config[] __initdata = {
	{
		.gpio = 94,		/* SD Card Detect Line */
		.settings = {
			[GPIOMUX_SUSPENDED] = &sd_det_line,
			[GPIOMUX_ACTIVE] = &sd_det_line,
		},
	},
};
/*================location end=====================*/

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct msm_gpiomux_config msm8960_hdmi_configs[] __initdata = {
	{
		.gpio = 99,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 100,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 101,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 102,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_2_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},

};

static struct msm_gpiomux_config msm8930_mhl_configs[] __initdata = {
	{
		.gpio = 72,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_3_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 71,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_4_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 73,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_5_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},

};
#endif

/*===================================================
module:unuse gpio
function:gpio config
software version:1230
hardware version:P0
author:fengchunsong
day:20121123
location:start
===================================================*/
static struct gpiomux_setting unused_gpio_pins = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

/* Unused pins. Masked it here first before you use it */
static struct msm_gpiomux_config
		msm8930_unused_gpio_pins_config[] __initdata = {
	{.gpio = 1,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 7,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#ifndef CONFIG_NFC_PN544
	{.gpio = 6,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 8,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#endif
	{.gpio = 12,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,	},
	{.gpio = 13,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 14,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 15,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 18,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 19,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#ifndef CONFIG_NFC_PN544
	{.gpio = 24,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 25,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#endif
	{.gpio = 26,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 27,    .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 33,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 34,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#ifndef CONFIG_NFC_PN544
	{.gpio = 35,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
#endif
	{.gpio = 36,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 43,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 46,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 47,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 48,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 50,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 51,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 55,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 63,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 64,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 65,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 66,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 67,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 68,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 69,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 70,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 71,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 72,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 73,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 74,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 75,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 77,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 78,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 79,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 80,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 81,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 82,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 89,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 90,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 91,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 92,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 93,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 95,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 96,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 97,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 99,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 100,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 101,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 102,   .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
	{.gpio = 106,  .settings[GPIOMUX_SUSPENDED] =  &unused_gpio_pins,},
};
/*================location end=====================*/

int __init msm8930_init_gpiomux(void)
{
	int rc = msm_gpiomux_init(NR_GPIO_IRQS);
	if (rc) {
		pr_err(KERN_ERR "msm_gpiomux_init failed %d\n", rc);
		return rc;
	}

	msm_gpiomux_install(msm8930_unused_gpio_pins_config,
			ARRAY_SIZE(msm8930_unused_gpio_pins_config));

	msm_gpiomux_install(msm8960_gsbi_configs,
			ARRAY_SIZE(msm8960_gsbi_configs));

	msm_gpiomux_install(msm8960_gsbi12_configs,
			ARRAY_SIZE(msm8960_gsbi12_configs));

		msm_gpiomux_install(msm8960_gsbi3_configs,
			ARRAY_SIZE(msm8960_gsbi3_configs));

	msm_gpiomux_install(msm8960_atmel_configs,
			ARRAY_SIZE(msm8960_atmel_configs));

	msm_gpiomux_install(msm8960_slimbus_config,
			ARRAY_SIZE(msm8960_slimbus_config));

	msm_gpiomux_install(msm8960_audio_codec_configs,
			ARRAY_SIZE(msm8960_audio_codec_configs));

	msm_gpiomux_install(msm8960_audio_mbhc_configs,
			ARRAY_SIZE(msm8960_audio_mbhc_configs));

	msm_gpiomux_install(wcnss_5wire_interface,
			ARRAY_SIZE(wcnss_5wire_interface));

	if (machine_is_msm8930_mtp() || machine_is_msm8930_fluid() ||
		machine_is_msm8930_cdp()) {
#if 0 /* fengchunsong gpio47 NC */
		msm_gpiomux_install(hap_lvl_shft_config,
			ARRAY_SIZE(hap_lvl_shft_config));
#endif

	}

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	msm_gpiomux_install(msm8960_hdmi_configs,
			ARRAY_SIZE(msm8960_hdmi_configs));
	if (machine_is_msm8930_fluid())
		msm_gpiomux_install(msm8930_mhl_configs,
			ARRAY_SIZE(msm8930_mhl_configs));

#endif

	msm_gpiomux_install(msm8960_lcd_config,
			ARRAY_SIZE(msm8960_lcd_config));

	msm_gpiomux_install(msm8930_sensor_irq_config,
			ARRAY_SIZE(msm8930_sensor_irq_config));

	msm_gpiomux_install(msm8960_sd_det_config,
			ARRAY_SIZE(msm8960_sd_det_config));

	msm_gpiomux_install(msm_sitar_config, ARRAY_SIZE(msm_sitar_config));

	return 0;
}
