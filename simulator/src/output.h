#pragma once
#include "program.h"
#include "amdgpu_operations.h"
#include "json.hpp"
#include "simulator.h"
#include <string>
using json = nlohmann::json;

void to_json(json& j, const Program* p) {
  j = json(*p);
}

void to_json(json& j, const Program& p) {
  j = json{ {"rET", p.simsum->registerEventTicks}, {"ops", p.ops}, {"lines", p.lineCorralation}, {"name",p.name} };
  //j = json{ {"boop" } };
}

void to_json(json& j, const operand& oa) {
  j = oa.raw;
}
void to_json(json& j, const actual_operation& ao) {
  j = json{ {"op", ao.op->opcode_str}, {"oa", ao.oa} };
}

namespace simulator {
  void to_json(json& j, const RegisterEvent& re) {
    j = json{ {"id", re.who}, {"r", re.reads}, {"w", re.writes} };
  }
}

namespace output {


  std::string gimmyjson(const std::vector<int> &pgrmids) {
    std::vector<json> Jpgrms;
    Jpgrms.resize(pgrmids.size());
    std::transform(pgrmids.begin(), pgrmids.end(), Jpgrms.begin(), [](const int& pid) { return  json(findpgrm(pid)); });
    return (json{ {"programs", Jpgrms}, {"source", findpgrm(pgrmids[0]).source} }).dump();
  }

  std::vector<std::uint8_t> gimmyMsgPack(const std::vector<int> &pgrmids) {
    std::vector<json> Jpgrms;
    Jpgrms.resize(pgrmids.size());
    std::transform(pgrmids.begin(), pgrmids.end(), Jpgrms.begin(), [](const int& pid) {return json(findpgrm(pid)); });
    return json::to_msgpack(json{ {"programs", Jpgrms}, {"source",findpgrm(pgrmids[0]).source} });
  }

} // namespace output
