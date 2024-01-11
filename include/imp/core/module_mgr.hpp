#ifndef IMP_CORE_MODULE_MGR_HPP
#define IMP_CORE_MODULE_MGR_HPP

#include "imp/core/hermes.hpp"
#include "imp/util/log.hpp"
#include <memory>

namespace imp {
class ModuleI;

template<typename T>
class Module;

class ModuleMgr : public std::enable_shared_from_this<ModuleMgr> {
public:
  template<class T, class TR, typename... Args>
    requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  std::shared_ptr<T> create(const Args&&... args);

  template<typename T, typename... Args>
    requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> create(const Args&&... args);

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
  std::shared_ptr<ModuleMgr> module_mgr;

  virtual ~ModuleI() = default;

private:
  void set_module_mgr_(std::shared_ptr<ModuleMgr> mgr) {
    module_mgr = std::move(mgr);
  }
};

template<typename T>
class Module : public ModuleI {
public:
  Module() : Module(std::vector<std::string>{}) {}
  explicit Module(std::vector<std::string>&& dependencies);

  ~Module() override;

protected:
  bool received_shutdown_{false};

  virtual void r_initialize_(const E_Initialize& e);
  virtual void r_shutdown_(const E_Shutdown& e);

private:
  std::once_flag is_initialized_;
  std::once_flag is_shutdown_;
};

template<class T, class TR, typename... Args>
  requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
std::shared_ptr<T> ModuleMgr::create(const Args&&... args) {
  modules_[EPI<T>::name] = std::shared_ptr<ModuleI>(new TR(std::forward<Args>(args)...));
  modules_[EPI<T>::name]->set_module_mgr_(shared_from_this());

  return std::static_pointer_cast<T>(modules_[EPI<T>::name]);
}

template<typename T, typename... Args>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::create(const Args&&... args) {
  return create<T, T>(std::forward<Args>(args)...);
}

template<typename T>
  requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::get() const {
  return std::static_pointer_cast<T>(modules_.at(EPI<T>::name));
}

template<typename T>
Module<T>::Module(std::vector<std::string>&& dependencies) : ModuleI() {
  module_name = EPI<T>::name;

  Hermes::sub<E_Initialize>(
    module_name,
    std::forward<std::vector<std::string>>(dependencies),
    IMP_MAKE_RECEIVER(E_Initialize, [&](const auto& e) {
      std::call_once(is_initialized_, [&]() { r_initialize_(e); });
    })
  );

  Hermes::sub<E_Shutdown>(
    module_name,
    std::forward<std::vector<std::string>>(dependencies),
    IMP_MAKE_RECEIVER(E_Shutdown, [&](const auto& e) {
      std::call_once(is_shutdown_, [&]() { r_shutdown_(e); });
    })
  );

  IMP_LOG_DEBUG("Module created: {}", module_name);
}

template<typename T>
Module<T>::~Module() {
  IMP_LOG_DEBUG("Module destroyed: {}", module_name);
}

template<typename T>
void Module<T>::r_initialize_(const E_Initialize& e) {
  IMP_LOG_DEBUG("Module initialized: {}", module_name);
}

template<typename T>
void Module<T>::r_shutdown_(const E_Shutdown& e) {
  // Release the module manager so it can be freed
  module_mgr = nullptr;

  received_shutdown_ = true;
  IMP_LOG_DEBUG("Module shutdown: {}", module_name);
}
} // namespace imp

#endif//IMP_CORE_MODULE_MGR_HPP
