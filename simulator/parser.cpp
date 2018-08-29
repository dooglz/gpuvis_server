#include "parser.h"
#include <iostream>
#include <vector>
#include <algorithm>
#define beginsWith(a,b) (a.find(b, 0) == 0)
#define FAIL(a) std::cerr << "Invalid program file:\n" << #a << std::endl; return nullptr;
using namespace parser;

Program::Program(const std::string &raw) : raw(raw) { std::cout << "Program Constructor!" << std::endl; }

Program::~Program() {}


static void parseASM(const std::string &input) {
  std::cout << "asm: " << input << std::endl;
}

std::unique_ptr<Program> parser::parse(const std::string &input) {
  if (!beginsWith(input,"ShaderType = IL_SHADER_COMPUTE")) {
    FAIL(input.substr(0, 10));
  }

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

  auto fs = std::find_if(lines.begin(), lines.end(), [](std::string x) {return beginsWith(x, "shader main"); });

  if (fs == lines.end()) {
    FAIL("No ASM!");
  }

  auto fe = std::find_if(fs, lines.end(), [](std::string x) {return beginsWith(x, "end"); });
  if (fe == lines.end()) {
    FAIL("Too Much ASM!");
  }
  while (++fs != fe) {
    parseASM(*fs);
  }

  return std::unique_ptr<Program>(new Program(input));
}
