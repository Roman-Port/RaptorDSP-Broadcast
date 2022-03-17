#include "demodulator.h"
#include "../../defines.h"
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>
#include <cassert>
#include <volk/volk.h>

#define MALLOC_BUFFER(type) (type*)volk_malloc(sizeof(type) * buffer_size, volk_get_alignment())

rds_demodulator::rds_demodulator(size_t buffer_size) :
	buffer_size(buffer_size),
	sync(),
	mpx_mixer(RDS_SUBCARRIER_FREQ),
	mpx_filter(),
	mpx_pll(M_PI / 200, 0, 0),
	sync_filter(),
	sync_delay(),
	last_sync(0),
	last_sync_slope(0),
	last_data(0),
	discarded(false),
	last_differential_bit(0)
{
	//Create buffers
	buffer_mpx = MALLOC_BUFFER(lv_32fc_t);
	buffer_mpx_pll = MALLOC_BUFFER(lv_32fc_t);
	buffer_baseband = MALLOC_BUFFER(float);
	buffer_sync = MALLOC_BUFFER(float);
}

rds_demodulator::~rds_demodulator() {
	//Free buffers
	volk_free(buffer_mpx);
	volk_free(buffer_mpx_pll);
	volk_free(buffer_baseband);
	volk_free(buffer_sync);
}

void rds_demodulator::configure(float sample_rate) {
	//Configure downconverter
	mpx_mixer.set_sample_rate(sample_rate);

	//Create the MPX decimation filter
	raptor_filter_builder_lowpass mpxFilterBuilder(sample_rate, 2500);
	mpxFilterBuilder.automatic_tap_count(200);
	mpx_filter.configure(&mpxFilterBuilder, mpxFilterBuilder.calculate_decimation(&sample_rate));

	//Set up PLL
	mpx_pll.d_max_freq = 2 * M_PI * RDS_PLL_BW_HZ / sample_rate;
	mpx_pll.d_min_freq = 2 * M_PI * -RDS_PLL_BW_HZ / sample_rate;

	//Set up sync buffer
	raptor_filter_builder_bandpass syncFilterBuilder(sample_rate, (RDS_SYMBOL_RATE * 2) - 150, (RDS_SYMBOL_RATE * 2) + 150);
	syncFilterBuilder.automatic_tap_count(100);
	sync_filter.configure(&syncFilterBuilder, 1);

	//Configure filter delay to half(?) of the taps used in the sync filter
	sync_delay.configure(syncFilterBuilder.get_ntaps() / 2, 0);
}

void rds_demodulator::process(const float* mpx, int count) {
	//Sanity check
	assert(count <= buffer_size);
	assert(count >= 0);
	assert(mpx != 0);

	//First, downconvert and decimate
	count = downconvert_mpx(mpx, count);

	//Create the sync signal
	int syncCount = produce_sync(count);
	assert(syncCount == count);

	//Delay the data stream
	sync_delay.process(buffer_baseband, buffer_baseband, count);

	//Run sampler using the clock and data streams
	for (int i = 0; i < count; i++)
		process_sampler_frame(buffer_baseband[i], buffer_sync[i]);
}

int rds_demodulator::downconvert_mpx(const float* src, int count) {
	//Downconvert from the 57k subcarrier to baseband
	mpx_mixer.process(src, buffer_mpx, count);

	//Filter and decimate
	count = mpx_filter.process(buffer_mpx, buffer_mpx, count);

	//Process PLL
	mpx_pll.process(buffer_mpx, buffer_mpx_pll, count);

	//Multiply the input by the conjugate of the PLL output
	volk_32fc_x2_multiply_conjugate_32fc(buffer_mpx, buffer_mpx, buffer_mpx_pll, count);

	//Extract imaginary part
	for (int i = 0; i < count; i++)
		buffer_baseband[i] = buffer_mpx[i].imag();

	return count;
}

int rds_demodulator::produce_sync(int count) {
	//Multiply the signal by itself to produce the squared version
	volk_32f_x2_multiply_32f(buffer_sync, buffer_baseband, buffer_baseband, count);

	//Do band-pass filtering
	return sync_filter.process(buffer_sync, buffer_sync, count);
}

void rds_demodulator::process_sampler_frame(float sampleData, float sampleSync) {
	//Get the difference between the last sync value and the current one
	float syncDiff = sampleSync - last_sync;

	//True at the peak of every positive cycle of the sync sine wave at 2375 Hz
	if (syncDiff < 0 && last_sync_slope * syncDiff < 0)
		clock_bit(last_data > 0 ? 1 : 0);

	//Update state
	last_sync = sampleSync;
	last_data = sampleData;
	last_sync_slope = syncDiff;
}

void rds_demodulator::clock_bit(unsigned char bit) {
	//Discard every other bit
	if (discarded)
	{
		//Apply differential decoding and push out bit
		sync.push((bit ^ last_differential_bit) & 1);

		//Reset state
		last_differential_bit = bit;
		discarded = false;
	}
	else
	{
		discarded = true;
	}
}