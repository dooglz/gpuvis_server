#pragma once
#include <memory>
#include <string>
namespace parser {
class Program;
}
namespace decoder {

class decoderT {
public:
  virtual bool compatible(const std::string& input) = 0;
  virtual std::unique_ptr<parser::Program> parse(const std::string& input) = 0;
  virtual const std::string name() = 0;
};

std::unique_ptr<decoder::decoderT> find(const std::string& input);
} // namespace decoder
