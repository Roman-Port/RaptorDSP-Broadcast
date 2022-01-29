#pragma once

#include <raptordsp/defines.h>
#include <raptordsp/broadcast/stereo_processor.h>
#include <raptordsp/broadcast/deemphasis_processor.h>
#include <raptordsp/broadcast/rds_demod.h>
#include <raptordsp/broadcast/rds_sync.h>

typedef void(*raptor_broadcast_fm_demod_rds_frame_cb)(void* ctx, rds_frame_t* frame);

class raptor_broadcast_fm_demod {

public:
	raptor_broadcast_fm_demod(size_t bufferSize, float deemphasisTime);
	~raptor_broadcast_fm_demod();

	float configure(float sampleRate);
	void bind_rds_callback(raptor_broadcast_fm_demod_rds_frame_cb callback, void* ctx);

	int process(raptor_complex* iq, int count, float* audioL, float* audioR);

	bool is_rds_detected();
	bool is_stereo_detected();

	bool get_stereo_enabled();
	void set_stereo_enabled(bool enabled);

private:
	size_t buffer_size;
	float deemphasis_time;

	float* mpx_buffer;

	float gain;
	raptor_complex last_sample;

	raptor_rds_demod rds_demod;
	raptor_rds_sync rds_sync;
	raptor_broadcast_fm_demod_rds_frame_cb rds_callback;
	void* rds_callback_ctx;

	raptor_stereo_processor stereo;
	raptor_deemphasis_processor deemphasis_l;
	raptor_deemphasis_processor deemphasis_r;

};