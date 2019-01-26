/*

* Internal

    if (pc) {
        goto *pc;
    }

    ...
    pc = &&yield_11;    //
    return;             // yield and restore
yield_11:               //

    ...
    pc = &&yield_N;     //
    return;             // yield and restore
yield_N:                //

** Source
void f(gen_t *co)
{
    co_begin(co);       // coroutine begin

    for (co->i = 0; ; co->i++) {
        co_yield(co);   // yield
    }

    co_end(co);         // coroutine end
}

** Expand Macro
void f(gen_t *co)
{
 //
 // co_begin(co);
 //
    if (co->pc) {
        goto *co->pc;
    }
    for (co->i = 0; ; co->i++) {
     //
     // co_yield(co);
     //
        pc = &&yield_11;    // 1. save restore point, next call will be "yield_11:"
        return;             // 2. yield
    yield_11:;              // 3. put a label after each return as restore point
    }

 //
 // co_end(co);
 //
    pc = &&yield_end;
yield_end:;
}

*/
#ifndef COGOTO_GEN_H
#define COGOTO_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: generator context.
typedef struct {
    // start point where coroutine function continue to run after yield.
    const void *pc;

    //   0: inited
    //  >0: running
    //  <0: stopped (-1: success)
    int state;
} gen_t;

// gen_t.pc
#define GEN_PC(CO)      (((gen_t *)(CO))->pc)
// gen_t.state
#define GEN_STATE(CO)   (((gen_t *)(CO))->state)

inline static int co_state(const gen_t *const co)
{
    assert(co);
    return co->state;
}


// co_begin(gen_t *);
#define co_begin(CO, ...)                           \
do {                                                \
    const void *_pc = GEN_PC(CO);                   \
    if (_pc) {                                      \
        goto *_pc;                                  \
    }                                               \
    GEN_STATE(CO) = 1;                              \
} while (0)


// co_yield(gen_t *);
#define co_yield(CO, ...)                                                                       \
do {                                                                                            \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    GEN_PC(CO) = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_YIELD_N */    \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):;                    /* 3. put label after each *return* as restore point */ \
} while (0)


// co_return(gen_t *);
#define co_return(CO, ...)                                                                      \
do {                                                                                            \
    __VA_ARGS__;                /* run before return, intent for handle return value */         \
    goto CO_RETURN;             /* return */                                                    \
} while (0)


// co_end(gen_t *)
#define co_end(CO)                          \
do {                                        \
CO_RETURN:                                  \
    GEN_PC(CO) = &&CO_END;                  \
    GEN_STATE(CO) = -1;    /* finish */     \
CO_END:;                                    \
} while (0)


//
// Helper Macros
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_YIELD_13
//      CO_LABEL(__LINE__) -> CO_YIELD_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_YIELD_##N

#endif //COGOTO_GEN_H
