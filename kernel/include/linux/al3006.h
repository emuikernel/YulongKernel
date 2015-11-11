/*
 *  al3006.h - Definitions for al3006 ALS&Proxi chip
 *
 *  Copyright (C) 2010 Yulong Tech. Co., Ltd.
 *  Jay.HF <huangfujie@yulong.com>
 *  Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __AL3006_H__
#define __AL3006_H__

#include <linux/ioctl.h>

#define AL3006_DRIVER_NAME                   "al3006"
#define DRIVER_VERSION                       "1.1.1"
#define AL3006_VENDORID                      0x0001

/*
 * or 0x1DH depends voltage level on pin 4
 */
#define AL3006_I2C_ADDR                      0x1C

/*
 * GPIOs
 */
#define AL3006_IRQ_GPIO                      76
#define AL3006_3V_ENABLE                     107

/*
 * AL3006 register Address definitions
 */
#define AL3006_CONF                          0x00
#define AL3006_TIMING_CTL                    0x01
#define AL3006_ALS_CTL                       0x02
#define AL3006_INT_STATUS                    0x03
#define AL3006_PS_CTL                        0x04
#define AL3006_TATA                          0x05
#define AL3006_ALS_WINDOW                    0x08

/*
 * AL3006 IOCTL interface
 */
#define IOCTL_AL3006_MAGIC                   'B'
#define IOCTL_AL3006_GET_LIGHT_DATA \
	_IOWR(IOCTL_AL3006_MAGIC, 1, unsigned long)
#define IOCTL_AL3006_GET_PROMIXITY_DATA \
	_IOWR(IOCTL_AL3006_MAGIC, 2, unsigned long)
#define IOCTL_AL3006_SET_LIGHT_ACTIVATE \
	_IOWR(IOCTL_AL3006_MAGIC, 3, unsigned long)
#define IOCTL_AL3006_SET_PROMIXITY_ACTIVATE \
	_IOWR(IOCTL_AL3006_MAGIC, 4, unsigned long)
#define IOCTL_AL3006_SET_LIGHT_DELAY \
	_IOWR(IOCTL_AL3006_MAGIC, 5, unsigned long)
#define IOCTL_AL3006_SET_PROMIXITY_DELAY \
	_IOWR(IOCTL_AL3006_MAGIC, 6, unsigned long)
#define IOCTL_AL3006_ENABLE_PROXIMITY_SENSOR \
	_IOWR(IOCTL_AL3006_MAGIC, 7, unsigned long)
#define IOCTL_AL3006_ENABLE_LIGHT_SENSOR \
	_IOWR(IOCTL_AL3006_MAGIC, 8, unsigned long)
#define IOCTL_AL3006_ENABLE_TEST_MODE \
	_IOWR(IOCTL_AL3006_MAGIC, 9, unsigned long)
#define IOCTL_AL3006_DISABLE_TEST_MODE \
	_IOWR(IOCTL_AL3006_MAGIC, 10, unsigned long)

#define MAX_ERROR_COUNT                      8

/*
 * dynamic major by default
 */
#ifndef CSENSOR_MAJOR
#define CSENSOR_MAJOR                        0
#endif

/*
 * Number of ltr502 device
 */
#ifndef CSENSOR_NR_AL3006_DEVS
#define CSENSOR_NR_AL3006_DEVS               2
#endif

/*
 * light snesor data
 */
typedef struct _dls {
	int light;
} ALSDATA;

/*
 * proximity snesor data
 */
typedef struct _dps {
	int proximity;
} PSDATA;

/*
 * al3006 platform data
 */
struct al3006_platform_data {
	int setup;
	int teardown;
};

#endif

