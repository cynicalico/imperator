#ifndef BAPHY_MODULE_HPP
#define BAPHY_MODULE_HPP

#include "baphy/core/event_bus.hpp"
#include "baphy/core/msgs.hpp"
#include <memory>

namespace baphy {

template<typename T>
struct ModuleInfo;

class ModuleI;

template<typename T>
class Module;

class ModuleMgr : public std::enable_shared_from_this<ModuleMgr> {
public:
  template<class T, class TR, typename... Args>
  requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  void create(const Args &&...args);

  template<typename T, typename... Args>
  requires std::derived_from<T, ModuleI>
  void create(const Args &&...args);

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
  explicit Module(std::vector<std::string> &&dependencies);

  ~Module() override;

protected:
  virtual void initialize_(const EInitialize &e);
  virtual void shutdown_(const EShutdown &e);

private:
  std::once_flag is_initialized_;
};

template<class T, class TR, typename... Args>
requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
void ModuleMgr::create(const Args &&...args) {
  modules_[ModuleInfo<T>::name] = std::shared_ptr<ModuleI>(new TR(std::forward<Args>(args)...));
  modules_[ModuleInfo<T>::name]->set_module_mgr_(shared_from_this());
}

template<typename T, typename... Args>
requires std::derived_from<T, ModuleI>
void ModuleMgr::create(const Args &&...args) {
  create<T, T>(std::forward<Args>(args)...);
}

template<typename T>
requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::get() const {
  return std::dynamic_pointer_cast<T>(modules_.at(ModuleInfo<T>::name));
}

template<typename T>
Module<T>::Module(std::vector<std::string> &&dependencies) : ModuleI() {
  module_name = ModuleInfo<T>::name;
  EventBus::sub<EInitialize>(
      module_name,
      std::forward<std::vector<std::string>>(dependencies),
      [&](const auto &e) {
        std::call_once(is_initialized_, [&]() { initialize_(e); });
      }
  );
  EventBus::sub<EShutdown>(module_name, [&](const auto &e) { shutdown_(e); });
}

template<typename T>
Module<T>::~Module() {
  BAPHY_LOG_DEBUG("{} destroyed", module_name);
}

template<typename T>
void Module<T>::initialize_(const EInitialize &e) {
  BAPHY_LOG_DEBUG("Initialized {}", module_name);
}

template<typename T>
void Module<T>::shutdown_(const EShutdown &e) {
  // Release the module manager so it can be freed
  module_mgr = nullptr;

  BAPHY_LOG_DEBUG("{} shutdown", module_name);
}

} // namespace baphy

#define BAPHY_DECLARE_MODULE(module)            \
  template<> struct baphy::ModuleInfo<module> { \
    static constexpr auto name = #module;       \
  }

#endif //BAPHY_MODULE_HPP
