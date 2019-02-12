#include "isa.h"
#include "fiji.h"
#include <algorithm>

const operation &isa::getOpt(const unsigned int opcode) { 
  const auto a = std::find_if(std::begin(ISA), std::end(ISA), [opcode](const operation& o) { return o.opcode == opcode; });
  if (std::end(ISA) == a) {
    return ISA[0];
  }
  return *a;
}

const operation &isa::getOpt(const std::string &opcode_string) { 
  const auto a = std::find_if(std::begin(ISA), std::end(ISA), [opcode_string](const operation& o) { return o.opcode_str == opcode_string; });
  if (std::end(ISA) == a) {
    return ISA[0];
  }
  return *a;
}