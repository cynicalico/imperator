#ifndef BAPHY_UTIL_ENUM_BITOPS_HPP
#define BAPHY_UTIL_ENUM_BITOPS_HPP

#include <type_traits>

namespace baphy {

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
std::enable_if_t<EnableBitops<Enum>::enable, Enum> operator&(Enum lhs, Enum rhs) {
  using underlying = std::underlying_type_t<Enum>;
  return static_cast<Enum>(
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
bool is_enum_set(Enum e, Enum mask) {
  return (e & mask) == mask;
}

} // namespace baphy

#define ENUM_ENABLE_BITOPS(x)                 \
  template<> struct baphy::EnableBitops<x> {  \
    static constexpr bool enable = true;      \
  }

#endif//BAPHY_UTIL_ENUM_BITOPS_HPP
