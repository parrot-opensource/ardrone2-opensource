/*
 * OmniVision OV9740 Camera Driver
 *
 * Copyright (C) 2011 NVIDIA Corporation
 *
 * Based on ov9640 camera driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <media/v4l2-chip-ident.h>
#include <media/v4l2-device.h>
#include <media/ov9740.h>

#define to_ov9740_sensor(sd)	container_of(sd, struct ov9740_priv, subdev)

/* General Status Registers */
#define OV9740_MODEL_ID_HI		0x0000
#define OV9740_MODEL_ID_LO		0x0001
#define OV9740_REVISION_NUMBER		0x0002
#define OV9740_MANUFACTURER_ID		0x0003
#define OV9740_SMIA_VERSION		0x0004

/* General Setup Registers */
#define OV9740_MODE_SELECT		0x0100
#define OV9740_IMAGE_ORT		0x0101
#define OV9740_SOFTWARE_RESET		0x0103
#define OV9740_GRP_PARAM_HOLD		0x0104
#define OV9740_MSK_CORRUP_FM		0x0105

/* Timing Setting */
#define OV9740_FRM_LENGTH_LN_HI		0x0340 /*VTS*/
#define OV9740_FRM_LENGTH_LN_LO		0x0341 /*VTS*/
#define OV9740_LN_LENGTH_PCK_HI		0x0342 /*HTS*/
#define OV9740_LN_LENGTH_PCK_LO		0x0343 /*HTS*/
#define OV9740_X_ADDR_START_HI		0x0344
#define OV9740_X_ADDR_START_LO		0x0345
#define OV9740_Y_ADDR_START_HI		0x0346
#define OV9740_Y_ADDR_START_LO		0x0347
#define OV9740_X_ADDR_END_HI		0x0348
#define OV9740_X_ADDR_END_LO		0x0349
#define OV9740_Y_ADDR_END_HI		0x034A
#define OV9740_Y_ADDR_END_LO		0x034B
#define OV9740_X_OUTPUT_SIZE_HI		0x034C
#define OV9740_X_OUTPUT_SIZE_LO		0x034D
#define OV9740_Y_OUTPUT_SIZE_HI		0x034E
#define OV9740_Y_OUTPUT_SIZE_LO		0x034F

/* IO Control Registers */
#define OV9740_IO_CREL00		0x3002
#define OV9740_IO_CREL01		0x3004
#define OV9740_IO_CREL02		0x3005
#define OV9740_IO_OUTPUT_SEL01		0x3026
#define OV9740_IO_OUTPUT_SEL02		0x3027

/* AWB Registers */
#define OV9740_AWB_MANUAL_CTRL		0x3406

/* Analog Control Registers */
#define OV9740_ANALOG_CTRL01		0x3601
#define OV9740_ANALOG_CTRL02		0x3602
#define OV9740_ANALOG_CTRL03		0x3603
#define OV9740_ANALOG_CTRL04		0x3604
#define OV9740_ANALOG_CTRL10		0x3610
#define OV9740_ANALOG_CTRL12		0x3612
#define OV9740_ANALOG_CTRL20		0x3620
#define OV9740_ANALOG_CTRL21		0x3621
#define OV9740_ANALOG_CTRL22		0x3622
#define OV9740_ANALOG_CTRL30		0x3630
#define OV9740_ANALOG_CTRL31		0x3631
#define OV9740_ANALOG_CTRL32		0x3632
#define OV9740_ANALOG_CTRL33		0x3633

/* Sensor Control */
#define OV9740_SENSOR_CTRL03		0x3703
#define OV9740_SENSOR_CTRL04		0x3704
#define OV9740_SENSOR_CTRL05		0x3705
#define OV9740_SENSOR_CTRL07		0x3707

/* Timing Control */
#define OV9740_TIMING_CTRL17		0x3817
#define OV9740_TIMING_CTRL19		0x3819
#define OV9740_TIMING_CTRL33		0x3833
#define OV9740_TIMING_CTRL35		0x3835

/* Banding Filter */
#define OV9740_AEC_MAXEXPO_60_H		0x3A02
#define OV9740_AEC_MAXEXPO_60_L		0x3A03
#define OV9740_AEC_B50_STEP_HI		0x3A08
#define OV9740_AEC_B50_STEP_LO		0x3A09
#define OV9740_AEC_B60_STEP_HI		0x3A0A
#define OV9740_AEC_B60_STEP_LO		0x3A0B
#define OV9740_AEC_CTRL0D		0x3A0D
#define OV9740_AEC_CTRL0E		0x3A0E
#define OV9740_AEC_MAXEXPO_50_H		0x3A14
#define OV9740_AEC_MAXEXPO_50_L		0x3A15

/* AEC/AGC Control */
#define OV9740_AEC_ENABLE		0x3503
#define OV9740_GAIN_CEILING_01		0x3A18
#define OV9740_GAIN_CEILING_02		0x3A19
#define OV9740_AEC_HI_THRESHOLD		0x3A11
#define OV9740_AEC_3A1A			0x3A1A
#define OV9740_AEC_CTRL1B_WPT2		0x3A1B
#define OV9740_AEC_CTRL0F_WPT		0x3A0F
#define OV9740_AEC_CTRL10_BPT		0x3A10
#define OV9740_AEC_CTRL1E_BPT2		0x3A1E
#define OV9740_AEC_LO_THRESHOLD		0x3A1F

/* BLC Control */
#define OV9740_BLC_AUTO_ENABLE		0x4002
#define OV9740_BLC_MODE			0x4005

/* VFIFO */
#define OV9740_VFIFO_READ_START_HI	0x4608
#define OV9740_VFIFO_READ_START_LO	0x4609

/* DVP Control */
#define OV9740_DVP_VSYNC_CTRL02		0x4702
#define OV9740_DVP_VSYNC_MODE		0x4704
#define OV9740_DVP_VSYNC_CTRL06		0x4706

/* PLL Setting */
#define OV9740_PLL_MODE_CTRL01		0x3104
#define OV9740_PRE_PLL_CLK_DIV		0x0305
#define OV9740_PLL_MULTIPLIER		0x0307
#define OV9740_VT_SYS_CLK_DIV		0x0303
#define OV9740_VT_PIX_CLK_DIV		0x0301
#define OV9740_PLL_CTRL3010		0x3010
#define OV9740_VFIFO_CTRL00		0x460E

/* ISP Control */
#define OV9740_ISP_CTRL00		0x5000
#define OV9740_ISP_CTRL01		0x5001
#define OV9740_ISP_CTRL03		0x5003
#define OV9740_ISP_CTRL05		0x5005
#define OV9740_ISP_CTRL12		0x5012
#define OV9740_ISP_CTRL19		0x5019
#define OV9740_ISP_CTRL1A		0x501A
#define OV9740_ISP_CTRL1E		0x501E
#define OV9740_ISP_CTRL1F		0x501F
#define OV9740_ISP_CTRL20		0x5020
#define OV9740_ISP_CTRL21		0x5021

/* AWB */
#define OV9740_AWB_CTRL00		0x5180
#define OV9740_AWB_CTRL01		0x5181
#define OV9740_AWB_CTRL02		0x5182
#define OV9740_AWB_CTRL03		0x5183
#define OV9740_AWB_ADV_CTRL01		0x5184
#define OV9740_AWB_ADV_CTRL02		0x5185
#define OV9740_AWB_ADV_CTRL03		0x5186
#define OV9740_AWB_ADV_CTRL04		0x5187
#define OV9740_AWB_ADV_CTRL05		0x5188
#define OV9740_AWB_ADV_CTRL06		0x5189
#define OV9740_AWB_ADV_CTRL07		0x518A
#define OV9740_AWB_ADV_CTRL08		0x518B
#define OV9740_AWB_ADV_CTRL09		0x518C
#define OV9740_AWB_ADV_CTRL10		0x518D
#define OV9740_AWB_ADV_CTRL11		0x518E
#define OV9740_AWB_CTRL0F		0x518F
#define OV9740_AWB_CTRL10		0x5190
#define OV9740_AWB_CTRL11		0x5191
#define OV9740_AWB_CTRL12		0x5192
#define OV9740_AWB_CTRL13		0x5193
#define OV9740_AWB_CTRL14		0x5194

/* MIPI Control */
#define OV9740_MIPI_CTRL00		0x4800
#define OV9740_MIPI_3837		0x3837
#define OV9740_MIPI_CTRL01		0x4801
#define OV9740_MIPI_CTRL03		0x4803
#define OV9740_MIPI_CTRL05		0x4805
#define OV9740_VFIFO_RD_CTRL		0x4601
#define OV9740_MIPI_CTRL_3012		0x3012
#define OV9740_SC_CMMM_MIPI_CTR		0x3014

/* supported resolutions */
#define RES_1280x720_W			1280
#define RES_1280x720_H			720
#define RES_640x480_W			640
#define RES_640x480_H			480

/* Misc. structures */
struct ov9740_reg {
	u16				reg;
	u8				val;
};

struct ov9740_priv {
	struct v4l2_subdev		subdev;
	struct media_pad 		pad;
	struct ov9740_platform_data     *pdata;
	struct v4l2_mbus_framefmt	format;
	int				ident;
	u16				model;
	u8				revision;
	u8				manid;
	u8				smiaver;

	bool				flag_vflip;
	bool				flag_hflip;

	int 				power_count;
};

static const struct ov9740_reg ov9740_defaults[] = {
	/* Banding Filter */
	{ OV9740_AEC_B50_STEP_HI,	0x00},
	{ OV9740_AEC_B50_STEP_LO,	0xe8},
	{ OV9740_AEC_CTRL0E,		0x03},
	{ OV9740_AEC_MAXEXPO_50_H,	0x15},
	{ OV9740_AEC_MAXEXPO_50_L,	0xc6},
	{ OV9740_AEC_B60_STEP_HI,	0x00},
	{ OV9740_AEC_B60_STEP_LO,	0xc0},
	{ OV9740_AEC_CTRL0D,		0x04},
	{ OV9740_AEC_MAXEXPO_60_H,	0x18},
	{ OV9740_AEC_MAXEXPO_60_L,	0x20},

	/* LC */
	{ 0x5842, 0x02}, { 0x5843, 0x5e}, { 0x5844, 0x04}, { 0x5845, 0x32},
	{ 0x5846, 0x03}, { 0x5847, 0x29}, { 0x5848, 0x02}, { 0x5849, 0xcc},

	/* Un-documented OV9740 registers */
	{ 0x5800, 0x29}, { 0x5801, 0x25}, { 0x5802, 0x20}, { 0x5803, 0x21},
	{ 0x5804, 0x26}, { 0x5805, 0x2e}, { 0x5806, 0x11}, { 0x5807, 0x0c},
	{ 0x5808, 0x09}, { 0x5809, 0x0a}, { 0x580A, 0x0e}, { 0x580B, 0x16},
	{ 0x580C, 0x06}, { 0x580D, 0x02}, { 0x580E, 0x00}, { 0x580F, 0x00},
	{ 0x5810, 0x04}, { 0x5811, 0x0a}, { 0x5812, 0x05}, { 0x5813, 0x02},
	{ 0x5814, 0x00}, { 0x5815, 0x00}, { 0x5816, 0x03}, { 0x5817, 0x09},
	{ 0x5818, 0x0f}, { 0x5819, 0x0a}, { 0x581A, 0x07}, { 0x581B, 0x08},
	{ 0x581C, 0x0b}, { 0x581D, 0x14}, { 0x581E, 0x28}, { 0x581F, 0x23},
	{ 0x5820, 0x1d}, { 0x5821, 0x1e}, { 0x5822, 0x24}, { 0x5823, 0x2a},
	{ 0x5824, 0x4f}, { 0x5825, 0x6f}, { 0x5826, 0x5f}, { 0x5827, 0x7f},
	{ 0x5828, 0x9f}, { 0x5829, 0x5f}, { 0x582A, 0x8f}, { 0x582B, 0x9e},
	{ 0x582C, 0x8f}, { 0x582D, 0x9f}, { 0x582E, 0x4f}, { 0x582F, 0x87},
	{ 0x5830, 0x86}, { 0x5831, 0x97}, { 0x5832, 0xae}, { 0x5833, 0x3f},
	{ 0x5834, 0x8e}, { 0x5835, 0x7c}, { 0x5836, 0x7e}, { 0x5837, 0xaf},
	{ 0x5838, 0x8f}, { 0x5839, 0x8f}, { 0x583A, 0x9f}, { 0x583B, 0x7f},
	{ 0x583C, 0x5f},

	/* Y Gamma */
	{ 0x5480, 0x07}, { 0x5481, 0x18}, { 0x5482, 0x2c}, { 0x5483, 0x4e},
	{ 0x5484, 0x5e}, { 0x5485, 0x6b}, { 0x5486, 0x77}, { 0x5487, 0x82},
	{ 0x5488, 0x8c}, { 0x5489, 0x95}, { 0x548A, 0xa4}, { 0x548B, 0xb1},
	{ 0x548C, 0xc6}, { 0x548D, 0xd8}, { 0x548E, 0xe9},

	/* UV Gamma */
	{ 0x5490, 0x0f}, { 0x5491, 0xff}, { 0x5492, 0x0d}, { 0x5493, 0x05},
	{ 0x5494, 0x07}, { 0x5495, 0x1a}, { 0x5496, 0x04}, { 0x5497, 0x01},
	{ 0x5498, 0x03}, { 0x5499, 0x53}, { 0x549A, 0x02}, { 0x549B, 0xeb},
	{ 0x549C, 0x02}, { 0x549D, 0xa0}, { 0x549E, 0x02}, { 0x549F, 0x67},
	{ 0x54A0, 0x02}, { 0x54A1, 0x3b}, { 0x54A2, 0x02}, { 0x54A3, 0x18},
	{ 0x54A4, 0x01}, { 0x54A5, 0xe7}, { 0x54A6, 0x01}, { 0x54A7, 0xc3},
	{ 0x54A8, 0x01}, { 0x54A9, 0x94}, { 0x54AA, 0x01}, { 0x54AB, 0x72},
	{ 0x54AC, 0x01}, { 0x54AD, 0x57},

	/* AWB */
	{ OV9740_AWB_CTRL00,		0xf0},
	{ OV9740_AWB_CTRL01,		0x00},
	{ OV9740_AWB_CTRL02,		0x41},
	{ OV9740_AWB_CTRL03,		0x42},
	{ OV9740_AWB_ADV_CTRL01,	0x8a},
	{ OV9740_AWB_ADV_CTRL02,	0x61},
	{ OV9740_AWB_ADV_CTRL03,	0xce},
	{ OV9740_AWB_ADV_CTRL04,	0xa8},
	{ OV9740_AWB_ADV_CTRL05,	0x17},
	{ OV9740_AWB_ADV_CTRL06,	0x1f},
	{ OV9740_AWB_ADV_CTRL07,	0x27},
	{ OV9740_AWB_ADV_CTRL08,	0x41},
	{ OV9740_AWB_ADV_CTRL09,	0x34},
	{ OV9740_AWB_ADV_CTRL10,	0xf0},
	{ OV9740_AWB_ADV_CTRL11,	0x10},
	{ OV9740_AWB_CTRL0F,		0xff},
	{ OV9740_AWB_CTRL10,		0x00},
	{ OV9740_AWB_CTRL11,		0xff},
	{ OV9740_AWB_CTRL12,		0x00},
	{ OV9740_AWB_CTRL13,		0xff},
	{ OV9740_AWB_CTRL14,		0x00},

	/* CIP */
	{ 0x530D, 0x12},

	/* CMX */
	{ 0x5380, 0x01}, { 0x5381, 0x00}, { 0x5382, 0x00}, { 0x5383, 0x17},
	{ 0x5384, 0x00}, { 0x5385, 0x01}, { 0x5386, 0x00}, { 0x5387, 0x00},
	{ 0x5388, 0x00}, { 0x5389, 0xe0}, { 0x538A, 0x00}, { 0x538B, 0x20},
	{ 0x538C, 0x00}, { 0x538D, 0x00}, { 0x538E, 0x00}, { 0x538F, 0x16},
	{ 0x5390, 0x00}, { 0x5391, 0x9c}, { 0x5392, 0x00}, { 0x5393, 0xa0},
	{ 0x5394, 0x18},

	/* 50/60 Detection */
	{ 0x3C0A, 0x9c}, { 0x3C0B, 0x3f},

	/* Output Select */
	{ OV9740_IO_OUTPUT_SEL01,	0x00},
	{ OV9740_IO_OUTPUT_SEL02,	0x00},
	{ OV9740_IO_CREL00,		0x00},
	{ OV9740_IO_CREL01,		0x00},
	{ OV9740_IO_CREL02,		0x00},

	/* AWB Control */
	{ OV9740_AWB_MANUAL_CTRL,	0x00},

	/* Analog Control */
	{ OV9740_ANALOG_CTRL03,		0xaa},
	{ OV9740_ANALOG_CTRL32,		0x2f},
	{ OV9740_ANALOG_CTRL20,		0x66},
	{ OV9740_ANALOG_CTRL21,		0xc0},
	{ OV9740_ANALOG_CTRL31,		0x52},
	{ OV9740_ANALOG_CTRL33,		0x50},
	{ OV9740_ANALOG_CTRL30,		0xca},
	{ OV9740_ANALOG_CTRL04,		0x0c},
	{ OV9740_ANALOG_CTRL01,		0x40},
	{ OV9740_ANALOG_CTRL02,		0x16},
	{ OV9740_ANALOG_CTRL10,		0xa1},
	{ OV9740_ANALOG_CTRL12,		0x24},
	{ OV9740_ANALOG_CTRL22,		0x9f},

	/* Sensor Control */
	{ OV9740_SENSOR_CTRL03,		0x42},
	{ OV9740_SENSOR_CTRL04,		0x10},
	{ OV9740_SENSOR_CTRL05,		0x45},
	{ OV9740_SENSOR_CTRL07,		0x14},

	/* Timing Control */
	{ OV9740_TIMING_CTRL33,		0x04},
	{ OV9740_TIMING_CTRL35,		0x02},
	{ OV9740_TIMING_CTRL19,		0x6e},
	{ OV9740_TIMING_CTRL17,		0x94},

	/* AEC/AGC Control */
	{ OV9740_AEC_ENABLE,		0x10},
	{ OV9740_GAIN_CEILING_01,	0x00},
	{ OV9740_GAIN_CEILING_02,	0x7f},
	{ OV9740_AEC_HI_THRESHOLD,	0xa0},
	{ OV9740_AEC_3A1A,		0x05},
	{ OV9740_AEC_CTRL1B_WPT2,	0x50},
	{ OV9740_AEC_CTRL0F_WPT,	0x50},
	{ OV9740_AEC_CTRL10_BPT,	0x4c},
	{ OV9740_AEC_CTRL1E_BPT2,	0x4c},
	{ OV9740_AEC_LO_THRESHOLD,	0x26},

	/* BLC Control */
	{ OV9740_BLC_AUTO_ENABLE,	0x45},
	{ OV9740_BLC_MODE,		0x18},

	/* DVP Control */
	{ OV9740_DVP_VSYNC_CTRL02,	0x04},
	{ OV9740_DVP_VSYNC_MODE,	0x00},
	{ OV9740_DVP_VSYNC_CTRL06,	0x08},

	/* PLL Setting */
	{ OV9740_PLL_MODE_CTRL01,	0x20},
	{ OV9740_PRE_PLL_CLK_DIV,	0x03},
	{ OV9740_PLL_MULTIPLIER,	0x4c},
	{ OV9740_VT_SYS_CLK_DIV,	0x01},
	{ OV9740_VT_PIX_CLK_DIV,	0x08},
	{ OV9740_PLL_CTRL3010,		0x01},
	{ OV9740_VFIFO_CTRL00,		0x82},

	/* Timing Setting */
	/*VTS*/
	{ OV9740_FRM_LENGTH_LN_HI,	0x03},
	{ OV9740_FRM_LENGTH_LN_LO,	0x07},
	/*HTS*/
	{ OV9740_LN_LENGTH_PCK_HI,	0x06},
	{ OV9740_LN_LENGTH_PCK_LO,	0x62},

	/* MIPI Control */
	{ OV9740_MIPI_CTRL00,		0x4C},
	{ OV9740_MIPI_3837,		0x01},
	{ OV9740_MIPI_CTRL01,		0x0f},
	{ OV9740_MIPI_CTRL03,		0x05},
	{ OV9740_MIPI_CTRL05,		0x10},
	{ OV9740_VFIFO_RD_CTRL,		0x16},
	{ OV9740_MIPI_CTRL_3012,	0x70},
	{ OV9740_SC_CMMM_MIPI_CTR,	0x01},
};

static const struct ov9740_reg ov9740_start_streaming[] = {
	/* Start Streaming */
	{ OV9740_MODE_SELECT,		0x01},
};

static const struct ov9740_reg ov9740_stop_streaming[] = {
	/* Software Reset */
	{ OV9740_SOFTWARE_RESET,	0x01},

	/* Setting Streaming to Standby */
	{ OV9740_MODE_SELECT,		0x00},
};

static const struct ov9740_reg ov9740_regs_640x480[] = {
	{ OV9740_X_ADDR_START_HI,	0x00},
	{ OV9740_X_ADDR_START_LO,	0xa0},
	{ OV9740_Y_ADDR_START_HI,	0x00},
	{ OV9740_Y_ADDR_START_LO,	0x00},
	{ OV9740_X_ADDR_END_HI,		0x04},
	{ OV9740_X_ADDR_END_LO,		0x63},
	{ OV9740_Y_ADDR_END_HI,		0x02},
	{ OV9740_Y_ADDR_END_LO,		0xd3},
	{ OV9740_X_OUTPUT_SIZE_HI,	0x02},
	{ OV9740_X_OUTPUT_SIZE_LO,	0x80},
	{ OV9740_Y_OUTPUT_SIZE_HI,	0x01},
	{ OV9740_Y_OUTPUT_SIZE_LO,	0xe0},
	{ OV9740_ISP_CTRL1E,		0x03},
	{ OV9740_ISP_CTRL1F,		0xc0},
	{ OV9740_ISP_CTRL20,		0x02},
	{ OV9740_ISP_CTRL21,		0xd0},
	{ OV9740_VFIFO_READ_START_HI,	0x01},
	{ OV9740_VFIFO_READ_START_LO,	0x40},
	{ OV9740_ISP_CTRL00,		0xff},
	{ OV9740_ISP_CTRL01,		0xff},
	{ OV9740_ISP_CTRL03,		0xff},
};

static const struct ov9740_reg ov9740_regs_1280x720[] = {
	{ OV9740_X_ADDR_START_HI,	0x00},
	{ OV9740_X_ADDR_START_LO,	0x00},
	{ OV9740_Y_ADDR_START_HI,	0x00},
	{ OV9740_Y_ADDR_START_LO,	0x00},
	{ OV9740_X_ADDR_END_HI,		0x05},
	{ OV9740_X_ADDR_END_LO,		0x03},
	{ OV9740_Y_ADDR_END_HI,		0x02},
	{ OV9740_Y_ADDR_END_LO,		0xd3},
	{ OV9740_X_OUTPUT_SIZE_HI,	0x05},
	{ OV9740_X_OUTPUT_SIZE_LO,	0x00},
	{ OV9740_Y_OUTPUT_SIZE_HI,	0x02},
	{ OV9740_Y_OUTPUT_SIZE_LO,	0xd0},
	{ OV9740_ISP_CTRL1E,		0x05},
	{ OV9740_ISP_CTRL1F,		0x00},
	{ OV9740_ISP_CTRL20,		0x02},
	{ OV9740_ISP_CTRL21,		0xd0},
	{ OV9740_VFIFO_READ_START_HI,	0x02},
	{ OV9740_VFIFO_READ_START_LO,	0x30},
	{ OV9740_ISP_CTRL00,		0xff},
	{ OV9740_ISP_CTRL01,		0xef},
	{ OV9740_ISP_CTRL03,		0xff},
};

static enum v4l2_mbus_pixelcode ov9740_codes[] = {
	V4L2_MBUS_FMT_UYVY8_2X8,
};

static struct v4l2_subdev_frame_size_enum ov9740_sizes[] = {
	{
		.index 		= 0,
		.pad   		= 0,
		.code		= V4L2_MBUS_FMT_UYVY8_2X8,
		.min_width 	= 640,
		.min_height 	= 480,
		.max_width 	= 640,
		.max_height 	= 480,
	},
	{
		.index 		= 1,
		.pad   		= 0,
		.code		= V4L2_MBUS_FMT_UYVY8_2X8,
		.min_width 	= 1280,
		.min_height 	= 720,
		.max_width 	= 1280,
		.max_height 	= 720,
	},
};

static const struct v4l2_queryctrl ov9740_controls[] = {
	{
		.id		= V4L2_CID_VFLIP,
		.type		= V4L2_CTRL_TYPE_BOOLEAN,
		.name		= "Flip Vertically",
		.minimum	= 0,
		.maximum	= 1,
		.step		= 1,
		.default_value	= 0,
	},
	{
		.id		= V4L2_CID_HFLIP,
		.type		= V4L2_CTRL_TYPE_BOOLEAN,
		.name		= "Flip Horizontally",
		.minimum	= 0,
		.maximum	= 1,
		.step		= 1,
		.default_value	= 0,
	},
};

/* read a register */
static int ov9740_reg_read(struct i2c_client *client, u16 reg, u8 *val)
{
	int ret;
	struct i2c_msg msg[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 2,
			.buf	= (u8*)&reg,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= val,
		},
	};

	reg = swab16(reg);

	ret = i2c_transfer(client->adapter, msg, 2);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev, "Failed reading register 0x%04x!\n", reg);
		return ret;
	}

	return 0;
}

static int ov9740_read_reg(struct i2c_client *client, u16 data_length, u16 reg,
			   u32 *val)
{
	int err;
	struct i2c_msg msg[1];
	unsigned char data[4] = {0};

	if (!client->adapter)
		return -ENODEV;
	if (data_length != 1 && data_length != 2
			&& data_length != 4)
		return -EINVAL;

	msg->addr = client->addr;
	msg->flags = 0;
	msg->len = 2;
	msg->buf = data;

	/* Write addr - high byte goes out first */
	data[0] = (u8) (reg >> 8);;
	data[1] = (u8) (reg & 0xff);
	err = i2c_transfer(client->adapter, msg, 1);

	/* Read back data */
	if (err >= 0) {
		msg->len = data_length;
		msg->flags = I2C_M_RD;
		err = i2c_transfer(client->adapter, msg, 1);
	}
	if (err >= 0) {
		*val = 0;
		/* high byte comes first */
		if (data_length == 1)
			*val = data[0];
		else if (data_length == 2)
			*val = data[1] + (data[0] << 8);
		else
			*val = data[3] + (data[2] << 8) +
				(data[1] << 16) + (data[0] << 24);
		return 0;
	}
	v4l_err(client, "read from offset 0x%x error %d", reg, err);
	return err;
}

/* write a register */
static int ov9740_reg_write(struct i2c_client *client, u16 reg, u8 val)
{
	struct i2c_msg msg;
	u8 buf[3];
	int ret;

	reg = swab16(reg);

	memcpy(buf + 0, &reg, 2);
	memcpy(buf + 2, &val, 1);

	msg.addr	= client->addr;
	msg.flags	= 0;
	msg.len		= 3;
	msg.buf		= buf;

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev, "Failed writing register 0x%04x!\n", reg);
		return ret;
	}

	return 0;
}


/* Read a register, alter its bits, write it back */
static int ov9740_reg_rmw(struct i2c_client *client, u16 reg, u8 set, u8 unset)
{
	u8 val;
	int ret;

	ret = ov9740_reg_read(client, reg, &val);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev,
			"[Read]-Modify-Write of register %02x failed!\n", reg);
		return ret;
	}

	val |= set;
	val &= ~unset;

	ret = ov9740_reg_write(client, reg, val);
	if (IS_ERR_VALUE(ret)) {
		dev_err(&client->dev,
			"Read-Modify-[Write] of register %02x failed!\n", reg);
		return ret;
	}

	return 0;
}

static int ov9740_reg_write_ary(struct i2c_client *client,
				const struct ov9740_reg *regary, int regarylen)
{
	int i;
	int ret;

	for (i = 0; i < regarylen; i++) {
		ret = ov9740_reg_write(client, regary[i].reg, regary[i].val);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	return 0;
}

/* Start/Stop streaming from the device */
static int ov9740_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov9740_priv *priv = to_ov9740_sensor(sd);
	int ret;

	/* Program orientation register. */
	if (priv->flag_vflip)
		ret = ov9740_reg_rmw(client, OV9740_IMAGE_ORT, 0x2, 0);
	else
		ret = ov9740_reg_rmw(client, OV9740_IMAGE_ORT, 0, 0x2);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (priv->flag_hflip)
		ret = ov9740_reg_rmw(client, OV9740_IMAGE_ORT, 0x1, 0);
	else
		ret = ov9740_reg_rmw(client, OV9740_IMAGE_ORT, 0, 0x1);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (enable) {
		dev_info(&client->dev, "Enabling Streaming\n");
		ret = ov9740_reg_write_ary(client, ov9740_start_streaming,
					   ARRAY_SIZE(ov9740_start_streaming));
	} else {
		dev_info(&client->dev, "Disabling Streaming\n");
		ret = ov9740_reg_write_ary(client, ov9740_stop_streaming,
					   ARRAY_SIZE(ov9740_stop_streaming));
	}

	return ret;
}



/* Get status of additional camera capabilities */
static int ov9740_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct ov9740_priv *priv = to_ov9740_sensor(sd);

	switch (ctrl->id) {
	case V4L2_CID_VFLIP:
		ctrl->value = priv->flag_vflip;
		break;
	case V4L2_CID_HFLIP:
		ctrl->value = priv->flag_hflip;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Set status of additional camera capabilities */
static int ov9740_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct ov9740_priv *priv = to_ov9740_sensor(sd);

	switch (ctrl->id) {
	case V4L2_CID_VFLIP:
		priv->flag_vflip = ctrl->value;
		break;
	case V4L2_CID_HFLIP:
		priv->flag_hflip = ctrl->value;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Get chip identification */
static int ov9740_g_chip_ident(struct v4l2_subdev *sd,
			       struct v4l2_dbg_chip_ident *id)
{
	struct ov9740_priv *priv = to_ov9740_sensor(sd);

	id->ident = priv->ident;
	id->revision = priv->revision;

	return 0;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int ov9740_get_register(struct v4l2_subdev *sd,
			       struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u8 val;

	if (reg->reg & ~0xffff)
		return -EINVAL;

	reg->size = 2;

	ret = ov9740_reg_read(client, reg->reg, &val);
	if (ret)
		return ret;

	reg->val = (__u64)val;

	return ret;
}

static int ov9740_set_register(struct v4l2_subdev *sd,
			       struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (reg->reg & ~0xffff || reg->val & ~0xff)
		return -EINVAL;

	return ov9740_reg_write(client, reg->reg, reg->val);
}
#endif

/* select nearest higher resolution for capture */
static int ov9740_res_roundup(u32 *width, u32 *height)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ov9740_sizes); i++)
		if (ov9740_sizes[i].max_width >= *width &&
			ov9740_sizes[i].max_height >= *height) {
			*width = ov9740_sizes[i].max_width;
			*height = ov9740_sizes[i].max_height;
			return i;
		}

	*width = ov9740_sizes[ARRAY_SIZE(ov9740_sizes) - 1].max_width;
	*height = ov9740_sizes[ARRAY_SIZE(ov9740_sizes) - 1].max_height;

	return ARRAY_SIZE(ov9740_sizes) - 1;
}

/* Setup registers according to resolution and color encoding */
static int ov9740_set_res_code(struct i2c_client *client, u32 width,
			       enum v4l2_mbus_pixelcode code)
{
	int ret;

	/* select register configuration for given resolution */
	switch (width) {
	case RES_640x480_W:
		dev_info(&client->dev, "Setting image size to 640x480\n");
		ret = ov9740_reg_write_ary(client, ov9740_regs_640x480,
					   ARRAY_SIZE(ov9740_regs_640x480));
		break;
	case RES_1280x720_W:
		dev_info(&client->dev, "Setting image size to 1280x720\n");
		ret = ov9740_reg_write_ary(client, ov9740_regs_1280x720,
					   ARRAY_SIZE(ov9740_regs_1280x720));
		break;
	default:
		dev_err(&client->dev, "Failed to select resolution!\n");
		return -EINVAL;
	}

	return ret;
}

/* set the format we will capture in */
static int ov9740_s_fmt(struct v4l2_subdev *sd,
			struct v4l2_mbus_framefmt *mf)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	enum v4l2_colorspace cspace;
	enum v4l2_mbus_pixelcode code = mf->code;
	int ret;

	ov9740_res_roundup(&mf->width, &mf->height);

	printk(KERN_WARNING, "ov9740 %x\n", code);

	switch (code) {
	case V4L2_MBUS_FMT_UYVY8_2X8:
		cspace = V4L2_COLORSPACE_SRGB;
		break;
	default:
		return -EINVAL;
	}

	ret = ov9740_reg_write_ary(client, ov9740_defaults,
				   ARRAY_SIZE(ov9740_defaults));
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = ov9740_set_res_code(client, mf->width, code);
	if (IS_ERR_VALUE(ret))
		return ret;

	mf->code	= code;
	mf->colorspace	= cspace;

	return ret;
}

static int ov9740_try_fmt(struct v4l2_subdev *sd,
			  struct v4l2_mbus_framefmt *mf)
{
	ov9740_res_roundup(&mf->width, &mf->height);

	mf->field = V4L2_FIELD_NONE;

	printk(KERN_WARNING, "ov9740 try %x\n", mf->code);

	switch (mf->code) {
	case V4L2_MBUS_FMT_UYVY8_2X8:
		mf->colorspace = V4L2_COLORSPACE_SRGB;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 subdev video operations
 */

static int ov9740_pad_enum_mbus_code(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_mbus_code_enum *code_enum)
{
	if (code_enum->index >= ARRAY_SIZE(ov9740_codes))
		return -EINVAL;

	code_enum->pad  = 0;
	code_enum->code = ov9740_codes[code_enum->index];

	return 0;
}

static int ov9740_pad_enum_frame_size(struct v4l2_subdev *subdev,
				  struct v4l2_subdev_fh *fh,
				  struct v4l2_subdev_frame_size_enum *fse)
{
	if (fse->pad != 0)
		return -EINVAL;

	if (fse->index >= ARRAY_SIZE(ov9740_sizes))
		return -EINVAL;

	if (fse->code != ov9740_codes[fse->index])
		return -EINVAL;

	*fse = ov9740_sizes[fse->index];

	return 0;
}

static struct v4l2_mbus_framefmt *
__ov9740_pad_get_format(struct ov9740_priv *sensor, struct v4l2_subdev_fh *fh,
			unsigned int pad, enum v4l2_subdev_format_whence which)
{
	if (pad != 0)
		return NULL;

	switch (which) {
	case V4L2_SUBDEV_FORMAT_TRY:
		return v4l2_subdev_get_try_format(fh, pad);
	case V4L2_SUBDEV_FORMAT_ACTIVE:
		return &sensor->format;
	default:
		return NULL;
	}
}

static int ov9740_pad_get_format(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_format *fmt)
{
	struct ov9740_priv *sensor = to_ov9740_sensor(subdev);
	struct v4l2_mbus_framefmt *format;

	format = __ov9740_pad_get_format(sensor, fh, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	fmt->format = *format;
	return 0;
}

static int ov9740_pad_set_format(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_format *fmt)
{
	struct ov9740_priv *sensor = to_ov9740_sensor(subdev);
	struct v4l2_mbus_framefmt *format;
	int ret;

	format = __ov9740_pad_get_format(sensor, fh, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	ret = ov9740_try_fmt(subdev, &fmt->format);
	if (ret)
		return ret;

	if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE) {

		/* TODO: please check */
		sensor->pdata->csi_configure(subdev, 0);

		ret = ov9740_s_fmt(subdev, &fmt->format);
		if (ret)
			return ret;

		sensor->format = fmt->format;
	}

	return 0;
}

static int ov9740_set_power(struct v4l2_subdev *subdev, int on)
{
	struct ov9740_priv *sensor = to_ov9740_sensor(subdev);
	struct ov9740_platform_data *pdata = sensor->pdata;
	int ret = 0;

	if (!pdata || !pdata->set_power || !pdata->set_xclk)
		return -EINVAL;

	/* If the power count is modified from 0 to != 0 or from != 0 to 0,
	 * update the power state.
	 */
	if (sensor->power_count == !on) {
		if (on) {
			pdata->set_xclk(subdev, pdata->ext_clk);
			udelay(1000);
			ret = pdata->set_power(subdev, 1);

		} else {
			ret = pdata->set_power(subdev, 0);
			udelay(100);
			pdata->set_xclk(subdev, 0);
		}

		if (ret < 0)
			goto done;
	}

	/* Update the power count. */
	sensor->power_count += on ? 1 : -1;
	WARN_ON(sensor->power_count < 0);

done:
	return ret;
}

static int ov9740_detect(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	u32 model_id, mfr_id, rev;

	if (!client)
		return -ENODEV;

	if (ov9740_read_reg(client, 2, OV9740_MODEL_ID_HI, &model_id))
		return -ENODEV;
	if (ov9740_read_reg(client, 1, OV9740_MANUFACTURER_ID, &mfr_id))
		return -ENODEV;
	if (ov9740_read_reg(client, 1, OV9740_REVISION_NUMBER, &rev))
		return -ENODEV;

	v4l_info(client, "model id detected 0x%x mfr 0x%x, rev# 0x%x\n",
							model_id, mfr_id, rev);
	if (model_id != 0x9740) {
		/* We didn't read the values we expected, so
		 * this must not be an OV9740.
		 */
		v4l_warn(client, "model id mismatch 0x%x mfr 0x%x\n",
							model_id, mfr_id);

		return -ENODEV;
	}
	return 0;
}

static int ov9740_registered(struct v4l2_subdev *subdev)
{
	struct i2c_client *client = v4l2_get_subdevdata(subdev);
	int ret;

	ret = ov9740_set_power(subdev, 1);
	if (ret) {
		v4l_warn(client, "Power on failed\n");
		return ret;
	}

	ret = ov9740_detect(subdev);

	ov9740_set_power(subdev, 0);

	return ret;
}

static int ov9740_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct ov9740_priv *sensor = to_ov9740_sensor(sd);
	struct v4l2_mbus_framefmt *format;

	format = __ov9740_pad_get_format(sensor, fh, 0, V4L2_SUBDEV_FORMAT_TRY);
	if (!format)
		return -EINVAL;

	memset(format, 0, sizeof(*format));

	format->code = V4L2_MBUS_FMT_UYVY8_2X8;

	ov9740_try_fmt(sd, format);

	return ov9740_set_power(sd, 1);
}

static int ov9740_close(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	return ov9740_set_power(sd, 0);
}


static struct v4l2_subdev_core_ops ov9740_core_ops = {
	.s_power		= ov9740_set_power,
	.g_ctrl			= ov9740_g_ctrl,
	.s_ctrl			= ov9740_s_ctrl,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register		= ov9740_get_register,
	.s_register		= ov9740_set_register,
#endif
};

static const struct v4l2_subdev_pad_ops ov9740_pad_ops = {
	.enum_mbus_code 	= ov9740_pad_enum_mbus_code,
	.enum_frame_size 	= ov9740_pad_enum_frame_size,
	.get_fmt 		= ov9740_pad_get_format,
	.set_fmt 		= ov9740_pad_set_format,
};

static struct v4l2_subdev_video_ops ov9740_video_ops = {
	.s_stream		= ov9740_s_stream,

};

static struct v4l2_subdev_ops ov9740_subdev_ops = {
	.core			= &ov9740_core_ops,
	.video			= &ov9740_video_ops,
	.pad 			= &ov9740_pad_ops,
};

static const struct v4l2_subdev_internal_ops ov9740_internal_ops = {
	.registered 		= ov9740_registered,
	.open 			= ov9740_open,
	.close 			= ov9740_close,
};

/*
 * i2c_driver function
 */
static int __devinit ov9740_probe(struct i2c_client *client,
				  const struct i2c_device_id *did)
{
	struct ov9740_platform_data *pdata;
	struct v4l2_mbus_framefmt *format;
	struct ov9740_priv *sensor;
	int rval;

	pdata = (struct ov9740_platform_data *) client->dev.platform_data;

	if (pdata == NULL)
		return -ENODEV;

	sensor = kzalloc(sizeof(*sensor), GFP_KERNEL);
	if (sensor == NULL)
		return -ENOMEM;

	sensor->pdata = pdata;

	v4l2_i2c_subdev_init(&sensor->subdev, client, &ov9740_subdev_ops);

	format = __ov9740_pad_get_format(sensor, NULL, 0,
					 V4L2_SUBDEV_FORMAT_ACTIVE);

	memset(format, 0, sizeof(*format));

	format->code = V4L2_MBUS_FMT_UYVY8_2X8;

	ov9740_try_fmt(&sensor->subdev, format);

	sensor->subdev.internal_ops = &ov9740_internal_ops;
	sensor->subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

	sensor->pad.flags = MEDIA_PAD_FL_SOURCE;
	rval = media_entity_init(&sensor->subdev.entity, 1, &sensor->pad, 0);
	if (rval < 0)
		kfree(sensor);

	return rval;
}

static int __devexit ov9740_remove(struct i2c_client *client)
{
	struct v4l2_subdev *subdev = i2c_get_clientdata(client);
	struct ov9740_priv *sensor = to_ov9740_sensor(subdev);

	if (sensor->power_count && sensor->pdata) {
		if (sensor->pdata->set_power)
			sensor->pdata->set_power(&sensor->subdev, 0);

		sensor->pdata->set_xclk(&sensor->subdev, 0);
		sensor->power_count = 0;
	}

	media_entity_cleanup(&sensor->subdev.entity);
	v4l2_device_unregister_subdev(subdev);
	kfree(sensor);

	return 0;
}

static const struct i2c_device_id ov9740_id[] = {
	{ "ov9740", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ov9740_id);

static struct i2c_driver ov9740_i2c_driver = {
	.driver = {
		.name = "ov9740",
	},
	.probe    = ov9740_probe,
	.remove   = __devexit_p(ov9740_remove),
	.id_table = ov9740_id,
};

static int __init ov9740_module_init(void)
{
	return i2c_add_driver(&ov9740_i2c_driver);
}

static void __exit ov9740_module_exit(void)
{
	i2c_del_driver(&ov9740_i2c_driver);
}

module_init(ov9740_module_init);
module_exit(ov9740_module_exit);

MODULE_DESCRIPTION("SoC Camera driver for OmniVision OV9740");
MODULE_AUTHOR("Andrew Chew <achew@nvidia.com>");
MODULE_LICENSE("GPL v2");
