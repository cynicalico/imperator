#ifndef IMPERATOR_MODULE_MODULE_MGR_H
#define IMPERATOR_MODULE_MODULE_MGR_H

#include "imperator/core/type_id.h"
#include "imperator/util/log.h"
#include <memory>

namespace imp {
template<typename T>
struct ModuleInfo;
} // namespace imp

#define IMPERATOR_DECLARE_MODULE(module)      \
  template<> struct imp::ModuleInfo<module> { \
    static constexpr auto name = #module;     \
  }

namespace imp {
class Engine;

class ModuleI {
public:
  std::string imp_module_name;
  std::shared_ptr<Engine> imp_engine;

  explicit ModuleI(std::shared_ptr<Engine> engine, std::string module_name);
  virtual ~ModuleI() = default;
};

class ModuleMgr {
public:
  explicit ModuleMgr(std::shared_ptr<Engine> engine);

  template<class T, class TR, typename... Args>
    requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  std::shared_ptr<T> create(Args&&... args);

  template<typename T, typename... Args>
    requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> create(Args&&... args);

  template<typename T>
    requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> get() const;

private:
  std::shared_ptr<Engine> engine_;
  std::vector<std::shared_ptr<ModuleI>> modules_{};
};

template<class T, class TR, typename... Args>
  requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
std::shared_ptr<T> ModuleMgr::create(Args&&... args) {
  auto idx = type_id<T>();
  if (modules_.size() <= idx)
    modules_.emplace_back();

  modules_[idx] = std::make_shared<TR>(engine_, std::forward<Args>(args)...);
  return get<T>();
}

template<typename T, typename... Args>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::create(Args&&... args) {
  return create<T, T>(std::forward<Args>(args)...);
}

template<typename T>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::get() const {
  auto idx = type_id<T>();
  return std::static_pointer_cast<T>(modules_[idx]);
}

template<typename T>
class Module : public ModuleI {
public:
  explicit Module(std::shared_ptr<Engine> engine);
  ~Module() override;
};

template<typename T>
Module<T>::Module(std::shared_ptr<Engine> engine)
  : ModuleI(std::move(engine), ModuleInfo<T>::name) {
  IMPERATOR_LOG_DEBUG("Module created: {}", imp_module_name);
}

template<typename T>
Module<T>::~Module() {
  IMPERATOR_LOG_DEBUG("Module destroyed: {}", imp_module_name);
}
} // namespace imp

#endif//IMPERATOR_MODULE_MODULE_MGR_H
