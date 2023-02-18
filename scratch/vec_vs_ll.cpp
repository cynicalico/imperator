#include "fmt/format.h"
#include "rapidcsv.h"
#include <chrono>
#include <list>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <tuple>

auto HERE = std::filesystem::path(__FILE__).parent_path();

long long time() {
  using namespace std::chrono;

  auto now = steady_clock::now().time_since_epoch();
  return duration_cast<nanoseconds>(now).count();
}

template<typename T>
class IntrusizeLL {
  struct node_ {
    T v{T()};

    node_ *prev{nullptr};
    node_ *next{nullptr};
  };

public:
  std::size_t size{0};
  node_ *head{nullptr};
  node_ *tail{nullptr};

  IntrusizeLL() = default;

  void add(T v) {
    if (!head) {
      head = new node_{v, nullptr, nullptr};
      tail = head;

    } else if (size == 1) {
      head->next = new node_{v, head, nullptr};
      tail = head->next;

    } else {
      tail->next = new node_{v, tail, nullptr};
      tail = tail->next;
    }

    size++;
  }

  void print() {
    auto curr = head;
    while (curr) {
      fmt::print("{} ", curr->v);
      curr = curr->next;
    }
    fmt::print("\n");
  }
};

int main() {
//  auto l = IntrusizeLL<int>();
//  l.add(1);
//  l.add(2);
//  l.add(3);
//  l.print();

  long long t1, t2, t3, t4, t5, t6, t7, t8;
  long long s;

  int m = rand() % 10;
  std::vector<long long> sums{};
  std::vector<std::tuple<std::size_t, double, double, double, double>> times{};

  for (int n = 10; n < 1000; n += 10) {
    std::vector<int> v{};
    t5 = time();
    for (int i = 0; i < n + m; ++i)
      v.insert(v.begin() + (v.size() > 0 ? (rand() % v.size()) : 0), rand() % 10);
    t6 = time();
    t1 = time();
    s = 0;
    for (int i = 0; i < n + m; ++i)
      s += v[i];
    t2 = time();
    sums.emplace_back(s);

    std::list<int> l{};
//    auto l = IntrusizeLL<int>();
    t7 = time();
    for (int i = 0; i < n + m; ++i) {
      auto pos = (l.size() > 0 ? (rand() % l.size()) : 0);
      auto it = l.begin();
      std::advance(it, pos);
      l.insert(it, rand() % 10);
    }
    t8 = time();
    t3 = time();
    s = 0;
//    auto curr = l.head;
//    while (curr) {
//      s += curr->v;
//      curr = curr->next;
//    }
    for (const auto &e: l)
      s += e;
    t4 = time();
    sums.emplace_back(s);

    fmt::print("{}\t{:.6f}\t{:.6f}\t{:.6f}\t{:.6f}\n", n, (t6 - t5) / 1e9, (t8 - t7) / 1e9, (t2 - t1) / 1e9, (t4 - t3) / 1e9);
    times.emplace_back(n, (t6 - t5) / 1e9, (t8 - t7) / 1e9, (t2 - t1) / 1e9, (t4 - t3) / 1e9);

    m = rand() % 10;
  }

  // So we don't optimize it out
  fmt::print("{}\n", sums[0]);

  auto d = rapidcsv::Document("", rapidcsv::LabelParams(-1, -1));
  d.SetRow<std::string>(0, {"Elements", "VectorIns", "LLIns", "VectorIter", "LLIter"});
  for (std::size_t i = 0; i < times.size(); ++i) {
    d.SetCell(0, i+1, std::get<0>(times[i]));
    d.SetCell(1, i+1, std::get<1>(times[i]));
    d.SetCell(2, i+1, std::get<2>(times[i]));
    d.SetCell(3, i+1, std::get<3>(times[i]));
    d.SetCell(4, i+1, std::get<4>(times[i]));
  }
  d.Save(HERE / "out.csv");

  std::filesystem::current_path(HERE);
  std::system("python vec_vs_ll_grapher.py");
}