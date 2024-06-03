#ifndef IMPERATOR_UTIL_ENUM_BITOPS_H
#define IMPERATOR_UTIL_ENUM_BITOPS_H

#include <type_traits>

namespace imp {
template<typename Enum>
struct EnableBitops {
    static constexpr bool enable = false;
};

template<typename Enum>
std::enable_if_t<EnableBitops<Enum>::enable, Enum> operator|(Enum lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
            static_cast<underlying>(lhs) |
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
std::enable_if_t<EnableBitops<Enum>::enable, Enum> &operator|=(Enum &lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) |
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
std::enable_if_t<EnableBitops<Enum>::enable, Enum> operator&(Enum lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return static_cast<Enum>(
            static_cast<underlying>(lhs) &
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
std::enable_if_t<EnableBitops<Enum>::enable, Enum> &operator&=(Enum &lhs, Enum rhs) {
    using underlying = std::underlying_type_t<Enum>;
    return lhs = static_cast<Enum>(
            static_cast<underlying>(lhs) &
            static_cast<underlying>(rhs)
    );
}

template<typename Enum>
std::underlying_type_t<Enum> unwrap(Enum e) {
    using underlying = std::underlying_type_t<Enum>;
    return static_cast<underlying>(e);
}

template<typename Enum>
bool is_flag_set(Enum e, Enum mask) { return (e & mask) == mask; }
} // namespace imp

#define ENUM_ENABLE_BITOPS(x)               \
  template<> struct imp::EnableBitops<x> {  \
  static constexpr bool enable = true;      \
}

#endif//IMPERATOR_UTIL_ENUM_BITOPS_H
