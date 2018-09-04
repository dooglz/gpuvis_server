#include "gpu.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <cmath>
#include <iostream>
#include <stack>

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
    computeUnits.push_back(ComputeUnit(SimdUs, SLanes, *this, i, nullptr));
  }
}

ComputeUnit::ComputeUnit(size_t SimdUs, size_t SLanes, const GPU &gpu, size_t id, GPU_Component *const parent)
    : GPU_Component(gpu, id, parent) {
  for (size_t i = 0; i < SimdUs; ++i) {
    simdUnits.push_back(SimdUnit(SLanes, gpu, i, this));
  }
}

SimdUnit::SimdUnit(size_t SLanes, const GPU &gpu, size_t id, GPU_Component *const parent)
    : GPU_Component(gpu, id, parent) {
  for (size_t i = 0; i < SLanes; ++i) {
    simdLanes.push_back(SimdLane(gpu, i, this));

  }

}

GPU::~GPU() {}

// auto glambda = [](auto a) { return a; };

auto glambda2 = [](size_t global_id, uint8_t &x, uint8_t &y, uint8_t &z) {
  size_t gi = global_id;
  std::stack<uint8_t> chain;
  uint8_t shift = 0;
  while (shift < 32) {
    chain.push((gi >> shift) & 0xFF);
    shift += 8;
    std::cout << ((gi >> shift) & 0xFF) << "_";
  }
  std::cout << std::endl;


  return;
};

auto glambda = [](GPU_Component *parent, size_t id, uint8_t dim) {
  if (parent == nullptr) {
    return id;
  }
  std::stack<size_t> chain;
  GPU_Component const*  p = parent;
  chain.push(id);
  while (p != nullptr) {
    chain.push(p->_localId);
    p = p->_parent;
  }
  const uint8_t storagebits = 8; // 256 max dimensional ID
  uint8_t shift = 0;
  size_t global_id = 0;
  while (!chain.empty()) {
    const auto lid = chain.top();
    chain.pop();
    if (lid > (1 << 8) || shift > 56) {
      throw std::out_of_range("lid");
    }
    global_id += (lid << shift);
    shift += storagebits;
  }

  uint8_t x, y, z;
  glambda2(global_id, x, y, z);

  return global_id;
};

GPU_Component::GPU_Component(const GPU &gpu, size_t id, GPU_Component *parent)
    : _gpu(gpu), _localId(id), _parent(parent), _globalId(glambda(parent,id, 0)), _globalIdX(glambda(parent, id, 1)),
      _globalIdY(glambda(parent, id, 2)), _globalIdZ(glambda(parent, id, 3)) {
 // std::cout << "gpu: New thing: " << _globalId << std::endl;

}

//--- Meaty stuff now

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
bool SimdUnit::tick(const operation &op) {
  bool ret = true;
  for (auto sl : simdLanes) {
    ret &= sl.tick(op);
  }
  return ret;
}

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

SimdLane::SimdLane(const GPU &gpu, size_t id, GPU_Component *const parent) : GPU_Component(gpu, id, parent) {}

bool SimdLane::tick(const operation &op) {
  for (auto rr : op.reads) {
    VGPR.read(rr);
  }
  for (auto wr : op.writes) {
    VGPR.write(wr);
  }

  return true;
}