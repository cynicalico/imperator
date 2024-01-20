#ifndef IMP_GFX_MODULE_BATCHER_HPP
#define IMP_GFX_MODULE_BATCHER_HPP

#include "imperator/core/module_mgr.hpp"

namespace imp {
class Batcher : public Module<Batcher> {
public:
    Batcher() : Module() {}
    
protected:
    void r_initialize_(const E_Initialize& p) override;
    void r_shutdown_(const E_Shutdown& p) override;
};
} // namespace imp

IMPERATOR_PRAISE_HERMES(imp::Batcher);

#endif//IMP_GFX_MODULE_BATCHER_HPP
