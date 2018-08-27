cmake_minimum_required (VERSION 3.11)
project (gpuvis_webserver)
add_executable(gpuvis_webserver server.cpp bridge.cpp)

add_library(mongoose STATIC "lib/mongoose/mongoose.c" "lib/mongoose/mongoose.h")
target_include_directories(mongoose PUBLIC "lib/mongoose")

target_link_libraries(gpuvis_webserver
  PRIVATE mongoose
)

if ( CMAKE_HOST_UNIX )
  target_compile_options(mongoose PUBLIC "-w")
  target_link_libraries(gpuvis_webserver
    PRIVATE dl
  )
endif()
if ( MSVC )
  target_compile_options(mongoose PUBLIC "/W0")
endif()

#As this is a dynamic module lib, can't use target_link_libraries.
target_include_directories(gpuvis_webserver PRIVATE "${CMAKE_BINARY_DIR}/simulator")
add_dependencies(gpuvis_webserver gpuvis_simulator)
