#ifndef IMPERATOR_UTIL_PLATFORM_HPP
#define IMPERATOR_UTIL_PLATFORM_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define IMPERATOR_PLATFORM_WINDOWS
#    if defined(WIN64) || defined(_WIN64)
#        define IMPERATOR_PLATFORM_WINDOWS64
#    else
#        define IMPERATOR_PLATFORM_WINDOWS32
#    endif
#elif __APPLE__
#    include <TargetConditionals.h>
#    if TARGET_IPHONE_SIMULATOR
#        define IMPERATOR_PLATFORM_APPLE_IOS_SIMULATOR
#    elif TARGET_OS_MACCATALYST
#        define IMPERATOR_PLATFORM_APPLE_CATALYST
#    elif TARGET_OS_IPHONE
#        define IMPERATOR_PLATFORM_APPLE_IOS
#    elif TARGET_OS_MAC
#        define IMPERATOR_PLATFORM_APPLE
#    else
#        error "Unknown Apple platform"
#    endif
#elif __linux__
#    define IMPERATOR_PLATFORM_LINUX
#elif __unix__
#    define IMPERATOR_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
#    define IMPERATOR_PLATFORM_POSIX
#else
#    error "Unknown compiler"
#endif

namespace imp {
void log_platform();
} // namespace imp

#endif //IMPERATOR_UTIL_PLATFORM_HPP
