/****************************************************************************/
/*                                                                          */
/*             Copyright (c) 2011-2012  YULONG Company                      */
/*                                                                          */
/* PROPRIETARY RIGHTS of YULONG Company are involved in the                 */
/* subject matter of this material.  All manufacturing, reproduction, use,  */
/* and sales rights pertaining to this subject matter are governed by the   */
/* license agreement.  The recipient of this software implicitly accepts    */
/* the terms of the license.                                                */
/*                                                                          */
/****************************************************************************/

#ifndef __ANDROID_DEBUG_H__
#define __ANDROID_DEBUG_H__

#define IOCTL_UART_MAGIC	'D'
#define IOCTL_UART_SWITCH_AP \
			_IOWR(IOCTL_UART_MAGIC, 1, unsigned long)
#define IOCTL_UART_SWITCH_WIFI_LOG \
			_IOWR(IOCTL_UART_MAGIC, 2, unsigned long)
#define IOCTL_UART_SWITCH_MODEM \
			_IOWR(IOCTL_UART_MAGIC, 3, unsigned long)
#define IOCTL_UART_SWITCH_WIFI_RF_TEST \
			_IOWR(IOCTL_UART_MAGIC, 4, unsigned long)
#define IOCTL_USB_SWITCH_MODEM \
			_IOWR(IOCTL_UART_MAGIC, 5, unsigned long)
#define IOCTL_USB_SWITCH_AP \
			_IOWR(IOCTL_UART_MAGIC, 6, unsigned long)
#define IOCTL_GET_CPU_VERSION \
			_IOWR(IOCTL_UART_MAGIC, 7, unsigned long)

struct ANDROID_DEBUG_DEV {
	struct cdev android_debug_cdev;
	int major;
	int name;
};

#endif
