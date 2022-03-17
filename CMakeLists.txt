cmake_minimum_required (VERSION 3.8)
project ("raptordsp-broadcast")

find_package (Volk CONFIG REQUIRED)
find_package (RaptorDSP CONFIG REQUIRED)

add_library (raptordsp-broadcast SHARED "src/util/delay_line.cpp" "src/util/delay_line.h" "src/util/fast_atan2.h" "src/util/fast_atan2.cpp" "src/util/mixer.cpp" "src/util/mixer.h" "src/util/pll.h" "src/util/pll.cpp" "src/receiver/audio/deemphasis_decoder.cpp" "src/receiver/audio/deemphasis_decoder.h" "src/receiver/audio/stereo_processor.cpp" "src/receiver/audio/stereo_processor.h" "src/receiver/rds/sync.cpp" "src/receiver/rds/sync.h" "src/receiver/rds/demodulator.cpp" "src/receiver/rds/demodulator.h" "src/receiver/fm_receiver.h" "src/receiver/fm_receiver.cpp"  "src/receiver/primitive/fm_demod.cpp" "src/receiver/primitive/fm_demod.h" "src/util/volk_buffer.h" "src/util/volk_buffer.cpp" "include/raptorbroadcast/fm_receiver.h" "src/receiver/fm_receiver_api.cpp" "include/raptorbroadcast/common.h")

target_include_directories(raptordsp-broadcast PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(raptordsp-broadcast raptordsp)

# Set output (I think there's a nicer way to do this?)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Prepare install target
install(TARGETS raptordsp-broadcast EXPORT raptordsp-broadcast-targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
export(PACKAGE raptordsp-broadcast)
configure_file(cmake/raptordsp-broadcastConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/raptordsp-broadcast/raptordsp-broadcastConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/raptordsp-broadcast)
install(EXPORT raptordsp-broadcast-targets
  FILE
    raptordsp-broadcast-targets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/raptordsp-broadcastConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/raptordsp-broadcast/raptordsp-broadcastConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

# Add benchmarking
add_subdirectory (apps)