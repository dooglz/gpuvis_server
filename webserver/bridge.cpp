#include "../simulator/simulator.h"
#include <chrono>
#include <iostream>
#include <string>

// const __declspec(dllimport) std::string hello();

#if defined(_MSC_VER) // Microsoft compiler
#include <windows.h>
#elif defined(__GNUC__) // GNU compiler
#include <dlfcn.h>
#else
#error define your copiler
#endif

/*
#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 4
*/

void *LoadSharedLibrary(char *pcDllname, int iMode = 2) {
  std::string sDllName = pcDllname;
#if defined(_MSC_VER) // Microsoft compiler
  sDllName += ".dll";
  return (void *)LoadLibrary(pcDllname);
#elif defined(__GNUC__) // GNU compiler
  sDllName = "./lib"+sDllName+".so";
  auto ret = dlopen(sDllName.c_str(), iMode);
  if (!ret) {
        fprintf(stderr, "dlopen error %s\n", dlerror());
  }
  return ret;
#endif
}
void *GetFunction(void *Lib, char *Fnname) {
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

void test() {
  std::cout << "woop" << std::endl;

  void *hDLL;

  hDLL = LoadSharedLibrary("gpuvis_simulator");
  if (hDLL == nullptr) {
    std::cerr << "can't Load Simulator Lib!" << std::endl;
    return;
  }

  auto AddFn = reinterpret_cast<hello_ptr *>(GetFunction(hDLL, "hello"));
  if (AddFn == nullptr) {
    std::cerr << "can't get function!" << std::endl;
    return;
  }
  std::cout << AddFn() << std::endl;
  std::cout << "Unload Lib" << std::endl;
  FreeSharedLibrary(hDLL);
  std::cout << "done" << std::endl;

}
