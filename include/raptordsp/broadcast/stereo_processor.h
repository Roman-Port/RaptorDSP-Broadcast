#include <raptordsp/misc/delay_line.h>
#include <raptordsp/analog/pll.h>
#include <raptordsp/filter/fir/filter_complex.h>

class raptor_stereo_processor {

public:
	raptor_stereo_processor(size_t bufferSize);
	~raptor_stereo_processor();

	float configure(float mpxSampleRate);
	int process(float* mpx, int count, float* audioL, float* audioR);
	bool is_stereo_detected();

	bool stereo_enabled;

private:
	raptor_filter_fcc pilot_filter;
	raptor_complex* pilot_buffer;

	float* mpx_delayed;
	raptor_delay_line_float mpx_delay_line;

	raptor_pll pll;
	raptor_complex* pll_buffer;	

	raptor_filter_real audio_filter_l;
	raptor_filter_real audio_filter_r;
	

};