#include "../isa/fiji.h"
#include "../parser.h"
#include "gpu.h"
//#include <algorithm>
#include <array>
#include <cmath>
//#include <execution>
#include <iostream>
#include <stack>
//#include <utility>
#define CUSHIFT(gi) (gi >> 0) & 0xFF
#define SUSHIFT(gi) (gi >> 8) & 0xFF
#define SLSHIFT(gi) (gi >> 16) & 0xFF

void idToCmp(size_t gi, uint8_t &CU, uint8_t &SU, uint8_t &SL) {
  CU = CUSHIFT(gi);
  SU = SUSHIFT(gi);
  SL = SLSHIFT(gi);
}
std::string idToStr(size_t gi) {
  const std::array<size_t, 3> a = {CUSHIFT(gi), SUSHIFT(gi), SLSHIFT(gi)};
  // return std::accumulate(std::next(a.begin()), a.end(), std::to_string(a[0]),
  //                        [](std::string a, size_t b) { return (b == 0 ? a : a + "_" + (std::to_string(b))); });
  std::string str;
  for (auto b : a) {
    str += (b == 0 ? "" : "_" + std::to_string(b));
  }
  return str;
}

auto getGlobalID = [](GPU_Component *parent, size_t id, uint8_t dim) {
  if (parent == nullptr) {
    return id;
  }
  std::stack<size_t> chain;
  GPU_Component const *p = parent;
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
    if (lid > (1 << 8) || shift > 24) {
      throw std::out_of_range("lid");
    }
    global_id += (lid << shift);
    shift += storagebits;
  }

  // uint8_t x, y, z;
  // glambda2(global_id, x, y, z);

  return global_id;
};

size_t SimdLane::executionID() const {
  uint8_t CU, SU, SL;
  size_t ex = 0;
  idToCmp(_globalId, CU, SU, SL);
  ex += (CU - 1) * (_gpu._SimdUs * _gpu._SLanes);
  ex += (SU - 1) * (_gpu._SLanes);
  ex += (SL - 1);
  return ex;
}

size_tV SimdLane::executionIDXYZ() const {
  size_t exc = executionID();
  size_tV ex = {0, 0, 0};
  // TODO(40008190):
  if (exc <= _gpu.active_lanes) {
    ex.x = executionID();

  } else {
    throw("");
  }
  ex.y = 0;
  ex.z = 0;

  return ex;
}

Register::Register(const GPU &gpu, GPU_Component *const parent, size_t id) : GPU_Component(gpu, id, parent, "Reg") {}
ScalerUnit::ScalerUnit(const GPU &gpu, GPU_Component *const parent, size_t id)
    : GPU_Component(gpu, id, parent, "SALU"), SGPR(gpu, parent) {}

GPU::GPU(size_t CUs, size_t SimdUs, size_t SLanes) {
  tickcount = 0;
  state = READY;
  _CUs = CUs;
  _SimdUs = SimdUs;
  _SLanes = SLanes;
  for (size_t i = 1; i <= CUs; ++i) {
    computeUnits.emplace_back(SimdUs, SLanes, *this, i, nullptr);
  }
}

ComputeUnit::ComputeUnit(size_t SimdUs, size_t SLanes, const GPU &gpu, size_t id, GPU_Component *const parent)
    : GPU_Component(gpu, id, parent, "CU"), SU(gpu, this), LDS(gpu, this) {
  for (size_t i = 1; i <= SimdUs; ++i) {
    simdUnits.emplace_back(SLanes, gpu, i, this);
  }
}

SimdUnit::SimdUnit(size_t SLanes, const GPU &gpu, size_t id, GPU_Component *const parent)
    : GPU_Component(gpu, id, parent, "SU") {
  for (size_t i = 1; i <= SLanes; ++i) {
    simdLanes.emplace_back(gpu, i, this);
  }
}

SimdLane::SimdLane(const GPU &gpu, size_t id, GPU_Component *const parent)
    : GPU_Component(gpu, id, parent, "SL"), VGPR(gpu, this) {}

GPU::~GPU() = default;

GPU_Component::GPU_Component(const GPU &gpu, size_t id, GPU_Component *parent, std::string type)
    : _gpu(gpu), _localId(id), _globalId(getGlobalID(parent, id, 0)), _parent(parent), _type(std::move(type)) {
  // std::cout << "gpu: New thing: " << _type << _globalId << " " << idToStr(_globalId) << std::endl;
}

void GPU::launchParameters(size_tV lp) {
  _launchParamters = lp;
  active_cus = static_cast<size_t>(ceil(double(lp.x * lp.y * lp.z) / double(_SLanes * _SimdUs)));
  active_lanes = lp.x * lp.y * lp.z;
  std::cout << "gpu: launch: " << lp << " cus:" << active_cus << std::endl;
}

const std::vector<Register *> GPU::GetAllRegisters() {
  std::vector<Register *> ret;
  for (auto& a : computeUnits) {
    for (auto& b : a.simdUnits) {
      for (auto& c : b.simdLanes) {
        ret.emplace_back(&c.VGPR);
      }
    }
    ret.emplace_back(&a.SU.SGPR);
  }
  return ret;
}