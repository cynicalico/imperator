#include "fmt/format.h"
#include "fmt/ranges.h"
#include "rapidcsv.h"
#include <chrono>
#include <filesystem>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

auto HERE = std::filesystem::path(__FILE__).parent_path();

template<typename T>
class PrioList {
public:
  PrioList() = default;

  void add(const std::string &name, T v, std::set<std::string> deps = {});

  std::string debug_stringify();

private:
  std::vector<std::tuple<std::string, T>> o_{};
  std::vector<std::set<std::string>> saved_deps_{};

  std::unordered_map<std::string, std::size_t> idx_{};

  std::unordered_map<std::string, std::set<std::string>> unmet_deps_{};
};

template<typename T>
void PrioList<T>::add(const std::string &name, T v, std::set<std::string> deps) {

}

template<typename T>
std::string PrioList<T>::debug_stringify() {
  std::string s = "[";
  std::size_t i = 0;
  for (const auto &e: o_) {
    if (i != 0)
      s += ", ";
    s += fmt::format("{}", e.first);

    i++;
  }
  return s + "]";
}

class Stopwatch {
public:
  Stopwatch() { start(); }

  void start() { start_ = time_(); }
  void stop() { end_ = time_(); }

  double elapsed_s() { return (end_ - start_) / 1e9; }

private:
  long long start_{}, end_{};

  long long time_() {
    using namespace std::chrono;

    auto now = steady_clock::now().time_since_epoch();
    return duration_cast<nanoseconds>(now).count();
  }
};

template<typename T>
void move_range(size_t start, size_t length, size_t dst, std::vector<T> & v)
{
  typename std::vector<T>::iterator first, middle, last;
  if (start < dst)
  {
    first  = v.begin() + start;
    middle = first + length;
    last   = v.begin() + dst;
  }
  else
  {
    first  = v.begin() + dst;
    middle = v.begin() + start;
    last   = middle + length;
  }
  std::rotate(first, middle, last);
}

int main(int, char *[]) {
//  auto p = PrioList<int>();
//  p.add("a", 0);
//  p.add("b", 0);
//  p.add("c", 0);

  std::vector<std::tuple<std::size_t, double, double, double>> times{};

  std::size_t N = 1'000'000;
  for (int i = 1000; i <= N; i += 1000) {
    std::vector<int> v1{};
    for (int j = 0; j < i; ++j)
      v1.emplace_back(rand() % 10);
    auto sw1 = Stopwatch();
    for (int j = 0; j < v1.size() - 1; ++j)
      std::swap(v1[j], v1[j + 1]);
    sw1.stop();

    std::vector<int> v2{};
    for (int j = 0; j < i; ++j)
      v2.emplace_back(rand() % 10);
    auto sw2 = Stopwatch();
    auto tmp = v2[0];
    std::copy(&v2[1], &v2[v2.size() - 1], &v2[0]);
    v2[v2.size() - 1] = tmp;
    sw2.stop();

    std::vector<int> v3{};
    for (int j = 0; j < i; ++j)
      v3.emplace_back(rand() % 10);
    auto sw3 = Stopwatch();
    move_range(1, v3.size()-2, 0, v3);
    sw3.stop();

    times.emplace_back(i, sw1.elapsed_s(), sw2.elapsed_s(), sw3.elapsed_s());

    fmt::print("\r{:10} / {:10} / {:10.2f}%", i, N, ((double)i / (double)N) * 100);
    std::fflush(stdout);
  }
  fmt::print("\n");

  auto d = rapidcsv::Document("", rapidcsv::LabelParams(-1, -1));
  d.SetRow<std::string>(0, {"Elements", "Swap", "Copy", "Rotate"});
  for (std::size_t i = 0; i < times.size(); ++i) {
    d.SetCell(0, i+1, std::get<0>(times[i]));
    d.SetCell(1, i+1, std::get<1>(times[i]));
    d.SetCell(2, i+1, std::get<2>(times[i]));
    d.SetCell(3, i+1, std::get<3>(times[i]));
  }
  d.Save(HERE / "out.csv");

  std::filesystem::current_path(HERE);
  std::system("python compare.py 3");
}