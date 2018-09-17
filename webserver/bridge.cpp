#include "../simulator/src/gpuvis.h"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <string>
void InitBridge() {
  std::cout << "woop" << std::endl;
  std::cout << gpuvis::hello() << std::endl;
}

void ShutdownBridge() {}

std::string inputFile(const std::string &ip) {
  auto pgrm = gpuvis::loadProgram(ip);
  if (pgrm == 0) {
    throw std::exception("Problem loading Program");
  }

  auto gpu = gpuvis::initGPU();
  if (gpu == 0) {
    throw std::exception("Problem initGPU");
  }

  auto res = gpuvis::runProgram(pgrm, gpu);
  if (res == 0) {
    throw std::exception("Problem runProgram");
  }

  auto json = gpuvis::summary(pgrm, gpu);
  if (json == "") {
    throw std::exception("Problem json");
  }
  return json;
}
