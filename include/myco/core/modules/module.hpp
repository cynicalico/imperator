#pragma once

#include "myco/core/scheduler.hpp"
#include <mutex>
#include <string>

namespace myco {

class Engine;

template<typename T>
struct ModuleInfo;

class ModuleI {
public:
  std::string name;

  virtual ~ModuleI() = default;
};

template<typename T>
class Module : public ModuleI {
  friend class Engine;

public:
  std::shared_ptr<Engine> engine;

  Module() : Module(std::vector<std::string>{}) {}

  explicit Module(std::vector<std::string> &&dependencies) : ModuleI() {
    name = ModuleInfo<T>::name;
    Scheduler::sub<Initialize>(
        name,
        std::forward<std::vector<std::string>>(dependencies),
        [&](const auto &e) {
          std::call_once(is_initialized_, [&](){ initialize_(e); });
        });
    Scheduler::sub<ReleaseEngine>(name, [&](const auto &) { release_engine_(); });

    MYCO_LOG_DEBUG("Initialized {}", name);
  }

  ~Module() override {
    MYCO_LOG_DEBUG("{} destroyed", name);
  }

protected:
  virtual void initialize_(const Initialize &e);

  void release_engine_();

private:
  std::once_flag is_initialized_;
};

template<typename T>
void Module<T>::initialize_(const Initialize &e) {
  engine = e.engine;
}

template<typename T>
void Module<T>::release_engine_() {
  engine = nullptr;
}

} // namespace myco

#define DECLARE_MYCO_MODULE(module)             \
  template<> struct myco::ModuleInfo<module> {  \
    static constexpr auto name = #module;       \
  }
