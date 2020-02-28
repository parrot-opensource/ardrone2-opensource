/*
 * drivers/media/video/ov9740_int.c
 *
 * OV 9740 camera driver for v4l2-int-device
 *
 * Based on imx046.c and ov9740.c 
 *	
 * author: Julien BERAUD
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/i2c.h>
#include <linux/delay.h>

#include <media/v4l2-int-device.h>
#include <media/ov9740_int.h>

#include "ov9740_regs.h"
#include "isp/isp.h"
#include "isp/ispcsi2.h"


#define OV9740_DRIVER_NAME  "ov9740"
#define OV9740_MOD_NAME "OV9740: "

#define I2C_M_WR 0

#if 0
static const struct ov9740_reg ov9740_defaults[] = {

	/* Output Select */
	{ OV9740_IO_OUTPUT_SEL01,	0x00, I2C_8BIT},
	{ OV9740_IO_OUTPUT_SEL02,	0x00, I2C_8BIT},
	{ OV9740_IO_CREL00,		0xe8, I2C_8BIT},
	{ OV9740_IO_CREL01,		0x03, I2C_8BIT},
	{ OV9740_IO_CREL02,		0xff, I2C_8BIT},
	
	/* MIPI Control */
//	{ OV9740_MIPI_2_LANE,	0x40, I2C_8BIT},
	{ OV9740_MIPI_3837,		0x01, I2C_8BIT},
	{ OV9740_MIPI_CTRL01,		0x0f, I2C_8BIT},
	{ OV9740_MIPI_CTRL03,		0x05, I2C_8BIT},
	{ OV9740_MIPI_CTRL05,		0x10, I2C_8BIT},
	{ OV9740_VFIFO_RD_CTRL,		0x16, I2C_8BIT},
	{ OV9740_MIPI_CTRL_3012,	0x70, I2C_8BIT},
	{ OV9740_SC_CMMM_MIPI_CTR,	0x01, I2C_8BIT},

	/* Banding Filter */
	{ OV9740_AEC_B50_STEP,		0x00e8, I2C_16BIT},
	{ OV9740_AEC_CTRL0E,		0x03,	I2C_8BIT},
	{ OV9740_AEC_MAXEXPO_50,	0x15c6, I2C_16BIT},
	{ OV9740_AEC_B60_STEP,		0x00c0, I2C_16BIT},
	{ OV9740_AEC_CTRL0D,		0x04,	I2C_8BIT},
	{ OV9740_AEC_MAXEXPO_60,	0x1820, I2C_16BIT},

    /* ISP settings */
    { OV9740_ISP_CTRL00,    0x01,	I2C_8BIT},
    { OV9740_ISP_CTRL01,    0x00,	I2C_8BIT},
    { OV9740_ISP_CTRL04,    0x00,	I2C_8BIT},
    
    /* Sensor Control */
	{ OV9740_SENSOR_CTRL03,		0x42, I2C_8BIT},
	{ OV9740_SENSOR_CTRL04,		0x10, I2C_8BIT},
	{ OV9740_SENSOR_CTRL05,		0x45, I2C_8BIT},
	{ OV9740_SENSOR_CTRL07,		0x14, I2C_8BIT},
	
    /* Analog Control */
	{ OV9740_ANALOG_CTRL03,		0xaa, I2C_8BIT},
	{ OV9740_ANALOG_CTRL32,		0x2f, I2C_8BIT},
	{ OV9740_ANALOG_CTRL20,		0x66, I2C_8BIT},
	{ OV9740_ANALOG_CTRL21,		0xc0, I2C_8BIT},
	/* to move after */
    
	{ OV9740_ANALOG_CTRL22,		0x9f, I2C_8BIT},

    {OV9740_COARSE_INT_TIME,    0x0343, I2C_16BIT},

	/* Timing Control */
	{ OV9740_TIMING_CTRL33,		0x04, I2C_8BIT},
	{ OV9740_TIMING_CTRL35,		0x02, I2C_8BIT},

	/* DVP Control */
	{ OV9740_DVP_VSYNC_CTRL02,	0x04, I2C_8BIT},
	{ OV9740_DVP_VSYNC_MODE,	0x00, I2C_8BIT},
	{ OV9740_DVP_VSYNC_CTRL06,	0x08, I2C_8BIT},

	{ OV9740_TIMING_CTRL19,		0x6e, I2C_8BIT},
	{ OV9740_TIMING_CTRL17,		0x94, I2C_8BIT},
    
    /* LC */
    { 0x5841, 0x04, I2C_8BIT},
	{ 0x5842, 0x02, I2C_8BIT}, { 0x5843, 0x5e, I2C_8BIT},
	{ 0x5844, 0x04, I2C_8BIT}, { 0x5845, 0x32, I2C_8BIT},
	{ 0x5846, 0x03, I2C_8BIT}, { 0x5847, 0x29, I2C_8BIT},
	{ 0x5848, 0x02, I2C_8BIT}, { 0x5849, 0xcc, I2C_8BIT},

	/* Un-documented OV9740 registers */
	{ 0x5800, 0x29, I2C_8BIT}, { 0x5801, 0x25, I2C_8BIT},
	{ 0x5802, 0x20, I2C_8BIT}, { 0x5803, 0x21, I2C_8BIT},
	{ 0x5804, 0x26, I2C_8BIT}, { 0x5805, 0x2e, I2C_8BIT},
	{ 0x5806, 0x11,	I2C_8BIT}, { 0x5807, 0x0c, I2C_8BIT},
	{ 0x5808, 0x09, I2C_8BIT}, { 0x5809, 0x0a, I2C_8BIT},
	{ 0x580A, 0x0e, I2C_8BIT}, { 0x580B, 0x16, I2C_8BIT},
	{ 0x580C, 0x06, I2C_8BIT}, { 0x580D, 0x02, I2C_8BIT},
	{ 0x580E, 0x00, I2C_8BIT}, { 0x580F, 0x00, I2C_8BIT},
	{ 0x5810, 0x04, I2C_8BIT}, { 0x5811, 0x0a, I2C_8BIT},
	{ 0x5812, 0x05, I2C_8BIT}, { 0x5813, 0x02, I2C_8BIT},
	{ 0x5814, 0x00, I2C_8BIT}, { 0x5815, 0x00, I2C_8BIT},
	{ 0x5816, 0x03,	I2C_8BIT}, { 0x5817, 0x09, I2C_8BIT},
	{ 0x5818, 0x0f, I2C_8BIT}, { 0x5819, 0x0a, I2C_8BIT},
	{ 0x581A, 0x07, I2C_8BIT}, { 0x581B, 0x08, I2C_8BIT},
	{ 0x581C, 0x0b, I2C_8BIT}, { 0x581D, 0x14, I2C_8BIT},
	{ 0x581E, 0x28, I2C_8BIT}, { 0x581F, 0x23, I2C_8BIT},
	{ 0x5820, 0x1d, I2C_8BIT}, { 0x5821, 0x1e, I2C_8BIT}, 
	{ 0x5822, 0x24, I2C_8BIT}, { 0x5823, 0x2a, I2C_8BIT},
	{ 0x5824, 0x4f, I2C_8BIT}, { 0x5825, 0x6f, I2C_8BIT},
	{ 0x5826, 0x5f, I2C_8BIT}, { 0x5827, 0x7f, I2C_8BIT},
	{ 0x5828, 0x9f, I2C_8BIT}, { 0x5829, 0x5f, I2C_8BIT},
	{ 0x582A, 0x8f, I2C_8BIT}, { 0x582B, 0x9e, I2C_8BIT},
	{ 0x582C, 0x8f, I2C_8BIT}, { 0x582D, 0x9f, I2C_8BIT},
	{ 0x582E, 0x4f, I2C_8BIT}, { 0x582F, 0x87, I2C_8BIT},
	{ 0x5830, 0x86, I2C_8BIT}, { 0x5831, 0x97, I2C_8BIT},
	{ 0x5832, 0xae, I2C_8BIT}, { 0x5833, 0x3f, I2C_8BIT},
	{ 0x5834, 0x8e, I2C_8BIT}, { 0x5835, 0x7c, I2C_8BIT},
	{ 0x5836, 0x7e, I2C_8BIT}, { 0x5837, 0xaf, I2C_8BIT},
	{ 0x5838, 0x8f, I2C_8BIT}, { 0x5839, 0x8f, I2C_8BIT},
	{ 0x583A, 0x9f, I2C_8BIT}, { 0x583B, 0x7f, I2C_8BIT},
	{ 0x583C, 0x5f, I2C_8BIT},

	/* Y Gamma */
	{ 0x5480, 0x07, I2C_8BIT}, { 0x5481, 0x18, I2C_8BIT},
	{ 0x5482, 0x2c, I2C_8BIT}, { 0x5483, 0x4e, I2C_8BIT},
	{ 0x5484, 0x5e, I2C_8BIT}, { 0x5485, 0x6b, I2C_8BIT},
	{ 0x5486, 0x77, I2C_8BIT}, { 0x5487, 0x82, I2C_8BIT},
	{ 0x5488, 0x8c, I2C_8BIT}, { 0x5489, 0x95, I2C_8BIT},
	{ 0x548A, 0xa4, I2C_8BIT}, { 0x548B, 0xb1, I2C_8BIT},
	{ 0x548C, 0xc6, I2C_8BIT}, { 0x548D, 0xd8, I2C_8BIT},
	{ 0x548E, 0xe9, I2C_8BIT},

	/* UV Gamma */
	{ 0x5490, 0x0f, I2C_8BIT}, { 0x5491, 0xff, I2C_8BIT},
	{ 0x5492, 0x0d, I2C_8BIT}, { 0x5493, 0x05, I2C_8BIT},
	{ 0x5494, 0x07, I2C_8BIT}, { 0x5495, 0x1a, I2C_8BIT},
	{ 0x5496, 0x04, I2C_8BIT}, { 0x5497, 0x01, I2C_8BIT},
	{ 0x5498, 0x03, I2C_8BIT}, { 0x5499, 0x53, I2C_8BIT}, 
	{ 0x549A, 0x02, I2C_8BIT}, { 0x549B, 0xeb, I2C_8BIT},
	{ 0x549C, 0x02, I2C_8BIT}, { 0x549D, 0xa0, I2C_8BIT},
	{ 0x549E, 0x02, I2C_8BIT}, { 0x549F, 0x67, I2C_8BIT},
	{ 0x54A0, 0x02, I2C_8BIT}, { 0x54A1, 0x3b, I2C_8BIT},
	{ 0x54A2, 0x02, I2C_8BIT}, { 0x54A3, 0x18, I2C_8BIT},
	{ 0x54A4, 0x01, I2C_8BIT}, { 0x54A5, 0xe7, I2C_8BIT},
	{ 0x54A6, 0x01, I2C_8BIT}, { 0x54A7, 0xc3, I2C_8BIT},
	{ 0x54A8, 0x01, I2C_8BIT}, { 0x54A9, 0x94, I2C_8BIT},
	{ 0x54AA, 0x01, I2C_8BIT}, { 0x54AB, 0x72, I2C_8BIT},
	{ 0x54AC, 0x01, I2C_8BIT}, { 0x54AD, 0x57, I2C_8BIT},
	
    { OV9740_GAIN_CEILING_01,	0x00, I2C_8BIT},
	{ OV9740_GAIN_CEILING_02,	0x7f, I2C_8BIT},

    { OV9740_ISP_CTRL03,    0xa7,	I2C_8BIT},
    
    { OV9740_ANALOG_CTRL31,		0x52, I2C_8BIT},
	{ OV9740_ANALOG_CTRL33,		0x50, I2C_8BIT},
	{ OV9740_ANALOG_CTRL30,		0xca, I2C_8BIT},
	{ OV9740_ANALOG_CTRL04,		0x0c, I2C_8BIT},
	{ OV9740_ANALOG_CTRL01,		0x40, I2C_8BIT},
	{ OV9740_ANALOG_CTRL02,		0x16, I2C_8BIT},
	{ OV9740_ANALOG_CTRL10,		0xa1, I2C_8BIT},
	{ OV9740_ANALOG_CTRL12,		0x24, I2C_8BIT},
    
    /* Make default size 1280x720 */
    { OV9740_X_OUTPUT_SIZE,     0x0500, I2C_16BIT},
	{ OV9740_Y_OUTPUT_SIZE,     0x02d0, I2C_16BIT},
        

    /* AWB */
	{ OV9740_AWB_CTRL00,		0xf0, I2C_8BIT},
	{ OV9740_AWB_CTRL01,		0x00, I2C_8BIT},
	{ OV9740_AWB_CTRL02,		0x41, I2C_8BIT},
	{ OV9740_AWB_CTRL03,		0x42, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL01,	0x8a, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL02,	0x61, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL03,	0xce, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL04,	0xa8, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL05,	0x17, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL06,	0x1f, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL07,	0x27, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL08,	0x41, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL09,	0x34, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL10,	0xf0, I2C_8BIT},
	{ OV9740_AWB_ADV_CTRL11,	0x10, I2C_8BIT},
	{ OV9740_AWB_CTRL0F,		0xff, I2C_8BIT},
	{ OV9740_AWB_CTRL10,		0x00, I2C_8BIT},
	{ OV9740_AWB_CTRL11,		0xff, I2C_8BIT},
	{ OV9740_AWB_CTRL12,		0x00, I2C_8BIT},
	{ OV9740_AWB_CTRL13,		0xff, I2C_8BIT},
	{ OV9740_AWB_CTRL14,		0x00, I2C_8BIT},

	/* CIP */
	{ 0x530D, 0x12, I2C_8BIT},

	/* CMX */
	{ 0x5380, 0x01, I2C_8BIT}, { 0x5381, 0x00, I2C_8BIT},
	{ 0x5382, 0x00, I2C_8BIT}, { 0x5383, 0x17, I2C_8BIT},
	{ 0x5384, 0x00, I2C_8BIT}, { 0x5385, 0x01, I2C_8BIT},
	{ 0x5386, 0x00, I2C_8BIT}, { 0x5387, 0x00, I2C_8BIT},
	{ 0x5388, 0x00, I2C_8BIT}, { 0x5389, 0xe0, I2C_8BIT},
	{ 0x538A, 0x00, I2C_8BIT}, { 0x538B, 0x20, I2C_8BIT},
	{ 0x538C, 0x00, I2C_8BIT}, { 0x538D, 0x00, I2C_8BIT},
	{ 0x538E, 0x00, I2C_8BIT}, { 0x538F, 0x16, I2C_8BIT},
	{ 0x5390, 0x00, I2C_8BIT}, { 0x5391, 0x9c, I2C_8BIT},
	{ 0x5392, 0x00, I2C_8BIT}, { 0x5393, 0xa0, I2C_8BIT},
	{ 0x5394, 0x18, I2C_8BIT},

	/* 50/60 Detection */
	{ 0x3C0A, 0x9c, I2C_8BIT}, { 0x3C0B, 0x3f, I2C_8BIT},


	/* AWB Control */
	{ OV9740_AWB_MANUAL_CTRL,	0x01, I2C_8BIT},


	/* AEC/AGC Control */
	{ OV9740_AEC_ENABLE,		0x13, I2C_8BIT},
	{ OV9740_AEC_HI_THRESHOLD,	0xa0, I2C_8BIT},
	{ OV9740_AEC_3A1A,		0x05, I2C_8BIT},
	{ OV9740_AEC_CTRL1B_WPT2,	0x50, I2C_8BIT},
	{ OV9740_AEC_CTRL0F_WPT,	0x50, I2C_8BIT},
	{ OV9740_AEC_CTRL10_BPT,	0x4c, I2C_8BIT},
	{ OV9740_AEC_CTRL1E_BPT2,	0x4c, I2C_8BIT},
	{ OV9740_AEC_LO_THRESHOLD,	0x26, I2C_8BIT},

	/* BLC Control */
	{ OV9740_BLC_AUTO_ENABLE,	0x45, I2C_8BIT},
	{ OV9740_BLC_MODE,		0x18, I2C_8BIT},


	/* PLL Setting */
	{ OV9740_PLL_MODE_CTRL01,	0x80, I2C_8BIT},
	{ OV9740_PRE_PLL_CLK_DIV,	0x0002, I2C_16BIT},
	{ OV9740_PLL_MULTIPLIER,	0x005f, I2C_16BIT},
	{ OV9740_VT_SYS_CLK_DIV,	0x0001, I2C_16BIT},

	{ OV9740_PLL_CTRL3010,		0x01, I2C_8BIT},
	{ 0x300c,	0x03, I2C_8BIT}, { 0x300d,	0x13},
    { 0x300e,	0x11, I2C_8BIT},
	{ OV9740_FRM_LENGTH_LN,		0x0307, I2C_16BIT},
	{ OV9740_LN_LENGTH_PCK,		0x0662, I2C_16BIT},

	{ OV9740_VFIFO_CTRL00,		0xb1, I2C_8BIT},

	/* Pixel format default RAW 10bit*/
	{ OV9740_PIX_FORMAT,		0xF8, I2C_8BIT},
};
#else
static const struct ov9740_reg ov9740_defaults[] = {
 {0x0103, 0x01,I2C_8BIT}, 
 {0x3026, 0x00,I2C_8BIT},
 {0x3027, 0x00,I2C_8BIT},
 {0x3002, 0xe8,I2C_8BIT},
 {0x3004, 0x03,I2C_8BIT},
 {0x3005, 0xff,I2C_8BIT},
 {0x3703, 0x42,I2C_8BIT},
 {0x3704, 0x10,I2C_8BIT},
 {0x3705, 0x45,I2C_8BIT},
 {0x3603, 0xaa,I2C_8BIT},
 {0x3632, 0x2f,I2C_8BIT},
 {0x3620, 0x66,I2C_8BIT},
 {0x3621, 0xc0,I2C_8BIT},
 {0x0202, 0x03,I2C_8BIT},
 {0x0203, 0x43,I2C_8BIT},
 {0x3833, 0x04,I2C_8BIT},
 {0x3835, 0x02,I2C_8BIT},
 {0x4702, 0x04,I2C_8BIT},
 {0x4704, 0x00,I2C_8BIT},
 {0x4706, 0x08,I2C_8BIT},
 {0x3819, 0x6e,I2C_8BIT},
 {0x3817, 0x94,I2C_8BIT},
 {0x3a18, 0x00,I2C_8BIT},
 {0x3a19, 0x7f,I2C_8BIT},
 {0x5003, 0xa7,I2C_8BIT},
 {0x3631, 0x52,I2C_8BIT},
 {0x3633, 0x50,I2C_8BIT},
 {0x3630, 0xca,I2C_8BIT},
 {0x3604, 0x0c,I2C_8BIT},
 {0x3601, 0x40,I2C_8BIT},
 {0x3602, 0x16,I2C_8BIT},
 {0x3610, 0xa1,I2C_8BIT},
 {0x3612, 0x24,I2C_8BIT},
 {0x034c, 0x05,I2C_8BIT},
 {0x034d, 0x00,I2C_8BIT},
 {0x034e, 0x02,I2C_8BIT},
 {0x034f, 0xd0,I2C_8BIT},
 {0x0202, 0x03,I2C_8BIT},
 {0x0203, 0x43,I2C_8BIT},
 {0x5004, 0x00,I2C_8BIT},
 {0x4300, 0xf8,I2C_8BIT},
 {0x3837, 0x01,I2C_8BIT},
 {0x3002, 0x00,I2C_8BIT},
 {0x3004, 0x00,I2C_8BIT},
 {0x3005, 0x00,I2C_8BIT},
 {0x4801, 0x0f,I2C_8BIT},
 {0x4803, 0x05,I2C_8BIT},
 {0x4805, 0x10,I2C_8BIT},
 {0x4601, 0x16,I2C_8BIT},
 {0x3012, 0x70,I2C_8BIT},
 {0x3014, 0x01,I2C_8BIT},
 {0x300d, 0x0e,I2C_8BIT},
 {0x460e, 0xb1,I2C_8BIT},
 {0x3707, 0x14,I2C_8BIT},
 {0x3622, 0x9f,I2C_8BIT},
 {0x5841, 0x04,I2C_8BIT},
 {0x4002, 0x45,I2C_8BIT},
 {0x5000, 0x01,I2C_8BIT},
 {0x5001, 0x00,I2C_8BIT},
 {0x3406, 0x01,I2C_8BIT},
 {0x5000, 0xff,I2C_8BIT},
 {0x5001, 0xef,I2C_8BIT},
 {0x5003, 0xff,I2C_8BIT},
 {0x3503, 0x13,I2C_8BIT},
 {0x0205, 0x3f,I2C_8BIT},
 {0x4005, 0x18,I2C_8BIT},
 {0x3104, 0x80,I2C_8BIT},
 {0x0305, 0x03,I2C_8BIT},
 {0x0307, 0x5f,I2C_8BIT},
 {0x0303, 0x01,I2C_8BIT},
 {0x3010, 0x01,I2C_8BIT},
 {0x300c, 0x03,I2C_8BIT},
 {0x300d, 0x13,I2C_8BIT},
 {0x300e, 0x11,I2C_8BIT},
 {0x0340, 0x03,I2C_8BIT},
 {0x0341, 0x07,I2C_8BIT},
 {0x0342, 0x06,I2C_8BIT},
 {0x0343, 0x62,I2C_8BIT},
 {0x0100, 0x01,I2C_8BIT},
};
#endif
/**
 * struct ov9740_sensor - main structure for storage of sensor information
 * @pdata: access functions and data for platform level information
 * @v4l2_int_device: V4L2 device structure structure
 * @i2c_client: iic client device structure
 * @pix: V4L2 pixel format information structure
 * @timeperframe: time per frame expressed as V4L fraction
 * @scaler:
 * @ver: ov9740 chip version
 * @fps: frames per second value
 */
struct ov9740_sensor {
	const struct ov9740_platform_data *pdata;
	struct v4l2_int_device *v4l2_int_device;
	struct i2c_client *i2c_client;
	struct v4l2_pix_format pix;
	struct v4l2_fract timeperframe;
	int scaler;
	int ver;
	int fps;
	bool resuming;
};

static struct ov9740_sensor ov9740;
static struct i2c_driver ov9740sensor_i2c_driver;
static unsigned long xclk_current = OV9740_XCLK_NOM_1;

/* list of image formats supported by ov9740 sensor */
const static struct v4l2_fmtdesc ov9740_formats[] = {
	{
		.description	= "Bayer10 (BGb/GrR)",
		.pixelformat	= V4L2_PIX_FMT_SBGGR10,
	}
};

#define NUM_CAPTURE_FORMATS ARRAY_SIZE(ov9740_formats)

static enum v4l2_power current_power_state;

/* Structure of Sensor settings that change with image size */
static struct ov9740_sensor_settings ov9740_settings[] = {
	 /* NOTE: must be in same order as image_size array */

	/* 640x480 */
	{
		.clk = {
			.pre_pll_div = 3,
			.pll_mult = 31,
			.vt_pix_clk_div = 10,
			.vt_sys_clk_div = 1,
		},
		.mipi = {
			.data_lanes = 1,
			.ths_settle_lower = 10,
			.ths_settle_upper = 35,
		},
		.frame = {
			.line_len_pck = 1634,
			.x_addr_start = 160,
			.x_addr_end = 1123,
			.y_addr_start = 0,
			.y_addr_end = 723,
			.x_output_size = 640,
			.y_output_size = 480,
			.x_scale = 960,
			.y_scale = 720,
			.fifo_read_start = 320,
			.enable_scale = 1,
		},
	},

	/* 1280x720 */
	{
		.clk = {
			.pre_pll_div = 4,
			.pll_mult = 32,
			.vt_pix_clk_div = 10,
			.vt_sys_clk_div = 1,
		},
		.mipi = {
			.data_lanes = 1,
			.ths_settle_lower = 15,
			.ths_settle_upper = 20,
		},
		.frame = {
			.line_len_pck = 1634,
			.x_addr_start = 0,
			.x_addr_end = 1283,
			.y_addr_start = 0,
			.y_addr_end = 719,
			.x_output_size = 1280,
			.y_output_size = 720,
			.x_scale = 1280,
			.y_scale = 720,
			.fifo_read_start = 4,
			.enable_scale = 0,
		},
	},
};

#define OV9740_MODES_COUNT ARRAY_SIZE(ov9740_settings)

static unsigned isize_current = OV9740_MODES_COUNT - 1;
static struct ov9740_clock_freq current_clk;

struct i2c_list {
	struct i2c_msg *reg_list;
	unsigned int list_size;
};

/**
 * struct vcontrol - Video controls
 * @v4l2_queryctrl: V4L2 VIDIOC_QUERYCTRL ioctl structure
 * @current_value: current value of this control
 */
static struct vcontrol {
	struct v4l2_queryctrl qc;
	int current_value;
} ov9740sensor_video_control[] = {
	{
		{
			.id = V4L2_CID_EXPOSURE,
			.type = V4L2_CTRL_TYPE_INTEGER,
			.name = "Exposure",
			.minimum = OV9740_MIN_EXPOSURE,
			.maximum = OV9740_MAX_EXPOSURE,
			.step = OV9740_EXPOSURE_STEP,
			.default_value = OV9740_DEF_EXPOSURE,
		},
		.current_value = OV9740_DEF_EXPOSURE,
	},
	{
		{
			.id = V4L2_CID_GAIN,
			.type = V4L2_CTRL_TYPE_INTEGER,
			.name = "Analog Gain",
			.minimum = OV9740_EV_MIN_GAIN,
			.maximum = OV9740_EV_MAX_GAIN,
			.step = OV9740_EV_GAIN_STEP,
			.default_value = OV9740_EV_DEF_GAIN,
		},
		.current_value = OV9740_EV_DEF_GAIN,
	},
	{
		{
			.id = V4L2_CID_TEST_PATTERN,
			.type = V4L2_CTRL_TYPE_INTEGER,
			.name = "Test Pattern",
			.minimum = OV9740_MIN_TEST_PATT_MODE,
			.maximum = OV9740_MAX_TEST_PATT_MODE,
			.step = OV9740_MODE_TEST_PATT_STEP,
			.default_value = OV9740_MIN_TEST_PATT_MODE,
		},
		.current_value = OV9740_MIN_TEST_PATT_MODE,
	}
};

/**
 * find_vctrl - Finds the requested ID in the video control structure array
 * @id: ID of control to search the video control array for
 *
 * Returns the index of the requested ID from the control structure array
 */
static int find_vctrl(int id)
{
	int i;

	if (id < V4L2_CID_BASE)
		return -EDOM;

	for (i = (ARRAY_SIZE(ov9740sensor_video_control) - 1); i >= 0; i--)
		if (ov9740sensor_video_control[i].qc.id == id)
			break;
	if (i < 0)
		i = -EINVAL;
	return i;
}

/**
 * ov9740_read_reg - Read a value from a register in an ov9740 sensor device
 * @client: i2c driver client structure
 * @data_length: length of data to be read
 * @reg: register address / offset
 * @val: stores the value that gets read
 *
 * Read a value from a register in an ov9740 sensor device.
 * The value is returned in 'val'.
 * Returns zero if successful, or non-zero otherwise.
 */
static int ov9740_read_reg(struct i2c_client *client, u16 data_length, u16 reg,
			   u32 *val)
{
	int err;
	struct i2c_msg msg[1];
	unsigned char data[4] = {0};

	if (!client->adapter)
		return -ENODEV;
	if (data_length != I2C_8BIT && data_length != I2C_16BIT
			&& data_length != I2C_32BIT)
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
		if (data_length == I2C_8BIT)
			*val = data[0];
		else if (data_length == I2C_16BIT)
			*val = data[1] + (data[0] << 8);
		else
			*val = data[3] + (data[2] << 8) +
				(data[1] << 16) + (data[0] << 24);
		return 0;
	}
	v4l_err(client, "read from offset 0x%x error %d", reg, err);
	return err;
}

/**
 * Write a value to a register in ov9740 sensor device.
 * @client: i2c driver client structure.
 * @reg: Address of the register to read value from.
 * @val: Value to be written to a specific register.
 * Returns zero if successful, or non-zero otherwise.
 */
static int ov9740_write_reg(struct i2c_client *client, u16 reg, u32 val,
			    u16 data_length)
{
	int err = 0;
	struct i2c_msg msg[1];
	unsigned char data[6];
	int retries = 0;

	if (!client->adapter)
		return -ENODEV;

	if (data_length != I2C_8BIT && data_length != I2C_16BIT
			&& data_length != I2C_32BIT)
		return -EINVAL;

retry:
	msg->addr = client->addr;
	msg->flags = I2C_M_WR;
	msg->len = data_length+2;  /* add address bytes */
	msg->buf = data;

	/* high byte goes out first */
	data[0] = (u8) (reg >> 8);
	data[1] = (u8) (reg & 0xff);
	if (data_length == I2C_8BIT) {
		data[2] = val & 0xff;
	} else if (data_length == I2C_16BIT) {
		data[2] = (val >> 8) & 0xff;
		data[3] = val & 0xff;
	} else {
		data[2] = (val >> 24) & 0xff;
		data[3] = (val >> 16) & 0xff;
		data[4] = (val >> 8) & 0xff;
		data[5] = val & 0xff;
	}

	if (data_length == 1)
		dev_dbg(&client->dev, "OV9740 Wrt:[0x%04X]=0x%02X\n",
				reg, val);
	else if (data_length == 2)
		dev_dbg(&client->dev, "OV9740 Wrt:[0x%04X]=0x%04X\n",
				reg, val);

	err = i2c_transfer(client->adapter, msg, 1);

	if (err >= 0)
		return 0;

	if (retries <= 5) {
		v4l_info(client, "Retrying I2C... %d", retries);
		retries++;
		mdelay(20);
		goto retry;
	}

	return err;
}

/**
 * Initialize a list of ov9740 registers.
 * The list of registers is terminated by the pair of values
 * {OV3640_REG_TERM, OV3640_VAL_TERM}.
 * @client: i2c driver client structure.
 * @reglist[]: List of address of the registers to write data.
 * Returns zero if successful, or non-zero otherwise.
 */
static int ov9740_write_regs(struct i2c_client *client,
			     const struct ov9740_reg reglist[])
{
	int err = 0;
	const struct ov9740_reg *list = reglist;

	while (!((list->reg == I2C_REG_TERM)
		&& (list->val == I2C_VAL_TERM))) {
		err = ov9740_write_reg(client, list->reg,
				list->val, list->length);
		if (err)
			return err;
		list++;
	}
	return 0;
}

/**
 * ov9740_find_size - Find the best match for a requested image capture size
 * @width: requested image width in pixels
 * @height: requested image height in pixels
 *
 * Find the best match for a requested image capture size.  The best match
 * is chosen as the nearest match that has the same number or fewer pixels
 * as the requested size, or the smallest image size if the requested size
 * has fewer pixels than the smallest image.
 * Since the available sizes are subsampled in the vertical direction only,
 * the routine will find the size with a height that is equal to or less
 * than the requested height.
 */
static unsigned ov9740_find_size(unsigned int width, unsigned int height)
{
	unsigned isize;

	for (isize = 0; isize < OV9740_MODES_COUNT; isize++) {
		if ((ov9740_settings[isize].frame.y_output_size >= height) &&
		    (ov9740_settings[isize].frame.x_output_size >= width))
			break;
	}

	printk(KERN_DEBUG "ov9740_find_size: Req Size=%dx%d, "
			"Calc Size=%dx%d\n", width, height,
			ov9740_settings[isize].frame.x_output_size,
			ov9740_settings[isize].frame.y_output_size);

	return isize;
}

/**
 * Set CSI2 Virtual ID.
 * @client: i2c client driver structure
 * @id: Virtual channel ID.
 *
 * Sets the channel ID which identifies data packets sent by this device
 * on the CSI2 bus.
 **/
static int ov9740_set_virtual_id(struct i2c_client *client, u32 id)
{
	u32 reg_val;

	ov9740_read_reg(client, I2C_8BIT, OV9740_MIPI_CTRL14, &reg_val);
	reg_val = reg_val | ((id & 0x3)<<6);
	return ov9740_write_reg(client, OV9740_MIPI_CTRL14, 
							reg_val, I2C_8BIT);
}

/**
 * ov9740_set_framerate - Sets framerate by adjusting frame_length_lines reg.
 * @s: pointer to standard V4L2 device structure
 * @fper: frame period numerator and denominator in seconds
 *
 * The maximum exposure time is also updated since it is affected by the
 * frame rate.
 **/
static int ov9740_set_framerate(struct v4l2_int_device *s,
				struct v4l2_fract *fper)
{
	int err = 0;
	u16 isize = isize_current;
	u32 frame_length_lines, line_time_q8;
	struct ov9740_sensor *sensor = s->priv;
	struct ov9740_sensor_settings *ss;

	if ((fper->numerator == 0) || (fper->denominator == 0)) {
		/* supply a default nominal_timeperframe */
		fper->numerator = 1;
		fper->denominator = OV9740_MAX_FPS;
	}

	sensor->fps = fper->denominator / fper->numerator;
	if (sensor->fps < OV9740_MIN_FPS) {
		sensor->fps = OV9740_MIN_FPS;
		fper->numerator = 1;
		fper->denominator = sensor->fps;
	} else if (sensor->fps > OV9740_MAX_FPS) {
		sensor->fps = OV9740_MAX_FPS;
		fper->numerator = 1;
		fper->denominator = sensor->fps;
	}

	ss = &ov9740_settings[isize_current];

	line_time_q8 = ((u32)ss->frame.line_len_pck * 1000000) /
			(current_clk.vt_pix_clk >> 8); /* usec's */

	frame_length_lines = (((u32)fper->numerator * 1000000 * 256 /
			       fper->denominator)) / line_time_q8;

	/* Range check frame_length_lines */
	if (frame_length_lines > OV9740_MAX_FRAME_LENGTH_LINES)
		frame_length_lines = OV9740_MAX_FRAME_LENGTH_LINES;

	ov9740_settings[isize].frame.frame_len_lines = frame_length_lines;

	printk(KERN_DEBUG "OV9740 Set Framerate: fper=%d/%d, "
	       "frame_len_lines=%d, max_expT=%dus\n", fper->numerator,
	       fper->denominator, frame_length_lines, OV9740_MAX_EXPOSURE);

	return err;
}

/**
 * ov9740sensor_calc_xclk - Calculate the required xclk frequency
 *
 * Xclk is not determined from framerate for the OV9740
 */
static unsigned long ov9740sensor_calc_xclk(void)
{
	return OV9740_XCLK_NOM_1;
}

/**
 * Sets the correct orientation based on the sensor version.
 *   IU046F2-Z   version=2  orientation=3
 *   IU046F4-2D  version>2  orientation=0
 */
static int ov9740_set_orientation(struct i2c_client *client, u32 ver)
{
	int err;
	u8 orient;

	orient = (ver <= 0x2) ? 0x3 : 0x0;
	err = ov9740_write_reg(client, OV9740_IMAGE_ORT,
			       orient, I2C_8BIT);
	return err;
}

/**
 * ov9740sensor_set_exposure_time - sets exposure time per input value
 * @exp_time: exposure time to be set on device (in usec)
 * @s: pointer to standard V4L2 device structure
 * @lvc: pointer to V4L2 exposure entry in ov9740sensor_video_controls array
 *
 * If the requested exposure time is within the allowed limits, the HW
 * is configured to use the new exposure time, and the
 * ov9740sensor_video_control[] array is updated with the new current value.
 * The function returns 0 upon success.  Otherwise an error code is
 * returned.
 */
static int ov9740sensor_set_exposure_time(u32 exp_time,
					  struct v4l2_int_device *s,
					  struct vcontrol *lvc)
{
	int err = 0, i;
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;
	u16 coarse_int_time = 0;
	u32 line_time_q8 = 0;
	struct ov9740_sensor_settings *ss;

	if ((current_power_state == V4L2_POWER_ON) || sensor->resuming) {
		if (exp_time < OV9740_MIN_EXPOSURE) {
			v4l_err(client, "Exposure time %d us not within"
					" the legal range.\n", exp_time);
			v4l_err(client, "Exposure time must be between"
					" %d us and %d us\n",
					OV9740_MIN_EXPOSURE,
					OV9740_MAX_EXPOSURE);
			exp_time = OV9740_MIN_EXPOSURE;
		}

		if (exp_time > OV9740_MAX_EXPOSURE) {
			v4l_err(client, "Exposure time %d us not within"
					" the legal range.\n", exp_time);
			v4l_err(client, "Exposure time must be between"
					" %d us and %d us\n",
					OV9740_MIN_EXPOSURE,
					OV9740_MAX_EXPOSURE);
			exp_time = OV9740_MAX_EXPOSURE;
		}

		ss = &ov9740_settings[isize_current];

		line_time_q8 = ((u32)ss->frame.line_len_pck * 1000000) /
				(current_clk.vt_pix_clk >> 8); /* usec's */

		coarse_int_time = ((exp_time * 256) + (line_time_q8 >> 1)) /
				  line_time_q8;

		if (coarse_int_time > ss->frame.frame_len_lines - 2)
			err = ov9740_write_reg(client,
					       OV9740_FRM_LENGTH_LN,
					       coarse_int_time + 2,
					       I2C_16BIT);
		else
			err = ov9740_write_reg(client,
					       OV9740_FRM_LENGTH_LN,
					       ss->frame.frame_len_lines,
					       I2C_16BIT);

		err = ov9740_write_reg(client, OV9740_COARSE_INT_TIME,
				       coarse_int_time, I2C_16BIT);
	}

	if (err) {
		v4l_err(client, "Error setting exposure time: %d", err);
	} else {
		i = find_vctrl(V4L2_CID_EXPOSURE);
		if (i >= 0) {
			lvc = &ov9740sensor_video_control[i];
			lvc->current_value = exp_time;
		}
	}

	return err;
}

/**
 * This table describes what should be written to the sensor register for each
 * gain value. The gain(index in the table) is in terms of 0.1EV, i.e. 10
 * indexes in the table give 2 time more gain
 *
 * Elements in TS2_8_GAIN_TBL doesn't comply linearity. This is because
 * there is nonlinear dependecy between analogue_gain_code_global and real gain
 * value: Gain_analog = 256 / (256 - analogue_gain_code_global)
 */

static const u16 OV9740_EV_GAIN_TBL[OV9740_EV_TABLE_GAIN_MAX + 1] = {
	/* Gain x1 */
	0,  16, 33, 48, 62, 74, 88, 98, 109, 119,

	/* Gain x2 */
	128, 136, 144, 152, 159, 165, 171, 177, 182, 187,

	/* Gain x4 */
	192, 196, 200, 204, 208, 211, 214, 216, 219, 222,

	/* Gain x8 */
	224
};

/**
 * ov9740sensor_set_gain - sets sensor analog gain per input value
 * @gain: analog gain value to be set on device
 * @s: pointer to standard V4L2 device structure
 * @lvc: pointer to V4L2 analog gain entry in ov9740sensor_video_control array
 *
 * If the requested analog gain is within the allowed limits, the HW
 * is configured to use the new gain value, and the ov9740sensor_video_control
 * array is updated with the new current value.
 * The function returns 0 upon success.  Otherwise an error code is
 * returned.
 */
static int ov9740sensor_set_gain(u16 lineargain, struct v4l2_int_device *s,
				 struct vcontrol *lvc)
{
	int err = 0, i;
	u16 reg_gain = 0;
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;

	if (current_power_state == V4L2_POWER_ON || sensor->resuming) {

		if (lineargain < OV9740_EV_MIN_GAIN) {
			lineargain = OV9740_EV_MIN_GAIN;
			v4l_err(client, "Gain out of legal range.");
		}
		if (lineargain > OV9740_EV_MAX_GAIN) {
			lineargain = OV9740_EV_MAX_GAIN;
			v4l_err(client, "Gain out of legal range.");
		}

		reg_gain = OV9740_EV_GAIN_TBL[lineargain];

		err = ov9740_write_reg(client, OV9740_ANALOG_GAIN_GLOBAL,
					reg_gain, I2C_16BIT);
	}

	if (err) {
		v4l_err(client, "Error setting analog gain: %d", err);
	} else {
		i = find_vctrl(V4L2_CID_GAIN);
		if (i >= 0) {
			lvc = &ov9740sensor_video_control[i];
			lvc->current_value = lineargain;
		}
	}

	return err;
}

/**
 * ov9740_update_clocks - calcs sensor clocks based on sensor settings.
 * @isize: image size enum
 */
static int ov9740_update_clocks(u32 xclk, unsigned isize)
{
	current_clk.vco_clk =
			xclk * ov9740_settings[isize].clk.pll_mult /
			ov9740_settings[isize].clk.pre_pll_div;

	current_clk.vt_pix_clk = current_clk.vco_clk * 2 /
			(ov9740_settings[isize].clk.vt_pix_clk_div *
			ov9740_settings[isize].clk.vt_sys_clk_div);

		current_clk.mipi_clk = 2 * current_clk.vco_clk;

	current_clk.ddr_clk = current_clk.mipi_clk / 2;

	printk(KERN_DEBUG "OV9740: xclk=%u, vco_clk=%u, "
		"vt_pix_clk=%u,  mipi_clk=%u,  ddr_clk=%u\n",
		xclk, current_clk.vco_clk, current_clk.vt_pix_clk,
		current_clk.mipi_clk, current_clk.ddr_clk);

	return 0;
}

/**
 * ov9740_setup_mipi - initializes sensor & isp MIPI registers.
 * @c: i2c client driver structure
 * @isize: image size enum
 */
static int ov9740_setup_mipi(struct v4l2_int_device *s, unsigned isize)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;

	/* NOTE: Make sure ov9740_update_clocks is called 1st */

	/* Set number of lanes in isp */
	sensor->pdata->csi2_lane_count(s,
				       ov9740_settings[isize].mipi.data_lanes);

	/* Send settings to ISP-CSI2 Receiver PHY */
	sensor->pdata->csi2_calc_phy_cfg0(s, current_clk.mipi_clk,
		ov9740_settings[isize].mipi.ths_settle_lower,
		ov9740_settings[isize].mipi.ths_settle_upper);

	return 0;
}

/**
 * ov9740_configure_frame - initializes image frame registers
 * @c: i2c client driver structure
 * @isize: image size enum
 */
static int ov9740_configure_frame(struct i2c_client *client, unsigned isize)
{

	ov9740_write_reg(client, OV9740_FRM_LENGTH_LN,
		ov9740_settings[isize].frame.frame_len_lines, I2C_16BIT);

	ov9740_write_reg(client, OV9740_LN_LENGTH_PCK,
		ov9740_settings[isize].frame.line_len_pck, I2C_16BIT);

	ov9740_write_reg(client, OV9740_X_ADDR_START,
		ov9740_settings[isize].frame.x_addr_start, I2C_16BIT);

	ov9740_write_reg(client, OV9740_X_ADDR_END,
		ov9740_settings[isize].frame.x_addr_end, I2C_16BIT);

	ov9740_write_reg(client, OV9740_Y_ADDR_START,
		ov9740_settings[isize].frame.y_addr_start, I2C_16BIT);

	ov9740_write_reg(client, OV9740_Y_ADDR_END,
		ov9740_settings[isize].frame.y_addr_end, I2C_16BIT);

	ov9740_write_reg(client, OV9740_X_OUTPUT_SIZE,
		ov9740_settings[isize].frame.x_output_size, I2C_16BIT);

	ov9740_write_reg(client, OV9740_Y_OUTPUT_SIZE,
		ov9740_settings[isize].frame.y_output_size, I2C_16BIT);

	ov9740_write_reg( client, OV9740_VFIFO_READ_START,
					  ov9740_settings[isize].frame.fifo_read_start, I2C_16BIT);


	ov9740_write_reg( client, OV9740_ISP_CTRL00, 0xff, I2C_8BIT);
	
    if(ov9740_settings[isize].frame.enable_scale)
	{
		u32 x_scale = ov9740_settings[isize].frame.x_scale;
		u32 y_scale = ov9740_settings[isize].frame.y_scale;
		
		/*enable scaling in sensor register */
		ov9740_write_reg( client, OV9740_ISP_CTRL01, 0xff, I2C_8BIT);
		
        /*these scaling params are just valid for 640*480 */
		ov9740_write_reg( client, OV9740_ISP_CTRL1E, (x_scale >> 8) & 0x7, I2C_8BIT);
		ov9740_write_reg( client, OV9740_ISP_CTRL1F, (x_scale & 0xFF), I2C_8BIT);
		ov9740_write_reg( client, OV9740_ISP_CTRL20, (y_scale >> 8) & 0x3, I2C_8BIT);
		ov9740_write_reg( client, OV9740_ISP_CTRL21, (y_scale & 0xFF), I2C_8BIT);
	}
	else
	{
		/* disable scaling in sensor register */
		ov9740_write_reg( client, OV9740_ISP_CTRL01, 0xef, I2C_8BIT);
	}
    
	ov9740_write_reg( client, OV9740_ISP_CTRL03, 0xff, I2C_8BIT);

	return 0;
}

 /**
 * ov9740_configure_test_pattern - Configure 3 possible test pattern modes
 * @ mode: Test pattern mode. Possible modes : 1 , 2 and 4.
 * @s: pointer to standard V4L2 device structure
 * @lvc: pointer to V4L2 exposure entry in ov9740sensor_video_controls array
 *
 * If the requested test pattern mode is within the allowed limits, the HW
 * is configured for that particular test pattern, and the
 * ov9740sensor_video_control[] array is updated with the new current value.
 * The function returns 0 upon success.  Otherwise an error code is
 * returned.
 */
static int ov9740_configure_test_pattern(int mode, struct v4l2_int_device *s,
					 struct vcontrol *lvc)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;

	if ((current_power_state == V4L2_POWER_ON) || sensor->resuming) {

		switch (mode) {
		case OV9740_TEST_PATT_COLOR_BAR:
		case OV9740_TEST_PATT_PN9:
			/* red */
			ov9740_write_reg(client, OV9740_TEST_PATT_RED,
							0x07ff, I2C_16BIT);
			/* green-red */
			ov9740_write_reg(client, OV9740_TEST_PATT_GREENR,
							0x00ff,	I2C_16BIT);
			/* blue */
			ov9740_write_reg(client, OV9740_TEST_PATT_BLUE,
							0x0000, I2C_16BIT);
			/* green-blue */
			ov9740_write_reg(client, OV9740_TEST_PATT_GREENB,
							0x0000,	I2C_16BIT);
			break;
		case OV9740_TEST_PATT_SOLID_COLOR:
			/* red */
			ov9740_write_reg(client, OV9740_TEST_PATT_RED,
				(OV9740_BLACK_LEVEL_AVG & 0x00ff), I2C_16BIT);
			/* green-red */
			ov9740_write_reg(client, OV9740_TEST_PATT_GREENR,
				(OV9740_BLACK_LEVEL_AVG & 0x00ff), I2C_16BIT);
			/* blue */
			ov9740_write_reg(client, OV9740_TEST_PATT_BLUE,
				(OV9740_BLACK_LEVEL_AVG & 0x00ff), I2C_16BIT);
			/* green-blue */
			ov9740_write_reg(client, OV9740_TEST_PATT_GREENB,
				(OV9740_BLACK_LEVEL_AVG & 0x00ff), I2C_16BIT);
			break;
		}
		/* test-pattern mode */
		ov9740_write_reg(client, OV9740_TEST_PATT_MODE,
						(mode & 0x7), I2C_16BIT);
	}
	lvc->current_value = mode;
	return 0;
}
/**
 * ov9740_configure - Configure the ov9740 for the specified image mode
 * @s: pointer to standard V4L2 device structure
 *
 * Configure the ov9740 for a specified image size, pixel format, and frame
 * period.  xclk is the frequency (in Hz) of the xclk input to the ov9740.
 * fper is the frame period (in seconds) expressed as a fraction.
 * Returns zero if successful, or non-zero otherwise.
 * The actual frame period is returned in fper.
 */
static int ov9740_configure(struct v4l2_int_device *s)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;
	unsigned isize = isize_current;
	int err, i;
	struct vcontrol *lvc = NULL;

//	err = ov9740_write_reg(client, OV9740_SOFTWARE_RESET, 0x01, I2C_8BIT);
//	mdelay(5);

	ov9740_write_regs(client, ov9740_defaults);

	ov9740_setup_mipi(s, isize);

	/* configure image size and pixel format */
	ov9740_configure_frame(client, isize);

//	ov9740_set_orientation(client, sensor->ver);

	sensor->pdata->csi2_cfg_vp_out_ctrl(s, 2);
	sensor->pdata->csi2_ctrl_update(s, false);
	
	sensor->pdata->csi2_cfg_virtual_id(s, 0, OV9740_CSI2_VIRTUAL_ID);
	sensor->pdata->csi2_ctx_update(s, 0, false);
	ov9740_set_virtual_id(client, OV9740_CSI2_VIRTUAL_ID);

	/* Set initial exposure and gain */
/*	i = find_vctrl(V4L2_CID_EXPOSURE);
	if (i >= 0) {
		lvc = &ov9740sensor_video_control[i];
		ov9740sensor_set_exposure_time(lvc->current_value,
					sensor->v4l2_int_device, lvc);
	}

	i = find_vctrl(V4L2_CID_GAIN);
	if (i >= 0) {
		lvc = &ov9740sensor_video_control[i];
		ov9740sensor_set_gain(lvc->current_value,
				sensor->v4l2_int_device, lvc);
	}

	i = find_vctrl(V4L2_CID_TEST_PATTERN);
	if (i >= 0) {
		lvc = &ov9740sensor_video_control[i];
		ov9740_configure_test_pattern(lvc->current_value,
				sensor->v4l2_int_device, lvc);
	}
*/
/* configure streaming ON */
//	err = ov9740_write_reg(client, OV9740_MODE_SELECT, 0x01, I2C_8BIT);
	mdelay(1);

	return err;
}

/**
 * ov9740_detect - Detect if an ov9740 is present, and if so which revision
 * @client: pointer to the i2c client driver structure
 *
 * Detect if an ov9740 is present, and if so which revision.
 * A device is considered to be detected if the manufacturer ID (MIDH and MIDL)
 * and the product ID (PID) registers match the expected values.
 * Any value of the version ID (VER) register is accepted.
 * Returns a negative error number if no device is detected, or the
 * non-negative value of the version ID register if a device is detected.
 */
static int ov9740_detect(struct i2c_client *client)
{
	u32 model_id, mfr_id, rev;
	struct ov9740_sensor *sensor;

	if (!client)
		return -ENODEV;

	sensor = i2c_get_clientdata(client);

	if (ov9740_read_reg(client, I2C_16BIT, OV9740_MODEL_ID, &model_id))
		return -ENODEV;
	if (ov9740_read_reg(client, I2C_8BIT, OV9740_MANUFACTURER_ID, &mfr_id))
		return -ENODEV;
	if (ov9740_read_reg(client, I2C_8BIT, OV9740_REVISION_NUMBER, &rev))
		return -ENODEV;

	v4l_info(client, "model id detected 0x%x mfr 0x%x, rev# 0x%x\n",
							model_id, mfr_id, rev);
	if (model_id != OV9740_MOD_ID) {
		/* We didn't read the values we expected, so
		 * this must not be an OV9740.
		 */
		v4l_warn(client, "model id mismatch 0x%x mfr 0x%x\n",
							model_id, mfr_id);

		return -ENODEV;
	}
	return rev;
}

/**
 * ioctl_queryctrl - V4L2 sensor interface handler for VIDIOC_QUERYCTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @qc: standard V4L2 VIDIOC_QUERYCTRL ioctl structure
 *
 * If the requested control is supported, returns the control information
 * from the ov9740sensor_video_control[] array.
 * Otherwise, returns -EINVAL if the control is not supported.
 */
static int ioctl_queryctrl(struct v4l2_int_device *s, struct v4l2_queryctrl *qc)
{
	int i;

	i = find_vctrl(qc->id);
	if (i == -EINVAL)
		qc->flags = V4L2_CTRL_FLAG_DISABLED;

	if (i < 0)
		return -EINVAL;

	*qc = ov9740sensor_video_control[i].qc;
	return 0;
}

/**
 * ioctl_g_ctrl - V4L2 sensor interface handler for VIDIOC_G_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_G_CTRL ioctl structure
 *
 * If the requested control is supported, returns the control's current
 * value from the ov9740sensor_video_control[] array.
 * Otherwise, returns -EINVAL if the control is not supported.
 */
static int ioctl_g_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	struct vcontrol *lvc;
	int i;

	i = find_vctrl(vc->id);
	if (i < 0)
		return -EINVAL;
	lvc = &ov9740sensor_video_control[i];

	switch (vc->id) {
	case  V4L2_CID_EXPOSURE:
		vc->value = lvc->current_value;
		break;
	case V4L2_CID_GAIN:
		vc->value = lvc->current_value;
		break;
	case V4L2_CID_TEST_PATTERN:
		vc->value = lvc->current_value;
		break;
	}

	return 0;
}

/**
 * ioctl_s_ctrl - V4L2 sensor interface handler for VIDIOC_S_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_S_CTRL ioctl structure
 *
 * If the requested control is supported, sets the control's current
 * value in HW (and updates the ov9740sensor_video_control[] array).
 * Otherwise, * returns -EINVAL if the control is not supported.
 */
static int ioctl_s_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int retval = -EINVAL;
	int i;
	struct vcontrol *lvc;

	i = find_vctrl(vc->id);
	if (i < 0)
		return -EINVAL;
	lvc = &ov9740sensor_video_control[i];

	switch (vc->id) {
	case V4L2_CID_EXPOSURE:
		retval = ov9740sensor_set_exposure_time(vc->value, s, lvc);
		break;
	case V4L2_CID_GAIN:
		retval = ov9740sensor_set_gain(vc->value, s, lvc);
		break;
	case V4L2_CID_TEST_PATTERN:
		retval = ov9740_configure_test_pattern(vc->value, s, lvc);
		break;
	}

	return retval;
}

/**
 * ioctl_enum_fmt_cap - Implement the CAPTURE buffer VIDIOC_ENUM_FMT ioctl
 * @s: pointer to standard V4L2 device structure
 * @fmt: standard V4L2 VIDIOC_ENUM_FMT ioctl structure
 *
 * Implement the VIDIOC_ENUM_FMT ioctl for the CAPTURE buffer type.
 */
static int ioctl_enum_fmt_cap(struct v4l2_int_device *s,
			      struct v4l2_fmtdesc *fmt)
{
	int index = fmt->index;
	enum v4l2_buf_type type = fmt->type;

	memset(fmt, 0, sizeof(*fmt));
	fmt->index = index;
	fmt->type = type;

	switch (fmt->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if (index >= NUM_CAPTURE_FORMATS)
			return -EINVAL;
	break;
	default:
		return -EINVAL;
	}

	fmt->flags = ov9740_formats[index].flags;
	strlcpy(fmt->description, ov9740_formats[index].description,
					sizeof(fmt->description));
	fmt->pixelformat = ov9740_formats[index].pixelformat;

	return 0;
}

/**
 * ioctl_try_fmt_cap - Implement the CAPTURE buffer VIDIOC_TRY_FMT ioctl
 * @s: pointer to standard V4L2 device structure
 * @f: pointer to standard V4L2 VIDIOC_TRY_FMT ioctl structure
 *
 * Implement the VIDIOC_TRY_FMT ioctl for the CAPTURE buffer type.  This
 * ioctl is used to negotiate the image capture size and pixel format
 * without actually making it take effect.
 */
static int ioctl_try_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	unsigned isize;
	int ifmt;
	struct v4l2_pix_format *pix = &f->fmt.pix;
	struct ov9740_sensor *sensor = s->priv;
	struct v4l2_pix_format *pix2 = &sensor->pix;

	isize = ov9740_find_size(pix->width, pix->height);
	isize_current = isize;

	pix->width = ov9740_settings[isize].frame.x_output_size;
	pix->height = ov9740_settings[isize].frame.y_output_size;
	for (ifmt = 0; ifmt < NUM_CAPTURE_FORMATS; ifmt++) {
		if (pix->pixelformat == ov9740_formats[ifmt].pixelformat)
			break;
	}
	if (ifmt == NUM_CAPTURE_FORMATS)
		ifmt = 0;
	pix->pixelformat = ov9740_formats[ifmt].pixelformat;
	pix->field = V4L2_FIELD_NONE;
	pix->bytesperline = pix->width * 2;
	pix->sizeimage = pix->bytesperline * pix->height;
	pix->priv = 0;
	pix->colorspace = V4L2_COLORSPACE_SRGB;
	*pix2 = *pix;
	return 0;
}

/**
 * ioctl_s_fmt_cap - V4L2 sensor interface handler for VIDIOC_S_FMT ioctl
 * @s: pointer to standard V4L2 device structure
 * @f: pointer to standard V4L2 VIDIOC_S_FMT ioctl structure
 *
 * If the requested format is supported, configures the HW to use that
 * format, returns error code if format not supported or HW can't be
 * correctly configured.
 */
static int ioctl_s_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct ov9740_sensor *sensor = s->priv;
	struct v4l2_pix_format *pix = &f->fmt.pix;
	int rval;

	rval = ioctl_try_fmt_cap(s, f);
	if (rval)
		return rval;
	else
		sensor->pix = *pix;


	return rval;
}

/**
 * ioctl_g_fmt_cap - V4L2 sensor interface handler for ioctl_g_fmt_cap
 * @s: pointer to standard V4L2 device structure
 * @f: pointer to standard V4L2 v4l2_format structure
 *
 * Returns the sensor's current pixel format in the v4l2_format
 * parameter.
 */
static int ioctl_g_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct ov9740_sensor *sensor = s->priv;
	f->fmt.pix = sensor->pix;

	return 0;
}

/**
 * ioctl_g_pixclk - V4L2 sensor interface handler for ioctl_g_pixclk
 * @s: pointer to standard V4L2 device structure
 * @pixclk: pointer to unsigned 32 var to store pixelclk in HZ
 *
 * Returns the sensor's current pixel clock in HZ
 */
static int ioctl_priv_g_pixclk(struct v4l2_int_device *s, u32 *pixclk)
{
	*pixclk = current_clk.vt_pix_clk;

	return 0;
}

/**
 * ioctl_g_activesize - V4L2 sensor interface handler for ioctl_g_activesize
 * @s: pointer to standard V4L2 device structure
 * @pix: pointer to standard V4L2 v4l2_pix_format structure
 *
 * Returns the sensor's current active image basesize.
 */
static int ioctl_priv_g_activesize(struct v4l2_int_device *s,
				   struct v4l2_rect *pix)
{
	struct ov9740_frame_settings *frm;

	frm = &ov9740_settings[isize_current].frame;
	pix->left = frm->x_addr_start ;
	pix->top = frm->y_addr_start;
	pix->width = (frm->x_addr_end + 1) - frm->x_addr_start;
	pix->height = (frm->y_addr_end + 1) - frm->y_addr_start;

	return 0;
}

/**
 * ioctl_g_fullsize - V4L2 sensor interface handler for ioctl_g_fullsize
 * @s: pointer to standard V4L2 device structure
 * @pix: pointer to standard V4L2 v4l2_pix_format structure
 *
 * Returns the sensor's biggest image basesize.
 */
static int ioctl_priv_g_fullsize(struct v4l2_int_device *s,
				 struct v4l2_rect *pix)
{
	struct ov9740_frame_settings *frm;

	frm = &ov9740_settings[isize_current].frame;
	pix->left = 0;
	pix->top = 0;
	pix->width = frm->line_len_pck;
	pix->height = frm->frame_len_lines;

	return 0;
}

/**
 * ioctl_g_pixelsize - V4L2 sensor interface handler for ioctl_g_pixelsize
 * @s: pointer to standard V4L2 device structure
 * @pix: pointer to standard V4L2 v4l2_pix_format structure
 *
 * Returns the sensor's configure pixel size.
 */
static int ioctl_priv_g_pixelsize(struct v4l2_int_device *s,
				  struct v4l2_rect *pix)
{
	struct ov9740_frame_settings *frm;

	frm = &ov9740_settings[isize_current].frame;
	pix->left = 0;
	pix->top = 0;
	pix->width = 1;
	pix->height = 1;

	return 0;
}

/**
 * ioctl_priv_g_pixclk_active - V4L2 sensor interface handler
 *                              for ioctl_priv_g_pixclk_active
 * @s: pointer to standard V4L2 device structure
 * @pixclk: pointer to unsigned 32 var to store pixelclk in HZ
 *
 * The function calculates optimal pixel clock which can use
 * the data received from sensor complying with all the
 * peculiarities of the sensors and the currently selected mode.
 */
static int ioctl_priv_g_pixclk_active(struct v4l2_int_device *s, u32 *pixclk)
{
	struct v4l2_rect full, active;
	u32 sens_pixclk;

	ioctl_priv_g_activesize(s, &active);
	ioctl_priv_g_fullsize(s, &full);
	ioctl_priv_g_pixclk(s, &sens_pixclk);

	*pixclk = (sens_pixclk / full.width) * active.width;

	return 0;
}

/**
 * ioctl_g_parm - V4L2 sensor interface handler for VIDIOC_G_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_G_PARM ioctl structure
 *
 * Returns the sensor's video CAPTURE parameters.
 */
static int ioctl_g_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct ov9740_sensor *sensor = s->priv;
	struct v4l2_captureparm *cparm = &a->parm.capture;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	memset(a, 0, sizeof(*a));
	a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	cparm->capability = V4L2_CAP_TIMEPERFRAME;
	cparm->timeperframe = sensor->timeperframe;

	return 0;
}

/**
 * ioctl_s_parm - V4L2 sensor interface handler for VIDIOC_S_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_S_PARM ioctl structure
 *
 * Configures the sensor to use the input parameters, if possible.  If
 * not possible, reverts to the old parameters and returns the
 * appropriate error code.
 */
static int ioctl_s_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct ov9740_sensor *sensor = s->priv;
	struct v4l2_fract *timeperframe = &a->parm.capture.timeperframe;
	int err = 0;

	sensor->timeperframe = *timeperframe;
	ov9740sensor_calc_xclk();
	ov9740_update_clocks(xclk_current, isize_current);
	err = ov9740_set_framerate(s, &sensor->timeperframe);
	*timeperframe = sensor->timeperframe;

	return err;
}


/**
 * ioctl_g_priv - V4L2 sensor interface handler for vidioc_int_g_priv_num
 * @s: pointer to standard V4L2 device structure
 * @p: void pointer to hold sensor's private data address
 *
 * Returns device's (sensor's) private data area address in p parameter
 */
static int ioctl_g_priv(struct v4l2_int_device *s, void *p)
{
	struct ov9740_sensor *sensor = s->priv;

	return sensor->pdata->priv_data_set(s, p);

}

static int __ov9740_power_off_standby(struct v4l2_int_device *s,
				      enum v4l2_power on)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;
	int rval;

	rval = sensor->pdata->power_set(s, on);
	if (rval < 0) {
		v4l_err(client, "Unable to set the power state: "
			OV9740_DRIVER_NAME " sensor\n");
		return rval;
	}

	sensor->pdata->set_xclk(s, 0);
	return 0;
}

static int ov9740_power_off(struct v4l2_int_device *s)
{
	return __ov9740_power_off_standby(s, V4L2_POWER_OFF);
}

static int ov9740_power_standby(struct v4l2_int_device *s)
{
	return __ov9740_power_off_standby(s, V4L2_POWER_STANDBY);
}

static int ov9740_power_on(struct v4l2_int_device *s)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;
	int rval;

	sensor->pdata->set_xclk(s, xclk_current);

	rval = sensor->pdata->power_set(s, V4L2_POWER_ON);
	if (rval < 0) {
		v4l_err(client, "Unable to set the power state: "
			OV9740_DRIVER_NAME " sensor\n");
		sensor->pdata->set_xclk(s, 0);
		return rval;
	}

	return 0;
}

/**
 * ioctl_s_power - V4L2 sensor interface handler for vidioc_int_s_power_num
 * @s: pointer to standard V4L2 device structure
 * @on: power state to which device is to be set
 *
 * Sets devices power state to requrested state, if possible.
 */
static int ioctl_s_power(struct v4l2_int_device *s, enum v4l2_power on)
{
	struct ov9740_sensor *sensor = s->priv;
	struct vcontrol *lvc;
	int i;

	switch (on) {
	case V4L2_POWER_ON:
		ov9740_power_on(s);
		if (current_power_state == V4L2_POWER_STANDBY) {
			sensor->resuming = true;
			ov9740_configure(s);
		}
		break;
	case V4L2_POWER_OFF:
		ov9740_power_off(s);

		/* Reset defaults for controls */
		i = find_vctrl(V4L2_CID_GAIN);
		if (i >= 0) {
			lvc = &ov9740sensor_video_control[i];
			lvc->current_value = OV9740_EV_DEF_GAIN;
		}
		i = find_vctrl(V4L2_CID_EXPOSURE);
		if (i >= 0) {
			lvc = &ov9740sensor_video_control[i];
			lvc->current_value = OV9740_DEF_EXPOSURE;
		}
		i = find_vctrl(V4L2_CID_TEST_PATTERN);
		if (i >= 0) {
			lvc = &ov9740sensor_video_control[i];
			lvc->current_value = OV9740_MIN_TEST_PATT_MODE;
		}
		break;
	case V4L2_POWER_STANDBY:
		ov9740_power_standby(s);
		break;
	}

	sensor->resuming = false;
	current_power_state = on;
	return 0;
}

/**
 * ioctl_init - V4L2 sensor interface handler for VIDIOC_INT_INIT
 * @s: pointer to standard V4L2 device structure
 *
 * Initialize the sensor device (call ov9740_configure())
 */
static int ioctl_init(struct v4l2_int_device *s)
{
	return 0;
}

/**
 * ioctl_dev_exit - V4L2 sensor interface handler for vidioc_int_dev_exit_num
 * @s: pointer to standard V4L2 device structure
 *
 * Delinitialise the dev. at slave detach.  The complement of ioctl_dev_init.
 */
static int ioctl_dev_exit(struct v4l2_int_device *s)
{
	return 0;
}

/**
 * ioctl_dev_init - V4L2 sensor interface handler for vidioc_int_dev_init_num
 * @s: pointer to standard V4L2 device structure
 *
 * Initialise the device when slave attaches to the master.  Returns 0 if
 * ov9740 device could be found, otherwise returns appropriate error.
 */
static int ioctl_dev_init(struct v4l2_int_device *s)
{
	struct ov9740_sensor *sensor = s->priv;
	struct i2c_client *client = sensor->i2c_client;
	int err;

	err = ov9740_power_on(s);
	if (err)
		return -ENODEV;

	err = ov9740_detect(client);
	if (err < 0) {
		v4l_err(client, "Unable to detect "
				OV9740_DRIVER_NAME " sensor\n");

		/*
		 * Turn power off before leaving the function.
		 * If not, CAM Pwrdm will be ON which is not needed
		 * as there is no sensor detected.
		 */
		ov9740_power_off(s);

		return err;
	}
	sensor->ver = err;
	v4l_info(client, OV9740_DRIVER_NAME
		" chip version 0x%02x detected\n", sensor->ver);

	err = ov9740_power_off(s);
	if (err)
		return -ENODEV;

	return 0;
}

/**
 * ioctl_enum_framesizes - V4L2 sensor if handler for vidioc_int_enum_framesizes
 * @s: pointer to standard V4L2 device structure
 * @frms: pointer to standard V4L2 framesizes enumeration structure
 *
 * Returns possible framesizes depending on choosen pixel format
 **/
static int ioctl_enum_framesizes(struct v4l2_int_device *s,
				 struct v4l2_frmsizeenum *frms)
{
	int ifmt;

	for (ifmt = 0; ifmt < NUM_CAPTURE_FORMATS; ifmt++) {
		if (frms->pixel_format == ov9740_formats[ifmt].pixelformat)
			break;
	}
	/* Is requested pixelformat not found on sensor? */
	if (ifmt == NUM_CAPTURE_FORMATS)
		return -EINVAL;

	/* Check that the index we are being asked for is not
	   out of bounds. */
	if (frms->index >= OV9740_MODES_COUNT)
		return -EINVAL;

	frms->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	frms->discrete.width =
		ov9740_settings[frms->index].frame.x_output_size;
	frms->discrete.height =
		ov9740_settings[frms->index].frame.y_output_size;

	return 0;
}

static const struct v4l2_fract ov9740_frameintervals[] = {
	{ .numerator = 1, .denominator = 30 },
};

static int ioctl_enum_frameintervals(struct v4l2_int_device *s,
				     struct v4l2_frmivalenum *frmi)
{
	int ifmt;

	/* Check that the requested format is one we support */
	for (ifmt = 0; ifmt < NUM_CAPTURE_FORMATS; ifmt++) {
		if (frmi->pixel_format == ov9740_formats[ifmt].pixelformat)
			break;
	}

	if (ifmt == NUM_CAPTURE_FORMATS)
		return -EINVAL;

	/* Check that the index we are being asked for is not
	   out of bounds. */
	if (frmi->index >= ARRAY_SIZE(ov9740_frameintervals))
		return -EINVAL;

	/* Make sure that the 8MP size reports a max of 10fps */
	if (frmi->width == OV9740_IMAGE_WIDTH_MAX &&
	    frmi->height == OV9740_IMAGE_HEIGHT_MAX) {
		if (frmi->index != 0)
			return -EINVAL;
	}

	frmi->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	frmi->discrete.numerator =
				ov9740_frameintervals[frmi->index].numerator;
	frmi->discrete.denominator =
				ov9740_frameintervals[frmi->index].denominator;

	return 0;
}

static struct v4l2_int_ioctl_desc ov9740_ioctl_desc[] = {
	{ .num = vidioc_int_enum_framesizes_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_enum_framesizes},
	{ .num = vidioc_int_enum_frameintervals_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_enum_frameintervals},
	{ .num = vidioc_int_dev_init_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_dev_init},
	{ .num = vidioc_int_dev_exit_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_dev_exit},
	{ .num = vidioc_int_s_power_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_s_power },
	{ .num = vidioc_int_g_priv_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_g_priv },
	{ .num = vidioc_int_init_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_init },
	{ .num = vidioc_int_enum_fmt_cap_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_enum_fmt_cap },
	{ .num = vidioc_int_try_fmt_cap_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_try_fmt_cap },
	{ .num = vidioc_int_g_fmt_cap_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_g_fmt_cap },
	{ .num = vidioc_int_s_fmt_cap_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_s_fmt_cap },
	{ .num = vidioc_int_g_parm_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_g_parm },
	{ .num = vidioc_int_s_parm_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_s_parm },
	{ .num = vidioc_int_queryctrl_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_queryctrl },
	{ .num = vidioc_int_g_ctrl_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_g_ctrl },
	{ .num = vidioc_int_s_ctrl_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_s_ctrl },
	{ .num = vidioc_int_priv_g_pixclk_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_priv_g_pixclk },
	{ .num = vidioc_int_priv_g_activesize_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_priv_g_activesize },
	{ .num = vidioc_int_priv_g_fullsize_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_priv_g_fullsize },
	{ .num = vidioc_int_priv_g_pixelsize_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_priv_g_pixelsize },
	{ .num = vidioc_int_priv_g_pixclk_active_num,
	  .func = (v4l2_int_ioctl_func *)ioctl_priv_g_pixclk_active },
};

static struct v4l2_int_slave ov9740_slave = {
	.ioctls = ov9740_ioctl_desc,
	.num_ioctls = ARRAY_SIZE(ov9740_ioctl_desc),
};

static struct v4l2_int_device ov9740_int_device = {
	.module = THIS_MODULE,
	.name = OV9740_DRIVER_NAME,
	.priv = &ov9740,
	.type = v4l2_int_type_slave,
	.u = {
		.slave = &ov9740_slave,
	},
};

/**
 * ov9740_probe - sensor driver i2c probe handler
 * @client: i2c driver client device structure
 *
 * Register sensor as an i2c client device and V4L2
 * device.
 */
static int __devinit ov9740_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct ov9740_sensor *sensor = &ov9740;
	int err;

	if (i2c_get_clientdata(client))
		return -EBUSY;

	sensor->pdata = client->dev.platform_data;

	if (!sensor->pdata) {
		v4l_err(client, "no platform data?\n");
		return -ENODEV;
	}

	sensor->v4l2_int_device = &ov9740_int_device;
	sensor->i2c_client = client;

	i2c_set_clientdata(client, sensor);

	/* Make the default capture format 720p V4L2_PIX_FMT_SBGGR10 */
	sensor->pix.width = OV9740_IMAGE_WIDTH_MAX;
	sensor->pix.height = OV9740_IMAGE_HEIGHT_MAX;
	sensor->pix.pixelformat = V4L2_PIX_FMT_SBGGR10;
    sensor->timeperframe = ov9740_frameintervals[0];

	err = v4l2_int_device_register(sensor->v4l2_int_device);
	if (err)
		i2c_set_clientdata(client, NULL);

	return 0;
}

/**
 * ov9740_remove - sensor driver i2c remove handler
 * @client: i2c driver client device structure
 *
 * Unregister sensor as an i2c client device and V4L2
 * device.  Complement of ov9740_probe().
 */
static int __exit ov9740_remove(struct i2c_client *client)
{
	struct ov9740_sensor *sensor = i2c_get_clientdata(client);

	if (!client->adapter)
		return -ENODEV;	/* our client isn't attached */

	v4l2_int_device_unregister(sensor->v4l2_int_device);
	i2c_set_clientdata(client, NULL);

	return 0;
}

static const struct i2c_device_id ov9740_id[] = {
	{ OV9740_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, ov9740_id);

static struct i2c_driver ov9740sensor_i2c_driver = {
	.driver = {
		.name = OV9740_DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = ov9740_probe,
	.remove = __exit_p(ov9740_remove),
	.id_table = ov9740_id,
};

static struct ov9740_sensor ov9740 = {
	.timeperframe = {
		.numerator = 1,
		.denominator = 30,
	},
};

/**
 * ov9740sensor_init - sensor driver module_init handler
 *
 * Registers driver as an i2c client driver.  Returns 0 on success,
 * error code otherwise.
 */
static int __init ov9740sensor_init(void)
{
	int err;

	err = i2c_add_driver(&ov9740sensor_i2c_driver);
	if (err) {
		printk(KERN_ERR "Failed to register" OV9740_DRIVER_NAME ".\n");
		return err;
	}
	return 0;
}
late_initcall(ov9740sensor_init);

/**
 * ov9740sensor_cleanup - sensor driver module_exit handler
 *
 * Unregisters/deletes driver as an i2c client driver.
 * Complement of ov9740sensor_init.
 */
static void __exit ov9740sensor_cleanup(void)
{
	i2c_del_driver(&ov9740sensor_i2c_driver);
}
module_exit(ov9740sensor_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ov9740 camera sensor driver");

