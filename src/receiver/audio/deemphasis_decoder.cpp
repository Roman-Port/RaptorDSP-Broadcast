#include "deemphasis_decoder.h"
#include <cmath>
#include <cstring>

deemphasis_decoder::deemphasis_decoder(float deemphasis_time) :
    sample_rate(0),
    deemphasis_time(deemphasis_time),
    alpha(0),
    state(0)
{

}

void deemphasis_decoder::set_sample_rate(float sample_rate) {
    this->sample_rate = sample_rate;
    configure();
}

void deemphasis_decoder::set_deemphasis_time(float deemphasis_time) {
    this->deemphasis_time = deemphasis_time;
    configure();
}

float deemphasis_decoder::get_deemphasis_time() {
    return deemphasis_time;
}

void deemphasis_decoder::configure() {
    if (sample_rate != 0 && deemphasis_time != 0)
        alpha = 1.0f - exp(-1.0f / (sample_rate * (deemphasis_time * 1e-6f)));
    else
        alpha = 0;
}

void deemphasis_decoder::process(const float* input, float* output, int count) {
    if (alpha == 0) {
        //Disabled. Simply copy
        memcpy(output, input, sizeof(float) * count);
    }
    else {
        //Apply
        for (int i = 0; i < count; i++)
        {
            state += alpha * (input[i] - state);
            output[i] = state;
        }
    }
}