#ifndef CO_SWITCH_GOTO_HPP
#define CO_SWITCH_GOTO_HPP

#include "../memory.h"
#include <stdint.h>

// co_t: coroutine context, must be inherited (as first field) by user-defined type.
// e.g.
//   typedef struct {
//     co_t co;
//     ...
//   } user_defined_t;
typedef struct co_t co_t;
typedef void (*co_fun_t)(co_t *);

class co_t {
protected:
    // save the start point where coroutine continue to run when yield
    // >=0: running
    //  -1: finished
    intptr_t pc;

    // when coroutine finished, run caller next
    co_t *caller;
    // coroutine called by co_call()
    co_t *callee;

    int state();
    void add_callee(co_t &callee);

public:
    co_t() = default;

    // coroutine function
    virtual void operator()() = 0;

    // Loop call operator() function until finished.
    friend void co_run(co_t &&);
};

inline int co_t::state()
{
    return this->pc;
}

inline void co_t::add_callee(co_t &callee)
{
    this->callee  = &callee;
    callee.caller = this;
}

//
// co_begin(), co_end(), co_return(), co_call() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. RETURN_LABEL(13)       -> return_13
//      RETURN_LABEL(__LINE__) -> return_118
#define RETURN_LABEL(N)     RETURN_LABEL_(N)
#define RETURN_LABEL_(N)    RETURN_##N

#define CASE_GOTO(N)        case N: goto RETURN_LABEL(N);

// co_t::co_begin(...);
#define co_begin(...)                               \
do {                                                \
    const intptr_t pc = this->pc;                   \
    switch (pc) {                                   \
    case  0: break;         /* coroutine begin */   \
 /* case  N: goto return_N;  */                     \
 /* ...                      */                     \
    MAP(CASE_GOTO, __VA_ARGS__);                    \
    case -1: goto finally;  /* coroutine end   */   \
    default: FATAL("pc:%td isn't valid.", pc);      \
    }                                               \
} while (0)


// Yield from the coroutine.
// co_t::co_return();
#define co_return(...)                                                                  \
    __VA_ARGS__;                /* run before return, intent for handle return value */ \
    this->pc = __LINE__;        /* 1. save the restore point, at label return_N */      \
    goto finally;               /* 2. return */                                         \
RETURN_LABEL(__LINE__):         /* 3. put label after each *return* as restore point */ \


// co_t::co_end()
#define co_end()                            \
    this->pc = -1;   /* finish */           \
finally:                                    \


// Call another coroutine.
// co_t::co_call(co_t &&callee);
#define co_call(CALLEE)                     \
    this->add_callee((co_t *)(CALLEE));     \
    co_return()


// Loop running the coroutine function until finished.
inline void co_run(co_t &&co_)
{
    co_t *co = &co_;
    for (;;) {
        if (co->pc < 0) {    // finished
            // stop or return to caller
            if (co->caller == NULL) {
                return;
            } else {
                co = co->caller;
                co->callee = NULL;
            }
        } else if (co->callee != NULL) { // call another coroutine
            co = co->callee;
        } else {    // run continue once
            co->operator()();
        }
    }
}



// Count the number of arguments.
// e.g. LEN(A)       -> 1
//      LEN(A,B)     -> 2
//      LEN(A,B,C,D) -> 4
//
// BUG: LEN() -> 1, excepted 0
#define LEN(...)        LEN_PATT_(__VA_ARGS__, LEN_PATT_PADDING)
#define LEN_PATT_(...)  LEN_PATT(__VA_ARGS__)
#define LEN_PATT_PADDING                                    \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0
#define LEN_PATT(                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N


#define MAP(F, ...)       MAP_N_(LEN(__VA_ARGS__), F, __VA_ARGS__)
#define MAP_N_(...)       MAP_N(__VA_ARGS__)
#define MAP_N(N, F, ...)  MAP_##N(F, __VA_ARGS__)
#define MAP_0( F, ...)
#define MAP_1( F, X, ...) F(X) MAP_0( F, __VA_ARGS__)
#define MAP_2( F, X, ...) F(X) MAP_1( F, __VA_ARGS__)
#define MAP_3( F, X, ...) F(X) MAP_2( F, __VA_ARGS__)
#define MAP_4( F, X, ...) F(X) MAP_3( F, __VA_ARGS__)
#define MAP_5( F, X, ...) F(X) MAP_4( F, __VA_ARGS__)
#define MAP_6( F, X, ...) F(X) MAP_5( F, __VA_ARGS__)
#define MAP_7( F, X, ...) F(X) MAP_6( F, __VA_ARGS__)
#define MAP_8( F, X, ...) F(X) MAP_7( F, __VA_ARGS__)
#define MAP_9( F, X, ...) F(X) MAP_8( F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X) MAP_9( F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X) MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X) MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X) MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X) MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X) MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X) MAP_15(F, __VA_ARGS__)
#define MAP_17(F, X, ...) F(X) MAP_16(F, __VA_ARGS__)
#define MAP_18(F, X, ...) F(X) MAP_17(F, __VA_ARGS__)
#define MAP_19(F, X, ...) F(X) MAP_18(F, __VA_ARGS__)

#endif // CO_SWITCH_GOTO_HPP
