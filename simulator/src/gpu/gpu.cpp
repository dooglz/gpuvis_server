#include "gpu.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <iostream>

const void Register::read(const operand &addr) {
  if (addr.isRegister) {
    for (auto r : addr.regs) {
      reads[_gpu.tickcount].push_back(r);
    }
  } else {
    reads[_gpu.tickcount].push_back(128);
  }
}

const void Register::write(const operand &addr) {
  if (addr.isRegister) {
    for (auto r : addr.regs) {
      writes[_gpu.tickcount].push_back(r);
    }
  } else {
    writes[_gpu.tickcount].push_back(128);
  }
}

bool ComputeUnit::tick(const actual_operation op) {
  bool ret = true;
  if (op.op->type == SCALER) {
    ret &= SU.tick(op);
  } else if (op.op->type == VECTOR) {
    for (auto &su : simdUnits) {
      ret &= su.tick(op);
    }
    return ret;
  } else if (op.op->type == FLAT) {
    for (auto &su : simdUnits) {
      ret &= su.tick(op);
    }
    return ret;
  }

  return true;
}

bool SimdUnit::tick(const actual_operation op) {
  bool ret = true;
  for (auto &sl : simdLanes) {
    ret &= sl.tick(op);
  }
  return ret;
}

bool GPU::tick(const actual_operation op) {
  tickcount++;
  if (op.op->opcode == s_endpgm) {
    state = END;
  }

  std::cout << "gpu: processing: " << op.op->opcode_str << " -- ";
  for (auto a : op.oa) {
    if (a.isRegister) {
      std::cout << ", [";
      for (auto r : a.regs) {
        std::cout << +r << ",";
      }
      std::cout << "]";
    } else {
      std::cout << ", " << a.raw;
    }
  }
  std::cout << std::endl;
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
    VGPR.read(op.oa[rr]);
  }
  for (auto wr : op.op->writes) {
    VGPR.write(op.oa[wr]);
  }

  return true;
}

bool ScalerUnit::tick(const actual_operation op) {
  for (auto rr : op.op->reads) {
    SGPR.read(op.oa[rr]);
  }
  for (auto wr : op.op->writes) {
    SGPR.write(op.oa[wr]);
  }
  return true;
}
