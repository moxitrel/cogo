#define LEN(...)    LEN_(__VA_ARGS__, LEN_PATT_PADDING)
#define LEN_(...)   LEN_PATT(__VA_ARGS__)
#define LEN_PATT_PADDING                        \
    19 , 18, 17, 16, 15, 14, 13, 12, 11, 10,    \
     9 ,  8,  7,  6,  5,  4,  3,  2,  1,  0
#define LEN_PATT(                               \
    _1 , _2, _3, _4, _5, _6, _7, _8, _9, _10,   \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,  N ,   \
    ...)   N



//#define GET(N, ...)   GET_##N(__VA_ARGS__)
//#define GET_0(X, ...) X
//#define GET_1(X, ...) GET_0(__VA_ARGS__)
//#define GET_2(X, ...) GET_1(__VA_ARGS__)
//#define GET_3(X, ...) GET_2(__VA_ARGS__)
//#define GET_4(X, ...) GET_3(__VA_ARGS__)
//#define GET_5(X, ...) GET_4(__VA_ARGS__)
//#define GET_6(X, ...) GET_5(__VA_ARGS__)
//#define GET_7(X, ...) GET_6(__VA_ARGS__)
//#define GET_8(X, ...) GET_7(__VA_ARGS__)
//
//
//#define REST(N, ...)   REST_##N(__VA_ARGS__)
//#define REST_0(...)    __VA_ARGS__
//#define REST_1(X, ...) REST_0(__VA_ARGS__)
//#define REST_2(X, ...) REST_1(__VA_ARGS__)
//#define REST_3(X, ...) REST_2(__VA_ARGS__)
//#define REST_4(X, ...) REST_3(__VA_ARGS__)
//#define REST_5(X, ...) REST_4(__VA_ARGS__)
//#define REST_6(X, ...) REST_5(__VA_ARGS__)
//#define REST_7(X, ...) REST_6(__VA_ARGS__)
//#define REST_8(X, ...) REST_7(__VA_ARGS__)


#define MAP(F, ...)         MAP_N(LEN(__VA_ARGS__), F, __VA_ARGS__)
#define MAP_N(...)          MAP_N_(__VA_ARGS__)
#define MAP_N_(N, F, ...)   MAP_##N(F, __VA_ARGS__)
#define MAP_0(F,    ...)
#define MAP_1(F, X, ...) F(X) MAP_0(F, __VA_ARGS__)
#define MAP_2(F, X, ...) F(X) MAP_1(F, __VA_ARGS__)
#define MAP_3(F, X, ...) F(X) MAP_2(F, __VA_ARGS__)
#define MAP_4(F, X, ...) F(X) MAP_3(F, __VA_ARGS__)
#define MAP_5(F, X, ...) F(X) MAP_4(F, __VA_ARGS__)
#define MAP_6(F, X, ...) F(X) MAP_5(F, __VA_ARGS__)
#define MAP_7(F, X, ...) F(X) MAP_6(F, __VA_ARGS__)
#define MAP_8(F, X, ...) F(X) MAP_7(F, __VA_ARGS__)
#define MAP_9(F, X, ...) F(X) MAP_8(F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X) MAP_9(F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X) MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X) MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X) MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X) MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X) MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X) MAP_15(F, __VA_ARGS__)
