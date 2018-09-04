#pragma once
#include "../isa/isa.h"
#include <vector>

#define R9Fury 56, 4, 16

class Register {
public:
  Register();
  ~Register();

private:
};

class ExecutionUnit {
public:
  ExecutionUnit();
  ~ExecutionUnit();

private:
};

struct SimdLane {
  SimdLane(size_t id = 0);
  bool tick(const operation &op);
  size_t id;
  Register VGPR;
  ExecutionUnit EX;
};

struct SimdUnit {
  SimdUnit(size_t SLanes, size_t id = 0);
  bool tick(const operation &op);
  size_t id;
  std::vector<SimdLane> simdLanes;
};

struct ScalerUnit {
  bool tick(const operation &op) {}
  Register SGPR;
  ExecutionUnit EX;
};

struct ComputeUnit {
  ComputeUnit(size_t SimdUs, size_t SLanes, size_t id = 0);
  size_t id;
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
