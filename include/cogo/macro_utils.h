#ifndef CX2_MACRO_UTILS_H_
#define CX2_MACRO_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CX2_IDENTITY(...)      __VA_ARGS__

#define CX2_TO_STRING(...)     CX2_TO_STRING1(__VA_ARGS__)
#define CX2_TO_STRING1(...)    #__VA_ARGS__

// Get the nth element of tuple.
// e.g. CX2_GET_BY_COMMA(        , P0,P1,P2,P3,...)  -> P1 (bug, want P0)
//      CX2_GET_BY_COMMA(_1      , P0,P1,P2,P3,...)  -> P1
//      CX2_GET_BY_COMMA(_1,_2   , P0,P1,P2,P3,...)  -> P2
//      CX2_GET_BY_COMMA(_1,_2,_3, P0,P1,P2,P3,...)  -> P3
#define CX2_GET_BY_COMMA(...)  CX2_GET_BY_COMMA1(__VA_ARGS__)
#define CX2_GET_BY_COMMA1(...) CX2_GET_BY_COMMA2(__VA_ARGS__)
#define CX2_GET_BY_COMMA2(                               \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,             \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,    \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,    \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,    \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,    \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, N, ...) \
  N

// Get the nth element of tuple. (Resolved the CX2_GET_BY_COMMA(...)'s BUG)
// e.g. CX2_GET(        , P0,P1,P2,P3,...) -> CX2_GET_01N(0,1,P0,P1,P1) -> ARG_GET_01(P0,P1,P1) -> P0
//      CX2_GET(_1      , P0,P1,P2,P3,...) -> CX2_GET_01N(0,0,P0,P1,P1) -> ARG_GET_00(P0,P1,P1) -> P1
//      CX2_GET(_1,_2   , P0,P1,P2,P3,...) -> CX2_GET_01N(1,1,P0,P1,P2) -> ARG_GET_11(P0,P1,P2) -> P2
//      CX2_GET(_1,_2,_3, P0,P1,P2,P3,...) -> CX2_GET_01N(1,1,P0,P1,P3) -> ARG_GET_11(P0,P1,P3) -> P3
//
// See: https://stackoverflow.com/questions/11317474/macro-to-count-number-of-arguments
//      http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
#define CX2_GET(X0, ...)                   \
  CX2_GET_01N(                             \
      CX2_HAS_COMMA(X0),                   \
      CX2_HAS_COMMA(CX2_COMMA X0 /**/ ()), \
      CX2_GET_BY_COMMA(__VA_ARGS__),       \
      CX2_GET_BY_COMMA(, __VA_ARGS__),     \
      CX2_GET_BY_COMMA(X0, __VA_ARGS__))
#define CX2_GET_01N(...)          CX2_GET_01N1(__VA_ARGS__)
#define CX2_GET_01N1(D1, D2, ...) CX2_GET_##D1##D2(__VA_ARGS__)
#define CX2_GET_01(O, I, N)       O
#define CX2_GET_00(O, I, N)       I
#define CX2_GET_11(O, I, N)       N
#define CX2_HAS_COMMA(...)        CX2_GET_BY_COMMA(__VA_ARGS__, CX2_HAS_COMMA_PADDING)
#define CX2_COMMA(...)            ,
#define CX2_HAS_COMMA_PADDING     CX2_IDENTITY( \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               \
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0)

// Count the number of arguments.
#define CX2_COUNT(...)    CX2_COUNT1(__VA_ARGS__)
#define CX2_COUNT1(...)   CX2_GET(__VA_ARGS__, CX2_COUNT_PADDING)
#define CX2_COUNT_PADDING CX2_IDENTITY(     \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// Is arg list empty?
#define CX2_IS_EMPTY(...)    CX2_IS_EMPTY1(__VA_ARGS__)
#define CX2_IS_EMPTY1(...)   CX2_GET(__VA_ARGS__, CX2_IS_EMPTY_PADDING)
#define CX2_IS_EMPTY_PADDING CX2_IDENTITY( \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,          \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,          \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,          \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,          \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,          \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1)

// e.g. CX2_MAP(;, CASE_GOTO, 10, 20, 30)
//      -> CX2_MAP_3(;, CASE_GOTO, 10, 20, 30)
//      -> CASE_GOTO(10);
//         CASE_GOTO(20);
//         CASE_GOTO(30)
#define CX2_MAP(SEP, F, ...) CX2_MAP1(CX2_COUNT(__VA_ARGS__), SEP, F, __VA_ARGS__)
#define CX2_MAP1(...)        CX2_MAP2(__VA_ARGS__)
#define CX2_MAP2(N, ...)     CX2_MAP_##N(__VA_ARGS__)
#define CX2_MAP_0(...)
#define CX2_MAP_1(SEP, F, ...)     F(__VA_ARGS__)
#define CX2_MAP_2(SEP, F, X, ...)  F(X) SEP CX2_MAP_1(SEP, F, __VA_ARGS__)
#define CX2_MAP_3(SEP, F, X, ...)  F(X) SEP CX2_MAP_2(SEP, F, __VA_ARGS__)
#define CX2_MAP_4(SEP, F, X, ...)  F(X) SEP CX2_MAP_3(SEP, F, __VA_ARGS__)
#define CX2_MAP_5(SEP, F, X, ...)  F(X) SEP CX2_MAP_4(SEP, F, __VA_ARGS__)
#define CX2_MAP_6(SEP, F, X, ...)  F(X) SEP CX2_MAP_5(SEP, F, __VA_ARGS__)
#define CX2_MAP_7(SEP, F, X, ...)  F(X) SEP CX2_MAP_6(SEP, F, __VA_ARGS__)
#define CX2_MAP_8(SEP, F, X, ...)  F(X) SEP CX2_MAP_7(SEP, F, __VA_ARGS__)
#define CX2_MAP_9(SEP, F, X, ...)  F(X) SEP CX2_MAP_8(SEP, F, __VA_ARGS__)
#define CX2_MAP_10(SEP, F, X, ...) F(X) SEP CX2_MAP_9(SEP, F, __VA_ARGS__)
#define CX2_MAP_11(SEP, F, X, ...) F(X) SEP CX2_MAP_10(SEP, F, __VA_ARGS__)
#define CX2_MAP_12(SEP, F, X, ...) F(X) SEP CX2_MAP_11(SEP, F, __VA_ARGS__)
#define CX2_MAP_13(SEP, F, X, ...) F(X) SEP CX2_MAP_12(SEP, F, __VA_ARGS__)
#define CX2_MAP_14(SEP, F, X, ...) F(X) SEP CX2_MAP_13(SEP, F, __VA_ARGS__)
#define CX2_MAP_15(SEP, F, X, ...) F(X) SEP CX2_MAP_14(SEP, F, __VA_ARGS__)
#define CX2_MAP_16(SEP, F, X, ...) F(X) SEP CX2_MAP_15(SEP, F, __VA_ARGS__)
#define CX2_MAP_17(SEP, F, X, ...) F(X) SEP CX2_MAP_16(SEP, F, __VA_ARGS__)
#define CX2_MAP_18(SEP, F, X, ...) F(X) SEP CX2_MAP_17(SEP, F, __VA_ARGS__)
#define CX2_MAP_19(SEP, F, X, ...) F(X) SEP CX2_MAP_18(SEP, F, __VA_ARGS__)
#define CX2_MAP_20(SEP, F, X, ...) F(X) SEP CX2_MAP_19(SEP, F, __VA_ARGS__)
#define CX2_MAP_21(SEP, F, X, ...) F(X) SEP CX2_MAP_20(SEP, F, __VA_ARGS__)
#define CX2_MAP_22(SEP, F, X, ...) F(X) SEP CX2_MAP_21(SEP, F, __VA_ARGS__)
#define CX2_MAP_23(SEP, F, X, ...) F(X) SEP CX2_MAP_22(SEP, F, __VA_ARGS__)
#define CX2_MAP_24(SEP, F, X, ...) F(X) SEP CX2_MAP_23(SEP, F, __VA_ARGS__)
#define CX2_MAP_25(SEP, F, X, ...) F(X) SEP CX2_MAP_24(SEP, F, __VA_ARGS__)
#define CX2_MAP_26(SEP, F, X, ...) F(X) SEP CX2_MAP_25(SEP, F, __VA_ARGS__)
#define CX2_MAP_27(SEP, F, X, ...) F(X) SEP CX2_MAP_26(SEP, F, __VA_ARGS__)
#define CX2_MAP_28(SEP, F, X, ...) F(X) SEP CX2_MAP_27(SEP, F, __VA_ARGS__)
#define CX2_MAP_29(SEP, F, X, ...) F(X) SEP CX2_MAP_28(SEP, F, __VA_ARGS__)
#define CX2_MAP_30(SEP, F, X, ...) F(X) SEP CX2_MAP_29(SEP, F, __VA_ARGS__)
#define CX2_MAP_31(SEP, F, X, ...) F(X) SEP CX2_MAP_30(SEP, F, __VA_ARGS__)
#define CX2_MAP_32(SEP, F, X, ...) F(X) SEP CX2_MAP_31(SEP, F, __VA_ARGS__)
#define CX2_MAP_33(SEP, F, X, ...) F(X) SEP CX2_MAP_32(SEP, F, __VA_ARGS__)
#define CX2_MAP_34(SEP, F, X, ...) F(X) SEP CX2_MAP_33(SEP, F, __VA_ARGS__)
#define CX2_MAP_35(SEP, F, X, ...) F(X) SEP CX2_MAP_34(SEP, F, __VA_ARGS__)
#define CX2_MAP_36(SEP, F, X, ...) F(X) SEP CX2_MAP_35(SEP, F, __VA_ARGS__)
#define CX2_MAP_37(SEP, F, X, ...) F(X) SEP CX2_MAP_36(SEP, F, __VA_ARGS__)
#define CX2_MAP_38(SEP, F, X, ...) F(X) SEP CX2_MAP_37(SEP, F, __VA_ARGS__)
#define CX2_MAP_39(SEP, F, X, ...) F(X) SEP CX2_MAP_38(SEP, F, __VA_ARGS__)
#define CX2_MAP_40(SEP, F, X, ...) F(X) SEP CX2_MAP_39(SEP, F, __VA_ARGS__)
#define CX2_MAP_41(SEP, F, X, ...) F(X) SEP CX2_MAP_40(SEP, F, __VA_ARGS__)
#define CX2_MAP_42(SEP, F, X, ...) F(X) SEP CX2_MAP_41(SEP, F, __VA_ARGS__)
#define CX2_MAP_43(SEP, F, X, ...) F(X) SEP CX2_MAP_42(SEP, F, __VA_ARGS__)
#define CX2_MAP_44(SEP, F, X, ...) F(X) SEP CX2_MAP_43(SEP, F, __VA_ARGS__)
#define CX2_MAP_45(SEP, F, X, ...) F(X) SEP CX2_MAP_44(SEP, F, __VA_ARGS__)
#define CX2_MAP_46(SEP, F, X, ...) F(X) SEP CX2_MAP_45(SEP, F, __VA_ARGS__)
#define CX2_MAP_47(SEP, F, X, ...) F(X) SEP CX2_MAP_46(SEP, F, __VA_ARGS__)
#define CX2_MAP_48(SEP, F, X, ...) F(X) SEP CX2_MAP_47(SEP, F, __VA_ARGS__)
#define CX2_MAP_49(SEP, F, X, ...) F(X) SEP CX2_MAP_48(SEP, F, __VA_ARGS__)
#define CX2_MAP_50(SEP, F, X, ...) F(X) SEP CX2_MAP_49(SEP, F, __VA_ARGS__)
#define CX2_MAP_51(SEP, F, X, ...) F(X) SEP CX2_MAP_50(SEP, F, __VA_ARGS__)
#define CX2_MAP_52(SEP, F, X, ...) F(X) SEP CX2_MAP_51(SEP, F, __VA_ARGS__)
#define CX2_MAP_53(SEP, F, X, ...) F(X) SEP CX2_MAP_52(SEP, F, __VA_ARGS__)
#define CX2_MAP_54(SEP, F, X, ...) F(X) SEP CX2_MAP_53(SEP, F, __VA_ARGS__)
#define CX2_MAP_55(SEP, F, X, ...) F(X) SEP CX2_MAP_54(SEP, F, __VA_ARGS__)
#define CX2_MAP_56(SEP, F, X, ...) F(X) SEP CX2_MAP_55(SEP, F, __VA_ARGS__)
#define CX2_MAP_57(SEP, F, X, ...) F(X) SEP CX2_MAP_56(SEP, F, __VA_ARGS__)
#define CX2_MAP_58(SEP, F, X, ...) F(X) SEP CX2_MAP_57(SEP, F, __VA_ARGS__)
#define CX2_MAP_59(SEP, F, X, ...) F(X) SEP CX2_MAP_58(SEP, F, __VA_ARGS__)
#define CX2_MAP_60(SEP, F, X, ...) F(X) SEP CX2_MAP_59(SEP, F, __VA_ARGS__)

// Expand to SK if ID defined as empty, i.e. "#define ID", else FK
#define CX2_IF_NIL(ID, SK, FK)     CX2_IF_NIL1(CX2_IS_EMPTY(ID), SK, FK)
#define CX2_IF_NIL1(...)           CX2_IF_NIL2(__VA_ARGS__)
#define CX2_IF_NIL2(BOOL, SK, FK)  CX2_IF_NIL3_##BOOL(SK, FK)
#define CX2_IF_NIL3_1(SK, FK)      SK
#define CX2_IF_NIL3_0(SK, FK)      FK

#define CX2_NO_WARN(OPT, ...)                                        \
  _Pragma("GCC diagnostic push")                                     \
      _Pragma(CX2_TO_STRING(CX2_PRAGMA_GCC_DIAGNOSTIC_IGNORED(OPT))) \
          __VA_ARGS__                                                \
              _Pragma("GCC diagnostic pop")
#define CX2_PRAGMA_GCC_DIAGNOSTIC_IGNORED(OPT) GCC diagnostic ignored OPT

#ifdef __cplusplus
}
#endif
#endif  // CX2_MACRO_UTILS_H_
