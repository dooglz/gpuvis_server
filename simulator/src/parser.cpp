#include "parser.h"
#include "decoders/decoder.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace parser;

Program::Program(const std::string &raw, std::vector<const operation *> &_ops) : raw(raw), ops(_ops) {
  std::cout << "Program Constructor" << std::endl;
  for (const auto op : ops) {
    opcount[op->type]++;
  }

  for (auto elem : opcount) {
    std::cout << opcode_type_string[elem.first] << ": " << elem.second << "\n";
  }
  std::cout << "Program Ready, " << ops.size() << " ops" << std::endl;
}

Program::~Program() {}

std::unique_ptr<Program> parser::parse(const std::string &input) {
  auto d = decoder::find(input);
  if (d != nullptr) {
    std::cout << "Parsing file as " << d->name() << std::endl;
    return d->parse(input);
  }
  std::cerr << "Invalid program file" << std::endl;
  return nullptr;
}
