
/*******************************************************************************
*                                                                              *
*   File Name:    ltr558als.c                                                      *
*   Description:   Linux device driver for Taos ambient light and         *
*   proximity sensors.                                     *
*   Author:    taojun                                           *
*
*   2011/11/4 create
*
********************************************************************************
*    Proprietary to yulong 
*******************************************************************************/


#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/pm.h>
#include <linux/poll.h>
#include <linux/wakelock.h>
#include <asm/gpio.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include <linux/ltr558als.h>

#define PROX_DEFAULT_THRESHOLD_HI  220//646
#define PROX_DEFAULT_THRESHOLD_LO  209//516
#define ALS_INTEGRATION_TIME 100

struct ltr558_data {
	      struct i2c_client *client;
	      struct mutex lock;
        struct input_dev *input_dev;
        struct input_dev *input_dev_ls;
        int prox_threshold_hi;
        int prox_threshold_lo;
        unsigned	int 	als_ps_int;
        struct wake_lock 	wake_lock;
};

struct prox_calibrate{
        int prox_data;
};

static struct work_struct irq_workqueue;
static struct ltr558_data *the_data = NULL;
static struct prox_calibrate prox_cal_info[20];

static int prox_active = 0;
static int light_active = 0;
static int ps_gainrange;
static int als_gainrange;
static int itg_tm;
static int als_integration_time;
static int GPIO_INT_NO = 255;

#define LTR558_DEBUG

#ifdef LTR558_DEBUG
	#define YL_DEBUG(fmt, args...) pr_info(fmt, ##args)
#else
	#define YL_DEBUG(fmt, args...)
#endif

static int ltr558_als_read(int gainrange);
static int ltr558_dev_init(void);
extern int yl_params_kernel_read(char *buf, size_t count, int ver);
extern int yl_params_kernel_write(char *buf, size_t count, int ver);

/* 
 * #########
 * ## I2C ##
 * #########
 */

// I2C Read
static int ltr558_i2c_read_reg(u8 regnum)
{
	int readdata;
	
	readdata = i2c_smbus_read_byte_data(the_data->client, regnum);
	
	return readdata;
}

// I2C Write
static int ltr558_i2c_write_reg(u8 regnum, u8 value)
{
	int writeerror;
	
	writeerror = i2c_smbus_write_byte_data(the_data->client, regnum, value);
	if (writeerror < 0)
		return writeerror;
	else
		return 0;
}



/****************************************/
/*
          calibrate
*/
/****************************************/
static void taos_write_crosstalk_to_flash(unsigned char* input)
{
	unsigned char CrossTalk[512] = "PROJECT0";
	int size = sizeof(CrossTalk);
	int ret = 0;

	ret = yl_params_kernel_read(CrossTalk, sizeof(CrossTalk), 1);
	if (NULL != input)
		memcpy(&CrossTalk[17], input, 3);

	pr_info("%s: write-CrossTalk = %s size = %d\n",
		__func__, CrossTalk, size);
	pr_info("%s: write-CrossTalk[17] = 0x%x\n", __func__, CrossTalk[17]);
	pr_info("%s: write-CrossTalk[18] = 0x%x\n", __func__, CrossTalk[18]);
	pr_info("%s: write-CrossTalk[19] = 0x%x\n", __func__, CrossTalk[19]);

	ret = yl_params_kernel_write(CrossTalk, size, 1);
	if (ret != sizeof(CrossTalk))
		pr_err("%s: write yl_params error!\n", __func__);
}

static void taos_read_crosstalk_from_flash(unsigned char* output)
{
   unsigned char CrossTalk[512] ="PROJECT0";
   int size = sizeof(CrossTalk);
   yl_params_kernel_read(CrossTalk,size,1);   

	printk(KERN_ERR "goto--output==%p\n",output);
   if(NULL != output){  

      memcpy(output,&CrossTalk[17],3);              

   } 

	printk(KERN_ERR"%s read-CrossTalk[16] = 0x%x\n",__func__,CrossTalk[17]);
	printk(KERN_ERR"%s read-CrossTalk[17] = 0x%x\n",__func__,CrossTalk[18]);
	printk(KERN_ERR"%s read-CrossTalk[18] = 0x%x\n",__func__,CrossTalk[19]);
	printk(KERN_ERR"%s read-CrossTalk = %s size = %d\n",__func__,CrossTalk,size);
}


static int ltr558_prox_poll(struct prox_calibrate *prox_cal_info)
{
        int psval_lo, psval_hi, psdata;

        psval_lo = ltr558_i2c_read_reg(LTR558_PS_DATA_0);
	if (psval_lo < 0){
		psdata = psval_lo;
                return -ENODATA;
	}
		
	psval_hi = ltr558_i2c_read_reg(LTR558_PS_DATA_1);
	if (psval_hi < 0){
		psdata = psval_hi;
                return -ENODATA;
	}
		
	prox_cal_info->prox_data = ((psval_hi & 7)* 256) + psval_lo;
       YL_DEBUG("%s : primitive prox_data psdata = %d lo %d  hi %d  ", __func__, prox_cal_info->prox_data,psval_lo,psval_hi); 
       
        return 0;
}

static int ltr558_prox_calibrate(int *cal_prox_mean)
{
            int prox_mean,i,prox_sum = 0,prox_max = 0,ret=0;
            char a[3];

            for (i = 0; i < 20; i++) {
                if ((ret = ltr558_prox_poll(&prox_cal_info[i])) < 0) {
                    printk(KERN_ERR "%s: call to prox_poll failed in ioctl prox_calibrate\n",__func__);
                    return (ret);
                }
                prox_sum += prox_cal_info[i].prox_data;
                if (prox_cal_info[i].prox_data > prox_max)
                    prox_max = prox_cal_info[i].prox_data;
                mdelay(100);
            }

            prox_mean = prox_sum/20;
            *cal_prox_mean=prox_mean;
            YL_DEBUG("%s:prox_mean = %d\n",__func__,prox_mean);
            if(prox_mean < 100)
            {
              //the_data->prox_threshold_hi = (u16)(12 * prox_mean/10);
              the_data->prox_threshold_hi = (u16)(prox_mean+20);
	    }
            else if(prox_mean < 400)
            {
	      the_data->prox_threshold_hi = (u16)(prox_mean+28);
           
	    }
	    else if(prox_mean < 600)
            {
              the_data->prox_threshold_hi = (u16)(18 * prox_mean/10);
	    }
            else
            {
                printk(KERN_ERR "%s:Clibrate failed!\n",__func__);
                
	    }

            the_data->prox_threshold_lo = 8 * the_data->prox_threshold_hi/10;
            if(the_data->prox_threshold_hi < 100)
            {
                //the_data->prox_threshold_hi = 200;
                the_data->prox_threshold_lo = (u16)(prox_mean+16);
	    }
            else if(the_data->prox_threshold_hi < 400)
            {
                //the_data->prox_threshold_hi = 200;
                the_data->prox_threshold_lo = (u16)(prox_mean+24);
	    }
            else if(the_data->prox_threshold_hi > 900)
            {
               the_data->prox_threshold_hi = 900;
               the_data->prox_threshold_lo = 750;
	    }
	    memset(a,0,sizeof(a));
	    a[0]=0x0A;
	    a[1]=the_data->prox_threshold_hi & 0x00ff;
	    a[2]=(the_data->prox_threshold_hi & 0xff00) >> 8;
	    taos_write_crosstalk_to_flash(a);
	    printk(KERN_ERR "%s: %d,%d,%d\n",__func__,a[0],a[1],a[2]);
            YL_DEBUG("%s :the_data->prox_threshold_hi = %d\n",__func__, the_data->prox_threshold_hi );
            YL_DEBUG("%s :the_data->prox_threshold_lo = %d\n",__func__, the_data->prox_threshold_lo );        
            return ret;
}

static int ltr558_ps_enable(int gainrange)
{
	int error=0;
	int setgain;
	int ps_contr_data = 0;
        char buf[4] = {0};
       buf[0] = the_data->prox_threshold_lo & 0x00;
       buf[1] = (the_data->prox_threshold_lo >> 8) & 0x00;
       buf[2] = the_data->prox_threshold_hi & 0x0ff;  //up
       buf[3] = (the_data->prox_threshold_hi >> 8) & 0x07;
       ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_0, buf[0]);
       ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_1, buf[1]);
       ltr558_i2c_write_reg(LTR558_PS_THRES_UP_0,  buf[2]);
       ltr558_i2c_write_reg(LTR558_PS_THRES_UP_1,  buf[3]);
       switch (gainrange) {
		case PS_RANGE1:
			setgain = MODE_PS_ON_Gain1;//03
			break;

		case PS_RANGE2:
			setgain = MODE_PS_ON_Gain2;//07
			break;

		case PS_RANGE4:
			setgain = MODE_PS_ON_Gain4;//0b
			break;

		case PS_RANGE8:
			setgain = MODE_PS_ON_Gain8;//0f
			break;

		default:
			setgain = MODE_PS_ON_Gain1;
			break;
	}

	error=ltr558_i2c_write_reg(LTR558_PS_CONTR, setgain); 
	mdelay(WAKEUP_DELAY);
	ps_contr_data  = ltr558_i2c_read_reg(LTR558_PS_CONTR);
	if (ps_contr_data != setgain) {
		error=ltr558_i2c_write_reg(LTR558_PS_CONTR, setgain);
		mdelay(WAKEUP_DELAY);
	}
	return error;
}

static int ltr558_ps_disable(void)
{
	int error;
	int ps_disable_data;

	error = ltr558_i2c_write_reg(LTR558_PS_CONTR, MODE_PS_StdBy);
	ps_disable_data = ltr558_i2c_read_reg(LTR558_PS_CONTR);
	if (ps_disable_data != MODE_PS_StdBy) {
		error = ltr558_i2c_write_reg(LTR558_PS_CONTR, MODE_PS_StdBy);
	}
	return error;
}


static int ltr558_ps_read(void)
{
	int psval_lo, psval_hi, psdata;
        int als_ps_status,interrupt,newdata;

        als_ps_status = ltr558_i2c_read_reg(LTR558_ALS_PS_STATUS);
	interrupt = als_ps_status & 10;
	newdata = als_ps_status & 5;
        printk(KERN_ERR"%s:interruptcode=%d,newdata=%d\n",__func__,interrupt,newdata);
        psval_lo = ltr558_i2c_read_reg(LTR558_PS_DATA_0);
	if (psval_lo < 0){
		psdata = psval_lo;
		goto out;
	}
		
	psval_hi = ltr558_i2c_read_reg(LTR558_PS_DATA_1);
	if (psval_hi < 0){
		psdata = psval_hi;
		goto out;
	}
		
	psdata = ((psval_hi & 7)* 256) + psval_lo;
        YL_DEBUG("%s : primitive prox_data psdata = %d", __func__, psdata); 
        YL_DEBUG("%s : the_data->prox_threshold_lo = %d", __func__, the_data->prox_threshold_lo); 
        YL_DEBUG("%s : the_data->prox_threshold_hi = %d", __func__, the_data->prox_threshold_hi); 
        if (psdata < the_data->prox_threshold_lo){   
		input_report_abs(the_data->input_dev, ABS_DISTANCE, 5);
		input_sync(the_data->input_dev);
		ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_0,0x00);
		ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_1,0x00);
		ltr558_i2c_write_reg(LTR558_PS_THRES_UP_0,
					the_data->prox_threshold_hi & 0xff);
		ltr558_i2c_write_reg(LTR558_PS_THRES_UP_1,
					(the_data->prox_threshold_hi >> 8) & 0x07);

        }else if (psdata > the_data->prox_threshold_hi){ 
		input_report_abs(the_data->input_dev,ABS_DISTANCE,3);
		input_sync(the_data->input_dev);
		ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_0,
				the_data->prox_threshold_lo & 0x0ff);
		ltr558_i2c_write_reg(LTR558_PS_THRES_LOW_1,
				(the_data->prox_threshold_lo >> 8) & 0x07);
		ltr558_i2c_write_reg(LTR558_PS_THRES_UP_0, 0xff);
		ltr558_i2c_write_reg(LTR558_PS_THRES_UP_1, 0x07);
       }
       return 0;
     out:
       return psdata;
}

static int ltr558_als_enable(int gainrange)
{
	int error=0;
	int als_enable_data;

	if (gainrange == 1) {
		error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_ON_Range1);//0x0b
		als_enable_data = ltr558_i2c_read_reg(LTR558_ALS_CONTR);
		if (als_enable_data != MODE_ALS_ON_Range1) {
			error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_ON_Range1);
		}
	}
	else if (gainrange == 2) {
		error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_ON_Range2);//03
		als_enable_data = ltr558_i2c_read_reg(LTR558_ALS_CONTR);
                if (als_enable_data != MODE_ALS_ON_Range2) {
			error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_ON_Range2);
                }
	}
	else
		error =-1;
        
	mdelay(WAKEUP_DELAY);
	return error;
}

static int ltr558_als_disable(void)
{
        int error;
	int als_disable_data;

	error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_StdBy); 
	als_disable_data = ltr558_i2c_read_reg(LTR558_ALS_CONTR);
	if (als_disable_data != MODE_ALS_StdBy) {
		error = ltr558_i2c_write_reg(LTR558_ALS_CONTR, MODE_ALS_StdBy);
	}
	return error;
}

static int ltr558_als_read(int gainrange)
{
	int alsval_ch0_lo, alsval_ch0_hi, alsval_ch0;
	int alsval_ch1_lo, alsval_ch1_hi, alsval_ch1;
        int ratio;
        int lux_val;
        int ch0_coeff,ch1_coeff;

	alsval_ch1_lo = ltr558_i2c_read_reg(LTR558_ALS_DATA_CH1_0);
	alsval_ch1_hi = ltr558_i2c_read_reg(LTR558_ALS_DATA_CH1_1);
	alsval_ch1 = (alsval_ch1_hi * 256) + alsval_ch1_lo;

	alsval_ch0_lo = ltr558_i2c_read_reg(LTR558_ALS_DATA_CH0_0);
	alsval_ch0_hi = ltr558_i2c_read_reg(LTR558_ALS_DATA_CH0_1);
	alsval_ch0 = (alsval_ch0_hi * 256) + alsval_ch0_lo;
        //YL_DEBUG("taojun_als_data0=%d,data1=%d,data2=%d,data3=%d\n",alsval_ch1_lo,alsval_ch1_hi,alsval_ch0_lo,alsval_ch0_hi);

	//ratio = alsval_ch1 / alsval_ch0;
        if ((alsval_ch0 + alsval_ch1) == 0) {
              YL_DEBUG("%s:Both CH0 and CH1 are zero\n",__func__);
              return 0;
         } else {
            ratio = (alsval_ch1 * 1000)  / (alsval_ch0+alsval_ch1);   //*1000
         }

        
       // YL_DEBUG(">>>>>%s: ratio = %d \n", __func__, ratio);
        if (ratio > 850)
        {
           ch0_coeff = 0;
           ch1_coeff = 0;
        }else if(ratio > 640)
        {
           ch0_coeff = 16900;  //*10000
           ch1_coeff = 1690;
        }else if(ratio > 450)
        {
           ch0_coeff = 37725;
           ch1_coeff = 13363;
        }else{
           ch0_coeff = 17743;
           ch1_coeff = -11059;
        }
     
        lux_val = (alsval_ch0*ch0_coeff - alsval_ch1*ch1_coeff)/10000; //0 to 75 in lux

        switch (als_integration_time)
        {
           case 100:
                itg_tm = 1;
                break;
           case 200:
                itg_tm = 2;
                break;
           case 400:
                itg_tm = 3;
                break;
           default:
                itg_tm = 1;
         }
         
         if(1== als_gainrange){
         	
         	lux_val = lux_val/itg_tm/200;
         }

	return lux_val; /**pc modify from.[lux_val/itg_tm/200].12.04.17**/
	// Compute Lux data from ALS data (ch0 and ch1)
	// For Ratio < 0.69:
	// 1.3618*CH0 â€?1.5*CH1
	// For 0.69 <= Ratio < 1:
	// 0.57*CH0 â€?0.345*CH1
	// For high gain, divide the calculated lux by 150.

}

static int ltr558_open(struct inode *inode, struct file *file)
{
        YL_DEBUG("%s: open\n", __func__);

	return 0;
}

static int ltr558_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long ltr558_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
 
   int ret = 0;
   struct prox_offset taos_cal_data;
   struct prox_offset *taos_cal_ptr = &taos_cal_data;
   unsigned char hi_cross[3]={0};  
   int prox_mean;
   

    switch (cmd) {
     case LTR558_IOCTL_ALS_ON:
	YL_DEBUG("%s: >>>Als on\n", __func__);
	if(0 == prox_active) {
		if (ltr558_als_enable(als_gainrange))
		return -EIO;
	}
	light_active = 1;
        break;
     case LTR558_IOCTL_ALS_OFF:
     	YL_DEBUG("%s: Als off\n", __func__);
	if (ltr558_als_disable())
		return -EIO;
	if (0 == prox_active)
	cancel_work_sync(&irq_workqueue);
	light_active = 0;
	break;
     case LTR558_IOCTL_PROX_ON:
     	YL_DEBUG("%s: Prox on\n", __func__);
      if(light_active == 1)
	ltr558_als_disable();
      if (ltr558_ps_enable(ps_gainrange)) {
		return -EIO;
	}
      ret = irq_set_irq_wake(the_data->als_ps_int,1);
      if (ret != 0) {
          printk(KERN_ERR"SENSOR_LTR:YLLOG:%s: set irq wake source failed! \n",__func__);
	  return(ret);
      }
      prox_active = 1;
      YL_DEBUG("%s: enable ltr558_als proximity \n", __func__);
     break;
     case LTR558_IOCTL_PROX_OFF:
     	YL_DEBUG("%s: Prox off\n", __func__);
	if (ltr558_ps_disable())
            return -EIO;
        prox_active = 0;
        ret = irq_set_irq_wake(the_data->als_ps_int,0);
        if (ret != 0) {
	    printk(KERN_ERR"SENSOR_TAOS:YLLOG:%s: clear irq wake source failed ! \n",__func__);
	    return(ret);
	}
        if (0 == light_active) {
           cancel_work_sync(&irq_workqueue);
           YL_DEBUG("%s:  disable ltr558_als light & prox  \n", __func__);
        }else{
           ltr558_als_enable(als_gainrange);
           YL_DEBUG("%s:  enable ltr558_als  \n", __func__);
        }
        break;
     case LTR558_IOCTL_PROX_OFFSET:
     	YL_DEBUG("%s: Prox offset\n", __func__);
          taos_read_crosstalk_from_flash(hi_cross);
	  printk(KERN_ERR "%s: %d,%d,%d\n",__func__,hi_cross[0],hi_cross[1],hi_cross[2]);
          if( hi_cross[0] == 0x0A )
	     the_data->prox_threshold_hi = hi_cross[2]<<8 | hi_cross[1];    
	  else{
	          the_data->prox_threshold_hi = PROX_DEFAULT_THRESHOLD_HI;
		  the_data->prox_threshold_lo = PROX_DEFAULT_THRESHOLD_LO;
	  }
           if(the_data->prox_threshold_hi<400)
             the_data->prox_threshold_lo=the_data->prox_threshold_hi-4;
          else if(400 < the_data->prox_threshold_hi && the_data->prox_threshold_hi < 900)  
	     the_data->prox_threshold_lo = 9 * the_data->prox_threshold_hi/10;
          else{
		the_data->prox_threshold_hi = PROX_DEFAULT_THRESHOLD_HI;
		the_data->prox_threshold_lo = PROX_DEFAULT_THRESHOLD_LO;
	    }
 	  printk(KERN_ERR"%s: prox_hig 	= [%5d] \n",__func__,the_data->prox_threshold_hi);
	  printk(KERN_ERR"%s: prox_low 	= [%5d] \n",__func__,the_data->prox_threshold_lo);
	  break;
      case LTR558_IOCTL_PROX_CALIBRATE:
      	YL_DEBUG("%s: >>>PROX_CALIBRATE\n", __func__);
          wake_lock(&the_data->wake_lock);
           if((ret = ltr558_prox_calibrate(&prox_mean)) < 0){
               ret = -EINVAL;
            }
           ((struct prox_offset *)taos_cal_ptr)->x = (unsigned short)(the_data->prox_threshold_hi);
	   ((struct prox_offset *)taos_cal_ptr)->y = (unsigned short)(the_data->prox_threshold_lo);
	   ((struct prox_offset *)taos_cal_ptr)->z = (unsigned short)(prox_mean);
           if(copy_to_user((struct prox_offset *)arg, taos_cal_ptr, sizeof(taos_cal_data)))
	      printk(KERN_ERR "%s: data trans error,use default offset ! \r\n",__func__);	
            wake_unlock(&the_data->wake_lock);		
          break;
       default:
       	YL_DEBUG("%s: default\n", __func__);
             ret = -EINVAL;
	   break;
	}
   return ret;
}
static struct file_operations ltr558_fops = {
	.owner		= THIS_MODULE,
	.open		= ltr558_open,
	.release	= ltr558_release,
	.unlocked_ioctl = ltr558_ioctl,
};
static struct miscdevice ltr558_dev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME_LTR558,
	.fops	= &ltr558_fops,
};


static void ltr558_schedwork(struct work_struct *work)
{
	int als_ps_status;
	int interrupt, newdata;
        int lux_val = 0;
	als_ps_status = ltr558_i2c_read_reg(LTR558_ALS_PS_STATUS);
	interrupt = als_ps_status & 10;
	newdata = als_ps_status & 5;
       // printk(KERN_ERR"taojun_interruptcode=%d,newdata=%d\n",interrupt,newdata);
	switch (interrupt){
		case 2:
		     if ((newdata == 1) | (newdata == 5))
		         wake_lock(&the_data->wake_lock);
             ltr558_ps_read();
             wake_unlock(&the_data->wake_lock);
		break;
                case 8:
		     if ((newdata == 4) | (newdata == 5)){
                          lux_val = ltr558_als_read(als_gainrange);
		          input_report_abs(the_data->input_dev, ABS_MISC, lux_val);
                          input_sync(the_data->input_dev);
			}
		break;
                case 10:
		     if ((newdata == 1) | (newdata == 5))
				ltr558_ps_read();
		     if ((newdata == 4) | (newdata == 5)){
				lux_val = ltr558_als_read(als_gainrange);
		                input_report_abs(the_data->input_dev, ABS_MISC, lux_val);
                                input_sync(the_data->input_dev);
			}
		break;
	}	
	enable_irq(the_data->als_ps_int);
}


static irqreturn_t ltr558_irq_handler(int irq, void *dev_id)
{
	disable_irq_nosync(the_data->als_ps_int);
	schedule_work(&irq_workqueue);
	return IRQ_HANDLED;
}

static int ltr558_dev_init(void)
{
	int error;
        ps_gainrange = PS_RANGE2;
        als_gainrange = ALS_RANGE2_64K;
        als_integration_time = ALS_INTEGRATION_TIME;

	error = ltr558_als_disable();
	if (error < 0)
		goto out;
	
	error = ltr558_ps_disable();
	if (error < 0)
		goto out;

       ltr558_i2c_write_reg(LTR558_PS_LED, 0x7b);   //0111 1011   60khz 100% 50ma
       ltr558_i2c_write_reg(LTR558_PS_N_PULSES, 0x08);   //8 pulses count
       ltr558_i2c_write_reg(LTR558_PS_MEAS_RATE, 0x00);  //50ms
       //ltr558_i2c_write_reg(LTR558_ALS_MEAS_RATE, 0x03);//500ms report   //modify by taojun
       ltr558_i2c_write_reg(LTR558_INTERRUPT, 0x03);//als and ps interrupt
       ltr558_i2c_write_reg(LTR558_INTERRUPT_PERSIST,0x50);
       
       ltr558_i2c_write_reg(LTR558_ALS_THRES_LOW_0, 0x01);
       ltr558_i2c_write_reg(LTR558_ALS_THRES_LOW_1, 0x00);
       ltr558_i2c_write_reg(LTR558_ALS_THRES_UP_0, 0x00);
       ltr558_i2c_write_reg(LTR558_ALS_THRES_UP_1, 0x00);
       error = 0;

	out:
	return error;
}

/*
 * return : 0--suc; non_zero fail
*/
static int ltr558_inputdev_init (void)
{
   int ret = 0 ;

   the_data->input_dev = input_allocate_device();
   if (the_data->input_dev == NULL) {
       return -ENOMEM;
   }
   
   the_data->input_dev->name = DEVICE_NAME_PS;   //DEVICE_NAME: prox
   the_data->input_dev->id.bustype = BUS_I2C;
   set_bit(EV_ABS,the_data->input_dev->evbit);
   input_set_capability(the_data->input_dev,EV_ABS,ABS_DISTANCE);
   input_set_capability(the_data->input_dev,EV_ABS,ABS_MISC);//add taojun
   input_set_abs_params(the_data->input_dev, ABS_MISC, 0, 100000, 0, 0); //add taojun
   input_set_abs_params(the_data->input_dev,ABS_DISTANCE, 0, 1000, 0, 0);//vincent for siji
   ret = input_register_device(the_data->input_dev);
   if (ret != 0){
       YL_DEBUG("fail to register input device in %s ", __func__); 
       input_free_device(the_data->input_dev);
       goto out;
   }
   out:
    return ret;
}

static int ltr558_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
        int manufac_id;
 
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);


        printk(KERN_ERR"%s:probe start \n",__func__);  
 
        manufac_id = i2c_smbus_read_byte_data(client, LTR558_MANUFACTURER_ID);
        if (manufac_id == 5)
        {
            YL_DEBUG("%s:lir558als ls & ps sensor is selected: manufac_id = %d \n",__func__,manufac_id);
        }else{
            return -1;
        }
        

	/* Return 1 if adapter supports everything we need, 0 if not. */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WRITE_BYTE | I2C_FUNC_SMBUS_READ_BYTE_DATA))
	{
		printk(KERN_ALERT "%s: LTR-558ALS functionality check failed.\n", __func__);
		ret = -EIO;
		goto out;

	}


	/* data memory allocation */
	the_data = (struct ltr558_data*)kzalloc(sizeof(struct ltr558_data), GFP_KERNEL);
	if (the_data == NULL) {
		printk(KERN_ALERT "%s: LTR-558ALS kzalloc failed.\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

        the_data->client = client;
	i2c_set_clientdata(client, the_data);
        

        the_data->prox_threshold_hi = PROX_DEFAULT_THRESHOLD_HI;
        the_data->prox_threshold_lo = PROX_DEFAULT_THRESHOLD_LO;



//	wake_lock_init(&proximity_wake_lock, WAKE_LOCK_SUSPEND, "proximity");
	INIT_WORK(&irq_workqueue,ltr558_schedwork);
        wake_lock_init(&the_data->wake_lock, WAKE_LOCK_SUSPEND, "taos_wake_lock");
        ltr558_inputdev_init(); 
        
        gpio_request(LTR558_GPIO_IRQ, DEVICE_NAME_LTR558);
        gpio_direction_input(LTR558_GPIO_IRQ);
        the_data->als_ps_int = the_data->client->irq;
        ret = request_irq(the_data->als_ps_int, ltr558_irq_handler, IRQF_TRIGGER_FALLING, DEVICE_NAME_LTR558, NULL);
	if (ret) {
		printk(KERN_ALERT "%s: LTR-558ALS request irq failed.\n", __func__);
		goto out;
	}
	mdelay(PON_DELAY);
        ret = ltr558_dev_init();
	if (ret) {
		printk(KERN_ALERT "%s: LTR-558ALS device init failed.\n", __func__);
		goto out;
	}
                ret = misc_register(&ltr558_dev);
	if (ret) {
		printk(KERN_ALERT "%s: LTR-558ALS misc_register als failed.\n", __func__);
		goto outps;
	}
	ret = 0;
	goto out;

	outps:
	misc_deregister(&ltr558_dev);
	out:
	return ret;
}


static int ltr558_remove(struct i2c_client *client)
{
	kfree(i2c_get_clientdata(client));

//	free_irq(gpio_to_irq(GPIO_INT_NO), NULL);
       free_irq(the_data->als_ps_int, NULL) ;
	gpio_free(GPIO_INT_NO);

//	wake_lock_destroy(&proximity_wake_lock);

	misc_deregister(&ltr558_dev);
	ltr558_ps_disable();//first
	ltr558_als_disable();//interrupts diable too

	
	return 0;
}


static int ltr558_suspend(struct i2c_client *client, pm_message_t mesg)
{
	int ret=0;

	printk(KERN_ERR"%s:suspend start \n",__func__);
	ret = ltr558_ps_disable();
	if (ret == 0)
		ret = ltr558_als_disable();

	disable_irq(the_data->als_ps_int);

	printk(KERN_ERR"%s:suspend end \n",__func__);

	return ret;
}


static int ltr558_resume(struct i2c_client *client)
{
	int ret=0;

	printk(KERN_ERR"%s:resume start \n",__func__); 
	enable_irq(the_data->als_ps_int);
	ret = ltr558_dev_init();
	if (ret) {
		printk(KERN_ERR "%s: LTR-558ALS device init failed.\n", __func__);
		return ret;
	}
	if (prox_active == 1)
	ltr558_ps_enable(ps_gainrange);
//       ltr558_ps_enable(ps_gainrange); // first
//       ltr558_als_enable(als_gainrange);
	printk(KERN_ERR"%s:resume end \n",__func__);
	return ret;
}


static const struct i2c_device_id ltr558_id[] = {
	{ DEVICE_I2C_ID, 0 },
	{}
};


static struct i2c_driver ltr558_driver = {
	.probe = ltr558_probe,
	.remove = ltr558_remove,
	.id_table = ltr558_id,
	.driver = {
		.owner = THIS_MODULE,
		.name = DEVICE_CLIENT,
	},
	.suspend = ltr558_suspend,
	.resume = ltr558_resume,
};


static int __init ltr558_driverinit(void)
{
	printk(KERN_ALERT "%s: LTR-558ALS Driver Module LOADED \n", __func__);
	return i2c_add_driver(&ltr558_driver);
}


static void __exit ltr558_driverexit(void)
{
	i2c_del_driver(&ltr558_driver);
	printk(KERN_ALERT "%s: LTR-558ALS Driver Module REMOVED \n", __func__);
	printk(KERN_INFO "%s\n", "0012345678900");
}


module_init(ltr558_driverinit)
module_exit(ltr558_driverexit)

MODULE_AUTHOR("Lite-On Technology Corp");
MODULE_DESCRIPTION("LTR-558ALS Driver");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(DRIVER_VERSION);

