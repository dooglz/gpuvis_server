#pragma once
#include <string>
#include <utility>
#include <vector>

enum OPCODE_TYPE {
  DS = 0,
  EXP = 1,
  FLAT = 2,
  MIMG = 3,
  MTBUF = 4,
  MUBUF = 5,
  SMEM = 6,
  SOP1 = 7,
  SOP2 = 8,
  SOPC = 9,
  SOPK = 10,
  SOPP = 11,
  VINTRP = 12,
  VOP1 = 13,
  VOP2 = 14,
  VOP3 = 15,
  VOP3P = 16,
  VOPC = 17,
  UNKOWN_VECTOR = 18,
  UNKOWN_SCALER = 19,
  UNKOWN = 20,
};
static const bool isScaler(OPCODE_TYPE t) { return (t < VINTRP || t == UNKOWN_SCALER); }
static const bool isVector(OPCODE_TYPE t) { return (t >= VINTRP && t < UNKOWN_SCALER); }

static const std::string opcode_type_string[] = {
    "DS",   "EXP",  "FLAT",  "MIMG", "MTBUF",         "MUBUF",         "SMEM",
    "SOP1", "SOP2", "SOPC",  "SOPK", "SOPP",          "VINTRP",        "VOP1",
    "VOP2", "VOP3", "VOP3P", "VOPC", "UNKOWN_VECTOR", "UNKOWN_SCALER", "UNKOWN",
};

struct operation {
  const std::string opcode_str;
  const unsigned int opcode;
  const OPCODE_TYPE type;
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
  operand(std::string r, bool re = false, bool cn = false, bool ad = false,
          bool sc = false, const std::vector<uint8_t> rgs = {})
      : raw(std::move(r)), isRegister(re), isConstant(cn), isAddress(ad), isScaler(sc),
        regs(rgs) {}
  ~operand() = default;
};

struct actual_operation {
  const operation* op;
  const std::vector<operand> oa;
  ~actual_operation() = default;
};

#define TODO
#define REGS(...) std::vector<uint8_t>({__VA_ARGS__})
#define OP(opcode, type, branch, ticks, help, r, w)                                      \
  { #opcode, opcode, (OPCODE_TYPE)type, branch, ticks, #help, r, w }
