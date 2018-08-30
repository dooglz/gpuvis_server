#include "../simulator/src/simulator.h"
#include "gtest/gtest.h"
#include <fstream>
#include <streambuf>
#include <string>

TEST(Example, Equals) { EXPECT_EQ(1, 1); }
TEST(Simulator, hello) { ASSERT_NE(gpuvis::hello(), ""); }

TEST(Simulator, basicprogram) {
  std::ifstream t("testresources/Fiji_HelloWorld_helloworld.cl_isa.txt");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  ASSERT_NE(str, "") << "Can't open test file";
  ASSERT_TRUE(gpuvis::loadProgram(str));
}
