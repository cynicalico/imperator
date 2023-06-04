#ifndef BAPHY_MODULE_HPP
#define BAPHY_MODULE_HPP

#include "baphy/core/event_bus.hpp"
#include "baphy/core/msgs.hpp"

namespace baphy {

template<typename T>
struct ModuleInfo;

class ModuleI {
public:
  std::string name;

  virtual ~ModuleI() = default;
};

template<typename T>
class Module;

class ModuleMgr {
public:
  template<class T, class TR, typename... Args> requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
  void create(const Args &&...args);

  template<typename T, typename... Args> requires std::derived_from<T, ModuleI>
  void create(const Args &&...args);

  template<typename T> requires std::derived_from<T, ModuleI>
  std::shared_ptr<T> get() const;

private:
  std::unordered_map<std::string, std::shared_ptr<ModuleI>> modules_{};
};

template<class T, class TR, typename... Args> requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
void ModuleMgr::create(const Args &&...args) {
  modules_[ModuleInfo<T>::name] = std::shared_ptr<ModuleI>(new TR(std::forward<Args>(args)...));
}

template<typename T, typename... Args> requires std::derived_from<T, ModuleI>
void ModuleMgr::create(const Args &&...args) {
  create<T, T>(std::forward<Args>(args)...);
}

template<typename T> requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::get() const {
  return std::dynamic_pointer_cast<T>(modules_.at(ModuleInfo<T>::name));
}

template<typename T>
class Module : public ModuleI {
public:
  std::shared_ptr<ModuleMgr> module_mgr;

  Module() : Module(std::vector<std::string>{}) {}
  explicit Module(std::vector<std::string> &&dependencies);

  ~Module() override;

protected:
  virtual void initialize_(const EInitialize &e);

  void shutdown_();

private:
  std::once_flag is_initialized_;
};

template<typename T>
Module<T>::Module(std::vector<std::string> &&dependencies) : ModuleI() {
  name = ModuleInfo<T>::name;
  EventBus::sub<EInitialize>(
      name,
      std::forward<std::vector<std::string>>(dependencies),
      [&](const auto &e) {
        std::call_once(is_initialized_, [&]() { initialize_(e); });
      }
  );
  EventBus::sub<EShutdown>(name, [&](const auto &) { shutdown_(); });
}

template<typename T>
Module<T>::~Module() {
  BAPHY_LOG_DEBUG("{} destroyed", name);
}

template<typename T>
void Module<T>::initialize_(const EInitialize &e) {
  module_mgr = e.module_mgr;

  BAPHY_LOG_DEBUG("Initialized {}", name);
}

template<typename T>
void Module<T>::shutdown_() {
  module_mgr = nullptr;

  BAPHY_LOG_DEBUG("{} shutdown", name);
}

} // namespace baphy

#define BAPHY_DECLARE_MODULE(module)              \
    template<> struct baphy::ModuleInfo<module> { \
        static constexpr auto name = #module;    \
    }

#endif //BAPHY_MODULE_HPP
