#ifndef IMP_UTIL_ONESHOT_MACRO_HPP
#define IMP_UTIL_ONESHOT_MACRO_HPP

#include <mutex>

#define IMP_ONESHOT(f)                  \
  do {                                  \
    static std::once_flag oneshot_flag; \
    std::call_once(oneshot_flag, f);    \
  } while(0);

#endif//IMP_UTIL_ONESHOT_MACRO_HPP
