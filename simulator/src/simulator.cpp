#include "amdgpu_operations.h"
#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {

bool run(const Program& pgrm) {

  auto gpu = std::make_unique<GPU>();
  gpu->launchParameters({ 64, 1, 1 });

  auto op = pgrm.ops.begin();
  while (gpu->state == GPU::READY && op != pgrm.ops.end()) {
    gpu->tick(*op);
    op++;
  }
  if (gpu->state != GPU::END) {
    std::cerr << "Program execution fininished in error state! " << std::endl;
  }

  return true;
}

void pgrmstats(const Program& pgrm) {
  std::cout << "---\nProgram Stats " << pgrm.name<<"\n";
  std::map<OPCODE_TYPE, size_t> opcount;
  size_t vregr = 0, vregw = 0, sregr = 0, sregw = 0;
  for (const auto op : pgrm.ops) {
    opcount[op.op->type]++;
    if (isVector(op.op->type)) {
      vregr += op.op->reads.size();
      vregw += op.op->writes.size();
    } else if (isScaler(op.op->type)) {
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

ProgramSummary summary(const Program& pgrm, const int GPUID) {
  auto a = gpu_db.find(GPUID);
  if (a == gpu_db.end()) {
    throw("INVALID GPU ID");
  }
  GPU& gpu = *a->second;
  return {pgrm.name,GetRegisterEventTicks(gpu), pgrm.ops, pgrm.lineCorralation};
}

} // namespace simulator
