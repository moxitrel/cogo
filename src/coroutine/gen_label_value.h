#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

typedef struct {
    // save the start point where coroutine continue to run when yield
    const void *pc;
    //  >0: running
    //   0: inited
    //  <0: finished
    int state;
} gen_t;

// return gen_t.pc
#define GEN_PC(CO)      (((gen_t *)(CO))->pc)
// return gen_t.state
#define GEN_STATE(CO)   (((gen_t *)(CO))->state)

inline static int co_state(const gen_t *const co)
{
    assert(co)
    return co->state;
}

//
// co_begin(), co_end(), co_return() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_RETURN_13
//      CO_LABEL(__LINE__) -> CO_RETURN_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_RETURN_##N


// co_begin(co_t *, ...);
#define co_begin(CO, ...)                           \
do {                                                \
    gen_t *const _co = (gen_t *)(CO);               \
    switch (_co->state) {                           \
    case  0:                /* coroutine begin */   \
        _co->state = 1;                             \
        break;                                      \
 /* case -1:              *//* coroutine end   */   \
 /*     goto CO_END;      */                        \
    default:                                        \
        goto *_co->pc;                              \
    }                                               \
} while (0)


// Yield from the coroutine.
// co_return(co_t *);
#define co_return(CO, ...)                                                                      \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    GEN_PC(CO) = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_RETURN_N */   \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):                     /* 3. put label after each *return* as restore point */ \


// co_end(co_t *)
#define co_end(CO)                          \
    GEN_PC(CO) = &&CO_END;                  \
    GEN_STATE(CO) = -1;    /* finish */     \
CO_END:                                     \


// Count the number of arguments.
// e.g. LEN(A)       -> 1
//      LEN(A,B)     -> 2
//      LEN(A,B,C,D) -> 4
//
#define ARG_PAT(                                            \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N
#define LEN_PADDING                                         \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0
#define BOOL_PADDING                                        \
      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,       \
      1,   1,   1,   1,   1,   1,   1,   1,   1,   0
#define ARG_PAT_(...)       ARG_PAT(__VA_ARGS__)
#define GET_COMMA(...)      ,

#define LEN(...)            ARG_PAT_(__VA_ARGS__, LEN_PADDING)


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

#endif //COROUTINE_GEN_H
