#include "parser.h"
#include "decoders/decoder.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using namespace parser;

Program::Program(std::string raw, std::vector<actual_operation > &_ops) : raw(std::move(raw)), ops(_ops) {
  std::cout << "Program Constructor" << std::endl;

  std::cout << "Program Ready, " << ops.size() << " ops" << std::endl;
}

Program::~Program() = default;

std::unique_ptr<Program> parser::parse(const std::string &input) {
  auto d = decoder::find(input);
  if (d != nullptr) {
    std::cout << "Parsing file as " << d->name() << std::endl;
    return d->parse(input);
  }
  std::cerr << "Invalid program file" << std::endl;
  return nullptr;
}
