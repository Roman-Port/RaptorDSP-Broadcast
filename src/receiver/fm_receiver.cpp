#include "fm_receiver.h"
#include "../defines.h"
#include <cassert>

fm_receiver::fm_receiver(size_t buffer_size) :
	buffer_mpx(buffer_size),
	fm(FM_DEVIATION_DEFAULT),
	stereo(buffer_size),
	rds(buffer_size),
	deemphasis_l(DEEMPHASIS_TIME_DEFAULT),
	deemphasis_r(DEEMPHASIS_TIME_DEFAULT)
{

}

fm_receiver::~fm_receiver() {

}

float fm_receiver::configure(float sample_rate) {
	fm.set_sample_rate(sample_rate);
	rds.configure(sample_rate);
	float audioSampleRate = stereo.configure(sample_rate);
	deemphasis_l.set_sample_rate(audioSampleRate);
	deemphasis_r.set_sample_rate(audioSampleRate);
	return audioSampleRate;
}

int fm_receiver::process(const lv_32fc_t* iq, int count, float* audio_l, float* audio_r) {
	//Sanity check
	assert(count <= buffer_mpx.count);
	assert(iq != 0);

	//Demodulate FM into the MPX buffer
	fm.process(iq, buffer_mpx.buffer, count);

	//Process RDS
	rds.process(buffer_mpx.buffer, count);

	//Process audio if buffers are supplied
	int audioCount;
	if (audio_l != 0 && audio_r != 0) {
		audioCount = stereo.process(buffer_mpx.buffer, count, audio_l, audio_r);
		deemphasis_l.process(audio_l, audio_l, audioCount);
		deemphasis_r.process(audio_r, audio_r, audioCount);
	}
	else {
		audioCount = 0;
	}

	return audioCount;
}