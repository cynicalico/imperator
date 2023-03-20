#include "myco/util/thread_pool.hpp"

namespace myco {

ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency() - 1) {}

ThreadPool::ThreadPool(std::size_t pool_size) : thread_count_(pool_size) {
  initialize_pool_();
}

ThreadPool::~ThreadPool() {
  if (!threads_.empty())
    shutdown();
}

void ThreadPool::cancel_job(const std::string &tag) {
  {
    std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
    auto it = wait_mapping_.find(tag);
    if (it != wait_mapping_.end()) {
      should_cancel_[it->second] = true;
      cancel_cvs_[it->second].notify_all();
    }
  }
}

void ThreadPool::cancel_all() {
  {
    std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
    for (const auto &[tag, idx]: wait_mapping_) {
      should_cancel_[idx] = true;
      cancel_cvs_[idx].notify_all();
    }
  }
}

void ThreadPool::shutdown() {
  cancel_all();

  terminate_pool_ = true;
  take_a_job_.notify_all();

  for (auto &t: threads_)
    t.join();

  threads_.clear();
}

void ThreadPool::initialize_pool_() {
  for (std::size_t i = 0; i < thread_count_; ++i)
    threads_.emplace_back(&ThreadPool::process_jobs_, this, i);

  cancel_mutexes_ = std::vector<std::mutex>(thread_count_);
  cancel_cvs_ = std::vector<std::condition_variable>(thread_count_);
  should_cancel_ = std::vector<bool>(thread_count_);
}

void ThreadPool::process_jobs_(std::size_t i) {
  while (true) {
    std::pair<std::string, std::unique_ptr<Job>> j;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      take_a_job_.wait(lock, [this]() {
        return !jobs_.empty() || terminate_pool_;
      });
      if (terminate_pool_)
        break;

      j = std::move(jobs_.front());
      jobs_.pop();
    }
    {
      std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
      if (wait_mapping_.contains(j.first)) {
        auto curr_i = wait_mapping_[j.first];
        should_cancel_[curr_i] = true;
        cancel_cvs_[curr_i].notify_all();
      }
      wait_mapping_[j.first] = i;
      should_cancel_[i] = false;
    }

    // FIXME: I'm not a huge fan of using exceptions in any of this code
    //  because generally exceptions are "slow", but I genuinely can't think
    //  of any better way to do this. I'd like to not impose the user having
    //  to put an explicit return in the job functions that are being passed.
    try {
      using namespace std::chrono;

      j.second->run([&](double wait_time) {
        // FIXME: This has a really coarse resolution, especially when compiled with MSVC,
        //  and shouldn't be relied on for anything precise
        //  The `every` timers are much more precise if you need good repeat timing set up
        auto target_time = steady_clock::now() + duration<double>(wait_time);

        std::unique_lock<std::mutex> lock(cancel_mutexes_[i]);
        cancel_cvs_[i].wait_until(lock, target_time, [&] {
          return should_cancel_[i];
        });

        if (should_cancel_[i])
          throw JobCancelledException();
      });
    } catch (JobCancelledException &) { /* do nothing */ }

    {
      std::unique_lock<std::mutex> lock(wait_mapping_mutex_);
      auto it = wait_mapping_.find(j.first);
      if (it->second == i) {
        should_cancel_[it->second] = false;
        wait_mapping_.erase(it);
      }
    }
  }
}

} // namespace myco
