#include "gpu.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <iostream>

const void Register::read(uint8_t addr) { reads[_gpu.tickcount].push_back(addr); }
const void Register::write(uint8_t addr) { writes[_gpu.tickcount].push_back(addr); }

bool ComputeUnit::tick(const actual_operation op) {
  if (op.op->type == SCALER) {

  } else if (op.op->type == VECTOR) {
    bool ret = true;
    for (auto su : simdUnits) {
      ret &= su.tick(op);
    }
    return ret;
  } else if (op.op->type == FLAT) {
  }

  return true;
}

bool SimdUnit::tick(const actual_operation op) {
  bool ret = true;
  for (auto sl : simdLanes) {
    ret &= sl.tick(op);
  }
  return ret;
}

bool GPU::tick(const actual_operation op) {
  tickcount++;
  if (op.op->opcode == s_endpgm) {
    state = END;
  }
  std::cout << "gpu: processing: " << op.op->opcode_str << " -- " << op.oa << std::endl;
  bool ret = true;
  for (size_t i = 0; i < active_cus; ++i) {
    ret &= computeUnits[i].tick(op);
  }
  if (!ret) {
    state = CRASH;
  }
  return ret;
}

bool SimdLane::tick(const actual_operation op) {
  for (auto rr : op.op->reads) {
    VGPR.read(rr);
  }
  for (auto wr : op.op->writes) {
    VGPR.write(wr);
  }

  return true;
}

bool ScalerUnit::tick(const actual_operation op) {
  for (auto rr : op.op->reads) {
    SGPR.read(rr);
  }
  for (auto wr : op.op->writes) {
    SGPR.write(wr);
  }
  return true;
}
