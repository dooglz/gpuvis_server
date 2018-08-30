#pragma once
#include <memory>
#include <string>
namespace parser {
class Program {
public:
  Program(const std::string &raw);
  ~Program();
  const std::string raw;

private:
};

std::unique_ptr<Program> parse(const std::string &input);
} // namespace parser