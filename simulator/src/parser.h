#pragma once
#include "isa/isa.h"
#include <map>
#include <memory>
#include <string>

namespace parser {
class Program {
public:
  Program(const std::string& raw, std::vector<operation*>& ops);
  ~Program();
  const std::string raw;
  const std::vector<operation*> ops;
  std::map<opcode_type, uint16_t> opcount;

private:
};

std::unique_ptr<Program> parse(const std::string& input);
} // namespace parser
