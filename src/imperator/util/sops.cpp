#include "imperator/util/sops.hpp"

#include "fmt/format.h"
#include "fmt/ranges.h"
#include <sstream>

namespace imp {

void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

std::string ltrim_copy(std::string s) {
  ltrim(s);
  return s;
}

std::string rtrim_copy(std::string s) {
  rtrim(s);
  return s;
}

std::string trim_copy(std::string s) {
  trim(s);
  return s;
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

std::vector<std::string> split(const std::string &s, const std::string &delim) {
  std::vector<std::string> res{};

  std::size_t pos_start = 0, pos_end, delim_len = delim.length();
  std::string token;
  while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
    res.emplace_back(s.substr(pos_start, pos_end - pos_start));
    pos_start = pos_end + delim_len;
  }
  res.emplace_back(s.substr(pos_start));

  return res;
}

std::vector<std::string> split_re(const std::string &s, const RE2 &regexp) {
  std::vector<std::string> res;

  std::string_view sp(s);
  std::size_t pos = 0;

  std::string text_to_keep, match;
  while (RE2::Consume(&sp, regexp, &text_to_keep, &match)) {
    res.emplace_back(text_to_keep);
    pos += text_to_keep.size() + match.size();
  }
  res.emplace_back(s.substr(pos));

  return res;
}

} // namespace imp
