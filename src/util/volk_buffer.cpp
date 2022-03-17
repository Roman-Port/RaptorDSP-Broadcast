#include "volk_buffer.h"
#include <volk/volk.h>
#include <cassert>

template<typename T>
volk_buffer<T>::volk_buffer(size_t element_count) :
	count(element_count),
	buffer((T*)volk_malloc(sizeof(T) * element_count, volk_get_alignment()))
{
	assert(buffer != 0);
}

template<typename T>
volk_buffer<T>::~volk_buffer() {
	if (buffer != 0) {
		volk_free(buffer);
		buffer = 0;
	}
}

template class volk_buffer<float>;
template class volk_buffer<lv_32fc_t>;