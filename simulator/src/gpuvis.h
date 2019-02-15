#pragma once
#include <gpuvis_simulator_export.h>
#include <string>
#include <vector>



namespace gpuvis {
/// Return version string
GPUVIS_SIMULATOR_EXPORT const std::string version();

/// Load program, 
/// @return program ID, or 0 if Error

GPUVIS_SIMULATOR_EXPORT const uint16_t loadProgram(const std::string &pgrm, const std::string &name = "PROGRAM");
/// assigns source text to program.  
/// @throw std::invalid_argument on invalid pgrmid
GPUVIS_SIMULATOR_EXPORT void setSource(int pgrmid, const std::string &sourceString) noexcept(false);

/// Runs program in simulator
/// @return  0 On success, non 0 for error.
GPUVIS_SIMULATOR_EXPORT const int runProgram(int pgrmid);

/// Get simulation Summary as a json string
/// @return Json String
GPUVIS_SIMULATOR_EXPORT std::string summaryJSON(const std::vector<int>& pgrmids);

/// Get simulation Summary as a Mesagepack uint8_t array
/// @return std::vector<std::uint8_t> Messagepack array
GPUVIS_SIMULATOR_EXPORT std::vector<std::uint8_t> summaryMSGPK(const std::vector<int>& pgrmids);

} // namespace gpuvis
