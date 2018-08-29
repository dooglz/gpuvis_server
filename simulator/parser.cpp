#include "parser.h"

Program::Program(const std::string& raw) : raw(raw)
{

}

Program::~Program()
{
}

std::unique_ptr<Program> Parse(const std::string & input)
{
  return std::unique_ptr<Program>();
}
