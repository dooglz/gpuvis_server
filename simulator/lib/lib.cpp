#include "lib.h"
#include "../src/simulator.h"
#include <gpuvis_simulator_export.h>
#include <iostream>
#include <string>

#if defined(_MSC_VER) // Microsoft compiler
#include <windows.h>
#elif defined(__GNUC__) // GNU compiler
#include <dlfcn.h>
#else
#error define your copiler
#endif

void *LoadSharedLibrary(char *pcDllname, int iMode = 2) {
  std::string sDllName = pcDllname;
#if defined(_MSC_VER) // Microsoft compiler
  sDllName += ".dll";
  return (void *)LoadLibrary(pcDllname);
#elif defined(__GNUC__) // GNU compiler
  sDllName = "./lib" + sDllName + ".so";
  auto ret = dlopen(sDllName.c_str(), iMode);
  if (!ret) {
    fprintf(stderr, "dlopen error %s\n", dlerror());
  }
  return ret;
#endif
}

void *GetFunction(void *Lib, char *Fnname) {
  std::cout << Fnname << std::endl;
#if defined(_MSC_VER) // Microsoft compiler
  return (void *)GetProcAddress(static_cast<HINSTANCE>(Lib), Fnname);
#elif defined(__GNUC__) // GNU compiler
  return dlsym(Lib, Fnname);
#endif
}

bool FreeSharedLibrary(void *hDLL) {
#if defined(_MSC_VER) // Microsoft compiler
  return FreeLibrary(static_cast<HINSTANCE>(hDLL));
#elif defined(__GNUC__) // GNU compiler
  return dlclose(hDLL);
#endif
}

#define GET(a) reinterpret_cast<a##_ptr *>(GetFunction(hDLL, #a))
#define CHECK(a)                                                                                                       \
  if (_##a == nullptr) {                                                                                               \
    std::cerr << "can't get function: " << #a << std::endl;                                                            \
    return;                                                                                                            \
  }
#define FUNC(a)                                                                                                        \
  _##a = GET(a);                                                                                                       \
  CHECK(a);

void *hDLL = NULL;

const void isLoaded() {
  if (hDLL == NULL) {
    hDLL = LoadSharedLibrary("gpuvis_simulator");
    if (hDLL == nullptr) {
      std::cerr << "Can't Load GPUVIS DLL!" << std::endl;
      throw std::runtime_error("Can't Load GPUVIS DLL!");
    }
  }
}

const std::string gpuvis::hello() {
  isLoaded();
  static hello_ptr* _hello = reinterpret_cast<hello_ptr *>(GetFunction(hDLL, "hello"))
  return std::string();
}

const bool gpuvis::loadProgram(const std::string &pgrm) {
  isLoaded();
  return false;
}
