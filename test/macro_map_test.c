//
// Created by M2 on 2018/12/4.
//

#define LEN(...)        LEN_PATT_(__VA_ARGS__, LEN_PATT_PADDING)
#define LEN_PATT_(...)  LEN_PATT(__VA_ARGS__)
#define LEN_PATT_PADDING                                    \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0
#define LEN_PATT(                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N


#define MAP(F, ...)       MAP_N_(LEN(__VA_ARGS__), F, __VA_ARGS__)
//#define MAP_N_(...)       MAP_N(__VA_ARGS__)
//#define MAP_N(N, F, ...)  MAP_##N(F, __VA_ARGS__)
//#define MAP_0( F, ...)
//#define MAP_1( F, X, ...) F(X) MAP_0( F, __VA_ARGS__)
//#define MAP_2( F, X, ...) F(X) MAP_1( F, __VA_ARGS__)
//#define MAP_3( F, X, ...) F(X) MAP_2( F, __VA_ARGS__)

LEN()
MAP(CASE_GOTO, );
