#pragma once
#include <string>
#include <utility>
#include <vector>

enum opcode_type { VECTOR, SCALER, MEMORY, FLAT, GLOBAL };

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
  ~operation() = default;
};

// typedef std::string operand;

struct operand {
  const std::string raw;
  const bool isRegister;
  const bool isConstant;
  const bool isAddress;
  const bool isScaler; // Else Vector
  const std::vector<uint8_t> regs;
  operand() = delete;
  operand(std::string r, bool re = false, bool cn = false, bool ad = false, bool sc = false,
          const std::vector<uint8_t> rgs = {})
      : raw(std::move(r)), isRegister(re), isConstant(cn), isAddress(ad), isScaler(sc), regs(rgs) {}
  ~operand() = default;
};

struct actual_operation {
  const operation *op;
  const std::vector<operand> oa;
  ~actual_operation() = default;
};

#define TODO
#define REGS(...) std::vector<uint8_t>({__VA_ARGS__})
#define OP(opcode, type, branch, ticks, help, r, w)                                                                    \
  { #opcode, opcode, type, branch, ticks, #help, r, w }
