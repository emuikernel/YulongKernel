/*
 *  mpu_i2c.c - The driver is currently for LTE8930.
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

#include <linux/delay.h>
#include <linux/i2c.h>
#include "mpu_i2c.h"

int i2c_write(struct i2c_adapter *i2c_adap,
              unsigned char address,
              unsigned int len, unsigned char const *data)
{
    int res;
    struct i2c_msg msgs[1];

    if (NULL == data || NULL == i2c_adap)
        return -EINVAL;

    msgs[0].addr = address;
    msgs[0].flags = 0;    /* write */
    msgs[0].buf = (unsigned char *) data;
    msgs[0].len = len;

    res = i2c_transfer(i2c_adap, msgs, 1);
    if (res < 1) {
        return res;
    }
    else
    {
        return 0;
    }
}

int i2c_write_reg(struct i2c_adapter *i2c_adap,
                  unsigned char address,
                  unsigned char reg, 
                  unsigned char value)
{
    int res = 0;
    int count = 0;
    unsigned char data[2];

    data[0] = reg;
    data[1] = value;

    do{
        count++;
        res = i2c_write(i2c_adap, address, 2, data);
        if (res) {
            mdelay(1);
            if (count > 4) {
                printk(KERN_ERR "[%s]: fail \r\n",__func__);
            }
        }
    }while(res && (count < 5));

    return res;
}

int i2c_read_reg(struct i2c_adapter *i2c_adap,
                 unsigned char address,
                 unsigned char reg,
                 unsigned char *value)
{
    struct i2c_msg msgs[2];
    unsigned char data[2];
    int res;

    if (NULL == i2c_adap)
        return -EINVAL;

    msgs[0].addr = address;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = data;

    res = i2c_transfer(i2c_adap, msgs, 2);
    if (res >= 0) {
        *value = data[0];
        return 0;
    }    
    else
    {
        printk(KERN_ERR "[%s]: fail \r\n", __func__);
        return -1;
    }
}

