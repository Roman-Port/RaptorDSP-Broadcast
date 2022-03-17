#pragma once

class deemphasis_decoder {

public:
	deemphasis_decoder(float deemphasis_time);
	void set_sample_rate(float sample_rate);
	void set_deemphasis_time(float deemphasis_time);
	float get_deemphasis_time();
	void process(const float* input, float* output, int count);

private:
	float sample_rate;
	float deemphasis_time;

	float alpha;
	float state;

	void configure();

};