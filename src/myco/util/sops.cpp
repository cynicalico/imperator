#include "myco/util/sops.hpp"
#include "myco/util/log.hpp"
#include <sstream>

namespace myco {

std::vector<std::string> split(const std::string &s, const std::string &delim) {
  std::vector<std::string> res;

  size_t pos_start = 0, pos_end, delim_len = delim.length();
  std::string token;
  while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }
  res.push_back(s.substr(pos_start));

  return res;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> res;

  std::stringstream ss(s);
  std::string item;
  while (getline(ss, item, delim))
    res.push_back(item);

  // This happens when there's a delimiter at the end with nothing after it
  if (item.empty())
    res.push_back(item);

  return res;
}

} // namespace myco
