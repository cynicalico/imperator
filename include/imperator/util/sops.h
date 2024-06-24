#ifndef IMPERATOR_UTIL_SOPS_H
#define IMPERATOR_UTIL_SOPS_H

#include "re2/re2.h"

#include <string>
#include <vector>

namespace imp {
void ltrim(std::string &s);

void rtrim(std::string &s);

void trim(std::string &s);

std::string ltrim_copy(std::string s);

std::string rtrim_copy(std::string s);

std::string trim_copy(std::string s);

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> split(const std::string &s, const std::string &delim);

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
 * @param regexp the regular expression to split on
 * @return a vector of substrings of s split on regexp
 */
std::vector<std::string> split_re(const std::string &s, const RE2 &regexp);

namespace internal {
template<typename R, typename F, typename... Args>
std::optional<R> sto_opt(F &&f, Args &&...args) {
    std::optional<R> v{};
    try {
        v = f(std::forward<Args>(args)...);
    } catch (const std::exception &e) {}
    return v;
}
} // namespace internal

template<typename T>
std::optional<double> stod(const T &str, size_t *idx = nullptr) {
    double (*f)(const T &, size_t *) = std::stod;
    return internal::sto_opt<double>(f, str, idx);
}

template<typename T>
std::optional<float> stof(const T &str, size_t *idx = nullptr) {
    float (*f)(const T &, size_t *) = std::stof;
    return internal::sto_opt<float>(f, str, idx);
}

template<typename T>
std::optional<int> stoi(const T &str, std::size_t *idx = nullptr, int base = 10) {
    int (*f)(const T &, size_t *, int) = std::stoi;
    return internal::sto_opt<int>(f, str, idx, base);
}

template<typename T>
std::optional<long> stol(const T &str, std::size_t *idx = nullptr, int base = 10) {
    long (*f)(const T &, size_t *, int) = std::stol;
    return internal::sto_opt<long>(f, str, idx, base);
}

template<typename T>
std::optional<long double> stold(const T &str, size_t *idx = nullptr) {
    long double (*f)(const T &, size_t *) = std::stold;
    return internal::sto_opt<long double>(f, str, idx);
}

template<typename T>
std::optional<long long> stoll(const T &str, std::size_t *idx = nullptr, int base = 10) {
    long long (*f)(const T &, size_t *, int) = std::stoll;
    return internal::sto_opt<long long>(f, str, idx, base);
}

template<typename T>
std::optional<unsigned long> stoul(const T &str, std::size_t *idx = nullptr, int base = 10) {
    unsigned long (*f)(const T &, size_t *, int) = std::stoul;
    return internal::sto_opt<unsigned long>(f, str, idx, base);
}

template<typename T>
std::optional<unsigned long long> stoull(const T &str, std::size_t *idx = nullptr, int base = 10) {
    unsigned long long (*f)(const T &, size_t *, int) = std::stoull;
    return internal::sto_opt<unsigned long long>(f, str, idx, base);
}
} // namespace imp

#endif //IMPERATOR_UTIL_SOPS_H
