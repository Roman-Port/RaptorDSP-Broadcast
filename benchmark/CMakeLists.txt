add_executable (raptordsp-broadcast-benchmark "main.cpp")

# Link
target_link_libraries(raptordsp-broadcast-benchmark raptordsp-broadcast raptordsp)

# Prepare install target
install(TARGETS raptordsp-broadcast-benchmark
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
