#include "imp/gfx/module/gfx_2d.hpp"

namespace imp {
void Gfx2D::r_initialize_(const E_Initialize& p) {
  Module::r_initialize_(p);
}

void Gfx2D::r_shutdown_(const E_Shutdown& p) {
  Module::r_shutdown_(p);
}
} // namespace imp
