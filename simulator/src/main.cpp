#include "gpuvis.h"
#include "amdgpu_operations.h"
#include "output.h"
#include "program.h"
#include "simulator.h"
#include <string>
// git_revision built by pre-build step.
#define GIT_REVISION 000
#define GPUVIS_VERISON 0.1
#include "git_revision.h"
#define TO_STR2(x) #x
#define TO_STR(x) TO_STR2(x)
#define VERSION_STRING TO_STR(GPUVIS_VERISON) " (" TO_STR(GIT_REVISION) ")"
//


const std::string gpuvis::version() { return VERSION_STRING; }

std::map<uint16_t, std::unique_ptr<Program>> pgrm_db;
uint16_t key = 0;

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

const int gpuvis::runProgram(int pgrmid) {
  bool result = simulator::run(findpgrm(pgrmid));
  return (result ? 1 : 0);
}


std::string gpuvis::summaryJSON(const std::vector<int> &pgrmids) {
  return output::gimmyjson(pgrmids);
}

std::vector<std::uint8_t> gpuvis::summaryMSGPK(const std::vector<int> &pgrmids) {
  return output::gimmyMsgPack(pgrmids);
}