#include "isa.h"
#include "fiji.h"

const operation &isa::getOpt(const unsigned int opcode) { return ISA[0]; }

const operation &isa::getOpt(const std::string &opcode_string) { return ISA[0]; }