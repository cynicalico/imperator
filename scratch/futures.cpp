#include "myco/util/time.hpp"
#include "fmt/format.h"
#include <concepts>
#include <future>
#include <thread>

template<typename T>
concept JobFunc = std::invocable<T>;

class Job {
public:
  Job() = default;
  virtual ~Job() = default;

  virtual void run() = 0;
};

template<JobFunc F>
class JobWrap : public Job {
public:
  std::packaged_task<std::invoke_result_t<F>()> task_;

  JobWrap() : task_([](){}) {}

  explicit JobWrap(F &&f) : task_(std::forward<F>(f)) {}

  auto get_future() {
    return task_.get_future();
  }

  void run() override {
    task_();
  }
};

class JobMgr {
  Job *j_{nullptr};

public:
  template<JobFunc F>
  auto add(F &&f) {
    auto jw = new JobWrap(std::forward<F>(f));
    auto fu = jw->get_future();
    j_ = std::move(jw);
    return fu;
  }

  void run() {
    j_->run();
  }
};

int main(int, char *[])  {
  int n = 0;
  auto m = JobMgr();
  auto f = m.add([&](){
    fmt::print("A side effect!\n");
    return 5;
  });

  m.run();
  f.wait();

  fmt::print("Future: {}\n", f.get());

  m.add([](){});
  m.run();
}