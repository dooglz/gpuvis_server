#include "../simulator/src/gpuvis.h"
#include <chrono>
#include <iostream>
#include <string>

void InitBridge() {
  std::cout << "woop" << std::endl;
  std::cout << gpuvis::hello() << std::endl;
}

void ShutdownBridge() {}

void inputFile(const std::string &ip) { gpuvis::loadProgram(ip); }
