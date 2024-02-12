#include "imp/util/platform.hpp"

#include "imp/util/log.hpp"

namespace imp {

void log_platform() {
#define STRINGIFY(x) #x
  IMP_LOG_DEBUG("Platform(s) detected");
#if defined(IMP_PLATFORM_WINDOWS)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_WINDOWS));
#endif
#if defined(IMP_PLATFORM_WINDOWS32)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_WINDOWS32));
#endif
#if defined(IMP_PLATFORM_WINDOWS64)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_WINDOWS64));
#endif
#if defined(IMP_PLATFORM_APPLE_IOS_SIMULATOR)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_APPLE_IOS_SIMULATOR));
#endif
#if defined(IMP_PLATFORM_APPLE_CATALYST)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_APPLE_CATALYST));
#endif
#if defined(IMP_PLATFORM_APPLE_IOS)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_APPLE_IOS));
#endif
#if defined(IMP_PLATFORM_APPLE)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_APPLE));
#endif
#if defined(IMP_PLATFORM_LINUX)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_LINUX));
#endif
#if defined(IMP_PLATFORM_UNIX)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_UNIX));
#endif
#if defined(IMP_PLATFORM_POSIX)
  IMP_LOG_DEBUG("=> {}", STRINGIFY(IMP_PLATFORM_POSIX));
#endif
#undef STRINGIFY
}

} // namespace imp
