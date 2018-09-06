#pragma once
#include "parser.h"

namespace simulator {
bool run(const parser::Program &pgrm);
void pgrmstats(const parser::Program &pgrm);
} // namespace simulator
