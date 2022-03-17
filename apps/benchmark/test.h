#pragma once

#include <stdio.h>

struct test_params_t {

	/// <summary>
	/// Output filename prefix for files
	/// </summary>
	char output_filename_prefix[256];

	/// <summary>
	/// The sample rate of the input file
	/// </summary>
	int input_sample_rate;

	/// <summary>
	/// Internal working buffer size
	/// </summary>
	int buffer_size;

	/// <summary>
	/// Number of samples to skip without actually performing any readings
	/// </summary>
	int skip_samples;

	/// <summary>
	/// Number of samples to run the test for
	/// </summary>
	int duration_samples;

	/// <summary>
	/// The noise floor to apply, in dB
	/// </summary>
	float noise_floor_start_db;

	/// <summary>
	/// The noise floor to apply, in dB
	/// </summary>
	float noise_floor_end_db;

};

struct test_results_t {

	/// <summary>
	/// Sample rate of the internal IF
	/// </summary>
	float sample_rate_if;

	/// <summary>
	/// Sample rate of output audio.
	/// </summary>
	float sample_rate_audio;

	/// <summary>
	/// Total RDS frames recieved
	/// </summary>
	int rds_total_frames;

	/// <summary>
	/// Number of times RDS sync was lost
	/// </summary>
	int rds_sync_loss_incidents;

	/// <summary>
	/// Average error on the stereo pilot PLL
	/// </summary>
	float avg_pll_error;

	/// <summary>
	/// Average level of the left channel, in dB
	/// </summary>
	float avg_level_l_db;

	/// <summary>
	/// Average level of the right channel, in dB
	/// </summary>
	float avg_level_r_db;

	/// <summary>
	/// Average ratio between the left and right samples, calculated for each output sample using L/R. Higher is better
	/// </summary>
	float avg_lr_ratio;

	/// <summary>
	/// Average mixing value
	/// </summary>
	float avg_lr_mixing;

};

void run_test(FILE* file, test_params_t params, test_results_t* results);