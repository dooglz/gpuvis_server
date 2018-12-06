#include "gpuvis.h"
#include "isa/isa.h"
#include "output.h"
#include "parser.h"
#include "simulator.h"
#include <string>

const std::string gpuvis::hello() { return "Hello Simulator World"; }

static std::map<uint16_t, std::unique_ptr<parser::Program>> pgrm_db;
static uint16_t key = 0;

auto findpgrm(int id) {
  auto a = pgrm_db.find(id);
  if (a == pgrm_db.end()) {
    throw("Invalid ProgramID");
  }
  return a;
}

const int gpuvis::loadSource(int pgrmid, const std::string& src) {
  auto a = findpgrm(pgrmid);
  (a->second)->source = src;
  return 0;
}

const int gpuvis::loadProgram(const std::string& pgrm, const std::string& name) {
  auto a = parser::parse(pgrm,name);
  if (a != nullptr) {
    simulator::pgrmstats(*a);
    pgrm_db[++key] = std::move(a);
    return key;
  }
  return 0;
}

const int gpuvis::runProgram(int pgrmid, int gpuid) {
  auto a = findpgrm(pgrmid);
  bool result = simulator::run(*(a->second), gpuid);
  return (result ? 1 : 0);
}

const simulator::SimulationSummary summary(int pgrmid, int gpuid) {
  auto a = findpgrm(pgrmid);
  simulator::SimulationSummary summary{{simulator::summary(*(a->second), gpuid)},
                                       (a->second)->source};
  return summary;
}

const simulator::SimulationSummary summary(const std::vector<int>& pgrmids, int gpuid) {
  std::vector<simulator::ProgramSummary> ps;
  std::string source;
  for (auto pgm : pgrmids) {
    auto a = findpgrm(pgm);
    ps.push_back(simulator::summary(*(a->second), gpuid));
    source = (a->second)->source;
  }
  simulator::SimulationSummary summary{ps, source};
  return summary;
}

std::string gpuvis::summaryJSON(const std::vector<int>& pgrmids, int gpuid) {
  return output::gimmyjson(summary(pgrmids, gpuid));
}

std::vector<std::uint8_t> gpuvis::summaryMSGPK(const std::vector<int>& pgrmids,
                                               int gpuid) {
  return output::gimmyMsgPack(summary(pgrmids, gpuid));
}

const int gpuvis::initGPU() { return simulator::init(); }
