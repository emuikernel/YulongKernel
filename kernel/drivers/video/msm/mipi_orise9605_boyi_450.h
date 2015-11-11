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

#ifndef MIPI_ORISE9605_BOYI_450_H
#define MIPI_ORISE9605_BOYI_450_H

#include "mipi_dsi.h"

#define ORISE9605_BOYI_450_HBP 48
#define ORISE9605_BOYI_450_HFP 40
#define ORISE9605_BOYI_450_HPW 8
#define ORISE9605_BOYI_450_VBP 16
#define ORISE9605_BOYI_450_VFP 15
#define ORISE9605_BOYI_450_VPW 1


#define ORISE9605_BOYI_450_HFPPS 1
#define ORISE9605_BOYI_450_HBPPS 1
#define ORISE9605_BOYI_450_HSAPS 1

#define ORISE9605_BOYI_450_TCLK_POST 0x19
#define ORISE9605_BOYI_450_TCLK_PRE 0x2F

#define LCD_ORISE9605_BOYI_450_FRAME_RATE 60

#define ORISE9605_BOYI_450_XRES    540
#define ORISE9605_BOYI_450_YRES    960

#ifdef VIDEO_WVGA_PT_PARA

static struct mipi_dsi_phy_ctrl orise9605_boyi_450_dsi_video_mode_phy_db = {
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

static char orise9605_boyi_450_video0[2] = {0x00, 0x00};
static char orise9605_boyi_450_video1[4] = {0xFF, 0x96, 0x05, 0x01};
static char orise9605_boyi_450_video2[2] = {0x00, 0x80};
static char orise9605_boyi_450_video3[3] = {0xFF, 0x96, 0x05};
static char orise9605_boyi_450_video4[2] = {0x00, 0x00};
static char orise9605_boyi_450_video5[2] = {0xA0, 0x00};
static char orise9605_boyi_450_video6[2] = {0x00, 0x92};
static char orise9605_boyi_450_video7[3] = {0xff, 0x10, 0x02};
static char orise9605_boyi_450_video8[2] = {0x00, 0xa0};
static char orise9605_boyi_450_video9[2] = {0xc1, 0x00};
static char orise9605_boyi_450_video10[2] = {0x00, 0x80};
static char orise9605_boyi_450_video11[3] = {0xC1, 0x36, 0x66};
static char orise9605_boyi_450_video12[2] = {0x00, 0x89};
static char orise9605_boyi_450_video13[2] = {0xC0, 0x01};
static char orise9605_boyi_450_video14[2] = {0x00, 0xB4};
static char orise9605_boyi_450_video15[2] = {0xC0, 0x50};
static char orise9605_boyi_450_video16[2] = {0x00, 0x91};
static char orise9605_boyi_450_video17[2] = {0xC5, 0x76};
static char orise9605_boyi_450_video18[2] = {0x00, 0x93};
static char orise9605_boyi_450_video19[2] = {0xC5, 0x76};
static char orise9605_boyi_450_video20[2] = {0x00, 0x90};
static char orise9605_boyi_450_video21[7] = {
	0xC0, 0x00, 0x44, 0x00, 0x00, 0x00, 0x03
};
static char orise9605_boyi_450_video22[2] = {0x00, 0xA6};
static char orise9605_boyi_450_video23[4] = {0xC1, 0x00, 0x00, 0x00};
static char orise9605_boyi_450_video24[2] = {0x00, 0x80};
static char orise9605_boyi_450_video25[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video26[2] = {0x00, 0x90};
static char orise9605_boyi_450_video27[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video28[2] = {0x00, 0xA0};
static char orise9605_boyi_450_video29[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video30[2] = {0x00, 0xB0};
static char orise9605_boyi_450_video31[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video32[2] = {0x00, 0xC0};
static char orise9605_boyi_450_video33[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04,
	0x04, 0x00, 0x00, 0x04, 0x04, 0x04, 0x00, 0x00
};
static char orise9605_boyi_450_video34[2] = {0x00, 0xD0};
static char orise9605_boyi_450_video35[16] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04
};
static char orise9605_boyi_450_video36[2] = {0x00, 0xE0};
static char orise9605_boyi_450_video37[11] = {
	0xCB, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video38[2] = {0x00, 0xF0};
static char orise9605_boyi_450_video39[11] = {
	0xCB, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff
};
static char orise9605_boyi_450_video40[2] = {0x00, 0x80};
static char orise9605_boyi_450_video41[11] = {
	0xCC, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0A,
	0x0E, 0x00, 0x00
};
static char orise9605_boyi_450_video42[2] = {0x00, 0x90};
static char orise9605_boyi_450_video43[16] = {
	0xCC, 0x0C, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09
};
static char orise9605_boyi_450_video44[2] = {0x00, 0xA0};
static char orise9605_boyi_450_video45[16] = {
	0xCC, 0x0D, 0x00, 0x00, 0x0B, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video46[2] = {0x00, 0xB0};
static char orise9605_boyi_450_video47[11] = {
	0xCC, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0A,
	0x0E, 0x00, 0x00
};
static char orise9605_boyi_450_video48[2] = {0x00, 0xC0};
static char orise9605_boyi_450_video49[16] = {
	0xCC, 0x0C, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09
};
static char orise9605_boyi_450_video50[2] = {0x00, 0xD0};
static char orise9605_boyi_450_video51[16] = {
	0xCC, 0x0D, 0x00, 0x00, 0x0B, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video52[2] = {0x00, 0x80};
static char orise9605_boyi_450_video53[13] = {
	0xCE, 0x85, 0x03, 0x18, 0x84, 0x03, 0x18, 0x00,
	0x0F, 0x00, 0x00, 0x0F, 0x00
};
static char orise9605_boyi_450_video54[2] = {0x00, 0x90};
static char orise9605_boyi_450_video55[15] = {
	0xCE, 0x33, 0xC5, 0x18, 0x33, 0xC6, 0x18, 0xF0,
	0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video56[2] = {0x00, 0xA0};
static char orise9605_boyi_450_video57[15] = {
	0xCE, 0x38, 0x03, 0x03, 0xBF, 0x00, 0x18, 0x00,
	0x38, 0x02, 0x03, 0xC0, 0x00, 0x18, 0x00
};
static char orise9605_boyi_450_video58[2] = {0x00, 0xB0};
static char orise9605_boyi_450_video59[15] = {
	0xCE, 0x38, 0x01, 0x03, 0xC1, 0x00, 0x18, 0x00,
	0x38, 0x00, 0x03, 0xC2, 0x00, 0x18, 0x00
};
static char orise9605_boyi_450_video60[2] = {0x00, 0xC0};
static char orise9605_boyi_450_video61[15] = {
	0xCE, 0x30, 0x00, 0x03, 0xC3, 0x00, 0x18, 0x00,
	0x30, 0x01, 0x03, 0xC4, 0x00, 0x18, 0x00
};
static char orise9605_boyi_450_video62[2] = {0x00, 0xD0};
static char orise9605_boyi_450_video63[15] = {
	0xCE, 0x30, 0x02, 0x03, 0xC5, 0x00, 0x18, 0x00,
	0x30, 0x03, 0x03, 0xC6, 0x00, 0x18, 0x00
};
static char orise9605_boyi_450_video64[2] = {0x00, 0x80};
static char orise9605_boyi_450_video65[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video66[2] = {0x00, 0x90};
static char orise9605_boyi_450_video67[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video68[2] = {0x00, 0xa0};
static char orise9605_boyi_450_video69[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video70[2] = {0x00, 0xb0};
static char orise9605_boyi_450_video71[15] = {
	0xCF, 0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0xF0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video72[2] = {0x00, 0xc0};
static char orise9605_boyi_450_video73[11] = {
	0xCF, 0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00
};
static char orise9605_boyi_450_video74[2] = {0x00, 0x80};
static char orise9605_boyi_450_video75[2] = {0xd6, 0x58};
static char orise9605_boyi_450_video76[2] = {0x00, 0x00};
static char orise9605_boyi_450_video77[3] = {0xd8, 0x6f, 0x6f};
static char orise9605_boyi_450_video78[2] = {0x00, 0x00};
static char orise9605_boyi_450_video79[2] = {0xd9, 0x5B};
static char orise9605_boyi_450_video80[2] = {0x00, 0x00};
static char orise9605_boyi_450_video81[17] = {
	0xE1, 0x02, 0x07, 0x0B, 0x0C, 0x05, 0x0B, 0x0A,
	0x08, 0x05, 0x09, 0x11, 0x09, 0x10, 0x10, 0x0B,
	0x08
};
static char orise9605_boyi_450_video82[2] = {0x00, 0x00};
static char orise9605_boyi_450_video83[17] = {
	0xE2, 0x02, 0x07, 0x0B, 0x0C, 0x05, 0x0B, 0x0A,
	0x08, 0x05, 0x09, 0x11, 0x09, 0x10, 0x10, 0x0B,
	0x08
};
static char orise9605_boyi_450_video84[2] = {0x00, 0xB1};
static char orise9605_boyi_450_video85[2] = {0xC5, 0x28};
static char orise9605_boyi_450_video86[2] = {0x00, 0xB2};
static char orise9605_boyi_450_video87[5] = {0xF5, 0x15, 0x00, 0x15, 0x00};
static char orise9605_boyi_450_video88[2] = {0x00, 0xC0};
static char orise9605_boyi_450_video89[2] = {0xC5, 0x00};
static char orise9605_boyi_450_video90[2] = {0x00, 0x80};
static char orise9605_boyi_450_video91[2] = {0xC4, 0x9C};
static char orise9605_boyi_450_video92[2] = {0x00, 0x80};
static char orise9605_boyi_450_video93[29] = {
	0xca, 0x80, 0x61, 0x6A, 0x71, 0x7A, 0x81, 0x8A,
	0x91, 0x9A, 0xA2, 0xAA, 0xB2, 0xBB, 0xC2, 0xC2,
	0xC2, 0xC3, 0xFF, 0xC3, 0xFF, 0xC3, 0xFF, 0x05,
	0x03, 0x05, 0x03, 0x05, 0x03
};
static char orise9605_boyi_450_video94[2] = {0x00, 0x00};
static char orise9605_boyi_450_video95[2] = {0xc7, 0x10};
static char orise9605_boyi_450_video96[19] = {
	0xc8, 0x80, 0x99, 0x99, 0x99, 0xA9, 0xAA, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x89, 0x78, 0x67,
	0x56, 0x45, 0x33
};
static char orise9605_boyi_450_video97[2] = {0x00, 0x00};
static char orise9605_boyi_450_video98[2] = {0xc7, 0x11};
static char orise9605_boyi_450_video99[19] = {
	0xc8, 0x80, 0x99, 0x99, 0x99, 0xA9, 0xAA, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x89, 0x78, 0x67,
	0x56, 0x45, 0x33
};
static char orise9605_boyi_450_video100[2] = {0x00, 0x00};
static char orise9605_boyi_450_video101[2] = {0xc7, 0x12};
static char orise9605_boyi_450_video102[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x99, 0x9A, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x88, 0x88, 0x77,
	0x66, 0x45, 0x33
};
static char orise9605_boyi_450_video103[2] = {0x00, 0x00};
static char orise9605_boyi_450_video104[2] = {0xc7, 0x13};
static char orise9605_boyi_450_video105[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x99, 0x89, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x88, 0x88, 0x78,
	0x67, 0x45, 0x33
};
static char orise9605_boyi_450_video106[2] = {0x00, 0x00};
static char orise9605_boyi_450_video107[2] = {0xc7, 0x14};
static char orise9605_boyi_450_video108[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x99, 0x88, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x89, 0x88, 0x88, 0x88,
	0x77, 0x45, 0x33
};
static char orise9605_boyi_450_video109[2] = {0x00, 0x00};
static char orise9605_boyi_450_video110[2] = {0xc7, 0x15};
static char orise9605_boyi_450_video111[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x99, 0x88, 0x98,
	0x99, 0x89, 0x99, 0x99, 0x89, 0x88, 0x88, 0x88,
	0x78, 0x46, 0x33
};
static char orise9605_boyi_450_video112[2] = {0x00, 0x00};
static char orise9605_boyi_450_video113[2] = {0xc7, 0x16};
static char orise9605_boyi_450_video114[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x89, 0x88, 0x98,
	0x99, 0x89, 0x98, 0x99, 0x89, 0x88, 0x88, 0x88,
	0x78, 0x56, 0x34
};
static char orise9605_boyi_450_video115[2] = {0x00, 0x00};
static char orise9605_boyi_450_video116[2] = {0xc7, 0x17};
static char orise9605_boyi_450_video117[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x89, 0x88, 0x88,
	0x98, 0x99, 0x88, 0x99, 0x89, 0x88, 0x88, 0x88,
	0x88, 0x57, 0x34
};
static char orise9605_boyi_450_video118[2] = {0x00, 0x00};
static char orise9605_boyi_450_video119[2] = {0xc7, 0x18};
static char orise9605_boyi_450_video120[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x89, 0x88, 0x88,
	0x88, 0x98, 0x99, 0x99, 0x88, 0x88, 0x88, 0x88,
	0x77, 0x67, 0x45
};
static char orise9605_boyi_450_video121[2] = {0x00, 0x00};
static char orise9605_boyi_450_video122[2] = {0xc7, 0x19};
static char orise9605_boyi_450_video123[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x89, 0x88, 0x88,
	0x88, 0x88, 0x98, 0x99, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x67, 0x45
};
static char orise9605_boyi_450_video124[2] = {0x00, 0x00};
static char orise9605_boyi_450_video125[2] = {0xc7, 0x1a};
static char orise9605_boyi_450_video126[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x99, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x99, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x77, 0x55
};
static char orise9605_boyi_450_video127[2] = {0x00, 0x00};
static char orise9605_boyi_450_video128[2] = {0xc7, 0x1b};
static char orise9605_boyi_450_video129[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x89, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x98, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x77, 0x57
};
static char orise9605_boyi_450_video130[2] = {0x00, 0x00};
static char orise9605_boyi_450_video131[2] = {0xc7, 0x1c};
static char orise9605_boyi_450_video132[19] = {
	0xC8, 0x80, 0x99, 0x99, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x98, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x56
};
static char orise9605_boyi_450_video133[2] = {0x00, 0x00};
static char orise9605_boyi_450_video134[2] = {0xc7, 0x1d};
static char orise9605_boyi_450_video135[19] = {
	0xC8, 0x80, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88
};
static char orise9605_boyi_450_video136[2] = {0x00, 0x00};
static char orise9605_boyi_450_video137[2] = {0xC7, 0x00};
static char orise9605_boyi_450_video138[2] = {0x00, 0x00};
static char orise9605_boyi_450_video139[35] = {
	0xEC, 0x40, 0x44, 0x44, 0x43, 0x44, 0x44, 0x43,
	0x44, 0x44, 0x43, 0x44, 0x44, 0x43, 0x44, 0x44,
	0x43, 0x44, 0x44, 0x43, 0x44, 0x44, 0x43, 0x44,
	0x44, 0x43, 0x44, 0x44, 0x43, 0x44, 0x44, 0x43,
	0x44, 0x04, 0x00
};
static char orise9605_boyi_450_video140[2] = {0x00, 0x00};
static char orise9605_boyi_450_video141[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static char orise9605_boyi_450_video142[2] = {0x11, 0x00};
static char orise9605_boyi_450_video143[2] = {0x29, 0x00};
static char orise9605_boyi_450_video144[2] = {0x51, 0x00};
static char orise9605_boyi_450_video145[2] = {0x53, 0x24};
static char orise9605_boyi_450_video146[2] = {0x55, 0x01};


static struct dsi_cmd_desc orise9605_boyi_450_video_display_on_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video0), orise9605_boyi_450_video0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video1), orise9605_boyi_450_video1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video2), orise9605_boyi_450_video2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video3), orise9605_boyi_450_video3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video4), orise9605_boyi_450_video4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video5), orise9605_boyi_450_video5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video6), orise9605_boyi_450_video6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video7), orise9605_boyi_450_video7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video8), orise9605_boyi_450_video8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video9), orise9605_boyi_450_video9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video10), orise9605_boyi_450_video10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video11), orise9605_boyi_450_video11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video12), orise9605_boyi_450_video12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video13), orise9605_boyi_450_video13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video14), orise9605_boyi_450_video14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video15), orise9605_boyi_450_video15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video16), orise9605_boyi_450_video16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video17), orise9605_boyi_450_video17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video18), orise9605_boyi_450_video18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video19), orise9605_boyi_450_video19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video20), orise9605_boyi_450_video20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video21), orise9605_boyi_450_video21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video22), orise9605_boyi_450_video22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video23), orise9605_boyi_450_video23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video24), orise9605_boyi_450_video24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video25), orise9605_boyi_450_video25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video26), orise9605_boyi_450_video26},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video27), orise9605_boyi_450_video27},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video28), orise9605_boyi_450_video28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video29), orise9605_boyi_450_video29},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video30), orise9605_boyi_450_video30},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video31), orise9605_boyi_450_video31},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video32), orise9605_boyi_450_video32},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video33), orise9605_boyi_450_video33},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video34), orise9605_boyi_450_video34},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video35), orise9605_boyi_450_video35},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video36), orise9605_boyi_450_video36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video37), orise9605_boyi_450_video37},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video38), orise9605_boyi_450_video38},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video39), orise9605_boyi_450_video39},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video40), orise9605_boyi_450_video40},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video41), orise9605_boyi_450_video41},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video42), orise9605_boyi_450_video42},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video43), orise9605_boyi_450_video43},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video44), orise9605_boyi_450_video44},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video45), orise9605_boyi_450_video45},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video46), orise9605_boyi_450_video46},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video47), orise9605_boyi_450_video47},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video48), orise9605_boyi_450_video48},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video49), orise9605_boyi_450_video49},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video50), orise9605_boyi_450_video50},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video51), orise9605_boyi_450_video51},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video52), orise9605_boyi_450_video52},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video53), orise9605_boyi_450_video53},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video54), orise9605_boyi_450_video54},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video55), orise9605_boyi_450_video55},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video56), orise9605_boyi_450_video56},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video57), orise9605_boyi_450_video57},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video58), orise9605_boyi_450_video58},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video59), orise9605_boyi_450_video59},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video60), orise9605_boyi_450_video60},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video61), orise9605_boyi_450_video61},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video62), orise9605_boyi_450_video62},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video63), orise9605_boyi_450_video63},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video64), orise9605_boyi_450_video64},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video65), orise9605_boyi_450_video65},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video66), orise9605_boyi_450_video66},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video67), orise9605_boyi_450_video67},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video68), orise9605_boyi_450_video68},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video69), orise9605_boyi_450_video69},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video70), orise9605_boyi_450_video70},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video71), orise9605_boyi_450_video71},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video72), orise9605_boyi_450_video72},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video73), orise9605_boyi_450_video73},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video74), orise9605_boyi_450_video74},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video75), orise9605_boyi_450_video75},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video76), orise9605_boyi_450_video76},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video77), orise9605_boyi_450_video77},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video78), orise9605_boyi_450_video78},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video79), orise9605_boyi_450_video79},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video80), orise9605_boyi_450_video80},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video81), orise9605_boyi_450_video81},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video82), orise9605_boyi_450_video82},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video83), orise9605_boyi_450_video83},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video84), orise9605_boyi_450_video84},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video85), orise9605_boyi_450_video85},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video86), orise9605_boyi_450_video86},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video87), orise9605_boyi_450_video87},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video88), orise9605_boyi_450_video88},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video89), orise9605_boyi_450_video89},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video90), orise9605_boyi_450_video90},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video91), orise9605_boyi_450_video91},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video92), orise9605_boyi_450_video92},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video93), orise9605_boyi_450_video93},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video94), orise9605_boyi_450_video94},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video95), orise9605_boyi_450_video95},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video96), orise9605_boyi_450_video96},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video97), orise9605_boyi_450_video97},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video98), orise9605_boyi_450_video98},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video99), orise9605_boyi_450_video99},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video100), orise9605_boyi_450_video100},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video101), orise9605_boyi_450_video101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video102), orise9605_boyi_450_video102},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video103), orise9605_boyi_450_video103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video104), orise9605_boyi_450_video104},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video105), orise9605_boyi_450_video105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video106), orise9605_boyi_450_video106},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video107), orise9605_boyi_450_video107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video108), orise9605_boyi_450_video108},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video109), orise9605_boyi_450_video109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video110), orise9605_boyi_450_video110},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video111), orise9605_boyi_450_video111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video112), orise9605_boyi_450_video112},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video113), orise9605_boyi_450_video113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video114), orise9605_boyi_450_video114},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video115), orise9605_boyi_450_video115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video116), orise9605_boyi_450_video116},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video117), orise9605_boyi_450_video117},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video118), orise9605_boyi_450_video118},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video119), orise9605_boyi_450_video119},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video120), orise9605_boyi_450_video120},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video121), orise9605_boyi_450_video121},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video122), orise9605_boyi_450_video122},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video123), orise9605_boyi_450_video123},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video124), orise9605_boyi_450_video124},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video125), orise9605_boyi_450_video125},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video126), orise9605_boyi_450_video126},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video127), orise9605_boyi_450_video127},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video128), orise9605_boyi_450_video128},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video129), orise9605_boyi_450_video129},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video130), orise9605_boyi_450_video130},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video131), orise9605_boyi_450_video131},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video132), orise9605_boyi_450_video132},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video133), orise9605_boyi_450_video133},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video134), orise9605_boyi_450_video134},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video135), orise9605_boyi_450_video135},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video136), orise9605_boyi_450_video136},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video137), orise9605_boyi_450_video137},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video138), orise9605_boyi_450_video138},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video139), orise9605_boyi_450_video139},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video140), orise9605_boyi_450_video140},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video141), orise9605_boyi_450_video141},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(orise9605_boyi_450_video142), orise9605_boyi_450_video142},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20,
		sizeof(orise9605_boyi_450_video143), orise9605_boyi_450_video143},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video144), orise9605_boyi_450_video144},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video145), orise9605_boyi_450_video145},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(orise9605_boyi_450_video146), orise9605_boyi_450_video146},

};

static char orise9605_boyi_450_sleepin[2] = {0x28, 0x00};
static char orise9605_boyi_450_disp_off[2] = {0x10, 0x00};

static struct dsi_cmd_desc orise9605_boyi_450_video_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150,
		sizeof(orise9605_boyi_450_sleepin), orise9605_boyi_450_sleepin},
	{DTYPE_DCS_WRITE, 1, 0, 0, 50,
		sizeof(orise9605_boyi_450_disp_off), orise9605_boyi_450_disp_off},
};
#endif
#endif
