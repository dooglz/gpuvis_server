#include "decoder.h"
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
class rga_disasm_compute : public decoderT {
  const std::string name() override { return "rga_disasm_compute"; }
  bool compatible(const std::string &input) override;
  std::unique_ptr<parser::Program> parse(const std::string &input) override;
  const actual_operation parseASM(const std::string &input);
};

bool rga_disasm_compute::compatible(const std::string &input) {
  return beginsWith(input, "ShaderType = IL_SHADER_COMPUTE");
}


const operand parseOperand(const std::string &input) {
  const auto tokens = split<std::string>(input, " ,");
  if (tokens.size() > 1) {
    operand ret = tokens[1];
    for (size_t i = 2; i < tokens.size(); i++)
    {
      ret += ("_"+tokens[i]);
    }
  }
  return "";
}

const actual_operation rga_disasm_compute::parseASM(const std::string &input) {
  const auto tokens = split<std::string>(input, " ,");
  if (!tokens.empty()) {
    const std::string opcode = tokens[0];
    for (auto &opi : ISA) {
      if (opi.opcode_str == opcode) {
          return { &opi, parseOperand(input) };
      }
    }
  }
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  const actual_operation nop = { &ISA[0], "NOP" };
  return nop;
}

std::unique_ptr<Program> rga_disasm_compute::parse(const std::string &input) {
  std::cout << "rga_disasm_compute parsing" << std::endl;
  std::vector<std::string> lines;

  std::string currentline = "";
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
  std::unique_ptr<decoder::decoderT> b[] = {std::make_unique<decoder::rga_disasm_compute>()};
  for (auto &d : b) {
    if (d->compatible(input)) {
      return std::move(d);
    }
  }
  return nullptr;
}
} // namespace decoder