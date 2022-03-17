#pragma once

#include <volk/volk_typedefs.h>

template<typename T>
class volk_buffer {

public:
	volk_buffer(size_t element_count);
	~volk_buffer();

	size_t count;
	T* buffer;

};

typedef volk_buffer<float> volk_buffer_float;
typedef volk_buffer<lv_32fc_t> volk_buffer_complex;