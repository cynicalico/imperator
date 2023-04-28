#ifndef MYCO_UTIL_ENUM_BITOPS_H
#define MYCO_UTIL_ENUM_BITOPS_H

#include <type_traits>

#define ENABLE_BITOPS(x)                 \
    template<> struct EnableBitops<x> {  \
        static const bool enable = true; \
    }

template<typename Enum>
struct EnableBitops {
    static const bool enable = false;
};

template<typename Enum>
typename std::enable_if_t<EnableBitops<Enum>::enable, Enum>
operator|(Enum lhs, Enum rhs) {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            static_cast<underlying>(lhs) |
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
typename std::enable_if_t<EnableBitops<Enum>::enable, Enum>
operator&(Enum lhs, Enum rhs) {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            static_cast<underlying>(lhs) &
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
typename std::underlying_type<Enum>::type
unwrap(Enum e) {
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<underlying>(e);
}

template<typename Enum>
typename std::underlying_type<Enum>::type
set(Enum e, Enum mask) {
    return (e & mask) == mask;
}

#endif//MYCO_UTIL_ENUM_BITOPS_H
