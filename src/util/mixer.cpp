#include "mixer.h"
#include "../defines.h"
#include <volk/volk.h>

mixer::mixer(float freq_offset) :
	sample_rate(0),
	freq_offset(freq_offset),
	phase(1, 0),
	inc(0, 0)
{
	configure();
}

mixer::~mixer() {

}

void mixer::set_freq_offset(float freq_offset) {
	this->freq_offset = freq_offset;
	configure();
}

void mixer::set_sample_rate(float sample_rate) {
	this->sample_rate = sample_rate;
	configure();
}

void mixer::configure() {
	//Check if it's valid or else we'll divide by zero
	if (sample_rate == 0) {
		inc.real(0);
		inc.imag(0);
	}
	else {
		float angle = 2 * M_PI * freq_offset / sample_rate;
		inc.real(std::cos(angle));
		inc.imag(std::sin(angle));
	}
}

void mixer::process(const float* input, lv_32fc_t* output, int count) {
	//Convert
	for (int i = 0; i < count; i++) {
		output[i] = lv_32fc_t(input[i], 0);
	}

	//Process as normal
	process(output, output, count);
}

void mixer::process(const lv_32fc_t* input, lv_32fc_t* output, int count) {
	volk_32fc_s32fc_x2_rotator_32fc(output, input, inc, &phase, count);
}