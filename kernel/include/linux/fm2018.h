#ifndef FM2018_H
#define FM2018_H

#include <linux/ioctl.h>

struct fm2018_platform_data {
	int gpio_fm2018_pwdn;
	int gpio_fm2018_scl;
	int gpio_fm2018_sda;
	int gpio_fm2018_rst;
	int gpio_fm2018_irq_ana;
	int (*fm2018_power)(int);
	int (*priv_data_set)(void *);
};

#define FM2018_IOCTL_MAGIC 'F'

#define FM2018_IOCTL_SET_BYPASS_MODE _IOWR(FM2018_IOCTL_MAGIC, 1, unsigned long)
#define FM2018_IOCTL_POWER_OFF _IOWR(FM2018_IOCTL_MAGIC, 2, unsigned long)
#define FM2018_IOCTL_CDMA_HANDSET _IOWR(FM2018_IOCTL_MAGIC, 3, unsigned long)
#define FM2018_IOCTL_CDMA_HANDFREE _IOWR(FM2018_IOCTL_MAGIC, 4, unsigned long)
#define FM2018_IOCTL_LOCAL_RECORD _IOWR(FM2018_IOCTL_MAGIC, 5, unsigned long)
#define FM2018_IOCTL_GSM_HANDSET _IOWR(FM2018_IOCTL_MAGIC, 6, unsigned long)
#define FM2018_IOCTL_GSM_HANDFREE _IOWR(FM2018_IOCTL_MAGIC, 7, unsigned long)

#endif
