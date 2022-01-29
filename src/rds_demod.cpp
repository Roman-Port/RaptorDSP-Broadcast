#include <raptordsp/broadcast/rds_demod.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>
#include <raptordsp/filter/builder/builder_rootraised.h>
#include <volk/volk.h>
#include <cassert>

#define RDS_SUBCARRIER_FREQ 57000
#define RDS_BW 2500

raptor_rds_demod::raptor_rds_demod(size_t buffer_size) : buffer_size(buffer_size), output_waiting(0), output_read(0) {
	buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * buffer_size);
	assert(buffer != 0);
}

void raptor_rds_demod::configure(float sampleRate) {
	//Configure rotator
	float freq = 2.0f * M_PI * RDS_SUBCARRIER_FREQ / sampleRate;
	ro_rotation = raptor_complex(std::cos(freq), std::sin(freq));
	ro_phase = raptor_complex(1, 0);

	//Configure coarse decimation filter
	raptor_filter_builder_bandpass builderCoarse(sampleRate, -6000, 6000);
	builderCoarse.automatic_tap_count(3000, 80);
	ro_filter_coarse.configure(&builderCoarse, builderCoarse.calculate_decimation(&sampleRate));

	//Configure fine decimation filter
	raptor_filter_builder_bandpass builderFine(sampleRate, -RDS_BW, RDS_BW);
	builderFine.automatic_tap_count(200, 80);
	ro_filter_fine.configure(&builderFine, builderFine.calculate_decimation(&sampleRate));

	//Configure costas loop
	ro_loop.configure(M_PI / 100, 2, false);

	//Configure the matched filter
	raptor_filter_builder_root_raised_cosine builder(sampleRate, RDS_SYMBOL_RATE * 2, 0.35f);
	builder.set_ntaps(361);
	matched_filter.configure(&builder, 1);

	//Configure the clock recovery
	clock_recovery_mm.configure(
		(sampleRate / (RDS_SYMBOL_RATE * 2)),
		0.8f,
		0.5f,
		0.150f,
		0.001f
	);

	//Reset differential decoding
	dd_discard = false;
	dd_previous = 0;
}

void raptor_rds_demod::input(float* in, int count) {
	//Make sure the output is empty
	assert(output_waiting == 0);

	//Copy real input in
	assert(count <= buffer_size);
	for (int i = 0; i < count; i++)
		buffer[i] = raptor_complex(in[i], 0);

	//Rotate all
	volk_32fc_s32fc_x2_rotator_32fc(buffer, buffer, ro_rotation, &ro_phase, count);

	//Apply coarse
	count = ro_filter_coarse.process(buffer, buffer, count);

	//Filter fine
	count = ro_filter_fine.process(buffer, buffer, count);

	//Process AGC
	agc.process(buffer, buffer, count);

	//Process costas loop
	ro_loop.process(buffer, buffer, count);

	//Apply the matched filter
	count = matched_filter.process(buffer, buffer, count);

	//Process clock recovery
	count = clock_recovery_mm.process(buffer, count, buffer, buffer_size);

	//Set
	output_waiting = count;
	output_read = 0;
}

bool raptor_rds_demod::output(unsigned char* result) {
	//Discard every other sample
	if (output_waiting > 0 && dd_discard) {
		output_read++;
		output_waiting--;
		dd_discard = false;
	}

	//If there are still samples to read, apply
	if (output_waiting > 0) {
		//Get the sample to read
		float sample = buffer[output_read++].real();
		output_waiting--;

		//Apply differential decoding...Do binary slicing
		unsigned char value = sample > 0 ? 1 : 0;

		//Do differential decoding on output
		(*result) = (value ^ dd_previous) & 1;
		dd_previous = value;

		//Update state
		dd_discard = true;

		return true;
	}
	return false;
}