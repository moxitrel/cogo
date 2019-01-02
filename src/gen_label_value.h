#ifndef COGOTO_GEN_H
#define COGOTO_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: coroutine context.
typedef struct {
    // save the start point where coroutine continue to run when yield
    const void *pc;

    //  >0: running
    //   0: inited
    //  <0: finished
    int state;
} gen_t;

// gen_t.pc
#define GEN_PC(CO)      (((gen_t *)(CO))->pc)
// gen_t.state
#define GEN_STATE(CO)   (((gen_t *)(CO))->state)

inline static int co_state(const gen_t *const co)
{
    assert(co)
    return co->state;
}


// co_begin(gen_t *, ...);
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


// Yield from the coroutine. (yield)
// co_yield(gen_t *);
#define co_yield(CO, ...)                                                                       \
do {                                                                                            \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    GEN_PC(CO) = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_YIELD_N */    \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):;                    /* 3. put label after each *return* as restore point */ \
} while (0)


// co_return(gen_t *,);
#define co_return(CO, ...)                                                                      \
do {                                                                                            \
    __VA_ARGS__;                /* run before return, intent for handle return value */         \
    GEN_PC(CO) = &&CO_END;      /* 1. set coroutine end */                                      \
    GEN_STATE(CO) = -1;         /*    set coroutine end */                                      \
    goto CO_END;                /* 2. return */                                                 \
} while (0)


// co_end(gen_t *)
#define co_end(CO)                          \
do {                                        \
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
