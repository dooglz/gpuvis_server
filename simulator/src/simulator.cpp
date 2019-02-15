#include "amdgpu_operations.h"
#include "simulator.h"
#include "gpu/gpu.h"
#include <iostream>

namespace simulator {
auto gpu = std::make_unique<GPU>();

bool run(Program& pgrm) {
  gpu.reset(new GPU());
  gpu->launchParameters({ 64, 1, 1 });

  auto op = pgrm.ops.begin();
  while (gpu->state == GPU::READY && op != pgrm.ops.end()) {
    gpu->tick(*op);
    op++;
  }

  if (gpu->state != GPU::END) {
    pgrm.simstate = ERROR;
    std::cerr << "Program execution fininished in error state! " << std::endl;
    return false;
  }
  pgrm.simstate = RUNSUCCESS;
  pgrm.simsum.reset(new SimulationSummary(*gpu, pgrm));
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

RegisterEventTicks GetRegisterEventTicks(const GPU& gpu) {
  std::map<size_t, std::vector<RegisterEvent>> events;
  const auto regs = gpu.GetAllRegisters();
  const auto lasttick = gpu.tickcount;
  for (size_t i = 0; i < lasttick; i++) {
    // std::cout << "---TICK " << i << "--- " << pgrm.ops[i].op->opcode_str  << "\n";
    for (auto r : regs) {
      std::string who = idToStr(r->_globalId);
      std::vector<uint8_t> reads(r->reads.count(i) > 0 ? r->reads.at(i) : std::vector<uint8_t>());
      std::vector<uint8_t> writes(r->writes.count(i) > 0 ? r->writes.at(i) : std::vector<uint8_t>());
      if (!reads.empty() || !writes.empty()) {
        events[i].push_back({who, reads, writes});
      }
    }
  }
  return events;
}

void GetRegisterActivity(GPU& gpu) {

}
/*
ProgramSummary summary(const Program& pgrm) {
  return {pgrm.name,GetRegisterEventTicks(*gpu), pgrm.ops, pgrm.lineCorralation};
}

const SimulationSummary summary(int pgrmid) {
  auto &p = findpgrm(pgrmid);
  simulator::SimulationSummary 
  return summary;
}

const SimulationSummary summary(const std::vector<int> &pgrmids) {
  std::vector<simulator::ProgramSummary> ps;
  std::string source;
  for (auto pgm : pgrmids) {
    auto& p = findpgrm(pgm);
    ps.push_back(simulator::summary(p));
    source = p.source;
  }
  simulator::SimulationSummary summary{ ps, source };
  return summary;
}

*/

SimulationSummary::SimulationSummary(const GPU & gpu, const Program & pgrm):registerEventTicks(GetRegisterEventTicks(gpu))
{

}

} // namespace simulator
