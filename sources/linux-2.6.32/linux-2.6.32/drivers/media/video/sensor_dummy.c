/*
 * driver/media/video/sensor_dummy.c
 *
 * Copyright (C) 2011 MM Solutions Ltd
 * Contact: Stanimir Varbanov <svarbanov@mm-sol.com>
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

#include <linux/v4l2-mediabus.h>
#include <linux/i2c.h>
#include <media/media-entity.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/sensor-dummy.h>

struct dummy_priv {
	struct v4l2_subdev subdev;
	struct media_pad pad;
	struct v4l2_mbus_framefmt format;
	struct dummy_platform_data *pdata;

	unsigned int streaming;
};

static inline struct dummy_priv *to_dummy(struct v4l2_subdev *sd)
{
	return container_of(sd, struct dummy_priv, subdev);
}

struct supported_devices {
	char *name;
	u8 address;
};

static const struct supported_devices sensors[] = {
	{"ov7675", 0x42},
	{"ov9740", 0x48},
};

static int dummy_set_power(struct v4l2_subdev *subdev, int on)
{
	return 0;
}

static int dummy_set_stream(struct v4l2_subdev *subdev, int enable)
{
	struct dummy_priv *dummy = to_dummy(subdev);
	int rval = 0;

	if (dummy->streaming == enable)
		return 0;

	if (rval == 0)
		dummy->streaming = enable;

	return rval;
}

static int dummy_get_frame_interval(struct v4l2_subdev *subdev,
				     struct v4l2_subdev_frame_interval *fi)
{
	memset(fi, 0, sizeof(*fi));
	fi->interval.numerator = 1;
	fi->interval.denominator = 30;

	return 0;
}

static int dummy_set_frame_interval(struct v4l2_subdev *subdev,
				     struct v4l2_subdev_frame_interval *fi)
{
	return 0;
}

static int dummy_enum_mbus_code(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_mbus_code_enum *code)
{
	return -EINVAL;
}

static int dummy_enum_frame_size(struct v4l2_subdev *subdev,
				  struct v4l2_subdev_fh *fh,
				  struct v4l2_subdev_frame_size_enum *fse)
{
	return -EINVAL;
}

static int dummy_enum_frame_ival(struct v4l2_subdev *subdev,
				  struct v4l2_subdev_fh *fh,
				  struct v4l2_subdev_frame_interval_enum *fie)
{
	return -EINVAL;
}

static struct v4l2_mbus_framefmt *
__dummy_get_pad_format(struct dummy_priv *dummy, struct v4l2_subdev_fh *fh,
			unsigned int pad, enum v4l2_subdev_format_whence which)
{
	if (pad != 0)
		return NULL;

	switch (which) {
	case V4L2_SUBDEV_FORMAT_TRY:
		return v4l2_subdev_get_try_format(fh, pad);
	case V4L2_SUBDEV_FORMAT_ACTIVE:
		return &dummy->format;
	default:
		return NULL;
	}
}

static int dummy_get_pad_format(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_format *fmt)
{
	struct dummy_priv *dummy = to_dummy(subdev);
	struct v4l2_mbus_framefmt *format;

	format = __dummy_get_pad_format(dummy, fh, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	fmt->format = *format;
	return 0;
}

static int dummy_set_pad_format(struct v4l2_subdev *subdev,
				 struct v4l2_subdev_fh *fh,
				 struct v4l2_subdev_format *fmt)
{
	struct dummy_priv *dummy = to_dummy(subdev);
	struct v4l2_mbus_framefmt *format;

	format = __dummy_get_pad_format(dummy, fh, fmt->pad, fmt->which);
	if (format == NULL)
		return -EINVAL;

	*format = fmt->format;

	if (fmt->which == V4L2_SUBDEV_FORMAT_ACTIVE)
		return 0;

	return 0;
}

static int dummy_get_skip_top_lines(struct v4l2_subdev *subdev, u32 *lines)
{
	*lines = 0;
	return 0;
}

static int dummy_get_skip_frames(struct v4l2_subdev *subdev, u32 *frames)
{
	*frames = 0;
	return 0;
}

static int dummy_registered(struct v4l2_subdev *subdev)
{
	struct dummy_priv *dummy = to_dummy(subdev);
	struct i2c_client *client = v4l2_get_subdevdata(subdev);
	char *name = NULL;
	int rval;
	int i;

	rval = dummy_set_power(subdev, 1);
	if (rval) {
		rval = -ENODEV;
		goto out;
	}

	dummy->streaming = false;

	for (i = 0; i < ARRAY_SIZE(sensors); i++)
		if (sensors[i].address == client->addr) {
			name = sensors[i].name;
			break;
		}

	if (name)
		strlcpy(subdev->name, name, sizeof(subdev->name));
	else
		strlcpy(subdev->name, "unknown", sizeof(subdev->name));

	v4l2_info(client,  "chip found @ 0x%02x (%s)\n", client->addr,
		  subdev->name);
out:
	return rval;
}

static int dummy_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	return dummy_set_power(sd, 1);
}

static int dummy_close(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	return dummy_set_power(sd, 0);
}

static const struct v4l2_subdev_video_ops dummy_video_ops = {
	.s_stream = dummy_set_stream,
	.g_frame_interval = dummy_get_frame_interval,
	.s_frame_interval = dummy_set_frame_interval,
};

static const struct v4l2_subdev_core_ops dummy_core_ops = {
	.s_power = dummy_set_power,
};

static const struct v4l2_subdev_pad_ops dummy_pad_ops = {
	.enum_mbus_code = dummy_enum_mbus_code,
	.enum_frame_size = dummy_enum_frame_size,
	.enum_frame_interval = dummy_enum_frame_ival,
	.get_fmt = dummy_get_pad_format,
	.set_fmt = dummy_set_pad_format,
};

static const struct v4l2_subdev_sensor_ops dummy_sensor_ops = {
	.g_skip_top_lines = dummy_get_skip_top_lines,
	.g_skip_frames = dummy_get_skip_frames,
};

static const struct v4l2_subdev_ops dummy_subdev_ops = {
	.core = &dummy_core_ops,
	.video = &dummy_video_ops,
	.pad = &dummy_pad_ops,
	.sensor = &dummy_sensor_ops,
};

static const struct v4l2_subdev_internal_ops dummy_internal_ops = {
	.registered = dummy_registered,
	.open = dummy_open,
	.close = dummy_close,
};

static int dummy_probe(struct i2c_client *client,
		       const struct i2c_device_id *devid)
{
	struct dummy_priv *dummy;
	int rval;

	if (client == NULL || client->dev.platform_data == NULL)
		return -ENODEV;

	dummy = kzalloc(sizeof(*dummy), GFP_KERNEL);
	if (dummy == NULL)
		return -ENOMEM;

	dummy->pdata = client->dev.platform_data;

	v4l2_i2c_subdev_init(&dummy->subdev, client, &dummy_subdev_ops);
	dummy->subdev.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	dummy->subdev.internal_ops = &dummy_internal_ops;

	dummy->pad.flags = MEDIA_PAD_FLAG_OUTPUT;
	rval = media_entity_init(&dummy->subdev.entity, 1, &dummy->pad, 0);
	if (rval < 0)
		kfree(dummy);

	dummy->subdev.entity.type = MEDIA_ENTITY_TYPE_SUBDEV_SENSOR;
	return rval;
}

static int __exit dummy_remove(struct i2c_client *client)
{
	struct v4l2_subdev *subdev = i2c_get_clientdata(client);
	struct dummy_priv *dummy = to_dummy(subdev);

	media_entity_cleanup(&dummy->subdev.entity);
	v4l2_device_unregister_subdev(subdev);
	kfree(dummy);
	return 0;
}

static const struct i2c_device_id dummy_id_table[] = {
	{ SENSOR_DUMMY_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, dummy_id_table);

static struct i2c_driver dummy_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = SENSOR_DUMMY_NAME,
	},
	.probe = dummy_probe,
	.remove = __exit_p(dummy_remove),
	.id_table = dummy_id_table,
};

static int __init dummy_init(void)
{
	return i2c_add_driver(&dummy_i2c_driver);
}

static void __exit dummy_exit(void)
{
	i2c_del_driver(&dummy_i2c_driver);
}

module_init(dummy_init);
module_exit(dummy_exit);

MODULE_AUTHOR("Stanimir Varbanov <svarbanov@mm-sol.com>");
MODULE_DESCRIPTION("Dummy subdev sensor driver");
MODULE_LICENSE("GPL");
