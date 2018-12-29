/*

* Example
#include "gen_switch.hpp"

// Must inherit gen_t
struct T : public gen_t {
    // Declare variables for coroutine function
    int i;

    // Define coroutine function with the return type void
    void f(...)
    {
        // coroutine begin
        co_begin(17,20);    // 17,20: the line numbers of co_yield(), i.e. the value of __LINE__

        // user codes
        for (i = 0; i < 9; i++) {
            printf("%d\n", i);
            co_yield();    // yield
        }
        printf("%d\n", i);
        co_yield();

        // coroutine end
        co_end();
    }
};

void example()
{
    T gen;

    // gen.state(): return the running state
    //  >=0: running
    //   <0: finished
    while (gen.state() >= 0) {
        gen.f(...);
    }
}


* Internal
void T::f(...)  
{
    //
    // co_begin(17,20)
    //
    switch (get_t::_pc) {         // where to continue
    case  0:  break;              // coroutine begin
    case 17:  goto CO_YIELD_17;  // restore
    case 20:  goto CO_YIELD_20;  // restore
    default:  return;             // coroutine end
    }

    for (i = 0; i < 9; i++) {
        printf("%d\n", i);
        //
        // co_yield();
        //
        get_t::_pc = 17;    // 1. save restore point, next call will be "case 17: goto CO_YIELD_17"
        return;             // 2. return
CO_YIELD_17:;              // 3. put a label after each return as restore point
    }
    printf("%d\n", i);
    //
    // co_yield();
    //
    get_t::_pc = 20;        // 1. save restore point, next call will be "case 20: goto CO_YIELD_20"
    return;                 // 2. return
CO_YIELD_20:;              // 3. label the restore point

    //
    // co_end();
    //
    get_t::_pc = -1;
}


* Reference
- Coroutines in C (https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html)
- Protothreads (http://dunkels.com/adam/pt/)

*/
#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: generator context.
//  .state() -> int: return the running state.
class gen_t {
protected:
    // Start point where coroutine continue to run after yield.
    //   0: inited
    //  >0: running
    //  <0: finished (-1: ok, -2: invalid _pc)
    int _pc = 0;
public:
    // Get the running state.
    int state() const
    {
        return _pc;
    }
};

//
// co_begin(), co_end(), co_yield() are not expressions. They are statements.
//

// Generate switch case clause. (case N: goto CO_YIELD_N)
// e.g. CO_LABEL(13)        -> CO_YIELD_13
//      CASE_GOTO(__LINE__) -> case 118: goto CO_YIELD_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_YIELD_##N
#define CASE_GOTO(N)    case N: goto CO_LABEL(N)


// Mark coroutine begin.
// gen_t::co_begin(...);
#define co_begin(...)                                   \
do {                                                    \
    switch (gen_t::_pc) {                               \
    case  0:                /* coroutine begin */       \
        break;                                          \
 /* case -1:              *//* coroutine end   */       \
 /*     goto CO_END;      */                            \
 /* case  N:              */                            \
 /*     goto CO_YIELD_N; */                             \
    MAP(CASE_GOTO, __VA_ARGS__);                        \
    default:                /* invalid _pc     */       \
 /*     gen_t::_pc = -2; */                             \
 /*     assert(((void)"_pc isn't valid.", false)); */   \
        goto CO_END;                                    \
    }                                                   \
} while (0)


// Yield from the coroutine.
// gen_t::co_yield();
#define co_yield(...)                                                                           \
do {                                                                                            \
    __VA_ARGS__;                /* run before return, intent for handle return value */         \
    gen_t::_pc = __LINE__;      /* 1. save the restore point, at label CO_LABEL(__LINE__) */    \
    goto CO_END;                /* 2. return */                                                 \
CO_LABEL(__LINE__):;            /* 3. put a label after each return as restore point */         \
} while (0)


// Mark coroutine end.
// gen_t::co_end()
#define co_end()                                            \
do {                                                        \
    gen_t::_pc = -1;   /* finish coroutine successfully */  \
CO_END:;                                                    \
} while (0)


// Count the number of arguments. (BUG)
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

//
// Count the number of arguments. (Solved the LEN1(...)'s BUG)
//
// e.g. LEN()       -> LEN_(0,1,1) -> LEN_01(1) -> 0
//      LEN(1)      -> LEN_(0,0,1) -> LEN_00(1) -> 1
//      LEN(1,2)    -> LEN_(1,1,2) -> LEN_11(2) -> 2
//      LEN(1,2,3)  -> LEN_(1,1,3) -> LEN_11(3) -> 3
//      LEN(1,2,...)-> LEN_(1,1,N) -> LEN_11(N) -> N
//
// SEE: https://stackoverflow.com/questions/11317474/macro-to-count-number-of-arguments
//      http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
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
