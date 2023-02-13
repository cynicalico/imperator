#include "fmt/format.h"
#include <chrono>
#include <list>
#include <vector>

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

  long long t1, t2, t3, t4;
  long long s;

  int m = rand() % 10;
  std::vector<long long> sums{};

  for (int n = 1000; n < 100000; n += 1000) {
    std::vector<int> v{};
    for (int i = 0; i < n + m; ++i)
      v.emplace_back(rand() % 10);
    t1 = time();
    s = 0;
    for (int i = 0; i < n + m; ++i)
      s += v[i];
    t2 = time();
    sums.emplace_back(s);

//    std::list<int> l{};
    auto l = IntrusizeLL<int>();
    for (int i = 0; i < n + m; ++i)
      l.add(rand() % 10);
    t3 = time();
    s = 0;
    auto curr = l.head;
    while (curr) {
      s += curr->v;
      curr = curr->next;
    }
//    for (const auto &e: l)
//      s += e;
    t4 = time();
    sums.emplace_back(s);

    fmt::print("{}\t{:.6f}\t{:.6f}\n", n, (t2 - t1) / 1e9, (t4 - t3) / 1e9);

    m = rand() % 10;
  }

  fmt::print("{}\n", sums[0]);
}