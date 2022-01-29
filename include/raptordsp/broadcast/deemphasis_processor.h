#pragma once

class raptor_deemphasis_processor {

public:
	raptor_deemphasis_processor();
	void configure(float sampleRate, float deemphasisTime);
	void process(const float* input, float* output, int count);

private:
	float alpha;
	float state;

};