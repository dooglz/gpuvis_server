cmake_minimum_required (VERSION 3.11)


project (gpuvis_server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(cli)
add_subdirectory(simulator)
add_subdirectory(webserver)

include(CMakeDependentOption)
option(GPUVIS_build_tests "Build and run unit tests." OFF)
CMAKE_DEPENDENT_OPTION(GPUVIS_build_debg_gtest "Also build debug version of Gtest" OFF "GPUVIS_build_tests" OFF)
if(${GPUVIS_build_tests})
  enable_testing()
  add_subdirectory(tests)
endif()
