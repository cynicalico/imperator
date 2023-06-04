#ifndef BAPHY_UTIL_TYPE_ID_H
#define BAPHY_UTIL_TYPE_ID_H

namespace baphy {

inline int type_id_seq = 0;
template<typename T> inline const int type_id = type_id_seq++;

} // namespace baphy

#endif//BAPHY_UTIL_TYPE_ID_H
