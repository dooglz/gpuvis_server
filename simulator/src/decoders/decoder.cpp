#include "decoder.h"
#include "../amdgpu_operations.h"
#include "../program.h"
#include <algorithm>
#include <iostream>
#include <isa.h>
#include <memory>
#include <vector>

OPCODE_TYPE a;

using namespace isa;

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
operand parseOperand(const std::string &input) {
  std::string raw = input;
  bool isRegister = (input[0] == 's' || ((input[0] == 'v') && (input[1] != 'm') && (input[1] != 'c')));
  bool isConstant = !isRegister;
  bool isAddress = false; // todo
  bool isLabel = false;   // todo
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
bool validateOperation(const actual_operation &op) {
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
  std::unique_ptr<Program> parse(const std::string &input, const std::string &name) override;
  const actual_operation parseASM(const std::string &input);
};

bool rga_disasm_compute::compatible(const std::string &input) {
  return beginsWith(input, "ShaderType = IL_SHADER_COMPUTE");
}

class rga_2_disasm_compute : public decoderT {
  const std::string name() override { return "rga_2_disasm"; }
  bool compatible(const std::string &input) override;
  std::unique_ptr<Program> parse(const std::string &input, const std::string &name) override;
  const bool parseASM(const std::string &input, const operation *&op_out, std::vector<operand> &oa_out);
  std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation() override;
  std::vector<std::tuple<uint8_t, uint8_t>> _lineCorralations;
  void handleLineNum(const std::string &line);
  uint8_t _currentasmNum;
};

bool rga_2_disasm_compute::compatible(const std::string &input) {
  bool looksgood = false;
  looksgood |= beginsWith(input, "AMD Kernel Code for");
  looksgood |= (beginsWith(input, "; -------- Disassembly ") && input.substr(44, 11) == "shader main");

  return looksgood;
}

std::unique_ptr<Program> rga_2_disasm_compute::parse(const std::string &input, const std::string &name) {
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

  auto fs = std::find_if(lines.begin(), lines.end(), [](std::string x) { return beginsWith(x, "@kernel "); });

  if (fs == lines.end()) {
    fs = std::find_if(lines.begin(), lines.end(), [](std::string x) { return beginsWith(x, "  type("); });
  }

  if (fs == lines.end()) {
    FAIL("No ASM!");
  }

  auto fe = std::find_if(fs, lines.end(), [](std::string x) { return (x.find("s_endpgm") != std::string::npos); });
  if (fe == lines.end()) {
    FAIL("Too Much ASM!");
  }
  _currentasmNum = 0;
  fe++;
  std::vector<actual_operation> ops;

  while (++fs != fe) {
    const operation *opi;
    std::vector<operand> operands;
    if (parseASM(*fs, opi, operands)) {
      _currentasmNum++;
      ops.push_back(actual_operation{opi, operands});
    }
  }
  auto pgrm = std::make_unique<Program>(input, ops, name);
  pgrm->lineCorralation = lineCorralation();
  return std::move(pgrm);
}

void rga_2_disasm_compute::handleLineNum(const std::string &line) {
  std::cerr << "asm: LineNum: " << line << std::endl;
  const auto tokens = split<std::string>(line, ":");
  if (!tokens.empty() && tokens.size() > 1) {
    int src_ln = -1;
    try {
      src_ln = std::stoi(tokens[tokens.size() - 1]);
    } catch (...) {
      return;
    }
    _lineCorralations.emplace_back(src_ln, _currentasmNum);
  }
}

const bool rga_2_disasm_compute::parseASM(const std::string &input, const operation *&op_out,
                                          std::vector<operand> &oa_out) {
  const auto tokens = split<std::string>(input, " ,\t");

  if (!tokens.empty()) {
    const std::string opcode = tokens[0];
    if (opcode == ";") {
      handleLineNum(input);
      return false;
    }
    const auto opi = isa::getOpt(opcode);
    std::vector<operand> operands;
    for (size_t i = 1; i < tokens.size(); i++) {
      if (tokens[i] == "//") {
        break;
      }
      operands.emplace_back(parseOperand(tokens[i]));
    }
    actual_operation ret = {&opi, operands};
    validateOperation(ret);
    op_out = &opi;
    oa_out = std::move(operands);
    return true;
  }
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  op_out = &isa::getOpt(0);
  oa_out.emplace_back("nop");
  return true;
}

std::vector<std::tuple<uint8_t, uint8_t>> rga_2_disasm_compute::lineCorralation() { return _lineCorralations; }

const actual_operation rga_disasm_compute::parseASM(const std::string &input) {
  const auto tokens = split<std::string>(input, " ,");
  if (!tokens.empty()) {
    const std::string opcode = tokens[0];
    const auto opi = isa::getOpt(opcode);
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
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  const actual_operation nop = {&isa::getOpt(0), { operand("nop") }
};
  return nop;
}

std::unique_ptr<Program> rga_disasm_compute::parse(const std::string &input, const std::string &name) {
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

  return std::make_unique<Program>(input, ops, name);
}

std::unique_ptr<decoder::decoderT> find(const std::string &input) {
  std::unique_ptr<decoder::decoderT> b[] = {std::make_unique<decoder::rga_disasm_compute>(),
                                            std::make_unique<decoder::rga_2_disasm_compute>()};
  for (auto &d : b) {
    if (d->compatible(input)) {
      return std::move(d);
    }
  }
  throw std::invalid_argument("No suitable decoder for file");
}

std::vector<std::tuple<uint8_t, uint8_t>> decoderT::lineCorralation() {
  return std::vector<std::tuple<uint8_t, uint8_t>>();
}

} // namespace decoder
