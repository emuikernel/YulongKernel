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

#ifndef MIPI_ORISE9806_BOE_450_H
#define MIPI_ORISE9806_BOE_450_H

#include "mipi_dsi.h"

#define ORISE9806_BOE_450_HBP 100
#define ORISE9806_BOE_450_HFP 100
#define ORISE9806_BOE_450_HPW 8
#define ORISE9806_BOE_450_VBP 20
#define ORISE9806_BOE_450_VFP 20
#define ORISE9806_BOE_450_VPW 1

#define ORISE9806_BOE_450_HFPPS 1
#define ORISE9806_BOE_450_HBPPS 1
#define ORISE9806_BOE_450_HSAPS 1

#define ORISE9806_BOE_450_TCLK_POST 0x20
#define ORISE9806_BOE_450_TCLK_PRE 0x2f

#define LCD_ORISE9806_BOE_450_FRAME_RATE 60

#define ORISE9806_BOE_450_XRES    540
#define ORISE9806_BOE_450_YRES    960

#ifdef VIDEO_WVGA_PT_PARA

static struct mipi_dsi_phy_ctrl orise9806_boe_450_dsi_video_mode_phy_db = {
	/* DSI Bit Clock at 500 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x02, 0x0a, 0x04, 0x00, 0x20},
	/* timing   */
	{0xbd, 0x8f, 0x23, 0x00, 0x26, 0x97, 0x25, 0x91,
	0x26, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x00, 0xec, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};


static char orise9806_boe_450_video0[2] = {0x00, 0x00};
static char orise9806_boe_450_video1[4] = {0xFF, 0x96, 0x08, 0x01};
static char orise9806_boe_450_video2[2] = {0x00, 0x80};
static char orise9806_boe_450_video3[3] = {0xFF, 0x96, 0x08};
static char orise9806_boe_450_video4[2] = {0x00, 0x00};
static char orise9806_boe_450_video5[2] = {0xA0, 0x00};
static char orise9806_boe_450_video6[2] = {0x00, 0x80};
static char orise9806_boe_450_video7[6] = {0xB3, 0x00, 0x00, 0x00, 0x21, 0x00};
static char orise9806_boe_450_video8[2] = {0x00, 0x92};
static char orise9806_boe_450_video9[2] = {0xB3, 0x01};
static char orise9806_boe_450_video10[2] = {0x00, 0xC0};
static char orise9806_boe_450_video11[2] = {0xB3, 0x11};
static char orise9806_boe_450_video12[2] = {0x00, 0x80};
static char orise9806_boe_450_video13[10] = {
	0xC0, 0x00, 0x48, 0x00, 0x06, 0x06, 0x00, 0x48,
	0x10, 0x10
};
static char orise9806_boe_450_video14[2] = {0x00, 0x92};
static char orise9806_boe_450_video15[5] = {0xC0, 0x00, 0x17, 0x00, 0x1A};
static char orise9806_boe_450_video16[2] = {0x00, 0xA2};
static char orise9806_boe_450_video17[4] = {0xC0, 0x01, 0x10, 0x00};
static char orise9806_boe_450_video18[2] = {0x00, 0xB3};
static char orise9806_boe_450_video19[3] = {0xC0, 0x00, 0x50};
static char orise9806_boe_450_video20[2] = {0x00, 0x81};
static char orise9806_boe_450_video21[2] = {0xC1, 0x55};
static char orise9806_boe_450_video22[2] = {0x00, 0x80};
static char orise9806_boe_450_video23[4] = {0xC4, 0x00, 0x84, 0xFA};
static char orise9806_boe_450_video24[2] = {0x00, 0xA0};
static char orise9806_boe_450_video25[9] = {
	0xC4, 0x33, 0x09, 0x90, 0x2B, 0x33, 0x09, 0x90,
	0x54
};
static char orise9806_boe_450_video26[2] = {0x00, 0x80};
static char orise9806_boe_450_video27[5] = {0xC5, 0x08, 0x00, 0x90, 0x11};
static char orise9806_boe_450_video28[2] = {0x00, 0x90};
static char orise9806_boe_450_video29[8] = {
	0xC5, 0x96, 0x76, 0x06, 0x76, 0x33, 0x33, 0x34
};
static char orise9806_boe_450_video30[2] = {0x00, 0xA0};
static char orise9806_boe_450_video31[8] = {
	0xC5, 0x96, 0x77, 0x00, 0x72, 0x33, 0x33, 0x34
};
static char orise9806_boe_450_video32[2] = {0x00, 0xB0};
static char orise9806_boe_450_video33[3] = {0xC5, 0x04, 0xF8};
static char orise9806_boe_450_video34[2] = {0x00, 0x80};
static char orise9806_boe_450_video35[2] = {0xC6, 0x64};
static char orise9806_boe_450_video36[2] = {0x00, 0xB0};
static char orise9806_boe_450_video37[6] = {0xC6, 0x03, 0x10, 0x00, 0x1F, 0x12};
static char orise9806_boe_450_video38[2] = {0x00, 0xE1};
static char orise9806_boe_450_video39[2] = {0xC0, 0x9F};
static char orise9806_boe_450_video40[2] = {0x00, 0x00};
static char orise9806_boe_450_video41[2] = {0xD0, 0x01};
static char orise9806_boe_450_video42[2] = {0x00, 0x00};
static char orise9806_boe_450_video43[3] = {0xD1, 0x01, 0x01};
static char orise9806_boe_450_video44[2] = {0x00, 0xB7,};
static char orise9806_boe_450_video45[2] = {0xB0, 0x10,};
static char orise9806_boe_450_video46[2] = {0x00, 0xC0,};
static char orise9806_boe_450_video47[2] = {0xB0, 0x55,};
static char orise9806_boe_450_video48[2] = {0x00, 0xB1,};
static char orise9806_boe_450_video49[3] = {0xB0, 0x03, 0x06};
static char orise9806_boe_450_video50[2] = {0x00, 0x80,};
static char orise9806_boe_450_video51[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9806_boe_450_video52[2] = {0x00, 0x90,};
static char orise9806_boe_450_video53[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video54[2] = {0x00, 0xA0,};
static char orise9806_boe_450_video55[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video56[2] = {0x00, 0xB0,};
static char orise9806_boe_450_video57[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9806_boe_450_video58[2] = {0x00, 0xC0,};
static char orise9806_boe_450_video59[16] = {
	0xCB, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video60[2] = {0x00, 0xD0,};
static char orise9806_boe_450_video61[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video62[2] = {0x00, 0xE0,};
static char orise9806_boe_450_video63[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9806_boe_450_video64[2] = {0x00, 0xF0,};
static char orise9806_boe_450_video65[11] = {
	0xCB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF
};
static char orise9806_boe_450_video66[2] = {0x00, 0x80,};
static char orise9806_boe_450_video67[11] = {
	0xCC, 0x00, 0x00, 0x0B, 0x09, 0x01, 0x25, 0x26,
	0x00, 0x00, 0x00
};
static char orise9806_boe_450_video68[2] = {0x00, 0x90,};
static char orise9806_boe_450_video69[16] = {
	0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0A, 0x02
};
static char orise9806_boe_450_video70[2] = {0x00, 0xA0,};
static char orise9806_boe_450_video71[16] = {
	0xCC, 0x25, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video72[2] = {0x00, 0xB0,};
static char orise9806_boe_450_video73[11] = {
	0xCC, 0x00, 0x00, 0x0A, 0x0C, 0x02, 0x26, 0x25,
	0x00, 0x00, 0x00
};
static char orise9806_boe_450_video74[2] = {0x00, 0xC0,};
static char orise9806_boe_450_video75[16] = {
	0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0B, 0x01
};
static char orise9806_boe_450_video76[2] = {0x00, 0xD0,};
static char orise9806_boe_450_video77[16] = {
	0xCC, 0x26, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video78[2] = {0x00, 0x80,};
static char orise9806_boe_450_video79[13] = {
	0xCE, 0x86, 0x01, 0x00, 0x85, 0x01, 0x00, 0x02,
	0x00, 0x00, 0x0F, 0x00, 0x00
};
static char orise9806_boe_450_video80[2] = {0x00, 0x90,};
static char orise9806_boe_450_video81[15] = {
	0xCE, 0xF0, 0x00, 0x00, 0xF0, 0x00, 0x00, 0xF0,
	0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video82[2] = {0x00, 0xA0,};
static char orise9806_boe_450_video83[15] = {
	0xCE, 0x18, 0x02, 0x03, 0xC4, 0x00, 0x80, 0x14,
	0x18, 0x01, 0x03, 0xC4, 0x00, 0x80, 0x14
};
static char orise9806_boe_450_video84[2] = {0x00, 0xB0,};
static char orise9806_boe_450_video85[15] = {
	0xCE, 0x18, 0x04, 0x03, 0xC2, 0x00, 0x80, 0x14,
	0x18, 0x03, 0x03, 0xC2, 0x00, 0x80, 0x14
};
static char orise9806_boe_450_video86[2] = {0x00, 0xC0,};
static char orise9806_boe_450_video87[15] = {
	0xCE, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video88[2] = {0x00, 0xD0,};
static char orise9806_boe_450_video89[15] = {
	0xCE, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video90[2] = {0x00, 0x80,};
static char orise9806_boe_450_video91[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video92[2] = {0x00, 0x90,};
static char orise9806_boe_450_video93[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video94[2] = {0x00, 0xA0,};
static char orise9806_boe_450_video95[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video96[2] = {0x00, 0xB0,};
static char orise9806_boe_450_video97[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9806_boe_450_video98[2] = {0x00, 0xC0,};
static char orise9806_boe_450_video99[11] = {
	0xCF, 0x02, 0x02, 0x20, 0x20, 0x00, 0x00, 0x01,
	0x00, 0x10, 0x00
};
static char orise9806_boe_450_video100[2] = {0x00, 0x00,};
static char orise9806_boe_450_video101[2] = {0xD7, 0x00,};
static char orise9806_boe_450_video102[2] = {0x00, 0x00,};
static char orise9806_boe_450_video103[3] = {0xD8, 0x89, 0x89};
static char orise9806_boe_450_video104[2] = {0x00, 0x00,};
static char orise9806_boe_450_video105[2] = {0xD9, 0x3C,};
static char orise9806_boe_450_video106[2] = {0x00, 0x00,};
static char orise9806_boe_450_video107[9] = {
	0xE1, 0x00, 0x0D, 0x13, 0x0E, 0x06, 0x0F, 0x0A,
	0x09
};
static char orise9806_boe_450_video108[9] = {
	0xE1, 0x04, 0x07, 0x0C, 0x09, 0x10, 0x10, 0x0A,
	0x00
};
static char orise9806_boe_450_video109[2] = {0x00, 0x00,};
static char orise9806_boe_450_video110[9] = {
	0xE2, 0x00, 0x0D, 0x13, 0x0E, 0x06, 0x0F, 0x0A,
	0x09
};
static char orise9806_boe_450_video111[9] = {
	0xE2, 0x04, 0x07, 0x0C, 0x09, 0x10, 0x10, 0x0A,
	0x00
};
static char orise9806_boe_450_video112[2] = {0x00, 0x00,};
static char orise9806_boe_450_video113[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static char orise9806_boe_450_video114[2] = {0x00, 0x00,};
static char orise9806_boe_450_video115[2] = {0x3A, 0x77,};
static char orise9806_boe_450_video116[2] = {0x00, 0x00,};
static char orise9806_boe_450_video117[2] = {0x36, 0x00,};
static char orise9806_boe_450_video118[2] = {0x11, 0x00,};
static char orise9806_boe_450_video119[2] = {0x29, 0x00,};

static struct dsi_cmd_desc orise9806_boe_450_video_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video0), orise9806_boe_450_video0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video1), orise9806_boe_450_video1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video2), orise9806_boe_450_video2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video3), orise9806_boe_450_video3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video4), orise9806_boe_450_video4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video5), orise9806_boe_450_video5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video6), orise9806_boe_450_video6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video7), orise9806_boe_450_video7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video8), orise9806_boe_450_video8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video9), orise9806_boe_450_video9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video10), orise9806_boe_450_video10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video11), orise9806_boe_450_video11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video12), orise9806_boe_450_video12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video13), orise9806_boe_450_video13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video14), orise9806_boe_450_video14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video15), orise9806_boe_450_video15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video16), orise9806_boe_450_video16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video17), orise9806_boe_450_video17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video18), orise9806_boe_450_video18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video19), orise9806_boe_450_video19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video20), orise9806_boe_450_video20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video21), orise9806_boe_450_video21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video22), orise9806_boe_450_video22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video23), orise9806_boe_450_video23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video24), orise9806_boe_450_video24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video25), orise9806_boe_450_video25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video26), orise9806_boe_450_video26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video27), orise9806_boe_450_video27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video28), orise9806_boe_450_video28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video29), orise9806_boe_450_video29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video30), orise9806_boe_450_video30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video31), orise9806_boe_450_video31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video32), orise9806_boe_450_video32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video33), orise9806_boe_450_video33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video34), orise9806_boe_450_video34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video35), orise9806_boe_450_video35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video36), orise9806_boe_450_video36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video37), orise9806_boe_450_video37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video38), orise9806_boe_450_video38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video39), orise9806_boe_450_video39},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video40), orise9806_boe_450_video40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video41), orise9806_boe_450_video41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video42), orise9806_boe_450_video42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video43), orise9806_boe_450_video43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video44), orise9806_boe_450_video44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video45), orise9806_boe_450_video45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video46), orise9806_boe_450_video46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video47), orise9806_boe_450_video47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video48), orise9806_boe_450_video48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video49), orise9806_boe_450_video49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video50), orise9806_boe_450_video50},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video51), orise9806_boe_450_video51},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video52), orise9806_boe_450_video52},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video53), orise9806_boe_450_video53},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video54), orise9806_boe_450_video54},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video55), orise9806_boe_450_video55},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video56), orise9806_boe_450_video56},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video57), orise9806_boe_450_video57},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video58), orise9806_boe_450_video58},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video59), orise9806_boe_450_video59},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video60), orise9806_boe_450_video60},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video61), orise9806_boe_450_video61},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video62), orise9806_boe_450_video62},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video63), orise9806_boe_450_video63},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video64), orise9806_boe_450_video64},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video65), orise9806_boe_450_video65},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video66), orise9806_boe_450_video66},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video67), orise9806_boe_450_video67},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video68), orise9806_boe_450_video68},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video69), orise9806_boe_450_video69},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video70), orise9806_boe_450_video70},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video71), orise9806_boe_450_video71},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video72), orise9806_boe_450_video72},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video73), orise9806_boe_450_video73},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video74), orise9806_boe_450_video74},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video75), orise9806_boe_450_video75},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video76), orise9806_boe_450_video76},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video77), orise9806_boe_450_video77},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video78), orise9806_boe_450_video78},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video79), orise9806_boe_450_video79},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video80), orise9806_boe_450_video80},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video81), orise9806_boe_450_video81},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video82), orise9806_boe_450_video82},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video83), orise9806_boe_450_video83},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video84), orise9806_boe_450_video84},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video85), orise9806_boe_450_video85},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video86), orise9806_boe_450_video86},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video87), orise9806_boe_450_video87},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video88), orise9806_boe_450_video88},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video89), orise9806_boe_450_video89},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video90), orise9806_boe_450_video90},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video91), orise9806_boe_450_video91},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video92), orise9806_boe_450_video92},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video93), orise9806_boe_450_video93},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video94), orise9806_boe_450_video94},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video95), orise9806_boe_450_video95},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video96), orise9806_boe_450_video96},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video97), orise9806_boe_450_video97},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video98), orise9806_boe_450_video98},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video99), orise9806_boe_450_video99},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video100), orise9806_boe_450_video100},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video101), orise9806_boe_450_video101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video102), orise9806_boe_450_video102},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video103), orise9806_boe_450_video103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video104), orise9806_boe_450_video104},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video105), orise9806_boe_450_video105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video106), orise9806_boe_450_video106},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video107), orise9806_boe_450_video107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video108), orise9806_boe_450_video108},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video109), orise9806_boe_450_video109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video110), orise9806_boe_450_video110},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video111), orise9806_boe_450_video111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video112), orise9806_boe_450_video112},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video113), orise9806_boe_450_video113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video114), orise9806_boe_450_video114},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video115), orise9806_boe_450_video115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video116), orise9806_boe_450_video116},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_video117), orise9806_boe_450_video117},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20,
		sizeof(orise9806_boe_450_video118), orise9806_boe_450_video118},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(orise9806_boe_450_video119), orise9806_boe_450_video119},
};

static char orise9806_boe_450_sleepin[2] = {0x10, 0x00};
static char orise9806_boe_450_disp_off[2] = {0x00, 0x00};

static struct dsi_cmd_desc orise9806_boe_450_video_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150,
		sizeof(orise9806_boe_450_sleepin), orise9806_boe_450_sleepin},
	{DTYPE_PERIPHERAL_OFF, 1, 0, 0, 0,
		sizeof(orise9806_boe_450_disp_off), orise9806_boe_450_disp_off},
};
#endif
#endif
