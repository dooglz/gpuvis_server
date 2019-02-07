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

GPUVIS_SIMULATOR_EXPORT const int runProgram(int pgrmid, int gpuid);
GPUVIS_SIMULATOR_EXPORT std::string summaryJSON(const std::vector<int>& pgrmids, int gpuid);
GPUVIS_SIMULATOR_EXPORT std::vector<std::uint8_t> summaryMSGPK(const std::vector<int>& pgrmids, int gpuid);
GPUVIS_SIMULATOR_EXPORT const int initGPU();
} // namespace gpuvis
