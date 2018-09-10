#pragma once
#include "../isa/isa.h"
#include <map>
#include <vector>
struct size_tV {
  size_t x;
  size_t y;
  size_t z;
  operator std::string() const {
    return ('(' + std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(z) + ')');
  }
};
inline std::ostream &operator<<(std::ostream &Str, size_tV const &v) {
  Str << std::string(v);
  return Str;
}

//#define R9Fury 56, 4, 16
#define R9Fury 2, 4, 16
class GPU;
class GPU_Component {
public:
  GPU_Component(const GPU &gpu, size_t id = 0, GPU_Component *parent = nullptr, std::string type = "");
  GPU_Component() = delete;

public:
  const GPU &_gpu;
  const size_t _localId;
  const size_t _globalId;
  const GPU_Component *_parent;
  const std::string _type;
};

class Register : public GPU_Component {
public:
  Register(const GPU &gpu, GPU_Component *parent = nullptr, size_t id = 0);
  Register() = delete;
  ~Register() = default;
  const void read(uint8_t addr);
  const void write(uint8_t addr);

  std::map<size_t, std::vector<uint8_t>> reads;
  std::map<size_t, std::vector<uint8_t>> writes;

private:
};

class ExecutionUnit {
public:
  ExecutionUnit() = default;
  ~ExecutionUnit() = default;

private:
};

struct SimdLane : public GPU_Component {

  SimdLane(const GPU &gpu, size_t id = 0, GPU_Component *parent = nullptr);

  bool tick(actual_operation op);
  Register VGPR;
  ExecutionUnit EX;
  size_t executionID() const;
  size_tV executionIDXYZ() const;
};

struct SimdUnit : public GPU_Component {
  SimdUnit(size_t SLanes, const GPU &gpu, size_t id = 0, GPU_Component *parent = nullptr);
  bool tick(const actual_operation op);
  std::vector<SimdLane> simdLanes;
};

struct ScalerUnit : public GPU_Component {
  ScalerUnit(const GPU &gpu, GPU_Component *parent = nullptr, size_t id = 0);
  bool tick(const actual_operation op);
  Register SGPR;
  ExecutionUnit EX;
};

struct ComputeUnit : public GPU_Component {
  ComputeUnit(size_t SimdUs, size_t SLanes, const GPU &gpu, size_t id = 0, GPU_Component *parent = nullptr);
  std::vector<SimdUnit> simdUnits;
  bool tick(const actual_operation op);
  ScalerUnit SU;
  Register LDS;
};

struct Memory {};

class GPU {
public:
  GPU(size_t CUs, size_t SimdUs, size_t SLanes);
  ~GPU();
  //
  std::vector<ComputeUnit> computeUnits;
  Memory ram;
  //
  size_tV _launchParamters;
  size_t _CUs, _SimdUs, _SLanes;
  size_t active_cus;
  size_t active_lanes;
  //
  void launchParameters(size_tV lp);
  bool tick(const actual_operation op);
  size_t tickcount;
  enum GPUstate { READY, END, CRASH };
  GPUstate state;
  //
};

static void gpustats(const GPU &gpu);