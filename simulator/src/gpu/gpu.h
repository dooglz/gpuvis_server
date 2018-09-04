#pragma once
#include "../isa/isa.h"
#include <vector>

//#define R9Fury 56, 4, 16
#define R9Fury 2, 4, 16
class GPU;
class GPU_Component {
public:
  GPU_Component(const GPU &gpu, size_t id = 0, GPU_Component *const parent = nullptr);
  GPU_Component() = delete;

public:
  const GPU &_gpu;
  const size_t _localId;
  const size_t _globalId, _globalIdX, _globalIdY, _globalIdZ;
  const GPU_Component *_parent;
};

class Register {
public:
  Register();
  ~Register();
  const void read(uint8_t addr){};
  const void write(uint8_t addr){};

private:
};

class ExecutionUnit {
public:
  ExecutionUnit();
  ~ExecutionUnit();

private:
};

struct SimdLane : public GPU_Component {
  SimdLane(const GPU &gpu, size_t id = 0, GPU_Component *constparent = nullptr);
  bool tick(const operation &op);
  Register VGPR;
  ExecutionUnit EX;
};

struct SimdUnit : public GPU_Component {
  SimdUnit(size_t SLanes, const GPU &gpu, size_t id = 0, GPU_Component *constparent = nullptr);
  bool tick(const operation &op);
  std::vector<SimdLane> simdLanes;
};

struct ScalerUnit {
  bool tick(const operation &op) {}
  Register SGPR;
  ExecutionUnit EX;
};

struct ComputeUnit : public GPU_Component {
  ComputeUnit(size_t SimdUs, size_t SLanes, const GPU &gpu, size_t id = 0, GPU_Component *const parent = nullptr);
  std::vector<SimdUnit> simdUnits;
  bool tick(const operation &op);
  ScalerUnit SU;
  Register LDS;
};

class GPU {
public:
  GPU(size_t CUs, size_t SimdUs, size_t SLanes);
  ~GPU();
  //
  std::vector<ComputeUnit> computeUnits;
  size_t launch_x, launch_y, launch_z;
  size_t _CUs, _SimdUs, _SLanes;
  size_t active_cus;
  //
  void launchParameters(size_t x, size_t y, size_t z);
  bool tick(const operation &op);
  size_t tickcount;
  enum GPUstate { READY, END, CRASH };
  GPUstate state;
};
