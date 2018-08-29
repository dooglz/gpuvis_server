#include "simulator.h"
#include "parser.h"
#include <string>

const char *hello() { return "Hello Simulator World"; }

const bool loadProgram(const char *pgrm) {
  
  auto a  = parser::parse(std::string(pgrm));
   return true;

}