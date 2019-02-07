#include "gpuvis.h"
#include "amdgpu_operations.h"
#include "output.h"
#include "program.h"
#include "simulator.h"
#include <string>
// git_revision built by pre-build step.
#define GIT_REVISION 000
#include "git_revision.h"
//

constexpr auto GPUVIS_VERISON = "0.1 ( GIT_REVISION )";

const std::string gpuvis::version() { return GPUVIS_VERISON; }

static std::map<uint16_t, std::unique_ptr<Program>> pgrm_db;
static uint16_t key = 0;

Program &findpgrm(int id) noexcept(false) {
  auto a = pgrm_db.find(id);
  if (a == pgrm_db.end()) {
    throw std::invalid_argument("Invalid ProgramID");
  }
  return *(a->second.get());
}

/// assigns source text to program.
void gpuvis::setSource(int pgrmid, const std::string &sourceString) {
  auto &p = findpgrm(pgrmid);
  p.source = sourceString;
}

/// Load program, returns Id. 0 == Error
const uint16_t gpuvis::loadProgram(const std::string &pgrm, const std::string &name) {
  auto a = Program::parse(pgrm, name);
  if (a != nullptr) {
    simulator::pgrmstats(*a);
    pgrm_db[++key] = std::move(a);
    return key;
  }
  return 0;
}

const int gpuvis::runProgram(int pgrmid, int gpuid) {
  bool result = simulator::run(findpgrm(pgrmid), gpuid);
  return (result ? 1 : 0);
}

const simulator::SimulationSummary summary(int pgrmid, int gpuid) {
  auto &p = findpgrm(pgrmid);
  simulator::SimulationSummary summary{{simulator::summary(p, gpuid)}, p.source};
  return summary;
}

const simulator::SimulationSummary summary(const std::vector<int> &pgrmids, int gpuid) {
  std::vector<simulator::ProgramSummary> ps;
  std::string source;
  for (auto pgm : pgrmids) {
    auto& p = findpgrm(pgm);
    ps.push_back(simulator::summary(p, gpuid));
    source = p.source;
  }
  simulator::SimulationSummary summary{ps, source};
  return summary;
}

std::string gpuvis::summaryJSON(const std::vector<int> &pgrmids, int gpuid) {
  return output::gimmyjson(summary(pgrmids, gpuid));
}

std::vector<std::uint8_t> gpuvis::summaryMSGPK(const std::vector<int> &pgrmids, int gpuid) {
  return output::gimmyMsgPack(summary(pgrmids, gpuid));
}

const int gpuvis::initGPU() { return simulator::init(); }
