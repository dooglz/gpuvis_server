#pragma once
#include "amdgpu_operations.h"
#include "json.hpp"
#include "simulator.h"
#include <string>
using json = nlohmann::json;

namespace simulator {
void to_json(json& j, const ProgramSummary& s) {
  j = json{{"rET", s.registerEventTicks}, {"ops", s.ops}, {"lines", s.lineCorralation}, {"name",s.name}};
}
void to_json(json& j, const SimulationSummary& s) {
  j = json{{"programs", s.programs}, {"source", s.source}};
}
void to_json(json& j, const RegisterEvent& re) {
  j = json{{"id", re.who}, {"r", re.reads}, {"w", re.writes}};
}

} // namespace simulator

void to_json(json& j, const operand& oa) {
  j = oa.raw;
  // to_json(j, oa.raw);
}

void to_json(json& j, const actual_operation& ao) {
  j = json{{"op", ao.op->opcode_str}, {"oa", ao.oa}};
}

namespace output {

std::string gimmyjson(const simulator::SimulationSummary& simsum) {
  return json(simsum).dump();
}

std::vector<std::uint8_t> gimmyMsgPack(const simulator::SimulationSummary& simsum) {
  return json::to_msgpack(json(simsum));
}

} // namespace output
