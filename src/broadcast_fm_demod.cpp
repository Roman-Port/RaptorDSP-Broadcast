#include <raptordsp/broadcast/broadcast_fm_demod.h>
#include <raptordsp/fast_atan2.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <volk/volk.h>
#include <cassert>

raptor_broadcast_fm_demod::raptor_broadcast_fm_demod(size_t bufferSize, float deemphasisTime) :
    buffer_size(bufferSize),
    deemphasis_time(deemphasisTime),
    mpx_buffer((float*)malloc(sizeof(float) * bufferSize)),
    gain(0),
    last_sample(0),
    rds_demod(bufferSize),
    rds_sync(),
    rds_callback(0),
    rds_callback_ctx(0),
    stereo(bufferSize),
    deemphasis_l(),
    deemphasis_r()
{
    assert(mpx_buffer != 0);
}

raptor_broadcast_fm_demod::~raptor_broadcast_fm_demod() {
    free(mpx_buffer);
}

float raptor_broadcast_fm_demod::configure(float sampleRate) {
    //Set up FM baseband demodulation
    gain = sampleRate / (2 * M_PI * 75000);
    last_sample = 0;

    //Configure RDS
    rds_demod.configure(sampleRate);

    //Configure audio
    float audioSampleRate = stereo.configure(sampleRate);
    deemphasis_l.configure(audioSampleRate, deemphasis_time);
    deemphasis_r.configure(audioSampleRate, deemphasis_time);
    return audioSampleRate;
}

void raptor_broadcast_fm_demod::bind_rds_callback(raptor_broadcast_fm_demod_rds_frame_cb callback, void* ctx) {
    rds_callback = callback;
    rds_callback_ctx = ctx;
}

int raptor_broadcast_fm_demod::process(raptor_complex* iq, int count, float* audioL, float* audioR) {
    //Demodulate baseband signal
    lv_32fc_t temp;
    for (int i = 0; i < count; i++) {
        //Apply conjugate
        temp = iq[i] * std::conj(last_sample);

        //Estimate angle
        mpx_buffer[i] = fast_atan2f(imag(temp), real(temp)) * gain;

        //Set state
        last_sample = iq[i];
    }

    //Process RDS
    rds_demod.input(mpx_buffer, count);
    unsigned char rdsSymbol;
    rds_frame_t rdsFrame;
    while (rds_demod.output(&rdsSymbol)) {
        if (rds_sync.process(rdsSymbol, &rdsFrame) && rds_callback != 0)
            rds_callback(rds_callback_ctx, &rdsFrame);
    }

    //Process audio
    int audioCount = stereo.process(mpx_buffer, count, audioL, audioR);
    deemphasis_l.process(audioL, audioL, audioCount);
    deemphasis_r.process(audioR, audioR, audioCount);
    return audioCount;
}

bool raptor_broadcast_fm_demod::is_rds_detected() {
    return rds_sync.d_state == rds_sync.SYNC;
}

bool raptor_broadcast_fm_demod::is_stereo_detected() {
    return stereo.is_stereo_detected();
}

bool raptor_broadcast_fm_demod::get_stereo_enabled() {
    return stereo.stereo_enabled;
}

void raptor_broadcast_fm_demod::set_stereo_enabled(bool enabled) {
    stereo.stereo_enabled = enabled;
}