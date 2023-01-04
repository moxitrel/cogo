#ifndef COGO_UTILS_H_
#define COGO_UTILS_H_
// clang-format off

// Get nth element of items. (BUG)
//
// e.g. COGO_ARG_BUGET(()        , (P0,P1,P2,P3,...))  -> P1 (bug, want P0)
//      COGO_ARG_BUGET((_1)      , (P0,P1,P2,P3,...))  -> P1
//      COGO_ARG_BUGET((_1,_2)   , (P0,P1,P2,P3,...))  -> P2
//      COGO_ARG_BUGET((_1,_2,_3), (P0,P1,P2,P3,...))  -> P3
#define COGO_ARG_BUGET(PAREN_COUNT, PAREN_ARGS)                     \
    COGO_ARG_BUGET1(COGO_RM_PAREN PAREN_COUNT, COGO_RM_PAREN PAREN_ARGS)
#define COGO_ARG_BUGET1(...)  COGO_ARG_BUGET2(__VA_ARGS__)
#define COGO_ARG_BUGET2(                                            \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,               \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)          N
#define COGO_RM_PAREN(...)  __VA_ARGS__

// Get nth element of items. (Solved the COGO_ARG_BUGET(...)'s BUG)
//
// e.g. COGO_ARG_GET(()        , (P0,P1,P2,P3,...)) -> COGO_ARG_GET_01N(0,1,P0,P1,P1) -> ARG_GET_01(P0,P1,P1) -> P0
//      COGO_ARG_GET((_1)      , (P0,P1,P2,P3,...)) -> COGO_ARG_GET_01N(0,0,P0,P1,P1) -> ARG_GET_00(P0,P1,P1) -> P1
//      COGO_ARG_GET((_1,_2)   , (P0,P1,P2,P3,...)) -> COGO_ARG_GET_01N(1,1,P0,P1,P2) -> ARG_GET_11(P0,P1,P2) -> P2
//      COGO_ARG_GET((_1,_2,_3), (P0,P1,P2,P3,...)) -> COGO_ARG_GET_01N(1,1,P0,P1,P3) -> ARG_GET_11(P0,P1,P3) -> P3
//
// See: https://stackoverflow.com/questions/11317474/macro-to-count-number-of-arguments
//      http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
#define COGO_ARG_GET(PAREN_COUNT, PAREN_ARGS)                               \
    COGO_ARG_GET_01N(                                                       \
        COGO_HAS_COMMA(COGO_RM_PAREN PAREN_COUNT),                          \
        COGO_HAS_COMMA(COGO_GET_COMMA COGO_RM_PAREN PAREN_COUNT ()),        \
        COGO_ARG_BUGET1( COGO_RM_PAREN PAREN_ARGS),                         \
        COGO_ARG_BUGET1(,COGO_RM_PAREN PAREN_ARGS),                         \
        COGO_ARG_BUGET(PAREN_COUNT, PAREN_ARGS)                             \
    )
#define COGO_ARG_GET_01N(...)            COGO_ARG_GET_01N1(__VA_ARGS__)
#define COGO_ARG_GET_01N1(D1, D2, ...)   COGO_ARG_GET_##D1##D2(__VA_ARGS__)
#define COGO_ARG_GET_01(O,I,N)           O
#define COGO_ARG_GET_00(O,I,N)           I
#define COGO_ARG_GET_11(O,I,N)           N
#define COGO_HAS_COMMA(...)              COGO_ARG_BUGET1(__VA_ARGS__, COGO_HAS_COMMA_PADDING)
#define COGO_GET_COMMA(...)              ,
#define COGO_HAS_COMMA_PADDING                              \
      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,       \
      1,   1,   1,   1,   1,   1,   1,   1,   0,   0

// Count the number of arguments.
#define COGO_ARG_COUNT(...)      COGO_ARG_COUNT1(__VA_ARGS__)
#define COGO_ARG_COUNT1(...)     COGO_ARG_GET((__VA_ARGS__),(COGO_ARG_COUNT_PADDING))
#define COGO_ARG_COUNT_PADDING                              \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0

// Is arg list empty?
#define COGO_ARG_EMPTY(...)      COGO_ARG_EMPTY1(__VA_ARGS__)
#define COGO_ARG_EMPTY1(...)     COGO_ARG_GET((__VA_ARGS__),(COGO_ARG_EMPTY_PADDING))
#define COGO_ARG_EMPTY_PADDING                              \
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,       \
      0,   0,   0,   0,   0,   0,   0,   0,   0,   1

//e.g. COGO_MAP(;, CASE_GOTO, 10, 20, 30)
//     -> COGO_MAP_3(;, CASE_GOTO, 10, 20, 30)
//     -> CASE_GOTO(10);
//        CASE_GOTO(20);
//        CASE_GOTO(30)
#define COGO_MAP(SEP, F, ...)    COGO_MAP1(COGO_ARG_COUNT(__VA_ARGS__), SEP, F, __VA_ARGS__)
#define COGO_MAP1(...)           COGO_MAP2(__VA_ARGS__)
#define COGO_MAP2(N, ...)        COGO_MAP_##N(__VA_ARGS__)
#define COGO_MAP_0(...)
#define COGO_MAP_1( SEP, F, ...)      F(__VA_ARGS__)
#define COGO_MAP_2( SEP, F, X, ...)   F(X) SEP COGO_MAP_1( SEP, F, __VA_ARGS__)
#define COGO_MAP_3( SEP, F, X, ...)   F(X) SEP COGO_MAP_2( SEP, F, __VA_ARGS__)
#define COGO_MAP_4( SEP, F, X, ...)   F(X) SEP COGO_MAP_3( SEP, F, __VA_ARGS__)
#define COGO_MAP_5( SEP, F, X, ...)   F(X) SEP COGO_MAP_4( SEP, F, __VA_ARGS__)
#define COGO_MAP_6( SEP, F, X, ...)   F(X) SEP COGO_MAP_5( SEP, F, __VA_ARGS__)
#define COGO_MAP_7( SEP, F, X, ...)   F(X) SEP COGO_MAP_6( SEP, F, __VA_ARGS__)
#define COGO_MAP_8( SEP, F, X, ...)   F(X) SEP COGO_MAP_7( SEP, F, __VA_ARGS__)
#define COGO_MAP_9( SEP, F, X, ...)   F(X) SEP COGO_MAP_8( SEP, F, __VA_ARGS__)
#define COGO_MAP_10(SEP, F, X, ...)   F(X) SEP COGO_MAP_9( SEP, F, __VA_ARGS__)
#define COGO_MAP_11(SEP, F, X, ...)   F(X) SEP COGO_MAP_10(SEP, F, __VA_ARGS__)
#define COGO_MAP_12(SEP, F, X, ...)   F(X) SEP COGO_MAP_11(SEP, F, __VA_ARGS__)
#define COGO_MAP_13(SEP, F, X, ...)   F(X) SEP COGO_MAP_12(SEP, F, __VA_ARGS__)
#define COGO_MAP_14(SEP, F, X, ...)   F(X) SEP COGO_MAP_13(SEP, F, __VA_ARGS__)
#define COGO_MAP_15(SEP, F, X, ...)   F(X) SEP COGO_MAP_14(SEP, F, __VA_ARGS__)
#define COGO_MAP_16(SEP, F, X, ...)   F(X) SEP COGO_MAP_15(SEP, F, __VA_ARGS__)
#define COGO_MAP_17(SEP, F, X, ...)   F(X) SEP COGO_MAP_16(SEP, F, __VA_ARGS__)
#define COGO_MAP_18(SEP, F, X, ...)   F(X) SEP COGO_MAP_17(SEP, F, __VA_ARGS__)
#define COGO_MAP_19(SEP, F, X, ...)   F(X) SEP COGO_MAP_18(SEP, F, __VA_ARGS__)

// Expand to SK if ID defined as empty, i.e. "#define ID", else FK
#define COGO_IFNIL(ID,SK,FK)        COGO_IFNIL1(COGO_ARG_EMPTY(ID),SK,FK)
#define COGO_IFNIL1(...)            COGO_IFNIL2(__VA_ARGS__)
#define COGO_IFNIL2(BOOL,SK,FK)     COGO_IFNIL3_##BOOL(SK,FK)
#define COGO_IFNIL3_1(SK,FK)        SK
#define COGO_IFNIL3_0(SK,FK)        FK

#define COGO_ARG(...)               COGO_RM_PAREN(__VA_ARGS__)

#endif /* COGO_UTILS_H_ */
