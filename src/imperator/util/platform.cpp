#include "imperator/util/platform.h"

#include "imperator/util/log.h"

namespace imp {
void log_platform() {
#define STRINGIFY(x) #x
  IMPERATOR_LOG_DEBUG("Platform(s) detected");
#if defined(IMPERATOR_PLATFORM_WINDOWS)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_WINDOWS));
#endif
#if defined(IMPERATOR_PLATFORM_WINDOWS32)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_WINDOWS32));
#endif
#if defined(IMPERATOR_PLATFORM_WINDOWS64)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_WINDOWS64));
#endif
#if defined(IMPERATOR_PLATFORM_APPLE_IOS_SIMULATOR)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_APPLE_IOS_SIMULATOR));
#endif
#if defined(IMPERATOR_PLATFORM_APPLE_CATALYST)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_APPLE_CATALYST));
#endif
#if defined(IMPERATOR_PLATFORM_APPLE_IOS)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_APPLE_IOS));
#endif
#if defined(IMPERATOR_PLATFORM_APPLE)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_APPLE));
#endif
#if defined(IMPERATOR_PLATFORM_LINUX)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_LINUX));
#endif
#if defined(IMPERATOR_PLATFORM_UNIX)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_UNIX));
#endif
#if defined(IMPERATOR_PLATFORM_POSIX)
  IMPERATOR_LOG_DEBUG("=> {}", STRINGIFY(IMPERATOR_PLATFORM_POSIX));
#endif
#undef STRINGIFY
}
} // namespace imp
