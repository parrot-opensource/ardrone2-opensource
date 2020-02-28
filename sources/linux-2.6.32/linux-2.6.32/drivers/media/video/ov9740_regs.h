/*
 * drivers/media/video/ov9740_regs.h
 *
 * OV 9740 camera driver registers
 *
 *   
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef OV9740_REGS_H
#define OV9740_REGS_H


/* The ID values we are looking for */
#define OV9740_MOD_ID			0x9740

/* OV9740 has 8/16/32 I2C registers */
#define I2C_8BIT			1
#define I2C_16BIT			2
#define I2C_32BIT			4

/* Terminating list entry for reg */
#define I2C_REG_TERM		0xFFFF
/* Terminating list entry for val */
#define I2C_VAL_TERM		0xFFFFFFFF
/* Terminating list entry for len */
#define I2C_LEN_TERM		0xFFFF

/* terminating token for reg list */
#define OV9740_TOK_TERM 		0xFF

/* delay token for reg list */
#define OV9740_TOK_DELAY		100

/* CSI2 Virtual ID */
#define OV9740_CSI2_VIRTUAL_ID	0x0

/* General Status Registers */
#define OV9740_MODEL_ID		0x0000
#define OV9740_REVISION_NUMBER		0x0002
#define OV9740_MANUFACTURER_ID		0x0003
#define OV9740_SMIA_VERSION		0x0004

/* General Setup Registers */
#define OV9740_MODE_SELECT		0x0100
#define OV9740_IMAGE_ORT		0x0101
#define OV9740_SOFTWARE_RESET		0x0103
#define OV9740_GRP_PARAM_HOLD		0x0104
#define OV9740_MSK_CORRUP_FM		0x0105

#define OV9740_FINE_INT_TIME		0x0200
#define OV9740_COARSE_INT_TIME		0x0202

#define OV9740_ANALOG_GAIN_GLOBAL	0x0204

/* Timing Setting */
#define OV9740_FRM_LENGTH_LN		0x0340 /*VTS*/
#define OV9740_LN_LENGTH_PCK		0x0342 /*HTS*/
#define OV9740_X_ADDR_START			0x0344
#define OV9740_Y_ADDR_START			0x0346
#define OV9740_X_ADDR_END			0x0348
#define OV9740_Y_ADDR_END			0x034A
#define OV9740_X_OUTPUT_SIZE		0x034C
#define OV9740_Y_OUTPUT_SIZE		0x034E

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
#define OV9740_AEC_MAXEXPO_60		0x3A02
#define OV9740_AEC_B50_STEP		0x3A08
#define OV9740_AEC_B60_STEP		0x3A0A
#define OV9740_AEC_CTRL0D		0x3A0D
#define OV9740_AEC_CTRL0E		0x3A0E
#define OV9740_AEC_MAXEXPO_50		0x3A14

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
#define OV9740_VFIFO_READ_START		0x4608

/* DVP Control */
#define OV9740_DVP_VSYNC_CTRL02		0x4702
#define OV9740_DVP_VSYNC_MODE		0x4704
#define OV9740_DVP_VSYNC_CTRL06		0x4706

/* PLL Setting */
#define OV9740_PLL_MODE_CTRL01		0x3104
#define OV9740_VT_PIX_CLK_DIV		0x0300
#define OV9740_VT_SYS_CLK_DIV		0x0302
#define OV9740_PRE_PLL_CLK_DIV		0x0304
#define OV9740_PLL_MULTIPLIER		0x0306
#define OV9740_PLL_CTRL3010		0x3010
#define OV9740_VFIFO_CTRL00		0x460E

/* Pixel format */
#define OV9740_PIX_FORMAT	0x4300

/* ISP Control */
#define OV9740_ISP_CTRL00		0x5000
#define OV9740_ISP_CTRL01		0x5001
#define OV9740_ISP_CTRL03		0x5003
#define OV9740_ISP_CTRL04		0x5004
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
#define OV9740_MIPI_2_LANE		0x301F
#define OV9740_MIPI_CTRL14		0x4814

/* Test pattern */
#define OV9740_TEST_PATT_MODE	0x0600
#define OV9740_TEST_PATT_RED	0x0602
#define OV9740_TEST_PATT_GREENR	0x0604
#define OV9740_TEST_PATT_BLUE	0x0606
#define OV9740_TEST_PATT_GREENB	0x0608

/* supported resolutions */
#define RES_1280x720_W			1280
#define RES_1280x720_H			720
#define RES_640x480_W			640
#define RES_640x480_H			480

/*
 * The nominal xclk input frequency of the OV9740 is 18MHz, maximum
 * frequency is 45MHz, and minimum frequency is 6MHz.
 */
#define OV9740_XCLK_MIN   	6000000
#define OV9740_XCLK_MAX   	27000000
#define OV9740_XCLK_NOM_1 	24000000
#define OV9740_XCLK_NOM_2 	12000000

/* FPS Capabilities */
#define OV9740_MIN_FPS		7
#define OV9740_DEF_FPS		15
#define OV9740_MAX_FPS		30

#define OV9740_IMAGE_WIDTH_MAX	1280
#define OV9740_IMAGE_HEIGHT_MAX	720
#define I2C_RETRY_COUNT		5

/* Analog gain values */
#define OV9740_EV_MIN_GAIN		0
#define OV9740_EV_MAX_GAIN		30
#define OV9740_EV_DEF_GAIN		21
#define OV9740_EV_GAIN_STEP		1
/* maximum index in the gain EVT */
#define OV9740_EV_TABLE_GAIN_MAX	30

/* Exposure time values */
#define OV9740_MIN_EXPOSURE		250
#define OV9740_MAX_EXPOSURE		128000
#define OV9740_DEF_EXPOSURE	    33000
#define OV9740_EXPOSURE_STEP	50

/* Test Pattern Values */
#define OV9740_MIN_TEST_PATT_MODE	0
#define OV9740_MAX_TEST_PATT_MODE	4
#define OV9740_MODE_TEST_PATT_STEP	1

#define OV9740_TEST_PATT_SOLID_COLOR 	1
#define OV9740_TEST_PATT_COLOR_BAR	2
#define OV9740_TEST_PATT_PN9		4

#define OV9740_MAX_FRAME_LENGTH_LINES	0xFFFF

#define SENSOR_DETECTED		1
#define SENSOR_NOT_DETECTED	0

/**
 * struct ov9740_reg - ov9740 register format
 * @reg: 16-bit offset to register
 * @val: 8/16/32-bit register value
 * @length: length of the register
 *
 * Define a structure for OV9740 register initialization values
 */
struct ov9740_reg {
	u16 	reg;
	u32 	val;
	u16	length;
};

/**
 * struct struct clk_settings - struct for storage of sensor
 * clock settings
 */
struct ov9740_clk_settings {
	u16	pre_pll_div;
	u16	pll_mult;
	u16	vt_pix_clk_div;
	u16	vt_sys_clk_div;
};

/**
 * struct struct mipi_settings - struct for storage of sensor
 * mipi settings
 */
struct ov9740_mipi_settings {
	u16	data_lanes;
	u16	ths_settle_lower;
	u16	ths_settle_upper;
};

/**
 * struct struct frame_settings - struct for storage of sensor
 * frame settings
 */
struct ov9740_frame_settings {
	u16	frame_len_lines_min;
	u16	frame_len_lines;
	u16	line_len_pck;
	u16	x_addr_start;
	u16	x_addr_end;
	u16	y_addr_start;
	u16	y_addr_end;
	u16	x_output_size;
	u16	y_output_size;
	u8	enable_scale;
	u16 x_scale;
	u16 y_scale;
	u16 fifo_read_start;
};

/**
 * struct struct ov9740_sensor_settings - struct for storage of
 * sensor settings.
 */
struct ov9740_sensor_settings {
	struct ov9740_clk_settings clk;
	struct ov9740_mipi_settings mipi;
	struct ov9740_frame_settings frame;
};

/**
 * struct struct ov9740_clock_freq - struct for storage of sensor
 * clock frequencies
 */
struct ov9740_clock_freq {
	u32 vco_clk;
	u32 mipi_clk;
	u32 ddr_clk;
	u32 vt_pix_clk;
};

#endif

