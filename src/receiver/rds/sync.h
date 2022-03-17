#pragma once

#include <raptorbroadcast/rds.h>
#include <raptorbroadcast/fm_receiver.h>

class rds_sync {

public:
	rds_sync();
	~rds_sync();

	void push(unsigned char bit);
	void reset();

	bool has_sync;

	raptorbroadcast_fm_receiver_rds_frame_cb cb_frame;
	void* cb_frame_ctx;

	raptorbroadcast_fm_receiver_rds_status_cb cb_status;
	void* cb_status_ctx;

private:
	unsigned long  bit_counter;
	unsigned long  lastseen_offset_counter, reg;
	unsigned int   block_bit_counter;
	unsigned int   wrong_blocks_counter;
	unsigned int   blocks_counter;
	unsigned int   group_good_blocks_counter;
	unsigned int   group[4];
	unsigned char  offset_chars[4];  // [ABCcDEx] (x=error)
	bool           presync;
	bool           good_block;
	bool           group_assembly_started;
	unsigned char  lastseen_offset;
	unsigned char  block_number;

	void enter_sync(unsigned int sync_block_number);
	void exit_sync();
	void push_frame();
	void notify_sync_changed();

};