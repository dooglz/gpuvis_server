#include "gpu.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <cmath>
#include <iostream>

Register::Register() {}
Register::~Register() {}
ExecutionUnit::ExecutionUnit() {}
ExecutionUnit::~ExecutionUnit() {}

GPU::GPU(size_t CUs, size_t SimdUs, size_t SLanes) {
  tickcount = 0;
  state = READY;
  _CUs = CUs;
  _SimdUs = SimdUs;
  _SLanes = SLanes;
  for (size_t i = 0; i < CUs; ++i) {
    computeUnits.push_back(ComputeUnit(SimdUs, SLanes, i));
  }
}

ComputeUnit::ComputeUnit(size_t SimdUs, size_t SLanes, size_t _id) {
  id = _id;
  for (size_t i = 0; i < SimdUs; ++i) {
    simdUnits.push_back(SimdUnit(SLanes, i));
  }
}

bool ComputeUnit::tick(const operation &op) {
  if (op.type == SCALER) {

  } else if (op.type == VECTOR) {
    bool ret = true;
    for (auto su : simdUnits) {
      ret &= su.tick(op);
    }
    return ret;
  }

  return true;
}

SimdUnit::SimdUnit(size_t SLanes, size_t _id) {
  id = _id;
  for (size_t i = 0; i < SLanes; ++i) {
    simdLanes.push_back(SimdLane(i));
  }
}

bool SimdUnit::tick(const operation &op) {
  bool ret = true;
  for (auto sl : simdLanes) {
    ret &= sl.tick(op);
  }
  return ret;
}

GPU::~GPU() {}

void GPU::launchParameters(size_t x, size_t y, size_t z) {
  launch_x = x;
  launch_y = y;
  launch_z = z;
  active_cus = (size_t)ceil(double(x * y * z) / double(_SLanes * _SimdUs));
  std::cout << "gpu: launch: " << x << ',' << y << ',' << z << ", cus:" << active_cus << std::endl;
}

bool GPU::tick(const operation &op) {
  if (op.opcode == s_endpgm) {
    state = END;
  }
  std::cout << "gpu: processing: " << op.opcode_str << std::endl;
  bool ret = true;
  for (size_t i = 0; i < active_cus; ++i) {
    ret &= computeUnits[i].tick(op);
  }
  if (!ret) {
    state = CRASH;
  }
  return ret;
}

SimdLane::SimdLane(size_t _id) { id = _id; }

bool SimdLane::tick(const operation &op) { return true; }
