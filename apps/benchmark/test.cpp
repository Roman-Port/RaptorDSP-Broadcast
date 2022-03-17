#include "test.h"
#include "util.h"
#include "wav.h"
#include <cmath>
#include <cassert>
#include <cstring>
#include <random>
#include <volk/volk.h>
#include <raptorbroadcast/fm_receiver.h>
#include <raptordsp/filter/fir/filter_base.h>
#include <raptordsp/filter/builder/builder_lowpass.h>

template<typename T>
T* create_buffer(size_t count) {
	//Allocate
	T* buffer = (T*)volk_malloc(sizeof(T) * count, volk_get_alignment());
	assert(buffer != 0);

	//Clear
	memset(buffer, 0, sizeof(T) * count);

	return buffer;
}

struct test_ctx_t {

	test_results_t* results;
	FILE* notes;
	float current_time;

};

void write_note(test_ctx_t* ctx, float start_time, float end_time, const char* text) {
	char buffer[256];
	sprintf(buffer, "%f\t%f\t%s\n", start_time, end_time, text);
	fwrite(buffer, 1, strlen(buffer), ctx->notes);
}

void write_note(test_ctx_t* ctx, float time, const char* text) {
	write_note(ctx, time, time, text);
}

void rds_frame_cb(void* ctxRaw, raptorbroadcast_rds_frame_t frame) {
	test_ctx_t* ctx = (test_ctx_t*)ctxRaw;
	ctx->results->rds_total_frames++;
	write_note(ctx, ctx->current_time, "RDS FRAME");
}

void rds_status_cb(void* ctxRaw, int sync) {
	test_ctx_t* ctx = (test_ctx_t*)ctxRaw;
	if (!sync) {
		write_note(ctx, ctx->current_time, "RDS LOST");
		ctx->results->rds_sync_loss_incidents++;
	}
	else {
		write_note(ctx, ctx->current_time, "RDS SYNC");
	}
}

void update_average(float* result, bool* isFirstUpdate, float value) {
	if ((*isFirstUpdate))
		(*result) = value;
	else
		(*result) = ((*result) + value) / 2.0f;
	(*isFirstUpdate) = false;
}

void run_test(FILE* file, test_params_t params, test_results_t* results) {
	//Allocate buffers
	lv_32fc_t* bufferIq = create_buffer<lv_32fc_t>(params.buffer_size);
	float* bufferAudioL = create_buffer<float>(params.buffer_size);
	float* bufferAudioR = create_buffer<float>(params.buffer_size);

	//Rewind to the beginning of the file
	fseek(file, 0, SEEK_SET);

	//Create context object
	test_ctx_t ctx;
	ctx.results = results;
	ctx.notes = 0;
	ctx.current_time = 0;

	//Create IF filter
	float ifSampleRate;
	raptor_filter_ccf ifFilter;
	raptor_filter_builder_lowpass ifFilterBuilder(750000, 250000 * 0.5);
	ifFilterBuilder.automatic_tap_count(250000 * 0.1);
	ifFilter.configure(&ifFilterBuilder, ifFilterBuilder.calculate_decimation(&ifSampleRate));

	//Initialize demodulator
	raptorbroadcast_fm_receiver_t* demod = raptorbroadcast_fm_receiver_create(params.buffer_size);
	raptorbroadcast_fm_receiver_set_stereo_mode(demod, RAPTORBROADCAST_STEREO_MODE_FORCE);
	raptorbroadcast_fm_receiver_bind_rds_frame(demod, rds_frame_cb, &ctx);
	raptorbroadcast_fm_receiver_bind_rds_status(demod, rds_status_cb, &ctx);
	float audioSampleRate = raptorbroadcast_fm_receiver_configure(demod, ifSampleRate);

	//Reset results
	results->sample_rate_if = ifSampleRate;
	results->sample_rate_audio = audioSampleRate;
	results->rds_total_frames = 0;
	results->rds_sync_loss_incidents = 0;
	results->avg_pll_error = 0;
	results->avg_level_l_db = 0;
	results->avg_level_r_db = 0;
	results->avg_lr_ratio = 0;
	results->avg_lr_mixing = 0;

	//Prepare random generator
	std::default_random_engine generator(98288913); // Just a random number to use as a constant seed
	std::normal_distribution<float> distribution(-1.0f, 1.0f);

	//Create filenames
	char outputFilenameAudio[256];
	sprintf(outputFilenameAudio, "%s_audio.wav", params.output_filename_prefix);
	char outputFilenameIq[256];
	sprintf(outputFilenameIq, "%s_iq.wav", params.output_filename_prefix);
	char outputFilenameNotes[256];
	sprintf(outputFilenameNotes, "%s_labels.txt", params.output_filename_prefix);

	//Open output files
	wav_writer outputAudio(outputFilenameAudio, (int)audioSampleRate, 2);
	wav_writer outputIq(outputFilenameIq, params.input_sample_rate, 2);
	ctx.notes = fopen(outputFilenameNotes, "w");

	//Run test loop
	float audioTemp[2];
	bool firstUpdatePll = true;
	bool firstUpdateAvgL = true;
	bool firstUpdateAvgR = true;
	bool firstUpdateAvgLRRatio = true;
	bool firstUpdateAvgLRMixing = true;
	int remianingSamplesSkip = params.skip_samples;
	int remianingSamplesTotal = params.duration_samples + params.skip_samples;
	int progress = 0;
	float noiseAmplitude;
	int read;
	int audioRead;
	do {
		//Determine how much can be read
		read = std::min(params.buffer_size, remianingSamplesTotal);

		//Read from disk
		read = fread(bufferIq, sizeof(lv_32fc_t), read, file);

		//Add noise
		for (int i = 0; i < read; i++) {
			//Calculate progress in range [0, 1]
			noiseAmplitude = progress++ / (float)params.duration_samples;

			//Calculate dB of noise
			noiseAmplitude = (params.noise_floor_start_db * (1 - noiseAmplitude)) + (params.noise_floor_end_db * noiseAmplitude);

			//Convert dB to raw amplitude
			noiseAmplitude = from_db(noiseAmplitude);

			//Apply
			bufferIq[i] += lv_32fc_t(distribution(generator) * noiseAmplitude, distribution(generator) * noiseAmplitude);
		}

		//Write to output
		outputIq.write((float*)bufferIq, read * 2);

		//Advance
		remianingSamplesSkip -= read;
		remianingSamplesTotal -= read;
		ctx.current_time += (float)read / params.input_sample_rate;

		//Filter
		audioRead = ifFilter.process(bufferIq, bufferIq, read);

		//Process
		audioRead = raptorbroadcast_fm_receiver_process(demod, bufferIq, audioRead, bufferAudioL, bufferAudioR);

		//Interlace and write audio to disk
		for (int i = 0; i < audioRead; i++) {
			audioTemp[0] = bufferAudioL[i];
			audioTemp[1] = bufferAudioR[i];
			outputAudio.write(audioTemp, 2);
		}

		//If we've skipped all the samples we need to, perform test
		float maxL = 0;
		float maxR = 0;
		if (remianingSamplesSkip <= 0) {
			//Set PLL error
			update_average(&results->avg_pll_error, &firstUpdatePll, std::abs(raptorbroadcast_fm_receiver_get_stereo_raw_error(demod)));

			//Set mixing amount
			update_average(&results->avg_lr_mixing, &firstUpdateAvgLRMixing, raptorbroadcast_fm_receiver_get_stereo_mixing(demod));

			//Update everything related with audio output
			for (int i = 0; i < audioRead; i++) {
				//Update level
				maxL = std::max(maxL, bufferAudioL[i]);
				maxR = std::max(maxR, bufferAudioR[i]);

				//Update average ratio
				if (bufferAudioR[i] != 0)
					update_average(&results->avg_lr_ratio, &firstUpdateAvgLRRatio, std::abs(bufferAudioL[i]) / std::abs(bufferAudioR[i]));
			}

			//Update average level
			update_average(&results->avg_level_l_db, &firstUpdateAvgL, maxL);
			update_average(&results->avg_level_r_db, &firstUpdateAvgR, maxR);
		}
	} while (read > 0 && remianingSamplesTotal > 0);

	//Close files
	fclose(ctx.notes);

	//Convert values to dB
	results->avg_level_l_db = to_db(results->avg_level_l_db);
	results->avg_level_r_db = to_db(results->avg_level_r_db);

	//Clean up and free buffers
	volk_free(bufferIq);
	volk_free(bufferAudioL);
	volk_free(bufferAudioR);

	//Release demodulator
	raptorbroadcast_fm_receiver_destroy(demod);
}