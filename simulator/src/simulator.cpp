#include "isa/isa.h"
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

RegisterEventTicks GetRegisterEventTicks(GPU& gpu) {
  std::map<size_t, std::vector<RegisterEvent>> events;
  const auto regs = gpu.GetAllRegisters();
  const auto lasttick = gpu.tickcount;
  for (size_t i = 0; i < lasttick; i++) {
    // std::cout << "---TICK " << i << "--- " << pgrm.ops[i].op->opcode_str  << "\n";
    for (auto r : regs) {
      std::string who = idToStr(r->_globalId);
      std::vector<uint8_t> reads = r->reads[i];
      std::vector<uint8_t> writes = r->writes[i];
      if (!reads.empty() || !writes.empty()) {
        events[i].push_back({who, reads, writes});
      }
    }
  }
  return events;
}

void GetRegisterActivity(GPU& gpu) {

}

SimulationSummary summary(const parser::Program& pgrm, const int GPUID) {
  auto a = gpu_db.find(GPUID);
  if (a == gpu_db.end()) {
    throw("INVALID GPU ID");
  }
  GPU& gpu = *a->second;
  return {GetRegisterEventTicks(gpu), pgrm.ops, pgrm.source, pgrm.lineCorralation};
}

} // namespace simulator
