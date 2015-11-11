/*
 *  al3006.c - Linux kernel modules for ambient light sensor
 *
 *  Copyright (C) 2007 Rodolfo Giometti <giometti@linux.it>
 *  Copyright (C) 2007 Eurotech S.p.A. <info@eurotech.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/input.h>

#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <asm/irq.h>
#include <linux/irq.h>
#include <linux/regulator/consumer.h>
#include <linux/cdev.h>
#include <linux/al3006.h>
#include <linux/earlysuspend.h>
#include <asm/uaccess.h>

/*
 * Defines
 */
#define TEST_MODE

enum {
    light = 0,
    proxi,
    dev_num
};

/*
 * Structs
 */ 
int csensor_major = CSENSOR_MAJOR; /* dynamic major by default */
int csensor_minor = 0; /* dynamic minor device number */
int csensor_nr_al3006_devs = CSENSOR_NR_AL3006_DEVS; /* number of al3006 device */

static atomic_t light_flag;
static atomic_t light_lock_flag;
static atomic_t proxi_flag;
static atomic_t proxi_lock_flag;
static atomic_t test_mode_flag;

unsigned int al3006_irq = 0;
unsigned int al3006_major;
unsigned int al3006_err_count = 0;

static unsigned char g_light_active_flag = 0;
static unsigned char g_proxi_active_flag = 0;
static unsigned char g_is_al3006_suspend = 0;

#ifdef TEST_MODE
static struct timer_list al3006_timer;
#endif

ALSDATA  g_adls;
ALSDATA *g_pdls = &g_adls; /* g_pdls->light 表示light sensor data */

PSDATA   g_adps;
PSDATA  *g_pdps = &g_adps; /* g_pdps->proxi 表示proximity sensor data */

struct al3006_device {
    // const struct al3006_platform_data *pdata;
    struct input_dev    *ip_dev;
    struct input_dev    *input_light_dev;
    struct input_dev    *input_proxi_dev;
    struct i2c_client   *i2c_client;
    struct work_struct   work_data;
#ifdef TEST_MODE
    struct work_struct   work_testmode;
#endif
    struct mutex         al3006_mutex;
    struct early_suspend early_suspend; /* suspend function */
};

#if 0
struct al3006_sensor_data {
    char ps_data;
    char als_data;
};
#endif

static struct al3006_device *al3006_dev = NULL;

#if 0
static DEFINE_MUTEX(dlsps_lock);
#endif

struct AL3006_DEV {
    struct cdev  al3006_cdev;
    struct mutex al3006_mutex;                /*add mutex for i2c read and write protected*/
};

static struct AL3006_DEV *pal3006_dev;

static struct regulator *pm8901_l1_3v3;
static struct regulator *pm8058_s3_1v8;

static int al3006_rxdata(char *rxData, int length)
{
    struct i2c_msg msgs[] = {
        {
            .addr  = al3006_dev->i2c_client->addr,
            .flags = 0,
            .len   = 1,
            .buf   = rxData,
        },
        {
            .addr  = al3006_dev->i2c_client->addr,
            .flags = I2C_M_RD,
            .len   = length,
            .buf   = rxData,
         },
    };

    if (i2c_transfer(al3006_dev->i2c_client->adapter, msgs, 2) < 0) {
        // pr_info("AL3006 al3006_rxdata: transfer error\n");
        return -EIO;
    }

    return 0;
}

static int al3006_txdata(char *txData, int length)
{
    struct i2c_msg msg[] = {
        {
            .addr  = al3006_dev->i2c_client->addr,
            .flags = 0,
            .len   = length,
            .buf   = txData,
         },
    };

    if (i2c_transfer(al3006_dev->i2c_client->adapter, msg, 1) < 0) {
        //pr_info("AL3006 al3006_txdata: transfer error\n");
        return -EIO;
    }

    return 0;
}

/*
 * Initialization function
 */
static int al3006_hardware_init(void)
{
    int err;
    char ii=0;
    char buffer[2];
// power set 
    printk(KERN_ERR" AL3006 POWER on \r\n");
    err = regulator_enable(pm8901_l1_3v3);
    if (err) {
        printk(KERN_ERR"%s: error enable pm8901_l1_3v3 failed \n", __func__);
        return err;
    }
    err = regulator_enable(pm8058_s3_1v8);
    if (err) {
        printk(KERN_ERR"%s: error enable pm8058_s3_1v8 failed \n", __func__);
        return err;
    }
//
    pr_info(  " %s \r\n",__FUNCTION__);
    // set AL3002 in power down mode and idle mode 
    buffer[0] = AL3006_CONF;
    buffer[1] = 0x0b;
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        {
        printk(KERN_ERR"++++1");
        return err;
        }    
    udelay(500);
    //pr_info("Write data: AL3006_CONF=0x0b\n");

    /* Initial timing control register 0x01 
     DPS Persist = 4
     Integration Time = 100ms
     DLS Persist = 4    
     Should be modified base on customer's specifications
  */
    buffer[0] = AL3006_TIMING_CTL;
    buffer[1] = 0x11;
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        {
        printk(KERN_ERR"++++2");
        return err;
        }
        //return err;
    udelay(500);
    //pr_info("Write data: AL3006_TIMING_CTL=0x00\n");


    /* Initial Ambient Light control register 0x02
     DLS Level = 64 levels, should be modified base on customer's specification
     Low Lux Threshold = 0x00, should be modified after determined light loss through window    
  */
      buffer[0] = AL3006_ALS_CTL;
    buffer[1] = 0xa0;//0xa0    
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        {
        printk(KERN_ERR"++++3");
        return err;
        }
        //return err;
    udelay(500);
    //pr_info("Write data: AL3006_ALS_CTL=0xa0\n");


     /* Initial Proximity control register 0x04h
     DPS Accuracy = 7 DPS counts
     DPS threshold level = 0x0A    
     Should be modified base on customer's specifications
  */
        buffer[0] = AL3006_PS_CTL;
    buffer[1] = 0x4f;//0x4a
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        {
        printk(KERN_ERR"++++4");
        return err;
        }
        //return err;
    udelay(500);
    //pr_info("Write data: AL3006_PS_CTL=0x0a\n");


/* Initial Proximity control register 0x08
     Window Loss compensation = 0x00h, 
     Should be modified after determined light loss through window    
  */
          buffer[0] = AL3006_ALS_WINDOW;
    buffer[1] = 0x00;    
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        return err;
    udelay(500);
    //pr_info("Write data: AL3006_ALS_WINDOW=0x00\n");


/* enable Ambient Light Sensor and/or Proximity Sensor 
     if Power up and enable Ambient Light Sensor only, set register_data = 0x00;
     if Power up and enable Proximity sensor only, set register_data = 0x01;
     if Power up and enable both Ambient Light and Proximity Sensor, set register_data = 0x02;
     Should be modified base on customer's specifications
  */ 
    buffer[0] = AL3006_CONF;
    buffer[1] = 0x03;//0x02;//0x00    
    err = al3006_txdata(buffer, 2);
    if (err < 0)
        return err;

    //pr_info("Write data: AL3006_CONF=0x0b\n");

    /* delay 100 milli seconds or 200ms to get the first ALS data */
    mdelay(100);

/* read ALS and PXY data register to reset AL3002/AL3005 interrupt */
    buffer[0] = AL3006_TATA;
    buffer[1] = 0xff;
    err = al3006_rxdata(buffer, 1);
    if (err < 0)
        return err;
    //pr_info("read data: AL3006_TATA=%d,%d\n",buffer[0],buffer[1]);
        for(ii=0;ii<8;ii++)
    {
    buffer[0] = ii;
    buffer[1] = 0xff;
    err = al3006_rxdata(buffer,1);
    if (err < 0) 
    printk(KERN_ERR" ++++err < 0\r\n");
    printk(KERN_ERR"buffer[%d]=0x%x,buffer[1]=0x%x\r\n",ii,buffer[0],buffer[1]);
    }

    return 0;
}

static int al3006_hardware_deinit(void)
{
// power set 
    int err;
    printk(KERN_ERR"%s power off 8901_l1 8058_S3",__func__);

    err = regulator_disable(pm8901_l1_3v3);
    if (err) {
        printk(KERN_ERR"%s: error disable pm8901_l1_3v3 failed \n", __func__);
        return err;
    }
    err = regulator_disable(pm8058_s3_1v8);
    if (err) {
        printk(KERN_ERR"%s: error disable pm8058_s3_1v8 failed \n", __func__);
        return err;
    }
    return 0;
}

/*
 * Set the delay time between light sensor events in ms
 */
int al3006_set_light_delay(short delay)
{
    return 0;
}

/*
 * Set the delay time between proximity sensor events in ms
 */
int al3006_set_proximity_delay(short delay)
{
    return 0;
}

/*
 * This function reads the light snesor value.
 */
int al3006_get_als_data(ALSDATA *al3006_als)
{
    int  bRet = -1;
    unsigned char buffer[2];
    pr_info(  " %s \r\n",__FUNCTION__);
    if(al3006_als == NULL)
    {
        pr_info(  "al3006_als is NULL in %s func\r\n",__FUNCTION__);
        return -EINVAL;
    }
    
    al3006_als->light = 0;

    /* read ALS and PXY data register to reset AL3002/AL3005 interrupt */
    buffer[0] = AL3006_TATA;
    buffer[1] = 0xff;
    bRet = al3006_rxdata(buffer, 1);
    if(bRet)
    {
        pr_info(  "func = %s:Read DLS_DPS_DATA register failed!\r\n",__FUNCTION__);
        return bRet;
    }
    //get the low six bits of uData[0] as light value
    al3006_als->light     = (buffer[0] & 0x3F);

    pr_info("read data: AL3006 light=%d\n",al3006_als->light);
    
    return bRet;
}

/*
 * This function reads the proximity snesor value.
 */
int al3006_get_ps_data(PSDATA *al3006_ps)
{
    int  bRet = -1;
    unsigned char buffer[2];
    pr_info(  " %s \r\n",__FUNCTION__);
    if(al3006_ps == NULL)
    {
        pr_info(  "al3006_ps is NULL in %s func\r\n",__FUNCTION__);
        return -EINVAL;
    }
    
    al3006_ps->proxi  = 0;

    /* read ALS and PXY data register to reset AL3002/AL3005 interrupt */
    buffer[0] = AL3006_TATA;
    buffer[1] = 0xff;
    bRet = al3006_rxdata(buffer, 1);
    if(bRet)
    {
        pr_info(  "func = %s:Read DLS_DPS_DATA register failed!\r\n",__FUNCTION__);
        return bRet;
    }
    //get the low six bits of uData[0] as light value
    al3006_ps->proxi     = (buffer[0] & 0x80);

    pr_info("read data: AL3006 proximity=%d\n",al3006_ps->proxi);
    
    return bRet;
}
/*
 * LUX calculation
 */



/*
 * Set the device int activity mode.
 * 0x00----enable ALS
 * 0x01----enable PS
 * 0x02----enable ALS & PS
 * 0x03----idle
 */
int OpenLightSensor(void)
{
    int bRet = -1;
    unsigned char buffer[2];
    unsigned char uchCONFIGRATION = 0;

    pr_info("++OpenLightSensor++\r\n");

    // Read current mode. 
    buffer[0] = AL3006_CONF;
    buffer[1] = 0x00;
    bRet = al3006_rxdata(buffer, 2);
    if (bRet < 0)
    {
        pr_info(  "func = %s::Read CONFIGRATION failed!\r\n",__FUNCTION__);
        return bRet;
    }

    uchCONFIGRATION = buffer[0];

    /*first set it in idle mode*/
    buffer[0] = AL3006_CONF;
    buffer[1] = 0x03;
    bRet = al3006_txdata(buffer, 2);
    if (bRet < 0)
    {
        pr_info(  "func = %s::Write CONFIGRATION(0x03) failed!\r\n",__FUNCTION__);
        return bRet;
    }

    if(uchCONFIGRATION == 0x03 || uchCONFIGRATION == 0x00)
    {    
        pr_info(  "%s::enable ALS SENSOR!\r\n",__FUNCTION__);
        /* 0x00 Enable ALS Sensor*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x00;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x00) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }
    else
    {
        pr_info(  "%s::enable DLS & DPS SENSOR!\r\n",__FUNCTION__);
        /* 0x02 Enable DLS & DPS Sensor*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x02;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x02) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }
    pr_info("--OpenLightSensor--\r\n");
    g_light_active_flag = 1;
    return bRet;    
}


/*
 * Set the ALS device or DPS + DLS in iddle mode
 */
int CloseLightSensor(void)
{
    int bRet = -1;
    unsigned char buffer[2];

    pr_info("+++CloseLightSensor+++\r\n");

    g_light_active_flag = 0;

    if(g_proxi_active_flag)/*if is true,proximity is used now*/
    {
        /* 0x01 set DPS in active mode only*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x01;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x01) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }
    else
    {
        /* 0x03 set iddle mode*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x03;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x03) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }

    pr_info("---CloseLightSensor---\r\n");
    return bRet;    

}

/**************************************************************************
* Function    : OpenproximitySensor
* Description : Set the device int activity mode.
   
* Calls       :
* Called By   : system ioctl IOCTL_AL3006_SET_PROMIXTY_ACTIVATE call.
*
* Input :  
*         void
*
*
* Return : 
*        int --- 0 successfull,others fail.
*
* others :     
**************************************************************************/
int OpenProximitySensor(void)
{
    int bRet = -1;
    unsigned char buffer[2];

    pr_info("+++OpenproximitySensor+++\r\n");

    if(g_light_active_flag == 1)
    {
        /* 0x02 Enable DLS + DPS*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x02;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x02) failed!\r\n",__FUNCTION__);
            return bRet;
        }
        //pr_info(  "OpenproximitySensor::Read CONFIGRATION(write:0x02|read:0x%x\r\n",get_value);
    }
    else
    {
        /* 0x01 Enable DPS only*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x01;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x01) failed!\r\n",__FUNCTION__);
            return bRet;
        }
        //pr_info(  "OpenproximitySensor::Read CONFIGRATION(write:0x02|read:0x%x\r\n",get_value);
    }

    /*
    *DPS_CONTROL register BITS 7:6 controls the accuracy of the DPS data,default 01. 
    *BITS 4:0 sets the DPS threshold level and is default to 01010 (decimal = 10).
    *01000101 xxcm ditection 7 accuracy
    */
    buffer[0] = AL3006_PS_CTL;
    buffer[1] = 0x4f;//0x59;//3cm
    bRet = al3006_txdata(buffer, 2);
    if (bRet < 0)
    {
        pr_info(  "func = %s::Write PS control(0x59) failed!\r\n",__FUNCTION__);
        return bRet;
    }
    //pr_info(  "OpenproximitySensor::Read DPS_CONTROL(write:0x5b|read:0x%x\r\n",get_value);
    
    pr_info("---OpenProximitySensor---\r\n");

    g_proxi_active_flag = 1;
    
    return bRet;    
}


/**************************************************************************
* Function    : CloseProximitySensor
* Description : Set the DPS device or DPS + DLS in iddle mode
   
* Calls       :
* Called By   : system ioctl IOCTL_AL3006_SET_PROMIXITY_ACTIVATE call.
*
* Input :  
*         void
*
*
* Return : 
*        int --- 0 successfull,others fail.
*
* others :     
**************************************************************************/
int CloseProximitySensor(void)
{
    int bRet = -1;
    unsigned char buffer[0];

    pr_info("+++CloseProximitySensor+++\r\n");

    g_proxi_active_flag = 0;

    if(g_light_active_flag)/*if true,DLS is used now*/
    {
        /* 0x00 set DLS in active mode only*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x00;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x00) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }
    else
    {
        /* 0x03 set DLS & DPS in iddle mode*/
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x03;
        bRet = al3006_txdata(buffer, 2);
        if (bRet < 0)
        {
            pr_info(  "func = %s::Write CONFIGRATION(0x03) failed!\r\n",__FUNCTION__);
            return bRet;
        }
    }

    pr_info("---CloseProximitySensor---\r\n");

    return bRet;    

}


/**************************************************************************
* Function    : al3006_open
* Description : open operation func of the acc device.
*
* Calls       :
* Called By   : system ioctl call.
*
* Input :  
*         inode --- Handle to the file node.
*        file --- Access filp handle for the device. 
*
* Output : 
*        none
*
* Return : 
*        returns a handle that identifies the open context of the device
*        to the calling application. returns zero if the device cannot 
*        be opened.
*
* others : 
**************************************************************************/
static int al3006_open(struct inode *inode, struct file *file)
{
    int minor = MINOR(inode->i_rdev);

    pr_info("func = %s\n",__FUNCTION__);
    pr_info("AL3006 device [%d] opened\r\n",minor);

    if(inode==NULL || file==NULL)
    {
        return -EINVAL;
    }
        
       file->private_data = pal3006_dev;  

    pr_info("AL3006! Open++!\r\n");

    return 0;
}


/**************************************************************************
* Function    : al3006_close
* Description : write operation func of the acc device.
*
* Calls       :
* Called By   : 
*
* Input :  
*         inode --- Handle to the file node.
*        file --- Access filp handle for the device. 
*
* Output : 
*        none
*
* Return : 
*        return zeor if successful, -EINVAL for error.
*        
*
* others :       
**************************************************************************/
static int al3006_close(struct inode *inode, struct file *file)
{
    int minor = MINOR(inode->i_rdev);
    
    pr_info("func = %s\r\n",__FUNCTION__);
        
    if(inode==NULL || file==NULL)
    {
        return -EINVAL;
    }    

    pr_info("AL3006 Device[%d] Release!\r\n",minor);
    return 0;
}


/**************************************************************************
* Function    : al3006_light_ioctl
* Description : ioctl operation func of the al3006 light sesnor device.
*
* Calls       :
* Called By   : system ioctl call.
*
* Input :  
*         inode --- Handle to the file node.
*        file --- Access filp handle for the device. 
*        cmd --- the ioctl command to call.
*        arg --- the pointer to the input or output parameter structure.
*
* Output : 
*        arg --- the pointer to the output structure.
*
* Return : 
*        return the number of byte if successful, -ve for error.
*
* others :     
**************************************************************************/                                        
static long al3006_light_ioctl(struct file *file, unsigned int cmd, unsigned long arg)                    
{
    int     bRet = 0;
    void    __user *argp = (void __user *)arg;
    short     light_delay = 0;
    short    flag;
    short   lightflag;

    pr_info("func = %s\n",__FUNCTION__);
    if(file==NULL)
    {
        pr_info("%s(the poiter is NULL),line = %d\r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }

    switch (cmd)
    {    
        case IOCTL_AL3006_GET_LIGHT_DATA:

            bRet = al3006_get_als_data(g_pdls);
            if(bRet)    
            {
                pr_info("%s,line = %d\r\n",__FUNCTION__,__LINE__);
                return bRet;
            }
            if(copy_to_user((ALSDATA *)arg, g_pdls, sizeof(ALSDATA)))
            {
                pr_info("copy_to_user fail(%s)\n",__FUNCTION__);
                return -EINVAL;
            }

            break;
        
        case IOCTL_AL3006_SET_LIGHT_ACTIVATE:

            pr_info("(IOCTL_AL3006_SET_LIGHT_ACTIVATE): func = %s\r\n",__FUNCTION__);

            if (copy_from_user(&lightflag, argp, sizeof(lightflag)))
            {
                pr_info("copy_from_user fail(%s),line(%d)\n",__FUNCTION__,__LINE__);
                return -EFAULT;
            }
            if (lightflag < 0 || lightflag > 1)
            {
                return -EINVAL;
            }
            if (lightflag == 1 && g_is_al3006_suspend == 0)
            {
                pr_info("(IOCTL_AL3006_SET_LIGHT_ACTIVATE):__flag = 1\n");
                if(al3006_err_count <= MAX_ERROR_COUNT)
                {
                    if((g_light_active_flag == 0)&&(g_proxi_active_flag ==0))
                        {
                        pr_info("(IOCTL_AL3006_LIGHT_ACTIVATE) al3006_hardware_init() \n");
                        al3006_hardware_init();//open power and initial
                        }
                    bRet = OpenLightSensor();/*set light sensor in activity mode*/
                    if(bRet)
                    {
                        pr_info("%s:set DLS and DPS active mode failed!\r\n",__FUNCTION__);
                        al3006_err_count++;
                        return -EINVAL;
                    }
                    else
                    {
                        al3006_err_count = 0;
                    }
                }
                else
                {
                    pr_info("IOCTL_AL3006_SET_LIGHT_ACTIVATE ----ON:failed!\r\n");
                            return -EINVAL;
                }
            }
            if (lightflag == 0 && g_is_al3006_suspend == 0)
            {            
                pr_info("(IOCTL_AL3006_SET_LIGHT_ACTIVATE):__flag = 0\n");
                bRet = CloseLightSensor();/*set light sensor in iddle mode*/
                if(bRet)
                {
                    pr_info(  "IOCTL_AL3006_SET_LIGHT_ACTIVATE----OFF:set DLS and DPS iddle mode failed!\r\n");
                    break;
                }
                if((g_light_active_flag == 0)&&(g_proxi_active_flag ==0))
                    al3006_hardware_deinit();//power off
            }
            atomic_set(&light_flag, lightflag);
            break;

        case IOCTL_AL3006_SET_LIGHT_DELAY:

            if (copy_from_user(&light_delay, argp, sizeof(light_delay)))
            {
                pr_info("(IOCTL_AL3006_SET_LIGHT_DELAY):fail\r\n");
                return -EFAULT;
            }
            bRet = al3006_set_light_delay(light_delay);
            break;

        case IOCTL_AL3006_ENABLE_LIGHT_SENSOR:
    
            if (copy_from_user(&flag, argp, sizeof(flag)))
            {
                pr_info("(IOCTL_AL3006_ENABLE_LIGHT_SENSOR)copy from user fail\r\n");
                return -EFAULT;
            }    
            pr_info("++IOCTL_AL3006_ENABLE_LIGHT_SENSOR++ flag(%d)\r\n",flag);
            if (flag < 0 || flag > 1)
            {
                return -EINVAL;
            }
            atomic_set(&light_lock_flag, flag);
            break;

        default:
            pr_info( "%s: Unrecognized IOCTL %d\r\n", __FUNCTION__, cmd);
            break;
    }
    return bRet;
} 


/**************************************************************************
* Function    : al3006_proxi_ioctl
* Description : ioctl operation func of the al3006 proximity device.
*
* Calls       :
* Called By   : system ioctl call.
*
* Input :  
*         inode --- Handle to the file node.
*        file --- Access filp handle for the device. 
*        cmd --- the ioctl command to call.
*        arg --- the pointer to the input or output parameter structure.
*
* Output : 
*        arg --- the pointer to the output structure.
*
* Return : 
*        return the number of byte if successful, -ve for error.
*
* others :     
**************************************************************************/    

#ifdef TEST_MODE

int al3006_enable_testmode(void)
{
    int bRet = -1;
    unsigned char buffer[2];

    pr_info("+++EntryCrossTalkMode+++\r\n");

    // set AL3002 in power down mode and idle mode 
    buffer[0] = AL3006_CONF;
    buffer[1] = 0x0C;
    bRet = al3006_txdata(buffer, 2);
    if (bRet < 0) 
    {
        pr_info("%s:Failed to write 0x%x= 0x%x\n", __FUNCTION__, AL3006_CONF,0x0C);
        return bRet;
    }    

    buffer[0] = AL3006_CONF;
    buffer[1] = 0x01;
    bRet = al3006_txdata(buffer, 2);
    if (bRet < 0) 
    {
        pr_info("%s:Failed to read 0x%x=0x%x\n", __FUNCTION__, AL3006_CONF,0x01);
        return bRet;
    }    

    buffer[0] = AL3006_ALS_WINDOW;
    buffer[1] = 0xe0;
    bRet = al3006_txdata(buffer, 2);
    if (bRet < 0) 
    {
        pr_info("%s:Failed to read 0x%x=0x%x\n", __FUNCTION__, AL3006_ALS_WINDOW,0xe0);
        return bRet;
    }
    
    pr_info("---EntryCrossTalkMode---\r\n");

    g_proxi_active_flag = 1;
    
    return bRet;    
}


static void al3006_timeout(unsigned long data)
{
    pr_info("al3006:time to poll register\n");

    schedule_work(&al3006_dev->work_testmode);

    mod_timer(&al3006_timer, jiffies + 1*HZ);

}

static void al3006_work_testmode(struct work_struct *work)
{
    int bRet = -1;
    unsigned char buffer[2];

    pr_info("func = %s\n",__FUNCTION__);
    if(NULL == work)
    {
        pr_info("%s:the poiter is NULL\n",__FUNCTION__);
        return;
    }

    buffer[0] = AL3006_TATA;
    buffer[1] = 0x00;
    bRet = al3006_rxdata(buffer, 1);
    if (bRet < 0) 
    {
        pr_info("%s:Failed to read 0x%x\n", __FUNCTION__, AL3006_INT_STATUS);
        return;
    }
    pr_info( "AL3006_TATA: %d\r\n",buffer[0]);

    input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE, buffer[0]);
    input_sync(al3006_dev->input_proxi_dev);

}
#endif
                                    
static long al3006_proxi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)                    
{
    int     bRet = 0;
    void    __user *argp = (void __user *)arg;
    short     proxi_delay = 0;
    short   proxiflag;
    short     flag;

    pr_info("func = %s\n",__FUNCTION__);
    if(file==NULL)
    {
        pr_info("%s(the poiter is NULL),line = %d\r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }

    switch (cmd)
    {    
        

        case IOCTL_AL3006_GET_PROMIXITY_DATA:

            bRet = al3006_get_ps_data(g_pdps);
            if(bRet)    
            {
                pr_info("%s,line = %d:can't get the dps data\r\n",__FUNCTION__,__LINE__);
                return bRet;
            }
            if(copy_to_user((PSDATA *)arg, g_pdps, sizeof(PSDATA)))
            {
                pr_info("copy_to_user fail(%s)(%d)\n",__FUNCTION__,__LINE__);
                return -EFAULT;
            }
            
            break;
    
        case IOCTL_AL3006_SET_PROMIXITY_ACTIVATE:

            pr_info("(IOCTL_AL3006_SET_PROMIXITY_ACTIVATE): func = %s\r\n",__FUNCTION__);

            if (copy_from_user(&proxiflag, argp, sizeof(proxiflag)))
            {
                pr_info("copy_from_user fail(%s)(%d)\n",__FUNCTION__,__LINE__);
                return -EFAULT;
            }
            
            if (proxiflag < 0 || proxiflag > 1)
            {
                return -EINVAL;
            }
            

            if (proxiflag == 1)
            {
                pr_info("(IOCTL_AL3006_SET_PROMIXITY_ACTIVATE):___flag = 1\n");
                if(al3006_err_count <= MAX_ERROR_COUNT)
                {
                    if((g_light_active_flag == 0)&&(g_proxi_active_flag ==0))
                        {
                        pr_info("(IOCTL_AL3006_SET_PROMIXITY_ACTIVATE) al3006_hardware_init() \n");
                        al3006_hardware_init();//open power and initial
                        }
                    bRet = OpenProximitySensor();/*set proximity sensor in activity mode*/
                    if(bRet)
                    {
                        pr_info("%s:set DLS and DPS active mode failed!\r\n",__FUNCTION__);
                        al3006_err_count++;
                        return -EINVAL;
                    }
                    else
                    {
                        al3006_err_count = 0;
                    }
                }
                else
                {
                    pr_info("IOCTL_AL3006_SET_PROMIXITY_ACTIVATE ----ON:failed!\r\n");
                    return -EINVAL;
                }
            }
            else
            {            
                pr_info("(IOCTL_AL3006_SET_PROMIXITY_ACTIVATE):___flag=0\n");
                bRet = CloseProximitySensor();/*set proximity sensor in iddle mode*/
                
                if(bRet)
                {
                    pr_info(  "IOCTL_AL3006_SET_PROMIXITY_ACTIVATE----OFF:set DLS and DPS iddle mode failed!\r\n");
                    return -EINVAL;
                }
                    if((g_light_active_flag == 0)&&(g_proxi_active_flag ==0))
                        al3006_hardware_deinit();//power off
            }
            atomic_set(&proxi_flag, proxiflag);
            break;

        case IOCTL_AL3006_SET_PROMIXITY_DELAY:

            if (copy_from_user(&proxi_delay, argp, sizeof(proxi_delay)))
            {
                pr_info("(IOCTL_AL3006_SET_PROMIXITY_DELAY):copy_from_user fail\r\n");
                return -EFAULT;
            }
            bRet = al3006_set_proximity_delay(proxi_delay);
            break;

        case IOCTL_AL3006_ENABLE_PROXIMITY_SENSOR:
                
                if (copy_from_user(&flag, argp, sizeof(flag)))
                {
                    pr_info("(IOCTL_AL3006_ENABLE_PROXIMITY_SENSOR):copy_from_user fail\r\n");
                    return -EFAULT;
                }    
                pr_info("++IOCTL_AL3006_ENABLE_PROXIMTY_SENSOR++ flag(%d)\r\n",flag);
                if (flag < 0 || flag > 1)
                {
                    return -EINVAL;
                }
                atomic_set(&proxi_lock_flag, flag);
            break;

        case IOCTL_AL3006_ENABLE_TEST_MODE:
            {
            #ifdef TEST_MODE
                /*进入测试模式前，关闭正常模式*/
                bRet = CloseProximitySensor();/*set proximity sensor in iddle mode*/        
                if(bRet)
                {
                    pr_info("IOCTL_AL3006_SET_PROMIXITY_ACTIVATE----OFF:set DLS&DPS idle mode failed!\r\n");
                    return -EINVAL;
                }
                atomic_set(&proxi_flag, 0);

                INIT_WORK(&al3006_dev->work_testmode, al3006_work_testmode);

                init_timer(&al3006_timer);
                al3006_timer.function = al3006_timeout;
                   al3006_timer.data = (unsigned long)pal3006_dev;
                al3006_timer.expires = jiffies + 1*HZ;
                add_timer(&al3006_timer);
    
                if(al3006_enable_testmode())
                {
                    pr_info(  "al3006_init:Failed to enter test mode!\r\n");
                     return -EINVAL;
                }
                                pr_info("%s:enter test mode!\r\n",__FUNCTION__);
                                input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE,1000);
                input_sync(al3006_dev->input_proxi_dev);
                atomic_set(&test_mode_flag, 1);
                
            #endif            
            }
            break;

        case IOCTL_AL3006_DISABLE_TEST_MODE:
            {
            #ifdef TEST_MODE
                al3006_hardware_init();
                del_timer(&al3006_timer);
                cancel_work_sync(&al3006_dev->work_testmode);
                                input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE,500);
                input_sync(al3006_dev->input_proxi_dev);
                atomic_set(&test_mode_flag, 0);

                /*退出测试模式后，开启正常模式*/
                if(al3006_err_count <= MAX_ERROR_COUNT)
                {
                    bRet = OpenProximitySensor();/*set proximity sensor in activity mode*/
                    if(bRet)
                    {
                        pr_info("%s:set DLS and DPS active mode failed!\r\n",__FUNCTION__);
                        al3006_err_count++;
                        return -EINVAL;
                    }
                    else
                    {
                        al3006_err_count = 0;
                    }
                }
                else
                {
                    pr_info("IOCTL_AL3006_SET_PROMIXITY_ACTIVATE ----ON:failed!\r\n");
                            return -EINVAL;
                }
                atomic_set(&proxi_flag, 1);
                                pr_info("%s:exit test mode!\r\n",__FUNCTION__);
            #endif
            }
            break;

        default:
            pr_info( "%s: Unrecognized IOCTL %d\r\n", __FUNCTION__, cmd);
            break;
    }
    return bRet;
} 

/**************************************************************************
* Function    : al3006_work_func
* Description : read DLS or DPS datas from the device and report to user.
*
* Calls       :
* Called By   : 
*
* Input :  
*         struct work_struct       --- *work.
*        
*         
* Output : 
*
* Return : 
*         Returns 0 .
*
* others :     
**************************************************************************/
static void al3006_work_func(struct work_struct *work)
{
    int bRet = -1;
    unsigned char buffer[2];
    unsigned char status = 0x00;
    // int ii = 0;
    
    printk(KERN_ERR"func = %s\n",__FUNCTION__);
    pr_info("func = %s\n",__FUNCTION__);
    if(NULL == work)
    {
        pr_info("%s:the poiter is NULL\n",__FUNCTION__);
        return;
    }
    buffer[0] = AL3006_INT_STATUS;
    buffer[1] = 0x00;
    bRet = al3006_rxdata(buffer, 1);
    if (bRet < 0) 
    {
        pr_info("%s:Failed to read 0x%x\n", __FUNCTION__, AL3006_INT_STATUS);
        return;
    }

    status = buffer[0];
    
    /*0x01 is the interrupt status data,when ALS INT*/    
    if(status == 0x1)
    {
        mutex_lock(&pal3006_dev->al3006_mutex);
        bRet = al3006_get_als_data(g_pdls);
        if(bRet)
        {
               pr_info( "Can't get light data\r\n");
        }
        mutex_unlock(&pal3006_dev->al3006_mutex);
        if(atomic_read(&light_flag))
        {
            if(al3006_dev->input_light_dev != NULL && atomic_read(&light_flag))
            {    
                    input_report_abs(al3006_dev->input_light_dev, ABS_MISC, g_pdls->light);
                    printk(KERN_ERR"++g_pdls->light =%d",g_pdls->light);
                    input_sync(al3006_dev->input_light_dev);
            }
        }
        
    }
    /*0x02 is the interrupt status data,when PS INT*/    
    else if(status == 0x2)
    {
        mutex_lock(&pal3006_dev->al3006_mutex);
        bRet = al3006_get_ps_data(g_pdps);
        if(bRet)
        {
               pr_info( "Can't get proximity data\r\n");
        }
        mutex_unlock(&pal3006_dev->al3006_mutex);
        if(atomic_read(&proxi_flag))
        {
            if(al3006_dev->input_proxi_dev != NULL && atomic_read(&proxi_flag))
            {
                    if(0x00 == (g_pdps->proxi & 0x80)) g_pdps->proxi =5.0f;
                    if(0x80 == (g_pdps->proxi & 0x80)) g_pdps->proxi =3.0f;
                    input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE, g_pdps->proxi);
                    printk(KERN_ERR"++g_pdls->proxi =%d",g_pdps->proxi);
                    input_sync(al3006_dev->input_proxi_dev);
            }    
        }    
    }
    /*0x03 is the interrupt status data,when ALS & PS INT at the same time*/    
    else if(status == 0x3)
    {
        mutex_lock(&pal3006_dev->al3006_mutex);
        bRet  = al3006_get_als_data(g_pdls);
        bRet |= al3006_get_ps_data(g_pdps);
        if(bRet)
        {
               pr_info( "Can't get light or proximity data\r\n");
        }
        mutex_unlock(&pal3006_dev->al3006_mutex);
        
        if(atomic_read(&light_flag) && atomic_read(&proxi_flag))
        {
            if((al3006_dev->input_light_dev != NULL) && (al3006_dev->input_proxi_dev != NULL) &&
                atomic_read(&light_flag) && atomic_read(&proxi_flag))
            {
                    if(0x00 == (g_pdps->proxi & 0x80)) g_pdps->proxi =5.0f;
                    if(0x80 == (g_pdps->proxi & 0x80)) g_pdps->proxi =3.0f;
                    input_report_abs(al3006_dev->input_light_dev, ABS_MISC, g_pdls->light);
                    input_sync(al3006_dev->input_light_dev);        
                    input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE, g_pdps->proxi);
                    input_sync(al3006_dev->input_proxi_dev);
            }
        }
    }

#if 0  //test for driver
        for(ii=0;ii<0;ii++)
        {
        buffer[0] = ii;
        buffer[1] = 0xff;
        bRet = al3006_rxdata(buffer,1);
        if (bRet < 0) 
        printk(KERN_ERR" ++++err < 0\r\n");
        printk(KERN_ERR"buffer[%d]=0x%x,status=0x%x\r\n",ii,buffer[0],status);
        bRet=-1;
        }

        //mutex_lock(&pal3006_dev->al3006_mutex);
        bRet  = al3006_get_als_data(g_pdls);
        bRet |= al3006_get_ps_data(g_pdps);
        if(bRet)
        {
               pr_info( "Can't get light or proximity data\r\n");
        }
        //mutex_unlock(&pal3006_dev->al3006_mutex);
        
        //if(atomic_read(&light_flag) && atomic_read(&proxi_flag))
        {
            //if((al3006_dev->input_light_dev != NULL) && (al3006_dev->input_proxi_dev != NULL) &&
            //    atomic_read(&light_flag) && atomic_read(&proxi_flag))
            {
                input_report_abs(al3006_dev->input_light_dev, ABS_MISC, g_pdls->light);
                input_report_abs(al3006_dev->input_proxi_dev, ABS_DISTANCE, g_pdps->proxi);
                input_sync(al3006_dev->input_light_dev);
            }
        }

#endif

    if(g_is_al3006_suspend == 0)
    {
        pr_info("AL3006_suspend== 0, func =  %s\r\n",__FUNCTION__);
        enable_irq(al3006_irq);
    }
}

static irqreturn_t al3006_irqhandle(int al3006_irq, void *data)
{
    printk(KERN_ERR" enter al3006_irqhandle!!!!!!!!!!!!!!!!!! \r\n");
        //disable_irq(al3006_irq);
    disable_irq_nosync(al3006_irq);
    schedule_work(&al3006_dev->work_data);
    return IRQ_HANDLED;
}


/**************************************************************************
* Function    : al3006_resume
* Description : Power manager function is called when system wake up.
* Calls       :
* Called By   :
*          When system wake up,system function will call this function.
* Input :  struct i2c_client *client
* Output :     
* Return : 0 successful,other faile  
* others : 
**************************************************************************/
static int al3006_resume(struct i2c_client *client)
{
    pr_info("func = %s\n",__FUNCTION__);

    //if(smartphone_calling_enable == 0)
    if (g_is_al3006_suspend == 1)
    {
        g_is_al3006_suspend = 0;
        pr_info("NOW is not in calling mode in %s\n",__FUNCTION__);
        //gpio_direction_output(AL3006_3V_ENABLE,1);
        mdelay(200);
        /*init al3006 hardware*/ 
            if(al3006_hardware_init())
        {
            pr_info(  "al3006_init:Failed to Init al3006!\r\n");
            return -EFAULT;
        }
        //omap_cfg_reg(N3_34XX_GPIO40);
        //gpio_direction_input(AL3006_IRQ_GPIO);
        schedule_work(&al3006_dev->work_data);

        if (atomic_read(&proxi_flag))        
        {            
            if(OpenProximitySensor())
            {
                pr_info(  "al3006_resume:Failed to open proximity sensor!\r\n");
                    return -EFAULT;
            }
        }
        if (atomic_read(&light_flag))
        {
            if(OpenLightSensor())
            {
                pr_info(  "al3006_resume:Failed to open light sensor!\r\n");
                    return -EFAULT;
            }
        }
        if (atomic_read(&test_mode_flag))
        {
            if(al3006_enable_testmode())
            {
                pr_info(  "al3006_init:Failed to enter test mode!\r\n");
                 return -EINVAL;
            }
        }    
        
    }
    
    return 0;
}

/**************************************************************************
* Function    : al3006_suspend
* Description : Power manager function is called when system suspend.
* Calls       :
* Called By   :
*          When system suspend,system function will call this function.
* Input :  struct i2c_client *client,pm_message_t state
* Output :     
* Return : 0 
* others : 
**************************************************************************/
static int al3006_suspend(struct i2c_client *client, pm_message_t state)
{
    pr_info("func = %s\n",__FUNCTION__);
    //if(smartphone_calling_enable == 0)
    if (!atomic_read(&proxi_flag))
    {
        int err;
        char buffer[2];
        pr_info("NOW is not in calling mode in %s\n",__FUNCTION__);

        // set AL3002 in power down mode and idle mode 
        buffer[0] = AL3006_CONF;
        buffer[1] = 0x0b;
        err = al3006_txdata(buffer, 2);
        if (err < 0)
            return err;
    
        g_is_al3006_suspend = 1;

        disable_irq(al3006_irq);
            cancel_work_sync(&al3006_dev->work_data);

        printk(KERN_ERR"%s power off 8901_l1 8058_S3",__func__);

        err = regulator_disable(pm8901_l1_3v3);
        if (err) {
            printk(KERN_ERR"%s: error disable pm8901_l1_3v3 failed \n", __func__);
            return err;
        }
        err = regulator_disable(pm8058_s3_1v8);
        if (err) {
            printk(KERN_ERR"%s: error disable pm8058_s3_1v8 failed \n", __func__);
            return err;
        }
    }
    return 0;
}

static void al3006_late_resume(struct early_suspend *h)
{
    struct al3006_device *ts;
    ts = container_of(h, struct al3006_device, early_suspend);
    al3006_resume(ts->i2c_client);
    return;
}

static void al3006_early_suspend(struct early_suspend *h)
{
    struct al3006_device *ts;
    ts = container_of(h, struct al3006_device, early_suspend);
    al3006_suspend(ts->i2c_client, PMSG_SUSPEND);
}

static const struct file_operations al3006_fops[] = {
    {
        .owner   = THIS_MODULE,
        .open    = al3006_open,
        .release = al3006_close,
        .unlocked_ioctl = al3006_light_ioctl,
    },
    {
        .owner   = THIS_MODULE,
        .open    = al3006_open,
        .release = al3006_close,
        .unlocked_ioctl = al3006_proxi_ioctl,
    },
};

static struct miscdevice al3006_device[] = {
    {
        .minor = MISC_DYNAMIC_MINOR,
        .name  = "light",
        .fops  = &al3006_fops[light],
    },
    {
        .minor = MISC_DYNAMIC_MINOR,
        .name  = "proximity",
        .fops  = &al3006_fops[proxi],
    },
};

/**************************************************************************
* Function    : al3006_probe
* Description : Probes the driver for valid I2C attachment.
*
* Calls       :
* Called By   : 
*
* Input :  
*         client: Pointer to structure of I2C client.
*        id:    Pointer to structure of i2c_device_id.
*         
* Output : 
*        none.
*
* Return : 
*         Returns 0 if successful, or -EBUSY if unable to get client attached data,
*         or -ENODEV platform_data is NULL.
*
* others : 
**************************************************************************/
static int al3006_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int bRet = 0;

    pr_info("enter %s func, location 1 +++ request_irq\n",__FUNCTION__);
    
    if (i2c_get_clientdata(client)) 
    {
        pr_info("DTA BUSY %s\n", client->name);
        return -EBUSY;
    }

    /*al3006_dev->pdata = client->dev.platform_data;

    if (!al3006_dev->pdata)
    {
        printk( "no platform data?\n");
        return -ENODEV;
    }*/

    al3006_dev->i2c_client = client;
    i2c_set_clientdata(client, al3006_dev);
    
    /*init mutex*/
    mutex_init(&pal3006_dev->al3006_mutex);

    /*init work queue function*/
    INIT_WORK(&al3006_dev->work_data, al3006_work_func);

    //gpio_request(AL3006_3V_ENABLE,"al3006");
    //gpio_direction_output(AL3006_3V_ENABLE,1);

    /*init gpio interrupt*/
    //omap_cfg_reg(N3_34XX_GPIO40);//gpio_40 for al3006 irq
    //gpio_direction_input(AL3006_IRQ_GPIO);//set gpio as input mode

    /*gpio translate into irq*/
    printk(KERN_ERR"al3006_dev->i2c_client->irq = %d\r\n",al3006_dev->i2c_client->irq);
    if (al3006_dev->i2c_client->irq > 0)
       {
        al3006_irq = al3006_dev->i2c_client->irq;
        //al3006_irq = gpio_to_irq(AL3006_IRQ_GPIO);
        printk(KERN_ERR"al3006_irq = %d\r\n",al3006_irq);
            }

    //set_irq_nested_thread(al3006_irq,0);IRQF_TRIGGER_LOW FALLING
    bRet = request_threaded_irq(al3006_irq,NULL,
    //bRet = request_irq(al3006_irq,
             al3006_irqhandle,
             IRQF_TRIGGER_FALLING,
             "al3006",
             NULL);//NULL
/*
    bRet = request_any_context_irq(al3006_irq,
             al3006_irqhandle,
             IRQF_TRIGGER_LOW,
             "al3006",
             NULL);    
*/
    if(bRet)
    {
        pr_info("al3006 probe: fail to install irq bRet=%d \r\n",bRet);
        goto err_free_irq;
    }

    disable_irq_nosync(al3006_irq);
    pr_info(  "++AL3006_Init!!");
    
    /*add input device for light sensor */
    al3006_dev->input_light_dev = input_allocate_device();
    /*add input device for proximity sensor */
    al3006_dev->input_proxi_dev = input_allocate_device();

    if (!al3006_dev->input_light_dev || !al3006_dev->input_proxi_dev) 
    {
        bRet = -ENOMEM;
        printk(KERN_ERR "al3006_probe: Failed to allocate input device\n");
        goto exit_input_dev_alloc_failed;
    }

    /*case data mode for input device and give the range of report datas*/
    set_bit(EV_ABS, al3006_dev->input_light_dev->evbit);
    set_bit(EV_ABS, al3006_dev->input_proxi_dev->evbit);

    /* light Lux data */
    input_set_abs_params(al3006_dev->input_light_dev, ABS_MISC, 0, 100000, 0, 0);
    /* proximity data */
    input_set_abs_params(al3006_dev->input_proxi_dev, ABS_DISTANCE, 0, 1000, 0, 0);
    

    /*input device name*/
    al3006_dev->input_light_dev->name = "light";
    al3006_dev->input_proxi_dev->name = "proximity";

    bRet  = input_register_device(al3006_dev->input_light_dev);
    if (bRet) 
    {
        printk(KERN_ERR
               "al3006_probe: Unable to register input device: %s\n",
               al3006_dev->input_light_dev->name);
        goto exit_input_light_register_device_failed;
    }

    bRet  = input_register_device(al3006_dev->input_proxi_dev);
    if (bRet) 
    {
        printk(KERN_ERR
               "al3006_probe: Unable to register input device: %s\n",
               al3006_dev->input_proxi_dev->name);
        goto exit_input_proximity_register_device_failed;
    }

    /*
    ** for acc suspend 
    */
    al3006_dev->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN; 
    al3006_dev->early_suspend.suspend = al3006_early_suspend;
    al3006_dev->early_suspend.resume = al3006_late_resume;
    register_early_suspend(&al3006_dev->early_suspend);

    /* 
    ** As default, report light & proximity information 
    */
    atomic_set(&light_flag, 0);
    atomic_set(&proxi_flag, 0);

    /*Hardware init for al3006 device*/
    if(al3006_hardware_init())
    {
        pr_info(  "al3006_init:Failed to Init al3006!\r\n");
        return 0;
    }
    
    //if(OpenLightSensor())
    //{
    //    pr_info(  "al3006_init:Failed to open light sensor!\r\n");
    //    return 0;
    //}
    enable_irq(al3006_irq);
    //disable_irq(al3006_irq);
    g_is_al3006_suspend = 0;
    pr_info(  "--al3006_Init...\r\n");
    return 0;

exit_input_light_register_device_failed:
    input_free_device(al3006_dev->input_light_dev);

exit_input_proximity_register_device_failed:
    input_free_device(al3006_dev->input_proxi_dev);

exit_input_dev_alloc_failed:
    kfree(al3006_dev);

err_free_irq:/*free interrupt number*/
    free_irq(al3006_irq,NULL);

    return bRet;
}


/**************************************************************************
* Function    : al3006_remove
* Description : AL3006 driver i2c remove handler, Complement of al3006_probe().
*               Unregister acc as an i2c client device. 
* 
* Calls       :
* Called By   : 
*
* Input :  
*         client: i2c driver client device structure
*         
* Output : 
*           none.
*
* Return : 
*        Returns zero if successful, or non-zero otherwise.
*
* others : 
**************************************************************************/
static int __exit al3006_remove(struct i2c_client *client)
{
    pr_info(" %s \n",__FUNCTION__);
    if (!client->adapter)
    {
        return -ENODEV;    /* our client isn't attached */
    }
    i2c_set_clientdata(client, NULL);
    free_irq(al3006_irq,NULL);
    return 0;
}

static const struct i2c_device_id al3006_id[] = {
    { AL3006_DRIVER_NAME, 0 },
    { },
};
MODULE_DEVICE_TABLE(i2c, al3006_id);

static struct i2c_driver al3006_i2c_driver = {
    .driver = {
        .name = AL3006_DRIVER_NAME,
        .owner = THIS_MODULE,
    },
    .probe    = al3006_probe,
    .remove   = __exit_p(al3006_remove),
    .id_table = al3006_id,
    //.suspend  = al3006_suspend,
    //.resume   = al3006_resume,
};

#if 0
/**************************************************************************
* Function    : create_csensor_class
* Description : create BMA-dev device class.
* Calls       :
* Called By   : al3006_init
*          
* Input :      void
* Output :     
* Return :     0 successful,others fail
* others : 
**************************************************************************/
static int create_csensor_class(void)
{
    pr_info(" %s \n",__FUNCTION__);
    if ( !al3006_dev_class )
    {
         al3006_dev_class = class_create(THIS_MODULE,"al3006_class");
         if (IS_ERR(al3006_dev_class))
        {
            printk(KERN_ERR "func = %s: failed to create csensor class\r\n",__FUNCTION__);
            return PTR_ERR(al3006_dev_class);
        }    
    }
    return 0;
}

/**************************************************************************
* Function    : create_csensor_class
* Description : create device node for al3006.
* Calls       :
* Called By   : al3006_setup_cdev
*          
* Input :      dev_t dev_no
*        int   idx
* Output :     
* Return :     void
* others : 
**************************************************************************/
static void csensor_create_dev_nod(dev_t dev_no, int idx)
{
    char dev_name[10];  
    char * a="light";
    char * b="proximity";

    if(idx == 0)
    {
        sprintf(dev_name,"al3006_%s",a);    
    }
    else
    {
        sprintf(dev_name,"al3006_%s",b);    
    }
    device_create(al3006_dev_class, NULL, dev_no , NULL, dev_name);
    printk(KERN_INFO "al3006 device node created .........[%d]\r\n",idx);
}

/**************************************************************************
* Function    : al3006_setup_cdev
* Description : add char device in the char system.
* Calls       :
* Called By   : al3006_init
*          
* Input :      struct AL3006_DEV *dev
*        int   index
* Output :     
* Return :     void
* others : 
**************************************************************************/
static void al3006_setup_cdev(struct AL3006_DEV *dev, int index)
{
    int bRet, dev_number;
    printk(KERN_INFO"al3006_setup_cdev()++\r\n");

    dev_number = MKDEV( csensor_major, csensor_minor + index );
    
    if((&dev->al3006_cdev) == NULL)
    {
        printk(KERN_INFO"al3006_cdev is null!\r\n");
    }

    cdev_init(&dev->al3006_cdev, &al3006_fops[index]);

    dev->al3006_cdev.owner = THIS_MODULE;
    dev->al3006_cdev.ops   = &al3006_fops[index];

    bRet = cdev_add(&dev->al3006_cdev, dev_number, 1);
    
    printk(KERN_NOTICE "mxw adding csesnor al3006 dev \r\n");
    if(bRet)
    {
        pr_info(KERN_INFO"al3006_setup_cdev: Error %d adding al3006 dev %d\r\n",bRet,index);    
    }
        printk(KERN_INFO"al3006_setup_cdev()--\r\n");
    csensor_create_dev_nod(dev_number,index);
}
#endif

/*
 * Registers driver as an i2c client driver.  Returns 0 on success,
 * error code otherwise.
 */
static int __init al3006_init(void)
{

    int bRet = 0;
    int dev_idx;
    unsigned int result = 0;
    dev_t dev_number = 0;
    pr_info(KERN_ERR"al3006_init: ++AL3006! Init!\r\n");

    pm8901_l1_3v3 = regulator_get(NULL, "8901_l1");
    if (IS_ERR(pm8901_l1_3v3)) {
        pr_err("%s: Unable to get pm8901_l1_3v3\n", __func__);
        return -ENODEV;
    }

    pm8058_s3_1v8 = regulator_get(NULL, "8058_s3");
    if (IS_ERR(pm8058_s3_1v8)) {
        pr_err("%s: Unable to get 8058_s3\n", __func__);
        regulator_put(pm8901_l1_3v3);
        return -ENODEV;
    }
    bRet = regulator_set_voltage(pm8901_l1_3v3, 3300000, 3300000);
    if (bRet) {
        pr_err("%s: unable to set Pm8901_l1 voltage to 3.3V\n", __func__);
        goto fail;
    }

    bRet = regulator_set_voltage(pm8058_s3_1v8, 1800000, 1800000);
    if (bRet) {
        pr_err("%s: unable to set Pm8058_S3 voltage to 1.8V\n", __func__);
        goto fail;
    }

    bRet = regulator_enable(pm8901_l1_3v3);
    if (bRet) {
        pr_err("%s: Enable regulator pm8901_l1_3v3 failed\n", __func__);
        goto fail;
    }

    bRet = regulator_enable(pm8058_s3_1v8);
    if (bRet) {
        pr_err("%s: Enable regulator 8058_s3 failed\n", __func__);
        regulator_disable(pm8901_l1_3v3);
        goto fail;
    }

    if(csensor_major)
    {
        dev_number = MKDEV(csensor_major, csensor_minor);
        bRet = register_chrdev_region(dev_number,(csensor_nr_al3006_devs+1), "al3006device");
    }
    else
    {
        bRet = alloc_chrdev_region(&dev_number,csensor_minor,(csensor_nr_al3006_devs+1), "al3006device");
    }

    if(!bRet)
    {
        csensor_major = MAJOR(dev_number);
        printk(KERN_INFO"al3006_init: AL3006 major number is %d!\r\n",csensor_major);
    }
    else
    {
        printk(KERN_ERR"al3006_init: request major number fail!\r\n");
        return result;
    }     
    
    pal3006_dev = kmalloc((csensor_nr_al3006_devs+1)*sizeof(struct AL3006_DEV), GFP_KERNEL);
    if ( !pal3006_dev )/*alloc failure*/
    {
        result = -ENOMEM;    
        goto out;    
    }
    
    memset(pal3006_dev,0,(csensor_nr_al3006_devs+1)*sizeof(struct AL3006_DEV));
#if 0
    result = create_csensor_class();
    if(0 != result)
    {
        goto out_unreg_chrdev;
    }

    for(dev_idx = 0; dev_idx < csensor_nr_al3006_devs; dev_idx++)
    {
        al3006_setup_cdev(&pal3006_dev[dev_idx],dev_idx);
    }    
#endif

    printk("before call i2c_add_driver in %s func\n",__FUNCTION__);

    /* add i2c driver for al3006 */
    bRet = i2c_add_driver(&al3006_i2c_driver);
    if (bRet)
    {
        goto error_destroy;
    }
    printk(KERN_INFO "AL3006 device create ok\n");
    
    return 0;
    
error_destroy:
    i2c_del_driver(&al3006_i2c_driver);
    //return result;
out_unreg_chrdev:
    unregister_chrdev(al3006_major, "al3006device");
out:
    printk(KERN_ERR "%s: Driver Initialisation failed\n", __FILE__);
    //return bRet;
fail:
    regulator_put(pm8901_l1_3v3);
    regulator_put(pm8058_s3_1v8);
    return bRet;
}


/*
 * Unregisters/deletes driver as an i2c client driver.
 * Complement of acc_init.
 */
static void __exit al3006_exit(void)
{
    int i;
    dev_t devno = MKDEV(csensor_major, csensor_minor);
    pr_info(" %s \n",__FUNCTION__);
    i2c_del_driver(&al3006_i2c_driver);

    cdev_del(&pal3006_dev->al3006_cdev);

    if(pal3006_dev)
    {
        for( i = 0; i <= csensor_nr_al3006_devs; i++)
        {
            cdev_del(&pal3006_dev[i].al3006_cdev);

        }
        kfree(pal3006_dev);
    }

        unregister_chrdev_region(devno,(csensor_nr_al3006_devs+1));

    printk(KERN_ERR "AL3006 exit\n");
}

// module_init(al3006_init);
// module_exit(al3006_exit);
late_initcall(al3006_init);
module_exit(al3006_exit);

MODULE_AUTHOR(" YuLong Driver");
MODULE_DESCRIPTION("AL3006 ambient light and proximity sensor driver");

