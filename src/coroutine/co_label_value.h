#ifndef CO_LABEL_VALUE_H
#define CO_LABEL_VALUE_H

#include "../memory.h"

// co_t: coroutine context, must be inherited (as first field) by user-defined type.
// e.g.
//   typedef struct {
//     co_t co;
//     ...
//   } user_defined_t;
typedef struct co_t co_t;
typedef void (*co_fun_t)(co_t *);
struct co_t {
    // coroutine function
    co_fun_t fun;

    // coroutine state
    // >=0: running
    // < 0: finished
    int state;

    // save the restore point
    void *pc;

    // when fun finished, run caller next
    co_t *caller;
    // coroutine called by co_call()
    co_t *callee;
};

#define CO(FUN)         ((co_t){.fun = (co_fun_t)(FUN),})
#define CO_FUN(CO)      (((co_t *)(CO))->fun)
#define CO_STATE(CO)    (((co_t *)(CO))->state)
#define CO_PC(CO)       (((co_t *)(CO))->pc)
#define CO_CALLER(CO)   (((co_t *)(CO))->caller)
#define CO_CALLEE(CO)   (((co_t *)(CO))->callee)

// get co_t.state
inline static int co_state(const void *o)
{
    assert(o);
    return CO_STATE(o);
}

inline static co_t *co_add_callee(co_t *co, co_t *callee)
{
    assert(co);
    assert(callee);
    CO_CALLEE(co)     = callee;
    CO_CALLER(callee) = co;
    return co;
}

//
// co_begin(), co_end(), co_return(), co_call() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto pc.
// e.g. RETURN_LABEL(13)       -> return_13
//      RETURN_LABEL(__LINE__) -> return_118
#define RETURN_LABEL(N)     RETURN_LABEL_(N)
#define RETURN_LABEL_(N)    return_##N

#define co_begin(CO, ...)                           \
do {                                                \
    const co_t *const _co = (co_t *)(CO);           \
    assert(_co);                                    \
                                                    \
    switch (CO_STATE(_co)) {                        \
    case  0:                /* coroutine begin */   \
        CO_STATE(_co) = 1;                          \
        break;                                      \
    case -1:                                        \
        goto finally;  /* coroutine end   */        \
    default:                                        \
        goto *CO_PC(_co);                           \
    }                                               \
} while (0)

#define co_return(CO, ...)                                                                              \
    __VA_ARGS__;                                /* run before return, intent for handle return value */ \
    CO_PC(CO) = &&RETURN_LABEL(__LINE__);       /* 1. set the restore point, at lable return_N */       \
    goto finally;                               /* 2. return */                                         \
RETURN_LABEL(__LINE__):                         /* 3. put pc after each *return* as restore point */    \

#define co_end(CO)                          \
    CO_STATE(CO) = -1;                      \
finally:                                    \


#define co_call(CO, CALLEE)                 \
    co_return(co_add_callee((co_t *)(CO), (co_t *)(CALLEE)))


inline static void co_run(void *co)
{
    for (;;) {
        if (CO_STATE(co) < 0) {
            if (CO_CALLER(co) == NULL) {
                return;
            }
            co = CO_CALLER(co);
            CO_CALLEE(co) = NULL;
        } else if (CO_CALLEE(co) != NULL) {
            co = CO_CALLEE(co);
        } else {
            CO_FUN(co) ((co_t *)co);
        }
    }
}

#endif // CO_LABEL_VALUE_H
