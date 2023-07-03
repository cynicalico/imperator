#ifndef BAPHY_UTIL_THREAD_POOL_HPP
#define BAPHY_UTIL_THREAD_POOL_HPP

#include "baphy/core/module_mgr.hpp"
#include "baphy/util/rnd.hpp"
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace baphy {

typedef std::function<void(double)> WaitFunc;
//typedef std::function<void(WaitFunc)> Job;

template<typename T>
concept JobFunc = std::invocable<T, WaitFunc>;

class Job {
public:
  Job() = default;

  virtual ~Job() = default;

  virtual void run(WaitFunc wait) = 0;
};

template<JobFunc F>
class JobWrap : public Job {
public:
  std::packaged_task<std::invoke_result_t<F, WaitFunc>(WaitFunc)> task_;

  JobWrap() : task_([]() {}) {}

  explicit JobWrap(F &&f) : task_(std::forward<F>(f)) {}

  auto get_future() {
    return task_.get_future();
  }

  void run(WaitFunc wait) override {
    task_(wait);
  }
};

template<typename T>
struct JobRes {
  std::string tag;
  std::future<T> f;

  bool avail() const {
    if (f.valid()) {
      auto status = f.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::ready)
        return true;
    }
    return false;
  }

  T get() {
    return f.get();
  }
};

struct JobCancelledException : public std::exception {
};

class ThreadPool : public Module<ThreadPool> {
public:
  ThreadPool();
  explicit ThreadPool(std::size_t pool_size);

  ~ThreadPool();

  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool &operator=(const ThreadPool &other) = delete;

  ThreadPool(ThreadPool &&other) = delete;
  ThreadPool &operator=(ThreadPool &&other) = delete;

  template<JobFunc F>
  JobRes<std::invoke_result_t<F, WaitFunc>>
  add_job(F &&f);

  template<JobFunc F>
  JobRes<std::invoke_result_t<F, WaitFunc>>
  add_job(const std::string &tag, F &&f);

  void cancel_job(const std::string &tag);
  void cancel_all();

  void shutdown();

private:
  std::queue<std::pair<std::string, std::unique_ptr<Job>>> jobs_{};

  std::mutex queue_mutex_;
  std::condition_variable take_a_job_;

  std::mutex wait_mapping_mutex_;
  std::unordered_map<std::string, std::size_t> wait_mapping_;

  std::vector<std::mutex> cancel_mutexes_;
  std::vector<std::condition_variable> cancel_cvs_;
  std::vector<bool> should_cancel_;

  bool terminate_pool_{false};

  std::size_t thread_count_{0};
  std::vector<std::jthread> threads_{};

  void initialize_pool_();

  void process_jobs_(std::size_t i);
};

template<JobFunc F>
JobRes<std::invoke_result_t<F, WaitFunc>>
ThreadPool::add_job(F &&f) {
  return add_job(base58(11), std::forward<F>(f));
}

template<JobFunc F>
JobRes<std::invoke_result_t<F, WaitFunc>>
ThreadPool::add_job(const std::string &tag, F &&f) {
  std::unique_lock<std::mutex> lock(queue_mutex_);
  auto jw = std::make_unique<JobWrap<F>>(std::forward<F>(f));
  auto fu = jw->get_future();
  jobs_.emplace(tag, std::move(jw));
  lock.unlock();
  take_a_job_.notify_one();

  return {tag, std::move(fu)};
}

} // namespace baphy

BAPHY_REGISTER_ENDPOINT(baphy::ThreadPool);

#endif//BAPHY_UTIL_THREAD_POOL_HPP
