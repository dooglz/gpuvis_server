#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {

bool run(const parser::Program& pgrm) {
  GPU gpu = GPU(R9Fury);
  gpu.launchParameters(64, 1, 1);

  for (const auto op : pgrm.ops) {
    gpu.tick(*op);
  }

  return true;
};

} // namespace simulator
