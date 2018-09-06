#pragma once
#include "isa/isa.h"
#include <map>
#include <memory>
#include <string>

namespace parser {
class Program {
public:
  Program(std::string raw, std::vector<const operation *> &_ops);
  ~Program();
  const std::string raw;
  const std::vector<const operation *> ops;


private:
};

std::unique_ptr<Program> parse(const std::string &input);
} // namespace parser
