#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {

bool run(const parser::Program &pgrm) {
  GPU gpu = GPU(R9Fury);
  gpu.launchParameters({64, 1, 1});

  auto op = pgrm.ops.begin();
  while (gpu.state == gpu.READY && op != pgrm.ops.end()) {
    gpu.tick(*op);
    op++;
  }
  if (gpu.state != gpu.END) {
    std::cerr << "Program execution fininished in error state! " << std::endl;
  }

  return true;
}

void pgrmstats(const parser::Program &pgrm) {
  std::cout << "---\nProgram Stats\n";
  std::map<opcode_type, size_t> opcount;
  size_t vregr = 0, vregw = 0, sregr = 0, sregw = 0;
  for (const auto op : pgrm.ops) {
    opcount[op.op->type]++;
    if (op.op->type == VECTOR) {
      vregr += op.op->reads.size();
      vregw += op.op->writes.size();
    } else if (op.op->type == SCALER) {
      sregr += op.op->reads.size();
      sregw += op.op->writes.size();
    }
  }

  for (auto elem : opcount) {
    std::cout << opcode_type_string[elem.first] << ": " << elem.second << "\n";
  }
  std::cout << "Vector Register Reads: " << vregr << "\n";
  std::cout << "Vector Register Writes: " << vregw << "\n";
  std::cout << "Scaler Register Reads: " << sregr << "\n";
  std::cout << "Scaler Register Writes: " << sregw << "\n";
  std::cout << "---" << std::endl;
}

} // namespace simulator
