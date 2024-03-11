#ifndef IMPERATOR_CORE_TYPE_ID_H
#define IMPERATOR_CORE_TYPE_ID_H

#include <type_traits>

namespace imp {
inline std::size_t type_id_seq = 0;

template<typename T>
inline const std::size_t type_id_impl = type_id_seq++;

template<typename T>
constexpr std::size_t type_id() {
  return type_id_impl<std::remove_cvref_t<T>>;
}
} // namespace imp

#endif//IMPERATOR_CORE_TYPE_ID_H
