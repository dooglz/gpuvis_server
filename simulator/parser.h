#pragma once
#include <string>
#include <memory>
class Program
{
public:
  Program(const std::string& raw);
  ~Program();
  const std::string raw;
private:

};

std::unique_ptr<Program> Parse(const std::string& input);
