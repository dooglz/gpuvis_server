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
  set(G2 ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest)
  if(NOT EXISTS ${G2})
   message(STATUS "Cloning Gtest")
    find_package(Git REQUIRED)
    execute_process(
    COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/google/googletest ${G2}
    )
  endif()
  if(NOT gtest_built)
    set(G3 ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_build)
    set(G4 ${CMAKE_CURRENT_BINARY_DIR}/lib/googletest_install)
    file(MAKE_DIRECTORY ${G3})
    message(STATUS "Configuring Gtest")
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${G2}  -DCMAKE_INSTALL_PREFIX=${G4} -Dgtest_force_shared_crt=ON
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${G3} 
    )
    if(result)
      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    message(STATUS "Building Gtest")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release --target install
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${G3}
    )
    if(result)
      message(FATAL_ERROR "Build step for googletest failed: ${result}")
    else()
      set(gtest_built TRUE)
    endif()
    message(STATUS  "Built Gtest!")
  else()
    message(STATUS "Skipping Gtest build")
  endif()
  set(GTEST_ROOT ${G4})
  find_package(GTest)
 endif()

 
add_executable(Unit_Tests_run test_example.cpp)
target_link_libraries(Unit_Tests_run ${Gtest})
#add_test(Unit_Tests_run Unit_Tests_run)
gtest_discover_tests(Unit_Tests_run)
add_test(NAME monolithic COMMAND Unit_Tests_run)
#add_test(NAME monolithic COMMAND Unit_Tests_run)