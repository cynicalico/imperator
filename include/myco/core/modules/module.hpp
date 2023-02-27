#pragma once

#include "myco/core/scheduler.hpp"
#include <string>

namespace myco {

class Engine;

template<typename T>
struct ModuleInfo;

class ModuleI {
public:
  std::string name;

  virtual ~ModuleI() {
    MYCO_LOG_DEBUG("ModuleI destroyed");
  }
};

template<typename T>
class Module : public ModuleI {
  friend class Engine;

public:
  std::shared_ptr<Engine> engine;

  Module() : Module(std::vector<std::string>{}) {}

  explicit Module(std::vector<std::string> &&dependencies) : ModuleI() {
    name = ModuleInfo<T>::name;
    Scheduler::sub<Initialize>(name, std::forward<std::vector<std::string>>(dependencies), [&](const auto &e){ initialize_(e); });
    Scheduler::sub<ReleaseEngine>(name, [&](const auto &){ release_engine_(); });
  }

  ~Module() override {
    MYCO_LOG_DEBUG("Module destroyed");
  };

protected:
  virtual void initialize_(const Initialize &e);
  void release_engine_();
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
  };
