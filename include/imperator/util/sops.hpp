#ifndef IMPERATOR_UTIL_SOPS_HPP
#define IMPERATOR_UTIL_SOPS_HPP

#include "re2/re2.h"
#include <string>
#include <vector>

namespace imp {

void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);

std::string ltrim_copy(std::string s);
std::string rtrim_copy(std::string s);
std::string trim_copy(std::string s);

std::vector<std::string> split(const std::string& s, char delim);
std::vector<std::string> split(const std::string& s, const std::string& delim);

// Macro for declaring regex splits to use with split_re
// A special form is required, the macro will do it without being as obtrusive
#define IMPERATOR_SPLIT_RE(x) RE2("(.+?)(" x ")")

/**
 * Split a given string on a regular expression
 * The regular expression *must* have the form:
 *   (.+?)(<pat>)
 * where <pat> is the expression to split on
 * To avoid mistakes, use the IMPERATOR_SPLIT_RE macro
 * If regexp does not have a match pattern, this will return an empty vector
 *
 * @param s the string to split
 * @param delim the regular expression to split on
 * @return a vector of substrings of s split on regexp
 */
std::vector<std::string> split_re(const std::string& s, const RE2& regexp);

} // namespace imp

#endif//IMPERATOR_UTIL_SOPS_HPP
