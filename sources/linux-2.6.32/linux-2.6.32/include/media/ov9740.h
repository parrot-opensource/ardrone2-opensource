/*
 * ov9740.h - Shared settings for the ov9740 CameraChip.
 *
 * Contributors:
 *   Julien BERAUD
 *
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef OV9740_H
#define OV9740_H

#include <media/v4l2-subdev.h>


/**
 * struct ov9740_platform_data - platform data values and access functions
 * @set_xclk: enable xclk with a certain frequency
 */
struct ov9740_platform_data {

	unsigned int ext_clk;			/* sensor external clk */

	void (*csi_configure)(struct v4l2_subdev *sd, int mode);
	int (*set_xclk)(struct v4l2_subdev *sd, int hz);
	int (*set_power)(struct v4l2_subdev *sd, int set);
};



#endif /* ifndef OV9740_H */
