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

#ifndef MIPI_HX8369_BOYI_405_H
#define MIPI_HX8369_BOYI_405_H

#include "mipi_dsi.h"

#define HX8369_BOYI_405_HBP 16
#define HX8369_BOYI_405_HFP 8
#define HX8369_BOYI_405_HPW 8
#define HX8369_BOYI_405_VBP 10
#define HX8369_BOYI_405_VFP 5
#define HX8369_BOYI_405_VPW 5


#define HX8369_BOYI_405_HFPPS 0
#define HX8369_BOYI_405_HBPPS 0
#define HX8369_BOYI_405_HSAPS 0

#define HX8369_BOYI_405_TCLK_POST 0x22
#define HX8369_BOYI_405_TCLK_PRE 0x1b

#define LCD_HX8369_BOYI_405_FRAME_RATE 60

#define HX8369_BOYI_405_XRES    480
#define HX8369_BOYI_405_YRES    800

#ifdef VIDEO_WVGA_PT_PARA

static struct mipi_dsi_phy_ctrl hx8369_boyi_405_dsi_video_mode_phy_db = {
	/* DSI Bit Clock at 500 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x02, 0x0a, 0x04, 0x00, 0x20},
	/* timing   */
	{0xA2, 0x88, 0x13, 0x00, 0x15, 0x8D, 0x17, 0x8A,
	0x15, 0x03, 0x04},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x00, 0xec, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};

static char hx8369_boyi_405_video0[4] = {0xB9, 0xFF, 0x83, 0x69};
static char hx8369_boyi_405_video1[16] = {
	0xB2, 0x00, 0x2b, 0x05,
	0x05, 0x70, 0x00, 0xFF,
	0x06, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x00, 0x01,
};
static char hx8369_boyi_405_video2[6] = {0xB4, 0x03, 0x10, 0x80, 0x06, 0x02};
static char hx8369_boyi_405_video3[27] = {
	0xD5, 0x00, 0x04, 0x03,
	0x00, 0x01, 0x06, 0x20,
	0x70, 0x11, 0x13, 0x00,
	0x11, 0x00, 0x46, 0x00,
	0x57, 0x01, 0x00, 0x44,
	0x07, 0x44, 0x46, 0x07,
	0x0F, 0x02, 0x03,
};
static char hx8369_boyi_405_video4[20] = {
	0xB1, 0x01, 0x00, 0x34,
	0x04, 0x00, 0x10, 0x10,
	0x28, 0x2f, 0x3F, 0x3F,
	0x01, 0x22, 0x01, 0xE6,
	0xE6, 0xE6, 0xE6, 0xE6,
};
static char hx8369_boyi_405_video5[3] = {0xB6, 0x55, 0x55};
static char hx8369_boyi_405_video6[2] = {0xCC, 0x00};
static char hx8369_boyi_405_video7[2] = {0x3a, 0x77};
static char hx8369_boyi_405_video8[35] = {
	0xE0, 0x0c, 0x1f, 0x23,
	0x10, 0x10, 0x27, 0x2b,
	0x35, 0x07, 0x0e, 0x0e,
	0x15, 0x16, 0x14, 0x16,
	0x12, 0x18, 0x0c, 0x1f,
	0x23, 0x10, 0x10, 0x27,
	0x2b, 0x35, 0x07, 0x0e,
	0x0e, 0x15, 0x16, 0x14,
	0x16, 0x12, 0x18,
};
static char hx8369_boyi_405_video9[14] = {
	0xBA, 0x00, 0xA0, 0xC6,
	0x00, 0x0A, 0x00, 0x10,
	0x30, 0x6c, 0x02, 0x11,
	0x18, 0x40,
};
static char hx8369_boyi_405_video10[7] = {
	0xc9, 0x3e, 0x0, 0x0,
	0x0,  0x1f, 0x0,
};
static char hx8369_boyi_405_video11[2] = {
	0x53, 0x24,
};
static char hx8369_boyi_405_video12[2] = {
	0x55, 0x00,
};
static char hx8369_boyi_405_video13[2] = {
	0x51, 0xaa,
};
static char hx8369_boyi_405_exit_sleep[2] = {0x11, 0x00};
static char hx8369_boyi_405_display_on[2] = {0x29, 0x00};


static char hx8369_boyi_405_enter_sleep[2] = {0x10, 0x00};

static struct dsi_cmd_desc hx8369_boyi_405_video_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(hx8369_boyi_405_enter_sleep), hx8369_boyi_405_enter_sleep}
};

static struct dsi_cmd_desc hx8369_boyi_405_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video0), hx8369_boyi_405_video0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video1), hx8369_boyi_405_video1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video2), hx8369_boyi_405_video2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video3), hx8369_boyi_405_video3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video4), hx8369_boyi_405_video4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video5), hx8369_boyi_405_video5},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video6), hx8369_boyi_405_video6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video7), hx8369_boyi_405_video7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video8), hx8369_boyi_405_video8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video9), hx8369_boyi_405_video9},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(hx8369_boyi_405_exit_sleep), hx8369_boyi_405_exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20,
		sizeof(hx8369_boyi_405_display_on), hx8369_boyi_405_display_on},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video10), hx8369_boyi_405_video10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video11), hx8369_boyi_405_video11},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video12), hx8369_boyi_405_video12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(hx8369_boyi_405_video13), hx8369_boyi_405_video13},
};

#endif
#endif
