#pragma once

#include <raptordsp/filter/fir/filter_complex.h>
#include "../../util/delay_line.h"
#include "../../util/pll.h"

class stereo_processor {

public:
	stereo_processor(size_t buffer_size);
	~stereo_processor();

	float configure(float mpxSampleRate);
	int process(float* mpx, int count, float* audioL, float* audioR);
	
	int stereo_mode;
	float mixing_min;
	float mixing_max;

	/// <summary>
	/// Returns the mix amount between [0, 1]. 0 indicates no stereo, 1 indicates full stereo, and any value between them indicates how much mixing is occuring.
	/// </summary>
	/// <returns></returns>
	float get_stereo_mixing();

	pll pll;

private:
	raptor_filter_fcc pilot_filter;
	raptor_complex* pilot_buffer;

	float* mpx_delayed;
	delay_line_float mpx_delay_line;

	raptor_complex* pll_buffer;

	raptor_filter_real audio_filter_l;
	raptor_filter_real audio_filter_r;


};