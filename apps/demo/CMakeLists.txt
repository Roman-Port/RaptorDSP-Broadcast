add_executable (raptorbroadcast-demo "main.cpp"       )

# Link
target_link_libraries(raptorbroadcast-demo raptordsp-broadcast raptordsp Volk::volk)

# Prepare install target
install(TARGETS raptorbroadcast-demo
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
