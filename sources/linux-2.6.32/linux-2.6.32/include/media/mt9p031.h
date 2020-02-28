#ifndef MT9P031_H
#define MT9P031_H

struct v4l2_subdev;

enum {
	MT9P031_COLOR_VERSION,
	MT9P031_MONOCHROME_VERSION,
};

enum {
	MT9P031_SKIP_X0 = 0,
	MT9P031_SKIP_X2 = 2,
	MT9P031_SKIP_X4 = 4
};

struct mt9p031_platform_data {
	int (*set_xclk)(struct v4l2_subdev *subdev, u32 hz);
	int (*set_power)(struct v4l2_subdev *subdev, int power);
	u32 ext_freq;    /* input frequency to the mt9p031 for PLL dividers */
	u32 target_freq; /* frequency target for the PLL */
	int version; /* MT9P031_COLOR_VERSION or MT9P031_MONOCHROME_VERSION */
};

#define V4L2_CID_GREEN1_DIGITAL_GAIN	(V4L2_CID_USER_BASE | 0x1002)
#define V4L2_CID_BLUE_DIGITAL_GAIN	(V4L2_CID_USER_BASE | 0x1003)
#define V4L2_CID_RED_DIGITAL_GAIN		(V4L2_CID_USER_BASE | 0x1004)
#define V4L2_CID_GREEN2_DIGITAL_GAIN	(V4L2_CID_USER_BASE | 0x1005)

#endif
