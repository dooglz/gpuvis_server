#pragma once
#include "isa/isa.h"
#include "parser.h"
#include <tuple>

namespace simulator {
const int init();
bool run(const parser::Program& pgrm, int GPUID);
void pgrmstats(const parser::Program& pgrm);

struct RegisterEvent {
  const std::string who;
  const std::vector<uint8_t> reads;
  const std::vector<uint8_t> writes;
  RegisterEvent() = default;
};

using RegisterEventTicks = std::map<size_t, std::vector<RegisterEvent>>;

struct SimulationSummary {
  const RegisterEventTicks registerEventTicks;
  const std::vector<actual_operation> ops;
  const std::string source;
  const std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation;
};

SimulationSummary summary(const parser::Program& pgrm, int GPUID);

} // namespace simulator
