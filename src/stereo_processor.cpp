#include <raptordsp/broadcast/stereo_processor.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>

#define STEREO_PILOT_FREQ 19000
#define AUDIO_FILTER_CUTOFF 16000

raptor_stereo_processor::raptor_stereo_processor(size_t bufferSize) :
    stereo_enabled(true)
{
    pilot_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * bufferSize);
    pll_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * bufferSize);
    mpx_delayed = (float*)malloc(sizeof(float) * bufferSize);
}

raptor_stereo_processor::~raptor_stereo_processor() {
    free(pilot_buffer);
    free(pll_buffer);
    free(mpx_delayed);
}

float raptor_stereo_processor::configure(float mpxSampleRate) {
    //Configure pilot filter
    raptor_filter_builder_bandpass pilotFilterBuilder(mpxSampleRate, STEREO_PILOT_FREQ - 500, STEREO_PILOT_FREQ + 500);
    pilotFilterBuilder.automatic_tap_count(1000, 30);
    pilot_filter.configure(&pilotFilterBuilder, 1);

    //Calculate the filter delay
    mpx_delay_line.configure(pilotFilterBuilder.get_ntaps() - ((pilotFilterBuilder.get_ntaps() - 1) / 2), 0.0f);

    //Configure pilot PLL
    pll.configure(M_PI / 200,
        2 * M_PI * (STEREO_PILOT_FREQ + 4) / mpxSampleRate,
        2 * M_PI * (STEREO_PILOT_FREQ - 4) / mpxSampleRate);

    //Create audio filter taps
    raptor_filter_builder_lowpass audioFilterBuilder(mpxSampleRate, AUDIO_FILTER_CUTOFF);
    audioFilterBuilder.automatic_tap_count(1000);

    //Configure
    float audioSampleRate;
    int decimation = audioFilterBuilder.calculate_decimation(&audioSampleRate);
    audio_filter_l.configure(&audioFilterBuilder, decimation);
    audio_filter_r.configure(&audioFilterBuilder, decimation);

    return audioSampleRate;
}

int raptor_stereo_processor::process(float* mpx, int count, float* audioL, float* audioR) {
    //Filter the pilot
    pilot_filter.process(mpx, pilot_buffer, count);

    //Process PLL on the pilot to get a clean waveform
    pll.process(pilot_buffer, pll_buffer, count);

    //Delay mpx by the filter delay to keep the audio in phase
    mpx_delay_line.process(mpx, mpx_delayed, count);

    //Demodulate L-R using the stereo pilot
    for (int i = 0; i < count; i++)
        audioR[i] = mpx_delayed[i] * std::imag(pll_buffer[i] * pll_buffer[i]);

    //Filter and decimate channels
    int audioCount = audio_filter_l.process(mpx_delayed, audioL, count);
    audio_filter_r.process(audioR, audioR, count);

    //If stereo is detected, enter recovery. Otherwise, copy L to R as usual
    if (is_stereo_detected() && stereo_enabled) {
        //Recover L and R audio channels
        float add;
        float sub;
        for (int i = 0; i < audioCount; i++)
        {
            add = audioL[i]; //L+R signal
            sub = 2 * audioR[i]; //L-R signal
            audioL[i] = (add + sub);
            audioR[i] = (add - sub);
        }
    }
    else {
        //Copy from the left buffer into the right buffer
        memcpy(audioR, audioL, sizeof(float) * audioCount);
    }

    return audioCount;
}

bool raptor_stereo_processor::is_stereo_detected() {
    return pll.get_avg_error() < 0.01f;
}