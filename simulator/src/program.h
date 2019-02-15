#pragma once
#include "amdgpu_operations.h"

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
namespace simulator{
struct SimulationSummary;
}
enum PROGRAM_SIMSTATE {
  NOTRUN,ERROR,RUNSUCCESS
};

class Program {
public:
  Program(std::string raw, std::vector<actual_operation> &_ops, const std::string &name);
  ~Program();
  //From user
  const std::string raw;                                     // raw input assembly
  const std::string name;                                    // raw input assembly
  std::string source;                                        // optional originating source code.
  //From parser
  std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation; // optional
  const std::vector<actual_operation> ops;
  //From simulator
  std::unique_ptr<simulator::SimulationSummary> simsum;
  PROGRAM_SIMSTATE simstate;

  /// finds a suitable parser for program input, and parses to a Program object
  /// @return nique_ptr of program object.
  /// @throw std::invalid_argument on invalid input
  static std::unique_ptr<Program> parse(const std::string &input, const std::string &name) noexcept(false);

private:
  Program(const Program &) = delete;
  Program() = delete;
};

extern std::map<uint16_t, std::unique_ptr<Program>> pgrm_db;
extern Program &findpgrm(int id) noexcept(false);