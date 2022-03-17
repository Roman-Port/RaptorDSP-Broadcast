#pragma once

#include <stdint.h>

struct raptorbroadcast_rds_frame_t {

	uint64_t payload;
	char offsets[4];

};