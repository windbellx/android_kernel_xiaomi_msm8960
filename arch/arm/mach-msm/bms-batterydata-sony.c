/* Copyright (c) 2012-2013, Xiaomi corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/mfd/pm8xxx/pm8921-bms.h>

static struct single_row_lut fcc_temp = {
	.x		= {-20, 0, 25, 40, 60},
	.y		= {1927, 1935, 1943, 1942, 1938},
	.cols		= 5
};

static struct single_row_lut fcc_sf = {
	.x		= {0},
	.y		= {100},
	.cols		= 1
};

static struct sf_lut rbatt_sf = { 
        .rows           = 28, 
        .cols           = 5,
        /* row_entries are temperature */
        .row_entries	= {-20, 0, 25, 40, 60},
        .percent        = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
        .sf		= {

{768,211,100,87,83},
{783,216,104,89,84},
{799,215,106,91,85},
{822,216,107,93,87},
{848,218,108,94,88},
{882,219,108,94,89},
{919,220,105,94,90},
{972,221,104,91,88},
{1039,223,104,89,84},
{1132,225,106,89,84},
{1243,228,106,91,85},
{1377,233,107,92,86},
{1540,239,109,93,88},
{1721,248,110,94,89},
{1933,258,111,93,85},
{2203,275,112,92,86},
{2585,311,113,93,87},
{3258,276,114,93,86},
{1426,227,100,87,83},
{1541,230,101,87,84},
{1759,238,103,88,85},
{2086,248,105,90,86},
{2595,262,107,91,86},
{3440,283,108,90,85},
{4991,318,109,91,86},
{8289,436,114,93,87},
{16540,774,124,96,90},
{43321,1887,147,105,98},

	 },
};

static struct pc_temp_ocv_lut pc_temp_ocv = {
	.rows		= 29,
	.cols		= 5,
	.temp		= {-20, 0, 25, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4174, 4172, 4167, 4162, 4155},
				{4075, 4094, 4094, 4093, 4090},
				{3994, 4043, 4051, 4050, 4046},
				{3948, 3995, 4009, 4010, 4008},
				{3910, 3954, 3975, 3976, 3973},
				{3875, 3923, 3944, 3945, 3942},
				{3845, 3892, 3913, 3916, 3914},
				{3820, 3864, 3879, 3887, 3888},
				{3799, 3839, 3850, 3855, 3859},
				{3781, 3816, 3827, 3829, 3828},
				{3766, 3796, 3809, 3809, 3807},
				{3749, 3781, 3793, 3794, 3792},
				{3732, 3770, 3780, 3782, 3780},
				{3715, 3760, 3771, 3772, 3769},
				{3697, 3748, 3762, 3760, 3754},
				{3678, 3734, 3749, 3743, 3730},
				{3656, 3714, 3730, 3722, 3709},
				{3627, 3690, 3703, 3697, 3685},
				{3595, 3665, 3669, 3666, 3657},
				{3585, 3656, 3664, 3663, 3654},
				{3572, 3646, 3659, 3659, 3650},
				{3557, 3635, 3651, 3652, 3643},
				{3538, 3620, 3638, 3639, 3629},
				{3516, 3599, 3613, 3613, 3599},
				{3486, 3564, 3572, 3569, 3557},
				{3446, 3512, 3516, 3514, 3503},
				{3394, 3439, 3444, 3443, 3434},
				{3318, 3340, 3347, 3346, 3341},
				{3200, 3200, 3200, 3200, 3200}
	},
};

static struct sf_lut pc_sf = {
	.rows		= 1,
	.cols		= 1,
	.row_entries	= {0},
	.percent	= {100},
	.sf		= {
				{100}
	},
};

struct pm8921_bms_battery_data sony_1900_data = {
	.fcc			= 1900,
	.fcc_temp_lut		= &fcc_temp,
	.fcc_sf_lut		= &fcc_sf,
	.pc_temp_ocv_lut	= &pc_temp_ocv,
	.pc_sf_lut		= &pc_sf,
	.rbatt_sf_lut		= &rbatt_sf,
	.default_rbatt_mohm	=159,
};
