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

#ifndef MIPI_ORISE9806_YASHI_450_H
#define MIPI_ORISE9806_YASHI_450_H

#include "mipi_dsi.h"

#define ORISE9806_YASHI_450_HBP 48
#define ORISE9806_YASHI_450_HFP 40
#define ORISE9806_YASHI_450_HPW 8
#define ORISE9806_YASHI_450_VBP 16
#define ORISE9806_YASHI_450_VFP 15
#define ORISE9806_YASHI_450_VPW 1


#define ORISE9806_YASHI_450_HFPPS 1
#define ORISE9806_YASHI_450_HBPPS 1
#define ORISE9806_YASHI_450_HSAPS 1

#define ORISE9806_YASHI_450_TCLK_POST 0x19
#define ORISE9806_YASHI_450_TCLK_PRE 0x2F

#define LCD_ORISE9806_YASHI_450_FRAME_RATE 60

#define ORISE9806_YASHI_450_XRES    540
#define ORISE9806_YASHI_450_YRES    960

#ifdef VIDEO_WVGA_PT_PARA
static struct mipi_dsi_phy_ctrl orise9806_yashi_450_dsi_video_mode_phy_db = {
	/* DSI Bit Clock at 500 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x02, 0x0a, 0x04, 0x00, 0x20},
	/* timing   */
	{0x8C, 0x48, 0x16, 0x00, 0x57, 0x5C, 0x1A, 0x4B,
	0x20, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x00, 0xC5, 0x31, 0xDA, 0x00, 0x40, 0x03, 0x62,
	0x01, 0x0f, 0x07,
	0x00, 0x1a, 0x00, 0x0, 0x02, 0x0, 0x20, 0x0, 0x01, 0x0},
};

static char orise9608_yashi_450_video0[2] = {0x00, 0x00};
static char orise9608_yashi_450_video1[4] = {0xff, 0x96, 0x08, 0x01};
static char orise9608_yashi_450_video2[2] = {0x00, 0x80};
static char orise9608_yashi_450_video3[3] = {0xff, 0x96, 0x08};
static char orise9608_yashi_450_video4[2] = {0x00, 0x00};
static char orise9608_yashi_450_video5[2] = {0xa0, 0x00};
static char orise9608_yashi_450_video6[2] = {0x00, 0x80};
static char orise9608_yashi_450_video7[6] = {
	0xb3, 0x00, 0x00, 0x20, 0x00, 0x00
};
static char orise9608_yashi_450_video8[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video9[2] = {0xb3, 0x09};
static char orise9608_yashi_450_video10[2] = {0x00, 0x80};
static char orise9608_yashi_450_video11[10] = {
	0xc0, 0x00, 0x48, 0x00, 0x10, 0x10, 0x00, 0x47, 0x10,
	0x10
};
static char orise9608_yashi_450_video12[2] = {0x00, 0x92};
static char orise9608_yashi_450_video13[5] = {0xc0, 0x00, 0x10, 0x00, 0x13};
static char orise9608_yashi_450_video14[2] = {0x00, 0xa2};
static char orise9608_yashi_450_video15[4] = {0xc0, 0x0c, 0x05, 0x02};
static char orise9608_yashi_450_video16[2] = {0x00, 0xb3};
static char orise9608_yashi_450_video17[3] = {0xc0, 0x00, 0x50};
static char orise9608_yashi_450_video18[2] = {0x00, 0x81};
static char orise9608_yashi_450_video19[2] = {0xc1, 0x77};
static char orise9608_yashi_450_video20[2] = {0x00, 0x80};
static char orise9608_yashi_450_video21[4] = {0xc4, 0x00, 0x84, 0xfc};
static char orise9608_yashi_450_video22[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video23[3] = {0xb3, 0x10, 0x00};
static char orise9608_yashi_450_video24[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video25[2] = {0xc0, 0x00};
static char orise9608_yashi_450_video26[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video27[9] = {
	0xc4, 0x33, 0x09, 0x90, 0x2b, 0x33, 0x09, 0x90,
	0x54
};
static char orise9608_yashi_450_video28[2] = {0x00, 0x80};
static char orise9608_yashi_450_video29[5] = {0xc5, 0x08, 0x00, 0xa0, 0x11};
static char orise9608_yashi_450_video30[2] = {0x00, 0x90};
static char orise9608_yashi_450_video31[8] = {
	0xc5, 0x96, 0x57, 0x00, 0x57, 0x33, 0x33, 0x34
};
static char orise9608_yashi_450_video32[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video33[8] = {
	0xc5, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x34
};
static char orise9608_yashi_450_video34[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video35[8] = {
	0xc5, 0x04, 0xac, 0x01, 0x00, 0x71, 0xb1, 0x83
};
static char orise9608_yashi_450_video36[2] = {0x00, 0x00};
static char orise9608_yashi_450_video37[2] = {0xd9, 0x65};
static char orise9608_yashi_450_video38[2] = {0x00, 0x80};
static char orise9608_yashi_450_video39[2] = {0xc6, 0x64};
static char orise9608_yashi_450_video40[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video41[6] = {
	0xc6, 0x03, 0x10, 0x00, 0x1f, 0x12
};
static char orise9608_yashi_450_video42[2] = {0x00, 0xe1};
static char orise9608_yashi_450_video43[2] = {0xc0, 0x9f};
static char orise9608_yashi_450_video44[2] = {0x00, 0xb7};
static char orise9608_yashi_450_video45[2] = {0xb0, 0x10};
static char orise9608_yashi_450_video46[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video47[2] = {0xb0, 0x55};
static char orise9608_yashi_450_video48[2] = {0x00, 0xb1};
static char orise9608_yashi_450_video49[2] = {0xb0, 0x03};
static char orise9608_yashi_450_video50[2] = {0x00, 0x81};
static char orise9608_yashi_450_video51[2] = {0xd6, 0x00};
static char orise9608_yashi_450_video52[2] = {0x00, 0xb4};
static char orise9608_yashi_450_video53[2] = {0xc0, 0x50};
static char orise9608_yashi_450_video54[2] = {0x00, 0x00};
static char orise9608_yashi_450_video55[17] = {
	0xe1, 0x01, 0x0d, 0x0D, 0x0f, 0x05, 0x0A, 0x09,
	0x07, 0x06, 0x08, 0x13, 0x08, 0x0d, 0x0e, 0x09,
	0x01
};
static char orise9608_yashi_450_video56[2] = {0x00, 0x00};
static char orise9608_yashi_450_video57[17] = {
	0xe2, 0x00, 0x0d, 0x0D, 0x0f, 0x05, 0x0A, 0x09,
	0x08, 0x06, 0x08, 0x13, 0x08, 0x0d, 0x0e, 0x09,
	0x01
};
static char orise9608_yashi_450_video58[2] = {0x00, 0x80};
static char orise9608_yashi_450_video59[11] = {
	0xcb, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video60[2] = {0x00, 0x90};
static char orise9608_yashi_450_video61[16] = {
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video62[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video63[16] = {
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video64[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video65[11] = {
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video66[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video67[16] = {
	0xcb, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x00, 0x00
};
static char orise9608_yashi_450_video68[2] = {0x00, 0xd0};
static char orise9608_yashi_450_video69[16] = {
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04
};
static char orise9608_yashi_450_video70[2] = {0x00, 0xe0};
static char orise9608_yashi_450_video71[11] = {
	0xcb, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video72[2] = {0x00, 0xf0};
static char orise9608_yashi_450_video73[11] = {
	0xcb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff
};
static char orise9608_yashi_450_video74[2] = {0x00, 0x80};
static char orise9608_yashi_450_video75[11] = {
	0xcc, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x0a,
	0x0e, 0x00, 0x00
};
static char orise9608_yashi_450_video76[2] = {0x00, 0x90};
static char orise9608_yashi_450_video77[16] = {
	0xcc, 0x0c, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09
};
static char orise9608_yashi_450_video78[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video79[16] = {
	0xcc, 0x0d, 0x00, 0x00, 0x0b, 0x0f, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video80[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video81[11] = {
	0xcc, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x0a,
	0x0e, 0x00, 0x00
};
static char orise9608_yashi_450_video82[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video83[16] = {
	0xcc, 0x0c, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video84[2] = {0x00, 0xd0};
static char orise9608_yashi_450_video85[16] = {
	0xcc, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0b,
	0x00, 0x00, 0x0d, 0x09, 0x01, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video86[2] = {0x00, 0x80};
static char orise9608_yashi_450_video87[13] = {
	0xce, 0x84, 0x03, 0x18, 0x83, 0x03, 0x18, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video88[2] = {0x00, 0x90};
static char orise9608_yashi_450_video89[15] = {
	0xce, 0x33, 0xbf, 0x18, 0x33, 0xc0, 0x18, 0x10,
	0x0f, 0x18, 0x10, 0x10, 0x18, 0x00, 0x00
};
static char orise9608_yashi_450_video90[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video91[15] = {
	0xce, 0x38, 0x02, 0x03, 0xc1, 0x00, 0x18, 0x00,
	0x38, 0x01, 0x03, 0xc2, 0x00, 0x18, 0x00
};
static char orise9608_yashi_450_video92[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video93[15] = {
	0xce, 0x38, 0x00, 0x03, 0xc3, 0x00, 0x18, 0x00,
	0x30, 0x00, 0x03, 0xc4, 0x00, 0x18, 0x00
};
static char orise9608_yashi_450_video94[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video95[15] = {
	0xce, 0x30, 0x01, 0x03, 0xc5, 0x00, 0x18, 0x00,
	0x30, 0x02, 0x03, 0xc6, 0x00, 0x18, 0x00
};
static char orise9608_yashi_450_video96[2] = {0x00, 0xd0};
static char orise9608_yashi_450_video97[15] = {
	0xce, 0x30, 0x03, 0x03, 0xc7, 0x00, 0x18, 0x00,
	0x30, 0x04, 0x03, 0xc8, 0x00, 0x18, 0x00
};
static char orise9608_yashi_450_video98[2] = {0x00, 0x80};
static char orise9608_yashi_450_video99[15] = {
	0xcf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video100[2] = {0x00, 0x90};
static char orise9608_yashi_450_video101[15] = {
	0xcf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video102[2] = {0x00, 0xa0};
static char orise9608_yashi_450_video103[15] = {
	0xcf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video104[2] = {0x00, 0xb0};
static char orise9608_yashi_450_video105[15] = {
	0xcf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9608_yashi_450_video106[2] = {0x00, 0xc0};
static char orise9608_yashi_450_video107[11] = {
	0xcf, 0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x02,
	0x04, 0x00, 0x00
};
static char orise9608_yashi_450_video108[2] = {0x00, 0x00};
static char orise9608_yashi_450_video109[3] = {0xd8, 0x6F, 0x6F};
static char orise9608_yashi_450_video110[2] = {0x51, 0x00 };
static char orise9608_yashi_450_video111[2] = {0x53, 0x24 };
static char orise9608_yashi_450_video112[2] = {0x55, 0x00};
static char orise9608_yashi_450_video113[2] = {0x00, 0xc7};
static char orise9608_yashi_450_video114[2] = {0xcf, 0x00};
static char orise9608_yashi_450_video115[2] = {0x00, 0x00};
static char orise9608_yashi_450_video116[4] = {0xff, 0xff, 0xff, 0xff};
static char orise9608_yashi_450_video117[2] = {0x00, 0x00};
static char orise9608_yashi_450_video118[2] = {0x3A, 0x77};
static char orise9608_yashi_450_video119[2] = {0x11, 0x00};
static char orise9608_yashi_450_video120[2] = {0x29, 0x00};
static char orise9608_yashi_450_video121[2] = {0x2C, 0x00};
static char orise9608_yashi_450_video122[2] = {0x00, 0xb1};
static char orise9608_yashi_450_video123[2] = {0xc6, 0x03};

static struct dsi_cmd_desc orise9806_yashi_450_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video0), orise9608_yashi_450_video0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video1), orise9608_yashi_450_video1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video2), orise9608_yashi_450_video2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video3), orise9608_yashi_450_video3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video4), orise9608_yashi_450_video4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video5), orise9608_yashi_450_video5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video6), orise9608_yashi_450_video6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video7), orise9608_yashi_450_video7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video8), orise9608_yashi_450_video8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video9), orise9608_yashi_450_video9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video10), orise9608_yashi_450_video10},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video11), orise9608_yashi_450_video11},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video12), orise9608_yashi_450_video12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video13), orise9608_yashi_450_video13},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video14), orise9608_yashi_450_video14},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video15), orise9608_yashi_450_video15},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video16), orise9608_yashi_450_video16},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video17), orise9608_yashi_450_video17},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video18), orise9608_yashi_450_video18},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video19), orise9608_yashi_450_video19},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video20), orise9608_yashi_450_video20},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video21), orise9608_yashi_450_video21},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video22), orise9608_yashi_450_video22},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video23), orise9608_yashi_450_video23},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video24), orise9608_yashi_450_video24},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video25), orise9608_yashi_450_video25},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video26), orise9608_yashi_450_video26},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video27), orise9608_yashi_450_video27},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video28), orise9608_yashi_450_video28},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video29), orise9608_yashi_450_video29},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video30), orise9608_yashi_450_video30},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video31), orise9608_yashi_450_video31},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video32), orise9608_yashi_450_video32},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video33), orise9608_yashi_450_video33},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video34), orise9608_yashi_450_video34},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video35), orise9608_yashi_450_video35},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video36), orise9608_yashi_450_video36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video37), orise9608_yashi_450_video37},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video38), orise9608_yashi_450_video38},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video39), orise9608_yashi_450_video39},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video40), orise9608_yashi_450_video40},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video41), orise9608_yashi_450_video41},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video122), orise9608_yashi_450_video122},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video123), orise9608_yashi_450_video123},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video42), orise9608_yashi_450_video42},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video43), orise9608_yashi_450_video43},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video44), orise9608_yashi_450_video44},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video45), orise9608_yashi_450_video45},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video46), orise9608_yashi_450_video46},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video47), orise9608_yashi_450_video47},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video48), orise9608_yashi_450_video48},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video49), orise9608_yashi_450_video49},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video50), orise9608_yashi_450_video50},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video51), orise9608_yashi_450_video51},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video52), orise9608_yashi_450_video52},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video53), orise9608_yashi_450_video53},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video54), orise9608_yashi_450_video54},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video55), orise9608_yashi_450_video55},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video56), orise9608_yashi_450_video56},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video57), orise9608_yashi_450_video57},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video58), orise9608_yashi_450_video58},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video59), orise9608_yashi_450_video59},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video60), orise9608_yashi_450_video60},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video61), orise9608_yashi_450_video61},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video62), orise9608_yashi_450_video62},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video63), orise9608_yashi_450_video63},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video64), orise9608_yashi_450_video64},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video65), orise9608_yashi_450_video65},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video66), orise9608_yashi_450_video66},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video67), orise9608_yashi_450_video67},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video68), orise9608_yashi_450_video68},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video69), orise9608_yashi_450_video69},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video70), orise9608_yashi_450_video70},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video71), orise9608_yashi_450_video71},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video72), orise9608_yashi_450_video72},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video73), orise9608_yashi_450_video73},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video74), orise9608_yashi_450_video74},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video75), orise9608_yashi_450_video75},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video76), orise9608_yashi_450_video76},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video77), orise9608_yashi_450_video77},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video78), orise9608_yashi_450_video78},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video79), orise9608_yashi_450_video79},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video80), orise9608_yashi_450_video80},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video81), orise9608_yashi_450_video81},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video82), orise9608_yashi_450_video82},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video83), orise9608_yashi_450_video83},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video84), orise9608_yashi_450_video84},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video85), orise9608_yashi_450_video85},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video86), orise9608_yashi_450_video86},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video87), orise9608_yashi_450_video87},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video88), orise9608_yashi_450_video88},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video89), orise9608_yashi_450_video89},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video90), orise9608_yashi_450_video90},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video91), orise9608_yashi_450_video91},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video92), orise9608_yashi_450_video92},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video93), orise9608_yashi_450_video93},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video94), orise9608_yashi_450_video94},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video95), orise9608_yashi_450_video95},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video96), orise9608_yashi_450_video96},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video97), orise9608_yashi_450_video97},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video98), orise9608_yashi_450_video98},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video99), orise9608_yashi_450_video99},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video100), orise9608_yashi_450_video100},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video101), orise9608_yashi_450_video101},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video102), orise9608_yashi_450_video102},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video103), orise9608_yashi_450_video103},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video104), orise9608_yashi_450_video104},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video105), orise9608_yashi_450_video105},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video106), orise9608_yashi_450_video106},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video107), orise9608_yashi_450_video107},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video108), orise9608_yashi_450_video108},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video109), orise9608_yashi_450_video109},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video110), orise9608_yashi_450_video110},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video111), orise9608_yashi_450_video111},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video112), orise9608_yashi_450_video112},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video113), orise9608_yashi_450_video113},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video114), orise9608_yashi_450_video114},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video115), orise9608_yashi_450_video115},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video116), orise9608_yashi_450_video116},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video117), orise9608_yashi_450_video117},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(orise9608_yashi_450_video118), orise9608_yashi_450_video118},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(orise9608_yashi_450_video119), orise9608_yashi_450_video119},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20,
		sizeof(orise9608_yashi_450_video120), orise9608_yashi_450_video120},
	{DTYPE_DCS_WRITE, 1, 0, 0, 1,
		sizeof(orise9608_yashi_450_video121), orise9608_yashi_450_video121},
};

static char orise9806_yashi_450_sleepin[2] = {0x28, 0x00};
static char orise9806_yashi_450_disp_off[2] = {0x10, 0x00};

static struct dsi_cmd_desc orise9806_yashi_450_video_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150,
		sizeof(orise9806_yashi_450_sleepin), orise9806_yashi_450_sleepin},
	{DTYPE_DCS_WRITE, 1, 0, 0, 200,
		sizeof(orise9806_yashi_450_disp_off), orise9806_yashi_450_disp_off},
};
#endif
#endif
