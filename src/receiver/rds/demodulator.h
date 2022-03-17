#pragma once

#include "sync.h"
#include "../../util/mixer.h"
#include "../../util/delay_line.h"
#include "../../util/pll.h"

#include <raptordsp/filter/fir/filter_base.h>

class rds_demodulator {

public:
	rds_demodulator(size_t buffer_size);
	~rds_demodulator();

	void configure(float sample_rate);
	void process(const float* mpx, int count);

	rds_sync sync;

private:
	size_t buffer_size;
	lv_32fc_t* buffer_mpx;
	lv_32fc_t* buffer_mpx_pll;
	float* buffer_baseband;
	float* buffer_sync;

	mixer mpx_mixer;
	raptor_filter_ccf mpx_filter;
	pll mpx_pll;

	raptor_filter_fff sync_filter;
	delay_line_float sync_delay;

	float last_sync;
	float last_sync_slope;
	float last_data;

	bool discarded;
	unsigned char last_differential_bit;

	/// <summary>
	/// Processes the MPX by downconverting, filtering, decimating, and PLLing it into buffer_baseband.
	/// </summary>
	/// <param name="src"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	int downconvert_mpx(const float* src, int count);

	/// <summary>
	/// Creates the sync signal from buffer_baseband into buffer_sync by squaring and filtering the signal. This will create a sine wave clock.
	/// </summary>
	/// <param name="count"></param>
	/// <returns></returns>
	int produce_sync(int count);

	/// <summary>
	/// Processes the sampler which is what actually clocks bits from the data stream using the clock in the sync stream.
	/// </summary>
	/// <param name="sampleData"></param>
	/// <param name="sampleSync"></param>
	void process_sampler_frame(float sampleData, float sampleSync);

	/// <summary>
	/// Clocks a single bit from the sampler and drops every other one, applies differential decoding, and pushes it out
	/// </summary>
	/// <param name="bit"></param>
	void clock_bit(unsigned char bit);

};