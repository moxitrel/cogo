#ifndef COGO_GEN_H
#define COGO_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: coroutine context.
class gen_t {
protected:
    // save the start point where coroutine continue to run when yield
    const void *_pc = nullptr;
    
    //   0: inited
    //  >0: running
    //  <0: stopped
    int _state = 0;
public:
    // get the running state.
    int state() const 
    {
        return _state;
    }
};


// gen_t::co_begin();
#define co_begin(...)                               \
do {                                                \
    if (gen_t::_pc) {                               \
        goto *gen_t::_pc;                           \
    }                                               \
    gen_t::_state = 1;                              \
} while (0)


// gen_t::co_yield();
#define co_yield(...)                                                                           \
do {                                                                                            \
    __VA_ARGS__;                        /* run before return, intent for handle return value */ \
    gen_t::_pc = &&CO_LABEL(__LINE__);  /* 1. save the restore point, at label CO_YIELD_N */    \
    goto CO_END;                        /* 2. return */                                         \
CO_LABEL(__LINE__):;                    /* 3. put label after each *return* as restore point */ \
} while (0)


// gen_t::co_return();
#define co_return(...)                                                                          \
do {                                                                                            \
    __VA_ARGS__;                /* run before return, intent for handle return value */         \
    goto CO_RETURN;             /* return */                                                    \
} while (0)


// gen_t::co_end()
#define co_end(...)                         \
do {                                        \
CO_RETURN:                                  \
    gen_t::_pc = &&CO_END;                  \
    gen_t::_state = -1;    /* finish */     \
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

#endif // COGO_GEN_H
