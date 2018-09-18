#pragma once
#include "json.hpp"
#include "simulator.h"
#include <string>
using json = nlohmann::json;

namespace simulator {
void to_json(json &j, const SimulationSummary &s) { j = json{{"rET", s.registerEventTicks}}; }
void to_json(json &j, const RegisterEvent &re) {
  j = json{{"id", re.who}, {"r", re.reads}, {"w", re.writes}};
}
} // namespace simulator

namespace output {

std::string gimmyjson(simulator::SimulationSummary &simsum) { return json(simsum).dump(); }

std::vector<std::uint8_t> gimmyMsgPack(simulator::SimulationSummary &simsum) {
  return json::to_msgpack(json(simsum));
}

} // namespace output
