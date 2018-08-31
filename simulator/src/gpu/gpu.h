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
  Register VGPR;
  ExecutionUnit EX;
};
struct SimdUnit {
  std::vector<SimdLane> SimdLanes;
};

struct ScalerUnit {
  Register SGPR;
  ExecutionUnit EX;
};

struct ComputeUnit {
  std::vector<SimdUnit> SimdUnits;
  ScalerUnit SU;
  Register LDS;
};

class GPU {
public:
  GPU(size_t CUs, size_t SimdUs, size_t SLanes);
  ~GPU();
  std::vector<ComputeUnit> ComputeUnits;
  void launchParameters(size_t x, size_t y, size_t z) {}
  bool tick(const operation& const op);
  size_t tickcount;
  enum GPUstate { READY, END, CRASH };
  GPUstate state;
};
