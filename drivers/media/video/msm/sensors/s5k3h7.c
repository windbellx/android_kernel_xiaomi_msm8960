/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_sensor.h"
#define SENSOR_NAME "s5k3h7"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k3h7"

DEFINE_MUTEX(s5k3h7_mut);
static struct msm_sensor_ctrl_t s5k3h7_s_ctrl;
static int otp_slot = 0;

static struct msm_camera_i2c_reg_conf s5k3h7_start_settings[] = {
	{0x0100, 0x0100},
};

static struct msm_camera_i2c_reg_conf s5k3h7_stop_settings[] = {
	{0x0100, 0x0000},
};

static struct msm_camera_i2c_reg_conf s5k3h7_groupon_settings[] = {
	{0x104, 0x0100},
};

static struct msm_camera_i2c_reg_conf s5k3h7_groupoff_settings[] = {
	{0x104, 0x0000},
};

static struct msm_camera_i2c_reg_conf s5k3h7_otp_settings[] = {
       {0x6028, 0xD000},
       // Offset control
       {0x38FA, 0x0030},       // gisp_offs_gains_bls_offs_0_
       {0x38FC, 0x0030},       // gisp_offs_gains_bls_offs_1_
       {0x0086, 0x01FF},       // analogue_gain_code_max
       // Setting for Analog (Don't change)
       {0x6218, 0xF1D0},       // open all clocks
       {0x6214, 0xF9F0},       // open all clocks
       {0x6226, 0x0001},       // open APB clock for I2C transaction
       {0xB0C0, 0x000C},
       {0xB0C0, 0x000C},
       {0xF400, 0x0BBC},       // workaround for the SW standby current
       {0x6226, 0x0000},       // close APB clock for I2C transaction
       {0x6218, 0xF9F0},       // close all clocks
       // set MIPI lane
       {0x0114, 0x0300},    // #smiaRegs_rw_output_lane_mode (03 - 4lane, 01 - 2lane)
       // set MCLK
       {0x0136, 0x1314},       // #smiaRegs_rw_op_cond_extclk_frequency_mhz
       // set PLL
       {0x0300, 0x0002},       // smiaRegs_rw_clocks_vt_pix_clk_div
       {0x0302, 0x0001},       // smiaRegs_rw_clocks_vt_sys_clk_div
       {0x0304, 0x0006},       // smiaRegs_rw_clocks_pre_pll_clk_div
       {0x0306, 0x00A3},       // smiaRegs_rw_clocks_pll_multiplier
       {0x0308, 0x0008},       // smiaRegs_rw_clocks_op_pix_clk_div
       {0x030A, 0x0001},       // smiaRegs_rw_clocks_op_sys_clk_div
       {0x030C, 0x0006},       // smiaRegs_rw_clocks_secnd_pre_pll_clk_div
       {0x030E, 0x00C8},       // smiaRegs_rw_clocks_secnd_pll_multiplier (MIPI: 334.75MHz -> 276.25MHz)
       // Set Output size
       {0x034C, 0x0CC0},       // smiaRegs_rw_frame_timing_x_output_size
       {0x034E, 0x0990},       // smiaRegs_rw_frame_timing_y_output_size
       // Set start-address
       {0x0344, 0x0004},       // smiaRegs_rw_frame_timing_x_addr_start
       {0x0346, 0x0004},       // smiaRegs_rw_frame_timing_y_addr_start
       {0x0348, 0x0CC3},       // smiaRegs_rw_frame_timing_x_addr_end
       {0x034A, 0x0993},       // smiaRegs_rw_frame_timing_y_addr_end
       // Set FPS
       {0x0342, 0x0E68},       // smiaRegs_rw_frame_timing_line_length_pck (7376d)
       {0x0340, 0x0BDD},       // smiaRegs_rw_frame_timing_frame_length_lines
       // Set int.time
       {0x0200, 0x0BEF},       // smiaRegs_rw_integration_time_fine_integration_time
       {0x0202, 0x060A},       // smiaRegs_rw_integration_time_coarse_integration_time
       // Set gain
       {0x0204, 0x0020},       //0020  // X1
       // Streaming on
       {0x0100, 0x0100},       // smiaRegs_rw_general_setup_mode_select
};


static struct msm_camera_i2c_reg_conf s5k3h7_prev_settings[] = {

	/*Timing configuration*/
	{0x6218, 0xF1D0},	// open all clocks
	{0x6214, 0xF9F0},	// open all clocks
	{0xF400, 0x0BBC},	// workaround for the SW standby current
	{0x6226, 0x0001},	// open APB clock for I2C transaction
	{0xB0C0, 0x000C},
	{0x6226, 0x0000},	// close APB clock for I2C transaction
	{0x6218, 0xF9F0},	// close all clocks

	//Offset control
	{0x38FA, 0x0030},  // gisp_offs_gains_bls_offs_0_
	{0x38FC, 0x0030},  // gisp_offs_gains_bls_offs_1_
	{0x0086, 0x01ff},	// smiaRegs_rd_analog_gain_analogue_gain_code_max
	{0xF616, 0x0004},	//aig_tmc_gain

	{0x32BD, 0x00E8},
	{0x32BE, 0x001B},
	{0xF442, 0x0018},
	{0xF446, 0x002A},
	{0xF448, 0x0020},
	{0xF44A, 0x0014},
	{0xF44C, 0x0016},
	{0x32CE, 0x0060},
	{0x32D0, 0x0024},
	{0x3278, 0x0050},
        {0x012A, 0x0060},
        {0x012C, 0x7077},
        {0x012E, 0x7777},

        //Sensor configurations
        //set MCLK
	{0x0136, 0x1314},	// #smiaRegs_rw_op_cond_extclk_frequency_mhz
        //set PLL
	{0x0114, 0x0300},      // #smiaRegs_rw_output_lane_mode (03 - 4lane, 01 - 2lane)
	{0x0300, 0x0002},	// smiaRegs_rw_clocks_vt_pix_clk_div
	{0x0302, 0x0001},	// smiaRegs_rw_clocks_vt_sys_clk_div
	{0x0304, 0x0006},	// smiaRegs_rw_clocks_pre_pll_clk_div
	{0x0306, 0x00A3},	// smiaRegs_rw_clocks_pll_multiplier
	{0x0308, 0x0008},	// smiaRegs_rw_clocks_op_pix_clk_div
	{0x030A, 0x0001},	// smiaRegs_rw_clocks_op_sys_clk_div
	{0x030C, 0x0006},	// smiaRegs_rw_clocks_secnd_pre_pll_clk_div
	{0x030E, 0x00C8},	// smiaRegs_rw_clocks_secnd_pll_multiplier (MIPI: 334.75MHz -> 276.25MHz)
        //Set Output size
	{0x034C, 0x0CC0},	//0CC0	// smiaRegs_rw_frame_timing_x_output_size
	{0x034E, 0x0990},	//0990	// smiaRegs_rw_frame_timing_y_output_size
        //Set start-address
	{0x0344, 0x0004},	// smiaRegs_rw_frame_timing_x_addr_start
	{0x0346, 0x0004},	// smiaRegs_rw_frame_timing_y_addr_start
	{0x0348, 0x0CC3},	// smiaRegs_rw_frame_timing_x_addr_end
	{0x034A, 0x0993},	// smiaRegs_rw_frame_timing_y_addr_end
        //Binning & Subsampling
        {0x0380, 0x0001},        // #smiaRegs_rw_sub_sample_x_even_inc
        {0x0382, 0x0001},        // #smiaRegs_rw_sub_sample_x_odd_inc
        {0x0384, 0x0001},        // #smiaRegs_rw_sub_sample_y_even_inc
        {0x0386, 0x0001},        // #smiaRegs_rw_sub_sample_y_odd_inc
        {0x0900, 0x0000},        // #smiaRegs_rw_binning_mode
        {0x0902, 0x0100},        // #smiaRegs_rw_binning_weighting
	//Set FPS
	{0x0342, 0x0EDC},	// smiaRegs_rw_frame_timing_line_length_pck (7376d)
	{0x0340, 0x0AB8},	// smiaRegs_rw_frame_timing_frame_length_lines
	//Set int.time
	{0x0200, 0x0BEF},	// smiaRegs_rw_integration_time_fine_integration_time
	{0x0202, 0x060A},	// smiaRegs_rw_integration_time_coarse_integration_time
	{0x0204, 0x0020},
//M2M
        {0x31FE, 0xC004},         //ash_uDecompressXgrid[0]
        {0x3200, 0xC4F0},         //ash_uDecompressXgrid[1]
        {0x3202, 0xCEC8},         //ash_uDecompressXgrid[2]
        {0x3204, 0xD8A0},         //ash_uDecompressXgrid[3]
        {0x3206, 0xE278},         //ash_uDecompressXgrid[4]
        {0x3208, 0xEC50},         //ash_uDecompressXgrid[5]
        {0x320A, 0xF628},         //ash_uDecompressXgrid[6]
        {0x320C, 0x0000},         //ash_uDecompressXgrid[7]
        {0x320E, 0x09D8},         //ash_uDecompressXgrid[8]
        {0x3210, 0x13B0},         //ash_uDecompressXgrid[9]
        {0x3212, 0x1D88},         //ash_uDecompressXgrid[10]
        {0x3214, 0x2760},         //ash_uDecompressXgrid[11]
        {0x3216, 0x3138},         //ash_uDecompressXgrid[12]
        {0x3218, 0x3B10},         //ash_uDecompressXgrid[13]
        {0x321A, 0x3FFC},         //ash_uDecompressXgrid[14]

        {0x321C, 0xC004},         //ash_uDecompressYgrid[0]
        {0x321E, 0xCCD0},         //ash_uDecompressYgrid[1]
        {0x3220, 0xD99C},         //ash_uDecompressYgrid[2]
        {0x3222, 0xE668},         //ash_uDecompressYgrid[3]
        {0x3224, 0xF334},         //ash_uDecompressYgrid[4]
        {0x3226, 0x0000},         //ash_uDecompressYgrid[5]
        {0x3228, 0x0CCC},         //ash_uDecompressYgrid[6]
        {0x322A, 0x1998},         //ash_uDecompressYgrid[7]
        {0x322C, 0x2664},         //ash_uDecompressYgrid[8]
        {0x322E, 0x3330},         //ash_uDecompressYgrid[9]
        {0x3230, 0x3FFC},         //ash_uDecompressYgrid[10]

        {0x3232, 0x0100},           //ash_uDecompressWidth
        {0x3234, 0x0100},           //ash_uDecompressHeight

        {0x3238, 0x0909},           //ash_uDecompressRadiusShifter
        {0x323A, 0x0B0F},           //ash_uDecompressFinalScale
        {0x3236, 0x0E00},           //ash_uDecompressScale
        {0x0b00, 0x0120},           //smiaRegs_rw_isp_luminance_correction_level

//BASE Profile parabola start
        {0x3160, 0x0600},            //ash_GrasCfg
        {0x3161, 0x0000},            //ash_GrasShifter
        {0x3164, 0x09C4},         //ash_luma_params[0]_tmpr
        {0x3166, 0x0100},         //ash_luma_params[0]_alpha[0]
        {0x3168, 0x0100},         //ash_luma_params[0]_alpha[1]
        {0x316A, 0x0100},         //ash_luma_params[0]_alpha[2]
        {0x316C, 0x0100},         //ash_luma_params[0]_alpha[3]
        {0x316E, 0x0011},         //ash_luma_params[0]_beta[0]
        {0x3170, 0x002C},         //ash_luma_params[0]_beta[1]
        {0x3172, 0x0000},         //ash_luma_params[0]_beta[2]
        {0x3174, 0x0012},         //ash_luma_params[0]_beta[3]
        {0x3176, 0x0A8C},         //ash_luma_params[1]_tmpr
        {0x3178, 0x0100},         //ash_luma_params[1]_alpha[0]
        {0x317A, 0x0100},         //ash_luma_params[1]_alpha[1]
        {0x317C, 0x0100},         //ash_luma_params[1]_alpha[2]
        {0x317E, 0x0100},         //ash_luma_params[1]_alpha[3]
        {0x3180, 0x0011},         //ash_luma_params[1]_beta[0]
        {0x3182, 0x002C},         //ash_luma_params[1]_beta[1]
        {0x3184, 0x0000},         //ash_luma_params[1]_beta[2]
        {0x3186, 0x0012},         //ash_luma_params[1]_beta[3]
        {0x3188, 0x0CE4},         //ash_luma_params[2]_tmpr
        {0x318A, 0x0040},         //ash_luma_params[2]_alpha[0]    GR
        {0x318C, 0x0048},         //ash_luma_params[2]_alpha[1]    R
        {0x318E, 0x0040},         //ash_luma_params[2]_alpha[2]    B
        {0x3190, 0x0040},         //ash_luma_params[2]_alpha[3]    GB
        {0x3192, 0x0011},         //ash_luma_params[2]_beta[0]
        {0x3194, 0x002C},         //ash_luma_params[2]_beta[1]
        {0x3196, 0x0000},         //ash_luma_params[2]_beta[2]
        {0x3198, 0x0012},         //ash_luma_params[2]_beta[3]
        {0x319A, 0x1004},         //ash_luma_params[3]_tmpr
        {0x319C, 0x0100},         //ash_luma_params[3]_alpha[0]
        {0x319E, 0x0100},         //ash_luma_params[3]_alpha[1]
        {0x31A0, 0x0100},         //ash_luma_params[3]_alpha[2]
        {0x31A2, 0x0100},         //ash_luma_params[3]_alpha[3]
        {0x31A4, 0x0011},         //ash_luma_params[3]_beta[0]
        {0x31A6, 0x002C},         //ash_luma_params[3]_beta[1]
        {0x31A8, 0x0000},         //ash_luma_params[3]_beta[2]
        {0x31AA, 0x0012},         //ash_luma_params[3]_beta[3]
        {0x31AC, 0x1388},         //ash_luma_params[4]_tmpr
        {0x31AE, 0x0100},         //ash_luma_params[4]_alpha[0]
        {0x31B0, 0x0100},         //ash_luma_params[4]_alpha[1]
        {0x31B2, 0x0100},         //ash_luma_params[4]_alpha[2]
        {0x31B4, 0x0100},         //ash_luma_params[4]_alpha[3]
        {0x31B6, 0x0011},         //ash_luma_params[4]_beta[0]
        {0x31B8, 0x002C},         //ash_luma_params[4]_beta[1]
        {0x31BA, 0x0000},         //ash_luma_params[4]_beta[2]
        {0x31BC, 0x0012},         //ash_luma_params[4]_beta[3]
        {0x31BE, 0x1964},         //ash_luma_params[5]_tmpr
        {0x31C0, 0x0100},         //ash_luma_params[5]_alpha[0]
        {0x31C2, 0x0100},         //ash_luma_params[5]_alpha[1]
        {0x31C4, 0x0100},         //ash_luma_params[5]_alpha[2]
        {0x31C6, 0x0100},         //ash_luma_params[5]_alpha[3]
        {0x31C8, 0x0011},         //ash_luma_params[5]_beta[0]
        {0x31CA, 0x002C},         //ash_luma_params[5]_beta[1]
        {0x31CC, 0x0000},         //ash_luma_params[5]_beta[2]
        {0x31CE, 0x0012},         //ash_luma_params[5]_beta[3]
        {0x31D0, 0x1D4C},         //ash_luma_params[6]_tmpr
        {0x31D2, 0x0100},         //ash_luma_params[6]_alpha[0]
        {0x31D4, 0x0100},         //ash_luma_params[6]_alpha[1]
        {0x31D6, 0x0100},         //ash_luma_params[6]_alpha[2]
        {0x31D8, 0x0100},         //ash_luma_params[6]_alpha[3]
        {0x31DA, 0x0011},         //ash_luma_params[6]_beta[0]
        {0x31DC, 0x002C},         //ash_luma_params[6]_beta[1]
        {0x31DE, 0x0000},         //ash_luma_params[6]_beta[2]
        {0x31E0, 0x0012},         //ash_luma_params[6]_beta[3]
        {0x3162, 0x0100},           //ash_bLumaMode
        {0x301c, 0x0200},            //smiaRegs_vendor_gras_nvm_address//index 228: page#3, byte #36
        {0x301e, 0x0300},            //WsmiaRegs_vendor_gras_load_from
        {0x323C, 0x0101},            //Wash_bSkipNvmGrasOfs  // skipping the value set in nvm page 0 address 47
        {0x1989, 0x0004},            //smiaRegs_ro_edof_cap_uAlphaTempInd
};

static struct msm_camera_i2c_reg_conf s5k3h7_video_60fps_settings[] = {
        {0x6218, 0xF1D0},       // open all clocks
        {0x6214, 0xF9F0},       // open all clocks
        {0xF400, 0x0BBC},// workaround for the SW standby current
        {0x6226, 0x0001},       // open APB clock for I2C transaction
        {0xB0C0, 0x000C},
        {0x6226, 0x0000},       // close APB clock for I2C transaction
        {0x6218, 0xF9F0},       // close all clocks

        //Offset control
        {0x38FA, 0x0030},  // gisp_offs_gains_bls_offs_0_
        {0x38FC, 0x0030},  // gisp_offs_gains_bls_offs_1_
        {0x0086, 0x01FF},      // analogue_gain_code_max
        {0xF616, 0x0004},       //aig_tmc_gain 

	{0x32BD, 0x00E8},
	{0x32BE, 0x001B},
	{0xF442, 0x0018},
	{0xF446, 0x002A},
	{0xF448, 0x0020},
	{0xF44A, 0x0014},
	{0xF44C, 0x0016},
	{0x32CE, 0x0060},
	{0x32D0, 0x0024},
	{0x3278, 0x0050},
        {0x012A, 0x0060},
        {0x012C, 0x7077},
        {0x012E, 0x7777},

        //Sensor configurations
        //set MCLK
        {0x0136, 0x1314},//0D00  // #smiaRegs_rw_op_cond_extclk_frequency_mhz
        //set PLL
        {0x0114, 0x0300},      // #smiaRegs_rw_output_lane_mode (03 - 4lane, 01 - 2lane)
        {0x0300, 0x0002},        // smiaRegs_rw_clocks_vt_pix_clk_div
        {0x0302, 0x0001},        // smiaRegs_rw_clocks_vt_sys_clk_div
        {0x0304, 0x0006},//0004  // smiaRegs_rw_clocks_pre_pll_clk_div
        {0x0306, 0x00A3},//00AC  // smiaRegs_rw_clocks_pll_multiplier
        {0x0308, 0x0008},        // smiaRegs_rw_clocks_op_pix_clk_div
        {0x030A, 0x0001},        // smiaRegs_rw_clocks_op_sys_clk_div
        {0x030C, 0x0006},//0004  // smiaRegs_rw_clocks_secnd_pre_pll_clk_div
        {0x030E, 0x00C8},//00CE  // smiaRegs_rw_clocks_secnd_pll_multiplier
        //Set Output size
        {0x034C, 0x0520},        // smiaRegs_rw_frame_timing_x_output_size
        {0x034E, 0x02E0},        // smiaRegs_rw_frame_timing_y_output_size
        //Set start-address
        {0x0344, 0x0144},        // smiaRegs_rw_frame_timing_x_addr_start
        {0x0346, 0x01EC},        // smiaRegs_rw_frame_timing_y_addr_start
        {0x0348, 0x0B83},        // smiaRegs_rw_frame_timing_x_addr_end
        {0x034A, 0x07AB},        // smiaRegs_rw_frame_timing_y_addr_end
        //Binning & Subsampling
        {0x0380, 0x0001},        // #smiaRegs_rw_sub_sample_x_even_inc
        {0x0382, 0x0003},        // #smiaRegs_rw_sub_sample_x_odd_inc
        {0x0384, 0x0001},        // #smiaRegs_rw_sub_sample_y_even_inc
        {0x0386, 0x0003},        // #smiaRegs_rw_sub_sample_y_odd_inc
        {0x0900, 0x0122},        // #smiaRegs_rw_binning_mode
        {0x0902, 0x0100},        // #smiaRegs_rw_binning_weighting
        //Set FPS
        {0x0342, 0x0EDC},        // smiaRegs_rw_frame_timing_line_length_pck
        {0x0340, 0x0477},        // smiaRegs_rw_frame_timing_frame_length_lines
        //Set int.time
        {0x0200, 0x0618},        // smiaRegs_rw_integration_time_fine_integration_time
        {0x0202, 0x04BC},        // smiaRegs_rw_integration_time_coarse_integration_time
        //Set gain
        {0x0204, 0x0020},        // X1
//M2M
        {0x31FE, 0xC004},         //ash_uDecompressXgrid[0]
        {0x3200, 0xC4F0},         //ash_uDecompressXgrid[1]
        {0x3202, 0xCEC8},         //ash_uDecompressXgrid[2]
        {0x3204, 0xD8A0},         //ash_uDecompressXgrid[3]
        {0x3206, 0xE278},         //ash_uDecompressXgrid[4]
        {0x3208, 0xEC50},         //ash_uDecompressXgrid[5]
        {0x320A, 0xF628},         //ash_uDecompressXgrid[6]
        {0x320C, 0x0000},         //ash_uDecompressXgrid[7]
        {0x320E, 0x09D8},         //ash_uDecompressXgrid[8]
        {0x3210, 0x13B0},         //ash_uDecompressXgrid[9]
        {0x3212, 0x1D88},         //ash_uDecompressXgrid[10]
        {0x3214, 0x2760},         //ash_uDecompressXgrid[11]
        {0x3216, 0x3138},         //ash_uDecompressXgrid[12]
        {0x3218, 0x3B10},         //ash_uDecompressXgrid[13]
        {0x321A, 0x3FFC},         //ash_uDecompressXgrid[14]

        {0x321C, 0xC004},         //ash_uDecompressYgrid[0]
        {0x321E, 0xCCD0},         //ash_uDecompressYgrid[1]
        {0x3220, 0xD99C},         //ash_uDecompressYgrid[2]
        {0x3222, 0xE668},         //ash_uDecompressYgrid[3]
        {0x3224, 0xF334},         //ash_uDecompressYgrid[4]
        {0x3226, 0x0000},         //ash_uDecompressYgrid[5]
        {0x3228, 0x0CCC},         //ash_uDecompressYgrid[6]
        {0x322A, 0x1998},         //ash_uDecompressYgrid[7]
        {0x322C, 0x2664},         //ash_uDecompressYgrid[8]
        {0x322E, 0x3330},         //ash_uDecompressYgrid[9]
        {0x3230, 0x3FFC},         //ash_uDecompressYgrid[10]

        {0x3232, 0x0100},           //ash_uDecompressWidth
        {0x3234, 0x0100},           //ash_uDecompressHeight

        {0x3238, 0x0909},           //ash_uDecompressRadiusShifter
        {0x323A, 0x0B0F},           //ash_uDecompressFinalScale
        {0x3236, 0x0E00},           //ash_uDecompressScale
        {0x0b00, 0x0120},           //smiaRegs_rw_isp_luminance_correction_level

//BASE Profile parabola start
        {0x3160, 0x0600},            //ash_GrasCfg
        {0x3161, 0x0000},            //ash_GrasShifter
        {0x3164, 0x09C4},         //ash_luma_params[0]_tmpr
        {0x3166, 0x0100},         //ash_luma_params[0]_alpha[0]
        {0x3168, 0x0100},         //ash_luma_params[0]_alpha[1]
        {0x316A, 0x0100},         //ash_luma_params[0]_alpha[2]
        {0x316C, 0x0100},         //ash_luma_params[0]_alpha[3]
        {0x316E, 0x0011},         //ash_luma_params[0]_beta[0]
        {0x3170, 0x002C},         //ash_luma_params[0]_beta[1]
        {0x3172, 0x0000},         //ash_luma_params[0]_beta[2]
        {0x3174, 0x0012},         //ash_luma_params[0]_beta[3]
        {0x3176, 0x0A8C},         //ash_luma_params[1]_tmpr
        {0x3178, 0x0100},         //ash_luma_params[1]_alpha[0]
        {0x317A, 0x0100},         //ash_luma_params[1]_alpha[1]
        {0x317C, 0x0100},         //ash_luma_params[1]_alpha[2]
        {0x317E, 0x0100},         //ash_luma_params[1]_alpha[3]
        {0x3180, 0x0011},         //ash_luma_params[1]_beta[0]
        {0x3182, 0x002C},         //ash_luma_params[1]_beta[1]
        {0x3184, 0x0000},         //ash_luma_params[1]_beta[2]
        {0x3186, 0x0012},         //ash_luma_params[1]_beta[3]
        {0x3188, 0x0CE4},         //ash_luma_params[2]_tmpr
        {0x318A, 0x0040},         //ash_luma_params[2]_alpha[0]    GR
        {0x318C, 0x0048},         //ash_luma_params[2]_alpha[1]    R
        {0x318E, 0x0040},         //ash_luma_params[2]_alpha[2]    B
        {0x3190, 0x0040},         //ash_luma_params[2]_alpha[3]    GB
        {0x3192, 0x0011},         //ash_luma_params[2]_beta[0]
        {0x3194, 0x002C},         //ash_luma_params[2]_beta[1]
        {0x3196, 0x0000},         //ash_luma_params[2]_beta[2]
        {0x3198, 0x0012},         //ash_luma_params[2]_beta[3]
        {0x319A, 0x1004},         //ash_luma_params[3]_tmpr
        {0x319C, 0x0100},         //ash_luma_params[3]_alpha[0]
        {0x319E, 0x0100},         //ash_luma_params[3]_alpha[1]
        {0x31A0, 0x0100},         //ash_luma_params[3]_alpha[2]
        {0x31A2, 0x0100},         //ash_luma_params[3]_alpha[3]
        {0x31A4, 0x0011},         //ash_luma_params[3]_beta[0]
        {0x31A6, 0x002C},         //ash_luma_params[3]_beta[1]
        {0x31A8, 0x0000},         //ash_luma_params[3]_beta[2]
        {0x31AA, 0x0012},         //ash_luma_params[3]_beta[3]
        {0x31AC, 0x1388},         //ash_luma_params[4]_tmpr
        {0x31AE, 0x0100},         //ash_luma_params[4]_alpha[0]
        {0x31B0, 0x0100},         //ash_luma_params[4]_alpha[1]
        {0x31B2, 0x0100},         //ash_luma_params[4]_alpha[2]
        {0x31B4, 0x0100},         //ash_luma_params[4]_alpha[3]
        {0x31B6, 0x0011},         //ash_luma_params[4]_beta[0]
        {0x31B8, 0x002C},         //ash_luma_params[4]_beta[1]
        {0x31BA, 0x0000},         //ash_luma_params[4]_beta[2]
        {0x31BC, 0x0012},         //ash_luma_params[4]_beta[3]
        {0x31BE, 0x1964},         //ash_luma_params[5]_tmpr
        {0x31C0, 0x0100},         //ash_luma_params[5]_alpha[0]
        {0x31C2, 0x0100},         //ash_luma_params[5]_alpha[1]
        {0x31C4, 0x0100},         //ash_luma_params[5]_alpha[2]
        {0x31C6, 0x0100},         //ash_luma_params[5]_alpha[3]
        {0x31C8, 0x0011},         //ash_luma_params[5]_beta[0]
        {0x31CA, 0x002C},         //ash_luma_params[5]_beta[1]
        {0x31CC, 0x0000},         //ash_luma_params[5]_beta[2]
        {0x31CE, 0x0012},         //ash_luma_params[5]_beta[3]
        {0x31D0, 0x1D4C},         //ash_luma_params[6]_tmpr
        {0x31D2, 0x0100},         //ash_luma_params[6]_alpha[0]
        {0x31D4, 0x0100},         //ash_luma_params[6]_alpha[1]
        {0x31D6, 0x0100},         //ash_luma_params[6]_alpha[2]
        {0x31D8, 0x0100},         //ash_luma_params[6]_alpha[3]
        {0x31DA, 0x0011},         //ash_luma_params[6]_beta[0]
        {0x31DC, 0x002C},         //ash_luma_params[6]_beta[1]
        {0x31DE, 0x0000},         //ash_luma_params[6]_beta[2]
        {0x31E0, 0x0012},         //ash_luma_params[6]_beta[3]
        {0x3162, 0x0100},           //ash_bLumaMode
        {0x301c, 0x0200},            //smiaRegs_vendor_gras_nvm_address//index 228: page#3, byte #36
        {0x301e, 0x0300},            //WsmiaRegs_vendor_gras_load_from
        {0x323C, 0x0101},            //Wash_bSkipNvmGrasOfs  // skipping the value set in nvm page 0 address 47
        {0x1989, 0x0004},            //smiaRegs_ro_edof_cap_uAlphaTempInd


};

static struct msm_camera_i2c_reg_conf s5k3h7_video_90fps_settings[] = {
        {0x6218, 0xF1D0},       // open all clocks
        {0x6214, 0xF9F0},       // open all clocks
        {0xF400, 0x0BBC},// workaround for the SW standby current
        {0x6226, 0x0001},       // open APB clock for I2C transaction
        {0xB0C0, 0x000C},
        {0x6226, 0x0000},       // close APB clock for I2C transaction
        {0x6218, 0xF9F0},       // close all clocks

        //Offset control
        {0x38FA, 0x0030},  // gisp_offs_gains_bls_offs_0_
        {0x38FC, 0x0030},  // gisp_offs_gains_bls_offs_1_
        {0x0086, 0x01FF},      // analogue_gain_code_max
	{0xF616, 0x0004},

        {0x32BD, 0x00E8},
        {0x32BE, 0x001B},
        {0xF442, 0x0018},
        {0xF446, 0x002A},
        {0xF448, 0x0020},
        {0xF44A, 0x0014},
        {0xF44C, 0x0016},
        {0x32CE, 0x0060},
        {0x32D0, 0x0024},
        {0x3278, 0x0050},
        {0x012A, 0x0060},
        {0x012C, 0x7077},
        {0x012E, 0x7777},

        //Sensor configurations
        //set MCLK
        {0x0136, 0x1314},//0D00  // #smiaRegs_rw_op_cond_extclk_frequency_mhz
        //set PLL
        {0x0300, 0x0002},        // smiaRegs_rw_clocks_vt_pix_clk_div
        {0x0302, 0x0001},        // smiaRegs_rw_clocks_vt_sys_clk_div
        {0x0304, 0x0006},//0004  // smiaRegs_rw_clocks_pre_pll_clk_div
        {0x0306, 0x00A3},//00AC  // smiaRegs_rw_clocks_pll_multiplier
        {0x0308, 0x0008},        // smiaRegs_rw_clocks_op_pix_clk_div
        {0x030A, 0x0001},        // smiaRegs_rw_clocks_op_sys_clk_div
        {0x030C, 0x0006},//0004  // smiaRegs_rw_clocks_secnd_pre_pll_clk_div
        {0x030E, 0x00C8},//00CE  // smiaRegs_rw_clocks_secnd_pll_multiplier
        //Set Output size
        {0x034C, 0x0520},        // smiaRegs_rw_frame_timing_x_output_size
        {0x034E, 0x02E0},        // smiaRegs_rw_frame_timing_y_output_size
        //Set start-address
        {0x0344, 0x0144},        // smiaRegs_rw_frame_timing_x_addr_start
        {0x0346, 0x01EC},        // smiaRegs_rw_frame_timing_y_addr_start
        {0x0348, 0x0B83},        // smiaRegs_rw_frame_timing_x_addr_end
        {0x034A, 0x07AB},        // smiaRegs_rw_frame_timing_y_addr_end
        //Binning & Subsampling
        {0x0380, 0x0001},        // #smiaRegs_rw_sub_sample_x_even_inc
        {0x0382, 0x0003},        // #smiaRegs_rw_sub_sample_x_odd_inc
        {0x0384, 0x0001},        // #smiaRegs_rw_sub_sample_y_even_inc
        {0x0386, 0x0003},        // #smiaRegs_rw_sub_sample_y_odd_inc
        {0x0900, 0x0122},        // #smiaRegs_rw_binning_mode
        {0x0902, 0x0100},        // #smiaRegs_rw_binning_weighting
        //Set FPS
        {0x0342, 0x0EDC},        // smiaRegs_rw_frame_timing_line_length_pck
        {0x0340, 0x02FA},        // smiaRegs_rw_frame_timing_frame_length_lines
        //Set int.time
        {0x0200, 0x0618},        // smiaRegs_rw_integration_time_fine_integration_time
        {0x0202, 0x04BC},        // smiaRegs_rw_integration_time_coarse_integration_time
        //Set gain
        {0x0204, 0x0020},        // X1

//M2M
        {0x31FE, 0xC004},         //ash_uDecompressXgrid[0]
        {0x3200, 0xC4F0},         //ash_uDecompressXgrid[1]
        {0x3202, 0xCEC8},         //ash_uDecompressXgrid[2]
        {0x3204, 0xD8A0},         //ash_uDecompressXgrid[3]
        {0x3206, 0xE278},         //ash_uDecompressXgrid[4]
        {0x3208, 0xEC50},         //ash_uDecompressXgrid[5]
        {0x320A, 0xF628},         //ash_uDecompressXgrid[6]
        {0x320C, 0x0000},         //ash_uDecompressXgrid[7]
        {0x320E, 0x09D8},         //ash_uDecompressXgrid[8]
        {0x3210, 0x13B0},         //ash_uDecompressXgrid[9]
        {0x3212, 0x1D88},         //ash_uDecompressXgrid[10]
        {0x3214, 0x2760},         //ash_uDecompressXgrid[11]
        {0x3216, 0x3138},         //ash_uDecompressXgrid[12]
        {0x3218, 0x3B10},         //ash_uDecompressXgrid[13]
        {0x321A, 0x3FFC},         //ash_uDecompressXgrid[14]

        {0x321C, 0xC004},         //ash_uDecompressYgrid[0]
        {0x321E, 0xCCD0},         //ash_uDecompressYgrid[1]
        {0x3220, 0xD99C},         //ash_uDecompressYgrid[2]
        {0x3222, 0xE668},         //ash_uDecompressYgrid[3]
        {0x3224, 0xF334},         //ash_uDecompressYgrid[4]
        {0x3226, 0x0000},         //ash_uDecompressYgrid[5]
        {0x3228, 0x0CCC},         //ash_uDecompressYgrid[6]
        {0x322A, 0x1998},         //ash_uDecompressYgrid[7]
        {0x322C, 0x2664},         //ash_uDecompressYgrid[8]
        {0x322E, 0x3330},         //ash_uDecompressYgrid[9]
        {0x3230, 0x3FFC},         //ash_uDecompressYgrid[10]

        {0x3232, 0x0100},           //ash_uDecompressWidth
        {0x3234, 0x0100},           //ash_uDecompressHeight

        {0x3238, 0x0909},           //ash_uDecompressRadiusShifter
        {0x323A, 0x0B0F},           //ash_uDecompressFinalScale
        {0x3236, 0x0E00},           //ash_uDecompressScale
        {0x0b00, 0x0120},           //smiaRegs_rw_isp_luminance_correction_level

//BASE Profile parabola start
        {0x3160, 0x0600},            //ash_GrasCfg
        {0x3161, 0x0000},            //ash_GrasShifter
        {0x3164, 0x09C4},         //ash_luma_params[0]_tmpr
        {0x3166, 0x0100},         //ash_luma_params[0]_alpha[0]
        {0x3168, 0x0100},         //ash_luma_params[0]_alpha[1]
        {0x316A, 0x0100},         //ash_luma_params[0]_alpha[2]
        {0x316C, 0x0100},         //ash_luma_params[0]_alpha[3]
        {0x316E, 0x0011},         //ash_luma_params[0]_beta[0]
        {0x3170, 0x002C},         //ash_luma_params[0]_beta[1]
        {0x3172, 0x0000},         //ash_luma_params[0]_beta[2]
        {0x3174, 0x0012},         //ash_luma_params[0]_beta[3]
        {0x3176, 0x0A8C},         //ash_luma_params[1]_tmpr
        {0x3178, 0x0100},         //ash_luma_params[1]_alpha[0]
        {0x317A, 0x0100},         //ash_luma_params[1]_alpha[1]
        {0x317C, 0x0100},         //ash_luma_params[1]_alpha[2]
        {0x317E, 0x0100},         //ash_luma_params[1]_alpha[3]
        {0x3180, 0x0011},         //ash_luma_params[1]_beta[0]
        {0x3182, 0x002C},         //ash_luma_params[1]_beta[1]
        {0x3184, 0x0000},         //ash_luma_params[1]_beta[2]
        {0x3186, 0x0012},         //ash_luma_params[1]_beta[3]
        {0x3188, 0x0CE4},         //ash_luma_params[2]_tmpr
        {0x318A, 0x0040},         //ash_luma_params[2]_alpha[0]    GR
        {0x318C, 0x0048},         //ash_luma_params[2]_alpha[1]    R
        {0x318E, 0x0040},         //ash_luma_params[2]_alpha[2]    B
        {0x3190, 0x0040},         //ash_luma_params[2]_alpha[3]    GB
        {0x3192, 0x0011},         //ash_luma_params[2]_beta[0]
        {0x3194, 0x002C},         //ash_luma_params[2]_beta[1]
        {0x3196, 0x0000},         //ash_luma_params[2]_beta[2]
        {0x3198, 0x0012},         //ash_luma_params[2]_beta[3]
        {0x319A, 0x1004},         //ash_luma_params[3]_tmpr
        {0x319C, 0x0100},         //ash_luma_params[3]_alpha[0]
        {0x319E, 0x0100},         //ash_luma_params[3]_alpha[1]
        {0x31A0, 0x0100},         //ash_luma_params[3]_alpha[2]
        {0x31A2, 0x0100},         //ash_luma_params[3]_alpha[3]
        {0x31A4, 0x0011},         //ash_luma_params[3]_beta[0]
        {0x31A6, 0x002C},         //ash_luma_params[3]_beta[1]
        {0x31A8, 0x0000},         //ash_luma_params[3]_beta[2]
        {0x31AA, 0x0012},         //ash_luma_params[3]_beta[3]
        {0x31AC, 0x1388},         //ash_luma_params[4]_tmpr
        {0x31AE, 0x0100},         //ash_luma_params[4]_alpha[0]
        {0x31B0, 0x0100},         //ash_luma_params[4]_alpha[1]
        {0x31B2, 0x0100},         //ash_luma_params[4]_alpha[2]
        {0x31B4, 0x0100},         //ash_luma_params[4]_alpha[3]
        {0x31B6, 0x0011},         //ash_luma_params[4]_beta[0]
        {0x31B8, 0x002C},         //ash_luma_params[4]_beta[1]
        {0x31BA, 0x0000},         //ash_luma_params[4]_beta[2]
        {0x31BC, 0x0012},         //ash_luma_params[4]_beta[3]
        {0x31BE, 0x1964},         //ash_luma_params[5]_tmpr
        {0x31C0, 0x0100},         //ash_luma_params[5]_alpha[0]
        {0x31C2, 0x0100},         //ash_luma_params[5]_alpha[1]
        {0x31C4, 0x0100},         //ash_luma_params[5]_alpha[2]
        {0x31C6, 0x0100},         //ash_luma_params[5]_alpha[3]
        {0x31C8, 0x0011},         //ash_luma_params[5]_beta[0]
        {0x31CA, 0x002C},         //ash_luma_params[5]_beta[1]
        {0x31CC, 0x0000},         //ash_luma_params[5]_beta[2]
        {0x31CE, 0x0012},         //ash_luma_params[5]_beta[3]
        {0x31D0, 0x1D4C},         //ash_luma_params[6]_tmpr
        {0x31D2, 0x0100},         //ash_luma_params[6]_alpha[0]
        {0x31D4, 0x0100},         //ash_luma_params[6]_alpha[1]
        {0x31D6, 0x0100},         //ash_luma_params[6]_alpha[2]
        {0x31D8, 0x0100},         //ash_luma_params[6]_alpha[3]
        {0x31DA, 0x0011},         //ash_luma_params[6]_beta[0]
        {0x31DC, 0x002C},         //ash_luma_params[6]_beta[1]
        {0x31DE, 0x0000},         //ash_luma_params[6]_beta[2]
        {0x31E0, 0x0012},         //ash_luma_params[6]_beta[3]
        {0x3162, 0x0100},           //ash_bLumaMode
        {0x301c, 0x0200},            //smiaRegs_vendor_gras_nvm_address//index 228: page#3, byte #36
        {0x301e, 0x0300},            //WsmiaRegs_vendor_gras_load_from
        {0x323C, 0x0101},            //Wash_bSkipNvmGrasOfs  // skipping the value set in nvm page 0 address 47
        {0x1989, 0x0004},            //smiaRegs_ro_edof_cap_uAlphaTempInd

};

static struct msm_camera_i2c_reg_conf s5k3h7_video_120fps_settings[] = {
        {0x6218, 0xF1D0},       // open all clocks
        {0x6214, 0xF9F0},       // open all clocks
        {0xF400, 0x0BBC},// workaround for the SW standby current
        {0x6226, 0x0001},       // open APB clock for I2C transaction
        {0xB0C0, 0x000C},
        {0x6226, 0x0000},       // close APB clock for I2C transaction
        {0x6218, 0xF9F0},       // close all clocks

        //Offset control
        {0x38FA, 0x0030},  // gisp_offs_gains_bls_offs_0_
        {0x38FC, 0x0030},  // gisp_offs_gains_bls_offs_1_
        {0x0086, 0x01FF},      // analogue_gain_code_max
	{0xF616, 0x0004},

        {0x32BD, 0x00E8},
        {0x32BE, 0x001B},
        {0xF442, 0x0018},
        {0xF446, 0x002A},
        {0xF448, 0x0020},
        {0xF44A, 0x0014},
        {0xF44C, 0x0016},
        {0x32CE, 0x0060},
        {0x32D0, 0x0024},
        {0x3278, 0x0050},
        {0x012A, 0x0060},
        {0x012C, 0x7077},
        {0x012E, 0x7777},

        //Sensor configurations
        //set MCLK
        {0x0136, 0x1314},//0D00  // #smiaRegs_rw_op_cond_extclk_frequency_mhz
        //set PLL
        {0x0300, 0x0002},        // smiaRegs_rw_clocks_vt_pix_clk_div
        {0x0302, 0x0001},        // smiaRegs_rw_clocks_vt_sys_clk_div
        {0x0304, 0x0006},//0004  // smiaRegs_rw_clocks_pre_pll_clk_div
        {0x0306, 0x00A3},//00AC  // smiaRegs_rw_clocks_pll_multiplier
        {0x0308, 0x0008},        // smiaRegs_rw_clocks_op_pix_clk_div
        {0x030A, 0x0001},        // smiaRegs_rw_clocks_op_sys_clk_div
        {0x030C, 0x0006},//0004  // smiaRegs_rw_clocks_secnd_pre_pll_clk_div
        {0x030E, 0x00C8},//00CE  // smiaRegs_rw_clocks_secnd_pll_multiplier
        //Set Output size
        {0x034C, 0x0320},        // smiaRegs_rw_frame_timing_x_output_size
        {0x034E, 0x0258},        // smiaRegs_rw_frame_timing_y_output_size
        //Set start-address
        {0x0344, 0x0000},        // smiaRegs_rw_frame_timing_x_addr_start
        {0x0346, 0x0000},        // smiaRegs_rw_frame_timing_y_addr_start
        {0x0348, 0x0CCF},        // smiaRegs_rw_frame_timing_x_addr_end
        {0x034A, 0x099F},        // smiaRegs_rw_frame_timing_y_addr_end
        //Binning & Subsampling
        {0x0380, 0x0001},        // #smiaRegs_rw_sub_sample_x_even_inc
        {0x0382, 0x0007},        // #smiaRegs_rw_sub_sample_x_odd_inc
        {0x0384, 0x0001},        // #smiaRegs_rw_sub_sample_y_even_inc
        {0x0386, 0x0007},        // #smiaRegs_rw_sub_sample_y_odd_inc
        {0x0900, 0x0100},        // #smiaRegs_rw_binning_mode
        {0x0901, 0x4400},        // #smiaRegs_rw_binning_type
        {0x0902, 0x0100},        // #smiaRegs_rw_binning_weighting
        //Set FPS
        {0x0342, 0x0EDC},        // smiaRegs_rw_frame_timing_line_length_pck
        {0x0340, 0x023C},        // smiaRegs_rw_frame_timing_frame_length_lines
        //Set int.time
        {0x0200, 0x0618},        // smiaRegs_rw_integration_time_fine_integration_time
        {0x0202, 0x025E},        // smiaRegs_rw_integration_time_coarse_integration_time
        //Set gain
        {0x0204, 0x0020},        // X1
//M2M
        {0x31FE, 0xC004},         //ash_uDecompressXgrid[0]
        {0x3200, 0xC4F0},         //ash_uDecompressXgrid[1]
        {0x3202, 0xCEC8},         //ash_uDecompressXgrid[2]
        {0x3204, 0xD8A0},         //ash_uDecompressXgrid[3]
        {0x3206, 0xE278},         //ash_uDecompressXgrid[4]
        {0x3208, 0xEC50},         //ash_uDecompressXgrid[5]
        {0x320A, 0xF628},         //ash_uDecompressXgrid[6]
        {0x320C, 0x0000},         //ash_uDecompressXgrid[7]
        {0x320E, 0x09D8},         //ash_uDecompressXgrid[8]
        {0x3210, 0x13B0},         //ash_uDecompressXgrid[9]
        {0x3212, 0x1D88},         //ash_uDecompressXgrid[10]
        {0x3214, 0x2760},         //ash_uDecompressXgrid[11]
        {0x3216, 0x3138},         //ash_uDecompressXgrid[12]
        {0x3218, 0x3B10},         //ash_uDecompressXgrid[13]
        {0x321A, 0x3FFC},         //ash_uDecompressXgrid[14]

        {0x321C, 0xC004},         //ash_uDecompressYgrid[0]
        {0x321E, 0xCCD0},         //ash_uDecompressYgrid[1]
        {0x3220, 0xD99C},         //ash_uDecompressYgrid[2]
        {0x3222, 0xE668},         //ash_uDecompressYgrid[3]
        {0x3224, 0xF334},         //ash_uDecompressYgrid[4]
        {0x3226, 0x0000},         //ash_uDecompressYgrid[5]
        {0x3228, 0x0CCC},         //ash_uDecompressYgrid[6]
        {0x322A, 0x1998},         //ash_uDecompressYgrid[7]
        {0x322C, 0x2664},         //ash_uDecompressYgrid[8]
        {0x322E, 0x3330},         //ash_uDecompressYgrid[9]
        {0x3230, 0x3FFC},         //ash_uDecompressYgrid[10]

        {0x3232, 0x0100},           //ash_uDecompressWidth
        {0x3234, 0x0100},           //ash_uDecompressHeight

        {0x3238, 0x0909},           //ash_uDecompressRadiusShifter
        {0x323A, 0x0B0F},           //ash_uDecompressFinalScale
        {0x3236, 0x0E00},           //ash_uDecompressScale
        {0x0b00, 0x0120},           //smiaRegs_rw_isp_luminance_correction_level

//BASE Profile parabola start
        {0x3160, 0x0600},            //ash_GrasCfg
        {0x3161, 0x0000},            //ash_GrasShifter
        {0x3164, 0x09C4},         //ash_luma_params[0]_tmpr
        {0x3166, 0x0100},         //ash_luma_params[0]_alpha[0]
        {0x3168, 0x0100},         //ash_luma_params[0]_alpha[1]
        {0x316A, 0x0100},         //ash_luma_params[0]_alpha[2]
        {0x316C, 0x0100},         //ash_luma_params[0]_alpha[3]
        {0x316E, 0x0011},         //ash_luma_params[0]_beta[0]
        {0x3170, 0x002C},         //ash_luma_params[0]_beta[1]
        {0x3172, 0x0000},         //ash_luma_params[0]_beta[2]
        {0x3174, 0x0012},         //ash_luma_params[0]_beta[3]
        {0x3176, 0x0A8C},         //ash_luma_params[1]_tmpr
        {0x3178, 0x0100},         //ash_luma_params[1]_alpha[0]
        {0x317A, 0x0100},         //ash_luma_params[1]_alpha[1]
        {0x317C, 0x0100},         //ash_luma_params[1]_alpha[2]
        {0x317E, 0x0100},         //ash_luma_params[1]_alpha[3]
        {0x3180, 0x0011},         //ash_luma_params[1]_beta[0]
        {0x3182, 0x002C},         //ash_luma_params[1]_beta[1]
        {0x3184, 0x0000},         //ash_luma_params[1]_beta[2]
        {0x3186, 0x0012},         //ash_luma_params[1]_beta[3]
        {0x3188, 0x0CE4},         //ash_luma_params[2]_tmpr
        {0x318A, 0x0040},         //ash_luma_params[2]_alpha[0]    GR
        {0x318C, 0x0048},         //ash_luma_params[2]_alpha[1]    R
        {0x318E, 0x0040},         //ash_luma_params[2]_alpha[2]    B
        {0x3190, 0x0040},         //ash_luma_params[2]_alpha[3]    GB
        {0x3192, 0x0011},         //ash_luma_params[2]_beta[0]
        {0x3194, 0x002C},         //ash_luma_params[2]_beta[1]
        {0x3196, 0x0000},         //ash_luma_params[2]_beta[2]
        {0x3198, 0x0012},         //ash_luma_params[2]_beta[3]
        {0x319A, 0x1004},         //ash_luma_params[3]_tmpr
        {0x319C, 0x0100},         //ash_luma_params[3]_alpha[0]
        {0x319E, 0x0100},         //ash_luma_params[3]_alpha[1]
        {0x31A0, 0x0100},         //ash_luma_params[3]_alpha[2]
        {0x31A2, 0x0100},         //ash_luma_params[3]_alpha[3]
        {0x31A4, 0x0011},         //ash_luma_params[3]_beta[0]
        {0x31A6, 0x002C},         //ash_luma_params[3]_beta[1]
        {0x31A8, 0x0000},         //ash_luma_params[3]_beta[2]
        {0x31AA, 0x0012},         //ash_luma_params[3]_beta[3]
        {0x31AC, 0x1388},         //ash_luma_params[4]_tmpr
        {0x31AE, 0x0100},         //ash_luma_params[4]_alpha[0]
        {0x31B0, 0x0100},         //ash_luma_params[4]_alpha[1]
        {0x31B2, 0x0100},         //ash_luma_params[4]_alpha[2]
        {0x31B4, 0x0100},         //ash_luma_params[4]_alpha[3]
        {0x31B6, 0x0011},         //ash_luma_params[4]_beta[0]
        {0x31B8, 0x002C},         //ash_luma_params[4]_beta[1]
        {0x31BA, 0x0000},         //ash_luma_params[4]_beta[2]
        {0x31BC, 0x0012},         //ash_luma_params[4]_beta[3]
        {0x31BE, 0x1964},         //ash_luma_params[5]_tmpr
        {0x31C0, 0x0100},         //ash_luma_params[5]_alpha[0]
        {0x31C2, 0x0100},         //ash_luma_params[5]_alpha[1]
        {0x31C4, 0x0100},         //ash_luma_params[5]_alpha[2]
        {0x31C6, 0x0100},         //ash_luma_params[5]_alpha[3]
        {0x31C8, 0x0011},         //ash_luma_params[5]_beta[0]
        {0x31CA, 0x002C},         //ash_luma_params[5]_beta[1]
        {0x31CC, 0x0000},         //ash_luma_params[5]_beta[2]
        {0x31CE, 0x0012},         //ash_luma_params[5]_beta[3]
        {0x31D0, 0x1D4C},         //ash_luma_params[6]_tmpr
        {0x31D2, 0x0100},         //ash_luma_params[6]_alpha[0]
        {0x31D4, 0x0100},         //ash_luma_params[6]_alpha[1]
        {0x31D6, 0x0100},         //ash_luma_params[6]_alpha[2]
        {0x31D8, 0x0100},         //ash_luma_params[6]_alpha[3]
        {0x31DA, 0x0011},         //ash_luma_params[6]_beta[0]
        {0x31DC, 0x002C},         //ash_luma_params[6]_beta[1]
        {0x31DE, 0x0000},         //ash_luma_params[6]_beta[2]
        {0x31E0, 0x0012},         //ash_luma_params[6]_beta[3]
        {0x3162, 0x0100},           //ash_bLumaMode
        {0x301c, 0x0200},            //smiaRegs_vendor_gras_nvm_address//index 228: page#3, byte #36
        {0x301e, 0x0300},            //WsmiaRegs_vendor_gras_load_from
        {0x323C, 0x0101},            //Wash_bSkipNvmGrasOfs  // skipping the value set in nvm page 0 address 47
        {0x1989, 0x0004},            //smiaRegs_ro_edof_cap_uAlphaTempInd
};

static struct msm_camera_i2c_reg_conf s5k3h7_snap_settings[] = {
	/*Timing configuration*/
	{0x6218, 0xF1D0},	// open all clocks
	{0x6214, 0xF9F0},	// open all clocks
	{0xF400, 0x0BBC},	// workaround for the SW standby current
	{0x6226, 0x0001},	// open APB clock for I2C transaction
	{0xB0C0, 0x000C},
	{0x6226, 0x0000},	// close APB clock for I2C transaction
	{0x6218, 0xF9F0},	// close all clocks

	//Offset control
	{0x38FA, 0x0030},  // gisp_offs_gains_bls_offs_0_
	{0x38FC, 0x0030},  // gisp_offs_gains_bls_offs_1_
	{0x0086, 0x01ff},	// smiaRegs_rd_analog_gain_analogue_gain_code_max
	{0xF616, 0x0004},	//aig_tmc_gain

        {0x32BD, 0x00E8},
        {0x32BE, 0x001B},
        {0xF442, 0x0018},
        {0xF446, 0x002A},
        {0xF448, 0x0020},
        {0xF44A, 0x0014},
        {0xF44C, 0x0016},
        {0x32CE, 0x0060},
        {0x32D0, 0x0024},
        {0x3278, 0x0050},
        {0x012A, 0x0060},
        {0x012C, 0x7077},
        {0x012E, 0x7777},

        //Sensor configurations
        //set MCLK
	{0x0136, 0x1314},	// #smiaRegs_rw_op_cond_extclk_frequency_mhz
        //set PLL
	{0x0114, 0x0300},      // #smiaRegs_rw_output_lane_mode (03 - 4lane, 01 - 2lane)
	{0x0300, 0x0002},	// smiaRegs_rw_clocks_vt_pix_clk_div
	{0x0302, 0x0001},	// smiaRegs_rw_clocks_vt_sys_clk_div
	{0x0304, 0x0006},	// smiaRegs_rw_clocks_pre_pll_clk_div
	{0x0306, 0x00A3},	// smiaRegs_rw_clocks_pll_multiplier
	{0x0308, 0x0008},	// smiaRegs_rw_clocks_op_pix_clk_div
	{0x030A, 0x0001},	// smiaRegs_rw_clocks_op_sys_clk_div
	{0x030C, 0x0006},	// smiaRegs_rw_clocks_secnd_pre_pll_clk_div
	{0x030E, 0x00C8},	// smiaRegs_rw_clocks_secnd_pll_multiplier (MIPI: 334.75MHz -> 276.25MHz)
        //Set Output size
	{0x034C, 0x0CC0},	//0CC0	// smiaRegs_rw_frame_timing_x_output_size
	{0x034E, 0x0990},	//0990	// smiaRegs_rw_frame_timing_y_output_size
        //Set start-address
	{0x0344, 0x0004},	// smiaRegs_rw_frame_timing_x_addr_start
	{0x0346, 0x0004},	// smiaRegs_rw_frame_timing_y_addr_start
	{0x0348, 0x0CC3},	// smiaRegs_rw_frame_timing_x_addr_end
	{0x034A, 0x0993},	// smiaRegs_rw_frame_timing_y_addr_end
        //Binning & Subsampling
        {0x0380, 0x0001},        // #smiaRegs_rw_sub_sample_x_even_inc
        {0x0382, 0x0001},        // #smiaRegs_rw_sub_sample_x_odd_inc
        {0x0384, 0x0001},        // #smiaRegs_rw_sub_sample_y_even_inc
        {0x0386, 0x0001},        // #smiaRegs_rw_sub_sample_y_odd_inc
        {0x0900, 0x0000},        // #smiaRegs_rw_binning_mode
        {0x0902, 0x0100},        // #smiaRegs_rw_binning_weighting
	//Set FPS
	{0x0342, 0x0EDC},	// smiaRegs_rw_frame_timing_line_length_pck (7376d)
	{0x0340, 0x0AB8},	// smiaRegs_rw_frame_timing_frame_length_lines
	//Set int.time
	{0x0200, 0x0BEF},	// smiaRegs_rw_integration_time_fine_integration_time
	{0x0202, 0x060A},	// smiaRegs_rw_integration_time_coarse_integration_time
	{0x0204, 0x0020},
//M2M
        {0x31FE, 0xC004},         //ash_uDecompressXgrid[0]
        {0x3200, 0xC4F0},         //ash_uDecompressXgrid[1]
        {0x3202, 0xCEC8},         //ash_uDecompressXgrid[2]
        {0x3204, 0xD8A0},         //ash_uDecompressXgrid[3]
        {0x3206, 0xE278},         //ash_uDecompressXgrid[4]
        {0x3208, 0xEC50},         //ash_uDecompressXgrid[5]
        {0x320A, 0xF628},         //ash_uDecompressXgrid[6]
        {0x320C, 0x0000},         //ash_uDecompressXgrid[7]
        {0x320E, 0x09D8},         //ash_uDecompressXgrid[8]
        {0x3210, 0x13B0},         //ash_uDecompressXgrid[9]
        {0x3212, 0x1D88},         //ash_uDecompressXgrid[10]
        {0x3214, 0x2760},         //ash_uDecompressXgrid[11]
        {0x3216, 0x3138},         //ash_uDecompressXgrid[12]
        {0x3218, 0x3B10},         //ash_uDecompressXgrid[13]
        {0x321A, 0x3FFC},         //ash_uDecompressXgrid[14]

        {0x321C, 0xC004},         //ash_uDecompressYgrid[0]
        {0x321E, 0xCCD0},         //ash_uDecompressYgrid[1]
        {0x3220, 0xD99C},         //ash_uDecompressYgrid[2]
        {0x3222, 0xE668},         //ash_uDecompressYgrid[3]
        {0x3224, 0xF334},         //ash_uDecompressYgrid[4]
        {0x3226, 0x0000},         //ash_uDecompressYgrid[5]
        {0x3228, 0x0CCC},         //ash_uDecompressYgrid[6]
        {0x322A, 0x1998},         //ash_uDecompressYgrid[7]
        {0x322C, 0x2664},         //ash_uDecompressYgrid[8]
        {0x322E, 0x3330},         //ash_uDecompressYgrid[9]
        {0x3230, 0x3FFC},         //ash_uDecompressYgrid[10]

        {0x3232, 0x0100},           //ash_uDecompressWidth
        {0x3234, 0x0100},           //ash_uDecompressHeight

        {0x3238, 0x0909},           //ash_uDecompressRadiusShifter
        {0x323A, 0x0B0F},           //ash_uDecompressFinalScale
        {0x3236, 0x0E00},           //ash_uDecompressScale
        {0x0b00, 0x0120},           //smiaRegs_rw_isp_luminance_correction_level

//BASE Profile parabola start
        {0x3160, 0x0600},            //ash_GrasCfg
        {0x3161, 0x0000},            //ash_GrasShifter
        {0x3164, 0x09C4},         //ash_luma_params[0]_tmpr
        {0x3166, 0x0100},         //ash_luma_params[0]_alpha[0]
        {0x3168, 0x0100},         //ash_luma_params[0]_alpha[1]
        {0x316A, 0x0100},         //ash_luma_params[0]_alpha[2]
        {0x316C, 0x0100},         //ash_luma_params[0]_alpha[3]
        {0x316E, 0x0011},         //ash_luma_params[0]_beta[0]
        {0x3170, 0x002C},         //ash_luma_params[0]_beta[1]
        {0x3172, 0x0000},         //ash_luma_params[0]_beta[2]
        {0x3174, 0x0012},         //ash_luma_params[0]_beta[3]
        {0x3176, 0x0A8C},         //ash_luma_params[1]_tmpr
        {0x3178, 0x0100},         //ash_luma_params[1]_alpha[0]
        {0x317A, 0x0100},         //ash_luma_params[1]_alpha[1]
        {0x317C, 0x0100},         //ash_luma_params[1]_alpha[2]
        {0x317E, 0x0100},         //ash_luma_params[1]_alpha[3]
        {0x3180, 0x0011},         //ash_luma_params[1]_beta[0]
        {0x3182, 0x002C},         //ash_luma_params[1]_beta[1]
        {0x3184, 0x0000},         //ash_luma_params[1]_beta[2]
        {0x3186, 0x0012},         //ash_luma_params[1]_beta[3]
        {0x3188, 0x0CE4},         //ash_luma_params[2]_tmpr
        {0x318A, 0x0040},         //ash_luma_params[2]_alpha[0]    GR
        {0x318C, 0x0048},         //ash_luma_params[2]_alpha[1]    R
        {0x318E, 0x0040},         //ash_luma_params[2]_alpha[2]    B
        {0x3190, 0x0040},         //ash_luma_params[2]_alpha[3]    GB
        {0x3192, 0x0011},         //ash_luma_params[2]_beta[0]
        {0x3194, 0x002C},         //ash_luma_params[2]_beta[1]
        {0x3196, 0x0000},         //ash_luma_params[2]_beta[2]
        {0x3198, 0x0012},         //ash_luma_params[2]_beta[3]
        {0x319A, 0x1004},         //ash_luma_params[3]_tmpr
        {0x319C, 0x0100},         //ash_luma_params[3]_alpha[0]
        {0x319E, 0x0100},         //ash_luma_params[3]_alpha[1]
        {0x31A0, 0x0100},         //ash_luma_params[3]_alpha[2]
        {0x31A2, 0x0100},         //ash_luma_params[3]_alpha[3]
        {0x31A4, 0x0011},         //ash_luma_params[3]_beta[0]
        {0x31A6, 0x002C},         //ash_luma_params[3]_beta[1]
        {0x31A8, 0x0000},         //ash_luma_params[3]_beta[2]
        {0x31AA, 0x0012},         //ash_luma_params[3]_beta[3]
        {0x31AC, 0x1388},         //ash_luma_params[4]_tmpr
        {0x31AE, 0x0100},         //ash_luma_params[4]_alpha[0]
        {0x31B0, 0x0100},         //ash_luma_params[4]_alpha[1]
        {0x31B2, 0x0100},         //ash_luma_params[4]_alpha[2]
        {0x31B4, 0x0100},         //ash_luma_params[4]_alpha[3]
        {0x31B6, 0x0011},         //ash_luma_params[4]_beta[0]
        {0x31B8, 0x002C},         //ash_luma_params[4]_beta[1]
        {0x31BA, 0x0000},         //ash_luma_params[4]_beta[2]
        {0x31BC, 0x0012},         //ash_luma_params[4]_beta[3]
        {0x31BE, 0x1964},         //ash_luma_params[5]_tmpr
        {0x31C0, 0x0100},         //ash_luma_params[5]_alpha[0]
        {0x31C2, 0x0100},         //ash_luma_params[5]_alpha[1]
        {0x31C4, 0x0100},         //ash_luma_params[5]_alpha[2]
        {0x31C6, 0x0100},         //ash_luma_params[5]_alpha[3]
        {0x31C8, 0x0011},         //ash_luma_params[5]_beta[0]
        {0x31CA, 0x002C},         //ash_luma_params[5]_beta[1]
        {0x31CC, 0x0000},         //ash_luma_params[5]_beta[2]
        {0x31CE, 0x0012},         //ash_luma_params[5]_beta[3]
        {0x31D0, 0x1D4C},         //ash_luma_params[6]_tmpr
        {0x31D2, 0x0100},         //ash_luma_params[6]_alpha[0]
        {0x31D4, 0x0100},         //ash_luma_params[6]_alpha[1]
        {0x31D6, 0x0100},         //ash_luma_params[6]_alpha[2]
        {0x31D8, 0x0100},         //ash_luma_params[6]_alpha[3]
        {0x31DA, 0x0011},         //ash_luma_params[6]_beta[0]
        {0x31DC, 0x002C},         //ash_luma_params[6]_beta[1]
        {0x31DE, 0x0000},         //ash_luma_params[6]_beta[2]
        {0x31E0, 0x0012},         //ash_luma_params[6]_beta[3]
        {0x3162, 0x0100},           //ash_bLumaMode
        {0x301c, 0x0200},            //smiaRegs_vendor_gras_nvm_address//index 228: page#3, byte #36
        {0x301e, 0x0300},            //WsmiaRegs_vendor_gras_load_from
        {0x323C, 0x0101},            //Wash_bSkipNvmGrasOfs  // skipping the value set in nvm page 0 address 47
        {0x1989, 0x0004},            //smiaRegs_ro_edof_cap_uAlphaTempInd
};

static struct msm_camera_i2c_reg_conf s5k3h7_recommend_settings[] = {
	{0x6010, 0x0001},

	{0x6028, 0x7000},
        {0x602A, 0x1750}, 
	{0x6F12, 0x10B5},
	{0x6F12, 0x00F0},
	{0x6F12, 0xE1FB},
	{0x6F12, 0x00F0},
	{0x6F12, 0xE3FB},
	{0x6F12, 0x10BC},
	{0x6F12, 0x08BC},
	{0x6F12, 0x1847},
	{0x6F12, 0x2DE9},
	{0x6F12, 0x7040},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x3867},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0140},
	{0x6F12, 0xD6E1},
	{0x6F12, 0xB010},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x3057},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1421},
	{0x6F12, 0x81E2},
	{0x6F12, 0x0110},
	{0x6F12, 0x82E1},
	{0x6F12, 0x1411},
	{0x6F12, 0xD5E1},
	{0x6F12, 0xB020},
	{0x6F12, 0xC2E1},
	{0x6F12, 0x0110},
	{0x6F12, 0xC5E1},
	{0x6F12, 0xB010},
	{0x6F12, 0x00EB},
	{0x6F12, 0xE601},
	{0x6F12, 0xD6E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x1827},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1410},
	{0x6F12, 0x80E2},
	{0x6F12, 0x0100},
	{0x6F12, 0x81E1},
	{0x6F12, 0x1400},
	{0x6F12, 0xD5E1},
	{0x6F12, 0xB010},
	{0x6F12, 0x80E1},
	{0x6F12, 0x0100},
	{0x6F12, 0xC5E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xF406},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xB00C},
	{0x6F12, 0xA0E1},
	{0x6F12, 0xA011},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xEC06},
	{0x6F12, 0x90E5},
	{0x6F12, 0x0000},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xBA39},
	{0x6F12, 0x53E1},
	{0x6F12, 0x0100},
	{0x6F12, 0xD091},
	{0x6F12, 0xBE09},
	{0x6F12, 0xD081},
	{0x6F12, 0xBC09},
	{0x6F12, 0xC2E1},
	{0x6F12, 0xB003},
	{0x6F12, 0xBDE8},
	{0x6F12, 0x7040},
	{0x6F12, 0x2FE1},
	{0x6F12, 0x1EFF},
	{0x6F12, 0x2DE9},
	{0x6F12, 0x3840},
	{0x6F12, 0x10E3},
	{0x6F12, 0x0100},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0050},
	{0x6F12, 0x9F15},
	{0x6F12, 0xC406},
	{0x6F12, 0x9015},
	{0x6F12, 0x2400},
	{0x6F12, 0x5013},
	{0x6F12, 0x0000},
	{0x6F12, 0x000A},
	{0x6F12, 0x1900},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xB846},
	{0x6F12, 0xD4E5},
	{0x6F12, 0xD700},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0000},
	{0x6F12, 0x001A},
	{0x6F12, 0x0600},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0120},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x0010},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x2F00},
	{0x6F12, 0x00EB},
	{0x6F12, 0xC501},
	{0x6F12, 0xDDE5},
	{0x6F12, 0x0000},
	{0x6F12, 0xA0E1},
	{0x6F12, 0xA001},
	{0x6F12, 0xC4E5},
	{0x6F12, 0xD700},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x8046},
	{0x6F12, 0x94E5},
	{0x6F12, 0x0000},
	{0x6F12, 0xD0E5},
	{0x6F12, 0x1102},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0000},
	{0x6F12, 0x001A},
	{0x6F12, 0x0900},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0120},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x0010},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x3700},
	{0x6F12, 0x00EB},
	{0x6F12, 0xB901},
	{0x6F12, 0xDDE5},
	{0x6F12, 0x0010},
	{0x6F12, 0x94E5},
	{0x6F12, 0x0000},
	{0x6F12, 0xC0E5},
	{0x6F12, 0x1112},
	{0x6F12, 0x01E2},
	{0x6F12, 0xFF00},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x4816},
	{0x6F12, 0xC1E1},
	{0x6F12, 0xBE04},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0500},
	{0x6F12, 0xBDE8},
	{0x6F12, 0x3840},
	{0x6F12, 0x00EA},
	{0x6F12, 0xB201},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x3416},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0000},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x012C},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8030},
	{0x6F12, 0x83E2},
	{0x6F12, 0x013C},
	{0x6F12, 0x80E2},
	{0x6F12, 0x0100},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0400},
	{0x6F12, 0xC3E1},
	{0x6F12, 0xBE28},
	{0x6F12, 0xFFBA},
	{0x6F12, 0xF9FF},
	{0x6F12, 0x2FE1},
	{0x6F12, 0x1EFF},
	{0x6F12, 0x2DE9},
	{0x6F12, 0x7040},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x08C6},
	{0x6F12, 0xDCE5},
	{0x6F12, 0x1021},
	{0x6F12, 0x52E3},
	{0x6F12, 0x0000},
	{0x6F12, 0x001A},
	{0x6F12, 0x0A00},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x0CE6},
	{0x6F12, 0x8CE0},
	{0x6F12, 0x0231},
	{0x6F12, 0x8EE0},
	{0x6F12, 0x8250},
	{0x6F12, 0xD5E1},
	{0x6F12, 0xB050},
	{0x6F12, 0x93E5},
	{0x6F12, 0xD840},
	{0x6F12, 0x82E2},
	{0x6F12, 0x0120},
	{0x6F12, 0x04E0},
	{0x6F12, 0x9504},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x2444},
	{0x6F12, 0x52E3},
	{0x6F12, 0x0400},
	{0x6F12, 0x83E5},
	{0x6F12, 0xD840},
	{0x6F12, 0xFFBA},
	{0x6F12, 0xF5FF},
	{0x6F12, 0xBDE8},
	{0x6F12, 0x7040},
	{0x6F12, 0x00EA},
	{0x6F12, 0x9801},
	{0x6F12, 0x2DE9},
	{0x6F12, 0x1040},
	{0x6F12, 0x00EB},
	{0x6F12, 0x9801},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xD405},
	{0x6F12, 0xD0E5},
	{0x6F12, 0x7310},
	{0x6F12, 0xBDE8},
	{0x6F12, 0x1040},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xCC05},
	{0x6F12, 0xFFEA},
	{0x6F12, 0xE6FF},
	{0x6F12, 0x2DE9},
	{0x6F12, 0xFF4F},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xB445},
	{0x6F12, 0x4DE2},
	{0x6F12, 0xA4D0},
	{0x6F12, 0xD4E1},
	{0x6F12, 0xB20D},
	{0x6F12, 0xD4E5},
	{0x6F12, 0x9CA0},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0150},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x5800},
	{0x6F12, 0xD4E1},
	{0x6F12, 0xB40D},
	{0x6F12, 0x5AE3},
	{0x6F12, 0x1000},
	{0x6F12, 0xA023},
	{0x6F12, 0x10A0},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x5400},
	{0x6F12, 0xD4E5},
	{0x6F12, 0xDB00},
	{0x6F12, 0xD4E5},
	{0x6F12, 0xD710},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x2020},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1500},
	{0x6F12, 0x81E2},
	{0x6F12, 0x0310},
	{0x6F12, 0x01E2},
	{0x6F12, 0xFF70},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0010},
	{0x6F12, 0x8DE5},
	{0x6F12, 0xA000},
	{0x6F12, 0xCDE1},
	{0x6F12, 0xBC07},
	{0x6F12, 0xCDE1},
	{0x6F12, 0xBC05},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x4C10},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB000},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x5010},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xF600},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x4800},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB000},
	{0x6F12, 0xD4E5},
	{0x6F12, 0xD910},
	{0x6F12, 0xD0E5},
	{0x6F12, 0x0800},
	{0x6F12, 0x80E0},
	{0x6F12, 0x0100},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x4400},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1500},
	{0x6F12, 0x80E0},
	{0x6F12, 0xA00F},
	{0x6F12, 0xA0E1},
	{0x6F12, 0xC000},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x4000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x3C15},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x2000},
	{0x6F12, 0x00EB},
	{0x6F12, 0x6F01},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x3415},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x1820},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x0800},
	{0x6F12, 0x00EB},
	{0x6F12, 0x6B01},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x2825},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0000},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x0400},
	{0x6F12, 0x92E5},
	{0x6F12, 0x0020},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xF004},
	{0x6F12, 0xD2E5},
	{0x6F12, 0x5921},
	{0x6F12, 0x90E5},
	{0x6F12, 0x4010},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xAC30},
	{0x6F12, 0x82E0},
	{0x6F12, 0x8221},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8210},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8310},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xFA30},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xBE04},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xF210},
	{0x6F12, 0x60E2},
	{0x6F12, 0x012C},
	{0x6F12, 0x02E0},
	{0x6F12, 0x9302},
	{0x6F12, 0x20E0},
	{0x6F12, 0x9120},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0004},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x4008},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x0000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xC084},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x5000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x5410},
	{0x6F12, 0x88E0},
	{0x6F12, 0x8000},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xFC0B},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0150},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9100},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0064},
	{0x6F12, 0xB0E1},
	{0x6F12, 0x4668},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA000},
	{0x6F12, 0xA053},
	{0x6F12, 0x0210},
	{0x6F12, 0xE043},
	{0x6F12, 0x0110},
	{0x6F12, 0x00EB},
	{0x6F12, 0x4C01},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0098},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x4998},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0140},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x05B0},
	{0x6F12, 0x00EA},
	{0x6F12, 0x0800},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x5C00},
	{0x6F12, 0x80E0},
	{0x6F12, 0x8450},
	{0x6F12, 0x55E1},
	{0x6F12, 0xF200},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA010},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9600},
	{0x6F12, 0x89E0},
	{0x6F12, 0x8000},
	{0x6F12, 0x00EB},
	{0x6F12, 0x4001},
	{0x6F12, 0x84E2},
	{0x6F12, 0x0140},
	{0x6F12, 0xC5E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x54E1},
	{0x6F12, 0x0A00},
	{0x6F12, 0xFFDA},
	{0x6F12, 0xF4FF},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0090},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x4804},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x5810},
	{0x6F12, 0x80E0},
	{0x6F12, 0x8900},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xFE09},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9100},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0064},
	{0x6F12, 0xB0E1},
	{0x6F12, 0x4668},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA000},
	{0x6F12, 0xA053},
	{0x6F12, 0x0210},
	{0x6F12, 0xE043},
	{0x6F12, 0x0110},
	{0x6F12, 0x00EB},
	{0x6F12, 0x3001},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0088},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x4888},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0140},
	{0x6F12, 0x00EA},
	{0x6F12, 0x0800},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x7C00},
	{0x6F12, 0x80E0},
	{0x6F12, 0x8450},
	{0x6F12, 0x55E1},
	{0x6F12, 0xF200},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA010},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9600},
	{0x6F12, 0x88E0},
	{0x6F12, 0x8000},
	{0x6F12, 0x00EB},
	{0x6F12, 0x2501},
	{0x6F12, 0x84E2},
	{0x6F12, 0x0140},
	{0x6F12, 0xC5E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x54E1},
	{0x6F12, 0x0A00},
	{0x6F12, 0xFFDA},
	{0x6F12, 0xF4FF},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0080},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0860},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0850},
	{0x6F12, 0x00EA},
	{0x6F12, 0x2300},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0040},
	{0x6F12, 0x00EA},
	{0x6F12, 0x1E00},
	{0x6F12, 0x45E0},
	{0x6F12, 0x0400},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x7C10},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x5C20},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8410},
	{0x6F12, 0x82E0},
	{0x6F12, 0x8000},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xF010},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xF000},
	{0x6F12, 0x0BE0},
	{0x6F12, 0x9100},
	{0x6F12, 0x5BE3},
	{0x6F12, 0x0000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA000},
	{0x6F12, 0xA0A3},
	{0x6F12, 0x0210},
	{0x6F12, 0xE0B3},
	{0x6F12, 0x0110},
	{0x6F12, 0x00EB},
	{0x6F12, 0x0E01},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA010},
	{0x6F12, 0x80E0},
	{0x6F12, 0x0B00},
	{0x6F12, 0x00EB},
	{0x6F12, 0x0B01},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA410},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0120},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8610},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xF010},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9100},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0210},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1117},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0000},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1227},
	{0x6F12, 0x62B2},
	{0x6F12, 0x0020},
	{0x6F12, 0x80E0},
	{0x6F12, 0x0200},
	{0x6F12, 0x00EB},
	{0x6F12, 0xFF00},
	{0x6F12, 0x88E0},
	{0x6F12, 0x0080},
	{0x6F12, 0x86E2},
	{0x6F12, 0x0160},
	{0x6F12, 0x84E2},
	{0x6F12, 0x0140},
	{0x6F12, 0x54E1},
	{0x6F12, 0x0500},
	{0x6F12, 0xFFDA},
	{0x6F12, 0xDEFF},
	{0x6F12, 0x85E2},
	{0x6F12, 0x0150},
	{0x6F12, 0x55E1},
	{0x6F12, 0x0A00},
	{0x6F12, 0xFFDA},
	{0x6F12, 0xD9FF},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x0000},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x021B},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9800},
	{0x6F12, 0x81E0},
	{0x6F12, 0x4014},
	{0x6F12, 0x51E3},
	{0x6F12, 0x020B},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x4C10},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xAC20},
	{0x6F12, 0xA0A1},
	{0x6F12, 0x4004},
	{0x6F12, 0x80A2},
	{0x6F12, 0x020B},
	{0x6F12, 0x82E0},
	{0x6F12, 0x0111},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA820},
	{0x6F12, 0xA0B3},
	{0x6F12, 0x020B},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0008},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x4008},
	{0x6F12, 0x82E0},
	{0x6F12, 0x8110},
	{0x6F12, 0xC1E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x0410},
	{0x6F12, 0x89E2},
	{0x6F12, 0x0190},
	{0x6F12, 0x50E1},
	{0x6F12, 0x0100},
	{0x6F12, 0xA0D1},
	{0x6F12, 0x0100},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x0400},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x4C00},
	{0x6F12, 0x59E3},
	{0x6F12, 0x0F00},
	{0x6F12, 0x80E2},
	{0x6F12, 0x0100},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x4C00},
	{0x6F12, 0xFFBA},
	{0x6F12, 0xA1FF},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x5000},
	{0x6F12, 0x80E2},
	{0x6F12, 0x0100},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0B00},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x5000},
	{0x6F12, 0xFFBA},
	{0x6F12, 0x7EFF},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x0400},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xAC20},
	{0x6F12, 0x50E3},
	{0x6F12, 0x020A},
	{0x6F12, 0xA0C1},
	{0x6F12, 0x0004},
	{0x6F12, 0xA0C1},
	{0x6F12, 0xC01F},
	{0x6F12, 0x80C0},
	{0x6F12, 0xA109},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xA812},
	{0x6F12, 0xA0D3},
	{0x6F12, 0x010C},
	{0x6F12, 0x81E0},
	{0x6F12, 0x8210},
	{0x6F12, 0xA0C1},
	{0x6F12, 0xC006},
	{0x6F12, 0x8DE5},
	{0x6F12, 0x9C10},
	{0x6F12, 0xC1E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x9C10},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xF400},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xB010},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0050},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0004},
	{0x6F12, 0x00EB},
	{0x6F12, 0xC500},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0088},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB000},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x4888},
	{0x6F12, 0xC0E1},
	{0x6F12, 0xB480},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x4800},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0060},
	{0x6F12, 0x40E2},
	{0x6F12, 0x029B},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB010},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x0800},
	{0x6F12, 0x80E0},
	{0x6F12, 0x8600},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xF000},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xF210},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0040},
	{0x6F12, 0x40E0},
	{0x6F12, 0x0100},
	{0x6F12, 0x07E0},
	{0x6F12, 0x9000},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xB010},
	{0x6F12, 0x8DE2},
	{0x6F12, 0x2000},
	{0x6F12, 0x80E0},
	{0x6F12, 0x8400},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xF000},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xF010},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x20C2},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0130},
	{0x6F12, 0x40E0},
	{0x6F12, 0x0100},
	{0x6F12, 0x02E0},
	{0x6F12, 0x9000},
	{0x6F12, 0xDCE5},
	{0x6F12, 0xD800},
	{0x6F12, 0x82E0},
	{0x6F12, 0x0720},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1310},
	{0x6F12, 0x81E0},
	{0x6F12, 0xA11F},
	{0x6F12, 0x82E0},
	{0x6F12, 0xC110},
	{0x6F12, 0xDCE5},
	{0x6F12, 0xDA20},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x3110},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x1302},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x4030},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x44C0},
	{0x6F12, 0x23E0},
	{0x6F12, 0x9931},
	{0x6F12, 0x80E0},
	{0x6F12, 0x533C},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x40C0},
	{0x6F12, 0x80E0},
	{0x6F12, 0xA00F},
	{0x6F12, 0x21E0},
	{0x6F12, 0x98C1},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x44C0},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x511C},
	{0x6F12, 0x01E0},
	{0x6F12, 0x9301},
	{0x6F12, 0x81E0},
	{0x6F12, 0xC000},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x50B2},
	{0x6F12, 0x9DE5},
	{0x6F12, 0x9C00},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xA820},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xB010},
	{0x6F12, 0x9DE5},
	{0x6F12, 0xAC00},
	{0x6F12, 0x80E0},
	{0x6F12, 0x0501},
	{0x6F12, 0x82E0},
	{0x6F12, 0x80A0},
	{0x6F12, 0xDAE1},
	{0x6F12, 0xF000},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0004},
	{0x6F12, 0x00EB},
	{0x6F12, 0x9000},
	{0x6F12, 0x40E0},
	{0x6F12, 0x0B00},
	{0x6F12, 0x84E2},
	{0x6F12, 0x0140},
	{0x6F12, 0x54E3},
	{0x6F12, 0x0F00},
	{0x6F12, 0x85E2},
	{0x6F12, 0x0150},
	{0x6F12, 0xCAE1},
	{0x6F12, 0xB000},
	{0x6F12, 0xFFBA},
	{0x6F12, 0xD3FF},
	{0x6F12, 0x86E2},
	{0x6F12, 0x0160},
	{0x6F12, 0x56E3},
	{0x6F12, 0x0B00},
	{0x6F12, 0xFFBA},
	{0x6F12, 0xC8FF},
	{0x6F12, 0x8DE2},
	{0x6F12, 0xB4D0},
	{0x6F12, 0xBDE8},
	{0x6F12, 0xF04F},
	{0x6F12, 0x2FE1},
	{0x6F12, 0x1EFF},
	{0x6F12, 0x2DE9},
	{0x6F12, 0xF041},
	{0x6F12, 0x00EB},
	{0x6F12, 0x8400},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0000},
	{0x6F12, 0xBD08},
	{0x6F12, 0xF041},
	{0x6F12, 0xA003},
	{0x6F12, 0x0010},
	{0x6F12, 0xA003},
	{0x6F12, 0x3800},
	{0x6F12, 0x000A},
	{0x6F12, 0x8100},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x6C11},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xBA01},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xBC21},
	{0x6F12, 0xD1E1},
	{0x6F12, 0xBE11},
	{0x6F12, 0x80E1},
	{0x6F12, 0x0208},
	{0x6F12, 0x00EB},
	{0x6F12, 0x7D00},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0070},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x5451},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x5401},
	{0x6F12, 0xD5E1},
	{0x6F12, 0xF030},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xBAEA},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xBCCA},
	{0x6F12, 0xD5E1},
	{0x6F12, 0xF220},
	{0x6F12, 0x00E0},
	{0x6F12, 0x930C},
	{0x6F12, 0x42E0},
	{0x6F12, 0x0360},
	{0x6F12, 0x02E0},
	{0x6F12, 0x9E02},
	{0x6F12, 0x4CE0},
	{0x6F12, 0x0E40},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0410},
	{0x6F12, 0x40E0},
	{0x6F12, 0x0200},
	{0x6F12, 0x00EB},
	{0x6F12, 0x6900},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0080},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x2401},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x10E3},
	{0x6F12, 0x020C},
	{0x6F12, 0xA011},
	{0x6F12, 0x0700},
	{0x6F12, 0x001B},
	{0x6F12, 0x6B00},
	{0x6F12, 0x56E3},
	{0x6F12, 0x0000},
	{0x6F12, 0xE003},
	{0x6F12, 0x0000},
	{0x6F12, 0x000A},
	{0x6F12, 0x0300},
	{0x6F12, 0x47E0},
	{0x6F12, 0x0800},
	{0x6F12, 0x00E0},
	{0x6F12, 0x9400},
	{0x6F12, 0xA0E1},
	{0x6F12, 0x0610},
	{0x6F12, 0x00EB},
	{0x6F12, 0x6200},
	{0x6F12, 0xC5E1},
	{0x6F12, 0xB400},
	{0x6F12, 0xBDE8},
	{0x6F12, 0xF041},
	{0x6F12, 0x2FE1},
	{0x6F12, 0x1EFF},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xEC10},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xEC00},
	{0x6F12, 0x2DE9},
	{0x6F12, 0x1040},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xE820},
	{0x6F12, 0x80E5},
	{0x6F12, 0x5010},
	{0x6F12, 0x42E0},
	{0x6F12, 0x0110},
	{0x6F12, 0xC0E1},
	{0x6F12, 0xB415},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xDC00},
	{0x6F12, 0x4FE2},
	{0x6F12, 0xD410},
	{0x6F12, 0x00EB},
	{0x6F12, 0x5900},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xD400},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xD440},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x9420},
	{0x6F12, 0x84E5},
	{0x6F12, 0x0400},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x0000},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x011C},
	{0x6F12, 0x82E0},
	{0x6F12, 0x8030},
	{0x6F12, 0x80E2},
	{0x6F12, 0x0100},
	{0x6F12, 0x50E3},
	{0x6F12, 0x0400},
	{0x6F12, 0xC3E1},
	{0x6F12, 0xB010},
	{0x6F12, 0xFF3A},
	{0x6F12, 0xFAFF},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xB000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xB410},
	{0x6F12, 0x84E5},
	{0x6F12, 0x5C00},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xA800},
	{0x6F12, 0x84E5},
	{0x6F12, 0x2C00},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xA800},
	{0x6F12, 0x00EB},
	{0x6F12, 0x4700},
	{0x6F12, 0x9FE5},
	{0x6F12, 0xA400},
	{0x6F12, 0x4FE2},
	{0x6F12, 0x711E},
	{0x6F12, 0x84E5},
	{0x6F12, 0x0000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x9C00},
	{0x6F12, 0x00EB},
	{0x6F12, 0x4200},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x2410},
	{0x6F12, 0xC1E1},
	{0x6F12, 0xB000},
	{0x6F12, 0x9FE5},
	{0x6F12, 0x5C00},
	{0x6F12, 0xD0E1},
	{0x6F12, 0xB012},
	{0x6F12, 0x51E3},
	{0x6F12, 0x1000},
	{0x6F12, 0x009A},
	{0x6F12, 0x0200},
	{0x6F12, 0xA0E3},
	{0x6F12, 0x090C},
	{0x6F12, 0x00EB},
	{0x6F12, 0x3400},
	{0x6F12, 0xFFEA},
	{0x6F12, 0xFEFF},
	{0x6F12, 0xBDE8},
	{0x6F12, 0x1040},
	{0x6F12, 0x2FE1},
	{0x6F12, 0x1EFF},
	{0x6F12, 0x0070},
	{0x6F12, 0xC41F},
	{0x6F12, 0x00D0},
	{0x6F12, 0x0061},
	{0x6F12, 0x0070},
	{0x6F12, 0x5014},
	{0x6F12, 0x0070},
	{0x6F12, 0x0000},
	{0x6F12, 0x00D0},
	{0x6F12, 0x00F4},
	{0x6F12, 0x0070},
	{0x6F12, 0x7004},
	{0x6F12, 0x0070},
	{0x6F12, 0xD005},
	{0x6F12, 0x0070},
	{0x6F12, 0xC61F},
	{0x6F12, 0x0070},
	{0x6F12, 0x1013},
	{0x6F12, 0x0070},
	{0x6F12, 0xB412},
	{0x6F12, 0x0070},
	{0x6F12, 0x8C1F},
	{0x6F12, 0x0070},
	{0x6F12, 0xAC1F},
	{0x6F12, 0x0070}, 
	{0x6F12, 0x0400},
	{0x6F12, 0x00D0},
	{0x6F12, 0x0093},
	{0x6F12, 0x0070},
	{0x6F12, 0x8012},
	{0x6F12, 0x0070},
	{0x6F12, 0xC00B},
	{0x6F12, 0x0070},
	{0x6F12, 0xE012},
	{0x6F12, 0x0070},
	{0x6F12, 0xD01F},
	{0x6F12, 0x0070},
	{0x6F12, 0x7005},
	{0x6F12, 0x0070},
	{0x6F12, 0x902D},
	{0x6F12, 0x0000},
	{0x6F12, 0x90A6},
	{0x6F12, 0x0070},
	{0x6F12, 0xFC18},
	{0x6F12, 0x0070},
	{0x6F12, 0xF804},
	{0x6F12, 0x0070},
	{0x6F12, 0x9818},
	{0x6F12, 0x0070},
	{0x6F12, 0xE018},
	{0x6F12, 0x0070},
	{0x6F12, 0x7018},
	{0x6F12, 0x0000},
	{0x6F12, 0xC06A},
	{0x6F12, 0x0070},
	{0x6F12, 0xE017},
	{0x6F12, 0x0000},
	{0x6F12, 0x781C},
	{0x6F12, 0x7847},
	{0x6F12, 0xC046},
	{0x6F12, 0xFFEA},
	{0x6F12, 0xB4FF},
	{0x6F12, 0x7847},
	{0x6F12, 0xC046},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x6CCE},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x781C},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x54C0},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x8448},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x146C},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x4C7E},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x8CDC},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x48DD},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x7C55},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x744C},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0xE8DE},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0x4045},
	{0x6F12, 0x1FE5},
	{0x6F12, 0x04F0},
	{0x6F12, 0x0000},
	{0x6F12, 0xE8CD},
	{0x6F12, 0x80F9},
	{0x6F12, 0x00FA},
	{0x6F12, 0x00FB},
	{0x6F12, 0x00FC},
	{0x6F12, 0x00FD},
	{0x6F12, 0x00FE},
	{0x6F12, 0x00FF},
	{0x6F12, 0x0000},
	{0x6F12, 0x0001},
	{0x6F12, 0x0002},
	{0x6F12, 0x0003},
	{0x6F12, 0x0004},
	{0x6F12, 0x0005},
	{0x6F12, 0x0006},
	{0x6F12, 0x8006},
	{0x6F12, 0x0000},
	{0x6F12, 0x00FB},
	{0x6F12, 0x00FC},
	{0x6F12, 0x00FD},
	{0x6F12, 0x00FE},
	{0x6F12, 0x00FF},
	{0x6F12, 0x0000},
	{0x6F12, 0x0001},
	{0x6F12, 0x0002},
	{0x6F12, 0x0003},
	{0x6F12, 0x0004},
	{0x6F12, 0x0005},
	{0x6F12, 0x0000},
	
	{0x6028, 0xD000},
};

static struct v4l2_subdev_info s5k3h7_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array s5k3h7_init_conf[] = {
	{&s5k3h7_recommend_settings[0],
	ARRAY_SIZE(s5k3h7_recommend_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_camera_i2c_conf_array s5k3h7_confs[] = {
	{&s5k3h7_snap_settings[0],
	ARRAY_SIZE(s5k3h7_snap_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
	{&s5k3h7_prev_settings[0],
	ARRAY_SIZE(s5k3h7_prev_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
        {&s5k3h7_prev_settings[0],
        ARRAY_SIZE(s5k3h7_prev_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
        {&s5k3h7_video_60fps_settings[0],
        ARRAY_SIZE(s5k3h7_video_60fps_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
        {&s5k3h7_video_90fps_settings[0],
        ARRAY_SIZE(s5k3h7_video_90fps_settings), 0, MSM_CAMERA_I2C_WORD_DATA},
        {&s5k3h7_video_120fps_settings[0],
        ARRAY_SIZE(s5k3h7_video_120fps_settings), 0, MSM_CAMERA_I2C_WORD_DATA},

};

static struct msm_sensor_output_info_t s5k3h7_dimensions[] = {
	/* snapshot */
	{
		.x_output = 0xCC0, /* 3264 */
		.y_output = 0x990, /* 2448 */
		.line_length_pclk = 0xEDC, /* 3470 */
		.frame_length_lines = 0xAB8, /* 2496*/
		.vt_pixel_clk = 260800000,
		.op_pixel_clk = 269000000,
		.binning_factor = 1,
	},
	/* preview */
	{
		.x_output = 0xCC0, /* 3264 */
		.y_output = 0x990, /* 2448 */
		.line_length_pclk = 0xEDC, /* 3470 */
		.frame_length_lines = 0xAB8, /* 2496*/
		.vt_pixel_clk = 260800000,
		.op_pixel_clk = 269000000,
		.binning_factor = 1,
	},
	/* 3264x2448,25fps keep for FHD*/
        {
                .x_output = 0xCC0,
                .y_output = 0x990,
                .line_length_pclk = 0xEDC,
                .frame_length_lines = 0xAB8,
                .vt_pixel_clk = 260800000/*320000000*/,
                .op_pixel_clk = 269000000/*320000000*/,
                .binning_factor = 1,
        },
	/* 1312x736,60fps*/
        {
                .x_output = 1312,
                .y_output = 736,
                .line_length_pclk = 0xEDC,
                .frame_length_lines = 0x477,
                .vt_pixel_clk = 260800000/*320000000*/,
                .op_pixel_clk = 269000000/*320000000*/,
                .binning_factor = 1,
        },
	/* 1312x736,90fps*/
        {
                .x_output = 1312,
                .y_output = 736,
                .line_length_pclk = 0xEDC,
                .frame_length_lines = 0x2FA,
                .vt_pixel_clk = 260800000/*320000000*/,
                .op_pixel_clk = 269000000/*320000000*/,
                .binning_factor = 1,
        },
	/* 800x600,120fps*/
        {
                .x_output = 800,
                .y_output = 600,
                .line_length_pclk = 0xEDC,
                .frame_length_lines = 0x23C,
                .vt_pixel_clk = 261100000/*320000000*/,
                .op_pixel_clk = 269000000/*320000000*/,
                .binning_factor = 1,
        },
};

static struct msm_camera_csi_params s5k3h7_csic_params = {
        .data_format = CSI_10BIT,
        .lane_cnt    = 4,
        .lane_assign = 0xe4,
        .dpcm_scheme = 0,
        .settle_cnt  = 0x14,
};

static struct msm_camera_csi_params *s5k3h7_csic_params_array[] = {
        &s5k3h7_csic_params,
        &s5k3h7_csic_params,
        &s5k3h7_csic_params,
        &s5k3h7_csic_params,
        &s5k3h7_csic_params,
        &s5k3h7_csic_params,
};


static struct msm_camera_csid_vc_cfg s5k3h7_cid_cfg[] = {
	{0, CSI_RAW10, CSI_DECODE_10BIT},
        {1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params s5k3h7_csi_params = {
	.csid_params = {
		.lane_cnt = 4,
		.lut_params = {
			.num_cid = ARRAY_SIZE(s5k3h7_cid_cfg),
			.vc_cfg = s5k3h7_cid_cfg,
		},
	},
	.csiphy_params = {
		.lane_cnt = 4,
		.settle_cnt = 0x14,
	},
};

static struct msm_camera_csi2_params *s5k3h7_csi_params_array[] = {
	&s5k3h7_csi_params,
	&s5k3h7_csi_params,
	&s5k3h7_csi_params,
	&s5k3h7_csi_params,
	&s5k3h7_csi_params,
	&s5k3h7_csi_params,
};

static struct msm_sensor_output_reg_addr_t s5k3h7_reg_addr = {
	.x_output = 0x34C,
	.y_output = 0x34E,
	.line_length_pclk = 0x342,
	.frame_length_lines = 0x340,
};

static struct msm_sensor_id_info_t s5k3h7_id_info = {
	.sensor_id_reg_addr = 0x0,
	.sensor_id = 0x3087,
};

static struct msm_sensor_exp_gain_info_t s5k3h7_exp_gain_info = {
	.coarse_int_time_addr = 0x202,
	.global_gain_addr = 0x204,
	.vert_offset = 8,
};

static const struct i2c_device_id s5k3h7_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&s5k3h7_s_ctrl},
	{ }
};

static struct i2c_driver s5k3h7_i2c_driver = {
	.id_table = s5k3h7_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client s5k3h7_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

#if defined(CONFIG_MACH_MITWO)
static char * s5k3h7_name[3] = {"s5k3h7", "s5k3h7a", "s5k3h7l"};
#else
static char * s5k3h7_name[3] = {"s5k3h7a", "s5k3h7a", "s5k3h7l"};
#endif
static int32_t s5k3h7_match_id(struct msm_sensor_ctrl_t *s_ctrl) {

	int32_t rc = 0;
	uint16_t chipid = 0;
	uint16_t byte = 0;
	int32_t i;

	rc = msm_camera_i2c_read(
			s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_id_info->sensor_id_reg_addr, &chipid,
			MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		pr_err("%s: %s: read id failed\n", __func__,
				s_ctrl->sensordata->sensor_name);
		return rc;
	}

	CDBG("s5k3h7_sensor id: 0x%04x\n", chipid);
	if( (chipid == 0x3087) || (chipid == 0x0803) || (chipid == 0x0A0A) ) {
		pr_info("%s :0x%04x", __func__, chipid);
		if(otp_slot != 0)
			return rc;
	} else {
		pr_err("s5k_sensor_match_id chip id doesnot match 0x%04x\n", chipid);
		return -ENODEV;
	}

	// otp slot == 0, read otp to find lsc
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x6010, 0x0001, MSM_CAMERA_I2C_WORD_DATA);
	msleep(10);
	msm_camera_i2c_write_tbl(s_ctrl->sensor_i2c_client, s5k3h7_otp_settings,
			ARRAY_SIZE(s5k3h7_otp_settings), MSM_CAMERA_I2C_WORD_DATA);

	for( i = 2; i > 0; i--) {
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0A02, i, MSM_CAMERA_I2C_BYTE_DATA);//page 1,2
		msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0A00, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
		for( chipid = 0; chipid < 5; chipid++) {
			msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x0A01, &byte, MSM_CAMERA_I2C_BYTE_DATA);
			if(byte == 1)
				break;
			msleep(10);
		}
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x0A04, &byte, MSM_CAMERA_I2C_WORD_DATA);
		pr_info("s5k3h7 vendor:0x%04x", byte);
		if(byte == 0)
			continue;

		if(byte & 0x1000) /* PRIMAX SHICOH VCM */
			s_ctrl->sensordata->actuator_info->cam_name = 0;//ACTUATOR_MAIN_CAM_0
		else if(byte & 0x2000) /* PRIMAX MITSUMI VCM */
			s_ctrl->sensordata->actuator_info->cam_name = 1;//ACTUATOR_MAIN_CAM_1

		byte &= 0x0FFF;

		if(byte == 0x0803) { /* Primax M2 */
			s_ctrl->sensordata->sensor_name = s5k3h7_name[0];
		} else if(byte == 0x0802) {/* Primax M2A */
			s_ctrl->sensordata->sensor_name = s5k3h7_name[1];
		} else if(byte == 0x0A0A) {/* Liteon M2A */
			s_ctrl->sensordata->sensor_name = s5k3h7_name[2];
		}
		break;
	}

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0A02, 0x0D, MSM_CAMERA_I2C_BYTE_DATA);//page D
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0A00, 0x01, MSM_CAMERA_I2C_BYTE_DATA);
	for( rc = 0; rc < 5; rc++) {
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x0A01, &byte, MSM_CAMERA_I2C_BYTE_DATA);
		if(byte == 1)
			break;
		msleep(10);
	}
	pr_info("s5k3h7: otp read:%d", byte);
	if(byte != 1) {
		otp_slot = 0xFF;
		return 0;
	}

	for( rc = 7; rc > 3; rc-- ) {
		msm_camera_i2c_read(s_ctrl->sensor_i2c_client, 0x0A00+rc, &byte, MSM_CAMERA_I2C_BYTE_DATA);
		pr_info("s5k3h7 %04x :%04x", 0x0A00+rc, byte);
		if(byte == 0)
			continue;

		if((byte == 0x11) || (byte == 0x22) || (byte == 0xFF))
			break;
		byte = 0;
	}
	if(byte !=0)
		otp_slot = byte;
	else
		otp_slot = 0xFF;

	if(otp_slot == 0x22) {
		pr_info("s5k3h7 use lsc2");
		s5k3h7_snap_settings[ARRAY_SIZE(s5k3h7_snap_settings) - 4].reg_data = 0x00E4;
		s5k3h7_prev_settings[ARRAY_SIZE(s5k3h7_prev_settings) - 4].reg_data = 0x00E4;
		s5k3h7_video_60fps_settings[ARRAY_SIZE(s5k3h7_video_60fps_settings) - 4].reg_data = 0x00E4;
		s5k3h7_video_90fps_settings[ARRAY_SIZE(s5k3h7_video_90fps_settings) - 4].reg_data = 0x00E4;
		s5k3h7_video_120fps_settings[ARRAY_SIZE(s5k3h7_video_120fps_settings) - 4].reg_data = 0x00E4;
	}

	return 0;
}


static int32_t s5k3h7_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint32_t fl_lines;
	uint8_t  offset;
	uint16_t digital_gain_int = 0;
	uint16_t digital_gain = 0x100;

	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;

	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
			MSM_CAMERA_I2C_WORD_DATA);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
			MSM_CAMERA_I2C_WORD_DATA);

	if((gain & 0x00FF) < 0x20 ) { /* large than 8x gain*/
		digital_gain_int = (gain & 0x00FF);
		digital_gain = (digital_gain_int << 8) + ((gain & 0xFF00) >> 8);
		gain = 256;
	}

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
			s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
			MSM_CAMERA_I2C_WORD_DATA);
	/* update digital gain */
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x020E, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0210, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0212, digital_gain, MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client, 0x0214, digital_gain, MSM_CAMERA_I2C_WORD_DATA);

	pr_debug("imx175 gain:0x%x digital:0x%x digital_int:0x%x", gain, digital_gain, digital_gain_int);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&s5k3h7_i2c_driver);
}

static struct v4l2_subdev_core_ops s5k3h7_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k3h7_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops s5k3h7_subdev_ops = {
	.core = &s5k3h7_subdev_core_ops,
	.video  = &s5k3h7_subdev_video_ops,
};

static struct msm_sensor_fn_t s5k3h7_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = s5k3h7_write_exp_gain,
	.sensor_write_snapshot_exp_gain = s5k3h7_write_exp_gain,
	.sensor_setting = msm_sensor_setting,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_match_id = s5k3h7_match_id,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
//	.sensor_adjust_frame_lines = msm_sensor_adjust_frame_lines,
	.sensor_get_csi_params = msm_sensor_get_csi_params,

};

static struct msm_sensor_reg_t s5k3h7_regs = {
	.default_data_type = MSM_CAMERA_I2C_WORD_DATA,
	.start_stream_conf = s5k3h7_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(s5k3h7_start_settings),
	.stop_stream_conf = s5k3h7_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(s5k3h7_stop_settings),
	.group_hold_on_conf = s5k3h7_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(s5k3h7_groupon_settings),
	.group_hold_off_conf = s5k3h7_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(s5k3h7_groupoff_settings),
	.init_settings = &s5k3h7_init_conf[0],
	.init_size = ARRAY_SIZE(s5k3h7_init_conf),
	.mode_settings = &s5k3h7_confs[0],
	.output_settings = &s5k3h7_dimensions[0],
	.num_conf = ARRAY_SIZE(s5k3h7_confs),
};

static struct msm_sensor_ctrl_t s5k3h7_s_ctrl = {
	.msm_sensor_reg = &s5k3h7_regs,
	.sensor_i2c_client = &s5k3h7_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &s5k3h7_reg_addr,
	.sensor_id_info = &s5k3h7_id_info,
	.sensor_exp_gain_info = &s5k3h7_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
        .csic_params = &s5k3h7_csic_params_array[0],
	.csi_params = &s5k3h7_csi_params_array[0],
	.msm_sensor_mutex = &s5k3h7_mut,
	.sensor_i2c_driver = &s5k3h7_i2c_driver,
	.sensor_v4l2_subdev_info = s5k3h7_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k3h7_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k3h7_subdev_ops,
	.func_tbl = &s5k3h7_func_tbl,
//	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.clk_rate = 19200000,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("SAMSUNG S5K3H7 Bayer sensor driver");
MODULE_LICENSE("GPL v2");


