#include "myco/util/helpers.hpp"
#include "myco/util/rnd.hpp"
#include "myco/util/time.hpp"
#include "priority_v1.hpp"
#include "priority_v2.hpp"
#include "priority_v3.hpp"
#include "priority_v4.hpp"
#include "priority_v5.hpp"
#include "rapidcsv.h"
#include <algorithm>
#include <filesystem>

auto HERE = std::filesystem::path(__FILE__).parent_path();

template<std::ranges::random_access_range R1,
         std::ranges::random_access_range R2,
         std::ranges::random_access_range R3>
void partial_shuffle_parallel(R1 &&r1, R2 &&r2, R3 &&r3, double percentage) {
  auto n = std::ranges::size(r1);
  while (n > 1) {
    n--;
    if (myco::get<bool>(percentage)) {
      auto k = myco::get(n + 1);
      std::swap(std::ranges::begin(r1)[k], std::ranges::begin(r1)[n]);
      std::swap(std::ranges::begin(r2)[k], std::ranges::begin(r2)[n]);
      std::swap(std::ranges::begin(r3)[k], std::ranges::begin(r3)[n]);
    }
  }
}

int main(int, char *[]) {
//  std::unordered_map<std::string, std::vector<std::string>> deps;
//  std::vector<std::string> items = {"a", "b", "c", "d", "e", "f", "g"};
//  deps[items[1]] = {items[0]};
//  deps[items[2]] = {items[1], items[0]};
//  for (std::size_t i = 2; i < items.size() - 1; ++i) {
//    deps[items[i + 1]] = {items[i], items[i - 1], items[i - 2]};
//  }
//
//  std::string check_string = "[";
//  for (const auto &[i, e]: myco::enumerate(items))
//    check_string += fmt::format("{}{}", i != 0 ? ", " : "", e);
//  check_string += "]";
//
//  do {
//    auto p = PrioListV4<int>();
//    for (const auto &i: items)
//      p.add(i, 0, deps[i]);
//    if (p.debug_stringify() != check_string)
//      MYCO_LOG_ERROR("Bad {} {}", p.debug_stringify(), check_string);
//  } while (std::next_permutation(items.begin(), items.end()));

  std::vector<std::tuple<std::size_t, double, double, double>> times{};

  for (auto N = 250; N <= 10'000; N += 250) {
    std::vector<std::string> items{};
    std::unordered_map<std::string, std::set<std::string>> deps_s{};
    std::unordered_map<std::string, std::vector<std::string>> deps_v{};

    for (auto i = 0; i < N; ++i) {
      std::string s(10, 0);
      std::generate_n(s.begin(), 10, []() { return myco::get<char>(65, 90); });
      items.emplace_back(s);
    }
    std::ranges::sort(items);
    deps_s[items[1]] = {items[0]};
    deps_v[items[1]] = {items[0]};
    deps_s[items[2]] = {items[1], items[0]};
    deps_v[items[2]] = {items[1], items[0]};
    for (std::size_t i = 2; i < items.size() - 1; i += 1) {
      deps_s[items[i + 1]] = {items[i], items[i - 1], items[i - 2]};
      deps_v[items[i + 1]] = {items[i], items[i - 1], items[i - 2]};
    }

    std::string check_string = "[";
    for (const auto &[i, e]: myco::enumerate(items))
      check_string += fmt::format("{}{}", i != 0 ? ", " : "", e);
    check_string += "]";
    myco::partial_shuffle(items, 0.25);

    auto sw2 = myco::Stopwatch();
    auto p2 = PrioListV3<int>();
    for (const auto &[i, e]: myco::enumerate(items))
      p2.add(e, 0, deps_s[e]);
    if (p2.debug_stringify() != check_string)
      MYCO_LOG_ERROR("Bad {} {}", p2.debug_stringify(), check_string);
    sw2.stop();

    auto sw3 = myco::Stopwatch();
    auto p3 = PrioListV4<int>();
    for (const auto &[i, e]: myco::enumerate(items))
      p3.add(e, 0, deps_v[e]);
    if (p3.debug_stringify() != check_string)
      MYCO_LOG_ERROR("Bad {} {}", p3.debug_stringify(), check_string);
    sw3.stop();

    auto sw4 = myco::Stopwatch();
    auto p4 = PrioListV5<int>();
    for (const auto &[i, e]: myco::enumerate(items))
      p4.add(e, 0, deps_v[e]);
    if (p4.debug_stringify() != check_string)
      MYCO_LOG_ERROR("Bad {} {}", p4.debug_stringify(), check_string);
    sw4.stop();

    fmt::print("{:<10} {:<10.3f} {:<10.3f} {:<10.3f}\n",
               N, sw2.elapsed_sec(), sw3.elapsed_sec(), sw4.elapsed_sec());
    std::fflush(stdout);

    times.emplace_back(N, sw2.elapsed_sec(), sw3.elapsed_sec(), sw4.elapsed_sec());
  }

  auto d = rapidcsv::Document("", rapidcsv::LabelParams(-1, -1));
  d.SetRow<std::string>(0, {"Elements", "V3", "V4", "V5"});
  for (std::size_t i = 0; i < times.size(); ++i) {
    d.SetCell(0, i + 1, std::get<0>(times[i]));
    d.SetCell(1, i + 1, std::get<1>(times[i]));
    d.SetCell(2, i + 1, std::get<2>(times[i]));
    d.SetCell(3, i + 1, std::get<3>(times[i]));
  }
  d.Save((HERE / "out.csv").string());

  std::filesystem::current_path(HERE);
  std::system("python compare.py 3");
}