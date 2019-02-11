#include "../simulator/src/gpuvis.h"
#include "CLI11.hpp"
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN64
// for win 10 , add reg value: Auto(REG_DWORD) = 1, to:
// HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug
#define sigtrap __debugbreak()
#else
#define void()
#endif


bool mp, b64;
std::string outputfile;

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}

void inputFile(const std::vector<std::string>& ip, const std::string& source = "") {

  struct inputprogram{
    std::string filepath;
    std::string name;
    std::string contents;
    int program;
  };
  //sperate helper list of all program ids.
  std::vector<int> programs;

  std::vector<inputprogram> inputprograms;
  for (auto ipi : ip) {
    std::ifstream t(ipi);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    t.close();
    if (str.empty()) {
      throw std::runtime_error("Can't open input Asm! " + ipi);
    }
    inputprograms.push_back({ipi, ipi.substr(ipi.find_last_of("/\\")+1,32),str,0});
  }

  std::string programSourceString;
  if (!source.empty()) {
    std::ifstream srct(source);
    programSourceString = std::string((std::istreambuf_iterator<char>(srct)),
                                      std::istreambuf_iterator<char>());
    srct.close();
    if (programSourceString.empty()) {
      throw std::runtime_error("Can't open source file! " + source);
    }
  }

  for (auto& p : inputprograms) {
    p.program = gpuvis::loadProgram(p.contents, p.name);
    if (p.program == 0) {
      throw std::runtime_error("Problem loading Program" + p.name);
    }
    programs.push_back(p.program);
    if (!programSourceString.empty()) {
      gpuvis::setSource(p.program, programSourceString);
    }
  }

  auto gpu = gpuvis::initGPU();
  if (gpu == 0) {
    throw std::runtime_error("Problem initGPU");
  }

  for (auto& p : inputprograms) {
    auto res = gpuvis::runProgram(p.program, gpu);
    if (res == 0) {
      throw std::runtime_error("Problem runProgram");
    }
  }

  if (!mp) {
    auto json = gpuvis::summaryJSON(programs, gpu);
    if (json.empty()) {
      throw std::runtime_error("Problem json");
    }
    if (b64) {
      json = base64_encode(reinterpret_cast<const unsigned char*>(json.c_str()),
                           json.length());
    }
    if (!outputfile.empty()) {
      std::ofstream fout(outputfile, std::ios::out | std::ios::binary);
      fout.write((char*)&json[0], json.size() * sizeof(char));
      fout.close();
    } else {
      std::cout << json << std::endl;
    }

  } else {
    auto msgpk = gpuvis::summaryMSGPK(programs, gpu);

    if (msgpk.empty()) {
      throw std::runtime_error("Problem msgpk");
    }
    if (!outputfile.empty()) {
      std::ofstream fout(outputfile, std::ios::out | std::ios::binary);
      if (b64) {
        std::string b64s = base64_encode(
            reinterpret_cast<const unsigned char*>(&msgpk[0]), msgpk.size());
        fout.write((char*)&b64s[0], b64s.size() * sizeof(char));
      } else {
        fout.write((char*)&msgpk[0], msgpk.size() * sizeof(uint8_t));
      }

      fout.close();
    } else {
      if (b64) {
        std::string b64s = base64_encode(
            reinterpret_cast<const unsigned char*>(&msgpk[0]), msgpk.size());
        std::cout << b64s << std::endl;
      } else {
        for (size_t i = 0; i < msgpk.size(); i++) {
          if (i > 0) {
            std::cout << ",";
          }
          std::cout << +msgpk[i];
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  
  sigtrap;

  CLI::App app("GPUVIS Server CLI");
  std::vector<std::string> files;
  std::string source = "";
  app.add_option("-f,--files,file", files, "input asm file(s)")->required();
  app.add_option("-o,--output", outputfile, "output file, default stdout");
  app.add_option("-s,--source", source, "input source file [needs correct asm]");
  app.add_flag("-m,--messagepack,", mp, "Output in Encoded Messagepack, default JSON");
  app.add_flag("-b,--b64,", b64, "Base64 Encode Output");

  CLI11_PARSE(app, argc, argv);



  /*
  mp = true;
  b64 = true;
  outputfile = "output/mp_b64.txt";
  */
  try {
    inputFile(files, source);
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
