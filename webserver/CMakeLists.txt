cmake_minimum_required (VERSION 3.11)
project (gpuvis_webserver)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)
add_executable(gpuvis_webserver ${SOURCE_FILES})

add_library(mongoose STATIC "lib/mongoose/mongoose.c" "lib/mongoose/mongoose.h")
target_include_directories(mongoose PUBLIC "lib/mongoose")

target_link_libraries(gpuvis_webserver
  PRIVATE mongoose
  PRIVATE gpuvis_simulator
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

add_custom_target(copy_resources ALL 
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/static" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/static
)

add_dependencies(gpuvis_webserver copy_resources)


