#include "myco/util/log.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <chrono>
#include <list>
#include <set>
#include <unordered_map>

template<typename T>
class Prio {
  using List = std::list<std::pair<std::string, T>>;

public:
  Prio() = default;

  void add(const std::string &name, T v, std::set<std::string> deps = {});

  std::string debug_stringify();

private:
  List l_{};
  std::unordered_map<std::string, std::set<std::string>> saved_deps_{};
  std::unordered_map<std::string, std::set<std::string>> unmet_deps_{};
};

template<typename T>
void Prio<T>::add(const std::string &name, T v, std::set<std::string> deps) {
  saved_deps_[name] = deps;

  // Find our minimum position based on what is already in the list
  auto min_ins_it = l_.begin();
  for (auto it = l_.begin(); it != l_.end(); ++it) {
    if (deps.contains((*it).first)) {
      // Mark that we have found this one already
      deps.erase((*it).first);
      min_ins_it = it;
    }
  }

  // If we didn't find some dependencies, save them for later
  for (const auto &d: deps) {
    unmet_deps_[d].emplace(name);
  }

  MYCO_LOG_INFO("name: {} deps: {} unmet: {}", name, deps, unmet_deps_[name]);

  if (!unmet_deps_[name].empty()) {
    auto max_ins_it = l_.begin();
    for (auto it = l_.begin(); it != l_.end(); ++it) {

    }
  }

  if (min_ins_it != l_.begin()) {
    std::advance(min_ins_it, 1);
  }
  l_.insert(min_ins_it, {name, v});
}

template<typename T>
std::string Prio<T>::debug_stringify() {
  std::string s = "[";
  std::size_t i = 0;
  for (const auto &e: l_) {
    if (i != 0)
      s += ", ";
    s += fmt::format("{}", e.first);

    i++;
  }
  return s + "]";
}

int main(int, char *[]) {
  std::unordered_map<std::string, std::set<std::string>> deps;
  std::vector<std::string> items = {"a", "b", "c"};
  for (std::size_t i = 0; i < items.size() - 1; ++i)
    deps[items[i + 1]] = {items[i]};

  std::string check_string = "[";
  std::size_t i = 0;
  for (const auto &e: items) {
    if (i != 0)
      check_string += ", ";
    check_string += fmt::format("{}", e);

    i++;
  }
  check_string += "]";

  items = {"a", "c", "b"};

  do {
//    fmt::print("{}\n", items);
//    std::fflush(stdout);
    auto p = Prio<int>();
    for (const auto &i: items)
      p.add(i, 0, deps[i]);
    fmt::print("{}\n", p.debug_stringify());
    std::fflush(stdout);
    assert(p.debug_stringify() == check_string);
    break;
  } while (std::next_permutation(items.begin(), items.end()));
}