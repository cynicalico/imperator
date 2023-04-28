#ifndef MYCO_CORE_MODULE_H
#define MYCO_CORE_MODULE_H

#include "myco/core/event_bus.h"
#include "myco/core/msgs.h"

#include <mutex>
#include <string>
#include <vector>

namespace myco {

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
    explicit Module(std::vector<std::string> &&dependencies);

    ~Module() override;

protected:
    virtual void initialize_(const Initialize &e);

    void release_engine_();

private:
    std::once_flag is_initialized_;
};

template<typename T>
Module<T>::Module(std::vector<std::string> &&dependencies) : ModuleI() {
    name = ModuleInfo<T>::name;
    EventBus::sub<Initialize>(
            name,
            std::forward<std::vector<std::string>>(dependencies),
            [&](const auto &e) {
                std::call_once(is_initialized_, [&]() { initialize_(e); });
            }
    );
    EventBus::sub<ReleaseEngine>(name, [&](const auto &) { release_engine_(); });

    MYCO_LOG_DEBUG("Initialized {}", name);
}

template<typename T>
Module<T>::~Module() {
    MYCO_LOG_DEBUG("{} destroyed", name);
}

template<typename T>
void Module<T>::initialize_(const Initialize &e) {
    engine = e.engine;
}

template<typename T>
void Module<T>::release_engine_() {
    engine = nullptr;
}

} // namespace myco

#define MYCO_DECLARE_MODULE(module)              \
    template<> struct myco::ModuleInfo<module> { \
        static constexpr auto name = #module;    \
    }

#endif//MYCO_CORE_MODULE_H
