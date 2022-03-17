#pragma once

#include <volk/volk_typedefs.h>

class mixer {

public:
	mixer(float freq_offset);
	~mixer();

	void set_sample_rate(float sample_rate);
	void set_freq_offset(float freq_offset);

	void process(const float* input, lv_32fc_t* output, int count);
	void process(const lv_32fc_t* input, lv_32fc_t* output, int count);
public:
	float sample_rate;
	float freq_offset;

	lv_32fc_t phase;
	lv_32fc_t inc;

	void configure();

};