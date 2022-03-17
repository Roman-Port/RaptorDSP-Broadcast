#include <stdio.h>
#include "test.h"

float test_noise_levels[][2] = {
	{ -100.0f,    0.0f },
	{ -100.0f, -100.0f },
	{  -60.0f,  -60.0f },
	{  -30.0f,  -30.0f },
	{  -20.0f,  -20.0f },
	{  -15.0f,  -15.0f },
	{  -10.0f,  -10.0f },
	{   -8.0f,   -8.0f },
	{   -6.0f,   -6.0f },
	{   -4.0f,   -4.0f },
	{   -3.0f,   -3.0f },
	{   -2.0f,   -2.0f },
	{   -1.0f,   -1.0f },
	{    0.0f,    0.0f }
};

void run_test_logged(FILE* file, int number, test_params_t params, test_results_t* results) {
	printf("RUNNING TEST %i (%f -> %f dB noise)...", number, params.noise_floor_start_db, params.noise_floor_end_db);
	run_test(file, params, results);
	printf("OK\n");
	printf("    RDS Frames       : %i\n", results->rds_total_frames);
	printf("    RDS Sync Losses  : %i\n", results->rds_sync_loss_incidents);
	printf("    Avg Pll Error    : %f\n", results->avg_pll_error);
	printf("    Avg Level L (dB) : %f\n", results->avg_level_l_db);
	printf("    Avg Level R (dB) : %f\n", results->avg_level_r_db);
	printf("    Avg L/R Ratio    : %f\n", results->avg_lr_ratio);
	printf("    Avg L/R Mixing   : %f\n", results->avg_lr_mixing);
}

int main() {
	//Open the file we'll be reading
	FILE* input = fopen("C:\\Users\\Roman\\Desktop\\benchmark.raw", "rb");
	if (input == 0) {
		printf("FATAL: Failed to open benchmark file.\n");
		return 1;
	}

	//Prepare the parameters
	test_params_t params;
	params.input_sample_rate = 750000;
	params.buffer_size = 65536;
	params.skip_samples = params.input_sample_rate * 1;
	params.duration_samples = params.input_sample_rate * 9;

	//Run tests
	test_results_t results;
	for (int i = 0; i < sizeof(test_noise_levels) / sizeof(float[2]); i++) {
		params.noise_floor_start_db = test_noise_levels[i][0];
		params.noise_floor_end_db = test_noise_levels[i][1];
		sprintf(params.output_filename_prefix, "C:\\Users\\Roman\\Desktop\\benchmark\\test_%i", i + 1);
		run_test_logged(input, i + 1, params, &results);
	}

	//Release files
	fclose(input);

	return 0;
}