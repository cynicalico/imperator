#include "myco/util/helpers.hpp"
#include "myco/util/rnd.hpp"
#include "myco/util/time.hpp"
#include "priority.hpp"
#include "priority_v2.hpp"
#include "rapidcsv.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>

auto HERE = std::filesystem::path(__FILE__).parent_path();

int main(int, char *[]) {
//  std::unordered_map<std::string, std::set<std::string>> deps;
//  std::vector<std::string> items = {"a", "b", "c", "d", "e", "f", "g"};
//  for (std::size_t i = 0; i < items.size() - 1; ++i)
//    deps[items[i + 1]] = {items[i]};
//
//  std::string check_string = "[";
//  std::size_t i = 0;
//  for (const auto &e: items) {
//    if (i != 0)
//      check_string += ", ";
//    check_string += fmt::format("{}", e);
//
//    i++;
//  }
//  check_string += "]";
//
//  do {
////    fmt::print("{}\n", items);
////    std::fflush(stdout);
//    auto p = PrioList<int>();
//    for (const auto &i: items)
//      p.add(i, 0, deps[i]);
////    fmt::print("{}\n", p.debug_stringify());
////    std::fflush(stdout);
//    assert(p.debug_stringify() == check_string);
//  } while (std::next_permutation(items.begin(), items.end()));

  auto p = PrioList<int>();
  p.add("a", 0);
  p.add("c", 1, {"b"});
  p.add("b", 2, {"a"});

  fmt::print("{}\n", p.debug_stringify());

//  std::vector<std::tuple<std::size_t, double, double>> times{};
//
//  for (auto N = 10; N < 2'500; N += 100) {
//    std::vector<std::string> items{};
//    std::unordered_map<std::string, std::set<std::string>> deps{};
//
//    for (auto i = 0; i < N; ++i) {
//      std::string s(10, 0);
//      std::generate_n(s.begin(), 10, []() { return myco::get<char>(65, 90); });
//      items.emplace_back(s);
//    }
//    std::sort(items.begin(), items.end());
//    for (std::size_t i = 0; i < items.size() - 1; ++i)
//      deps[items[i + 1]] = {items[i]};
//
//    std::random_device rd;
//    std::mt19937 g(rd());
//
//    std::string check_string = "[";
//    for (const auto [i, e]: myco::enumerate(items)) {
//      if (i != 0)
//        check_string += ", ";
//      check_string += fmt::format("{}", e);
//    }
//    check_string += "]";
//    std::shuffle(items.begin(), items.end(), g);
//
//    auto sw1 = myco::Stopwatch();
//    auto p1 = PrioList<int>();
//    for (const auto &i: items)
//      p1.add(i, 0, deps[i]);
//    if (p1.debug_stringify() != check_string)
//      MYCO_LOG_ERROR("Bad {} {}", p1.debug_stringify(), check_string);
//    sw1.stop();
//
//    auto sw2 = myco::Stopwatch();
//    auto p2 = PrioTracker<int>();
//    for (const auto &i: items)
//      p2.add(i, 0, deps[i]);
//    if (p2.debug_stringify() != check_string)
//      MYCO_LOG_ERROR("Bad {} {}", p2.debug_stringify(), check_string);
//    sw2.stop();
//
//    fmt::print("{}\t{:.3}\t{:.3f}\n", N, sw1.elapsed_sec(), sw2.elapsed_sec());
//    std::fflush(stdout);
//
//    times.emplace_back(N, sw1.elapsed_sec(), sw2.elapsed_sec());
//  }
//
//  auto d = rapidcsv::Document("", rapidcsv::LabelParams(-1, -1));
//  d.SetRow<std::string>(0, {"Elements", "New", "Old"});
//  for (std::size_t i = 0; i < times.size(); ++i) {
//    d.SetCell(0, i + 1, std::get<0>(times[i]));
//    d.SetCell(1, i + 1, std::get<1>(times[i]));
//    d.SetCell(2, i + 1, std::get<2>(times[i]));
////    d.SetCell(3, i+1, std::get<3>(times[i]));
//  }
//  d.Save(HERE / "out.csv");
//
//  std::filesystem::current_path(HERE);
//  std::system("python compare.py 2");
}