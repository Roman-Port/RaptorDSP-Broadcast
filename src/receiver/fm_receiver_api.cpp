#include <raptorbroadcast/fm_receiver.h>
#include <cassert>
#include "fm_receiver.h"
#include "../defines.h"

#define CONTEXT ((fm_receiver*)ctx)

raptorbroadcast_fm_receiver_t* raptorbroadcast_fm_receiver_create(size_t buffer_size) {
	return (raptorbroadcast_fm_receiver_t*)new fm_receiver(buffer_size);
}

float raptorbroadcast_fm_receiver_configure(raptorbroadcast_fm_receiver_t* ctx, float sample_rate) {
	return CONTEXT->configure(sample_rate);
}

int raptorbroadcast_fm_receiver_process(raptorbroadcast_fm_receiver_t* ctx, const lv_32fc_t* input_iq, int count, float* buffer_audio_l, float* buffer_audio_r) {
	return CONTEXT->process(input_iq, count, buffer_audio_l, buffer_audio_r);
}

void raptorbroadcast_fm_receiver_setup(raptorbroadcast_fm_receiver_t* ctx, int region) {
	switch (region) {
	case RAPTORBROADCAST_REGION_US:
		raptorbroadcast_fm_receiver_set_fm_deviation(ctx, FM_DEVIATION_US);
		raptorbroadcast_fm_receiver_set_deemphasis_time(ctx, DEEMPHASIS_TIME_US);
		break;
	case RAPTORBROADCAST_REGION_EU:
		raptorbroadcast_fm_receiver_set_fm_deviation(ctx, FM_DEVIATION_EU);
		raptorbroadcast_fm_receiver_set_deemphasis_time(ctx, DEEMPHASIS_TIME_EU);
		break;
	default:
		raptorbroadcast_fm_receiver_set_fm_deviation(ctx, FM_DEVIATION_DEFAULT);
		raptorbroadcast_fm_receiver_set_deemphasis_time(ctx, DEEMPHASIS_TIME_DEFAULT);
		break;
	}
}

void raptorbroadcast_fm_receiver_bind_rds_frame(raptorbroadcast_fm_receiver_t* ctx, raptorbroadcast_fm_receiver_rds_frame_cb callback, void* user_ctx) {
	CONTEXT->rds.sync.cb_frame_ctx = user_ctx;
	CONTEXT->rds.sync.cb_frame = callback;
}

void raptorbroadcast_fm_receiver_bind_rds_status(raptorbroadcast_fm_receiver_t* ctx, raptorbroadcast_fm_receiver_rds_status_cb callback, void* user_ctx) {
	CONTEXT->rds.sync.cb_status_ctx = user_ctx;
	CONTEXT->rds.sync.cb_status = callback;
}

float raptorbroadcast_fm_receiver_get_fm_deviation(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->fm.get_deviation();
}

void raptorbroadcast_fm_receiver_set_fm_deviation(raptorbroadcast_fm_receiver_t* ctx, float fm_deviation) {
	CONTEXT->fm.set_deviation(fm_deviation);
}

float raptorbroadcast_fm_receiver_get_deemphasis_time(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->deemphasis_l.get_deemphasis_time();
}

void raptorbroadcast_fm_receiver_set_deemphasis_time(raptorbroadcast_fm_receiver_t* ctx, float deemphasis_time) {
	CONTEXT->deemphasis_l.set_deemphasis_time(deemphasis_time);
	CONTEXT->deemphasis_r.set_deemphasis_time(deemphasis_time);
}

int raptorbroadcast_fm_receiver_get_stereo_mode(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->stereo.stereo_mode;
}

void raptorbroadcast_fm_receiver_set_stereo_mode(raptorbroadcast_fm_receiver_t* ctx, int mode) {
	CONTEXT->stereo.stereo_mode = mode;
}

float raptorbroadcast_fm_receiver_get_stereo_mixing_min(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->stereo.mixing_min;
}

void raptorbroadcast_fm_receiver_set_stereo_mixing_min(raptorbroadcast_fm_receiver_t* ctx, float min) {
	CONTEXT->stereo.mixing_min = min;
}

float raptorbroadcast_fm_receiver_get_stereo_mixing_max(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->stereo.mixing_max;
}

void raptorbroadcast_fm_receiver_set_stereo_mixing_max(raptorbroadcast_fm_receiver_t* ctx, float max) {
	CONTEXT->stereo.mixing_max = max;
}

float raptorbroadcast_fm_receiver_get_stereo_mixing(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->stereo.get_stereo_mixing();
}

float raptorbroadcast_fm_receiver_get_stereo_raw_error(raptorbroadcast_fm_receiver_t* ctx) {
	return CONTEXT->stereo.pll.d_avg_error;
}

void raptorbroadcast_fm_receiver_destroy(raptorbroadcast_fm_receiver_t* ctx) {
	delete (CONTEXT);
}