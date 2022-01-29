#include <stdio.h>
#include <stdlib.h>
#include <raptordsp/broadcast/broadcast_fm_demod.h>

#define BUFFER_SIZE 65536
#define ALLOCATE_BUFFER(output, type) type* output = (type*)malloc(sizeof(type) * BUFFER_SIZE); if (output == 0) { printf("Failed to allocate buffers.\n"); return 1; }

float convert_to_db(float amplitude) {
	return 20.0f * log10(amplitude);
}

int main(int argc, char* argv[]) {
	//Get args
	if (argc != 4) {
		printf("RaptorDSP-Broadcast Stereo Benchmark\nUsage: %s [IQ File (complex float32 raw)] [IQ File Sample Rate] [Output Filename (stereo float32 raw)]\n", argv[0]);
		return 1;
	}
	char* inputFilename = argv[1];
	char* outputFilename = argv[3];
	int inputSampleRate = atoi(argv[2]);

	//Open input file
	FILE* input = fopen(inputFilename, "rb");
	if (input == 0) {
		printf("Failed to open input file.\n");
		return 1;
	}

	//Open output file
	FILE* output = fopen(outputFilename, "wb");
	if (output == 0) {
		printf("Failed to open output file.\n");
		return 1;
	}

	//Create the demodulator
	raptor_broadcast_fm_demod demodulator(BUFFER_SIZE * 2, 0);

	//Allocate buffers
	ALLOCATE_BUFFER(buffer_iq, raptor_complex);
	ALLOCATE_BUFFER(buffer_audio_l, float);
	ALLOCATE_BUFFER(buffer_audio_r, float);

	//Configure
	float audioSampleRate = demodulator.configure(inputSampleRate);
	printf("=== BEGINNING BENCHMARK ===\n");
	printf("Input Sample Rate : %i\n", inputSampleRate);
	printf("Audio Sample Rate : %f\n", audioSampleRate);

	//Begin benchmark
	int cycle = 0;
	float maxL = 0;
	float maxR = 0;
	size_t read;
	do {
		//Read from file
		read = fread(buffer_iq, sizeof(raptor_complex), BUFFER_SIZE, input);

		//Process
		int audioRead = demodulator.process(buffer_iq, read, buffer_audio_l, buffer_audio_r);

		//Print any warnings
		if (!demodulator.is_stereo_detected())
			printf("WARN: No stereo detected!\n");

		//Perform benchmark on all but the first cycle
		if (cycle++ > 0) {
			for (int i = 0; i < audioRead; i++) {
				maxL = std::max(maxL, std::abs(buffer_audio_l[i]));
				maxR = std::max(maxR, std::abs(buffer_audio_r[i]));
			}
		}

		//Write to file
		float outputBuffer[2];
		for (int i = 0; i < audioRead; i++) {
			outputBuffer[0] = buffer_audio_l[i];
			outputBuffer[1] = buffer_audio_r[i];
			if (fwrite(outputBuffer, sizeof(float), 2, output) != 2) {
				printf("Failed to write to output file!\n");
			}
		}

		//Log
		printf("WORKING... (MaxL=%f dB; MaxR=%f dB)\n", convert_to_db(maxL), convert_to_db(maxR));
	} while (read != 0);

	//Print results
	printf("=== BENCHMARK COMPLETED ===\n");
	printf("Max L :      %f dB\n", convert_to_db(maxL));
	printf("Max R :      %f dB\n", convert_to_db(maxR));
	printf("Difference : %f dB\n", convert_to_db(std::abs(maxL - maxR)));

	//Free buffers
	free(buffer_iq);
	free(buffer_audio_l);
	free(buffer_audio_r);

	//Close files
	fclose(input);
	fclose(output);

	return 0;
}