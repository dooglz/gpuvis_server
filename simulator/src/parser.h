#pragma once
#include "isa/isa.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <tuple>

namespace parser {
class Program {
public:
  Program(std::string raw, std::vector<actual_operation>& _ops, const std::string& name);
  Program(const Program&) = delete;
  Program() = delete;
  ~Program();
  const std::string raw; // raw input assembly
  const std::string name; // raw input assembly
  std::string source;    // optional originating source code.
  std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation; // optional
  const std::vector<actual_operation> ops;

private:
};

std::unique_ptr<Program> parse(const std::string& input, const std::string& name);
} // namespace parser
