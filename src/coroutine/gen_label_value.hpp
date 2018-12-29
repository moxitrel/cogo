#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

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
// co_begin(), co_end(), co_yield() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_YIELD_13
//      CO_LABEL(__LINE__) -> CO_YIELD_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_YIELD_##N


// co_t::co_begin(...);
#define co_begin(...)                               \
do {                                                \
    switch (gen_t::_state) {                        \
    case  0:                /* coroutine begin */   \
        gen_t::_state = 1;                          \
        break;                                      \
 /* case -1:              *//* coroutine end   */   \
 /*     goto CO_END;      */                        \
    default:                                        \
        goto *gen_t::_pc;                           \
    }                                               \
} while (0)


// Yield from the coroutine. (yield)
// co_t::co_yield();
#define co_yield(...)                                                                           \
do {                                                                                            \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    gen_t::_pc = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_YIELD_N */    \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):;                    /* 3. put label after each *return* as restore point */ \
} while (0)


// co_t::co_end()
#define co_end()                            \
do {                                        \
    gen_t::_pc = &&CO_END;                  \
    gen_t::_state = -1;    /* finish */     \
CO_END:;                                    \
} while (0)

#endif // COROUTINE_GEN_H
