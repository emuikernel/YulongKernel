/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#include "msm.h"

#define SENSOR_NAME "mt9v113"
#define PLATFORM_DRIVER_NAME "msm_camera_mt9v113"
#define mt9v113_obj mt9v113_##obj
#define MSB                             1
#define LSB                             0

#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

#define MT9V113_VERBOSE_DGB

#ifdef MT9V113_VERBOSE_DGB
#define CDBG(fmt, args...) printk(fmt, ##args)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#endif

#define MT9V113_I2C_MAX_TIMES 4

DEFINE_MUTEX(mt9v113_mut);
static int effect_value = CAMERA_EFFECT_OFF;
extern int lcd_camera_power_onoff(int on);

static int mt9v113_i2c_write_word(unsigned short saddr,
		unsigned int waddr, unsigned int wdata);
static int mt9v113_i2c_write_byte(unsigned short saddr,
		unsigned int waddr, unsigned int wdata);

#define MT9V113_WriteSettingTbl(pTbl) \
		MT9V113_WriteRegsTbl(pTbl, sizeof(pTbl)/sizeof(pTbl[0]))

/******************************************************************************
MT9V113_WREG, *PMT9V113_WREG Definition
******************************************************************************/
typedef struct {
	unsigned int addr;          /*Reg Addr :16Bit*/
	unsigned int data;          /*Reg Data :16Bit or 8Bit*/
	unsigned int data_format;   /*Reg Data Format:16Bit = 0,8Bit = 1*/
	unsigned int delay_ms;      /*Operation NOP time(ms)*/
} MT9V113_WREG, *PMT9V113_WREG;

static struct msm_camera_i2c_reg_conf mt9v113_start_settings[] = {
	{0x3C40, 0x783C, 2, 0},  /* streaming on */
};

static struct msm_camera_i2c_reg_conf mt9v113_stop_settings[] = {
	{0x3C40, 0x7836, 2, 0},  /* streaming off*/
};

/******************************************************************************
Initial Setting Table
******************************************************************************/
MT9V113_WREG mt9v113_init_tbl[] = {
	{0x0010, 0x0216, 0, 0},
	{0x0012, 0x0B00, 0, 0},
	{0x0014, 0x2047, 0, 5},
	{0x0014, 0x2046, 0, 5},
	{0x0018, 0x4505, 0, 5},
	{0x0018, 0x4504, 0, 100},
	{0x0042, 0xFFF3, 0, 0},
	{0x3C00, 0x5004, 0, 0},
	{0x001A, 0x0520, 0, 0},
	{0x001A, 0x0564, 0, 100},
	{0x0012, 0x0200, 0, 0},
	{0x300A, 0x03BE, 0, 0},
	{0x300C, 0x02D6, 0, 0},
	{0x3010, 0x0012, 0, 0},
	{0x3040, 0x0041, 0, 0},
	{0x098E, 0x9803, 0, 0},
	{0x9803, 0x07,   1, 0},
	{0xA06E, 0x0098, 0, 0},
	{0xA070, 0x007E, 0, 0},
	{0xA072, 0x11,   1, 0},
	{0xA073, 0x13,   1, 0},
	{0xA074, 0x14,   1, 0},
	{0xA075, 0x16,   1, 0},
	{0xA076, 0x0006, 0, 0},
	{0xA078, 0x0007, 0, 0},
	{0xA01A, 0x0008, 0, 5},
	{0x3168, 0x84F8, 0, 0},
	{0x316A, 0x028A, 0, 0},
	{0x316C, 0xB477, 0, 0},
	{0x316E, 0x8268, 0, 0},
	{0x3180, 0x87FF, 0, 0},
	{0x3E02, 0x0600, 0, 0},
	{0x3E04, 0x221C, 0, 0},
	{0x3E06, 0x3632, 0, 0},
	{0x3E08, 0x3204, 0, 0},
	{0x3E0A, 0x3106, 0, 0},
	{0x3E0C, 0x3025, 0, 0},
	{0x3E0E, 0x190B, 0, 0},
	{0x3E10, 0x0700, 0, 0},
	{0x3E12, 0x24FF, 0, 0},
	{0x3E14, 0x3731, 0, 0},
	{0x3E16, 0x0401, 0, 0},
	{0x3E18, 0x211E, 0, 0},
	{0x3E1A, 0x3633, 0, 0},
	{0x3E1C, 0x3107, 0, 0},
	{0x3E1E, 0x1A16, 0, 0},
	{0x3E20, 0x312D, 0, 0},
	{0x3E22, 0x3303, 0, 0},
	{0x3E24, 0x1401, 0, 0},
	{0x3E26, 0x0600, 0, 0},
	{0x3E30, 0x0037, 0, 0},
	{0x3E32, 0x1638, 0, 0},
	{0x3E90, 0x0E05, 0, 0},
	{0x3E92, 0x1310, 0, 0},
	{0x3E94, 0x0904, 0, 0},
	{0x3E96, 0x0B00, 0, 0},
	{0x3E98, 0x130B, 0, 0},
	{0x3E9A, 0x0C06, 0, 0},
	{0x3E9C, 0x1411, 0, 0},
	{0x3E9E, 0x0E01, 0, 0},
	{0x3ECC, 0x4091, 0, 0},
	{0x3ECE, 0x430D, 0, 0},
	{0x3ED0, 0x1817, 0, 0},
	{0x3ED2, 0x8504, 0, 2},
	{0x0982, 0x0000, 0, 0},
	{0x098A, 0x0000, 0, 0},
	{0x8251, 0x3C3C, 0, 0},
	{0x8253, 0xBDD1, 0, 0},
	{0x8255, 0xF2D6, 0, 0},
	{0x8257, 0x15C1, 0, 0},
	{0x8259, 0x0126, 0, 0},
	{0x825B, 0x3ADC, 0, 0},
	{0x825D, 0x0A30, 0, 0},
	{0x825F, 0xED02, 0, 0},
	{0x8261, 0xDC08, 0, 0},
	{0x8263, 0xED00, 0, 0},
	{0x8265, 0xFC01, 0, 0},
	{0x8267, 0xFCBD, 0, 0},
	{0x8269, 0xF5FC, 0, 0},
	{0x826B, 0x30EC, 0, 0},
	{0x826D, 0x02FD, 0, 0},
	{0x826F, 0x0344, 0, 0},
	{0x8271, 0xB303, 0, 0},
	{0x8273, 0x4025, 0, 0},
	{0x8275, 0x0DCC, 0, 0},
	{0x8277, 0x3180, 0, 0},
	{0x8279, 0xED00, 0, 0},
	{0x827B, 0xCCA0, 0, 0},
	{0x827D, 0x00BD, 0, 0},
	{0x827F, 0xFBFB, 0, 0},
	{0x8281, 0x2013, 0, 0},
	{0x8283, 0xFC03, 0, 0},
	{0x8285, 0x44B3, 0, 0},
	{0x8287, 0x0342, 0, 0},
	{0x8289, 0x220B, 0, 0},
	{0x828B, 0xCC31, 0, 0},
	{0x828D, 0x80ED, 0, 0},
	{0x828F, 0x00CC, 0, 0},
	{0x8291, 0xA000, 0, 0},
	{0x8293, 0xBDFC, 0, 0},
	{0x8295, 0x1738, 0, 0},
	{0x8297, 0x3839, 0, 0},
	{0x8299, 0x3CD6, 0, 0},
	{0x829B, 0x15C1, 0, 0},
	{0x829D, 0x0126, 0, 0},
	{0x829F, 0x70FC, 0, 0},
	{0x82A1, 0x0344, 0, 0},
	{0x82A3, 0xB303, 0, 0},
	{0x82A5, 0x4025, 0, 0},
	{0x82A7, 0x13FC, 0, 0},
	{0x82A9, 0x7E26, 0, 0},
	{0x82AB, 0x83FF, 0, 0},
	{0x82AD, 0xFF27, 0, 0},
	{0x82AF, 0x0BFC, 0, 0},
	{0x82B1, 0x7E26, 0, 0},
	{0x82B3, 0xFD03, 0, 0},
	{0x82B5, 0x4CCC, 0, 0},
	{0x82B7, 0xFFFF, 0, 0},
	{0x82B9, 0x2013, 0, 0},
	{0x82BB, 0xFC03, 0, 0},
	{0x82BD, 0x44B3, 0, 0},
	{0x82BF, 0x0342, 0, 0},
	{0x82C1, 0x220E, 0, 0},
	{0x82C3, 0xFC7E, 0, 0},
	{0x82C5, 0x2683, 0, 0},
	{0x82C7, 0xFFFF, 0, 0},
	{0x82C9, 0x2606, 0, 0},
	{0x82CB, 0xFC03, 0, 0},
	{0x82CD, 0x4CFD, 0, 0},
	{0x82CF, 0x7E26, 0, 0},
	{0x82D1, 0xFC7E, 0, 0},
	{0x82D3, 0x2683, 0, 0},
	{0x82D5, 0xFFFF, 0, 0},
	{0x82D7, 0x2605, 0, 0},
	{0x82D9, 0xFC03, 0, 0},
	{0x82DB, 0x4A20, 0, 0},
	{0x82DD, 0x03FC, 0, 0},
	{0x82DF, 0x0348, 0, 0},
	{0x82E1, 0xFD7E, 0, 0},
	{0x82E3, 0xD0FC, 0, 0},
	{0x82E5, 0x7ED2, 0, 0},
	{0x82E7, 0x5F84, 0, 0},
	{0x82E9, 0xF030, 0, 0},
	{0x82EB, 0xED00, 0, 0},
	{0x82ED, 0xDC0A, 0, 0},
	{0x82EF, 0xB303, 0, 0},
	{0x82F1, 0x4625, 0, 0},
	{0x82F3, 0x10EC, 0, 0},
	{0x82F5, 0x0027, 0, 0},
	{0x82F7, 0x0CFD, 0, 0},
	{0x82F9, 0x034E, 0, 0},
	{0x82FB, 0xFC7E, 0, 0},
	{0x82FD, 0xD284, 0, 0},
	{0x82FF, 0x0FED, 0, 0},
	{0x8301, 0x0020, 0, 0},
	{0x8303, 0x19DC, 0, 0},
	{0x8305, 0x0AB3, 0, 0},
	{0x8307, 0x0346, 0, 0},
	{0x8309, 0x2415, 0, 0},
	{0x830B, 0xEC00, 0, 0},
	{0x830D, 0x8300, 0, 0},
	{0x830F, 0x0026, 0, 0},
	{0x8311, 0x0EFC, 0, 0},
	{0x8313, 0x7ED2, 0, 0},
	{0x8315, 0x840F, 0, 0},
	{0x8317, 0xFA03, 0, 0},
	{0x8319, 0x4FBA, 0, 0},
	{0x831B, 0x034E, 0, 0},
	{0x831D, 0xFD7E, 0, 0},
	{0x831F, 0xD2BD, 0, 0},
	{0x8321, 0xD2AD, 0, 0},
	{0x8323, 0x3839, 0, 0},
	{0x098E, 0x0000, 0, 0},
	{0x0982, 0x0000, 0, 0},
	{0x098A, 0x0000, 0, 0},
	{0x8340, 0x0048, 0, 0},
	{0x8342, 0x0040, 0, 0},
	{0x8344, 0x0000, 0, 0},
	{0x8346, 0x0040, 0, 0},
	{0x8348, 0x1817, 0, 0},
	{0x834A, 0x1857, 0, 0},
	{0x098E, 0x0000, 0, 0},
	{0x0982, 0x0000, 0, 0},
	{0x098A, 0x0000, 0, 0},
	{0x824D, 0x0251, 0, 0},
	{0x824F, 0x0299, 0, 0},
	{0x098E, 0x0000, 0, 2},
	{0x3210, 0x00B0, 0, 0},
	{0x3640, 0x00F0, 0, 0},
	{0x3642, 0x32A8, 0, 0},
	{0x3644, 0x0D50, 0, 0},
	{0x3646, 0xCE4C, 0, 0},
	{0x3648, 0xC1CF, 0, 0},
	{0x364A, 0x0190, 0, 0},
	{0x364C, 0x70EA, 0, 0},
	{0x364E, 0x18D0, 0, 0},
	{0x3650, 0xFECC, 0, 0},
	{0x3652, 0xCBCF, 0, 0},
	{0x3654, 0x01B0, 0, 0},
	{0x3656, 0x7BEB, 0, 0},
	{0x3658, 0x09B0, 0, 0},
	{0x365A, 0xFDAC, 0, 0},
	{0x365C, 0xF32F, 0, 0},
	{0x365E, 0x03B0, 0, 0},
	{0x3660, 0xAE0A, 0, 0},
	{0x3662, 0x18F0, 0, 0},
	{0x3664, 0xA7CC, 0, 0},
	{0x3666, 0xD80F, 0, 0},
	{0x3680, 0x518C, 0, 0},
	{0x3682, 0x826D, 0, 0},
	{0x3684, 0xD92D, 0, 0},
	{0x3686, 0x7BEC, 0, 0},
	{0x3688, 0x31CD, 0, 0},
	{0x368A, 0x028D, 0, 0},
	{0x368C, 0xF8AB, 0, 0},
	{0x368E, 0x9C0C, 0, 0},
	{0x3690, 0x320E, 0, 0},
	{0x3692, 0x1E8D, 0, 0},
	{0x3694, 0x130C, 0, 0},
	{0x3696, 0x888C, 0, 0},
	{0x3698, 0xD44C, 0, 0},
	{0x369A, 0x2E6E, 0, 0},
	{0x369C, 0x5D2E, 0, 0},
	{0x369E, 0x262C, 0, 0},
	{0x36A0, 0xD92C, 0, 0},
	{0x36A2, 0x8B6E, 0, 0},
	{0x36A4, 0x0FEC, 0, 0},
	{0x36A6, 0x2F8E, 0, 0},
	{0x36C0, 0x27B0, 0, 0},
	{0x36C2, 0x91EE, 0, 0},
	{0x36C4, 0xFEF1, 0, 0},
	{0x36C6, 0x646F, 0, 0},
	{0x36C8, 0x2352, 0, 0},
	{0x36CA, 0x25D0, 0, 0},
	{0x36CC, 0x950F, 0, 0},
	{0x36CE, 0xD571, 0, 0},
	{0x36D0, 0x04B1, 0, 0},
	{0x36D2, 0x1752, 0, 0},
	{0x36D4, 0x1390, 0, 0},
	{0x36D6, 0xDB2E, 0, 0},
	{0x36D8, 0x9232, 0, 0},
	{0x36DA, 0x3A70, 0, 0},
	{0x36DC, 0x64B2, 0, 0},
	{0x36DE, 0x2210, 0, 0},
	{0x36E0, 0x892D, 0, 0},
	{0x36E2, 0x8D32, 0, 0},
	{0x36E4, 0x142F, 0, 0},
	{0x36E6, 0x3812, 0, 0},
	{0x3700, 0xA52B, 0, 0},
	{0x3702, 0x0F4C, 0, 0},
	{0x3704, 0xA3AE, 0, 0},
	{0x3706, 0xB18D, 0, 0},
	{0x3708, 0x3E90, 0, 0},
	{0x370A, 0xA5AE, 0, 0},
	{0x370C, 0x2E4C, 0, 0},
	{0x370E, 0xEFAF, 0, 0},
	{0x3710, 0x8A11, 0, 0},
	{0x3712, 0x7A10, 0, 0},
	{0x3714, 0x98CE, 0, 0},
	{0x3716, 0x900B, 0, 0},
	{0x3718, 0x80EC, 0, 0},
	{0x371A, 0xC7B0, 0, 0},
	{0x371C, 0x8770, 0, 0},
	{0x371E, 0xD4AB, 0, 0},
	{0x3720, 0x63CA, 0, 0},
	{0x3722, 0x55CC, 0, 0},
	{0x3724, 0xA62C, 0, 0},
	{0x3726, 0x456F, 0, 0},
	{0x3740, 0x9290, 0, 0},
	{0x3742, 0x048E, 0, 0},
	{0x3744, 0x5D72, 0, 0},
	{0x3746, 0xCB11, 0, 0},
	{0x3748, 0xAF73, 0, 0},
	{0x374A, 0x95F0, 0, 0},
	{0x374C, 0x0531, 0, 0},
	{0x374E, 0x0372, 0, 0},
	{0x3750, 0x9B53, 0, 0},
	{0x3752, 0x80B2, 0, 0},
	{0x3754, 0xA7D0, 0, 0},
	{0x3756, 0x5A8F, 0, 0},
	{0x3758, 0x0A93, 0, 0},
	{0x375A, 0x9A52, 0, 0},
	{0x375C, 0xFFF3, 0, 0},
	{0x375E, 0x8B70, 0, 0},
	{0x3760, 0x976D, 0, 0},
	{0x3762, 0x0CD3, 0, 0},
	{0x3764, 0x9690, 0, 0},
	{0x3766, 0xDDB3, 0, 0},
	{0x3782, 0x00EC, 0, 0},
	{0x3784, 0x014C, 0, 0},
	{0x3210, 0x00B8, 0, 0},
	{0xA02F, 0x01FC, 0, 0},
	{0xA031, 0xFEBE, 0, 0},
	{0xA033, 0x0046, 0, 0},
	{0xA035, 0xFFBD, 0, 0},
	{0xA037, 0x013E, 0, 0},
	{0xA039, 0x0005, 0, 0},
	{0xA03B, 0xFF90, 0, 0},
	{0xA03D, 0xFF14, 0, 0},
	{0xA03F, 0x025C, 0, 0},
	{0xA041, 0x001A, 0, 0},
	{0xA043, 0x004B, 0, 0},
	{0xA045, 0xFFD0, 0, 0},
	{0xA047, 0x00AC, 0, 0},
	{0xA049, 0xFF84, 0, 0},
	{0xA04B, 0xFFF1, 0, 0},
	{0xA04D, 0x002A, 0, 0},
	{0xA04F, 0xFFE5, 0, 0},
	{0xA051, 0x0060, 0, 0},
	{0xA053, 0x0079, 0, 0},
	{0xA055, 0xFF27, 0, 0},
	{0xA057, 0x0010, 0, 0},
	{0xA059, 0xFFD9, 0, 0},
	{0xA061, 0x003C, 0, 0},
	{0xA063, 0x0035, 0, 0},
	{0xA065, 0x03  , 1, 0},
	{0xA066, 0x02  , 1, 0},
	{0x2112, 0x89AF, 0, 0},
	{0x2114, 0x4BC5, 0, 0},
	{0x2116, 0x4792, 0, 0},
	{0x2118, 0xCE71, 0, 0},
	{0x211A, 0x87C1, 0, 0},
	{0x211C, 0xB2CA, 0, 0},
	{0x211E, 0xCB8C, 0, 0},
	{0x2120, 0x9AEE, 0, 2},
	{0xA068, 0x8A  , 1, 0},
	{0xA06A, 0x7C  , 1, 0},
	{0xA06D, 0x7C  , 1, 0},
	{0xA06B, 0x84  , 1, 2},
	{0x9418, 0x2E  , 1, 0},
	{0x326E, 0x0006, 0, 0},
	{0x33F4, 0x000B, 0, 0},
	{0xA07A, 0x10  , 1, 0},
	{0xA07C, 0x04  , 1, 0},
	{0xA081, 0x1E  , 1, 0},
	{0xA082, 0x50  , 1, 0},
	{0xA0B1, 0x10  , 1, 0},
	{0xA0B2, 0x2D  , 1, 0},
	{0xA0B3, 0x10  , 1, 0},
	{0xA0B4, 0x2D  , 1, 0},
	{0xA0B5, 0x10  , 1, 0},
	{0xA0B6, 0x2D  , 1, 0},
	{0xA0B7, 0x10  , 1, 0},
	{0xA0B8, 0x2D  , 1, 0},
	{0xA05F, 0x80  , 1, 0},
	{0xA060, 0x05  , 1, 0},
	{0xA0B9, 0x0026, 0, 0},
	{0xA0BB, 0x00B4, 0, 0},
	{0xA07E, 0x001E, 0, 0},
	{0xA083, 0x03E8, 0, 0},
	{0xA085, 0x0096, 0, 0},
	{0x9C00, 0xFF  , 1, 0},
	{0xA087, 0x00  , 1, 0},
	{0xA088, 0x07  , 1, 0},
	{0xA089, 0x16  , 1, 0},
	{0xA08A, 0x30  , 1, 0},
	{0xA08B, 0x52  , 1, 0},
	{0xA08C, 0x6D  , 1, 0},
	{0xA08D, 0x86  , 1, 0},
	{0xA08E, 0x9B  , 1, 0},
	{0xA08F, 0xAB  , 1, 0},
	{0xA090, 0xB9  , 1, 0},
	{0xA091, 0xC5  , 1, 0},
	{0xA092, 0xCF  , 1, 0},
	{0xA093, 0xD8  , 1, 0},
	{0xA094, 0xE0  , 1, 0},
	{0xA095, 0xE7  , 1, 0},
	{0xA096, 0xEE  , 1, 0},
	{0xA097, 0xF4  , 1, 0},
	{0xA098, 0xFA  , 1, 0},
	{0xA0AD, 0x0005, 0, 0},
	{0xA0AF, 0x0021, 0, 0},
	{0xA020, 0x3C  , 1, 0},
	{0xA027, 0x002A, 0, 0},
	{0xA029, 0x0130, 0, 0},
	{0xA01C, 0x0060, 0, 0},
	{0xA023, 0x0080, 0, 0},
	{0xA025, 0x0080, 0, 0},
	{0xA01E, 0x0080, 0, 0},
	{0xA07C, 0x02  , 1, 0},
	{0xA07D, 0x01  , 1, 0},
	{0xA05F, 0x80  , 1, 0},
	{0xA060, 0x32  , 1, 0},
	{0x9C02, 0x01  , 1, 0},
	{0x8C03, 0x01  , 1, 0},
	{0x8C04, 0x03  , 1, 0},
	{0x8C05, 0x05  , 1, 2},
	{0x329E, 0x0000, 0, 0},
	{0x0018, 0x0002, 0, 50},
};


static unsigned char mt9v113_counter;
static unsigned char mt9v113_i2c_buf[4] = {0};
static struct i2c_client    *mt9v113_client;

static struct msm_sensor_ctrl_t mt9v113_s_ctrl;

static struct msm_camera_i2c_reg_conf mt9v113_prev_settings[] = {
};

static struct msm_camera_i2c_reg_conf mt9v113_snap_settings[] = {
};

static struct v4l2_subdev_info mt9v113_subdev_info[] = {
	{
		.code   = V4L2_MBUS_FMT_YUYV8_2X8,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt    = 1,
		.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array mt9v113_confs[] = {
	{&mt9v113_snap_settings[0],
	ARRAY_SIZE(mt9v113_snap_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
	{&mt9v113_prev_settings[0],
	ARRAY_SIZE(mt9v113_prev_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_sensor_output_info_t mt9v113_dimensions[] = {
	{
		.x_output = 0x280,
		.y_output = 0x1E0,
		.line_length_pclk = 0x287,
		.frame_length_lines = 0x1E7,
		.vt_pixel_clk = 14000000,
		.op_pixel_clk = 14000000,
		.binning_factor = 1,
	},
	{
		.x_output = 0x280,
		.y_output = 0x1E0,
		.line_length_pclk = 0x287,
		.frame_length_lines = 0x1E7,
		.vt_pixel_clk = 14000000,
		.op_pixel_clk = 14000000,
		.binning_factor = 1,
	},
};

static int mt9v113_brightness_enum_map[] = {
	CAMERA_BRIGHTNESS_LV0,
	CAMERA_BRIGHTNESS_LV1,
	CAMERA_BRIGHTNESS_LV2,
	CAMERA_BRIGHTNESS_LV3,/* DEFAULT */
	CAMERA_BRIGHTNESS_LV4,
	CAMERA_BRIGHTNESS_LV5,
	CAMERA_BRIGHTNESS_LV6,
};

static struct msm_camera_i2c_enum_conf_array mt9v113_brightness_enum_confs = {
	/* &mt9v113_brightness_confs[0][0] */
	.conf = NULL,
	.conf_enum = mt9v113_brightness_enum_map,
	.num_enum = ARRAY_SIZE(mt9v113_brightness_enum_map),
	/* ARRAY_SIZE(mt9v113_brightness_confs) */
	.num_index = 0,
	/* ARRAY_SIZE(mt9v113_brightness_confs[0]) */
	.num_conf = 0,
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static int mt9v113_set_brightness(int brightness)
{
	int rc = 0;

	pr_debug("mt9v113_set_brightness brightness:%d\n", brightness);
	switch (brightness) {
	case CAMERA_BRIGHTNESS_LV0:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x1C);
		break;
	case CAMERA_BRIGHTNESS_LV1:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x22);
		break;
	case CAMERA_BRIGHTNESS_LV2:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x2A);
		break;
	case CAMERA_BRIGHTNESS_LV3:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x3C);
		break;
	case CAMERA_BRIGHTNESS_LV4:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x42);
		break;
	case CAMERA_BRIGHTNESS_LV5:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x4b);
		break;
	case CAMERA_BRIGHTNESS_LV6:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA01F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA020, 0x52);
		break;
	default:
		break;
	}
	return rc;
}

static int mt9v113_brightness_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	rc = mt9v113_set_brightness(value);
	if (rc < 0) {
		CDBG("write faield\n");
		return rc;
	}
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	return rc;
}

static int mt9v113_saturation_enum_map[] = {
	MSM_V4L2_SATURATION_L0,
	MSM_V4L2_SATURATION_L1,
	MSM_V4L2_SATURATION_L2,
	MSM_V4L2_SATURATION_L3,
	MSM_V4L2_SATURATION_L4,
	MSM_V4L2_SATURATION_L5,/* default */
	MSM_V4L2_SATURATION_L6,
	MSM_V4L2_SATURATION_L7,
	MSM_V4L2_SATURATION_L8,
	MSM_V4L2_SATURATION_L9,
	MSM_V4L2_SATURATION_L10,
};

static struct msm_camera_i2c_enum_conf_array mt9v113_saturation_enum_confs = {
	.conf =  NULL,
	.conf_enum = mt9v113_saturation_enum_map,
	.num_enum = ARRAY_SIZE(mt9v113_saturation_enum_map),
	.num_index = 0,
	.num_conf = 0,
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static int mt9v113_set_saturation(int saturation)
{
	int rc = 0;
	CDBG("mt9v113_set_saturation saturation:%d\n", saturation);

	switch (saturation) {
	case MSM_V4L2_SATURATION_L0:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA05F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA05F, 0x01);
		break;
	case MSM_V4L2_SATURATION_L5:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA05F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA05F, 0x2a);
		break;
	case MSM_V4L2_SATURATION_L10:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA05F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA05F, 0xFB);
		break;
	case MSM_V4L2_SATURATION_L1:
	case MSM_V4L2_SATURATION_L2:
	case MSM_V4L2_SATURATION_L3:
	case MSM_V4L2_SATURATION_L4:
	case MSM_V4L2_SATURATION_L6:
	case MSM_V4L2_SATURATION_L7:
	case MSM_V4L2_SATURATION_L8:
	case MSM_V4L2_SATURATION_L9:
		break;
	default:
		break;
	}
	return rc;
}

static int mt9v113_saturation_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	if (effect_value == CAMERA_EFFECT_OFF)
		rc = mt9v113_set_saturation(value);
	pr_debug("--CAMERA-- %s ...(End)\n", __func__);
	return rc;
}

/* Contrast Settings */
static int mt9v113_contrast_enum_map[] = {
	MSM_V4L2_CONTRAST_L0,
	MSM_V4L2_CONTRAST_L1,
	MSM_V4L2_CONTRAST_L2,
	MSM_V4L2_CONTRAST_L3,
	MSM_V4L2_CONTRAST_L4,
	MSM_V4L2_CONTRAST_L5, /* default value */
	MSM_V4L2_CONTRAST_L6,
	MSM_V4L2_CONTRAST_L7,
	MSM_V4L2_CONTRAST_L8,
	MSM_V4L2_CONTRAST_L9,
	MSM_V4L2_CONTRAST_L10,
};

static struct msm_camera_i2c_enum_conf_array mt9v113_contrast_enum_confs = {
	.conf = NULL,
	.conf_enum = mt9v113_contrast_enum_map,
	.num_enum = ARRAY_SIZE(mt9v113_contrast_enum_map),
	.num_index = 0,
	.num_conf = 0,
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static int mt9v113_set_contrast(int contrast)
{
	int rc = 0;

	pr_debug("mt9v113_set_contrast contrast:%d\n", contrast);

	switch (contrast) {
	case MSM_V4L2_CONTRAST_L0:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9C02);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9C02, 0x01);
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA087);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA087, 0x00);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA088, 0x06);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA089, 0x11);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08A, 0x25);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08B, 0x41);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08C, 0x59);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08D, 0x6F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08E, 0x84);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08F, 0x97);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA090, 0xA7);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA091, 0xB5);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA092, 0xC2);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA093, 0xCD);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA094, 0xD7);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA095, 0xE1);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA096, 0xE9);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA097, 0xF1);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA098, 0xF8);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA099, 0xFF);
		break;
	case MSM_V4L2_CONTRAST_L5:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9C02);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9C02, 0x01);
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA087);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA087, 0x00);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA088, 0x07);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA089, 0x16);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08A, 0x30);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08B, 0x52);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08C, 0x6D);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08D, 0x86);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08E, 0x9B);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08F, 0xAB);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA090, 0xB9);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA091, 0xC5);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA092, 0xCF);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA093, 0xD8);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA094, 0xE0);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA095, 0xE7);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA096, 0xEE);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA097, 0xF4);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA098, 0xFA);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA099, 0xFF);
		break;
	case MSM_V4L2_CONTRAST_L10:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9C02);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9C02, 0x01);
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA087);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA087, 0x00);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA088, 0x0F);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA089, 0x2B);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08A, 0x4C);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08B, 0x72);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08C, 0x8D);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08D, 0xA1);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08E, 0xB0);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA08F, 0xBC);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA090, 0xC7);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA091, 0xD0);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA092, 0xD8);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA093, 0xDF);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA094, 0xE6);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA095, 0xEC);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA096, 0xF1);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA097, 0xF6);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA098, 0xFB);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA099, 0xFF);
		break;
	case MSM_V4L2_CONTRAST_L1:
		break;
	case MSM_V4L2_CONTRAST_L2:
		break;
	case MSM_V4L2_CONTRAST_L3:
		break;
	case MSM_V4L2_CONTRAST_L4:
		break;
	case MSM_V4L2_CONTRAST_L6:
		break;
	case MSM_V4L2_CONTRAST_L7:
		break;
	case MSM_V4L2_CONTRAST_L8:
		break;
	case MSM_V4L2_CONTRAST_L9:
		break;
	default:
		break;

	}
	return rc;
}

static int mt9v113_contrast_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	if (effect_value == CAMERA_EFFECT_OFF)
		rc = mt9v113_set_contrast(value);
	pr_debug("--CAMERA-- %s ...(End)\n", __func__);
	return rc;
}

/* Effect Settings */
static struct msm_camera_i2c_reg_conf mt9v113_no_effect[] = {
	{0x098E, 0xA010, 2, 0},
	{0xA010, 0x00, 1, 0},
};

static struct msm_camera_i2c_conf_array mt9v113_no_effect_confs[] = {
	{&mt9v113_no_effect[0], ARRAY_SIZE(mt9v113_no_effect), 30,
			MSM_CAMERA_I2C_WORD_DATA},
};

static int mt9v113_special_effect_enum_map[] = {
	MSM_V4L2_EFFECT_OFF,
	MSM_V4L2_EFFECT_MONO,
	MSM_V4L2_EFFECT_NEGATIVE,
	MSM_V4L2_EFFECT_SOLARIZE,
	MSM_V4L2_EFFECT_SEPIA,
};


static struct msm_camera_i2c_enum_conf_array
		mt9v113_special_effect_enum_confs = {
	.conf = NULL,
	.conf_enum = mt9v113_special_effect_enum_map,
	.num_enum = ARRAY_SIZE(mt9v113_special_effect_enum_map),
	.num_index = 0,
	.num_conf = 0,
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static int mt9v113_set_effect(int effect)
{
	int rc = 0;

	CDBG("mt9v113_set_effect effect:%d\n", effect);
	switch (effect) {
	case MSM_V4L2_EFFECT_OFF:
		/* normal  register settings */
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA010);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA010, 0x00);
		break;
	case MSM_V4L2_EFFECT_MONO:
		/* mono  register settings */
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA010);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA010, 0x01);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA012, 0x1E);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA013, 0xD8);
		break;
	case MSM_V4L2_EFFECT_NEGATIVE:
		/* nagative  register settings */
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA010);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA010, 0x03);
		break;
	case MSM_V4L2_EFFECT_SOLARIZE:
		/* solarize  register settings */
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA010);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA010, 0x01);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA012, 0x10);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA013, 0xD0);
		break;
	case MSM_V4L2_EFFECT_SEPIA:
		/* sepia  register settings */
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0xA010);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA010, 0x02);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA012, 0x1E);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0xA013, 0xD8);
		break;
	default:
		break;
	}
	return rc;
}


static int mt9v113_effect_msm_sensor_s_ctrl_by_enum(
		struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;
	effect_value = value;
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);

	rc = mt9v113_set_effect(effect_value);

	pr_debug("--CAMERA-- %s ...(End)\n", __func__);
	return rc;
}

/* white balance */
static int mt9v113_wb_oem_enum_map[] = {
	MSM_V4L2_WB_AUTO ,
	MSM_V4L2_WB_INCANDESCENT,
	MSM_V4L2_WB_FLUORESCENT,
	MSM_V4L2_WB_DAYLIGHT,
	MSM_V4L2_WB_CLOUDY_DAYLIGHT,
};

static struct msm_camera_i2c_enum_conf_array mt9v113_wb_oem_enum_confs = {
	.conf =  NULL,
	.conf_enum = mt9v113_wb_oem_enum_map,
	.num_enum = ARRAY_SIZE(mt9v113_wb_oem_enum_map),
	.num_index = 0,
	.num_conf = 0,
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

static int mt9v113_set_wb(int wb)
{
	int rc = 0;

	CDBG("mt9v113_set_wb wb: %d\n", wb);
	switch (wb) {
	case MSM_V4L2_WB_AUTO:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9401);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9401, 0x0D);
		break;
	case MSM_V4L2_WB_INCANDESCENT:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9401);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9401, 0x0C);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9436, 0x5E);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9437, 0x33);
		break;
	case MSM_V4L2_WB_DAYLIGHT:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9401);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9401, 0x0C);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9436, 0x42);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9437, 0x56);
		break;
	case MSM_V4L2_WB_CLOUDY_DAYLIGHT:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9401);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9401, 0x0C);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9436, 0x4B);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9437, 0x49);
		break;
	case MSM_V4L2_WB_FLUORESCENT:
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x098E, 0x9401);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9401, 0x0C);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9436, 0x4D);
		mt9v113_i2c_write_byte(mt9v113_client->addr, 0x9437, 0x3C);
		break;

	default:
		break;
	}
	return rc;
}

int mt9v113_wb_sensor_s_ctrl_by_enum(struct msm_sensor_ctrl_t *s_ctrl,
		struct msm_sensor_v4l2_ctrl_info_t *ctrl_info, int value)
{
	int rc = 0;

	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	rc = mt9v113_set_wb(value);
	if (rc < 0) {
		CDBG("write faield\n");
		return rc;
	}
	pr_debug("--CAMERA-- %s IN, value:%d\n", __func__, value);
	return rc;
}

/* total control */
struct msm_sensor_v4l2_ctrl_info_t mt9v113_v4l2_ctrl_info[] = {
	{
		.ctrl_id = V4L2_CID_SATURATION,
		.min = MSM_V4L2_SATURATION_L0,
		.max = MSM_V4L2_SATURATION_L10,
		.step = 1,
		.enum_cfg_settings = &mt9v113_saturation_enum_confs,
		.s_v4l2_ctrl = mt9v113_saturation_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_CONTRAST,
		.min = MSM_V4L2_CONTRAST_L0,
		.max = MSM_V4L2_CONTRAST_L10,
		.step = 1,
		.enum_cfg_settings = &mt9v113_contrast_enum_confs,
		.s_v4l2_ctrl = mt9v113_contrast_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_BRIGHTNESS,
		.min = CAMERA_BRIGHTNESS_LV0,
		.max = CAMERA_BRIGHTNESS_LV6,
		.step = 1,
		.enum_cfg_settings = &mt9v113_brightness_enum_confs,
		.s_v4l2_ctrl = mt9v113_brightness_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_SPECIAL_EFFECT,
		.min = MSM_V4L2_EFFECT_OFF,
		.max = MSM_V4L2_EFFECT_NEGATIVE,
		.step = 1,
		.enum_cfg_settings = &mt9v113_special_effect_enum_confs,
		.s_v4l2_ctrl = mt9v113_effect_msm_sensor_s_ctrl_by_enum,
	},
	{
		.ctrl_id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.min = MSM_V4L2_WB_OFF,
		.max = MSM_V4L2_WB_CLOUDY_DAYLIGHT,
		.step = 1,
		.enum_cfg_settings = &mt9v113_wb_oem_enum_confs,
		.s_v4l2_ctrl = mt9v113_wb_sensor_s_ctrl_by_enum,
	},
};

static struct msm_sensor_output_reg_addr_t mt9v113_reg_addr = {
	.x_output = 0x2703,
	.y_output = 0x2705,
	.line_length_pclk = 0x2713,
	.frame_length_lines = 0x2711,
};

static struct msm_sensor_id_info_t mt9v113_id_info = {
	.sensor_id_reg_addr = 0x000,
	.sensor_id = 0x2284,
};

static struct msm_sensor_exp_gain_info_t mt9v113_exp_gain_info = {
	.coarse_int_time_addr = 0x004f,
	.global_gain_addr = 0x0012,
	.vert_offset = 4,
};

static const struct i2c_device_id mt9v113_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&mt9v113_s_ctrl},
	{ }
};

static int mt9v113_i2c_txdata(u16 saddr, u8 *txdata, int length)
{
	struct i2c_msg msg[] = {
		{
			.addr  = (saddr >> 1),
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};
	if (i2c_transfer(mt9v113_client->adapter, msg, 1) < 0)
		return -EIO;
	else
		return 0;
}

static int mt9v113_i2c_write_byte(unsigned short saddr,
			unsigned int waddr, unsigned int wdata)
{
	int rc = -EIO;
	mt9v113_counter = 0;
	mt9v113_i2c_buf[0] = (waddr & 0xFF00) >> 8;
	mt9v113_i2c_buf[1] = (waddr & 0x00FF);
	mt9v113_i2c_buf[2] = (wdata & 0x00FF);
	while ((mt9v113_counter < MT9V113_I2C_MAX_TIMES) && (rc != 0)) {
		rc = mt9v113_i2c_txdata(saddr, mt9v113_i2c_buf, 3);
		if (rc < 0) {
			mt9v113_counter++;
			CDBG("mt9v113_i2c_txdata failed,Reg(0x%04x) = 0x%02x, "
				"rc=%d!\n", waddr, wdata, rc);
		}
	}
	return rc;
}

static int32_t mt9v113_sensor_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int32_t rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;

	CDBG("##############%s IN###########\r\n", __func__);

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	s_ctrl->sensor_i2c_addr = s_ctrl->sensor_i2c_addr;

	rc = msm_sensor_i2c_probe(client, id);

	if (client->dev.platform_data == NULL) {
		CDBG_HIGH("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}

	mt9v113_client = client;
	usleep_range(5000, 5100);

	return rc;
}

static struct i2c_driver mt9v113_i2c_driver = {
	.id_table = mt9v113_i2c_id,
	.probe  = mt9v113_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client mt9v113_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int mt9v113_i2c_write_word(unsigned short saddr,
			unsigned int waddr, unsigned int wdata)
{
	int rc = -EIO;

	mt9v113_counter = 0;
	mt9v113_i2c_buf[0] = (waddr & 0xFF00)>>8;
	mt9v113_i2c_buf[1] = (waddr & 0x00FF);
	mt9v113_i2c_buf[2] = (wdata & 0xFF00)>>8;
	mt9v113_i2c_buf[3] = (wdata & 0x00FF);
	while ((mt9v113_counter < MT9V113_I2C_MAX_TIMES) && (rc != 0)) {
		rc = mt9v113_i2c_txdata(saddr, mt9v113_i2c_buf, 4);
		if (rc < 0) {
			mt9v113_counter++;
			CDBG("mt9v113_i2c_txdata failed, Reg(0x%04x) = 0x%04x, "
				"rc=%d!\n", waddr, wdata, rc);
			}
		}
	return rc;
}

static int __init msm_sensor_init_module(void)
{
	int rc = 0;
	CDBG("MT9V113\n");

	rc = i2c_add_driver(&mt9v113_i2c_driver);

	return rc;
}

static struct v4l2_subdev_core_ops mt9v113_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops mt9v113_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops mt9v113_subdev_ops = {
	.core = &mt9v113_subdev_core_ops,
	.video  = &mt9v113_subdev_video_ops,
};

static int32_t MT9V113_WriteRegsTbl(PMT9V113_WREG pTb, int32_t len)
{
	int32_t i, ret = 0;

	for (i = 0; i < len; i++) {
		if (0 == pTb[i].data_format) {
			/*Reg Data Format:16Bit = 0*/
			if (0 == pTb[i].delay_ms) {
				mt9v113_i2c_write_word(mt9v113_client->addr,
						pTb[i].addr, pTb[i].data);
			} else {
				mt9v113_i2c_write_word(mt9v113_client->addr,
						pTb[i].addr, pTb[i].data);
				msleep(pTb[i].delay_ms);
			}
		} else {
			/*Reg Data Format:8Bit = 1*/
			if (0 == pTb[i].delay_ms) {
				mt9v113_i2c_write_byte(mt9v113_client->addr,
						pTb[i].addr, pTb[i].data);
			} else {
				mt9v113_i2c_write_byte(mt9v113_client->addr,
						pTb[i].addr, pTb[i].data);
				msleep(pTb[i].delay_ms);
			}
		}
	}
	return ret;
}

static int32_t mt9v113_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	CDBG("mt9v113_write_exp_gain : Not supported\n");
	return 0;
}

static void mt9v113_stream_switch(int bOn)
{
	if (bOn == 0) {
		CDBG("mt9v113_stream_switch: off\n");
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x3C40, 0x7836);
	} else {
		CDBG("mt9v113_stream_switch: on\n");
		mt9v113_i2c_write_word(mt9v113_client->addr, 0x3C40, 0x783C);
	}
}

static void mt9v113_sensor_start_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	CDBG("mt9v113_sensor_set_fps: Not supported\n");
	mt9v113_stream_switch(1);
}

static void mt9v113_sensor_stop_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	CDBG("mt9v113_sensor_set_fps: Not supported\n");
	mt9v113_stream_switch(0);
}

static int32_t mt9v113_sensor_set_fps(struct msm_sensor_ctrl_t *s_ctrl,
		struct fps_cfg *fps)
{
	CDBG("mt9v113_sensor_set_fps: Not supported\n");
	return 0;
}

extern int pwd_front;
static int32_t mt9v113_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;

	CDBG("%s: %d\n", __func__, __LINE__);
	info = s_ctrl->sensordata;

	CDBG("%s, sensor_pwd:%d, sensor_reset:%d\r\n",
		__func__, info->sensor_pwd, info->sensor_reset);

	gpio_direction_output(pwd_front, 1);
	rc = msm_sensor_power_up(s_ctrl);
	if (rc < 0) {
		CDBG("%s: msm_sensor_power_up failed\n", __func__);
		return rc;
	}

	usleep_range(1000, 1100);
	msleep(100);
	gpio_direction_output(pwd_front, 0);
	msleep(50);

	return rc;
}

int32_t mt9v113_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;

	CDBG("%s IN\r\n", __func__);
	info = s_ctrl->sensordata;

	msm_sensor_stop_stream(s_ctrl);
	msleep(40);
	gpio_direction_output(pwd_front, 1);
	usleep_range(5000, 5100);
	msleep(100);
	rc = msm_sensor_power_down(s_ctrl);
	msleep(50);
	return rc;
}

static int mt9v113_i2c_write_word_first_reset(unsigned short saddr,
			unsigned int waddr, unsigned int wdata)
{
	int rc = -EIO;

	mt9v113_counter = 0;
	mt9v113_i2c_buf[0] = (waddr & 0xFF00)>>8;
	mt9v113_i2c_buf[1] = (waddr & 0x00FF);
	mt9v113_i2c_buf[2] = (wdata & 0xFF00)>>8;
	mt9v113_i2c_buf[3] = (wdata & 0x00FF);
	rc = mt9v113_i2c_txdata(saddr, mt9v113_i2c_buf, 4);
	CDBG("%s:Reg(0x%04x) = 0x%04x, rc=%d!\n", __func__, waddr, wdata, rc);
	return rc;
}

static int32_t mt9v113_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;

	if (update_type == MSM_SENSOR_REG_INIT) {
		CDBG("###########Register INIT##############\n");
		CDBG("--CAMERA-- set sensor init setting\n");

		mt9v113_i2c_write_word_first_reset(mt9v113_client->addr,
					0x001A, 0x0566);
		msleep(50);
		mt9v113_i2c_write_word_first_reset(mt9v113_client->addr,
					0x001A, 0x0126);
		msleep(50);

		rc = MT9V113_WriteSettingTbl(mt9v113_init_tbl);

		if (rc < 0) {
			CDBG("sensor init setting failed\n");
			return rc;
		}
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		CDBG("##############PERIODIC : #####pengyong#######%d\n", res);

		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
	}
	return rc;
}

static struct msm_sensor_fn_t mt9v113_func_tbl = {
	.sensor_start_stream = mt9v113_sensor_start_stream,
	.sensor_stop_stream = mt9v113_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = mt9v113_sensor_set_fps,
	.sensor_setting = mt9v113_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = mt9v113_sensor_power_up,
	.sensor_power_down = mt9v113_sensor_power_down,
	.sensor_write_exp_gain = mt9v113_write_exp_gain,
	.sensor_write_snapshot_exp_gain = mt9v113_write_exp_gain,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t mt9v113_regs = {
	.default_data_type = MSM_CAMERA_I2C_WORD_DATA,
	.start_stream_conf = mt9v113_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(mt9v113_start_settings),
	.stop_stream_conf = mt9v113_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(mt9v113_stop_settings),
	.group_hold_on_conf = NULL,
	.group_hold_on_conf_size = 0,
	.group_hold_off_conf = NULL,
	.group_hold_off_conf_size = 0,

	.init_settings = NULL,
	.init_size = 0,
	.mode_settings = &mt9v113_confs[0],
	.no_effect_settings = &mt9v113_no_effect_confs[0],
	.output_settings = &mt9v113_dimensions[0],
	.num_conf = ARRAY_SIZE(mt9v113_confs),
};

static struct msm_sensor_ctrl_t mt9v113_s_ctrl = {
	.msm_sensor_reg = &mt9v113_regs,
	.msm_sensor_v4l2_ctrl_info = mt9v113_v4l2_ctrl_info,
	.num_v4l2_ctrl = ARRAY_SIZE(mt9v113_v4l2_ctrl_info),
	.sensor_i2c_client = &mt9v113_sensor_i2c_client,
	.sensor_i2c_addr = 0x7a,
	.sensor_output_reg_addr = &mt9v113_reg_addr,
	.sensor_id_info = &mt9v113_id_info,
	.sensor_exp_gain_info = &mt9v113_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	/* .csi_params = &mt9v113_csi_params_array[0], */
	.msm_sensor_mutex = &mt9v113_mut,
	.sensor_i2c_driver = &mt9v113_i2c_driver,
	.sensor_v4l2_subdev_info = mt9v113_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(mt9v113_subdev_info),
	.sensor_v4l2_subdev_ops = &mt9v113_subdev_ops,
	.func_tbl = &mt9v113_func_tbl,

};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("APTINA VGA YUV sensor driver");
MODULE_LICENSE("GPL v2");
