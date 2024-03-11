#include "imperator/module/module_mgr.h"

#include "imperator/core/engine.h"
#include "imperator/util/rnd.h"

namespace imp {
ModuleI::ModuleI(std::shared_ptr<Engine> engine, std::string module_name)
  : imp_module_name(std::move(module_name)),
    imp_engine(std::move(engine)) {
  imp_engine->sub_event<E_FreeEngine>(imp_module_name, [&](const auto&) { imp_engine.reset(); });
}

ModuleMgr::ModuleMgr(std::shared_ptr<Engine> engine): engine_(std::move(engine)) {
  engine_->sub_event<E_FreeEngine>(rnd::base58(11), [&](const auto&) { engine_.reset(); });
}
} // namespace imp
