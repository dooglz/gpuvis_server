#include "parser.h"
#include "simulator.h"
#include <string>

const std::string gpuvis::hello() { return "Hello Simulator World"; }

const bool gpuvis::loadProgram(const std::string &pgrm) {
  auto a = parser::parse(pgrm);
  return true;
}