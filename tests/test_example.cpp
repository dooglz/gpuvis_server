#include "../simulator/src/gpuvis.h"
#include "gtest/gtest.h"
#include <fstream>
#include <streambuf>
#include <string>

TEST(Example, Equals) { EXPECT_EQ(1, 1); }
//TEST(Simulator, hello) { ASSERT_NE(gpuvis::hello(), ""); }

static int pgrm = 0;
static int gpu = 0;

TEST(Simulator, basicprogram) {
  std::ifstream t("testresources/gfx900_helloworld_4d33bf14-d510-4ef0-b9b8-a5e43b98584e.txt");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  ASSERT_NE(str, "") << "Can't open test file";

  pgrm = gpuvis::loadProgram(str);
  ASSERT_GT(pgrm, 0);

  auto res = gpuvis::runProgram(pgrm);
  ASSERT_GT(res, 0);

  auto json = gpuvis::summaryJSON({pgrm});

  ASSERT_NE(json, "") << "empty JSON!";
  std::cout << "output size:" << json.length() << std::endl;
  if (json.length() > 512) {
    std::cout << std::endl
              << json.substr(0, 256) << "\n...\n"
              << json.substr(json.length() - 256, json.length()) << std::endl;
  } else {
    std::cout << std::endl << json << std::endl;
  }
}
