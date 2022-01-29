#pragma once

#include <stdint.h>

struct rds_frame_t {

	uint64_t payload;
	unsigned char offsets[4];

};

class raptor_rds_sync {

public:
	raptor_rds_sync();
	bool process(unsigned char input, rds_frame_t* result);
	void reset();

	enum { NO_SYNC, SYNC } d_state;

private:
	unsigned long  bit_counter;
	unsigned long  lastseen_offset_counter, reg;
	unsigned int   block_bit_counter;
	unsigned int   wrong_blocks_counter;
	unsigned int   blocks_counter;
	unsigned int   group_good_blocks_counter;
	unsigned int   group[4];
	unsigned char  offset_chars[4];  // [ABCcDEx] (x=error)
	bool           log;
	bool           debug;
	bool           presync;
	bool           good_block;
	bool           group_assembly_started;
	unsigned char  lastseen_offset;
	unsigned char  block_number;

	void enter_sync(unsigned int sync_block_number);
	void exit_sync();

};