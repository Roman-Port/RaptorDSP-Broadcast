#include "stereo_processor.h"
#include "../../defines.h"
#include <raptorbroadcast/fm_receiver.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>

#define STEREO_PILOT_FREQ 19000
#define AUDIO_FILTER_CUTOFF 16000

stereo_processor::stereo_processor(size_t bufferSize) :
    stereo_mode(RAPTORBROADCAST_STEREO_MODE_AUTO),
    pll(M_PI / 200, 0, 0),
    mixing_min(FM_STEREO_MIXING_MIN),
    mixing_max(FM_STEREO_MIXING_MAX)
{
    pilot_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * bufferSize);
    pll_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * bufferSize);
    mpx_delayed = (float*)malloc(sizeof(float) * bufferSize);
}

stereo_processor::~stereo_processor() {
    free(pilot_buffer);
    free(pll_buffer);
    free(mpx_delayed);
}

float stereo_processor::configure(float mpxSampleRate) {
    //Configure pilot filter
    raptor_filter_builder_bandpass pilotFilterBuilder(mpxSampleRate, STEREO_PILOT_FREQ - 500, STEREO_PILOT_FREQ + 500);
    pilotFilterBuilder.automatic_tap_count(1000, 30);
    pilot_filter.configure(&pilotFilterBuilder, 1);

    //Calculate the filter delay
    mpx_delay_line.configure(pilotFilterBuilder.get_ntaps() - ((pilotFilterBuilder.get_ntaps() - 1) / 2), 0.0f);

    //Configure pilot PLL
    pll.d_max_freq = 2 * M_PI * (STEREO_PILOT_FREQ + 4) / mpxSampleRate;
    pll.d_min_freq = 2 * M_PI * (STEREO_PILOT_FREQ - 4) / mpxSampleRate;

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

int stereo_processor::process(float* mpx, int count, float* audioL, float* audioR) {
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

    //Decode stereo and apply mixing as needed
    float mixingStereo = get_stereo_mixing();
    float mixingMono = 1 - mixingStereo;
    float add;
    float sub;
    for (int i = 0; i < audioCount; i++)
    {
        add = audioL[i]; //L+R signal
        sub = 2 * audioR[i]; //L-R signal
        audioL[i] = (mixingStereo * (add + sub)) + (mixingMono * add);
        audioR[i] = (mixingStereo * (add - sub)) + (mixingMono * add);
    }

    return audioCount;
}

float stereo_processor::get_stereo_mixing() {
    switch (stereo_mode) {
    case RAPTORBROADCAST_STEREO_MODE_AUTO: //Smooth transition between on and off
        return 1.0f - std::max(0.0f, std::min(1.0f, ((std::abs(pll.d_avg_error) - mixing_min) / (mixing_max - mixing_min))));
    case RAPTORBROADCAST_STEREO_MODE_BINARY: //Either on or off. Judge by comparing to the average of the start and stop
        return (std::abs(pll.d_avg_error) < ((mixing_min + mixing_max) / 2.0f)) ? 1.0f : 0.0f;
    case RAPTORBROADCAST_STEREO_MODE_OFF: //Force off
        return 0;
    case RAPTORBROADCAST_STEREO_MODE_FORCE: //Force on
        return 1;
    }
    return 0; //Unknown mode!
}