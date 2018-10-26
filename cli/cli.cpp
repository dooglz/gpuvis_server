#include "../simulator/src/gpuvis.h"
#include "CLI11.hpp"
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
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

void inputFile(const std::string& ip, const std::string& source = "") {

  std::ifstream t(ip);
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  t.close();
  if (str == "") {
    throw std::runtime_error("Can't open input Asm! " + ip);
  }

  auto pgrm = gpuvis::loadProgram(str);
  if (pgrm == 0) {
    throw std::runtime_error("Problem loading Program");
  }

  if (source != "") {
    std::ifstream srct(source);
    std::string srcstr((std::istreambuf_iterator<char>(srct)),
                       std::istreambuf_iterator<char>());
    srct.close();
    if (srcstr == "") {
      throw std::runtime_error("Can't open source file! " + source);
    } else {
      auto ret = gpuvis::loadSource(pgrm, srcstr);
      if (ret != 0) {
        throw std::runtime_error("Problem loadSource");
      }
    }
  }

  auto gpu = gpuvis::initGPU();
  if (gpu == 0) {
    throw std::runtime_error("Problem initGPU");
  }

  auto res = gpuvis::runProgram(pgrm, gpu);
  if (res == 0) {
    throw std::runtime_error("Problem runProgram");
  }

  if (!mp) {
    auto json = gpuvis::summaryJSON(pgrm, gpu);
    if (json == "") {
      throw std::runtime_error("Problem json");
    }
    if (b64) {
      json = base64_encode(reinterpret_cast<const unsigned char*>(json.c_str()),
                           json.length());
    }
    if (outputfile != "") {
      std::ofstream fout(outputfile, std::ios::out | std::ios::binary);
      fout.write((char*)&json[0], json.size() * sizeof(char));
      fout.close();
    } else {
      std::cout << json << std::endl;
    }

  } else {
    auto msgpk = gpuvis::summaryMSGPK(pgrm, gpu);

    if (msgpk.empty()) {
      throw std::runtime_error("Problem msgpk");
    }
    if (outputfile != "") {
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

  CLI::App app("GPUVIS Server CLI");
  std::string file;
  std::string source = "";
  app.add_option("-f,--file,file", file, "input asm file")->required();
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
    inputFile(file, source);
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
