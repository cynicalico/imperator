#pragma once

#include "myco/util/rnd.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace myco {

typedef std::function<void(double)> WaitFunc;
typedef std::function<void(WaitFunc)> Job;

struct JobCancelledException : public std::exception {};

class ThreadPool {
public:
  ThreadPool();
  explicit ThreadPool(std::size_t pool_size);

  ~ThreadPool();

  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool &operator=(const ThreadPool &other) = delete;

  ThreadPool(ThreadPool &&other) = delete;
  ThreadPool &operator=(ThreadPool &&other) = delete;

  template<typename T>
  std::string add_job(T &&f);

  template<typename T>
  std::string add_job(const std::string &tag, T &&f);

  void cancel_job(const std::string &tag);
  void cancel_all();

  void shutdown();

private:
  std::queue<std::pair<std::string, Job>> jobs_{};

  std::mutex queue_mutex_;
  std::condition_variable take_a_job_;

  std::mutex wait_mapping_mutex_;
  std::unordered_map<std::string, std::size_t> wait_mapping_;

  std::vector<std::mutex> cancel_mutexes_;
  std::vector<std::condition_variable> cancel_cvs_;
  std::vector<bool> should_cancel_;

  bool terminate_pool_{false};

  std::size_t thread_count_{0};
  std::vector<std::thread> threads_{};

  void initialize_pool_();

  void process_jobs_(std::size_t i);
};

template<typename T>
std::string ThreadPool::add_job(T &&f) {
  return add_job(base58(11), std::forward<T>(f));
}

template<typename T>
std::string ThreadPool::add_job(const std::string &tag, T &&f) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    jobs_.emplace(tag, std::forward<T>(f));
  }
  take_a_job_.notify_one();

  return tag;
}

} // namespace myco
