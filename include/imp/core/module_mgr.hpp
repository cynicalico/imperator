#ifndef IMP_CORE_MODULE_MGR_HPP
#define IMP_CORE_MODULE_MGR_HPP

#include "imp/core/hermes.hpp"
#include "imp/util/log.hpp"
#include <memory>
#include <utility>

namespace imp {
class ModuleI;

template<typename T>
class Module;

class ModuleMgr : public std::enable_shared_from_this<ModuleMgr> {
public:
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
  std::unordered_map<std::string, std::shared_ptr<ModuleI>> modules_{};
};

class ModuleI {
  friend class ModuleMgr;

public:
  std::string module_name;
  std::weak_ptr<ModuleMgr> module_mgr;

  explicit ModuleI(std::string module_name, std::weak_ptr<ModuleMgr> module_mgr)
    : module_name(std::move(module_name)), module_mgr(std::move(module_mgr)) {}

  virtual ~ModuleI() = default;
};

template<typename T>
class Module : public ModuleI {
public:
  explicit Module(const std::weak_ptr<ModuleMgr>& module_mgr);

  ~Module() override;
};

template<class T, class TR, typename... Args>
  requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
std::shared_ptr<T> ModuleMgr::create(Args&&... args) {
  modules_[EPI<T>::name] = std::shared_ptr<ModuleI>(new TR(shared_from_this(), std::forward<Args>(args)...));
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
  return std::static_pointer_cast<T>(modules_.at(EPI<T>::name));
}

template<typename T>
Module<T>::Module(const std::weak_ptr<ModuleMgr>& module_mgr) : ModuleI(EPI<T>::name, module_mgr) {
  IMP_LOG_DEBUG("Module created: {}", module_name);
}

template<typename T>
Module<T>::~Module() {
  IMP_LOG_DEBUG("Module destroyed: {}", module_name);
}
} // namespace imp

#endif//IMP_CORE_MODULE_MGR_HPP
