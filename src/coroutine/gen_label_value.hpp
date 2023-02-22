#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

// gen_t: coroutine context.
class gen_t {
protected:
    // save the start point where coroutine continue to run when yield
    void *_pc;
    //  >0: running
    //   0: inited
    //  <0: finished
    int _state = 0;
public:
    // Get the running state.
    int state() const 
    {
        return _state;
    }
};

//
// co_begin(), co_end(), co_return() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_RETURN_13
//      CO_LABEL(__LINE__) -> CO_RETURN_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_RETURN_##N


// co_t::co_begin(...);
#define co_begin(...)                               \
    switch (gen_t::_state) {                        \
    case  0:                /* coroutine begin */   \
        gen_t::_state = 1;                          \
        break;                                      \
 /* case -1:              *//* coroutine end   */   \
 /*     goto CO_END;      */                        \
    default:                                        \
        goto *gen_t::_pc;                           \
    }                                               \


// Yield from the coroutine. (yield)
// co_t::co_return();
#define co_return(...)                                                                          \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    gen_t::_pc = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_RETURN_N */   \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):                     /* 3. put label after each *return* as restore point */ \


// co_t::co_end()
#define co_end()                            \
    gen_t::_pc = &&CO_END;                  \
    gen_t::_state = -1;    /* finish */     \
CO_END:                                     \



// Count the number of arguments.
// e.g. LEN(1)      -> 1
//      LEN(1,2)    -> 2
//      LEN(1,2,3)  -> 3
// BUG: LEN1()      -> 1, expect 0
#define LEN1(...)       ARG_PAT(__VA_ARGS__, LEN_PADDING)

#define ARG_PAT(...)    ARG_PAT_(__VA_ARGS__)
#define ARG_PAT_(                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N
#define LEN_PADDING                                         \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0

// Count the number of arguments. (Solve the LEN1(...)'s BUG)
//
// e.g. LEN()       -> LEN_(0,1,1) -> LEN_01(1) -> 0
//      LEN(1)      -> LEN_(0,0,1) -> LEN_00(1) -> 1
//      LEN(1,2)    -> LEN_(1,1,2) -> LEN_11(2) -> 2
//      LEN(1,2,3)  -> LEN_(1,1,3) -> LEN_11(3) -> 3
//      LEN(1,2,...)-> LEN_(1,1,N) -> LEN_11(N) -> N
//
// SEE: http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
//      P99, advanced macro tricks (http://p99.gforge.inria.fr/p99-html/index.html)
//
#define LEN(...)                            \
LEN_(                                       \
    HAS_COMMA(__VA_ARGS__),                 \
    HAS_COMMA(GET_COMMA __VA_ARGS__ ()),    \
    LEN1(__VA_ARGS__)                       \
)
#define LEN_(D1, D2, N)     LEN_01N(D1, D2, N)
#define LEN_01N(D1, D2, N)  LEN_##D1##D2(N)
#define LEN_01(N)           0
#define LEN_00(N)           1
#define LEN_11(N)           N

#define HAS_COMMA_PADDING                                   \
      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,       \
      1,   1,   1,   1,   1,   1,   1,   1,   0,   0
#define HAS_COMMA(...)      ARG_PAT(__VA_ARGS__, HAS_COMMA_PADDING)
#define GET_COMMA(...)      ,


#define MAP(F, ...)       MAP_N_(LEN(__VA_ARGS__), F, __VA_ARGS__)
#define MAP_N_(...)       MAP_N(__VA_ARGS__)
#define MAP_N(N, F, ...)  MAP_##N(F, __VA_ARGS__)
#define MAP_0( F, ...)
#define MAP_1( F, X, ...) F(X)
#define MAP_2( F, X, ...) F(X); MAP_1( F, __VA_ARGS__)
#define MAP_3( F, X, ...) F(X); MAP_2( F, __VA_ARGS__)
#define MAP_4( F, X, ...) F(X); MAP_3( F, __VA_ARGS__)
#define MAP_5( F, X, ...) F(X); MAP_4( F, __VA_ARGS__)
#define MAP_6( F, X, ...) F(X); MAP_5( F, __VA_ARGS__)
#define MAP_7( F, X, ...) F(X); MAP_6( F, __VA_ARGS__)
#define MAP_8( F, X, ...) F(X); MAP_7( F, __VA_ARGS__)
#define MAP_9( F, X, ...) F(X); MAP_8( F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X); MAP_9( F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X); MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X); MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X); MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X); MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X); MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X); MAP_15(F, __VA_ARGS__)
#define MAP_17(F, X, ...) F(X); MAP_16(F, __VA_ARGS__)
#define MAP_18(F, X, ...) F(X); MAP_17(F, __VA_ARGS__)
#define MAP_19(F, X, ...) F(X); MAP_18(F, __VA_ARGS__)

#endif // COROUTINE_GEN_H
