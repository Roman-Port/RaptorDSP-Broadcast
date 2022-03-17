#pragma once

#include <volk/volk_typedefs.h>
#include "primitive/fm_demod.h"
#include "audio/stereo_processor.h"
#include "audio/deemphasis_decoder.h"
#include "rds/demodulator.h"
#include "../util/volk_buffer.h"

class fm_receiver {

public:
	fm_receiver(size_t buffer_size);
	~fm_receiver();

	float configure(float sample_rate);
	int process(const lv_32fc_t* iq, int count, float* audio_l, float* audio_r);

	fm_demod fm;
	stereo_processor stereo;
	rds_demodulator rds;
	deemphasis_decoder deemphasis_l;
	deemphasis_decoder deemphasis_r;

private:
	volk_buffer<float> buffer_mpx;

};