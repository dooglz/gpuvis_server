#pragma once
#include "amdgpu_operations.h"
#include "program.h"
#include <tuple>
#include <map>

class GPU;
namespace simulator {
bool run(Program& pgrm);
void pgrmstats(const Program& pgrm);

struct RegisterEvent {
  const std::string who;
  const std::vector<uint8_t> reads;
  const std::vector<uint8_t> writes;
  RegisterEvent() = default;
};

using RegisterEventTicks = std::map<size_t, std::vector<RegisterEvent>>;

struct ProgramSummary {
 // const std::string name;
 // const RegisterEventTicks registerEventTicks;
  //const std::vector<actual_operation> ops;
  //const std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation;
};
struct SimulationSummary {
  //const std::vector<ProgramSummary> programs;
  //const std::string source;
  const RegisterEventTicks registerEventTicks;

  SimulationSummary() = delete;
  SimulationSummary(const GPU& gpu, const Program& pgrm);

};

ProgramSummary summary(const Program& pgrm);

} // namespace simulator
