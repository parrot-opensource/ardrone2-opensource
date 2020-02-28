/*
 * A V4L2 driver for OmniVision OV7670 cameras.
 *
 * Copyright 2010 One Laptop Per Child
 *
 * This file may be distributed under the terms of the GNU General
 * Public License, version 2.
 */

#ifndef __OV7670_H
#define __OV7670_H

#include <media/v4l2-subdev.h>


/**
 * struct ov7670_platform_data - platform data values and access functions
 * @power_set: Power state access function, zero is off, non-zero is on.
 * @set_xclk: enable xclk with a certain frequency
 */
struct ov7670_platform_data {

	int min_width;			/* Filter out smaller sizes */
	int min_height;			/* Filter out smaller sizes */
	int clock_speed;		/* External clock speed (MHz) */
	bool use_smbus;			/* Use smbus I/O instead of I2C */

	int (*set_power)(struct v4l2_subdev *s, int power);
	u32 (*set_xclk)(struct v4l2_subdev *s, u32 xclkfreq);
};

#endif
