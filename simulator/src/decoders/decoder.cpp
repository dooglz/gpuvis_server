#include "decoder.h"
#include "../isa/isa.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

opcode_type a;

using namespace parser;

#define beginsWith(a, b) (a.find(b, 0) == 0)
#define FAIL(a)                                                                                                        \
  std::cerr << "Invalid program file:\n" << #a << std::endl;                                                           \
  return nullptr;

template <typename T> std::vector<T> split(const T &str, const T &delimiters) {
  std::vector<T> v;
  typename T::size_type start = 0;
  auto pos = str.find_first_of(delimiters, start);
  while (pos != T::npos) {
    if (pos != start) { // ignore empty tokens
      v.emplace_back(str, start, pos - start);
    }
    start = pos + 1;
    pos = str.find_first_of(delimiters, start);
  }
  if (start < str.length()) {                         // ignore trailing delimiter
    v.emplace_back(str, start, str.length() - start); // add what's left of the string
  }
  return v;
}

namespace decoder {
const operand parseOperand(const std::string& input) {
  std::string raw = input;
  bool isRegister = (input[0] == 's' || ((input[0] == 'v') && (input[1] != 'm') && (input[1]!='c')));
  bool isConstant = !isRegister;
  bool isAddress = false; // todo
  bool isLabel = false; //todo
  bool isScaler = isRegister && (input[0] == 's');
  std::vector<uint8_t> regs;
  if (isRegister) {
    if (input[1] == '[') {
      const auto tokens = split<std::string>(input, "[:]");
      const auto min = std::stoi(tokens[1]);
      const auto max = std::stoi(tokens[2]);
      for (auto i = min; i <= max; i++) {
        regs.push_back(i);
      }
      int a = 2;
    } else {
      regs.emplace_back(std::stoi(&input[1]));
    }
  }
  operand ret{raw, isRegister, isConstant, isAddress, isScaler, regs};
  return ret;
}
bool validateOperation(const actual_operation& op) {
  for (auto r : op.op->reads) {
    if (r > op.oa.size()) {
      throw("not enough operands");
    }
  }
  for (auto w : op.op->writes) {
    if (w > op.oa.size()) {
      throw("not enough operands");
    }
  }
  return true;
}


class rga_disasm_compute : public decoderT {
  const std::string name() override { return "rga_disasm_compute"; }
  bool compatible(const std::string &input) override;
  std::unique_ptr<parser::Program> parse(const std::string &input) override;
  const actual_operation parseASM(const std::string &input);
};

bool rga_disasm_compute::compatible(const std::string &input) {
  return beginsWith(input, "ShaderType = IL_SHADER_COMPUTE");
}

class rga_2_disasm_compute : public decoderT {
  const std::string name() override { return "rga_2_disasm"; }
  bool compatible(const std::string& input) override;
  std::unique_ptr<parser::Program> parse(const std::string& input) override;
  const actual_operation parseASM(const std::string& input);
};

bool rga_2_disasm_compute::compatible(const std::string& input) {
  return beginsWith(input, "AMD Kernel Code for");
}



std::unique_ptr<parser::Program> rga_2_disasm_compute::parse(const std::string& input) {
  std::vector<std::string> lines;

  std::string currentline;
  for (const char c : input) {
    switch (c) {
    case '\r':
      break;
    case '\n':
      if (currentline.length() > 0) {
        lines.push_back(currentline);
      }
      currentline = "";
      break;
    default:
      currentline += c;
    }
  }

  auto fs = std::find_if(lines.begin(), lines.end(),
                         [](std::string x) { return beginsWith(x, "@kernel "); });

  if (fs == lines.end()) {
    FAIL("No ASM!");
  }

  auto fe =
      std::find_if(fs, lines.end(), [](std::string x) { return beginsWith(x, "	s_endpgm"); });
  if (fe == lines.end()) {
    FAIL("Too Much ASM!");
  }
  fe++;
  std::vector<actual_operation> ops;

  while (++fs != fe) {
    ops.push_back(parseASM(*fs));
  }

  return std::make_unique<Program>(input, ops);

}

const actual_operation rga_2_disasm_compute::parseASM(const std::string& input) {
  const auto tokens = split<std::string>(input, " ,\t");
  if (!tokens.empty()) {
    const std::string opcode = tokens[0];
    for (auto& opi : ISA) {
      if (opi.opcode_str == opcode) {
        std::vector<operand> operands;
        for (size_t i = 1; i < tokens.size(); i++) {
          if (tokens[i] == "//") {
            break;
          }
          operands.emplace_back(parseOperand(tokens[i]));
        }
        actual_operation ret = {&opi, operands};
        validateOperation(ret);
        return ret;
      }
    }
  }
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  const actual_operation nop = {&ISA[0], {operand("nop")}};
  return nop;
}






const actual_operation rga_disasm_compute::parseASM(const std::string &input) {
  const auto tokens = split<std::string>(input, " ,");
  if (!tokens.empty()) {
    const std::string opcode = tokens[0];
    for (auto &opi : ISA) {
      if (opi.opcode_str == opcode) {
        std::vector<operand> operands;
        for (size_t i = 1; i < tokens.size(); i++) {
          if (tokens[i] == "//") {
            break;
          }
          operands.emplace_back(parseOperand(tokens[i]));
        }
        actual_operation ret = {&opi, operands};
        validateOperation(ret);
        return ret;
      }
    }
  }
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  const actual_operation nop = {&ISA[0], {operand("nop")}};
  return nop;
}

std::unique_ptr<Program> rga_disasm_compute::parse(const std::string &input) {
  std::cout << "rga_disasm_compute parsing" << std::endl;
  std::vector<std::string> lines;

  std::string currentline;
  for (const char c : input) {
    switch (c) {
    case '\r':
      break;
    case '\n':
      if (currentline.length() > 0) {
        lines.push_back(currentline);
      }
      currentline = "";
      break;
    default:
      currentline += c;
    }
  }

  auto fs = std::find_if(lines.begin(), lines.end(), [](std::string x) { return beginsWith(x, "shader main"); });

  if (fs == lines.end()) {
    FAIL("No ASM!");
  }

  auto fe = std::find_if(fs, lines.end(), [](std::string x) { return beginsWith(x, "end"); });
  if (fe == lines.end()) {
    FAIL("Too Much ASM!");
  }
  std::vector<actual_operation> ops;

  while (++fs != fe) {
    ops.push_back(parseASM(*fs));
  }

  return std::make_unique<Program>(input, ops);
}



std::unique_ptr<decoder::decoderT> find(const std::string &input) {
  std::unique_ptr<decoder::decoderT> b[] = {
      std::make_unique<decoder::rga_disasm_compute>(),
      std::make_unique<decoder::rga_2_disasm_compute>()};
  for (auto &d : b) {
    if (d->compatible(input)) {
      return std::move(d);
    }
  }
  return nullptr;
}
} // namespace decoder
