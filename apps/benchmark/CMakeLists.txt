add_executable (raptorbroadcast-benchmark "main.cpp"        "test.h" "test.cpp" "util.h" "util.cpp" "wav.h" "wav.cpp")

# Link
target_link_libraries(raptorbroadcast-benchmark raptordsp-broadcast raptordsp Volk::volk)

# Prepare install target
install(TARGETS raptorbroadcast-benchmark
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
