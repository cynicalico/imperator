#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
# define MYCO_PLATFORM_WINDOWS
# if defined(WIN64) || defined(_WIN64)
#   define MYCO_PLATFORM_WINDOWS64
# else
#   define MYCO_PLATFORM_WINDOWS32
# endif
#elif __APPLE__
# include <TargetConditionals.h>
# if TARGET_IPHONE_SIMULATOR
#   define MYCO_PLATFORM_APPLE_IOS_SIMULATOR
# elif TARGET_OS_MACCATALYST
#   define MYCO_PLATFORM_APPLE_CATALYST
# elif TARGET_OS_IPHONE
#   define MYCO_PLATFORM_APPLE_IOS
# elif TARGET_OS_MAC
#   define MYCO_PLATFORM_APPLE
# else
#   error "Unknown Apple platform"
# endif
#elif __linux__
# define MYCO_PLATFORM_LINUX
#elif __unix__
# define MYCO_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
# define MYCO_PLATFORM_POSIX
#else
# error "Unknown compiler"
#endif
