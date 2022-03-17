#include "pll.h"
#include "fast_atan2.h"
#include "../defines.h"
#include <cassert>
#include <math.h>

pll::pll(float loop_bw, float max_freq, float min_freq) :
    d_phase(0),
    d_freq(0),
    d_max_freq(max_freq),
    d_min_freq(min_freq),
    d_loop_bw(loop_bw),
    d_avg_error(0)
{
    //Set the damping factor for a critically damped system
    d_damping = sqrtf(2.0f) / 2.0f;

    //Update gains
    float denom = (1.0 + 2.0 * d_damping * d_loop_bw + d_loop_bw * d_loop_bw);
    d_alpha = (4 * d_damping * d_loop_bw) / denom;
    d_beta = (4 * d_loop_bw * d_loop_bw) / denom;
}

float mod_2pi(float in)
{
    if (in > M_PI)
        return in - (2.0 * M_PI);
    else if (in < -M_PI)
        return in + (2.0 * M_PI);
    else
        return in;
}

float phase_detector(lv_32fc_t sample, float ref_phase)
{
    float sample_phase;
    sample_phase = fast_atan2f(sample.imag(), sample.real());
    return mod_2pi(sample_phase - ref_phase);
}

void pll::process(const lv_32fc_t* input, lv_32fc_t* output, int count) {
    assert(count >= 0);
    float error;
    for (int i = 0; i < count; i++) {
        //Calculate error
        error = phase_detector(input[i], d_phase);

        //Calculate output from current phase
        output[i].real(cosf(d_phase));
        output[i].imag(sinf(d_phase));

        //Process
        advance_loop(error);
        phase_wrap();
        frequency_limit();
    }
}

void pll::advance_loop(float error)
{
    d_avg_error = (1 - d_alpha) * d_avg_error + d_alpha * error * error;
    d_freq = d_freq + d_beta * error;
    d_phase = d_phase + d_freq + d_alpha * error;
}

void pll::phase_wrap()
{
    while (d_phase > (2 * M_PI))
        d_phase -= 2 * M_PI;
    while (d_phase < (-2 * M_PI))
        d_phase += 2 * M_PI;
}

void pll::frequency_limit()
{
    if (d_freq > d_max_freq)
        d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
        d_freq = d_min_freq;
}