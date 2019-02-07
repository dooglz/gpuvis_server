cmake_minimum_required (VERSION 3.11)
project (gpuvis_simulator)
file(GLOB SOURCE_FILES src/*.cpp src/*.h src/gpu/*.h src/gpu/*.cpp src/decoders/*.h src/decoders/*.cpp)
message(${SOURCE_FILES})
add_library(gpuvis_simulator SHARED ${SOURCE_FILES})
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(gpuvis_simulator)
target_include_directories(gpuvis_simulator PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_library(gpuvis_simulator_isa_fiji SHARED src/isa/fiji.h src/isa/isa.h src/isa/isa.cpp)
target_include_directories(gpuvis_simulator_isa_fiji PUBLIC src/isa/ ${CMAKE_CURRENT_BINARY_DIR})
GENERATE_EXPORT_HEADER(gpuvis_simulator_isa_fiji)

add_library(json INTERFACE)
#target_sources(json INTERFACE "lib/json.hpp")
target_include_directories(json INTERFACE "lib")
target_link_libraries(gpuvis_simulator PRIVATE json gpuvis_simulator_isa_fiji)



add_custom_command(TARGET gpuvis_simulator
    PRE_BUILD
    COMMAND git log -1 "--format=format:#define GIT_REVISION %h%n" HEAD > "${CMAKE_CURRENT_BINARY_DIR}/git_revision.h"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} VERBATIM
)