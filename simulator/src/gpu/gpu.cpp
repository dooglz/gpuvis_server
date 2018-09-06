#include "gpu.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <iostream>

const void Register::read(uint8_t addr) { reads[_gpu.tickcount].push_back(addr); }
const void Register::write(uint8_t addr) { writes[_gpu.tickcount].push_back(addr); }

bool ComputeUnit::tick(const operation &op) {
  if (op.type == SCALER) {

  } else if (op.type == VECTOR) {
    bool ret = true;
    for (auto su : simdUnits) {
      ret &= su.tick(op);
    }
    return ret;
  } else if (op.type == FLAT) {
  }

  return true;
}

bool SimdUnit::tick(const operation &op) {
  bool ret = true;
  for (auto sl : simdLanes) {
    ret &= sl.tick(op);
  }
  return ret;
}

bool GPU::tick(const operation &op) {
  tickcount++;
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

bool SimdLane::tick(const operation &op) {
  for (auto rr : op.reads) {
    VGPR.read(rr);
  }
  for (auto wr : op.writes) {
    VGPR.write(wr);
  }

  return true;
}

bool ScalerUnit::tick(const operation &op) {
  for (auto rr : op.reads) {
    SGPR.read(rr);
  }
  for (auto wr : op.writes) {
    SGPR.write(wr);
  }
  return true;
}
