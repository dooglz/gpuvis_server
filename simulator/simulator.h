#include <gpuvis_simulator_export.h>

#define EXP(a, b)                                                              \
  GPUVIS_SIMULATOR_EXPORT a;                                                   \
  typedef decltype(b) b##_ptr;

// Avoid C++ name mangling in dll
extern "C" {
// Would love a string, but C has none :(
EXP(const char *hello(), hello)
EXP(const bool loadProgram(const char* pgrm), loadProgram)
}
