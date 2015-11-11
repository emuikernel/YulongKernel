/*
 *  mma8452.c - The driver is currently for LTE8930.
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/input-polldev.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/hwmon.h>

// #include <../regulator.h>
#include <linux/regulator/consumer.h>
#include <linux/input-polldev.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/earlysuspend.h>
#include <linux/wakelock.h>
#include <linux/miscdevice.h>
#include <linux/mma8452.h>
#include "mpu_i2c.h"

#define MMA8452_PM_EN    0

static unsigned int acc_flag_resumed;
static unsigned int acc_enabled;
static unsigned int acc_flag_caling;
static unsigned int polling;
static struct mma8452_status mma_status = {0, 0, 0, 0, 0};
// static struct mma8452_offset mma_offset = {0, 0, 0, 0};

struct mma8452_data {
    struct i2c_client       *i2c_client;
    struct input_polled_dev *input_polled_dev;
    struct mutex            *lock;
    struct wake_lock         wake_lock;
    struct early_suspend     early_op;
};
static struct mma8452_data *mma8452_dev = NULL;

static bool power_state = false;
static struct regulator *pm8901_mvs0;
static struct regulator *pm8058_lvs1;

static int set_power(struct regulator *s, unsigned int voltage, bool enable)
{
#if MMA8452_PM_EN
    int err;
    if(enable) {
        if(0 != voltage) {
            err = regulator_set_voltage(s, voltage, voltage);
            if (err) {
                printk(KERN_ERR"BJ_BSP_DRIVER: CP_ACC: in %s: set_voltage failed \n", __func__);
                goto fail_s;
            }
        }
        err = regulator_enable(s);
        if (err) {
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s enable error\n", __func__);
            goto fail_s;
        }
    }
    else
    {
        err = regulator_disable(s);
        if (err) {
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s disable error\n", __func__);
            goto fail_s;
        }
    }

    return 0;
fail_s:
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s fail_s \n", __func__);
    return -1;
#else
    return 0;
#endif
}

static int mma8452_set_power(bool enable)
{
    int err = 0;

    if(enable != power_state) {
        pr_info(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s [%d] \r\n",__func__,enable);

        err = set_power(pm8901_mvs0,0,enable);
        if(err) goto err1;
        err = set_power(pm8058_lvs1,0,enable);
        if(err) goto err2;

        power_state = enable;

        if(enable) {
            mdelay(5);
        }
    }

    return 0;

err2:
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s err2 \n", __func__);
err1:
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s err1 \n", __func__);
    return -1;
}

#if GSENSOR_WITH_MPU
static int mma8452_setup_mpu3050(void)
{
    int res = 0, count = 0;
    unsigned char ctl_reg, pwr_reg;
    // printk(KERN_INFO"%s\n", __func__);

    do{
        count++;
        res = i2c_write_reg(mma8452_dev->i2c_client->adapter,
                            MPU_I2C_ADDRESS,
                            MPU_USER_CONTROL_REG,
                            MPU_CTL_DEFAULT);
        if (res) {
            mdelay(10);
        }
    }while(res && count < 10);

    if (count > 9) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s setup mpu3050 fail, mpu state not sure ! \n", __func__);
        return -1;
    }

    res = i2c_read_reg(mma8452_dev->i2c_client->adapter,
                       MPU_I2C_ADDRESS,
                       MPU_USER_CONTROL_REG,
                       &ctl_reg);
    if (res) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s read ctl_reg fail \n", __func__);    
    }
    else    
    {
        if (ctl_reg) {
            if (ctl_reg & 0x20) {
                printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s error, not bypass mode ! \n", __func__);
                return -1;
            }
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s yet ctl_reg may not suitable, but func is OK ! \n", __func__);
        }
    }

    res = i2c_write_reg(mma8452_dev->i2c_client->adapter,
                        MPU_I2C_ADDRESS,
                        MPU_POWER_MANAGE_REG,
                        MPU_PWR_DEFAULT);
    if (res) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set pwr_reg fail \r\n", __func__);
    }
    else
    {
        res = i2c_read_reg(mma8452_dev->i2c_client->adapter,
                           MPU_I2C_ADDRESS,
                           MPU_POWER_MANAGE_REG,
                           &pwr_reg);
        if (res) {
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s read pwr_reg fail \n", __func__);    
        }
        else
        {
            if (MPU_PWR_DEFAULT != pwr_reg) {
                printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s pwr_reg may not suitable, and power consume may higher ! \n", __func__);
            }
        }
    }

    return res;
}
#endif

static int mma8452_refresh_regs(void)
{
    int result = 0;
    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                       MMA8452_CTRL_REG1, mma_status.ctl_reg1 & 0xFE);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C clear active bit fail \r\n", __func__);
    }

    result  = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                        MMA8452_OFF_X, (unsigned char)mma_status.offset_x);
    result |= i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                        MMA8452_OFF_Y, (unsigned char)mma_status.offset_y);
    result |= i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                        MMA8452_OFF_Z, (unsigned char)mma_status.offset_z);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write offset regs error \r\n", __func__);
    }

    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_XYZ_DATA_CFG, mma_status.mode);
    result = i2c_smbus_read_byte_data(mma8452_dev->i2c_client, MMA8452_XYZ_DATA_CFG);
    if ((result&0xFF) != mma_status.mode) {
            pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set mode fail \r\n", __func__);
            return -1;
    }

    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1, mma_status.ctl_reg1);
    result = i2c_smbus_read_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1);
    if ((result&0xFF) != mma_status.ctl_reg1) {
            pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set ctl_reg1 fail \r\n", __func__);
            return -1;
    }

    mdelay(5);
    return 0;
}

#if 0
static int mma8452_clear_regs(void)
{
    int result = 0;

    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_XYZ_DATA_CFG, 0x00);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write mode_reg error \r\n", __func__);
        return result;
    }
    
    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1, mma_status.ctl_reg1 & 0xFE);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write ctl_reg1 error \r\n", __func__);
        return result;
    }

    return result;
}
#endif

static int mma8452_init_regs(void)
{
    int result = 0;

    mma_status.ctl_reg1  = SPEED_LOW;
    mma_status.mode      = MODE_2G;
    mma_status.ctl_reg1 |= 0x01;
    result = mma8452_refresh_regs();
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C refresh regs fail \r\n", __func__);
        acc_enabled = 0;
        return result;
    }

    acc_enabled = 1;
    return result;
}

static int mma8452_active_chip(void)
{
    int result = 0;

    mma_status.ctl_reg1 |= 0x01;
    result = mma8452_refresh_regs();
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C refresh regs fail \r\n", __func__);
        acc_enabled = 0;
        return result;
    }    

    acc_enabled = 1;
    return result;
}

static int mma8452_deact_chip(void)
{
    int result = 0;

    mma_status.ctl_reg1 &= 0xFE;
    acc_enabled = 0;

    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                       MMA8452_CTRL_REG1, mma_status.ctl_reg1);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write ctl_reg1 error \r\n", __func__);
        acc_enabled = 1;
        return result;
    }

    // mdelay(MODE_CHANGE_DELAY_MS);
    mdelay(5);

    return result;
}

static int mma8452_set_delay(int delay_ms)
{
    int result = 0;
    int reset_delay_count = 0;
    if (delay_ms & 0xC7) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s delay_time(0x%x) is NOT correct ! \r\n", __func__, delay_ms);
        return -1;
    }

reset_delay:
    mma_status.ctl_reg1 &= 0xC7;
    mma_status.ctl_reg1 |= delay_ms;
    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                       MMA8452_CTRL_REG1, (mma_status.ctl_reg1 & 0xFE));
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s standby mma8452 fail \r\n", __func__);
    }

    result=i2c_smbus_read_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1);
    if (result & 0x1) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s stop gsensor fail\r\n", __func__);
    }
    
    result = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, 
                                       MMA8452_CTRL_REG1, mma_status.ctl_reg1);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write ctl_reg1 error \r\n", __func__);
    }

    result=i2c_smbus_read_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1);
    if ((result & 0x38) != delay_ms) {
        reset_delay_count++;
        if (reset_delay_count < 5) {
            pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s  %d times, result(0x%x), delay_ms(0x%x) \
                    \r\n", __func__, reset_delay_count, result, delay_ms);
            goto reset_delay;
        }
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s fail \r\n", __func__);
        return -1;
    }

    // pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s [0x%x]\r\n", __func__, delay_ms);
    return 0;
}

#if 0
int mma8452_set_offset(struct mma8452_offset *offset_ptr)
{
    int  bRet = 0;
    
    if (NULL == offset_ptr) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s the pointer(offset_ptr) is NULL \r\n", __func__);
        return -1;
    }

    bRet = mma8452_deact_chip();
    if (bRet) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip failed! \r\n", __func__);
        return bRet;
    }

    mma_status.offset_x = offset_ptr->x;
    mma_status.offset_y = offset_ptr->y;
    mma_status.offset_z = offset_ptr->z;

    bRet  = i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_X,(unsigned char)mma_status.offset_x);
    bRet |= i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Y,(unsigned char)mma_status.offset_y);
    bRet |= i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Z,(unsigned char)mma_status.offset_z);
    
    if (bRet) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write regs failed! \r\n", __func__);
        return bRet;
    }

    pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s x(%4d),y(%4d),z(%4d)\r\n",
            __func__, (unsigned int *)offset_ptr->x, (unsigned int *)offset_ptr->y, (unsigned int *)offset_ptr->z);

    bRet = mma8452_active_chip();
    if (bRet) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_active_chip failed! \r\n", __func__);
        return bRet;
    }

    return bRet;
}
#endif

static int mma8452_set_default_offset(void)
{
#if 0
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s function is called ! \r\n", __func__);
    mma_offset.x = 0;
    mma_offset.y = 0;
    mma_offset.z = 0;
    if (mma8452_set_offset(&mma_offset)) {
        printk(KERN_ERR"acc_8452_write_default_offset failed!\r\n");
        return -EFAULT;
    }
#endif
    return 0;
}

static int mma8452_set_received_offset(struct mma8452_offset *cal_data)
{
    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s\r\n", __func__);
    if (NULL == cal_data) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s the pointer(cal_data) is NULL \r\n", __func__);
        return -1;
    }

    mma_status.offset_x = cal_data->x;
    mma_status.offset_y = cal_data->y;
    mma_status.offset_z = cal_data->z;

    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s %6d, %6d ,%6d \
           \r\n", __func__, mma_status.offset_x, mma_status.offset_y, mma_status.offset_z);
    return 0;
}

static int mma8452_get_cal_data(short *x, short *y, short *z)
{
    unsigned char regs_data[6];

    if (i2c_smbus_read_i2c_block_data(mma8452_dev->i2c_client, 
                                      MMA8452_OUT_X_MSB, 6, regs_data) < 6) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C get data failed ! \r\n", __func__);
        return -1;
    }

    *x = ((regs_data[0] << 8) & 0xff00) | regs_data[1];
    *y = ((regs_data[2] << 8) & 0xff00) | regs_data[3];
    *z = ((regs_data[4] << 8) & 0xff00) | regs_data[5];

    *x = (short)((*x)>>4);
    *y = (short)((*y)>>4);
    *z = (short)((*z)>>4);

    return 0;
}

int mma8452_check_cal_data(struct mma8452_offset *cal_data)
{
    struct mma8452_offset offset_init_abs;
    struct mma8452_offset cal_data_abs;

    if ( NULL == cal_data ) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s NULL poiter ! \r\n", __func__);
        return -1;
    }

#if ACC_METER_PARAM_LIMITED
    offset_init_abs.x = abs(ACC8452_METER_XOFF_INIT);
    offset_init_abs.y = abs(ACC8452_METER_YOFF_INIT);
    offset_init_abs.z = abs(ACC8452_METER_ZOFF_INIT);
    cal_data_abs.x = abs(cal_data->x);
    cal_data_abs.y = abs(cal_data->y);
    cal_data_abs.z = abs(cal_data->z);
    if ((cal_data_abs.x > (offset_init_abs.x + ACC8452_METER_OFFSET_ALLOW))|| 
        (cal_data_abs.x < (offset_init_abs.x - ACC8452_METER_OFFSET_ALLOW))||
        (cal_data_abs.y > (offset_init_abs.y + ACC8452_METER_OFFSET_ALLOW))|| 
        (cal_data_abs.y < (offset_init_abs.y - ACC8452_METER_OFFSET_ALLOW))||
        (cal_data_abs.z > (offset_init_abs.z + ACC8452_METER_OFFSET_ALLOW))|| 
        (cal_data_abs.z < (offset_init_abs.z - ACC8452_METER_OFFSET_ALLOW))) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s acc8452_check_cal_data fail ! \r\n", __func__);
        return -1;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}

static int mma8452_read_data(short *x, short *y, short *z) {
    u8 tmp_data[7];
    if (i2c_smbus_read_i2c_block_data(mma8452_dev->i2c_client, 
                                      MMA8452_OUT_X_MSB, 7, tmp_data) < 7) {
        dev_err(&mma8452_dev->i2c_client->dev, "i2c block read failed\n");
        return -1;
    }

    *x = ((tmp_data[0] << 8) & 0xff00) | tmp_data[1];
    *y = ((tmp_data[2] << 8) & 0xff00) | tmp_data[3];
    *z = ((tmp_data[4] << 8) & 0xff00) | tmp_data[5];

    *x = (short)(*x) >> 4;
    *y = (short)(*y) >> 4;
    *z = (short)(*z) >> 4;

    return 0;
}

static void mma8452_report_abs(void)
{
    short x, y, z;
    int result = 0;
    unsigned char read_count = 0;

    if (mma8452_dev->i2c_client == NULL || mma8452_dev->input_polled_dev == NULL) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s NULL poiter ,line = %d \r\n",__func__,__LINE__);
        return;
    }

    do {
        result=i2c_smbus_read_byte_data(mma8452_dev->i2c_client, MMA8452_STATUS);
        read_count++;
    } while (!(result & 0x08) && read_count);

    if (mma8452_read_data(&x,&y,&z) != 0) {
        return;
    }

    input_report_abs(mma8452_dev->input_polled_dev->input, ABS_X, (y));
    input_report_abs(mma8452_dev->input_polled_dev->input, ABS_Y, (-x));

    input_report_abs(mma8452_dev->input_polled_dev->input, ABS_Z, (z));
    input_sync(mma8452_dev->input_polled_dev->input);   
}

static void mma8452_dev_poll(struct input_polled_dev *dev)
{
    if (acc_flag_resumed && acc_enabled && (!polling) && (!acc_flag_caling)) {
        polling = 1;
        mma8452_report_abs();
        polling = 0;
    }
} 

int mma8452_calibration(struct mma8452_offset *pCalAccOffset)
{
    int   bRet = 0;
    short x,y,z;
    short xavg,yavg,zavg;
    short off_x,off_y,off_z;
    short AvgCount;
    int   cal_cnt = 0;
    unsigned char new_data[3] = {0};
    struct mma8452_offset off_cal;
    memset(&off_cal, 0, sizeof(off_cal));
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s\r\n", __func__);

    if (NULL == pCalAccOffset) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s NULL Pointer ! \r\n", __func__);
        return -1;
    }

    pCalAccOffset->x = ACC8452_METER_XOFF_INIT;
    pCalAccOffset->y = ACC8452_METER_YOFF_INIT;
    pCalAccOffset->z = ACC8452_METER_ZOFF_INIT;

    if (mma8452_set_power(true)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_set_power fail1 \r\n", __func__);
        bRet = -1;
        goto cal_done;
    }

    msleep(5);
    if (mma8452_deact_chip()) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip fail ! \r\n", __func__);
        bRet = -1;
        goto cal_done;
    }
    off_x = 0;
    off_y = 0;
    off_z = 0;

    /* clear offset reg */
    if (i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_X, 0) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Y, 0) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Z, 0)) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s Clear offset regs error ! \r\n", __func__);
        bRet = -1;
        goto cal_done;
    }
    msleep(200);
    #if 0
    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s off_x = %6d, off_y =%6d, off_z = %6d",
           __func__, off_x, off_y, off_z);
    #endif
cal_retry:
    mma_status.ctl_reg1 |= 0x1;
    if (i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1, mma_status.ctl_reg1)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write ctl_reg1 error \r\n", __func__);
        acc_enabled = 0;
        bRet = -1;
        goto cal_done;
    }
    acc_enabled = 1;
    msleep(10);

    if (cal_cnt > MAX_CAL_CNT) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s Try %d times ,set default ! \r\n",__func__,cal_cnt);
        pCalAccOffset->x = ACC8452_METER_XOFF_INIT;
        pCalAccOffset->y = ACC8452_METER_YOFF_INIT;
        pCalAccOffset->z = ACC8452_METER_ZOFF_INIT;
        bRet = -1;
        goto cal_done;
    }

    xavg = 0;
    yavg = 0;
    zavg = 0;
    for (AvgCount=0; AvgCount<4; AvgCount++) {
        msleep(50);
        if (mma8452_get_cal_data(&x,&y,&z) != 0) {
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s read data fail ! \r\n", __func__);
            cal_cnt++;
            goto cal_retry;
        }
        xavg += x;
        yavg += y;
        zavg += z;
    }

    xavg /= 4;
    yavg /= 4;
    zavg /= 4;

    if (((xavg <  CAL_FUZZ) && (xavg > -CAL_FUZZ)) &&
        ((yavg <  CAL_FUZZ) && (yavg > -CAL_FUZZ)) &&
        ((zavg > ((0x400)-CAL_FUZZ))) && (zavg <  ((0x400)+CAL_FUZZ))) {
        off_cal.x = off_x;
        off_cal.y = off_y;
        off_cal.z = off_z;

        if (!mma8452_check_cal_data(&off_cal)) {
            /*save the calibration datas*/
            pCalAccOffset->x = off_x;
            pCalAccOffset->y = off_y;
            pCalAccOffset->z = off_z;
            printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: %s: Set calibration Success: Xoffset=%6d,Yoffset=%6d,Zoffset=%6d\n",__func__,
                   pCalAccOffset->x,pCalAccOffset->y,pCalAccOffset->z);
            bRet = 0;
        }
        else
        {
            pCalAccOffset->x = ACC8452_METER_XOFF_INIT;
            pCalAccOffset->y = ACC8452_METER_YOFF_INIT;
            pCalAccOffset->z = ACC8452_METER_ZOFF_INIT;
            printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s fail, please set the device Landscape!", __func__);
            bRet = -1;
        }
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s Calibration done\n", __func__);
        goto cal_done;
    }

    cal_cnt++;
    off_x -= (short)(xavg/4);
    off_y -= (short)(yavg/4); 
    off_z -= (short)(zavg/4 - 0x100);
    bRet = mma8452_deact_chip();
    if (bRet) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip fail ! \r\n", __func__);
        bRet = -1;
        goto cal_done;
    }
    new_data[0] = off_x;
    new_data[1] = off_y;
    new_data[2] = off_z;
    if (i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_X, new_data[0]) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Y, new_data[1]) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Z, new_data[2])) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C set offset regs fail ! \r\n", __func__);
        bRet = -1;
        goto cal_done;
    }
    goto cal_retry;

cal_done:
    if (bRet == 0) {
        mma_status.offset_x = pCalAccOffset->x;
        mma_status.offset_y = pCalAccOffset->y;
        mma_status.offset_z = pCalAccOffset->z;
    }
    else
    {
        pCalAccOffset->x = mma_status.offset_x;
        pCalAccOffset->y = mma_status.offset_y;
        pCalAccOffset->z = mma_status.offset_z;
    }

    if (mma8452_deact_chip()) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip fail ! \r\n", __func__);
        bRet = -1;
        goto cal_ret;
    }

    if (i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_X,(unsigned char)mma_status.offset_x) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Y,(unsigned char)mma_status.offset_y) |
        i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_OFF_Z,(unsigned char)mma_status.offset_z)) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C set offset regs fail ! \r\n", __func__);
        bRet = -1;
        goto cal_ret;
    }

    mma_status.ctl_reg1 |= 0x1;
    if (i2c_smbus_write_byte_data(mma8452_dev->i2c_client, MMA8452_CTRL_REG1,mma_status.ctl_reg1)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s I2C write ctl_reg1 error \r\n", __func__);
        acc_enabled = 0;
        bRet = -1;
        goto cal_ret;
    }
    acc_enabled = 1;
    msleep(5);

cal_ret:
    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: %s: %6d, %6d, %6d\r\n", 
           __func__, mma_status.offset_x, mma_status.offset_y, mma_status.offset_z);    
    return bRet;
}

static long mma8452_ioctl(struct file *file, unsigned int cmd, unsigned long arg)                    
{
    int    bRet;
    void   __user *argp = (void __user *)arg;
    int    turn_on;
    int    speed, count = 0;
    struct mma8452_offset acc_cal_data;
    struct mma8452_offset *cal_data = &acc_cal_data;

    bRet = 0;
    if ( mma8452_dev->i2c_client == NULL || file == NULL ) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s NULL poiter ,line = %d \r\n", __func__, __LINE__);
        return -EINVAL;
    }

    switch (cmd) {
        case MMA8452_IOCTL_ACTIVE:
            if (copy_from_user(&turn_on, argp, sizeof(turn_on))) {
                pr_info("BJ_BSP_DRIVER: CP_ACC: copy failed(%s),line(%d)\n", __func__, __LINE__);
                return -EFAULT;
            }

            printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: %s active(%d)\r\n", __func__, turn_on);
            if (turn_on & 0xFE) {
                printk(KERN_ERR" BJ_BSP_DRIVER: CP_ACC: turn_on flag (%d) is error\n",turn_on);
                return -EINVAL;
            }
            if ((acc_enabled != turn_on)) { // states changes
                if (turn_on == 1) {
                    if (mma8452_set_power(true)) {
                        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set_power fail \r\n", __func__);
                        return -1;
                    }
                #if GSENSOR_WITH_MPU
                    if (mma8452_setup_mpu3050()) {
                        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s setup_mpu3050 fail \r\n", __func__);                    
                    }
                #endif
                    if (mma8452_dev->input_polled_dev->input->open) {
                        mma8452_dev->input_polled_dev->input->open(mma8452_dev->input_polled_dev->input);
                        printk("BJ_BSP_Driver: CP_ACC: %s open input poll device\n", __func__);
                    }

                    bRet = mma8452_active_chip();
                    if (bRet) {
                        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s active chip fail \n", __func__);
                        return -1; 
                    }
                }
                if (turn_on == 0) {
                    if (mma8452_dev->input_polled_dev->input->close) {
                        mma8452_dev->input_polled_dev->input->close(mma8452_dev->input_polled_dev->input);
                        printk("BJ_BSP_Driver: CP_ACC: %s close input poll device\n", __func__);
                    }

                    bRet = mma8452_deact_chip();
                    if (bRet) {
                        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip fail \r\n", __func__);
                    }

                    if (polling && (count < 100)) {
                        // printk(KERN_INFO"%s:  %d ms  \r\n", __func__, count);
                        count++;
                        msleep(1);
                    }
    
                    if(mma8452_set_power(false))
                    {
                        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set_power fail \r\n", __func__);
                    }
                }
            }
            break;

        case MMA8452_IOCTL_SET_DELAY:
            printk(KERN_INFO "BJ_BSP_DRIVER: CP_ACC: in %s SET_DELAY  \r\n", __func__);
            if (copy_from_user(&speed, argp, sizeof(speed))) {
                pr_info("BJ_BSP_DRIVER: CP_ACC: copy failed(%s),line(%d)\n", __func__, __LINE__);
                return -EFAULT;
            }

            bRet = mma8452_set_delay(speed);
            if (bRet) {
                return -1;
            }
            break;

        case MMA8452_IOCTL_DO_CALIBRATION:
            printk(KERN_INFO "BJ_BSP_DRIVER: CP_ACC: in %s CALIBRATION  \r\n", __func__);
            acc_flag_caling = 1;
            wake_lock(&mma8452_dev->wake_lock);
            bRet = mma8452_calibration(cal_data);
            wake_unlock(&mma8452_dev->wake_lock);
            if (bRet) {
                printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s Calibration fail ! \r\n", __func__);
                bRet = -1;
            }
    
            if ((struct mma8452_offset *)arg != NULL) {
                if (copy_to_user((struct mma8452_offset *)arg, cal_data, sizeof(acc_cal_data))) {
                    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s copy offset value to user failed ! \r\n", __func__);
                    bRet = -1;
                }
            }

            acc_flag_caling = 0;
            break;

        case MMA8452_IOCTL_SET_OFFSET:
            printk(KERN_INFO "BJ_BSP_DRIVER: CP_ACC: in %s  SET_OFFSET  \r\n", __func__);
            if ((struct mma8452_offset *)arg == NULL || 
                copy_from_user(cal_data, (struct mma8452_offset *)argp, sizeof(acc_cal_data))) {
                bRet = mma8452_set_default_offset();
                if (bRet) {
                    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set_default_offset failed ! \r\n", __func__);
                    return bRet;
                }
            }
            else
            {
                bRet = mma8452_set_received_offset(cal_data);
                if (bRet) {
                    printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s set_received_offset failed ! \r\n", __func__);
                    return bRet;
                }
            }
            break;

        default:
            pr_info( "%s: Unrecognized IOCTL %d\r\n", __func__, cmd);
            break;
    }
    return bRet;
}

static int mma8452_open(struct inode *inode, struct file *file)
{
    if (mma8452_dev->input_polled_dev == NULL) {
        printk("BJ_BSP_Driver: CP_ACC: in %s open failed, because driver load failed\n", __func__);
        return -1;
    }
    return 0;
}

static int mma8452_close(struct inode *inode, struct file *file)
{
    return 0;
}

static const struct file_operations mma8452_fops = {
    .owner          = THIS_MODULE,
    .open           = mma8452_open,
    .release        = mma8452_close,
    .unlocked_ioctl = mma8452_ioctl,
};

static struct miscdevice mma8452_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "mma8452_dev", 
    .fops  = &mma8452_fops,
};

static void mma8452_early_suspend(struct early_suspend *handler)
{
}

static void mma8452_early_resume(struct early_suspend *handler)
{
}

static int __devinit mma8452_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int result = 0;
    struct input_dev *idev = NULL;
    struct i2c_adapter *adapter = NULL;

    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s\r\n", __func__);
    if (mma8452_set_power(true)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_set_power failed.\r\n", __func__);
        result = -1;
        goto out;
    }
    mdelay(10);

    /* check i2c bus */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA)) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s i2c_check_functionality failed.\r\n", __func__);
        result = -ENODEV;
        goto out_power_down;
    }

    mma8452_dev = kmalloc(sizeof(struct mma8452_data), GFP_KERNEL);
    if ( !mma8452_dev ) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_dev kmalloc error!\n", __func__);
        result = -ENOMEM;
        goto out_power_down;
    }
    memset(mma8452_dev, 0, sizeof(struct mma8452_data));

    acc_flag_resumed    = 1;
    acc_enabled         = 0;
    acc_flag_caling     = 0;
    polling             = 0;
    mma_status.offset_x = 0;
    mma_status.offset_y = 0;
    mma_status.offset_z = 0;

    mma8452_dev->i2c_client = client;
    adapter = to_i2c_adapter(client->dev.parent);
#if GSENSOR_WITH_MPU
    if (mma8452_setup_mpu3050()) {
        printk(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_setup_mpu3050 failed.\r\n", __func__);                    
    }
#endif
    result = i2c_smbus_read_byte_data(client, MMA8452_WHO_AM_I);
    if (MMA8452_ID != (result)) {
        dev_err(&client->dev, "read chip ID 0x%x is not equal to 0x%x!\r\n", result, MMA8452_ID);
        printk(KERN_INFO "J_BSP_DRIVER: CP_ACC: in %s read chip ID failed.\r\n", __func__);
        result = -EINVAL;
        goto out_free_dev;
    }

    result = mma8452_init_regs();
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s ma8452_init_regs fail \r\n", __func__);
        goto out_free_dev;
    }

    dev_info(&client->dev, "build time %s %s\n", __DATE__, __TIME__);
    wake_lock_init(&mma8452_dev->wake_lock, WAKE_LOCK_SUSPEND, "gsensor_wake_lock");

    mma8452_dev->input_polled_dev = input_allocate_polled_device();
    if (!mma8452_dev->input_polled_dev) {
        dev_err(&client->dev, "alloc poll device failed!\n");
        result = -ENOMEM;
        goto out_free_dev;
    }
    mma8452_dev->input_polled_dev->poll              = mma8452_dev_poll;
    mma8452_dev->input_polled_dev->poll_interval     = POLL_INTERVAL;
    mma8452_dev->input_polled_dev->poll_interval_max = POLL_INTERVAL_MAX;

    idev             = mma8452_dev->input_polled_dev->input;
    idev->name       = MMA8452_INPUT_NAME;
    idev->id.bustype = BUS_I2C;
    idev->evbit[0]   = BIT_MASK(EV_ABS);
    input_set_abs_params(idev, ABS_X, -8192, 8191, INPUT_FUZZ, INPUT_FLAT);
    input_set_abs_params(idev, ABS_Y, -8192, 8191, INPUT_FUZZ, INPUT_FLAT);
    input_set_abs_params(idev, ABS_Z, -8192, 8191, INPUT_FUZZ, INPUT_FLAT);
    result = input_register_polled_device(mma8452_dev->input_polled_dev);
    if (result) {
        dev_err(&client->dev, "register poll device failed!\n");
        goto out_free_input_dev;
    }

    if (mma8452_deact_chip()) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip failed.\r\n", __func__);
    }

    mma8452_dev->early_op.suspend = mma8452_early_suspend;
    mma8452_dev->early_op.resume  = mma8452_early_resume;
    register_early_suspend(&mma8452_dev->early_op);

    result = misc_register(&mma8452_device);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s misc_register failed.\r\n", __func__);
        goto out_unregister_input_dev;
    }

    if (mma8452_dev->input_polled_dev->input->close) {
        mma8452_dev->input_polled_dev->input->close(mma8452_dev->input_polled_dev->input);
        printk("BJ_BSP_Driver: CP_ACC: %s close input poll device\n", __func__);
    }

    result = mma8452_set_power(false);
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_set_power failed.\r\n", __func__);
        goto out_deregister_misc;
    }

    pr_info(KERN_INFO "%s: probe success\r\n", __func__);

    return 0;

out_deregister_misc:
    misc_deregister(&mma8452_device);
out_unregister_input_dev:
    input_unregister_polled_device(mma8452_dev->input_polled_dev);
out_free_input_dev:
    input_free_polled_device(mma8452_dev->input_polled_dev);
    mma8452_dev->input_polled_dev = NULL;
out_free_dev:
    kfree(mma8452_dev);
    mma8452_dev = NULL;
out_power_down:
    if (mma8452_set_power(false)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_set_power failed.\r\n", __func__);
    }
out:
    return result;
}

static int __devexit mma8452_remove(struct i2c_client *client)
{
    int result = 0;
    int count = 0;

    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s\n", __func__);
    result = mma8452_deact_chip();
    if (result) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_deact_chip fail \r\n", __func__);
    }
    if (polling && count < 50) {
        msleep(1);
        count++;
    }

    misc_deregister(&mma8452_device);
    input_unregister_polled_device(mma8452_dev->input_polled_dev);
    input_free_polled_device(mma8452_dev->input_polled_dev);
    kfree(mma8452_dev);
    if (mma8452_set_power(false)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s mma8452_set_power fail \r\n", __func__);
        return -1;
    }

    return result;
}

static int mma8452_suspend(struct i2c_client *client, pm_message_t mesg)
{
    int result = 0;
    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s\n", __func__);
    acc_flag_resumed = 0;

    return result;
}

static int mma8452_resume(struct i2c_client *client)
{
    int result=0;
    printk(KERN_INFO"BJ_BSP_DRIVER: CP_ACC: in %s\n", __func__);
    acc_flag_resumed = 1;

    return result;
}

static const struct i2c_device_id mma8452_id[] = {
    { MMA8452_DRV_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mma8452_id);

static struct i2c_driver mma8452_driver = {
    .driver = {
        .name  = MMA8452_DRV_NAME,
        .owner = THIS_MODULE,
    },
    .suspend   = mma8452_suspend,
    .resume    = mma8452_resume,
    .probe     = mma8452_probe,
    .remove    = __devexit_p(mma8452_remove),
    .id_table  = mma8452_id,
};

static int __init mma8452_init(void)
{
    power_state = false;
    pm8901_mvs0 = regulator_get(NULL, "8901_mvs0");
    if (NULL == pm8901_mvs0) {
        printk(KERN_ERR"BJ_BSP_DRIVER: CP_ACC: pm8901_mvs0 regulator_get failed !\n");
        return -1;
    }
    pm8058_lvs1 = regulator_get(NULL, "8058_lvs1");
    if (NULL == pm8058_lvs1) {
        printk(KERN_ERR"BJ_BSP_DRIVER: CP_ACC: pm8058_lvs1 regulator_get failed !\n");
        return -1;
    }

    return i2c_add_driver(&mma8452_driver);
}

static void __exit mma8452_exit(void)
{
    printk(KERN_INFO "BJ_BSP_DRIVER: CP_ACC: remove mma8452 i2c driver.\n");
    if (mma8452_set_power(false)) {
        pr_info(KERN_ERR "BJ_BSP_DRIVER: CP_ACC: in %s akm8975_set_power fail \r\n", __func__);
    }

    i2c_del_driver(&mma8452_driver);
}

MODULE_AUTHOR("Jay.HF@CoolPad");
MODULE_DESCRIPTION("MMA8452 3-Axis Accelerometer driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");

module_init(mma8452_init);
module_exit(mma8452_exit);

