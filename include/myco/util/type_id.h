#ifndef MYCO_UTIL_TYPE_ID_H
#define MYCO_UTIL_TYPE_ID_H

namespace myco {

inline int type_id_seq = 0;
template<typename T> inline const int type_id = type_id_seq++;

} // namespace myco

#endif//MYCO_UTIL_TYPE_ID_H
