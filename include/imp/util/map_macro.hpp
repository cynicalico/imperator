// Taken from: https://github.com/swansontec/map-macro/tree/master
// More specifically issue #11: https://github.com/swansontec/map-macro/issues/11

#ifndef IMP_UTIL_MAP_MACRO_HPP
#define IMP_UTIL_MAP_MACRO_HPP

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

#define NOP

#define MAP_POP0(F, X, ...) F(X) __VA_OPT__(MAP_POP1 NOP (F, __VA_ARGS__))
#define MAP_POP1(F, X, ...) F(X) __VA_OPT__(MAP_POP0 NOP (F, __VA_ARGS__))
#define MAP(F, ...) __VA_OPT__(EVAL(MAP_POP0(F, __VA_ARGS__)))

#define MAP_LIST_POP0(F, X, ...) F(X) __VA_OPT__(, MAP_LIST_POP1 NOP (F, __VA_ARGS__))
#define MAP_LIST_POP1(F, X, ...) F(X) __VA_OPT__(, MAP_LIST_POP0 NOP (F, __VA_ARGS__))
#define MAP_LIST(F, ...) __VA_OPT__(EVAL(MAP_LIST_POP0(F, __VA_ARGS__)))

#endif//IMP_UTIL_MAP_MACRO_HPP
