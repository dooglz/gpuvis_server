#pragma once
#include "json.hpp"
#include <string>
using json = nlohmann::json;
namespace output {
std::string gimmyjson() {
  auto j = R"({
    "happy": true,
    "pi": 3.141
  })"_json;
  return j.dump();
}
} // namespace output
