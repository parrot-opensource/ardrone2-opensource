/*
 * include/media/sensor_dummy.h
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

#ifndef __SENSOR_DUMMY_H_
#define __SENSOR_DUMMY_H_

#define SENSOR_DUMMY_NAME	"sen_dummy"

struct v4l2_subdev;

struct dummy_platform_data {
	int (*set_xclk)(struct v4l2_subdev *subdev, int hz);
};

#endif /* __SENSOR_DUMMY_H_ */
