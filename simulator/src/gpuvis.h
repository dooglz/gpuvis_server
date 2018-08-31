#pragma once
#include <gpuvis_simulator_export.h>
#include <string>

namespace gpuvis {
GPUVIS_SIMULATOR_EXPORT const std::string hello();
GPUVIS_SIMULATOR_EXPORT const int loadProgram(const std::string &pgrm);
GPUVIS_SIMULATOR_EXPORT const int runProgram(int pgrmid);

} // namespace gpuvis