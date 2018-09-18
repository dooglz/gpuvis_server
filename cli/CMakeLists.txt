cmake_minimum_required (VERSION 3.11)
project (gpuvis_cli)

add_executable(gpuvis_cli cli.cpp)

target_link_libraries(gpuvis_cli
  PRIVATE gpuvis_simulator
)

#if ( CMAKE_HOST_UNIX )
#  target_link_libraries(gpuvis_webserver
#    PRIVATE dl
#  )
#endif()



