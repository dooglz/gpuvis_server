#pragma once
#include <memory>
#include <vector>
#include <string>
#include <tuple>
namespace parser {
class Program;
}
namespace decoder {

class decoderT {
public:
  virtual bool compatible(const std::string &input) = 0;
  virtual std::unique_ptr<parser::Program> parse(const std::string& input,const std::string& name) = 0;
  virtual const std::string name() = 0;
  virtual ~decoderT() = default;

private:
  virtual std::vector<std::tuple<uint8_t, uint8_t>> lineCorralation();
};

std::unique_ptr<decoder::decoderT> find(const std::string &input);
} // namespace decoder
