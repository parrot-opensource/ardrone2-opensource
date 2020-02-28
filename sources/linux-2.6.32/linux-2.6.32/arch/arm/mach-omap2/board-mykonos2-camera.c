/**
 * arch/arm/mach-omap2/board-myconos2-camera.c
 *
 * Copyright (C) 2011 MM Solutions
 * Contact: Stanimir Varbanov <svarbanov@mm-sol.com>
 *
 * Based on board-zoom2-camera.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/delay.h>
#include <linux/i2c.h>

#include <plat/gpio.h>
#include <plat/control.h>
#include <plat/omap-pm.h>

#include <media/sensor-dummy.h>
#include <media/soc1040.h>
#include <media/mt9p031.h>

#include "../../../drivers/media/video/isp/isp.h"
#include "../../../drivers/media/video/isp/ispreg.h"
#include "../../../drivers/media/video/isp/ispcsi2.h"

#include "../../../drivers/media/video/ov7670.h"
#include "mux.h"

extern void __init m2_gpio_init(int gpio, int val);

/* horizontal positioned sensor */
#define SOC1040_NAME			"soc1040"
#define SOC1040_I2C_ADDR			0x5d
#define SOC1040_I2C_BUS_NUM		3
#define SOC1040_XCLK			ISP_XCLK_B
#define SOC1040_RESET_GPIO 		92
#define SOC1040_PWDN_GPIO 		93
#define SOC1040_XCLK_FREQ		(24 * 1000 * 1000)

#define SOC1040_CSI2_CLOCK_POLARITY	1	/* -/+ pin order */
#define SOC1040_CSI2_DATA0_POLARITY	1	/* -/+ pin order */
#define SOC1040_CSI2_CLOCK_LANE		1	/* Clock lane position: 1 */
#define SOC1040_CSI2_DATA0_LANE		2	/* Data0 lane position: 2 */
#define SOC1040_CSI2_PHY_THS_TERM	2
#define SOC1040_CSI2_PHY_THS_SETTLE	23
#define SOC1040_CSI2_PHY_TCLK_TERM	0
#define SOC1040_CSI2_PHY_TCLK_MISS	1
#define SOC1040_CSI2_PHY_TCLK_SETTLE	14


/* vertical positioned sensor */
#define OV7670_NAME			"ov7670"
/*Supposed to be 42 for read and 43 for write but the original ov7670 driver
* has been implemented this way*/
#define OV7670_I2C_ADDR			(0x42 >> 1)
#define OV7670_I2C_BUS_NUM		2
#define OV7670_XCLK			ISP_XCLK_A
#define OV7670_PWDN_GPIO 		91
#define OV7670_XCLK_FREQ 		(24 * 1000 * 1000)

#define ISP_MYKONOS2_MCLK		216000000


static void omap_mykonos2_cam_init(void)
{
	/* Vertical camera parallel interface */
	omap_mux_init_gpio(OV7670_PWDN_GPIO, 1);
	m2_gpio_init(OV7670_PWDN_GPIO, 1);

	omap_mux_init_signal("cam_xclka", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("cam_d2", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d3", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d4", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d5", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d6", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d7", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d8", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d9", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_pclk", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_vs", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_hs", OMAP_PIN_INPUT);

	m2_gpio_init(OV7670_PWDN_GPIO, 0);

	/* Horizontal camera csi-2 interface */
	omap_mux_init_gpio(SOC1040_PWDN_GPIO, 0);
	m2_gpio_init(SOC1040_PWDN_GPIO, 0);

	omap_mux_init_gpio(SOC1040_RESET_GPIO, 0);
	m2_gpio_init(SOC1040_RESET_GPIO, 0);

	omap_mux_init_signal("cam_xclkb", OMAP_PIN_OUTPUT);
	omap_mux_init_signal("csi2_dx0", OMAP_PIN_INPUT);
	omap_mux_init_signal("csi2_dy0", OMAP_PIN_INPUT);
	omap_mux_init_signal("csi2_dx1", OMAP_PIN_INPUT);
	omap_mux_init_signal("csi2_dy1", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d0.csi2_dx2", OMAP_PIN_INPUT);
	omap_mux_init_signal("cam_d1.csi2_dy2", OMAP_PIN_INPUT);
}

int omap3_init_camera(void *pdata);

static struct isp_csiphy_lanes_cfg m2_soc1040_csi2_lanecfg = {
	.clk = {
		.pol = SOC1040_CSI2_CLOCK_POLARITY,
		.pos = SOC1040_CSI2_CLOCK_LANE,
	},
	.data[0] = {
		.pol = SOC1040_CSI2_DATA0_POLARITY,
		.pos = SOC1040_CSI2_DATA0_LANE,
	},
};

/*
 * THS_TERM: Programmed value = ceil(12.5 ns/DDRClk period) - 1.
 * THS_SETTLE: Programmed value = ceil(90 ns/DDRClk period) + 3.
 */
#define THS_TERM_D 2000000
#define THS_TERM(ddrclk_khz)					\
(								\
	((25 * (ddrclk_khz)) % THS_TERM_D) ? 			\
		((25 * (ddrclk_khz)) / THS_TERM_D) :		\
		((25 * (ddrclk_khz)) / THS_TERM_D) - 1		\
)

#define THS_SETTLE_D 1000000
#define THS_SETTLE(ddrclk_khz)					\
(								\
	((90 * (ddrclk_khz)) % THS_SETTLE_D) ? 			\
		((90 * (ddrclk_khz)) / THS_SETTLE_D) + 4 :	\
		((90 * (ddrclk_khz)) / THS_SETTLE_D) + 3	\
)

/*
 * TCLK values are OK at their reset values
 */
#define TCLK_TERM	0
#define TCLK_MISS	1
#define TCLK_SETTLE	14

static void m2_soc1040_csi2_configure(struct v4l2_subdev *subdev,
					    int mode)
{
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);
	struct isp_csiphy_dphy_cfg csi2phy;

	csi2phy.ths_term = SOC1040_CSI2_PHY_THS_TERM;
	csi2phy.ths_settle = SOC1040_CSI2_PHY_THS_SETTLE;
	csi2phy.tclk_term = SOC1040_CSI2_PHY_TCLK_TERM;
	csi2phy.tclk_miss = SOC1040_CSI2_PHY_TCLK_MISS;
	csi2phy.tclk_settle = SOC1040_CSI2_PHY_TCLK_SETTLE;

	isp->platform_cb.csiphy_config(&isp->isp_csiphy2, &csi2phy,
					&m2_soc1040_csi2_lanecfg);
}

static int m2_soc1040_set_xclk(struct v4l2_subdev *subdev, int hz)
{
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);

	isp->platform_cb.set_xclk(isp, hz, SOC1040_XCLK);

	return hz;
}

static int m2_soc1040_set_power(struct v4l2_subdev *subdev, int on)
{
	if (on) {

		gpio_set_value(SOC1040_PWDN_GPIO, 0);

		udelay(1000);

		gpio_set_value(SOC1040_RESET_GPIO, 1);

		udelay(100);

		/* have to put sensor to reset to guarantee detection */
		gpio_set_value(SOC1040_RESET_GPIO, 0);
		udelay(1500);

		/* nRESET is active LOW. set HIGH to release reset */
		gpio_set_value(SOC1040_RESET_GPIO, 1);
		mdelay(20);

	} else {

		gpio_set_value(SOC1040_RESET_GPIO, 0);
		udelay(100);

		gpio_set_value(SOC1040_PWDN_GPIO, 1);
		udelay(100);
	}

	return 0;
}

static struct soc1040_platform_data m2_soc1040_platform_data = {
	.ext_clk		= SOC1040_XCLK_FREQ,
	.csi_configure		= m2_soc1040_csi2_configure,
	.set_xclk		= m2_soc1040_set_xclk,
	.set_power              = m2_soc1040_set_power,
};

static struct i2c_board_info m2_soc1040_i2c_board_info = {
	I2C_BOARD_INFO(SOC1040_NAME, SOC1040_I2C_ADDR),
	.platform_data = &m2_soc1040_platform_data,
};

static struct isp_subdev_i2c_board_info m2_soc1040_isp_subdev[] = {
	{
		.board_info = &m2_soc1040_i2c_board_info,
		.i2c_adapter_id = SOC1040_I2C_BUS_NUM,
	},
	{ NULL, 0,},
};

#if defined(CONFIG_VIDEO_MT9P031) || defined(CONFIG_VIDEO_MT9P031_MODULE)
#define MT9P031_I2C_ADDR		0x5D
static int m2_mt9p031_set_power(struct v4l2_subdev *subdev, int power)
{
	switch (power) {
	case 1:
		/* out of standby */
		gpio_set_value(OV7670_PWDN_GPIO, 0);
		mdelay(20);
		break;
	case 0:
		gpio_set_value(OV7670_PWDN_GPIO, 1);
		udelay(100);
		break;
	}
	return 0;
}

static int m2_mt9p031_set_xclk(struct v4l2_subdev *subdev, u32 hz)
{	
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);
	// printk("mt9p031_set_xclk %d\n", hz);
	return isp->platform_cb.set_xclk(isp, hz, ISP_XCLK_A);
}

struct mt9p031_platform_data m2_mt9p031_platform_data = {
	.set_xclk 		= m2_mt9p031_set_xclk,
	.set_power	= m2_mt9p031_set_power,
	.ext_freq		= 12000000,  // 21000000
	.target_freq	= 48000000,
	.version		= MT9P031_COLOR_VERSION
};

static struct i2c_board_info m2_mt9p031_i2c_board_info = {
	I2C_BOARD_INFO("mt9p031", MT9P031_I2C_ADDR),
	.platform_data = &m2_mt9p031_platform_data,
};


static struct isp_subdev_i2c_board_info m2_mt9p031_isp_subdev[] = {
	{
		.board_info = &m2_mt9p031_i2c_board_info,
		.i2c_adapter_id = OV7670_I2C_BUS_NUM,
	},
	{ NULL, 0,},
};

static struct isp_v4l2_subdevs_group m2_camera_subdevs[] = {
	{
		.subdevs = m2_soc1040_isp_subdev,
		.interface = ISP_INTERFACE_CSI2A_PHY2,
		.bus = {
			.csi2 = {
				.crc		= 1,
				.vpclk_div	= 1,
			}
		},
	},
	{
		.subdevs = m2_mt9p031_isp_subdev,
		.interface = ISP_INTERFACE_PARALLEL,
		.bus = {
			.parallel = {
				.data_lane_shift 	= 1, // 2,
				.clk_pol 		= 1,   /* Pixel clock polarity (Non Inverted) Data captured on the falling edge of PIXCLK */
				.bridge		= 0,
			}
		},
	},
	{ NULL, 0, },
};

#else

static int m2_ov7670_set_power(struct v4l2_subdev *subdev, int power)
{
	switch (power) {
	case 1:
		/* out of standby */
		gpio_set_value(OV7670_PWDN_GPIO, 0);
		mdelay(20);
		break;
	case 0:
		gpio_set_value(OV7670_PWDN_GPIO, 1);
		udelay(100);
		break;
	}
	/* Save powerstate to know what was before calling POWER_ON. */
	return 0;
}
static u32 m2_ov7670_set_xclk(struct v4l2_subdev *subdev, u32 hz)
{
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);
	isp->platform_cb.set_xclk(isp, hz, OV7670_XCLK);
	return hz;
}

struct ov7670_platform_data m2_ov7670_platform_data = {
	.use_smbus	= 1,
	.clock_speed	= OV7670_XCLK_FREQ,
	.set_power 	= m2_ov7670_set_power,
	.set_xclk 	= m2_ov7670_set_xclk,
};

static struct i2c_board_info m2_ov7670_i2c_board_info = {
	I2C_BOARD_INFO(OV7670_NAME, OV7670_I2C_ADDR),
	.platform_data = &m2_ov7670_platform_data,
};

static struct isp_subdev_i2c_board_info m2_ov7670_isp_subdev[] = {
	{
		.board_info = &m2_ov7670_i2c_board_info,
		.i2c_adapter_id = OV7670_I2C_BUS_NUM,
	},
	{ NULL, 0,},
};

static struct isp_v4l2_subdevs_group m2_camera_subdevs[] = {
	{
		.subdevs = m2_soc1040_isp_subdev,
		.interface = ISP_INTERFACE_CSI2A_PHY2,
		.bus = {
			.csi2 = {
				.crc		= 1,
				.vpclk_div	= 1,
			}
		},
	},
	{
		.subdevs = m2_ov7670_isp_subdev,
		.interface = ISP_INTERFACE_PARALLEL,
		.bus = {
			.parallel = {
				.data_lane_shift 	= 1,
				.clk_pol 		= 0,
				.bridge		= 2,
			}
		},
	},
	{ NULL, 0, },
};
#endif 

static struct isp_platform_data m2_isp_platform_data = {
	.subdevs = m2_camera_subdevs,
};

void __init mykonos2_cameras_init(void)
{
	omap_mykonos2_cam_init();

	if (omap3_init_camera(&m2_isp_platform_data) < 0)
		printk(KERN_WARNING
		       "%s: unable to register isp platform device\n",
		       __func__);

}

