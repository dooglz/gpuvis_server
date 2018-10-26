#include "gpuvis.h"
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

const int gpuvis::loadProgram(const std::string& pgrm) {
  auto a = parser::parse(pgrm);
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
  return simulator::summary(*(a->second), gpuid);
}

std::string gpuvis::summaryJSON(int pgrmid, int gpuid) {

  return output::gimmyjson(summary(pgrmid, gpuid));
}

std::vector<std::uint8_t> gpuvis::summaryMSGPK(int pgrmid, int gpuid) {
  return output::gimmyMsgPack(summary(pgrmid, gpuid));
}
const int gpuvis::initGPU() { return simulator::init(); }
