#pragma once

#include <volk/volk_typedefs.h>

class fm_demod {

public:
	fm_demod(float deviation);
	~fm_demod();

	void set_sample_rate(float sample_rate);
	void set_deviation(float deviation);
	float get_deviation();

	void process(const lv_32fc_t* input, float* output, int count);

private:
	float sample_rate;
	float deviation;

	float gain;
	lv_32fc_t last_sample;

	void configure();

};