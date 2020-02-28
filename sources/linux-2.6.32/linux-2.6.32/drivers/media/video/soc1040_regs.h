/*
 * soc1040_regs.h
 *
 * Register definitions for the SOC1040 Sensor.
 *
 * Leverage MT9P012.h
 *
 * Copyright (C) 2008 Hewlett Packard.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef SOC1040_REGS_H
#define SOC1040_REGS_H

/* The ID values we are looking for */
#define SOC1040_CHIP_ID			0x2481

/* SOC1040 has 8/16/32 I2C registers */
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
#define SOC1040_TOK_TERM 		0xFF

/* delay token for reg list */
#define SOC1040_TOK_DELAY		100

/* CSI2 Virtual ID */
//#define SOC1040_CCP2_CHANNEL_ID		0x0
#define SOC1040_CSI2_VIRTUAL_ID		0x0

#define SOC1040_CLKRC			0x11

/* Used registers */
#define SOC1040_REG_MODEL_ID		0x0000
#define SOC1040_REG_SW_RESET		0x301A
#define SOC1040_REG_CCP2_CHANNEL_ID	0xC984

/*
 * The nominal xclk input frequency of the SOC1040 is 24MHz
 */
#define SOC1040_XCLK_MIN   	6000000
#define SOC1040_XCLK_MAX   	45000000
#define SOC1040_XCLK_NOM_1 	24000000
#define SOC1040_XCLK_NOM_2 	24000000

/* FPS Capabilities */
#define SOC1040_MIN_FPS		7
#define SOC1040_DEF_FPS		15
#define SOC1040_MAX_FPS		30

#define I2C_RETRY_COUNT		5

/* Still capture 8 MP */
#define SOC1040_IMAGE_WIDTH_MAX		1280
#define SOC1040_IMAGE_HEIGHT_MAX	960

/* Analog gain values */
#define SOC1040_EV_MIN_GAIN		0
#define SOC1040_EV_MAX_GAIN		30
#define SOC1040_EV_DEF_GAIN		21
#define SOC1040_EV_GAIN_STEP		1
/* maximum index in the gain EVT */
#define SOC1040_EV_TABLE_GAIN_MAX	30

/* Exposure time values */
#define SOC1040_MIN_EXPOSURE		250
#define SOC1040_MAX_EXPOSURE		128000
#define SOC1040_DEF_EXPOSURE	    	33000
#define SOC1040_EXPOSURE_STEP		50

/* Test Pattern Values */
#define SOC1040_MIN_TEST_PATT_MODE	0
#define SOC1040_MAX_TEST_PATT_MODE	4
#define SOC1040_MODE_TEST_PATT_STEP	1

#define SOC1040_TEST_PATT_SOLID_COLOR 	1
#define SOC1040_TEST_PATT_COLOR_BAR	2
#define SOC1040_TEST_PATT_PN9		4

#define SOC1040_MAX_FRAME_LENGTH_LINES	0xFFFF

#define SENSOR_DETECTED		1
#define SENSOR_NOT_DETECTED	0

#define NUM_IMAGE_SIZES ARRAY_SIZE(soc1040_sizes)
/**
 * struct soc1040_reg - soc1040 register format
 * @reg: 16-bit offset to register
 * @val: 8/16/32-bit register value
 * @length: length of the register
 *
 * Define a structure for SOC1040 register initialization values
 */
struct soc1040_reg {
	u16 	reg;
	u32 	val;
	u16	length;
};

enum soc1040_image_size {
	HALF_HD
};

#define NUM_IMAGE_SIZES ARRAY_SIZE(soc1040_sizes)
/**
 * struct soc1040_capture_size - image capture size information
 * @width: image width in pixels
 * @height: image height in pixels
 */
struct soc1040_capture_size {
	unsigned long width;
	unsigned long height;
};

/**
 * struct struct clk_settings - struct for storage of sensor
 * clock settings
 */
struct soc1040_clk_settings {
	u16	div_m;
	u16	div_p;
	u16	div_n;
};

/**
 * struct struct mipi_settings - struct for storage of sensor
 * mipi settings
 */
struct soc1040_mipi_settings {
	u16	data_lanes;
	u16	ths_zero;
	u16	ths_exit;
	u16	ths_trail;
	u16	tclk_pre;
	u16	tclk_post;
	u16	tclk_trail;
	u16	tclk_zero;
	u16	t_lpx;
	u16	t_init;
	u16	ths_settle_lower;
	u16	ths_settle_upper;
};

/**
 * struct struct frame_settings - struct for storage of sensor
 * frame settings
 */
struct soc1040_frame_settings {
	u16	frame_len_lines_min;
	u16	frame_len_lines;
	u16	line_len_pck;
	u16	x_addr_start;
	u16	x_addr_end;
	u16	y_addr_start;
	u16	y_addr_end;
	u16	x_output_size;
	u16	y_output_size;
	u16	x_even_inc;
	u16	x_odd_inc;
	u16	y_even_inc;
	u16	y_odd_inc;
	u16	v_mode_add;
	u16	h_mode_add;
	u16	h_add_ave;
};

/**
 * struct struct soc1040_sensor_settings - struct for storage of
 * sensor settings.
 */
struct soc1040_sensor_settings {
	struct soc1040_clk_settings clk;
	struct soc1040_mipi_settings mipi;
	struct soc1040_frame_settings frame;
};

#endif /* ifndef SOC1040_REGS_H */
