/*
 * soc1040.h - Shared settings for the soc1040 CameraChip.
 *
 * Contributors:
 *   Julien BERAUD
 *
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef SOC1040_H
#define SOC1040_H

#include <media/v4l2-subdev.h>


/**
 * struct soc1040_platform_data - platform data values and access functions
 * @set_xclk: enable xclk with a certain frequency
 */
struct soc1040_platform_data {

	unsigned int ext_clk;			/* sensor external clk */

	void (*csi_configure)(struct v4l2_subdev *sd, int mode);
	int (*set_xclk)(struct v4l2_subdev *sd, int hz);
	int (*set_power)(struct v4l2_subdev *sd, int set);
};



#endif /* ifndef SOC1040_H */
