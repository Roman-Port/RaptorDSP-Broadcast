#include "wav.h"
#include <cstring>

const unsigned char header[] = {
	0x52,
	0x49,
	0x46,
	0x46,
	0x00,
	0x00,
	0x00,
	0x00,
	0x57,
	0x41,
	0x56,
	0x45,
	0x66,
	0x6d,
	0x74,
	0x20,
	0x10,
	0x00,
	0x00,
	0x00,
	0x03,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x80,
	0x8d,
	0x5b,
	0x00,
	0x08,
	0x00,
	0x20,
	0x00,
	0x64,
	0x61,
	0x74,
	0x61,
	0x00,
	0x00,
	0x00,
	0x00
};

void write_int(unsigned char* buf, int offset, int32_t value) {
	*((int32_t*)(buf + offset)) = value;
}

void write_short(unsigned char* buf, int offset, int16_t value) {
	*((int16_t*)(buf + offset)) = value;
}

wav_writer::wav_writer(const char* filename, int sample_rate, int channels) {
	//Open the file and reset
	file = fopen(filename, "wb");
	len = 0;

	//Create file header
	unsigned char updatedHeader[44];
	memcpy(updatedHeader, header, sizeof(header));
	write_short(updatedHeader, 22, channels);
	write_int(updatedHeader, 24, sample_rate);

	//Write file header
	fwrite(updatedHeader, 1, sizeof(header), file);
}

wav_writer::~wav_writer() {
	//Go back and update lengths
	update_int(40, len + 4);
	update_int(4, len + 40);

	//Close file
	fclose(file);
	file = 0;
}

void wav_writer::update_int(int offset, int32_t value) {
	//Rewind
	fseek(file, offset, SEEK_SET);

	//Write
	fwrite(&value, sizeof(int32_t), 1, file);
}

void wav_writer::write(const float* samples, int count) {
	fwrite(samples, sizeof(float), count, file);
	len += count * sizeof(float);
}