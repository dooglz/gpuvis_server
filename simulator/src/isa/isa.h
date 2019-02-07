#include <string>
#include <gpuvis_simulator_isa_fiji_export.h>

struct operation;

namespace isa {
GPUVIS_SIMULATOR_ISA_FIJI_EXPORT const operation &getOpt(const unsigned int opcode);
GPUVIS_SIMULATOR_ISA_FIJI_EXPORT const operation &getOpt(const std::string &opcode_string);
} // namespace isa