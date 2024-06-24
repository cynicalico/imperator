#ifndef IMPERATOR_CORE_MODULE_MGR_H
#define IMPERATOR_CORE_MODULE_MGR_H

#include "imperator/util/log.h"

#include <memory>
#include <string>

namespace imp {
template<typename T>
struct ModuleInfo;
} // namespace imp

#define IMPERATOR_DECLARE_MODULE(module)      \
    template<>                                \
    struct imp::ModuleInfo<module> {          \
        static constexpr auto name = #module; \
    }

namespace imp {
class ModuleI;

class ModuleMgr {
public:
    void clear() { modules_.clear(); }

    template<class T, class TR, typename... Args>
        requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
    std::shared_ptr<T> create(Args &&...args);

    template<typename T, typename... Args>
        requires std::derived_from<T, ModuleI>
    std::shared_ptr<T> create(Args &&...args);

    template<typename T>
        requires std::derived_from<T, ModuleI>
    std::shared_ptr<T> get() const;

private:
    std::unordered_map<std::string, std::shared_ptr<ModuleI>> modules_{};
};

template<class T, class TR, typename... Args>
    requires std::derived_from<T, ModuleI> && std::derived_from<TR, T>
std::shared_ptr<T> ModuleMgr::create(Args &&...args) {
    modules_.emplace(ModuleInfo<T>::name, std::make_shared<TR>(*this, std::forward<Args>(args)...));
    return get<T>();
}

template<typename T, typename... Args>
    requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::create(Args &&...args) {
    return create<T, T>(std::forward<Args>(args)...);
}

template<typename T>
    requires std::derived_from<T, ModuleI>
std::shared_ptr<T> ModuleMgr::get() const {
    return std::static_pointer_cast<T>(modules_.at(ModuleInfo<T>::name));
}

class ModuleI {
public:
    ModuleI(ModuleMgr &module_mgr, std::string module_name)
        : module_mgr_(module_mgr), module_name_(std::move(module_name)) {}

    virtual ~ModuleI() = default;

protected:
    ModuleMgr &module_mgr_;
    std::string module_name_;
};

template<typename T>
class Module : public ModuleI {
public:
    explicit Module(ModuleMgr &module_mgr);

    ~Module() override;
};

template<typename T>
Module<T>::Module(ModuleMgr &module_mgr) : ModuleI(module_mgr, ModuleInfo<T>::name) {
    IMPERATOR_LOG_DEBUG("Module created: {}", module_name_);
}

template<typename T>
Module<T>::~Module() {
    IMPERATOR_LOG_DEBUG("Module destroyed: {}", module_name_);
}
} // namespace imp

#endif //IMPERATOR_CORE_MODULE_MGR_H
