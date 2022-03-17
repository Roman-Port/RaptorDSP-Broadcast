#pragma once

#include <volk/volk_typedefs.h>

template<typename T>
class delay_line {

public:
	delay_line();
	delay_line(delay_line const& src);
	~delay_line();

	void configure(int delay, T defaultValue);
	void process(const T* input, T* output, int count);

private:
	T* buffer;
	int len;
	int pos;

};

typedef delay_line<float> delay_line_float;
typedef delay_line<lv_32fc_t> delay_line_complex;