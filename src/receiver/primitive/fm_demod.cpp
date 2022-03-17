#include "fm_demod.h"
#include "../../util/fast_atan2.h"
#include "../../defines.h"
#include <volk/volk.h>

fm_demod::fm_demod(float deviation) :
    deviation(deviation),
    sample_rate(0),
    last_sample(0, 0),
    gain(0)
{

}

fm_demod::~fm_demod() {

}

void fm_demod::set_sample_rate(float sample_rate) {
    this->sample_rate = sample_rate;
    configure();
}

void fm_demod::set_deviation(float deviation) {
    this->deviation = deviation;
    configure();
}

float fm_demod::get_deviation() {
    return deviation;
}

void fm_demod::configure() {
    gain = sample_rate / (2 * M_PI * deviation);
}

void fm_demod::process(const lv_32fc_t* input, float* output, int count) {
    //FM demodulate
    lv_32fc_t temp;
    for (int i = 0; i < count; i++) {
        //Apply conjugate
        temp = input[i] * std::conj(last_sample);

        //Estimate angle
        output[i] = fast_atan2f(imag(temp), real(temp));

        //Set state
        last_sample = input[i];
    }

    //Apply gain
    volk_32f_s32f_multiply_32f(output, output, gain, count);
}