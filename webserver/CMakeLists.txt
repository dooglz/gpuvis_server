#This is just so the JS code shows up in the C++ IDE, CMake doesn't build this or do anything with it
cmake_minimum_required (VERSION 3.11)
project (webserver LANGUAGES NONE)

file(GLOB INDEXJS index.js)
file(GLOB OTHERJS gpuvis.js rga.js)
add_custom_target(webserver SOURCES ${INDEXJS} ${OTHERJS})


IF (WIN32)
  #Create a nice batch script with correct absolute paths
  configure_file(run_windows.bat.in ${CMAKE_CURRENT_BINARY_DIR}/run_windows.bat.cfgd @ONLY)
  file(GENERATE 
    OUTPUT run_windows_$<CONFIG>.bat
    INPUT ${CMAKE_CURRENT_BINARY_DIR}/run_windows.bat.cfgd
  )
else(WIN32)
  configure_file(run.sh.in ${CMAKE_CURRENT_BINARY_DIR}/run.sh.cfgd @ONLY)
  file(GENERATE
        OUTPUT run_$<CONFIG>.sh
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/run.sh.cfgd
        )
endif(WIN32)

add_dependencies(webserver gpuvis_cli)