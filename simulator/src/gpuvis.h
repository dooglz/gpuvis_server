#pragma once
#include <gpuvis_simulator_export.h>
#include <string>
#include <vector>

namespace gpuvis {
GPUVIS_SIMULATOR_EXPORT const std::string hello();
GPUVIS_SIMULATOR_EXPORT const int loadProgram(const std::string &pgrm);
GPUVIS_SIMULATOR_EXPORT const int runProgram(int pgrmid, int gpuid);
GPUVIS_SIMULATOR_EXPORT std::string summaryJSON(int pgrmid, int gpuid);
GPUVIS_SIMULATOR_EXPORT std::vector<std::uint8_t> summaryMSGPK(int pgrmid, int gpuid);
GPUVIS_SIMULATOR_EXPORT const int initGPU();
} // namespace gpuvis