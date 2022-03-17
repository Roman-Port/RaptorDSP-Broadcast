#pragma once

#include <raptorbroadcast/common.h>
#include <raptorbroadcast/rds.h>
#include <volk/volk_typedefs.h>

#define RAPTORBROADCAST_STEREO_MODE_AUTO 0 /* Stereo is automatically enabled if the pilot is detected. As the signal strength decreases, stereo is slowly mixed with mono to produce a smooth transition. */
#define RAPTORBROADCAST_STEREO_MODE_BINARY 1 /* Like auto mode, but instead of smoothly transitioning between stereo and mono, stereo is either on or off. */
#define RAPTORBROADCAST_STEREO_MODE_OFF 2 /* Completely disables stereo, producing a mono signal even if a stereo pilot is detected. */
#define RAPTORBROADCAST_STEREO_MODE_FORCE 3 /* Forces stereo demoduation, even if no pilot is detected. NOT RECOMMENDED, as the output is likely to be junk if stereo isn't detected. */

typedef void (*raptorbroadcast_fm_receiver_rds_frame_cb)(void* ctx, raptorbroadcast_rds_frame_t frame);
typedef void (*raptorbroadcast_fm_receiver_rds_status_cb)(void* ctx, int has_sync);

typedef struct { void* placeholder; } raptorbroadcast_fm_receiver_t;

EXPORT_API raptorbroadcast_fm_receiver_t* raptorbroadcast_fm_receiver_create(size_t buffer_size);
EXPORT_API void raptorbroadcast_fm_receiver_destroy(raptorbroadcast_fm_receiver_t* ctx);

EXPORT_API float raptorbroadcast_fm_receiver_configure(raptorbroadcast_fm_receiver_t* ctx, float sample_rate);
EXPORT_API int raptorbroadcast_fm_receiver_process(raptorbroadcast_fm_receiver_t* ctx, const lv_32fc_t* input_iq, int count, float* buffer_audio_l, float* buffer_audio_r);
EXPORT_API void raptorbroadcast_fm_receiver_setup(raptorbroadcast_fm_receiver_t* ctx, int region);

EXPORT_API void raptorbroadcast_fm_receiver_bind_rds_frame(raptorbroadcast_fm_receiver_t* ctx, raptorbroadcast_fm_receiver_rds_frame_cb callback, void* user_ctx);
EXPORT_API void raptorbroadcast_fm_receiver_bind_rds_status(raptorbroadcast_fm_receiver_t* ctx, raptorbroadcast_fm_receiver_rds_status_cb callback, void* user_ctx);

EXPORT_API float raptorbroadcast_fm_receiver_get_fm_deviation(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API void raptorbroadcast_fm_receiver_set_fm_deviation(raptorbroadcast_fm_receiver_t* ctx, float fm_deviation);

EXPORT_API float raptorbroadcast_fm_receiver_get_deemphasis_time(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API void raptorbroadcast_fm_receiver_set_deemphasis_time(raptorbroadcast_fm_receiver_t* ctx, float deemphasis_time);

EXPORT_API int raptorbroadcast_fm_receiver_get_stereo_mode(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API void raptorbroadcast_fm_receiver_set_stereo_mode(raptorbroadcast_fm_receiver_t* ctx, int mode);

EXPORT_API float raptorbroadcast_fm_receiver_get_stereo_mixing_min(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API void raptorbroadcast_fm_receiver_set_stereo_mixing_min(raptorbroadcast_fm_receiver_t* ctx, float min);

EXPORT_API float raptorbroadcast_fm_receiver_get_stereo_mixing_max(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API void raptorbroadcast_fm_receiver_set_stereo_mixing_max(raptorbroadcast_fm_receiver_t* ctx, float max);

EXPORT_API float raptorbroadcast_fm_receiver_get_stereo_mixing(raptorbroadcast_fm_receiver_t* ctx);
EXPORT_API float raptorbroadcast_fm_receiver_get_stereo_raw_error(raptorbroadcast_fm_receiver_t* ctx);