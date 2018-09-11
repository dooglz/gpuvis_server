#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {

static std::map<uint16_t, std::unique_ptr<GPU>> gpu_db;
static uint16_t key = 0;

const int init() {
  std::unique_ptr<GPU> gpu = std::make_unique<GPU>(R9Fury);
  gpu->launchParameters({64, 1, 1});
  gpu_db[++key] = std::move(gpu);
  return key;
}

bool run(const parser::Program& pgrm, const int GPUID) {
  auto a = gpu_db.find(GPUID);
  if (a == gpu_db.end()) {
    throw("INVALID GPU ID");
    return false;
  }
  GPU& gpu = *a->second;
  auto op = pgrm.ops.begin();
  while (gpu.state == GPU::READY && op != pgrm.ops.end()) {
    gpu.tick(*op);
    op++;
  }
  if (gpu.state != GPU::END) {
    std::cerr << "Program execution fininished in error state! " << std::endl;
  }

  return true;
}

void pgrmstats(const parser::Program& pgrm) {
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

void summary(const parser::Program& pgrm, const int GPUID) {
  auto a = gpu_db.find(GPUID);
  if (a == gpu_db.end()) {
    throw("INVALID GPU ID");
  }
  GPU& gpu = *a->second;
  std::cout << "\n Memory Replay \n";
  auto regs = gpu.GetAllRegisters();
  const auto lasttick = gpu.tickcount;
  for (size_t i = 0; i < lasttick; i++) {
    std::cout << "---TICK " << i << "--- " << pgrm.ops[i].op->opcode_str << pgrm.ops[i]
              << "\n";
    for (auto r : regs) {
      bool print = false;
      // std::cout << r->_globalId << " -- " << idToStr(r->_globalId) << "\n";
      if (!r->reads[i].empty()) {
        std::cout << idToStr(r->_globalId) << "\t:R:";
        for (auto r : r->reads[i]) {
          std::cout << +r << ",";
        }
        print = true;
      }
      if (!r->writes[i].empty()) {
        if (!print) {
          std::cout << idToStr(r->_globalId) << "\t";
        }
        std::cout << " __ W:";
        for (auto w : r->writes[i]) {
          std::cout << +w << ",";
        }
        print = true;
      }
      if (print) {
        std::cout << "\n";
      }
    }
  }

  std::cout << "---" << std::endl;
}

} // namespace simulator
