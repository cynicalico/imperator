#ifndef BAPHY_CORE_TYPE_ID_HPP
#define BAPHY_CORE_TYPE_ID_HPP

#include <type_traits>

namespace baphy {

inline std::size_t type_id_seq = 0;

template<typename T>
inline const std::size_t type_id_impl = type_id_seq++;

template<typename T>
constexpr std::size_t type_id() {
  return type_id_impl<std::remove_cvref_t<T>>;
}

} // namespace baphy

#endif//BAPHY_CORE_TYPE_ID_HPP
