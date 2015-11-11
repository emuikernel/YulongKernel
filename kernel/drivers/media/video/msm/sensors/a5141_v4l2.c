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
#include "msm_ispif.h"
#include "msm_camera_i2c_mux.h"

#define SENSOR_NAME "a5141"
#define PLATFORM_DRIVER_NAME "msm_camera_ar0543"
#define ar0543_obj ar0543_##obj

#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

#define AR0543_VERBOSE_DGB

#ifdef AR0543_VERBOSE_DGB
#define LOG_TAG "ar0543-v4l2"
#define CDBG(fmt, args...) printk(fmt, ##args)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#define CDBG_HIGH(fmt, args...) printk(fmt, ##args)
#endif


struct ar0543_otp_struct {
	uint16_t Module_info_0;	/* Year */
	uint16_t Module_info_1;	/* Date */
	uint16_t Module_info_2;	/* Bit[15:12]:Vendor */
				/* Bit[11:8]:Version */
				/* Bit[7:0]:Reserve */
	uint16_t Module_info_3;	/* Reserve by customer */

	uint16_t R_Gr;		/* AWB information, R/Gr */
	uint16_t B_Gr;		/* AWB information, B/Gr */
	uint16_t Gb_Gr;		/* AWB information, Gb/Gr */
	uint16_t Reserve_0;	/* Reserve for AWB information */

	uint16_t LSC_data[106];	/* LSC Data */

	uint16_t Reserve_1;	/* Reserve information */

	uint16_t AF_infinity;	/* AF information */
	uint16_t AF_macro;	/* AF information */

	uint16_t Reserve_2;	/* Reserve */

	uint16_t Reserve_3;	/* Reserve */
	uint16_t Reserve_4;	/* Reserve*/
} st_ar0543_otp;

static struct msm_sensor_ctrl_t ar0543_s_ctrl;
DEFINE_MUTEX(ar0543_mut);

static struct msm_camera_i2c_reg_conf ar0543_start_stream_conf[] = {

};

static struct msm_camera_i2c_reg_conf ar0543_stop_stream_conf[] = {

};

static struct msm_camera_i2c_reg_conf ar0543_group_hold_on_conf[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf ar0543_group_hold_off_conf[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf ar0543_prev_settings[] = {
	/* Preview 1296x972 30fps */
	{0x3004, 0x0008}, /* x_addr_start */
	{0x3008, 0x0A25}, /* x_addr_end */
	{0x3002, 0x0008}, /* y_start_addr */
	{0x3006, 0x079D}, /* y_addr_end */
	{0x3040, 0x04C3}, /* read_mode */
	{0x034C, 0x0510}, /* x_output_size */
	{0x034E, 0x03CC}, /* y_output_size */
	{0x300C, 0x0CF0}, /* line_length_pck */
	{0x300A, 0x0457}, /* frame_length_lines */
	{0x0101, 0x03, 1},

};

static struct msm_camera_i2c_reg_conf ar0543_snap_settings[] = {
	/* Snapshot 2592*1944 14.8fps */
	{0x3004, 0x0008},/*x_addr_start*/
	{0x3008, 0x0A27},/*x_addr_end*/
	{0x3002, 0x0008},/*y_start_addr*/
	{0x3006, 0x079f},/*y_addr_end*/
	{0x3040, 0x0041},/*read_mode*/
	{0x034C, 0x0A20},/*x_output_size*/
	{0x034E, 0x0798},/*y_output_size*/
	{0x300C, 0x0E70},/*line_length_pck*/
	{0x300A, 0x07E5},/*frame_length_lines*/
	{0x3012, 0x07E4},/*coarse_integration_time*/
	{0x3014, 0x0C8C},/*fine_integration_time*/
	{0x3010, 0x00A0},/*fine_correction*/
	{0x0101, 0x03, 1},
};

static struct msm_camera_i2c_reg_conf ar0543_init_settings_conf[] = {
	{0x301A, 0x0018},
	{0x3064, 0xB800},
	{0x31AE, 0x0202},
	{0x316A, 0x8400},
	{0x316C, 0x8400},
	{0x316E, 0x8400},
	{0x3EFA, 0x1A1F},
	{0x3ED2, 0xD965},
	{0x3ED8, 0x7F1B},
	{0x3EDA, 0x2F11},
	{0x3EE2, 0x0060},
	{0x3EF2, 0xD965},
	{0x3EF8, 0x797F},
	{0x3EFC, 0x286F},
	{0x3EFE, 0x2C01},
	{0x3E00, 0x042F},
	{0x3E02, 0xFFFF},
	{0x3E04, 0xFFFF},
	{0x3E06, 0xFFFF},
	{0x3E08, 0x8071},
	{0x3E0A, 0x7281},
	{0x3E0C, 0x4011},
	{0x3E0E, 0x8010},
	{0x3E10, 0x60A5},
	{0x3E12, 0x4080},
	{0x3E14, 0x4180},
	{0x3E16, 0x0018},
	{0x3E18, 0x46B7},
	{0x3E1A, 0x4994},
	{0x3E1C, 0x4997},
	{0x3E1E, 0x4682},
	{0x3E20, 0x0018},
	{0x3E22, 0x4241},
	{0x3E24, 0x8000},
	{0x3E26, 0x1880},
	{0x3E28, 0x4785},
	{0x3E2A, 0x4992},
	{0x3E2C, 0x4997},
	{0x3E2E, 0x4780},
	{0x3E30, 0x4D80},
	{0x3E32, 0x100C},
	{0x3E34, 0x8000},
	{0x3E36, 0x184A},
	{0x3E38, 0x8042},
	{0x3E3A, 0x001A},
	{0x3E3C, 0x9610},
	{0x3E3E, 0x0C80},
	{0x3E40, 0x4DC6},
	{0x3E42, 0x4A80},
	{0x3E44, 0x0018},
	{0x3E46, 0x8042},
	{0x3E48, 0x8041},
	{0x3E4A, 0x0018},
	{0x3E4C, 0x804B},
	{0x3E4E, 0xB74B},
	{0x3E50, 0x8010},
	{0x3E52, 0x6056},
	{0x3E54, 0x001C},
	{0x3E56, 0x8211},
	{0x3E58, 0x8056},
	{0x3E5A, 0x827C},
	{0x3E5C, 0x0970},
	{0x3E5E, 0x8082},
	{0x3E60, 0x7281},
	{0x3E62, 0x4C40},
	{0x3E64, 0x8E4D},
	{0x3E66, 0x8110},
	{0x3E68, 0x0CAF},
	{0x3E6A, 0x4D80},
	{0x3E6C, 0x100C},
	{0x3E6E, 0x8440},
	{0x3E70, 0x4C81},
	{0x3E72, 0x7C5F},
	{0x3E74, 0x7000},
	{0x3E76, 0x0000},
	{0x3E78, 0x0000},
	{0x3E7A, 0x0000},
	{0x3E7C, 0x0000},
	{0x3E7E, 0x0000},
	{0x3E80, 0x0000},
	{0x3E82, 0x0000},
	{0x3E84, 0x0000},
	{0x3E86, 0x0000},
	{0x3E88, 0x0000},
	{0x3E8A, 0x0000},
	{0x3E8C, 0x0000},
	{0x3E8E, 0x0000},
	{0x3E90, 0x0000},
	{0x3E92, 0x0000},
	{0x3E94, 0x0000},
	{0x3E96, 0x0000},
	{0x3E98, 0x0000},
	{0x3E9A, 0x0000},
	{0x3E9C, 0x0000},
	{0x3E9E, 0x0000},
	{0x3EA0, 0x0000},
	{0x3EA2, 0x0000},
	{0x3EA4, 0x0000},
	{0x3EA6, 0x0000},
	{0x3EA8, 0x0000},
	{0x3EAA, 0x0000},
	{0x3EAC, 0x0000},
	{0x3EAE, 0x0000},
	{0x3EB0, 0x0000},
	{0x3EB2, 0x0000},
	{0x3EB4, 0x0000},
	{0x3EB6, 0x0000},
	{0x3EB8, 0x0000},
	{0x3EBA, 0x0000},
	{0x3EBC, 0x0000},
	{0x3EBE, 0x0000},
	{0x3EC0, 0x0000},
	{0x3EC2, 0x0000},
	{0x3EC4, 0x0000},
	{0x3EC6, 0x0000},
	{0x3EC8, 0x0000},
	{0x3ECA, 0x0000},
	{0x3170, 0x2150},
	{0x317A, 0x0150},
	{0x3ECC, 0x2200},
	{0x3174, 0x0000},
	{0x3176, 0x0000},
	{0x30BC, 0x0384},
	{0x30C0, 0x1220},
	{0x30D4, 0x9200},
	{0x30B2, 0xC000},
	{0x31B0, 0x00C4},
	{0x31B2, 0x0064},
	{0x31B4, 0x0E77},
	{0x31B6, 0x0D24},
	{0x31B8, 0x020E},
	{0x31BA, 0x0710},
	{0x31BC, 0x2A0D},
	{0x31BE, 0xC007},
	{0x305E, 0x112A},
	{0x3ECE, 0x000A},
	{0x0400, 0x0000},
	{0x0404, 0x0010},
	{0x0300, 0x0005},
	{0x0302, 0x0001},
	{0x0304, 0x0002},
	{0x0306, 0x002E},
	{0x0308, 0x000a},
	{0x030A, 0x0001},
	{0x3100, 0x0000},
};

static struct msm_camera_i2c_conf_array ar0543_init_settings[] = {
	{&ar0543_init_settings_conf[0],
	ARRAY_SIZE(ar0543_init_settings_conf), 1, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_camera_i2c_conf_array ar0543_mode_settings[] = {
	{&ar0543_snap_settings[0],
	ARRAY_SIZE(ar0543_snap_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
	{&ar0543_prev_settings[0],
	ARRAY_SIZE(ar0543_prev_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct v4l2_subdev_info ar0543_subdev_info[] = {
	{
	.code                    = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace              = V4L2_COLORSPACE_JPEG,
	.fmt                     = 1,
	.order                   = 0,
	},
};

static struct msm_sensor_output_info_t ar0543_output_settings[] = {
	{ /* For SNAPSHOT */
	.x_output                = 0xA20,    /* 2592 */
	.y_output                = 0x798,    /* 1944 */
	.line_length_pclk        = 0x0E70,   /* 3696 */
	.frame_length_lines      = 0x07E5,   /* 2021 */
	.vt_pixel_clk            = 110400000,/* =14.8*3696*2021 */
/*
	.op_pixel_clk            = 55200000,
	.vt_pixel_clk            = 55200000,
	.op_pixel_clk            = 110400000,
*/
	.op_pixel_clk            = 159408000,
	.binning_factor          = 0x0,
	},
	{ /* For PREVIEW */
	.x_output                = 0x0510,   /* 1296 */
	.y_output                = 0x03CC,   /* 972  */
	.line_length_pclk        = 0x0CF0,   /* 3312 */
	.frame_length_lines      = 0x0457,   /* 1111 */
	.vt_pixel_clk            = 110400000,/* =30.003*3312*1111 */
/*
	.op_pixel_clk            = 55200000,
	.op_pixel_clk            = 110400000,
	.vt_pixel_clk            = 55200000,
*/
	.op_pixel_clk            = 159408000,
	.binning_factor          = 0x0,
	},
};

static struct msm_sensor_output_reg_addr_t ar0543_output_reg_addr = {
	.x_output                = 0x034C,
	.y_output                = 0x034E,
	.line_length_pclk        = 0x300C,
	.frame_length_lines      = 0x300A,
};

static struct msm_sensor_id_info_t ar0543_id_info = {
	.sensor_id_reg_addr        = 0x0000,
	.sensor_id                 = 0x4800,
};

static struct msm_sensor_exp_gain_info_t ar0543_exp_gain_info = {
	.coarse_int_time_addr      = 0x3012,
	.global_gain_addr          = 0x305E,
	.vert_offset               = 0,
};

static unsigned short af_reg_dac;
static unsigned short af_reg_damp;

static void ar0543_start_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x8250, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x8650, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x8658, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0104,
		0x00, MSM_CAMERA_I2C_BYTE_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x065c, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x30f4,
		0x01f4, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x30f2,
		af_reg_dac, MSM_CAMERA_I2C_WORD_DATA);
}

static void ar0543_stop_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	int af_step_pos;
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x30f2,
		&af_reg_dac, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x30f4,
		&af_reg_damp, MSM_CAMERA_I2C_WORD_DATA);
	/*set to zero to void lens crash sound */
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x30f4, 0x0,
		MSM_CAMERA_I2C_WORD_DATA);

	for (af_step_pos = af_reg_dac & 0xff;
	     af_step_pos > 0; af_step_pos -= 8) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x30f2,
			af_step_pos&0xff,  MSM_CAMERA_I2C_WORD_DATA);
		msleep(2);
	}

	msleep(2);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x0058, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x0050, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0104,
		0x01, MSM_CAMERA_I2C_BYTE_DATA);
}

static void ar0543_group_hold_on(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0104,
		0x01, MSM_CAMERA_I2C_BYTE_DATA);
}

static void ar0543_group_hold_off(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0104,
		0x00, MSM_CAMERA_I2C_BYTE_DATA);
}

static int32_t ar0543_write_prev_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t gain, uint32_t line)
{
	uint16_t max_legal_gain = 0xE7F;
	uint16_t min_legal_gain = 0x127; /* 1.584375x */
	int32_t rc = 0;

	if (gain < min_legal_gain) {
		CDBG("gain < min_legal_gain, gain = %d\n", gain);
		gain = min_legal_gain;
	}
	if (gain > max_legal_gain) {
		CDBG("gain > max_legal_gain, gain = %d\n", gain);
		gain = max_legal_gain;
	}
	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x305E,
		(gain|0x1000), MSM_CAMERA_I2C_WORD_DATA);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3012,
		line, MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return rc;
}

static int32_t ar0543_write_pict_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
	uint16_t gain, uint32_t line)
{
	int32_t rc = 0;
	rc = ar0543_write_prev_exp_gain(s_ctrl, gain, line);
	rc = msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		(0x065C|0x2), MSM_CAMERA_I2C_WORD_DATA);
	return rc;
}

static const struct i2c_device_id ar0543_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&ar0543_s_ctrl},
	{ }
};

extern int rst_5m;
extern int pwd_5m;
extern int pwd_front;

int32_t ar0543_sensor_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int32_t rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;
	CDBG("%s E\n", __func__);

	s_ctrl = (struct msm_sensor_ctrl_t *)(id->driver_data);
	s_ctrl->sensor_i2c_addr = s_ctrl->sensor_i2c_addr;
	CDBG("%s: s_ctrl->sensor_i2c_addr = 0x%x\n", __func__,
		s_ctrl->sensor_i2c_addr);
	CDBG("%s: client->addr = 0x%x\n", __func__, client->addr);
	client->addr = client->addr;
	CDBG("%s: client->addr = 0x%x\n", __func__, client->addr);
	rc = msm_sensor_i2c_probe(client, id);
	if (client->dev.platform_data == NULL) {
		CDBG_HIGH("%s: NULL sensor data\n", __func__);
		return -EFAULT;
	}
	s_ctrl = client->dev.platform_data;
	usleep_range(5000, 5100);
	CDBG("%s X\n", __func__);
	return rc;
}

static struct i2c_driver ar0543_i2c_driver = {
	.id_table                  = ar0543_i2c_id,
	.probe                     = ar0543_sensor_i2c_probe,
	.driver                    = {
		.name              = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ar0543_sensor_i2c_client = {
	.addr_type                 = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&ar0543_i2c_driver);
}

static struct v4l2_subdev_core_ops ar0543_subdev_core_ops = {
	.ioctl                     = msm_sensor_subdev_ioctl,
	.s_power                   = msm_sensor_power,
};

static struct v4l2_subdev_video_ops ar0543_subdev_video_ops = {
	.enum_mbus_fmt             = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops ar0543_subdev_ops = {
	.core                      = &ar0543_subdev_core_ops,
	.video                     = &ar0543_subdev_video_ops,
};

static struct msm_cam_clk_info cam_8960_clk_info[] = {
	{"cam_clk", MSM_SENSOR_MCLK_24HZ},
};

static struct msm_cam_clk_info cam_8974_clk_info[] = {
	{"cam_src_clk", 19200000},
	{"cam_clk", -1},
};

extern int32_t msm_sensor_disable_i2c_mux(struct msm_camera_i2c_conf *i2c_conf);
extern int32_t msm_sensor_enable_i2c_mux(struct msm_camera_i2c_conf *i2c_conf);

static int32_t msm_sensor_power_up_for_ar0543(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	struct device *dev = NULL;
	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE)
		dev = &s_ctrl->pdev->dev;
	else
		dev = &s_ctrl->sensor_i2c_client->client->dev;

	s_ctrl->reg_ptr = kzalloc(sizeof(struct regulator *)
			* data->sensor_platform_info->num_vreg, GFP_KERNEL);
	if (!s_ctrl->reg_ptr) {
		pr_err("%s: could not allocate mem for regulators\n",
			__func__);
		return -ENOMEM;
	}

	/*
	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}
	*/

	rc = msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: regulator on failed\n", __func__);
		goto config_vreg_failed;
	}

	rc = msm_camera_enable_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 1);
	if (rc < 0) {
		pr_err("%s: enable regulator failed\n", __func__);
		goto enable_vreg_failed;
	}

	/* mov the position for 0.8v of iovdd because of i2c in
	 * power up process.
	 */
	rc = msm_camera_request_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: request gpio failed\n", __func__);
		goto request_gpio_failed;
	}

	rc = msm_camera_config_gpio_table(data, 1);
	if (rc < 0) {
		pr_err("%s: config gpio failed\n", __func__);
		goto config_gpio_failed;
	}

	if (s_ctrl->sensor_device_type == MSM_SENSOR_I2C_DEVICE) {
		if (s_ctrl->clk_rate != 0)
			cam_8960_clk_info->clk_rate = s_ctrl->clk_rate;

		rc = msm_cam_clk_enable(dev, cam_8960_clk_info,
			s_ctrl->cam_clk, ARRAY_SIZE(cam_8960_clk_info), 1);
		if (rc < 0) {
			pr_err("%s: clk enable failed\n", __func__);
			goto enable_clk_failed;
		}
	} else {
		rc = msm_cam_clk_enable(dev, cam_8974_clk_info,
			s_ctrl->cam_clk, ARRAY_SIZE(cam_8974_clk_info), 1);
		if (rc < 0) {
			pr_err("%s: clk enable failed\n", __func__);
			goto enable_clk_failed;
		}
	}


	if (!s_ctrl->power_seq_delay)
		usleep_range(1000, 2000);
	else if (s_ctrl->power_seq_delay < 20)
		usleep_range((s_ctrl->power_seq_delay * 1000),
			((s_ctrl->power_seq_delay * 1000) + 1000));
	else
		msleep(s_ctrl->power_seq_delay);

	if (data->sensor_platform_info->ext_power_ctrl != NULL)
		data->sensor_platform_info->ext_power_ctrl(1);

	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_enable_i2c_mux(data->sensor_platform_info->i2c_conf);

	if (s_ctrl->sensor_device_type == MSM_SENSOR_PLATFORM_DEVICE) {
		rc = msm_sensor_cci_util(s_ctrl->sensor_i2c_client,
			MSM_CCI_INIT);
		if (rc < 0) {
			pr_err("%s cci_init failed\n", __func__);
			goto cci_init_failed;
		}
	}
	s_ctrl->curr_res = MSM_SENSOR_INVALID_RES;
	return rc;

cci_init_failed:
	if (data->sensor_platform_info->i2c_conf &&
		data->sensor_platform_info->i2c_conf->use_i2c_mux)
		msm_sensor_disable_i2c_mux(
			data->sensor_platform_info->i2c_conf);
enable_clk_failed:
		msm_camera_config_gpio_table(data, 0);
config_gpio_failed:
	msm_camera_enable_vreg(dev,
			s_ctrl->sensordata->sensor_platform_info->cam_vreg,
			s_ctrl->sensordata->sensor_platform_info->num_vreg,
			s_ctrl->vreg_seq,
			s_ctrl->num_vreg_seq,
			s_ctrl->reg_ptr, 0);

enable_vreg_failed:
	msm_camera_config_vreg(dev,
		s_ctrl->sensordata->sensor_platform_info->cam_vreg,
		s_ctrl->sensordata->sensor_platform_info->num_vreg,
		s_ctrl->vreg_seq,
		s_ctrl->num_vreg_seq,
		s_ctrl->reg_ptr, 0);
config_vreg_failed:
	msm_camera_request_gpio_table(data, 0);
request_gpio_failed:
	kfree(s_ctrl->reg_ptr);
	return rc;
}

static int32_t msm_sensor_i2c_enable(struct msm_sensor_ctrl_t *s_ctrl,
		unsigned int on)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *data = s_ctrl->sensordata;
	if (1 == on) {
		rc = msm_camera_request_gpio_table(data, 1);
		if (rc < 0)
			pr_err("%s: request gpio failed\n", __func__);
	} else {
		rc = msm_camera_request_gpio_table(data, 0);
		if (rc < 0)
			pr_err("%s: request gpio failed\n", __func__);
	}
	return rc;
}

static int32_t msm_sensor_xclk_enable(struct msm_sensor_ctrl_t *s_ctrl,
		unsigned int on)
{
	int32_t rc = 0;
	if (1 == on) {
		if (s_ctrl->clk_rate != 0)
			cam_8960_clk_info->clk_rate = s_ctrl->clk_rate;

		rc = msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
			cam_8960_clk_info, s_ctrl->cam_clk,
			ARRAY_SIZE(cam_8960_clk_info), 1);
		if (rc < 0)
			pr_err("%s: clk enable failed\n", __func__);
	} else {
		msm_cam_clk_enable(&s_ctrl->sensor_i2c_client->client->dev,
			cam_8960_clk_info, s_ctrl->cam_clk,
			ARRAY_SIZE(cam_8960_clk_info), 0);
	}

	return rc;
}

static int first_time_power_end;
int32_t ar0543_sensor_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	struct msm_camera_sensor_info *info = NULL;
	CDBG("%s: E\n", __func__);
	pr_debug("edit tag %s", __func__);
	info = s_ctrl->sensordata;
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
	msleep(50);
	gpio_direction_output(pwd_5m, 0);
	usleep_range(5000, 5100);
	gpio_direction_output(rst_5m, 0);
	usleep_range(5000, 5100);

	/* msm_sensor_power_down(s_ctrl); */
	msm_sensor_i2c_enable(s_ctrl, 0);

	msleep(50);
	msm_sensor_xclk_enable(s_ctrl, 0);

	msleep(10);
	CDBG("%s: X\n", __func__);
	return 0;
}

int32_t ar0543_sensor_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct msm_camera_sensor_info *info = NULL;
	info = s_ctrl->sensordata;
	CDBG("%s, sensor_pwd:%d, sensor_reset:%d\r\n",
		__func__, info->sensor_pwd, info->sensor_reset);

	if (first_time_power_end == 0) {
		gpio_direction_output(pwd_5m, 0);
		gpio_direction_output(rst_5m, 0);

		usleep_range(10000, 11000);
		usleep_range(5000, 5100);
		rc = msm_sensor_power_up_for_ar0543(s_ctrl);
		if (rc < 0) {
			CDBG("%s: msm_sensor_power_up failed\n", __func__);
			return rc;
		}
		usleep_range(5000, 5100);
		gpio_direction_output(pwd_5m, 1);
		usleep_range(5000, 5100);
		gpio_direction_output(rst_5m, 1);
		usleep_range(5000, 5100);
		first_time_power_end = 1;
	} else {
		gpio_direction_output(rst_5m, 0);
		usleep_range(5000, 5100);
		msm_sensor_xclk_enable(s_ctrl, 1);
		msleep(20);

		usleep_range(5000, 5100);
		gpio_direction_output(pwd_5m, 1);
		usleep_range(5000, 5100);
		gpio_direction_output(rst_5m, 1);
		usleep_range(5000, 5100);
		usleep_range(5000, 5100);

		msm_sensor_i2c_enable(s_ctrl, 1);
		msleep(50);
	}
	return rc;
}

int32_t ar0543_sensor_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	rc = msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
		MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		CDBG("%s: %s: read id failed\n",
			__func__, s_ctrl->sensordata->sensor_name);
		return rc;
	}
	CDBG("%s: chipid = 0x%x\n", __func__, chipid);
	if (chipid != s_ctrl->sensor_id_info->sensor_id) {
		CDBG("%s: chip id mismatch\n", __func__);
		return -ENODEV;
	}
	return rc;
}

/*******************************************************************************
* otp_type: otp_type should be 0x30, 0x31, 0x32
* return value:
*     0, OTPM have no valid data
*     1, OTPM have valid data
*******************************************************************************/
int ar0543_check_otp_status(struct msm_sensor_ctrl_t *s_ctrl, int otp_type)
{
	uint16_t i = 0;
	uint16_t temp = 0;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x301A,
		0x0610, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3134,
		0xCD95, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x304C,
		(otp_type&0xff) << 8, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x304A,
		0x0200, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x304A,
		0x0210, MSM_CAMERA_I2C_WORD_DATA);

	do {
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x304A,
			&temp, MSM_CAMERA_I2C_WORD_DATA);
		if (0x60 == (temp & 0x60)) {
			CDBG("%s: read success\n", __func__);
			break;
		}
		usleep_range(5000, 5100);
		i++;
	} while (i < 10);

	CDBG("%s: the count of reading loop is: %d\n", __func__, i + 1);
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3800,
		&temp, MSM_CAMERA_I2C_WORD_DATA);

	if (0 == temp) {
		/* OTPM have no valid data */
		return 0;
	else
		/* OTPM have valid data */
		return 1;
}

/*******************************************************************************
* return value:
*     0, OTPM have no valid data
*     1, OTPM have valid data
*******************************************************************************/
int ar0543_read_otp(struct msm_sensor_ctrl_t *s_ctrl,
		struct ar0543_otp_struct *p_otp)
{
	uint16_t i = 0;
	int otp_status = 0;
	if (ar0543_check_otp_status(s_ctrl, 0x31)) {
		/* type 0x31 have valid data, now need check type 0x32 */
		CDBG("%s: type 0x31 have valid data\n", __func__);
		if (ar0543_check_otp_status(s_ctrl, 0x32)) {
			/* type 0x32 have valid data, need use type 0x32 */
			CDBG("%s: type 0x32 have valid data\n", __func__);
			otp_status = 1;
		} else {
			/* type 0x32 have no valid data, need use type 0x31 */
			CDBG("%s: type 0x32 have no valid data\n", __func__);
			otp_status = 1;
			/* use check function switch to type 0x31 */
			ar0543_check_otp_status(s_ctrl, 0x31);
		}
	} else {
		/* type 0x31 have no valid data, now need check type 0x30 */
		CDBG("%s: type 0x31 have no valid data\n", __func__);
		if (ar0543_check_otp_status(s_ctrl, 0x30)) {
			/* type 0x30 have valid data, need use type 0x30 */
			CDBG("%s: type 0x30 have valid data\n", __func__);
			otp_status = 1;
		} else {
			/* type 0x30 have no valid data, no otp data */
			;
		}
	}

	if (otp_status) {
		for (i = 0; i < (sizeof(st_ar0543_otp)/2); i++) {
			msm_camera_i2c_read(s_ctrl->sensor_i2c_client,
				(0x3800+i*2), ((uint16_t *)p_otp + i),
				MSM_CAMERA_I2C_WORD_DATA);
		}
	} else
		CDBG("%s: have no valid otp data\n", __func__);

	CDBG("%s: Module_info_0   = 0x%04x (%d)\n",
		__func__, p_otp->Module_info_0, p_otp->Module_info_0);
	CDBG("%s: Module_info_1   = 0x%04x (%d)\n",
		__func__, p_otp->Module_info_1, p_otp->Module_info_1);
	CDBG("%s: Module_info_2   = 0x%04x (%d)\n",
		__func__, p_otp->Module_info_2, p_otp->Module_info_2);
	CDBG("%s: Module_info_3   = 0x%04x (%d)\n",
		__func__, p_otp->Module_info_3, p_otp->Module_info_3);

	CDBG("%s: R_Gr            = 0x%04x (%d)\n",
		__func__, p_otp->R_Gr, p_otp->R_Gr);
	CDBG("%s: B_Gr            = 0x%04x (%d)\n",
		__func__, p_otp->B_Gr, p_otp->B_Gr);
	CDBG("%s: Gb_Gr           = 0x%04x (%d)\n",
		__func__, p_otp->Gb_Gr, p_otp->Gb_Gr);
	CDBG("%s: Reserve_0       = 0x%04x (%d)\n",
		__func__, p_otp->Reserve_0, p_otp->Reserve_0);

	CDBG("%s: LSC_data[0]     = 0x%04x (%d)\n",
		__func__, p_otp->LSC_data[0], p_otp->LSC_data[0]);
	CDBG("%s: LSC_data[105]   = 0x%04x (%d)\n",
		__func__, p_otp->LSC_data[105], p_otp->LSC_data[105]);

	CDBG("%s: Reserve_1       = 0x%04x (%d)\n",
		__func__, p_otp->Reserve_1, p_otp->Reserve_1);

	CDBG("%s: AF_infinity     = 0x%04x (%d)\n",
		__func__, p_otp->AF_infinity, p_otp->AF_infinity);
	CDBG("%s: AF_macro        = 0x%04x (%d)\n",
		__func__, p_otp->AF_macro, p_otp->AF_macro);

	CDBG("%s: Reserve_2       = 0x%04x (%d)\n",
		__func__, p_otp->Reserve_2, p_otp->Reserve_2);
	CDBG("%s: Reserve_3       = 0x%04x (%d)\n",
		__func__, p_otp->Reserve_3, p_otp->Reserve_3);
	CDBG("%s: Reserve_4       = 0x%04x (%d)\n",
		__func__, p_otp->Reserve_4, p_otp->Reserve_4);

	return otp_status;
}

static void ar0543_write_otp(struct msm_sensor_ctrl_t *s_ctrl,
		struct ar0543_otp_struct *p_otp)
{
	uint16_t i = 0;
	uint16_t temp = 0;
	/* 0 - 20 words */
	for (i = 0; i < 20; i++) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, (0x3600+i*2),
			p_otp->LSC_data[i], MSM_CAMERA_I2C_WORD_DATA);
	}
	/* 20 - 40 words */
	for (i = 0; i < 20; i++) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, (0x3640+i*2),
			p_otp->LSC_data[20+i], MSM_CAMERA_I2C_WORD_DATA);
	}
	/* 40 - 60 words */
	for (i = 0; i < 20; i++) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, (0x3680+i*2),
			p_otp->LSC_data[40+i], MSM_CAMERA_I2C_WORD_DATA);
	}
	/* 60 - 80 words */
	for (i = 0; i < 20; i++) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, (0x36c0+i*2),
			p_otp->LSC_data[60+i], MSM_CAMERA_I2C_WORD_DATA);
	}
	/* 80 - 100 words */
	for (i = 0; i < 20; i++) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, (0x3700+i*2),
			p_otp->LSC_data[80+i], MSM_CAMERA_I2C_WORD_DATA);
	}

	/* last 6 words */
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3782,
		p_otp->LSC_data[100], MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3784,
		p_otp->LSC_data[101], MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x37C0,
		p_otp->LSC_data[102], MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x37C2,
		p_otp->LSC_data[103], MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x37C4,
		p_otp->LSC_data[104], MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x37C6,
		p_otp->LSC_data[105], MSM_CAMERA_I2C_WORD_DATA);

  /* enable poly_sc_enable Bit */
	msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x3780,
		&temp, MSM_CAMERA_I2C_WORD_DATA);
	temp = temp | 0x8000;
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3780,
		temp, MSM_CAMERA_I2C_WORD_DATA);
}

static void ar0543_update_otp(struct msm_sensor_ctrl_t *s_ctrl)
{
	ar0543_read_otp(s_ctrl, &st_ar0543_otp);
	ar0543_write_otp(s_ctrl, &st_ar0543_otp);
}

static int32_t ar0543_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	static int32_t flag = 0;

	if (update_type == MSM_SENSOR_REG_INIT) {
		CDBG("%s: Register INIT\n", __func__);
		flag = 0;

		s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
		/* s_ctrl->curr_csi_params = NULL; */
		msm_sensor_enable_debugfs(s_ctrl);
		msm_sensor_write_init_settings(s_ctrl);
		ar0543_update_otp(s_ctrl);
	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		CDBG("%s: PERIODIC =%d\n", __func__, res);
		if (res == 0)
			msleep(40);
		else
			/* 10 frames */
			msleep(40);

		s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
		msm_sensor_write_res_settings(s_ctrl, res);
		if (res == 1) {
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3012,
				0x0457, MSM_CAMERA_I2C_WORD_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3014,
				0x0908, MSM_CAMERA_I2C_WORD_DATA);
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x3010,
				0x0184, MSM_CAMERA_I2C_WORD_DATA);
		}

		if (0 == flag) {
			msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0100,
				0x01, MSM_CAMERA_I2C_BYTE_DATA);
			flag = 1;
		}
		s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
		CDBG("PERIODIC : register setting is done\n");
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);

		msleep(50);
	}
	return rc;
}

static struct msm_sensor_fn_t ar0543_func_tbl = {
	.sensor_start_stream            = ar0543_start_stream,
	.sensor_stop_stream             = ar0543_stop_stream,
	.sensor_group_hold_on           = ar0543_group_hold_on,
	.sensor_group_hold_off          = ar0543_group_hold_off,
	.sensor_set_fps                 = msm_sensor_set_fps,
	.sensor_write_exp_gain          = ar0543_write_prev_exp_gain,
	.sensor_write_snapshot_exp_gain = ar0543_write_pict_exp_gain,
	.sensor_setting                 = ar0543_sensor_setting,
	.sensor_set_sensor_mode         = msm_sensor_set_sensor_mode,
	.sensor_mode_init               = msm_sensor_mode_init,
	.sensor_get_output_info         = msm_sensor_get_output_info,
	.sensor_config                  = msm_sensor_config,
	.sensor_power_up                = ar0543_sensor_power_up,
	.sensor_power_down              = ar0543_sensor_power_down,
	.sensor_match_id                = ar0543_sensor_match_id,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t ar0543_regs = {
	.default_data_type              = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf              = ar0543_start_stream_conf,
	.start_stream_conf_size         = ARRAY_SIZE(ar0543_start_stream_conf),
	.stop_stream_conf               = ar0543_stop_stream_conf,
	.stop_stream_conf_size          = ARRAY_SIZE(ar0543_stop_stream_conf),
	.group_hold_on_conf             = ar0543_group_hold_on_conf,
	.group_hold_on_conf_size        = ARRAY_SIZE(ar0543_group_hold_on_conf),
	.group_hold_off_conf            = ar0543_group_hold_off_conf,
	.group_hold_off_conf_size       = ARRAY_SIZE(ar0543_group_hold_off_conf),
	.init_settings                  = &ar0543_init_settings[0],
	.init_size                      = ARRAY_SIZE(ar0543_init_settings),
	.mode_settings                  = &ar0543_mode_settings[0],
	.output_settings                = &ar0543_output_settings[0],
	.num_conf                       = ARRAY_SIZE(ar0543_mode_settings),
};

static struct msm_sensor_ctrl_t ar0543_s_ctrl = {
	.msm_sensor_reg                 = &ar0543_regs,
	.sensor_i2c_client              = &ar0543_sensor_i2c_client,
	.sensor_i2c_addr                = 0x6C,
	.sensor_output_reg_addr         = &ar0543_output_reg_addr,
	.sensor_id_info                 = &ar0543_id_info,
	.sensor_exp_gain_info           = &ar0543_exp_gain_info,
	.cam_mode                       = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex               = &ar0543_mut,
	.sensor_i2c_driver              = &ar0543_i2c_driver,
	.sensor_v4l2_subdev_info        = ar0543_subdev_info,
	.sensor_v4l2_subdev_info_size   = ARRAY_SIZE(ar0543_subdev_info),
	.sensor_v4l2_subdev_ops         = &ar0543_subdev_ops,
	.func_tbl                       = &ar0543_func_tbl,
	.clk_rate                       = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Aptina 5M Bayer sensor driver");
MODULE_LICENSE("GPL v2");
