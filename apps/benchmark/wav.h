#pragma once

#include <stdio.h>
#include <stdint.h>

class wav_writer {

public:
	wav_writer(const char* filename, int sample_rate, int channels);
	~wav_writer();

	void write(const float* samples, int count);

private:
	FILE* file;
	int len;

	void update_int(int offset, int32_t value);

};