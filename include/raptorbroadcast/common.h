#pragma once

#if defined(_WIN32)
#define EXPORT_API extern "C" __declspec( dllexport )
#elif defined(__GNUC__)
#define EXPORT_API extern "C" __attribute__((visibility("default")))
#else
#define EXPORT_API extern "C"
#endif

#define RAPTORBROADCAST_REGION_DEFAULT 0
#define RAPTORBROADCAST_REGION_US 1
#define RAPTORBROADCAST_REGION_EU 2