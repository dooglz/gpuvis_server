#include "gpu.h"
#include "../parser.h"
#include <iostream>

Register::Register() {}
Register::~Register() {}
ExecutionUnit::ExecutionUnit() {}
ExecutionUnit::~ExecutionUnit() {}

GPU::GPU(size_t CUs, size_t SimdUs, size_t SLanes) {
  tickcount = 0;
  state = READY;
}
GPU::~GPU() {}

bool GPU::tick(const operation& const op) {

  std::cout << "gpu: processing" << op.opcode << std::endl;
  return true;
}
