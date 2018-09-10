#include "../simulator/src/gpuvis.h"
#include "gtest/gtest.h"
#include <fstream>
#include <streambuf>
#include <string>

TEST(Example, Equals) { EXPECT_EQ(1, 1); }
TEST(Simulator, hello) { ASSERT_NE(gpuvis::hello(), ""); }

static int pgrm = 0;
static int gpu = 0;

TEST(Simulator, basicprogram) {
  std::ifstream t("testresources/Fiji_HelloWorld_helloworld.cl_isa.txt");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  ASSERT_NE(str, "") << "Can't open test file";

  pgrm = gpuvis::loadProgram(str);
  ASSERT_GT(pgrm, 0);
  gpu = gpuvis::initGPU();

  auto res = gpuvis::runProgram(pgrm, gpu);
  ASSERT_GT(res, 0);

  gpuvis::summary(pgrm, gpu);

}
