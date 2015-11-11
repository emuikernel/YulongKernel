/*
 *  taos.h - definitions for TAOS ALS&Prox Chip
 *
 *  Copyright (C) 2010 Yulong Tech. Co., Ltd.
 *  Jay.HF <huangfujie@yulong.com>
 *  Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TAOS_H__
#define __TAOS_H__

#include <linux/alsprox_common.h>

#define TAOS_DRIVER_NAME                ALSPROX_DRIVER_NAME
#define TAOS_DEVICE_NAME                ALSPROX_DRIVER_NAME
#define TAOS_INPUT_NAME                 ALSPROX_INPUT_NAME

#define TAOS_ID_NAME_SIZE               10
#define TAOS_GPIO_IRQ                   49
#define TAOS_TRITON_CHIPIDVAL           0x00
#define TAOS_TRITON_MAXREGS             32
#define TAOS_DEVICE_ADDR1               0x29
#define TAOS_DEVICE_ADDR2               0x39
#define TAOS_DEVICE_ADDR3               0x49
#define TAOS_MAX_NUM_DEVICES            3
#define TAOS_MAX_DEVICE_REGS            32
#define I2C_MAX_ADAPTERS                8

/*
 * register && command define
 * TRITON register offsets
 */
#define TAOS_TRITON_CNTRL               0x00
#define TAOS_TRITON_ALS_TIME            0x01
#define TAOS_TRITON_PRX_TIME            0x02
#define TAOS_TRITON_WAIT_TIME           0x03
#define TAOS_TRITON_ALS_MINTHRESHLO     0x04
#define TAOS_TRITON_ALS_MINTHRESHHI     0x05
#define TAOS_TRITON_ALS_MAXTHRESHLO     0x06
#define TAOS_TRITON_ALS_MAXTHRESHHI     0x07
#define TAOS_TRITON_PRX_MINTHRESHLO     0x08
#define TAOS_TRITON_PRX_MINTHRESHHI     0x09
#define TAOS_TRITON_PRX_MAXTHRESHLO     0x0A
#define TAOS_TRITON_PRX_MAXTHRESHHI     0x0B
#define TAOS_TRITON_INTERRUPT           0x0C
#define TAOS_TRITON_PRX_CFG             0x0D
#define TAOS_TRITON_PRX_COUNT           0x0E
#define TAOS_TRITON_GAIN                0x0F
#define TAOS_TRITON_REVID               0x11
#define TAOS_TRITON_CHIPID              0x12
#define TAOS_TRITON_STATUS              0x13
#define TAOS_TRITON_ALS_CHAN0LO         0x14
#define TAOS_TRITON_ALS_CHAN0HI         0x15
#define TAOS_TRITON_ALS_CHAN1LO         0x16
#define TAOS_TRITON_ALS_CHAN1HI         0x17
#define TAOS_TRITON_PRX_LO              0x18
#define TAOS_TRITON_PRX_HI              0x19
#define TAOS_TRITON_TEST_STATUS         0x1F

/*
 * Triton cmd reg masks
 */
#define TAOS_TRITON_CMD_REG             0x80
#define TAOS_TRITON_CMD_AUTO            0x20
#define TAOS_TRITON_CMD_BYTE_RW         0x00
#define TAOS_TRITON_CMD_WORD_BLK_RW     0x20
#define TAOS_TRITON_CMD_SPL_FN          0x60
#define TAOS_TRITON_CMD_PROX_INTCLR     0x05
#define TAOS_TRITON_CMD_ALS_INTCLR      0x06
#define TAOS_TRITON_CMD_PROXALS_INTCLR  0x07
#define TAOS_TRITON_CMD_TST_REG         0x08
#define TAOS_TRITON_CMD_USER_REG        0x09

/*
 * Triton cntrl reg masks
 */
#define TAOS_TRITON_CNTL_PROX_INT_ENBL  0x20
#define TAOS_TRITON_CNTL_ALS_INT_ENBL   0x10
#define TAOS_TRITON_CNTL_WAIT_TMR_ENBL  0x08
#define TAOS_TRITON_CNTL_PROX_DET_ENBL  0x04
#define TAOS_TRITON_CNTL_ADC_ENBL       0x02
#define TAOS_TRITON_CNTL_PWRON          0x01

/*
 * Triton status reg masks
 */
#define TAOS_TRITON_STATUS_ADCVALID     0x01
#define TAOS_TRITON_STATUS_PRXVALID     0x02
#define TAOS_TRITON_STATUS_ADCINTR      0x10
#define TAOS_TRITON_STATUS_PRXINTR      0x20

/*
 * lux constants
 */
#define TAOS_MAX_LUX                    10000
#define TAOS_SCALE_MILLILUX             3
#define TAOS_FILTER_DEPTH               3
#define CHIP_ID                         0x29

/*
 * SWITCHER
 */
#define TAOS_FILTER_SWITCH              0
#define TAOS_ALGO_OPTIMIZE              0 /* "0":OPTIMIZE "1":NOT */

/*
 * ioctl numbers
 */
#define TAOS_IOCTL_ALS_ON               ALSPROX_IOCTL_ALS_ON
#define TAOS_IOCTL_ALS_OFF              ALSPROX_IOCTL_ALS_OFF
#define TAOS_IOCTL_ALS_OFFSET           ALSPROX_IOCTL_ALS_OFFSET
#define TAOS_IOCTL_ALS_CALIBRATE        ALSPROX_IOCTL_ALS_CALIBRATE
#define TAOS_IOCTL_PROX_ON              ALSPROX_IOCTL_PROX_ON
#define TAOS_IOCTL_PROX_OFF             ALSPROX_IOCTL_PROX_OFF
#define TAOS_IOCTL_PROX_OFFSET          ALSPROX_IOCTL_PROX_OFFSET
#define TAOS_IOCTL_PROX_CALIBRATE       ALSPROX_IOCTL_PROX_CALIBRATE
#define TAOS_IOCTL_PHONE_STATE          ALSPROX_IOCTL_PHONE_STATE

/*
 * device configuration
 */
struct taos_cfg {
	unsigned int   calibrate_target;
	unsigned short als_time;
	unsigned short scale_factor;
	unsigned short gain_trim;
	unsigned char  filter_history;
	unsigned char  filter_count;
	unsigned char  gain;
	unsigned short prox_threshold_hi;
	unsigned short prox_threshold_lo;
	unsigned short als_threshold_hi;
	unsigned short als_threshold_lo;
	unsigned char  prox_int_time;
	unsigned char  prox_adc_time;
	unsigned char  prox_wait_time;
	unsigned char  prox_intr_filter;
	unsigned char  prox_config;
	unsigned char  prox_pulse_cnt;
	unsigned char  prox_gain;
};

/*
 * proximity data
 */
struct taos_prox_info {
	unsigned short prox_clear;
	unsigned short prox_data;
	int prox_event;
};

#endif
