/*#include <cassert>
#include <stdio.h>
#include <raptordsp/filter/fir/filter_base.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include "../../src/receiver/fm_receiver.h"

#define BUFFER_SIZE 16384

void rds_frame_cb(void* ctx, raptorbroadcast_rds_frame_t frame) {
	//Encode bits into a string
	char logBuffer[65];
	for (int i = 0; i < 64; i++)
		logBuffer[i] = ((frame.payload >> i) & 1) ? 'X' : '-';
	logBuffer[64] = '\0';

	//Send
	printf("RDS: %c%c%c%c : [%s]\n", frame.offsets[0], frame.offsets[1], frame.offsets[2], frame.offsets[3], logBuffer);
}

void rds_status_cb(void* ctx, int sync) {
	if (sync)
		printf("RDS: SYNC OBTAINED\n");
	else
		printf("RDS: SYNC LOST\n");
}

int main() {
	//Open files
	FILE* inp = fopen("C:\\Users\\Roman\\Desktop\\92500000Hz KQRS-FM - Tom Petty & The Heartbreakers - Mary Jane's Last Dance.raw", "rb");
	FILE* out = fopen("C:\\Users\\Roman\\Desktop\\test_rds.bin", "wb");

	//Create IF filter
	float ifSampleRate;
	raptor_filter_ccf ifFilter;
	raptor_filter_builder_lowpass ifFilterBuilder(750000, 250000 * 0.5);
	ifFilterBuilder.automatic_tap_count(250000 * 0.1);
	ifFilter.configure(&ifFilterBuilder, ifFilterBuilder.calculate_decimation(&ifSampleRate));

	//Initialize demodulator
	fm_receiver demod(BUFFER_SIZE);
	demod.rds.sync.cb_frame = rds_frame_cb;
	demod.rds.sync.cb_status = rds_status_cb;
	float audioSampleRate = demod.configure(ifSampleRate);
	printf("IF SAMPLE RATE: %f\n", ifSampleRate);
	printf("AUDIO SAMPLE RATE: %f\n", audioSampleRate);

	//Process
	lv_32fc_t buffer_iq[BUFFER_SIZE];
	float buffer_audio_l[BUFFER_SIZE];
	float buffer_audio_r[BUFFER_SIZE];
	float temp[2];
	int read;
	int audioRead;
	do {
		//Read
		read = fread(buffer_iq, sizeof(lv_32fc_t), BUFFER_SIZE, inp);

		//Filter
		audioRead = ifFilter.process(buffer_iq, buffer_iq, read);

		//Process
		audioRead = demod.process(buffer_iq, audioRead, buffer_audio_l, buffer_audio_r);

		//Write
		for (int i = 0; i < audioRead; i++) {
			//Interleave
			temp[0] = buffer_audio_l[i];
			temp[1] = buffer_audio_r[i];

			//Write
			fwrite(temp, sizeof(temp), 1, out);
		}
	} while (read > 0);

	//Done
	printf("Done!\n");
	fclose(inp);
	fclose(out);
	return 0;
}*/

int main() {
	return 1;
}