#ifndef MYCO_UTIL_SOPS_H
#define MYCO_UTIL_SOPS_H

#include "re2/re2.h"
#include <string>
#include <vector>

namespace myco {

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

std::string ltrim_copy(std::string s);
std::string rtrim_copy(std::string s);
std::string trim_copy(std::string s);

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> split(const std::string &s, const std::string &delim);

/**
 * Split a given string on a regular expression
 * regexp must be a regular expression of the form (...), where ... can be any valid re
 * If regexp does not have a match pattern, this will return an empty vector
 *
 * FIXME: This is likely very fragile and almost certainly will not work with anything but ASCII
 *  text--it is probably possible to make it work with generic UTF-8 but I'm already pushing the
 *  limits on how re2 works here
 *
 * @param s the string to split
 * @param regexp the regular expression to split on
 * @return a vector of substrings of s split on regexp
 */
std::vector<std::string> split(const std::string &s, const RE2 &regexp);

} // namespace myco

#endif//MYCO_UTIL_SOPS_H
