#include "decoder.h"
#include "../isa/fiji.h"
#include "../parser.h"
#include <algorithm>
#include <iostream>
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
    if (pos != start) // ignore empty tokens
      v.emplace_back(str, start, pos - start);
    start = pos + 1;
    pos = str.find_first_of(delimiters, start);
  }
  if (start < str.length())                           // ignore trailing delimiter
    v.emplace_back(str, start, str.length() - start); // add what's left of the string
  return v;
}

namespace decoder {
class rga_disasm_compute : public decoderT {
  const std::string name() { return "rga_disasm_compute"; }
  bool compatible(const std::string &input);
  std::unique_ptr<parser::Program> parse(const std::string &input);
  const operation *parseASM(const std::string &input);
};

bool rga_disasm_compute::compatible(const std::string &input) {
  return beginsWith(input, "ShaderType = IL_SHADER_COMPUTE");
}

const operation *rga_disasm_compute::parseASM(const std::string &input) {
  const auto tokens = split<std::string>(input, " ,");
  if (tokens.size() > 0) {
    const std::string opcode = tokens[0];
    for (auto &opi : ISA) {
      if (opi.opcode_str == opcode) {
        return &opi;
      }
    }
  }
  std::cerr << "asm: UNKOWN: " << input << std::endl;
  return nullptr;
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
  std::vector<const operation *> ops;
  while (++fs != fe) {
    auto op = parseASM(*fs);
    if (op != nullptr) {
      ops.push_back(op);
    } else {
      ops.push_back(&ISA[0]);
    }
  }
  return std::unique_ptr<Program>(new Program(input, ops));
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