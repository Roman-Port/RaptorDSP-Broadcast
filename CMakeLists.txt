cmake_minimum_required (VERSION 3.8)
project ("raptordsp-broadcast")

find_package (Volk CONFIG REQUIRED)
find_package (RaptorDSP CONFIG REQUIRED)

add_library (raptordsp-broadcast SHARED "include/raptordsp/broadcast/stereo_processor.h" "src/stereo_processor.cpp"   "include/raptordsp/broadcast/rds_demod.h" "src/rds_sync.cpp" "src/rds_demod.cpp" "include/raptordsp/broadcast/rds_sync.h" "src/deemphasis_processor.cpp" "src/deemphasis_processor.cpp" "src/broadcast_fm_demod.cpp" "include/raptordsp/broadcast/broadcast_fm_demod.h")

target_include_directories(raptordsp-broadcast PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(raptordsp-broadcast raptordsp)