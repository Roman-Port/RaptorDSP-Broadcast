#pragma once

#include <raptordsp/filter/fir/filter_complex.h>
#include <raptordsp/filter/fir/filter_real.h>
#include <raptordsp/analog/costas_loop.h>
#include <raptordsp/analog/agc.h>
#include <raptordsp/digital/clock_recovery_mm_complex.h>

#define RDS_SYMBOL_RATE 1187.5f

class raptor_rds_demod {

public:
    raptor_rds_demod(size_t bufferSize);
    void configure(float sampleRate);
    void input(float* mpx, int count);
    bool output(unsigned char* result);

private:
    raptor_complex* buffer;
    size_t buffer_size;
    raptor_complex ro_phase;
    raptor_complex ro_rotation;
    raptor_filter_ccc ro_filter_coarse;
    raptor_filter_ccc ro_filter_fine;
    raptor_agc_complex agc;
    raptor_costas_loop ro_loop;
    raptor_filter_ccf matched_filter;
    raptor_clock_recovery_mm_complex clock_recovery_mm;
    unsigned char dd_previous;
    unsigned char dd_discard;

    int output_waiting;
    int output_read;

};