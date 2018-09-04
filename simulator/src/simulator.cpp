#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {

bool run(const parser::Program &pgrm) {
  GPU gpu = GPU(R9Fury);
  gpu.launchParameters(64, 1, 1);

  auto op = pgrm.ops.begin();
  while (gpu.state == gpu.READY && op != pgrm.ops.end()) {
    gpu.tick(**op);
    op++;
  }
  if (gpu.state != gpu.END) {
    std::cerr << "Program execution fininished in error state! " << std::endl;
  }

  return true;
}; // namespace simulator

} // namespace simulator
