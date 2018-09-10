#pragma once
#include <string>
#include <vector>


enum opcode_type { VECTOR, SCALER, MEMORY, FLAT };

static const std::string opcode_type_string[] = {"VECTOR", "SCALER", "MEMORY", "FLAT"};

struct operation {
  const std::string opcode_str;
  const unsigned int opcode;
  const opcode_type type;
  const bool branching;
  const uint8_t ticks;
  const std::string helptext;
  const std::vector<uint8_t> reads;
  const std::vector<uint8_t> writes;
};

typedef std::string operand;

struct actual_operation {
  const operation *op;
  const operand oa;
};


#define REGS(...) std::vector<uint8_t>({__VA_ARGS__})
#define OP(opcode, type, branch, ticks, help, r, w)                                                                    \
  { #opcode, opcode, type, branch, ticks, #help, r, w }
