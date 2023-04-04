#include "myco/util/io.hpp"
#include <fstream>

namespace myco {

std::string read_file_to_string(const std::filesystem::path &path) {
  std::ifstream t(path.string());
  std::string str;

  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)),
             std::istreambuf_iterator<char>());

  return str;
}

} // namespace myco
