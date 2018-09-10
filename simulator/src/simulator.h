#pragma once
#include "parser.h"

namespace simulator {
const int init();
bool run(const parser::Program &pgrm, int GPUID);
void pgrmstats(const parser::Program &pgrm);
void summary(const parser::Program &pgrm, int GPUID);
} // namespace simulator
