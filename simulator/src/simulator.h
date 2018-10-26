#pragma once
#include "parser.h"

namespace simulator {
const int init();
bool run(const parser::Program &pgrm, int GPUID);
void pgrmstats(const parser::Program &pgrm);

struct RegisterEvent {
  const std::string who;
  const std::vector<uint8_t> reads;
  const std::vector<uint8_t> writes;
  RegisterEvent() = default;
};

typedef std::map<size_t, std::vector<RegisterEvent>> RegisterEventTicks;

struct SimulationSummary {
  const RegisterEventTicks registerEventTicks;
  const std::vector<actual_operation> ops;
  const std::string source;
};

SimulationSummary summary(const parser::Program &pgrm, int GPUID);

} // namespace simulator
