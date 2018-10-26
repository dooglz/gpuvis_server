#pragma once
#include "isa/isa.h"
#include <map>
#include <memory>
#include <string>


namespace parser {
class Program {
public:
  Program(std::string raw, std::vector<actual_operation> &_ops);
  Program(const Program&) = delete;
  Program() = delete;
  ~Program();
  const std::string raw; //raw input assembly
  std::string source; //optional originating source code.
  const std::vector<actual_operation> ops;

private:
};

std::unique_ptr<Program> parse(const std::string &input);
} // namespace parser
