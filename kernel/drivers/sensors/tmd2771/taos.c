/****************************************************************************/
/*                                                                          */
/*             Copyright (c) 2011-2012  YULONG Company             　　　　 */
/*      宇龙计算机通信科技（深圳）有限公司  版权所有 2011-2012              */
/*                                                                          */
/* PROPRIETARY RIGHTS of YULONG Company are involved in the  　             */
/* subject matter of this material.  All manufacturing, reproduction, use,  */
/* and sales rights pertaining to this subject matter are governed by the   */
/* license agreement.  The recipient of this software implicitly accepts    */
/* the terms of the license.                                                */
/* 本软件文档资料是宇龙公司的资产,任何人士阅读和使用本资料必须获得          */
/* 相应的书面授权,承担保密责任和接受相应的法律约束.                         */
/*                                                                          */
/****************************************************************************/

/**************************************************************************
**  FileName:    taos.c
**  Author:      皇甫杰
**  Version:     1.00
**  Date:        2012-11-11
**  Description: 光距离传感器
**  History:         
**  <author>      <time>      <version >      <desc>
**  皇甫杰        2012-11-11  1.00            创建                                       
**************************************************************************/


#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/timer.h>
#include <asm/errno.h>
#include <asm/delay.h>
#include <linux/delay.h>
#include <linux/irq.h> 
#include <linux/interrupt.h> 
#include <linux/slab.h>
#include <mach/gpio.h> 
#include <linux/poll.h> 
#include <linux/wakelock.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/regulator/consumer.h>
#include <linux/earlysuspend.h>
#include <linux/taos.h>
#include <linux/gpio.h>
#include <asm/mach/time.h>
#include <linux/android_alarm.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/sysdev.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
#include <yl_debug/management.h>
#endif

#define TAOS_PM_EN             1 /* the power manager switch    */
#define TAOS_PWROFF_EN         0 /* the power off enable switch */

#define TAOS_DBG_EN            0 /* the debug info switch       */
#define TAOS_PARAM_EN          1 /* the calibrator data write into yl_params enable switch */

#define TAOS_SUSPEND_SLEEP_EN  0 /* the suspend goto sleep mode enable switch              */
#define TAOS_EARLY_SUSPEND_EN  0 /* the early_suspend and late_resume enable switch        */

#define TAOS_PROX_THRESHOLD_Hi 350
#define TAOS_PROX_THRESHOLD_Lo 300

struct taos_dev {
    struct i2c_client *client;
    struct input_dev  *input_dev;
    struct work_struct work;
    struct wake_lock   wake_lock;
    struct wake_lock   wake_lock_timeout;
#if TAOS_EARLY_SUSPEND_EN
    struct early_suspend early_suspend;
#endif
    unsigned int       als_ps_int;

    unsigned short     sat_als;
    unsigned short     sat_prox;
    int                prox_on;
    int                prox_enabled;
    int                als_enabled;
    int                suspend_flag;
    int                polling;
    unsigned int       phone_state;
    int                data_ready;
#if TAOS_SUSPEND_SLEEP_EN
    u8                 working; // reg_cntrl;
#endif
#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
    u8                 pm_state; /* the current pm state, add by Jay.HF 2012-12-06 */
#endif
};
static struct taos_dev *tmd2771_dev = NULL;

static struct taos_cfg tmd2771_cfg = {
    .calibrate_target  = 300000,
    .als_time          = 1000, // 500, // 200,
    .scale_factor      = 1,
    .gain_trim         = 512,
    .filter_history    = 3,
    .filter_count      = 1,    
    .gain              = 0,
    .prox_threshold_hi = TAOS_PROX_THRESHOLD_Hi, // 550, // 700, // 960,
    .prox_threshold_lo = TAOS_PROX_THRESHOLD_Lo, // 400, // 550, // 768,
    .als_threshold_hi  = 3000,
    .als_threshold_lo  = 20,
    .prox_int_time     = 0xEE,
    .prox_adc_time     = 0xFF,
    .prox_wait_time    = 0xEE,
    .prox_intr_filter  = 0x23,
    .prox_config       = 0x00,
    .prox_pulse_cnt    = 0x04,
    .prox_gain         = 0x22
};

u8 taos_triton_reg_init[16] = {
    0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
};
u8 taos_triton_gain_table[] = {1, 8, 16, 120};

struct time_scale_factor {
    u16 numerator;
    u16 denominator;
    u16 saturation;
};
struct time_scale_factor TritonTime = {1, 0, 0};
struct time_scale_factor *lux_timep = &TritonTime;

struct lux_data {
    u16 ratio;
    u16 clear;
    u16 ir;
};
struct lux_data TritonFN_lux_data[] = {
    { 9830,  8320,  15360 },
    { 12452, 10554, 22797 },
    { 14746, 6234,  11430 },
    { 17695, 3968,  6400  },
    { 0,     0,     0     }
};
struct lux_data *lux_tablep = TritonFN_lux_data;

struct taos_prox_info prox_cal_info[20];

bool power_state;
struct regulator *pm8038_l9;
struct regulator *pm8038_l11;
static int set_power(struct regulator *s, unsigned int voltage, bool enable)
{
#if TAOS_PM_EN
    int err;
    if (enable) {
        if (voltage > 0) {
            err = regulator_set_voltage(s, voltage, voltage);
            if (err) {
                printk(KERN_ERR"[Jay.HF]%s: set_voltage failed \n", __func__);
                return -1;
            }
        }

        err = regulator_enable(s);
        if (err) {
            printk(KERN_ERR"[Jay.HF]%s: enable error\n", __func__);
            return -1;
        }
    }
    else
    {
        err = regulator_disable(s);
        if (err) {
            printk(KERN_ERR"[Jay.HF]%s: disable error\n", __func__);
            return -1;
        }
    }
#endif
    return 0;
}

static int taos_set_power(bool enable)
{
    int err = 0;

    if(enable != power_state) {
        printk(KERN_INFO"[Jay.HF]%s: [%d] \r\n", __func__, enable);

        // err = set_power(pm8038_l9, 3300000, enable);
        err = set_power(pm8038_l9, 0, enable);
        if(err) 
            goto err1;
        err = set_power(pm8038_l11, 0, enable);
        if(err) 
            goto err2;

        power_state = enable;
        if (enable) {
            msleep(5);
        }
    }

    return 0;

err2:
    printk(KERN_ERR"[Jay.HF]%s: err2 \n", __func__);
err1:
    printk(KERN_ERR"[Jay.HF]%s: err1 \n", __func__);

    return -1;
}

static int taos_power_init(void)
{
    power_state = false;
#if TAOS_PM_EN
    pm8038_l9 = regulator_get(NULL, "8038_l9");
    if (NULL == pm8038_l9) {
        printk(KERN_ERR"[Jay.HF]%s: regulator_get failed !\n", __func__);
        return -1;
    }

    pm8038_l11 = regulator_get(NULL, "8038_l11");
    if (NULL == pm8038_l11) {
        regulator_put(pm8038_l9);
        printk(KERN_ERR"[Jay.HF]%s: regulator_get failed !\n", __func__);
        return -1;
    }
#endif
    return 0;
}

static int taos_power_deinit(void)
{
    power_state = false;
#if TAOS_PM_EN
    regulator_put(pm8038_l11);
    regulator_put(pm8038_l9);
#endif
    return 0;
}

#if TAOS_PARAM_EN
extern int yl_params_kernel_read(char *buf, size_t count, int ver);
extern int yl_params_kernel_write(char *buf, size_t count, int ver);

static void taos_write_param_to_flash(unsigned char* buf)
{
    int ret = 0;
    unsigned char param[512] ="PROJECT0";
    int size  = sizeof(param);

    ret = yl_params_kernel_read(param, size, 1);
    if (ret < 0) {
        printk(KERN_ERR"[Jay.HF]%s write-param failed\n", __func__);
        return;
    }

    if(NULL != buf) {
        memcpy(&param[17], buf, 3);
    }
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s write-param = %s, size = %d\n", __func__, param, size);
    printk(KERN_ERR"[Jay.HF]%s write-param[17] = 0x%x\n", __func__, param[17]);
    printk(KERN_ERR"[Jay.HF]%s write-param[18] = 0x%x\n", __func__, param[18]);
    printk(KERN_ERR"[Jay.HF]%s write-param[19] = 0x%x\n", __func__, param[19]);
#endif
    yl_params_kernel_write(param, size, 1);
}

static void taos_read_param_from_flash(unsigned char* out)
{
    u8 param[512] ="PROJECT0";
    int size = sizeof(param);

    yl_params_kernel_read(param, size, 1);
    if(NULL != out)
    {
        memcpy(out, &param[17], 3);
    }

#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s read-param = %s, size = %d\n", __func__, param, size);
    printk(KERN_ERR"[Jay.HF]%s read-param[17] = 0x%x\n", __func__, param[17]);
    printk(KERN_ERR"[Jay.HF]%s read-param[18] = 0x%x\n", __func__, param[18]);
    printk(KERN_ERR"[Jay.HF]%s read-param[19] = 0x%x\n", __func__, param[19]);
#endif
}
#endif

static int taos_get_lux(void)
{
#if TAOS_ALGO_OPTIMIZE
    u8 dev_gain = 0;
    u16 Tint = 0;
#endif
    u8 chdata[4];
    u16 raw_clear = 0, raw_ir = 0, raw_lux = 0;
    u32 lux = 0, ratio = 0;
    int ret = 0, tmp = 0, i = 0;
    struct lux_data *p;

    for (i=0;i<4;i++)
    {
        if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | (TAOS_TRITON_ALS_CHAN0LO + i))))) < 0) 
        {
            printk(KERN_ERR"[Jay.HF]%s: i2c_write to ch0/1/lo/hi regs failed \n", __func__);
            return (ret);
        }

        chdata[i] = i2c_smbus_read_byte(tmd2771_dev->client);
    }

#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: ch0(%d)\n", __func__, chdata[0]+chdata[1]*256);
    printk(KERN_ERR"[Jay.HF]%s: ch1(%d)\n", __func__, chdata[2]+chdata[3]*256);
#endif

    tmp = (tmd2771_cfg.als_time+25)/50; /* if atime =100  tmp = (atime+25)/50=2.5   time = 2.7*(256-atime)=  412.5 */
#if 0
    TritonTime.numerator = 1;
#endif
    TritonTime.denominator = tmp;

    tmp = 300*tmd2771_cfg.als_time; /* tmp = 300*atime  400 */
    if (tmp > 65535)
        tmp = 65535;
    TritonTime.saturation = tmp;
    raw_clear = chdata[1];
    raw_clear <<= 8;
    raw_clear |= chdata[0];
    raw_ir = chdata[3];
    raw_ir <<= 8;
    raw_ir |= chdata[2];

    if (raw_ir > raw_clear) 
    {
        raw_lux = raw_ir;
        raw_ir = raw_clear;
        raw_clear = raw_lux;
    }

#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: Cr(%d), Ir(%d)", __func__, raw_clear, raw_ir);
#endif

#if TAOS_ALGO_OPTIMIZE
    raw_clear *= (tmd2771_cfg.scale_factor);
    raw_ir *= (tmd2771_cfg.scale_factor);
    dev_gain = taos_triton_gain_table[tmd2771_cfg.gain & 0x3];
#endif

    if (raw_clear >= lux_timep->saturation)
    {
        return(TAOS_MAX_LUX);
    }

    if (raw_ir >= lux_timep->saturation)
    {
        return(TAOS_MAX_LUX);
    }

    if (raw_clear == 0)
        return(0);

#if TAOS_ALGO_OPTIMIZE
    if (dev_gain == 0 || dev_gain > 127) 
    {
        printk(KERN_ERR"[Jay.HF]%s: dev_gain = 0 or > 127 in taos_get_lux()\r\n", __func__);
        return -1;
    }
#endif

    if (lux_timep->denominator == 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: lux_timep->denominator = 0 in taos_get_lux()\r\n", __func__);
        return -1;
    }

    ratio = (raw_ir << 15) / raw_clear;
    for (p = lux_tablep; p->ratio && p->ratio < ratio; p++);
    if (!p->ratio)
    {
        return 0;
    }

#if TAOS_ALGO_OPTIMIZE
    Tint = tmd2771_cfg.als_time;
    raw_clear = ((raw_clear*400 + (dev_gain>>1))/dev_gain + (Tint>>1))/Tint;
    raw_ir = ((raw_ir*400 +(dev_gain>>1))/dev_gain + (Tint>>1))/Tint;
    lux = ((raw_clear*(p->clear)) - (raw_ir*(p->ir)));
    lux = (lux + 32000)/64000;
#else
    lux = ((raw_clear*(p->clear)) - (raw_ir*(p->ir)) + 8000)/16000;
#endif

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: lux(%d)\r\n", __func__, lux);
#endif

    if (lux > TAOS_MAX_LUX)
    {
        lux = TAOS_MAX_LUX;
        return(lux);
    }

    return(lux);
}

#if 0
static int taos_lux_filter(int lux)
{
    static u8 middle[] = {1,1,2,0,0,2,1,1};
    int index = 0;

    lux_history[2] = lux_history[1];
    lux_history[1] = lux_history[0];
    lux_history[0] = lux;

    if(lux_history[2] < 0) 
    {
        if(lux_history[1] > 0)
            return lux_history[1];  
        else 
            return lux_history[0];
    }

    if( lux_history[0] > lux_history[1] ) 
        index += 4;
    if( lux_history[1] > lux_history[2] ) 
        index += 2;
    if( lux_history[0] > lux_history[2] )
        index++;

    return(lux_history[middle[index]]);
}
#endif

static int taos_als_threshold_set(void)
{
    u8 chdata[2];
    u16 ch0, thresh_h, thresh_l;
    char buf[4];
    int i, count, ret = 0;
    for (i = 0; i < 2; i++)
    {
        chdata[i] = (i2c_smbus_read_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | (TAOS_TRITON_ALS_CHAN0LO + i))));
    }

    ch0 = chdata[0] + ((u16)chdata[1]<<8);

#if 1
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: ch0 = %d\r\n", __func__, ch0);
#endif
    thresh_h = (12/*15*/*ch0)/10;
    if (thresh_h >= 65535)
        thresh_h = 65535;
    thresh_l = (8/*6*/*ch0)/10;

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: (%d,%d)\r\n", __func__, thresh_l, thresh_h);
#endif
#else
    if (ch0 < 25) /* (25, 10)(70, 30)(93, 40) */
    {
        thresh_h = 93;
        thresh_l = 0;
    }
    else
    { 
        thresh_h = (16*ch0)/10;
        if (thresh_h >= 65535)
            thresh_h = 65535;
        if (thresh_h < 93)//(25,10)(70,30)(93,40)
            thresh_h = 93;
        thresh_l = (6*ch0)/10;
    }
#endif

    buf[0] = thresh_l & 0x0ff;
    buf[1] = thresh_l >> 8;
    buf[2] = thresh_h & 0x0ff;
    buf[3] = thresh_h >> 8;

    for (count=0; count<4; count++)
    { 
        ret = i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_ALS_MINTHRESHLO) + count, buf[count]);
        if (ret < 0) 
        {
            printk(KERN_ERR"[Jay.HF]%s: ALS threshold set fail\r\n", __func__);
            return (ret);
        }
    }

    return ret;
}

static int taos_als_default_threshold_set(void)
{
    char buf[4];
    int count, ret = 0;

#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s\r\n", __func__);
#endif
    buf[0] = 0xfe;
    buf[1] = 0x0;
    buf[2] = 0xff;
    buf[3] = 0x0;

    for (count=0; count<4; count++)
    { 
        ret = i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_ALS_MINTHRESHLO) + count, buf[count]);
        if (ret < 0) 
        {
            printk(KERN_ERR"[Jay.HF]%s: ALS set fail\r\n", __func__);
            return (ret);
        }
    }

    return ret;
}

static int taos_prox_threshold_set(void)
{
    u8 chdata[6];
    u16 proxdata = 0;
    u16 cleardata = 0;
    char pro_buf[4];
    int i, count, ret = 0;

    for (i = 0; i < 6; i++) {
        chdata[i] = (i2c_smbus_read_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG |(TAOS_TRITON_ALS_CHAN0LO + i))));
    }

    cleardata = chdata[0] + chdata[1]*256;
    proxdata  = chdata[4] + chdata[5]*256;

#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: cleardata = %6d, proxdata = %6d\n", __func__, cleardata, proxdata);
#endif

    if (tmd2771_dev->prox_on || (proxdata < tmd2771_cfg.prox_threshold_lo))
    {
        pro_buf[0] = 0x00; /* set the prox_threshold_lo to zero, */
        pro_buf[1] = 0x00; /* next time the proxdata must be bigger than the prox_threshold_lo, 
                              this is avoid two anear appear. add by Jay.HF 2012-09-15 */
        pro_buf[2] = tmd2771_cfg.prox_threshold_hi & 0x0ff;
        pro_buf[3] = tmd2771_cfg.prox_threshold_hi >> 8;
        input_report_abs(tmd2771_dev->input_dev, ABS_DISTANCE, 5);
    #if TAOS_DBG_EN
        printk(KERN_INFO"[Jay.HF]%s: prox(5), prox_threshold_hi(%d), prox_threshold_lo(%d)\n", 
               __func__, tmd2771_cfg.prox_threshold_hi, tmd2771_cfg.prox_threshold_lo);
    #endif
    }
    else if(proxdata > tmd2771_cfg.prox_threshold_hi)
    {
        if (cleardata > ((tmd2771_dev->sat_als * 80) / 100))
        {
            printk(KERN_ERR"[Jay.HF]%s: ERROR, cleardata = [%6d], ((tmd2771_dev->sat_als * 80) / 100) = [%6d] \n", 
                   __func__, cleardata, ((tmd2771_dev->sat_als*80)/100));
            return -ENODATA;
        }

        pro_buf[0] = tmd2771_cfg.prox_threshold_lo & 0x0ff;
        pro_buf[1] = tmd2771_cfg.prox_threshold_lo >> 8;
        pro_buf[2] = 0xff; /* set the prox_threshold_hi to 0xFFFF, */
        pro_buf[3] = 0xff; /* next time the proxdata must be less than the prox_threshold_hi, 
                              this is avoid two apart appear. add by Jay.HF 2012-09-15 */
        input_report_abs(tmd2771_dev->input_dev, ABS_DISTANCE, 3);
    #if TAOS_DBG_EN
        printk(KERN_INFO"[Jay.HF]%s: prox(3), prox_threshold_hi(%d), prox_threshold_lo(%d)\n", 
               __func__, tmd2771_cfg.prox_threshold_hi, tmd2771_cfg.prox_threshold_lo);
    #endif
    }
    else
    {
        pro_buf[0] = tmd2771_cfg.prox_threshold_lo & 0x0ff;
        pro_buf[1] = tmd2771_cfg.prox_threshold_lo >> 8;
        pro_buf[2] = tmd2771_cfg.prox_threshold_hi & 0x0ff;
        pro_buf[3] = tmd2771_cfg.prox_threshold_hi >> 8;
        input_report_abs(tmd2771_dev->input_dev, ABS_DISTANCE, 4);
    #if TAOS_DBG_EN
        printk(KERN_INFO"[Jay.HF]%s: prox(4), prox_threshold_hi(%d), prox_threshold_lo(%d)\n", 
               __func__, tmd2771_cfg.prox_threshold_hi, tmd2771_cfg.prox_threshold_lo);
    #endif
    }

    input_sync(tmd2771_dev->input_dev);
    for (count=0; count<4; count++)
    { 
        if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x08) + count, pro_buf[count]))) < 0)
        {
            printk(KERN_ERR"[Jay.HF]%s: i2c fail \n", __func__);
            return (ret);
        }
    }

    tmd2771_dev->prox_on = 0;
    return ret;
}

static int taos_als_init_regs(void)
{
    int  ret = 0;
    u8 itime = 0, reg_val = 0, reg_cntrl = 0;

    if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|TAOS_TRITON_CMD_ALS_INTCLR)))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[1] \n", __func__);
        return (ret);
    }

    itime = (unsigned char)(((tmd2771_cfg.als_time*32)/87) - 1);
    itime = (~itime);
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_ALS_TIME), itime))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[2] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, 
                (TAOS_TRITON_CMD_REG|TAOS_TRITON_INTERRUPT), tmd2771_cfg.prox_intr_filter))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[3] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN)))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[4] \n", __func__);
        return (ret);
    }

    reg_val = i2c_smbus_read_byte(tmd2771_dev->client);
    reg_val = reg_val & 0xFC;
    reg_val = reg_val | (tmd2771_cfg.gain & 0x03);
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN), reg_val))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[5] \n", __func__);
        return (ret);
    }

    reg_cntrl = (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ALS_INT_ENBL);
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[6] \n", __func__);
        return (ret);
    }

    return ret;
}

static int taos_prox_init_regs(void)
{
    int ret = 0;
    unsigned char reg_cntrl = 0;

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x01), tmd2771_cfg.prox_int_time))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[4] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x02), tmd2771_cfg.prox_adc_time))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[5] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x03), tmd2771_cfg.prox_wait_time))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[6] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0C), tmd2771_cfg.prox_intr_filter))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[7] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0D), tmd2771_cfg.prox_config))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[8] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0E), tmd2771_cfg.prox_pulse_cnt))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[9] \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0F), tmd2771_cfg.prox_gain))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[10] \n", __func__);
        return (ret);
    }

    reg_cntrl = TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL | 
                TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_WAIT_TMR_ENBL;
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[11] \n", __func__);
        return (ret);
    }

    return ret;
}

static int taos_prox_poll(struct taos_prox_info *prxp)
{
    int i = 0;
    u8 chdata[6];

    for (i = 0; i < 6; i++)
    {
        chdata[i] = (i2c_smbus_read_byte_data(tmd2771_dev->client, 
                     (TAOS_TRITON_CMD_REG | TAOS_TRITON_CMD_AUTO | (TAOS_TRITON_ALS_CHAN0LO + i))));
    }
    
    prxp->prox_clear = chdata[1];
    prxp->prox_clear <<= 8;
    prxp->prox_clear |= chdata[0];

    if (prxp->prox_clear > ((tmd2771_dev->sat_als*80)/100))
        return -1;
    prxp->prox_data = chdata[5];
    prxp->prox_data <<= 8;
    prxp->prox_data |= chdata[4];

    return 0;
}

static int taos_cal_init_prox(void) {
    int ret = 0;
    u8 reg_cntrl = 0;

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x01), tmd2771_cfg.prox_int_time))) < 0) 
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 1 \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x02), tmd2771_cfg.prox_adc_time))) < 0) 
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 2 \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x03), tmd2771_cfg.prox_wait_time))) < 0)
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 3 \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0D), tmd2771_cfg.prox_config))) < 0) 
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 4 \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0E), tmd2771_cfg.prox_pulse_cnt))) < 0) 
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 5 \n", __func__);
        return (ret);
    }

    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|0x0F), tmd2771_cfg.prox_gain))) < 0)
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 6 \n", __func__);
        return (ret);
    }

    reg_cntrl = (TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ADC_ENBL);
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) 
    {
        printk(KERN_ERR "[Jay.HF]%s: i2c failed 7 \n", __func__);
        return (ret);
    }

    msleep(50);

    return ret;
}

static int taos_cal_set_prox_threshold(int prox_avg)
{
    if( prox_avg < 0)
    {
        printk(KERN_ERR "[Jay.HF]%s: prox_avg = [%d], data error ! \n", __func__, prox_avg);
    }
    else if( prox_avg < 100)
    {
        tmd2771_cfg.prox_threshold_hi = 200;
        tmd2771_cfg.prox_threshold_lo = 180;       
    }
    else if( prox_avg < 300)
    {
        tmd2771_cfg.prox_threshold_hi = prox_avg*2;
        tmd2771_cfg.prox_threshold_lo = tmd2771_cfg.prox_threshold_hi*4/5;
    }
    else if( prox_avg < 500)
    {
        tmd2771_cfg.prox_threshold_hi = prox_avg*9/5 + 60;
        tmd2771_cfg.prox_threshold_lo = tmd2771_cfg.prox_threshold_hi*4/5;
    }
    else if( prox_avg < 600)
    {
        tmd2771_cfg.prox_threshold_hi = 960;
        tmd2771_cfg.prox_threshold_lo = 800;
    }
    else 
    {
        printk(KERN_ERR "[Jay.HF]%s: Calibration fail, prox_avg = [%d] \n", __func__, prox_avg);
        return -1;
    }

    return 0;
}

static int taos_cal_recover_regs(void) {

    int ret = 0;
    u8 count = 0;

    if (tmd2771_dev->prox_enabled)
    {
        tmd2771_dev->prox_on = 1;

        /* init regs */
        ret = taos_prox_init_regs();
        if(ret)
        {
            printk(KERN_ERR"[Jay.HF]%s: prox_init_regs fail \n", __func__);
            return ret;
        }

        /* set PROX threshold */
        ret = taos_prox_threshold_set();
        if(ret)
        {
            printk(KERN_ERR"[Jay.HF]%s: prox_threshold_set fail \n", __func__);
            return ret;
        }

        tmd2771_dev->prox_enabled = 1;
    }
    else if(tmd2771_dev->als_enabled)
    {
        taos_als_init_regs();
        tmd2771_dev->als_enabled = 1;
    }
    else
    {
        if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) 
        {
            printk(KERN_ERR"[Jay.HF]%s: set ctl_reg fail \n", __func__);
            return (ret);
        }

        tmd2771_dev->prox_enabled = 0;
        cancel_work_sync(&tmd2771_dev->work);
        tmd2771_dev->als_enabled = 0;
        if (tmd2771_dev->polling && (count < 50))
        {
        #if TAOS_DBG_EN
            printk(KERN_ERR"[Jay.HF]%s: %d ms to power down \r\n", __func__, count);
        #endif
            count++;
            msleep(1);
        }
    #if TAOS_PWROFF_EN
        if(taos_set_power(false))
        {
            printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
            ret = -1;
        }
    #endif
    }

    return ret;
}

static irqreturn_t taos_irq_handler(int irq, void *dev_id)
{
    wake_lock(&tmd2771_dev->wake_lock); /* add by Jay.HF 2012-12-31 */
#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: ++++++++++++++++++++++++++++taos_irq_handler++++++++++++++++++++++++++++\n", __func__);
#endif
    tmd2771_dev->data_ready = 1;
    schedule_work(&tmd2771_dev->work);
    return IRQ_HANDLED;
}

static int taos_als_get_data(void)
{
    u8 reg_val;
    int ret = 0;
    int lux_val = 0;
    if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0)
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[1] \n", __func__);
        return (ret);
    }

    reg_val = i2c_smbus_read_byte(tmd2771_dev->client);
    if ((reg_val & (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON)) != (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON))
    {
        printk(KERN_ERR"[Jay.HF]%s: ALS NOT enabled or power on !\r\n", __func__);
        return -1;
    }

    if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_STATUS)))) < 0)
    {
        printk(KERN_ERR"[Jay.HF]%s: i2c fail[2] \r\n", __func__);
        return (ret);
    }

    reg_val = i2c_smbus_read_byte(tmd2771_dev->client);
    if ((reg_val & TAOS_TRITON_STATUS_ADCVALID) != TAOS_TRITON_STATUS_ADCVALID)
    {
        printk(KERN_ERR"[Jay.HF]%s: ALS had NOT been integrated!\r\n", __func__);
        return -1;
    }

    if ((lux_val = taos_get_lux()) < 0)
    {
        printk(KERN_ERR"[Jay.HF]%s: returned error [%d] \r\n", __func__, lux_val);
        return -1;
    }

#if 0
    if (lux_val < 40 && lux_val > 10)
        return ret;
#endif

    input_report_abs(tmd2771_dev->input_dev, ABS_MISC, lux_val);
    input_sync(tmd2771_dev->input_dev);

    return ret;
}

static int taos_get_data(void)
{
    int status;
    int ret = 0;
    if ((ret = (i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | 0x13)))) < 0)
    {
        printk(KERN_ERR"[Jay.HF]%s: get_data fail \r\n", __func__);
        return (ret);
    }
    
    status = i2c_smbus_read_byte(tmd2771_dev->client);
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: status[0x%x], prox->0x20, als->0x10\r\n", __func__, status);
#endif

    if (tmd2771_dev->prox_enabled && (status & 0x20))
    {
        ret = taos_prox_threshold_set();
        if (ret)
        {
            printk(KERN_ERR"[Jay.HF]%s: prox_threshold_set fail\r\n", __func__);
            return ret;
        }
    }
    else if(tmd2771_dev->als_enabled && (status & 0x10))
    {
        ret = taos_als_threshold_set();
        if (ret)
        {
            printk(KERN_ERR"[Jay.HF]%s: als_threshold fail\r\n", __func__);
            return ret;
        }

        ret = taos_als_get_data();
        if (ret)
        {
            printk(KERN_ERR"[Jay.HF]%s: als_get_data fail\r\n", __func__);
            return ret;
        }
    }

    return ret;
}

static int taos_clear_interrupts(void)
{
    int ret = 0;

    ret = i2c_smbus_write_byte(tmd2771_dev->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x07));
    if (ret < 0) 
    {
        printk(KERN_ERR"[Jay.HF]%s: taos_clear_interrupts fail\r\n", __func__);
        return (ret);
    }
    else
    {
        printk(KERN_ERR"[Jay.HF]%s: taos_clear_interrupts success\r\n", __func__);
    }
    tmd2771_dev->data_ready = 0;

    return ret;
}

static void taos_work_func(struct work_struct * work)
{
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s: suspend_flag(%d), prox_enabled(%d), als_enabled(%d)\r\n",
           __func__, tmd2771_dev->suspend_flag, tmd2771_dev->prox_enabled, tmd2771_dev->als_enabled);
#endif
    if ((!tmd2771_dev->suspend_flag) && (tmd2771_dev->prox_enabled || tmd2771_dev->als_enabled) && (!tmd2771_dev->polling))
    {
        tmd2771_dev->polling = 1;
        taos_get_data();
        tmd2771_dev->polling = 0;
        wake_lock_timeout(&tmd2771_dev->wake_lock_timeout, 2*HZ);
    }
    taos_clear_interrupts();
    wake_unlock(&tmd2771_dev->wake_lock);
}

static long taos_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0, i = 0;
    void __user *argp = (void __user *)arg;
    unsigned int phone_on = 0;
#if 0
    int prox_max = 0, prox_min = 1023;
#endif
    int prox_sum = 0, prox_avg = 0;
    u8 count = 0;
    u8 reg_tmp = 0;
    struct prox_offset taos_cal_data;
    struct prox_offset *taos_cal_ptr = &taos_cal_data;
#if TAOS_PARAM_EN
    u8 prox_param[3];
#endif
    switch (cmd)
    {
        case TAOS_IOCTL_ALS_ON:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: ALS_ON Entry\r\n", __func__);
        #endif
        #if TAOS_PWROFF_EN
            if (taos_set_power(true))
            {
                printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
                return -1;
            }
        #endif

            if (0 == tmd2771_dev->prox_enabled) 
            {
            #if TAOS_DBG_EN
                printk(KERN_ERR"[Jay.HF]%s: ALS_ON taos_als_init_regs() \n", __func__);
            #endif
                ret = taos_als_init_regs();
                if (ret) 
                {
                    printk(KERN_ERR"[Jay.HF]%s: als_init_regs fail \n", __func__);
                    return ret;
                }

                mdelay(5);

                ret = taos_als_default_threshold_set();
                if (ret)
                {
                    printk(KERN_ERR"[Jay.HF]%s: als_threshold_set fail \n", __func__);
                }
            }
            else if (tmd2771_dev->prox_enabled == 1) 
            {
                reg_tmp = i2c_smbus_read_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | 0x01));
                // tmd2771_dev->prox_on = 1; /* resolv twice continuously move away, add by Jay.HF 2012-11-09 9:33 */

                if (reg_tmp != tmd2771_cfg.prox_int_time)
                {
                    printk(KERN_ERR"[Jay.HF]%s: ctl_reg1[0x%x] might be error, reset prox!\n", __func__,reg_tmp);
                    
                    /* init regs */
                    ret = taos_prox_init_regs();
                    if (ret)
                    {
                        printk(KERN_ERR"[Jay.HF]%s: prox_init_regs fail \n", __func__);
                        return ret;
                    }

                    mdelay(5);
                    
                    /* set prox threshold */
                    ret = taos_prox_threshold_set();
                    if (ret)
                    {
                        printk(KERN_ERR"[Jay.HF]%s: prox_threshold_set fail \n", __func__);
                        return ret;
                    }
                }
                tmd2771_dev->prox_enabled = 1;
            }
            taos_clear_interrupts(); /* Clear ALS or PROX Interrupt Flag, By Jay.HF 2012-10-17 */
            tmd2771_dev->als_enabled = 1;
        /* update tmd2771 power state, modify by Jay.HF 2012-12-21 */
        #ifdef CONFIG_YULONG_MODULE_MANAGEMENT
            tmd2771_dev->pm_state = STATUS_ON;
        #endif

        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: ALS_ON Exit\r\n", __func__);
        #endif
            break;

        case TAOS_IOCTL_ALS_OFF:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: ALS_OFF Entry\r\n", __func__);
        #endif
            tmd2771_dev->als_enabled = 0;

            if (0 == tmd2771_dev->prox_enabled) 
            {
                if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) 
                {
                    printk(KERN_ERR"[Jay.HF]%s: clear ALS regs\n", __func__);
                    return (ret);
                }

                cancel_work_sync(&tmd2771_dev->work);

                if (tmd2771_dev->polling && (count < 50))
                {
                #if TAOS_DBG_EN
                    printk(KERN_ERR"[Jay.HF]%s: %d ms\r\n", __func__, count);
                #endif
                    count++;
                    msleep(1);
                }
            #if TAOS_PWROFF_EN 
                if(taos_set_power(false))
                {
                    printk(KERN_ERR"[Jay.HF]%s: set_power fail \r\n", __func__);
                    ret = -1;
                }
            #endif
                tmd2771_dev->data_ready = 0;
            }
            /* update tmd2771 power state, modify by Jay.HF 2012-12-21 */
        #ifdef CONFIG_YULONG_MODULE_MANAGEMENT
            tmd2771_dev->pm_state = STATUS_OFF;
        #endif

        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: ALS_OFF Exit\r\n", __func__);
        #endif
            break;

        case TAOS_IOCTL_PROX_ON:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_ON Entry\r\n", __func__);
        #endif
            tmd2771_dev->prox_on = 1;
        #if TAOS_PWROFF_EN
            if (taos_set_power(true))
            {
                printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
                return -1;
            }
        #endif
            ret = taos_prox_init_regs();
            if (ret)
            {
                printk(KERN_ERR"[Jay.HF]%s: prox_init_regs fail \n", __func__);
                return ret;
            }
            msleep(5);

        #if TAOS_PARAM_EN /* Jay.HF add for yulong-area 20120924 start */
            taos_read_param_from_flash(prox_param);
        #if TAOS_DBG_EN
            printk(KERN_ERR"[Jay.HF]%s: %d, %d, %d\n", __func__, prox_param[0], prox_param[1], prox_param[2]);
        #endif
            if (0x0A == prox_param[0])
            {
            #if TAOS_DBG_EN
                printk(KERN_ERR"[Jay.HF]%s: read from yl_param\n", __func__);
            #endif
                tmd2771_cfg.prox_threshold_hi = (prox_param[2] << 8) | prox_param[1];
            }
            else
            {
            #if TAOS_DBG_EN
                printk(KERN_ERR"[Jay.HF]%s: use default value\n", __func__);
            #endif
                tmd2771_cfg.prox_threshold_hi = TAOS_PROX_THRESHOLD_Hi; // 550; // 700; /* prox_threshold_hi_param */
                tmd2771_cfg.prox_threshold_lo = TAOS_PROX_THRESHOLD_Lo; // 400; // 550; /* prox_threshold_lo_param */
            }

            if (200 < tmd2771_cfg.prox_threshold_hi && tmd2771_cfg.prox_threshold_hi < 900)
            {
                tmd2771_cfg.prox_threshold_lo = 9*tmd2771_cfg.prox_threshold_hi/10;
            }
            else
            {
                tmd2771_cfg.prox_threshold_hi = TAOS_PROX_THRESHOLD_Hi; // 550; // 700; /* prox_threshold_hi_param */
                tmd2771_cfg.prox_threshold_lo = TAOS_PROX_THRESHOLD_Lo; // 400; // 550; /* prox_threshold_lo_param */
            }
        #if TAOS_DBG_EN
            printk(KERN_ERR"[Jay.HF]%s: prox_threshold_hi = %d, prox_threshold_lo = %d\n",
                   __func__, tmd2771_cfg.prox_threshold_hi, tmd2771_cfg.prox_threshold_lo);
        #endif
        #endif /* Jay.HF add for yulong-area 20120924 end */

            /* set prox threshold */
            ret = taos_prox_threshold_set();
            if (ret)
            {
                printk(KERN_ERR"[Jay.HF]%s: prox_threshold_set fail \n", __func__);
                return ret;
            }

            taos_clear_interrupts(); /* Clear ALS or PROX Interrupt Flag, By Jay.HF 2012-09-24 */
            ret = irq_set_irq_wake(tmd2771_dev->als_ps_int, 1);
            if (ret != 0)
            {
                printk(KERN_ERR"[Jay.HF]%s: set irq wake source failed! \n", __func__);
                return(ret);
            }
            tmd2771_dev->prox_enabled = 1;
        /* update tmd2771 power state, modify by Jay.HF 2012-12-21 */
        #ifdef CONFIG_YULONG_MODULE_MANAGEMENT
            tmd2771_dev->pm_state = STATUS_ON;
        #endif

        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_ON Exit\r\n", __func__);
        #endif
            break;

        case TAOS_IOCTL_PROX_OFF:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_OFF Entry\r\n", __func__);
        #endif

            if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) 
            {
                printk(KERN_ERR"[Jay.HF]%s: set ctl_reg fail \n", __func__);
                return (ret);
            }
            
            tmd2771_dev->prox_enabled = 0;
            ret = irq_set_irq_wake(tmd2771_dev->als_ps_int, 0);
            if (ret != 0)
            {
                printk(KERN_ERR"[Jay.HF]%s: clear irq wake source failed ! \n", __func__);
                return(ret);
            }

            if (tmd2771_dev->als_enabled == 1)
            {
                taos_als_init_regs();
                msleep(5);
                ret = taos_als_default_threshold_set();
                if(ret)
                {
                    printk(KERN_ERR"[Jay.HF]%s: als_threshold_set fail \n", __func__);
                }

                tmd2771_dev->als_enabled = 1;
            } 
            else
            {
                cancel_work_sync(&tmd2771_dev->work);
                tmd2771_dev->als_enabled = 0;

                if (tmd2771_dev->polling && (count < 50))
                {
                #if TAOS_DBG_EN
                    printk(KERN_ERR"[Jay.HF]%s: %d ms to power down \r\n", __func__, count);
                #endif
                    count++;
                    msleep(1);
                }
            #if TAOS_PWROFF_EN 
                if(taos_set_power(false))
                {
                    printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
                    ret = -1;
                }
            #endif
                tmd2771_dev->data_ready = 0;
            }
            tmd2771_dev->prox_on = 0;
        /* update tmd2771 power state, modify by Jay.HF 2012-12-21 */
        #ifdef CONFIG_YULONG_MODULE_MANAGEMENT
            tmd2771_dev->pm_state = STATUS_OFF;
        #endif

        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_OFF Exit\r\n", __func__);
        #endif
            break;

        case TAOS_IOCTL_PHONE_STATE:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PHONE_STATE!\r\n", __func__);
        #endif

            if (copy_from_user(&phone_on, argp, sizeof(phone_on)))
            {
                printk(KERN_ERR"[Jay.HF]copy failed(%s),line(%d)\n", __FUNCTION__, __LINE__);
                return -EFAULT;
            }

            if (!(phone_on & 0xFE))
            {
                if (0x01 == phone_on)
                {
                    tmd2771_dev->phone_state = 0x01;
                #if TAOS_DBG_EN
                    printk(KERN_ERR"[Jay.HF]%s: set tmd2771_dev->phone_state ++ \r\n", __func__);
                #endif
                }
                else if(0x00 == phone_on) 
                {
                    tmd2771_dev->phone_state = 0x00;
                #if TAOS_DBG_EN
                    printk(KERN_ERR"[Jay.HF]%s: set tmd2771_dev->phone_state -- \r\n", __func__);
                #endif
                }
            }
            break;

        case TAOS_IOCTL_PROX_CALIBRATE:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_CALIBRATE\r\n", __func__);
        #endif
        #if TAOS_PWROFF_EN
            if(taos_set_power(true))
            {
                printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
                return -1;
            }
        #endif
            if ((struct prox_offset *)arg != NULL)
            {
                prox_sum = 0;
                wake_lock(&tmd2771_dev->wake_lock);
                ret = taos_cal_init_prox();
                if (ret) 
                {
                    printk(KERN_ERR"[Jay.HF]%s: regs for caling failed \n", __func__);
                    taos_cal_recover_regs();
                    wake_unlock(&tmd2771_dev->wake_lock);
                    return (ret);
                }

                for (i = 0; i < 20; i++) 
                {
                    msleep(50);
                    if ((ret = taos_prox_poll(&prox_cal_info[i])) < 0) 
                    {
                        printk(KERN_ERR"[Jay.HF]%s: call to prox_poll failed in ioctl prox_calibrate\n", __func__);
                        taos_cal_recover_regs();
                        wake_unlock(&tmd2771_dev->wake_lock);
                        return (ret);
                    }
                    prox_sum += prox_cal_info[i].prox_data;
                #if 0
                    if (prox_cal_info[i].prox_data > prox_max)
                       prox_max = prox_cal_info[i].prox_data;
                    if (prox_cal_info[i].prox_data < prox_min)
                       prox_min = prox_cal_info[i].prox_data;
                    printk(KERN_ERR"[Jay.HF]%s: pdata[%2d] = [%5d] \n", __func__, i, prox_cal_info[i].prox_data);
                #endif
                }

                prox_avg = prox_sum / 20;
                taos_cal_set_prox_threshold(prox_avg);

            #if TAOS_PARAM_EN /* add for new-yulong-area calibrate, by Jay.HF 20120924 start */
            #if TAOS_DBG_EN
                printk(KERN_ERR"[Jay.HF]%s: calibrator prox_threshold_hi = %d, prox_threshold_lo = %d\n",
                       __func__, tmd2771_cfg.prox_threshold_hi, tmd2771_cfg.prox_threshold_lo);
            #endif
                memset(prox_param, 0, sizeof(prox_param));
                prox_param[0] = 0x0A;
                prox_param[1] = tmd2771_cfg.prox_threshold_hi & 0x00ff;
                prox_param[2] = (tmd2771_cfg.prox_threshold_hi & 0xff00) >> 8;
                taos_write_param_to_flash(prox_param);
            #endif /* add for new-yulong-area calibrate, by Jay.HF 20120924 end */

            #if TAOS_DBG_EN
                printk(KERN_INFO"[Jay.HF]%s: prox_hig = [%5d] \n", __func__, tmd2771_cfg.prox_threshold_hi);
                printk(KERN_INFO"[Jay.HF]%s: prox_low = [%5d] \n", __func__, tmd2771_cfg.prox_threshold_lo);
                printk(KERN_INFO"[Jay.HF]%s: prox_avg = [%5d] \n", __func__, prox_avg);
            #endif

                ((struct prox_offset *)taos_cal_ptr)->x = (unsigned short)(tmd2771_cfg.prox_threshold_hi);
                ((struct prox_offset *)taos_cal_ptr)->y = (unsigned short)(tmd2771_cfg.prox_threshold_lo);
                ((struct prox_offset *)taos_cal_ptr)->z = (unsigned short)(prox_avg);

                if(copy_to_user((struct prox_offset *)arg, taos_cal_ptr, sizeof(taos_cal_data)))
                {
                    printk(KERN_ERR"[Jay.HF]%s: data trans error,use default offset ! \r\n", __func__);
                }

                taos_cal_recover_regs();
                wake_unlock(&tmd2771_dev->wake_lock);
            }
            else
            {
                printk(KERN_ERR"[Jay.HF]%s: (%d) null pointer !\r\n", __func__, __LINE__);
                return -1;
            }
            break;

        case TAOS_IOCTL_PROX_OFFSET:
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: PROX_OFFSET\r\n", __func__);
        #endif
            if (((struct prox_offset *)arg == NULL ) || copy_from_user(taos_cal_ptr, (struct prox_offset *)argp, sizeof(taos_cal_data)))
            {
                printk(KERN_ERR"[Jay.HF]%s: data trans error, use default offset ! \r\n", __func__);
                break;
            }

            tmd2771_cfg.prox_threshold_hi = (unsigned short)(((struct prox_offset *)taos_cal_ptr)->x);
            tmd2771_cfg.prox_threshold_lo = (unsigned short)(((struct prox_offset *)taos_cal_ptr)->y);
        #if TAOS_DBG_EN
            printk(KERN_INFO"[Jay.HF]%s: prox_hig = [%5d] \n", __func__, tmd2771_cfg.prox_threshold_hi);
            printk(KERN_INFO"[Jay.HF]%s: prox_low = [%5d] \n", __func__, tmd2771_cfg.prox_threshold_lo);
        #endif
            break;

        default:
            printk(KERN_ERR"[Jay.HF]%s: DEFAULT!\r\n", __func__);
            ret = -1;
            break;
    }

    return (ret);
}

static int taos_open(struct inode *inode, struct file *file) 
{
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s\r\n", __func__);
#endif
    return (0);
}

static int taos_release(struct inode *inode, struct file *file) 
{
#if TAOS_DBG_EN
    printk(KERN_ERR"[Jay.HF]%s\r\n", __func__);
#endif
    return (0);
}

static struct file_operations tmd2771_fops = {
    .owner          = THIS_MODULE,
    .open           = taos_open,
    .release        = taos_release,
    .unlocked_ioctl = taos_ioctl,
};

static struct miscdevice tmd2771_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = TAOS_DEVICE_NAME,
    .fops  = &tmd2771_fops,
};

static int taos_suspend(struct i2c_client *client, pm_message_t mesg)
{
    int ret = 0;
#if TAOS_SUSPEND_SLEEP_EN
    u8 reg_val = 0;
    u8 reg_cntrl = 0;
#endif

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s\r\n", __func__);
#endif
    printk(KERN_INFO"[Jay.HF]%s\r\n", __func__);
    taos_clear_interrupts(); /* Clear ALS or PROX Interrupt Flag, By Jay.HF 2012-12-27 */
#if TAOS_SUSPEND_SLEEP_EN
    if (!(tmd2771_dev->prox_enabled)) {
        printk(KERN_INFO"[Jay.HF]%s: tmd2771 power off begain\n", __func__);
        /* Jay.HF add for low-power-mode 20121108 */
        if ((ret = (i2c_smbus_write_byte(client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0)
        {
            printk(KERN_ERR"[Jay.HF]%s: i2c_smbus_write_byte failed 1\n", __func__);
            return 0;
        }
        reg_val = i2c_smbus_read_byte(client);
        if (reg_val & TAOS_TRITON_CNTL_PWRON) 
        {
            reg_cntrl = reg_val & (~TAOS_TRITON_CNTL_PWRON);
            if ((ret = (i2c_smbus_write_byte_data(client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) 
            {
                printk(KERN_ERR"[Jay.HF]%s: i2c_smbus_write_byte_data failed 2\n", __func__);
                return 0;
            }
            tmd2771_dev->working = 1;
            cancel_work_sync(&tmd2771_dev->work);
            printk(KERN_INFO"[Jay.HF]%s: tmd2771 power off success\n", __func__);
        }
    }
#endif
    tmd2771_dev->suspend_flag = 1;
    return ret;
}

static int taos_resume(struct i2c_client *client)
{
    int ret = 0;
#if TAOS_SUSPEND_SLEEP_EN
    u8 reg_val = 0;
    u8 reg_cntrl = 0;
#endif

    tmd2771_dev->suspend_flag = 0;

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s\r\n", __func__);
#endif
    printk(KERN_INFO"[Jay.HF]%s\r\n", __func__);
#if TAOS_SUSPEND_SLEEP_EN
    if (!(tmd2771_dev->prox_enabled)) {
        /* Jay.HF add for low-power-mode 20121108 */
        if ((ret = (i2c_smbus_write_byte(client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0)
        {
            printk(KERN_ERR"[Jay.HF]%s: i2c_smbus_write_byte failed 1\n", __func__);
            return 0;
        }
        reg_val = i2c_smbus_read_byte(client);
    		if (tmd2771_dev->working == 1)
        {
            reg_cntrl = reg_val | TAOS_TRITON_CNTL_PWRON;
            if ((ret = (i2c_smbus_write_byte_data(client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0)
            {
                printk(KERN_ERR"[Jay.HF]%s: i2c_smbus_write_byte failed 2\n", __func__);
                return 0;
            }
            tmd2771_dev->working = 0;
            printk(KERN_INFO"[Jay.HF]%s: tmd2771 power on success\n", __func__);
        }
    }
#endif
    if ((tmd2771_dev->prox_enabled || tmd2771_dev->als_enabled) && tmd2771_dev->data_ready) {
    #if TAOS_DBG_EN
        printk(KERN_ERR"[Jay.HF]%s: prox_enabled(%d), als_enabled(%d), data_ready(%d)\r\n",
               __func__, tmd2771_dev->prox_enabled, tmd2771_dev->als_enabled, tmd2771_dev->data_ready);
    #endif
        wake_lock_timeout(&tmd2771_dev->wake_lock_timeout, 2*HZ);
        taos_get_data();
    }
    taos_clear_interrupts(); /* add by Jay.HF 2012-09-24 */
    return ret;
}

#if TAOS_EARLY_SUSPEND_EN
static void taos_early_suspend(struct early_suspend *h)
{
    struct taos_dev *tmd2771;
#if TAOS_DBG_EN
    printk(KERN_INFO "[Jay.HF]%s\n", __func__);
#endif
    tmd2771 = container_of(h, struct taos_dev, early_suspend);
    taos_suspend(tmd2771->client, PMSG_SUSPEND);
}

static void taos_late_resume(struct early_suspend *h)
{
    struct taos_dev *tmd2771;
#if TAOS_DBG_EN
    printk(KERN_INFO "[Jay.HF]%s\n", __func__);
#endif
    tmd2771 = container_of(h, struct taos_dev, early_suspend);
    taos_resume(tmd2771->client);
    return;
}
#endif

#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
extern int yulong_register_module_management(struct module_management *handler);
extern int yulong_unregister_module_management(struct module_management *handler);

static int taos_alsprox_enable(struct taos_dev *alsprox)
{
    if (NULL == alsprox) {
        printk(KERN_INFO"[Jay.HF]%s: alsprox is NULL\r\n", __func__);
        return -1;
    }
#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: alsprox enable\r\n", __func__);
#endif
#if TAOS_PWROFF_EN
    if (taos_set_power(true)) {
        printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
        return -1;
    }
#endif
    if (taos_prox_init_regs()) {
        printk(KERN_ERR"[Jay.HF]%s: prox_init_regs fail \n", __func__);
        return -1;
    }
    msleep(5);
    alsprox->prox_on = 1;
    taos_prox_threshold_set();
    taos_clear_interrupts();
    irq_set_irq_wake(alsprox->als_ps_int, 1);
    alsprox->prox_enabled = 1;
    alsprox->pm_state = STATUS_ON; /* update the pm state */
    return 0;
}

static int taos_alsprox_disable(struct taos_dev *alsprox)
{
    int ret = 0;
    int count = 0;
    if (NULL == alsprox) {
        printk(KERN_INFO"[Jay.HF]%s: alsprox is NULL\r\n", __func__);
        return -1;
    }
#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: alsprox disable\r\n", __func__);
#endif
    if ((ret = (i2c_smbus_write_byte_data(alsprox->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
        printk(KERN_ERR"[Jay.HF]%s: set ctl_reg fail \n", __func__);
        return (ret);
    }

    ret = irq_set_irq_wake(alsprox->als_ps_int, 0);
    if (ret != 0) {
        printk(KERN_ERR"[Jay.HF]%s: clear irq wake source failed ! \n", __func__);
        return(ret);
    }
    cancel_work_sync(&alsprox->work);
    if (alsprox->polling && (count < 50)) {
    #if TAOS_DBG_EN
        printk(KERN_ERR"[Jay.HF]%s: %d ms to power down \r\n", __func__, count);
    #endif
        count++;
        msleep(1);
    }
#if TAOS_PWROFF_EN 
    if(taos_set_power(false)) {
        printk(KERN_ERR"[Jay.HF]%s: set power fail \r\n", __func__);
        return -1;
    }
#endif
    alsprox->prox_enabled = 0;
    alsprox->als_enabled = 0;
    alsprox->data_ready = 0;
    alsprox->prox_on = 0;
    alsprox->pm_state = STATUS_OFF; /* update the pm state */
    return 0;
}

static int taos_alsprox_pm_control(struct module_management *handler, unsigned int cmd)
{
    int err = -1;
    struct taos_dev *alsprox = NULL;
    if (NULL == handler) {
        printk(KERN_ERR "[Jay.HF]%s: handler is NULL.\n", __func__);
        return -1;
    }
    alsprox = (struct taos_dev *)(handler->private_data);
    if (NULL == alsprox) {
        printk(KERN_ERR "[Jay.HF]%s: alsprox_data is NULL.\n", __func__);
        return -1;
    }
    switch(cmd)
    {
        case SET_MODULE_ON:
            printk(KERN_ERR "[Jay.HF]%s: cmd = SET_MODULE_ON \n", __func__);
            err = taos_alsprox_enable(alsprox);
            break;
            
        case SET_MODULE_OFF:
            printk(KERN_ERR "[Jay.HF]%s: cmd = SET_MODULE_OFF \n", __func__);
            err = taos_alsprox_disable(alsprox);
            break;
            
        default:
            printk(KERN_ERR "[Jay.HF]%s: cmd is not found \n", __func__);
            return -1;
    }
    return err;
}

static int taos_alsprox_pm_get(struct module_management *handler)
{
    int state;
    struct taos_dev *alsprox = NULL;
    if (NULL == handler) {
        printk(KERN_ERR "[Jay.HF]%s: handler is NULL.\n", __func__);
        return -1;
    }
    alsprox = (struct taos_dev *)(handler->private_data);
    if (NULL == alsprox) {
        printk(KERN_ERR "[Jay.HF]%s: alsprox_data is NULL.\n", __func__);
        return -1;
    }
    state = alsprox->pm_state;
    return state;
}

static struct module_management alsprox_pm = {
    .name = "alsprox",
    .get = taos_alsprox_pm_get,
    .control = taos_alsprox_pm_control,
};
#endif

static int __devinit taos_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    int chip_id;

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: start\r\n", __func__);
#endif
    if (!client) {
        printk(KERN_ERR"[Jay.HF]%s: i2c client null pointer! \n", __func__);
        ret = -1;
        goto out;
    }
    if (taos_set_power(true)) {
        printk(KERN_ERR"[Jay.HF]%s: taos_set_power fail \r\n", __func__);
        ret = -1;
        goto out;
    }
    msleep(10);

    /* check i2c bus */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA)) {
        printk(KERN_ERR"[Jay.HF]%s: i2c check failed \r\n", __func__);
        ret = -1;
        goto out_power_off;
    }
    /* check taos chip id */
    chip_id = i2c_smbus_read_byte_data(client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CHIPID));
    if (chip_id != CHIP_ID) {
        printk(KERN_ERR"[Jay.HF]%s: error chip_id = %d\n", __func__, chip_id);
        ret = -1;
        goto out_power_off;
    }

    tmd2771_dev = kzalloc(sizeof(struct taos_dev), GFP_KERNEL);
    if (NULL == tmd2771_dev) {
        printk(KERN_ERR"[Jay.HF]%s: allocate tmd2771 dev fail \r\n", __func__);
        ret = -1;
        goto out_power_off;
    }
    memset(tmd2771_dev, 0, sizeof(struct taos_dev));
    tmd2771_dev->client = client;
    
    wake_lock_init(&tmd2771_dev->wake_lock,         WAKE_LOCK_SUSPEND, "taos_wake_lock");
    wake_lock_init(&tmd2771_dev->wake_lock_timeout, WAKE_LOCK_SUSPEND, "taos_wake_lock_timeout");
    INIT_WORK(&(tmd2771_dev->work), taos_work_func);
    tmd2771_dev->prox_on      = 0;
    tmd2771_dev->prox_enabled = 0;
    tmd2771_dev->als_enabled  = 0;
    tmd2771_dev->suspend_flag = 0;
    tmd2771_dev->polling      = 0;

    /* 鍒涘缓骞跺垵濮嬪寲input璁惧 */
    tmd2771_dev->input_dev = input_allocate_device();
    if (tmd2771_dev->input_dev == NULL) {
        printk(KERN_ERR"[Jay.HF]%s: allocate input device fail !\r\n", __func__);
        ret = -1;
        goto out_free_dev;
    }

    tmd2771_dev->input_dev->name = TAOS_INPUT_NAME;
    tmd2771_dev->input_dev->id.bustype = BUS_I2C;
    set_bit(EV_ABS, tmd2771_dev->input_dev->evbit);
    input_set_capability(tmd2771_dev->input_dev, EV_ABS, ABS_DISTANCE);
    input_set_capability(tmd2771_dev->input_dev, EV_ABS, ABS_MISC);
    /* light Lux data */
    input_set_abs_params(tmd2771_dev->input_dev, ABS_MISC, 0, 100000, 0, 0);
    /* proximity data */
    input_set_abs_params(tmd2771_dev->input_dev, ABS_DISTANCE, 0, 1000, 0, 0);
    ret = input_register_device(tmd2771_dev->input_dev);
    if (ret != 0) {
        printk(KERN_ERR"[Jay.HF]%s: input_register_device failed ! \n", __func__);
        goto out_free_input;
    }

    tmd2771_dev->sat_als  = (256-tmd2771_cfg.prox_int_time)<<10;
    tmd2771_dev->sat_prox = (256-tmd2771_cfg.prox_adc_time)<<10;
    if ((ret = (i2c_smbus_write_byte_data(tmd2771_dev->client, \
                    (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
        printk(KERN_ERR"[Jay.HF]%s: i2c_write failed in power down\n", __func__);
        goto out_unregister_input;
    }

    gpio_request(TAOS_GPIO_IRQ, "taos_irq");
    gpio_direction_input(TAOS_GPIO_IRQ);
    tmd2771_dev->als_ps_int = tmd2771_dev->client->irq;
#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: request irq [%d] \r\n", __func__, tmd2771_dev->als_ps_int);
#endif
    // ret = request_threaded_irq(tmd2771_dev->als_ps_int, NULL, taos_irq_handler, IRQF_TRIGGER_FALLING, "taos_irq", NULL);
    ret = request_irq(tmd2771_dev->als_ps_int, taos_irq_handler, IRQF_TRIGGER_FALLING, "taos_irq", NULL);
    if (ret != 0) {
        printk(KERN_ERR"[Jay.HF]%s: request_irq fail, releasing irq... \r\n", __func__);
        goto out_unregister_input;
    }

#if TAOS_EARLY_SUSPEND_EN
#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: early_suspend register start!\n", __func__);
#endif
    tmd2771_dev->early_suspend.level   = EARLY_SUSPEND_LEVEL_BLANK_SCREEN+2; //EARLY_SUSPEND_LEVEL_STOP_DRAWING; 
    tmd2771_dev->early_suspend.suspend = taos_early_suspend;
    tmd2771_dev->early_suspend.resume  = taos_late_resume;
    register_early_suspend(&tmd2771_dev->early_suspend);
#endif

    ret = misc_register(&tmd2771_device);
    if (ret) {
        printk(KERN_ERR"[Jay.HF]%s: tmd2771_device register failed \r\n", __FUNCTION__);
        goto out_free_irq;
    }
#if TAOS_PWROFF_EN
    ret = taos_set_power(false);
    if( 0 != ret ) {
        printk(KERN_ERR"[Jay.HF]%s: taos_set_power(false) fail \r\n", __func__);
        goto out_deregister_misc;
    }
#endif

/* register the power manager module, add by Jay.HF 2012-12-07 */
#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
    tmd2771_dev->pm_state = STATUS_OFF;
    alsprox_pm.private_data = (void *)tmd2771_dev;
    yulong_register_module_management(&alsprox_pm);
#endif
/* end */

#if TAOS_DBG_EN
    printk(KERN_INFO"[Jay.HF]%s: probe success\r\n", __func__);
#endif
    return 0;

#if TAOS_PWROFF_EN
out_deregister_misc:
    misc_deregister(&tmd2771_device);
#endif
out_free_irq:
    free_irq(tmd2771_dev->als_ps_int, NULL);
out_unregister_input:
    input_unregister_device(tmd2771_dev->input_dev);
out_free_input:
    input_free_device(tmd2771_dev->input_dev);
out_free_dev:
    kfree(tmd2771_dev);
out_power_off:
#if TAOS_PWROFF_EN
    if (taos_set_power(false)) {
        printk(KERN_ERR"[Jay.HF]%s: taos_set_power(false) failed.\r\n", __func__);
    }
#endif
out:
    return ret;
}

static int __devexit taos_remove(struct i2c_client *client) 
{
    int ret = 0;
#ifdef CONFIG_YULONG_MODULE_MANAGEMENT
    /* unregister the power manager module, add by Jay.HF 2012-12-07 */
    yulong_unregister_module_management(&alsprox_pm);
    /* end */
#endif

#if TAOS_EARLY_SUSPEND_EN
    unregister_early_suspend(&tmd2771_dev->early_suspend);
#endif
    misc_deregister(&tmd2771_device);
    free_irq(tmd2771_dev->als_ps_int, NULL);
    input_unregister_device(tmd2771_dev->input_dev);
    input_free_device(tmd2771_dev->input_dev);
    kfree(tmd2771_dev);
#if TAOS_PWROFF_EN
    if(taos_set_power(false)) {
        printk(KERN_ERR"[Jay.HF]%s: taos_set_power(false) failed.\r\n", __func__);
        ret = -1;
    }
#endif
    return (ret);
}

static const struct i2c_device_id taos_id[] = {
    { TAOS_DRIVER_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, taos_id);

static struct i2c_driver tmd2771_driver = {
    .driver = {
        .name  = TAOS_DRIVER_NAME,
        .owner = THIS_MODULE,
    },
#if !TAOS_EARLY_SUSPEND_EN
    .suspend   = taos_suspend,
    .resume    = taos_resume,
#endif
    .probe     = taos_probe,
    .remove    = __devexit_p(taos_remove),
    .id_table  = taos_id,
};

static int __init taos_init(void)
{
    int ret;
    ret = taos_power_init();
    if (0 != ret) {
        return ret;
    }

    ret = i2c_add_driver(&tmd2771_driver);
    return ret;
}

static void __exit taos_exit(void)
{
    i2c_del_driver(&tmd2771_driver);
    taos_power_deinit();
}

MODULE_AUTHOR("Jay.HF@CoolPad");
MODULE_DESCRIPTION("TAOS Light and Proximity (L&P) Sensor Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

module_init(taos_init);
module_exit(taos_exit);

