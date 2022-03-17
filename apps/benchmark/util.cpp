#include "util.h"
#include <cmath>

float to_db(float value) {
	return std::log10f(value) * 20.0f;
}

float from_db(float value) {
	return std::pow(10.0f, (value / 20.0f));
}