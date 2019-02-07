#pragma once
#include "amdgpu_operations.h"
#include "program.h"
#include <tuple>

namespace simulator {
const int init();
bool run(const Program& pgrm, int GPUID);
void pgrmstats(const Program& pgrm);

struct RegisterEvent {
  const std::string who;
  const std::vector<uint8_t> reads;
  const std::vector<uint8_t> writes;
  RegisterEvent() = default;
};

using RegisterEventTicks = std::map<size_t, std::vector<RegisterEvent>>;

struct ProgramSummary {
  const std::string name;
  const RegisterEventTicks registerEventTicks;
  const std::vector<actual_operation> ops;
  const std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation;
};
struct SimulationSummary {
  const std::vector<ProgramSummary> programs;
  const std::string source;
};



ProgramSummary summary(const Program& pgrm, int GPUID);

} // namespace simulator
