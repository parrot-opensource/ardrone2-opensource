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

#define OV9740_I2C_ADDR 0x10

#define OV9740_BLACK_LEVEL_AVG	64
/**
 * struct ov9740_platform_data - platform data values and access functions
 * @power_set: Power state access function, zero is off, non-zero is on.
 * @priv_data_set: device private data (pointer) access function
 * @set_xclk: enable xclk with a certain frequency
 */
struct ov9740_platform_data {
	int (*power_set)(struct v4l2_int_device *s, enum v4l2_power power);
	int (*ifparm)(struct v4l2_ifparm *p);
	int (*priv_data_set)(struct v4l2_int_device *s, void *);
	u32 (*set_xclk)(struct v4l2_int_device *s, u32 xclkfreq);
	int (*cfg_interface_bridge)(u32);
	int (*csi2_lane_count)(struct v4l2_int_device *s, int count);
	int (*csi2_cfg_vp_out_ctrl)(struct v4l2_int_device *s, u8 vp_out_ctrl);
	int (*csi2_ctrl_update)(struct v4l2_int_device *s, bool);
	int (*csi2_cfg_virtual_id)(struct v4l2_int_device *s, u8 ctx, u8 id);
	int (*csi2_ctx_update)(struct v4l2_int_device *s, u8 ctx, bool);
	int (*csi2_calc_phy_cfg0)(struct v4l2_int_device *s, u32, u32, u32);
};

#endif /* ifndef OV9740_H */
