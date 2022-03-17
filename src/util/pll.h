#pragma once

#include <volk/volk_typedefs.h>

class pll {

public:
	pll(float loop_bw, float max_freq, float min_freq);
	void process(const lv_32fc_t* input, lv_32fc_t* output, int count);

	float d_max_freq, d_min_freq;
	float d_avg_error;

private:
	float d_phase, d_freq;
	float d_damping, d_loop_bw;
	float d_alpha, d_beta;

	void advance_loop(float error);
	void phase_wrap();
	void frequency_limit();

};