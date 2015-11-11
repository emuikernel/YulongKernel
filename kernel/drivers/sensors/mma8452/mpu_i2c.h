/*
 *  mpu_i2c.h - The driver is currently for LTE8930.
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

#ifndef MPU_I2C_H
#define MPU_I2C_H

#include <linux/i2c.h>

/*
 * regs
 */
#define MPU_USER_CONTROL_REG   0x3D
#define MPU_POWER_MANAGE_REG   0x3E

/*
 * address
 */
#define MPU_I2C_ADDRESS        0x68
#define MPU_I2C_ADDRESS_HIGH   0x69

/*
 * config
 */
#define MPU_CTL_DEFAULT        0x0E
#define MPU_CTL_DMP_EN         0x80
#define MPU_CTL_FIFO_EN        0x40
#define MPU_CTL_AUX_EN         0x20
#define MPU_CTL_AUX_RST        0x08
#define MPU_CTL_MPU_RST        0x04    
#define MPU_CTL_FIFO_RST       0x02    
#define MPU_CTL_GYRO_RST       0x01
    
#define MPU_PWR_DEFAULT        0x78


int i2c_write(struct i2c_adapter *i2c_adap,
              unsigned char address,
              unsigned int  len, 
              unsigned char const *data);

int i2c_write_reg(struct i2c_adapter *i2c_adap,
              unsigned char address,
              unsigned char reg, 
              unsigned char value);

int i2c_read_reg(struct i2c_adapter *i2c_adap,
              unsigned char address,
              unsigned char reg,
              unsigned char *value);
#endif

