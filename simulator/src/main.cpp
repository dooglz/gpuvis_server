#include "gpuvis.h"
#include "parser.h"
#include "simulator.h"
#include <string>

const std::string gpuvis::hello() { return "Hello Simulator World"; }

static std::map<uint16_t, std::unique_ptr<parser::Program>> pgrm_db;
static uint16_t key = 0;

const int gpuvis::loadProgram(const std::string& pgrm) {
  auto a = parser::parse(pgrm);
  if (a != nullptr) {
    pgrm_db[++key] = std::move(a);
    return key;
  }
  return 0;
}

const int gpuvis::runProgram(int pgrmid) {
  auto a = pgrm_db.find(pgrmid);
  if (a != pgrm_db.end()) {
    bool result = simulator::run(*(a->second));
    return (result ? 1 : 0);
  }
  return 0;
}
