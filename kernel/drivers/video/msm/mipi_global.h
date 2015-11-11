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
 */

#ifndef MIPI_GLOBAL_H
#define MIPI_GLOBAL_H

#if defined(CONFIG_BOARD_MTR3)
#define SUPPORT_ORISE9806_YASHI_450
#define SUPPORT_ORISE9605_YASHI_450
#define SUPPORT_ORISE9605_BOYI_450
#define LCD_DEFAULT_TYPE 2
#elif defined(CONFIG_BOARD_QC1)
#define SUPPORT_ORISE9806_YASHI_450
#define SUPPORT_ORISE9605_YASHI_450
#define SUPPORT_ORISE9605_BOYI_450
#define LCD_DEFAULT_TYPE 2
#elif defined(CONFIG_BOARD_CP8860U) || defined(CONFIG_BOARD_CP8861U) || \
	defined(CONFIG_BOARD_CP8870U)
#define SUPPORT_ORISE9806_YASHI_450
#define SUPPORT_ORISE9605_YASHI_450
#define SUPPORT_ORISE9605_BOYI_450
#define LCD_DEFAULT_TYPE 2
#endif

#ifdef SUPPORT_NT35516_LEAD_450
#define NT35516_LEAD_450_ID 0
#include "mipi_nt35516_lead_450.h"
#endif

#ifdef SUPPORT_HX8369_BOYI_405
#define HX8369_BOYI_405_ID 1
#include "mipi_hx8369_boyi_405.h"
#endif

#ifdef SUPPORT_ORISE9806_YASHI_450
#define ORISE9806_YASHI_450_ID 2
#include "mipi_orise9806_yashi_450.h"
#endif

#ifdef SUPPORT_ORISE9806_BOE_450
#define ORISE9806_BOE_450_ID 3
#include "mipi_orise9806_boe_450.h"
#endif

#ifdef SUPPORT_ORISE9605_YASHI_450
#define ORISE9605_YASHI_450_ID 4
#include "mipi_orise9605_yashi_450.h"
#endif

#ifdef SUPPORT_ORISE9605_BOYI_450
#define ORISE9605_BOYI_450_ID 5
#include "mipi_orise9605_boyi_450.h"
#endif

int mipi_global_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);
#endif  /* MIPI_TRULY_H */
