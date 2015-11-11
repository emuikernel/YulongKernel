/*
 *  mma8452.h - The driver is currently for LTE8930.
 *              The software is for a 3-Axis Orientation/Acceleration Sensor.
 *              The driver provides ioctl and data input functions.
 *
 *  Copyright (C) 2010 Yulong Tech. Co., Ltd.
 *  Jay.HF <huangfujie@yulong.com>
 *  Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef MMA8452_H_
#define MMA8452_H_

#include <linux/ioctl.h>

#define assert(expr) { \
	if (!(expr)) { \
		printk(KERN_ERR "Assertion failed! %s, %d, %s, %s\n", \
			__FILE__, __LINE__, __func__, #expr); \
	} \
}

#define MMA8452_DRV_NAME             "mma8452"
#define MMA8452_I2C_ADDR             0x1C
#define MMA8452_ID                   0x2A
#define MMA8452_KEY                  0xAA01

#define POLL_INTERVAL_MAX            500
#define POLL_INTERVAL                200
#define INPUT_FUZZ                   32
#define INPUT_FLAT                   32
#define MODE_CHANGE_DELAY_MS         100

/*
 * calibriration parameters
 */
#define  ACC8452_METER_XOFF_INIT     0
#define  ACC8452_METER_YOFF_INIT     0
#define  ACC8452_METER_ZOFF_INIT     0

/*
 * (-5, +5)degree
 */
#define  ACC8452_METER_OFFSET_ALLOW  90

#define MAX_CAL_CNT                  20
#define CAL_FUZZ                     12
#define ACC_METER_PARAM_LIMITED      1

/*Define cmds for ioctl */
#define CMD_BASIC                    0xa0

/*
 * IOCTLs for MMA8452 library
 */
#define MMA8452_IOCTL_ACTIVE         _IOW(CMD_BASIC, 0x01, unsigned long)
#define MMA8452_IOCTL_SET_DELAY      _IOR(CMD_BASIC, 0x02, unsigned long)
#define MMA8452_IOCTL_SET_OFFSET     _IOW(CMD_BASIC, 0x03, unsigned long)
#define MMA8452_IOCTL_DO_CALIBRATION _IOW(CMD_BASIC, 0x04, unsigned long)

/*
 * MPU SWITCHER
 */
#define GSENSOR_WITH_MPU             0

/*
 * Define for HAL
 */
#define MMA_GSENSOR_FILE_PATH        "/data/misc/sensors/cal_mma8452"
#define MMA8452_DEVICE_NAME          "/dev/mma8452_dev"
#define MMA8452_INPUT_NAME           "mma8452"

/*
 * Define args for ioctl
 * register enum for mma8452 registers
 */
enum {
	MMA8452_STATUS = 0x00,
	MMA8452_OUT_X_MSB,
	MMA8452_OUT_X_LSB,
	MMA8452_OUT_Y_MSB,
	MMA8452_OUT_Y_LSB,
	MMA8452_OUT_Z_MSB,
	MMA8452_OUT_Z_LSB,

	MMA8452_SYSMOD = 0x0B,
	MMA8452_INT_SOURCE,
	MMA8452_WHO_AM_I,
	MMA8452_XYZ_DATA_CFG,
	MMA8452_HP_FILTER_CUTOFF,

	MMA8452_PL_STATUS,
	MMA8452_PL_CFG,
	MMA8452_PL_COUNT,
	MMA8452_PL_BF_ZCOMP,
	MMA8452_PL_P_L_THS_REG,

	MMA8452_FF_MT_CFG,
	MMA8452_FF_MT_SRC,
	MMA8452_FF_MT_THS,
	MMA8452_FF_MT_COUNT,

	MMA8452_TRANSIENT_CFG = 0x1D,
	MMA8452_TRANSIENT_SRC,
	MMA8452_TRANSIENT_THS,
	MMA8452_TRANSIENT_COUNT,

	MMA8452_PULSE_CFG,
	MMA8452_PULSE_SRC,
	MMA8452_PULSE_THSX,
	MMA8452_PULSE_THSY,
	MMA8452_PULSE_THSZ,
	MMA8452_PULSE_TMLT,
	MMA8452_PULSE_LTCY,
	MMA8452_PULSE_WIND,

	MMA8452_ASLP_COUNT,
	MMA8452_CTRL_REG1,
	MMA8452_CTRL_REG2,
	MMA8452_CTRL_REG3,
	MMA8452_CTRL_REG4,
	MMA8452_CTRL_REG5,

	MMA8452_OFF_X,
	MMA8452_OFF_Y,
	MMA8452_OFF_Z,

	MMA8452_REG_END,
};

enum {
	MODE_2G = 0,
	MODE_4G,
	MODE_8G,
};

/*******************************************************************
By John.Zhang 2011.7.26
mma_status.ctl_reg1 = 0x38;     //    1.56 HZ   ,  640.0  ms
mma_status.ctl_reg1 = 0x30;     //    6.25 HZ   ,  160.0  ms
mma_status.ctl_reg1 = 0x28;     //   12.5  HZ   ,   80.0  ms
mma_status.ctl_reg1 = 0x20;     //   50.0  HZ   ,   20.0  ms
mma_status.ctl_reg1 = 0x18;     //  100.0  HZ   ,   10.0  ms
mma_status.ctl_reg1 = 0x10;     //  200.0  HZ   ,    5.0  ms
mma_status.ctl_reg1 = 0x08;     //  400.0  HZ   ,    2.5  ms
mma_status.ctl_reg1 = 0x00;     //  800.0  HZ   ,    1.25 ms
******************************************************************/
enum {
	SPEED_TOP    = 0x00, /* 800    TOP    ,top line */
	SPEED_400    = 0x08, /* 400 */
	SPEED_HIGH   = 0x10, /* 200    HIGH   ,game */
	SPEED_100    = 0x18, /* 100 */
	SPEED_NORMAL = 0x20, /* 50     NORMAL ,g-sensor */
	SPEED_LOW    = 0x28, /* 10     LOW    ,compass */
	SPEED_5      = 0x30, /* 5 */
	SPEED_BOTTOM = 0x38, /* 1      BOTTOM ,bottom line */
};

/*
 * mma8452 offset
 */
struct mma8452_offset {
	unsigned short key;
	short x;
	short y;
	short z;
};

/*
 * mma8452 status
 */
struct mma8452_status {
	unsigned char mode;
	unsigned char ctl_reg1;
	short offset_x;
	short offset_y;
	short offset_z;
};

#endif
