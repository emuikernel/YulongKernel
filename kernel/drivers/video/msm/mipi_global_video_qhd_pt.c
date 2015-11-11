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

#include "msm_fb.h"

#define VIDEO_WVGA_PT_PARA
#include "mipi_dsi.h"
#include "mipi_global.h"

#define LCD_TYPE "lcd_id="

static int atoi(const char *name)
{
	int val = 0;

	for (;; name++) {
		switch (*name) {
		case '0' ... '9':
			val = 10*val + (*name-'0');
			break;
		default:
			return val;
		}
	}
}

static unsigned get_lcd_type(void)
{
#define SAVED_COMMAND_LINE_DATA 900
	char buff[SAVED_COMMAND_LINE_DATA];
	char *p;
	char *q;
	int saved_command_line_len = strlen(saved_command_line);
	int len = ((sizeof(buff) - 1) > saved_command_line_len ? saved_command_line_len : (sizeof(buff) - 1));
	unsigned lcd_type  = 0;

	strncpy(buff, saved_command_line, len);
	buff[len] = 0;

	p = strstr(buff, LCD_TYPE);
	if (p == NULL) {
		pr_err("%s: can't find '%s' in /proc/cmdline\n",
			__func__, LCD_TYPE);
		return (unsigned)LCD_DEFAULT_TYPE;
	}
	p += strlen(LCD_TYPE);  /* skip option */
	q = p;
	while (*q && *q != ' ' && *q != '\t')
		q += 1;
	*q = '\0';
	lcd_type = (unsigned)atoi(p);
	pr_info("-----lcd_type is %d\n", lcd_type);
	return lcd_type;
}

static struct msm_panel_info pinfo;
static int mipi_video_mipi_global_wvga_pt_init(void)
{
	int ret;
	unsigned lcd_id;
	pr_debug("---%s---\n", __func__);

	lcd_id = get_lcd_type();

	switch (lcd_id) {
#ifdef SUPPORT_ORISE9806_YASHI_450
	case ORISE9806_YASHI_450_ID:
		pinfo.xres = ORISE9806_YASHI_450_XRES;
		pinfo.yres = ORISE9806_YASHI_450_YRES;
		pinfo.lcdc.h_back_porch = ORISE9806_YASHI_450_HBP;
		pinfo.lcdc.h_front_porch = ORISE9806_YASHI_450_HFP;
		pinfo.lcdc.h_pulse_width = ORISE9806_YASHI_450_HPW;
		pinfo.lcdc.v_back_porch =  ORISE9806_YASHI_450_VBP;
		pinfo.lcdc.v_front_porch = ORISE9806_YASHI_450_VFP;
		pinfo.lcdc.v_pulse_width = ORISE9806_YASHI_450_VPW;
		pinfo.mipi.dsi_phy_db =
			&orise9806_yashi_450_dsi_video_mode_phy_db;
		/*ykl add*/
		pinfo.mipi.dsi_on_cmd =
			orise9806_yashi_450_video_display_on_cmds;
		pinfo.mipi.dsi_off_cmd =
			orise9806_yashi_450_video_display_off_cmds;
		pinfo.mipi.count_on_cmds =
			ARRAY_SIZE(orise9806_yashi_450_video_display_on_cmds);
		pinfo.mipi.count_off_cmds =
			ARRAY_SIZE(orise9806_yashi_450_video_display_off_cmds);
		/*end*/
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		/* LP-11 during the HFP period */
		pinfo.mipi.hfp_power_stop = ORISE9806_YASHI_450_HFPPS;
		/* LP-11 during the HBP period */
		pinfo.mipi.hbp_power_stop = ORISE9806_YASHI_450_HBPPS;
		/* LP-11 during the HSA period */
		pinfo.mipi.hsa_power_stop = ORISE9806_YASHI_450_HSAPS;
		pinfo.mipi.t_clk_post = ORISE9806_YASHI_450_TCLK_POST;
		pinfo.mipi.t_clk_pre = ORISE9806_YASHI_450_TCLK_PRE;
		pinfo.mipi.frame_rate = LCD_ORISE9806_YASHI_450_FRAME_RATE;
		break;
#endif
#ifdef SUPPORT_ORISE9605_YASHI_450
	case ORISE9605_YASHI_450_ID:
		pinfo.xres = ORISE9605_YASHI_450_XRES;
		pinfo.yres = ORISE9605_YASHI_450_YRES;
		pinfo.lcdc.h_back_porch = ORISE9605_YASHI_450_HBP;
		pinfo.lcdc.h_front_porch = ORISE9605_YASHI_450_HFP;
		pinfo.lcdc.h_pulse_width = ORISE9605_YASHI_450_HPW;
		pinfo.lcdc.v_back_porch =  ORISE9605_YASHI_450_VBP;
		pinfo.lcdc.v_front_porch = ORISE9605_YASHI_450_VFP;
		pinfo.lcdc.v_pulse_width = ORISE9605_YASHI_450_VPW;
		pinfo.mipi.dsi_phy_db =
			&orise9605_yashi_450_dsi_video_mode_phy_db;
		/*ykl add*/
		pinfo.mipi.dsi_on_cmd =
			orise9605_yashi_450_video_display_on_cmds;
		pinfo.mipi.dsi_off_cmd =
			orise9605_yashi_450_video_display_off_cmds;
		pinfo.mipi.count_on_cmds =
			ARRAY_SIZE(orise9605_yashi_450_video_display_on_cmds);
		pinfo.mipi.count_off_cmds =
			ARRAY_SIZE(orise9605_yashi_450_video_display_off_cmds);
		/*end*/
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		/* LP-11 during the HFP period */
		pinfo.mipi.hfp_power_stop = ORISE9605_YASHI_450_HFPPS;
		/* LP-11 during the HBP period */
		pinfo.mipi.hbp_power_stop = ORISE9605_YASHI_450_HBPPS;
		/* LP-11 during the HSA period */
		pinfo.mipi.hsa_power_stop = ORISE9605_YASHI_450_HSAPS;
		pinfo.mipi.t_clk_post = ORISE9605_YASHI_450_TCLK_POST;
		pinfo.mipi.t_clk_pre = ORISE9605_YASHI_450_TCLK_PRE;
		pinfo.mipi.frame_rate = LCD_ORISE9605_YASHI_450_FRAME_RATE;
		break;
#endif
#ifdef SUPPORT_ORISE9605_BOYI_450
	case ORISE9605_BOYI_450_ID:
		pinfo.xres = ORISE9605_BOYI_450_XRES;
		pinfo.yres = ORISE9605_BOYI_450_YRES;
		pinfo.lcdc.h_back_porch = ORISE9605_BOYI_450_HBP;
		pinfo.lcdc.h_front_porch = ORISE9605_BOYI_450_HFP;
		pinfo.lcdc.h_pulse_width = ORISE9605_BOYI_450_HPW;
		pinfo.lcdc.v_back_porch =  ORISE9605_BOYI_450_VBP;
		pinfo.lcdc.v_front_porch = ORISE9605_BOYI_450_VFP;
		pinfo.lcdc.v_pulse_width = ORISE9605_BOYI_450_VPW;
		pinfo.mipi.dsi_phy_db =
			&orise9605_boyi_450_dsi_video_mode_phy_db;
		/*ykl add*/
		pinfo.mipi.dsi_on_cmd =
			orise9605_boyi_450_video_display_on_cmds;
		pinfo.mipi.dsi_off_cmd =
			orise9605_boyi_450_video_display_off_cmds;
		pinfo.mipi.count_on_cmds =
			ARRAY_SIZE(orise9605_boyi_450_video_display_on_cmds);
		pinfo.mipi.count_off_cmds =
			ARRAY_SIZE(orise9605_boyi_450_video_display_off_cmds);
		/*end*/
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		/* LP-11 during the HFP period */
		pinfo.mipi.hfp_power_stop = ORISE9605_BOYI_450_HFPPS;
		/* LP-11 during the HBP period */
		pinfo.mipi.hbp_power_stop = ORISE9605_BOYI_450_HBPPS;
		/* LP-11 during the HSA period */
		pinfo.mipi.hsa_power_stop = ORISE9605_BOYI_450_HSAPS;
		pinfo.mipi.t_clk_post = ORISE9605_BOYI_450_TCLK_POST;
		pinfo.mipi.t_clk_pre = ORISE9605_BOYI_450_TCLK_PRE;
		pinfo.mipi.frame_rate = LCD_ORISE9605_BOYI_450_FRAME_RATE;
		break;
#endif
#ifdef SUPPORT_ORISE9806_BOE_450
	case ORISE9806_BOE_450_ID:
		pinfo.xres = ORISE9806_BOE_450_XRES;
		pinfo.yres = ORISE9806_BOE_450_YRES;
		pinfo.lcdc.h_back_porch = ORISE9806_BOE_450_HBP;
		pinfo.lcdc.h_front_porch = ORISE9806_BOE_450_HFP;
		pinfo.lcdc.h_pulse_width = ORISE9806_BOE_450_HPW;
		pinfo.lcdc.v_back_porch =  ORISE9806_BOE_450_VBP;
		pinfo.lcdc.v_front_porch = ORISE9806_BOE_450_VFP;
		pinfo.lcdc.v_pulse_width = ORISE9806_BOE_450_VPW;
		pinfo.mipi.dsi_phy_db =
			&orise9806_boe_450_dsi_video_mode_phy_db;
		/*ykl add*/
		pinfo.mipi.dsi_on_cmd = orise9806_boe_450_video_display_on_cmds;
		pinfo.mipi.dsi_off_cmd =
			orise9806_boe_450_video_display_off_cmds;
		pinfo.mipi.count_on_cmds =
			ARRAY_SIZE(orise9806_boe_450_video_display_on_cmds);
		pinfo.mipi.count_off_cmds =
			ARRAY_SIZE(orise9806_boe_450_video_display_off_cmds);
		/*end*/
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		/* LP-11 during the HFP period */
		pinfo.mipi.hfp_power_stop = ORISE9806_BOE_450_HFPPS;
		/* LP-11 during the HBP period */
		pinfo.mipi.hbp_power_stop = ORISE9806_BOE_450_HBPPS;
		/* LP-11 during the HSA period */
		pinfo.mipi.hsa_power_stop = ORISE9806_BOE_450_HSAPS;
		pinfo.mipi.t_clk_post = ORISE9806_BOE_450_TCLK_POST;
		pinfo.mipi.t_clk_pre = ORISE9806_BOE_450_TCLK_PRE;
		pinfo.mipi.frame_rate = LCD_ORISE9806_BOE_450_FRAME_RATE;
		break;
#endif
#ifdef SUPPORT_HX8369_BOYI_405
	case HX8369_BOYI_405_ID:
		pinfo.xres = HX8369_BOYI_405_XRES;
		pinfo.yres = HX8369_BOYI_405_YRES;
		pinfo.lcdc.h_back_porch = HX8369_BOYI_405_HBP;
		pinfo.lcdc.h_front_porch = HX8369_BOYI_405_HFP;
		pinfo.lcdc.h_pulse_width = HX8369_BOYI_405_HPW;
		pinfo.lcdc.v_back_porch =  HX8369_BOYI_405_VBP;
		pinfo.lcdc.v_front_porch = HX8369_BOYI_405_VFP;
		pinfo.lcdc.v_pulse_width = HX8369_BOYI_405_VPW;
		pinfo.mipi.dsi_phy_db = &hx8369_boyi_405_dsi_video_mode_phy_db;
		/*ykl add*/
		pinfo.mipi.dsi_on_cmd = hx8369_boyi_405_video_display_on_cmds;
		pinfo.mipi.dsi_off_cmd = hx8369_boyi_405_video_display_off_cmds;
		pinfo.mipi.count_on_cmds =
			ARRAY_SIZE(hx8369_boyi_405_video_display_on_cmds);
		pinfo.mipi.count_off_cmds =
			ARRAY_SIZE(hx8369_boyi_405_video_display_off_cmds);
		/*end*/
		pinfo.mipi.mode = DSI_VIDEO_MODE;
		/* LP-11 during the HFP period */
		pinfo.mipi.hfp_power_stop = HX8369_BOYI_405_HFPPS;
		/* LP-11 during the HBP period */
		pinfo.mipi.hbp_power_stop = HX8369_BOYI_405_HBPPS;
		/* LP-11 during the HSA period */
		pinfo.mipi.hsa_power_stop = HX8369_BOYI_405_HSAPS;
		pinfo.mipi.t_clk_post = HX8369_BOYI_405_TCLK_POST;
		pinfo.mipi.t_clk_pre = HX8369_BOYI_405_TCLK_PRE;
		pinfo.mipi.frame_rate = LCD_HX8369_BOYI_405_FRAME_RATE;
		break;
#endif
	default:
		pr_err("---invalidate lcd panel\n");
		return -EINVAL;
	}

	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0x00;	/* blue */
	/* number of dot_clk cycles HSYNC active edge is
	delayed from VSYNC active edge */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 0;
	pinfo.fb_num = 2;

	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_BURST_MODE;
	pinfo.mipi.dst_format =  DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB; /* RGB */
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;

	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;

	pinfo.mipi.dlane_swap = 0x01;
	/* append EOT at the end of data burst */
	pinfo.mipi.tx_eot_append = 0x01;
	/*ykl add*/
	pinfo.mipi.esc_byte_ratio = 4;
	/*end*/

	ret = mipi_global_device_register(&pinfo, MIPI_DSI_PRIM,
						lcd_id);
	pr_debug("---%s----exit---\n", __func__);

	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_mipi_global_wvga_pt_init);
