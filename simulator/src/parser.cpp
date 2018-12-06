#include "parser.h"
#include "decoders/decoder.h"
#include "isa/isa.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using namespace parser;

Program::Program(std::string raw, std::vector<actual_operation>& _ops, const std::string& name)
    : raw(std::move(raw)), ops(_ops), name(name) {
  std::cout << "Program Constructor" << std::endl;
  std::cout << "Program Ready, " << ops.size() << " ops" << std::endl;
}

Program::~Program() = default;

std::unique_ptr<Program> parser::parse(const std::string& input,
                                       const std::string& name) {
  auto d = decoder::find(input);
  if (d != nullptr) {
    std::cout << "Parsing file as " << d->name() << std::endl;
    return d->parse(input,name);
  }
  std::cerr << "Invalid program file" << std::endl;
  return nullptr;
}
