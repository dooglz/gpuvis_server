#pragma once
#include "json.hpp"
#include "simulator.h"
#include <string>
using json = nlohmann::json;

namespace simulator {
void to_json(json &j, const SimulationSummary &s) { j = json{{"registerEventTicks", s.registerEventTicks}}; }
void to_json(json &j, const RegisterEvent &re) {
  j = json{{"who", re.who}, {"reads", re.reads}, {"writes", re.writes}};
}
} // namespace simulator

namespace output {

std::string gimmyjson(simulator::SimulationSummary &simsum) {
  auto j = json(simsum);
  return j.dump(1);
}

} // namespace output
