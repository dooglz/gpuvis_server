#include "program.h"
#include "simulator.h"
#include "decoders/decoder.h"
#include "amdgpu_operations.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

Program::Program(std::string raw, std::vector<actual_operation> &_ops, const std::string &name)
    : raw(std::move(raw)), ops(_ops), name(name),simstate(NOTRUN) {
  std::cout << "Program Constructor" << std::endl;
  std::cout << "Program Ready, " << ops.size() << " ops" << std::endl;
}

Program::~Program() = default;

std::unique_ptr<Program> Program::parse(const std::string &input, const std::string &name) {
  auto d = decoder::find(input);
  std::cout << "Parsing file as " << d->name() << std::endl;
  return d->parse(input, name);
}
