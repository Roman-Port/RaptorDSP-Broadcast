#include <raptordsp/broadcast/deemphasis_processor.h>
#include <cmath>
#include <cstring>

raptor_deemphasis_processor::raptor_deemphasis_processor() :
    alpha(0),
    state(0)
{

}

void raptor_deemphasis_processor::configure(float sampleRate, float deemphasisTime) {
    if (sampleRate != 0 && deemphasisTime != 0)
        alpha = 1.0f - exp(-1.0f / (sampleRate * (deemphasisTime * 1e-6f)));
    else
        alpha = 0;
}

void raptor_deemphasis_processor::process(const float* input, float* output, int count) {
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