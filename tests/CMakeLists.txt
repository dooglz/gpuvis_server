enable_testing()
find_package(GTest)
include(GoogleTest)
#set(GTEST_FOUND 0)
#set(gtest_built 0)

set (Gtest GTest::Main GTest::GTest)
project(${CMAKE_PROJECT_NAME}_tests)
if(NOT ${GTEST_FOUND})
  # Clone Google Test
  message("No google test!")
  set(G_source ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest)
  if(NOT EXISTS ${G_source})
   message(STATUS "Cloning Gtest")
    find_package(Git REQUIRED)
    execute_process(
    COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/google/googletest ${G_source}
    )
  endif()
  if(NOT gtest_built)
    set(G_build ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_build)
    set(G_install ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_install)
    
    #Configure Release Gtest
    file(MAKE_DIRECTORY ${G_build})
    message(STATUS "Configuring Gtest")
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${G_source}  -DCMAKE_INSTALL_PREFIX=${G_install} -Dgtest_force_shared_crt=ON
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${G_build} 
    )
    if(result)
      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    #build Release Gtest
    message(STATUS "Building Gtest")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release --target install
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${G_build}
    )
    if(result)
      message(FATAL_ERROR "Build step for googletest failed: ${result}")
    else()
      set(gtest_built TRUE)
    endif()
    message(STATUS  "Built Gtest!")
    #Configure/Build Debug Gtest
    if(${GPUVIS_build_debg_gtest})
     # set(Gd_build ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_d_build)
     # set(Gd_install ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_d_install)
     # file(MAKE_DIRECTORY ${Gd_build})
     # message(STATUS "Configuring debug Gtest")
     # target_compile_options(mongoose PUBLIC "/W0")
     # execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${G_source}  -DCMAKE_INSTALL_PREFIX=${Gd_install} -Dgtest_force_shared_crt=OFF
     #   RESULT_VARIABLE result
     #   WORKING_DIRECTORY ${Gd_build} 
     # )
     # if(result)
     #   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
     # endif()
        message(STATUS "Building Debug Gtest")
        execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug --target install
          RESULT_VARIABLE result
          WORKING_DIRECTORY ${G_build}
        )
        if(result)
          message(FATAL_ERROR "Build step for debug googletest failed: ${result}")
        endif()
    endif()
  else()
    message(STATUS "Skipping Gtest build, gtest_built")
  endif()
  set(GTEST_ROOT ${G_install})
  find_package(GTest)
 else()
 message(STATUS "Skipping build Gtests, gtestfound")
 endif()
 
add_custom_target(copy_test_resources ALL 
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/testresources" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/testresources
)
file(GLOB_RECURSE SOURCE_FILES testresources/*)
set(SOURCE_FILES ${SOURCE_FILES} test_example.cpp)
add_executable(Unit_Tests_run ${SOURCE_FILES})
add_dependencies(Unit_Tests_run copy_test_resources gpuvis_simulator)
target_link_libraries(Unit_Tests_run ${Gtest} gpuvis_simulator)
#add_test(Unit_Tests_run Unit_Tests_run)
gtest_discover_tests(Unit_Tests_run WORKING_DIRECTORY "$<TARGET_FILE_DIR:gpuvis_simulator>")
#add_test(NAME monolithic COMMAND Unit_Tests_run)
#add_test(NAME monolithic COMMAND Unit_Tests_run)