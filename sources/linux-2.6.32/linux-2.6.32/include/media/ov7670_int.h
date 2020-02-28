/*
 * ov7670.h - Shared settings for the ov7670 CameraChip.
 *
 * Contributors:
 *   Julien BERAUD
 *
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef OV7670_H
#define OV7670_H

#include <media/v4l2-int-device.h>

/*Supposed to be 42 for read and 43 for write but the original ov7670 driver
* has been implemented this way*/
#define OV7670_I2C_ADDR 0x42

/**
 * struct ov7670_platform_data - platform data values and access functions
 * @power_set: Power state access function, zero is off, non-zero is on.
 * @priv_data_set: device private data (pointer) access function
 * @set_xclk: enable xclk with a certain frequency
 */
struct ov7670_platform_data {
	int (*power_set)(struct v4l2_int_device *s, enum v4l2_power power);
	int (*priv_data_set)(struct v4l2_int_device *s, void *);
	u32 (*set_xclk)(struct v4l2_int_device *s, u32 xclkfreq);
};

#endif /* ifndef OV7670_H */
