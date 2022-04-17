/* Labels as Values (GCC Extension)

* Example
void nat_func(nat_t* CO_THIS)
{
CO_BEGIN:

    for (CO_THIS->i = 0; ;CO_THIS->i++) {
        CO_YIELD;
    }

CO_END:;
}

* Internal
void nat_func(nat_t* CO_THIS)
{
    if (CO_THIS->pc) {          //
        goto *CO_THIS->pc;      // CO_BEGIN:
    }                           //

    for (CO_THIS->i = 0; ;CO_THIS->i++) {

        pc = &&yield_11;        //
        return;                 // CO_YIELD;
    yield_11:;                  //

    }

    pc = &&yield_end;           // CO_END:
yield_end:;                     //
}

* Drawbacks
- Use GCC extension.

*/
// clang-format off
#ifndef MOXITREL_COGO_YIELD_IMPL_H_
#define MOXITREL_COGO_YIELD_IMPL_H_
#include <stdint.h>

#ifdef assert
    #define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
    #define COGO_ASSERT(...) /* nop */
#endif

#define COGO_STATUS_STARTED     0
#define COGO_STATUS_STOPPED     -1

// yield context
typedef struct cogo_yield {
    // start point where coroutine function continue to run after yield.
    //  0: inited
    // -1: finish successfully
    const void *cogo_pc;
} cogo_yield_t;

// cogo_yield_t.cogo_pc
#define COGO_PC       (((cogo_yield_t *)(CO_THIS))->cogo_pc)

// get the current running state
#define CO_STATUS(CO) ((intptr_t)((cogo_yield_t *)(CO))->cogo_pc)

#define CO_BEGIN                                                \
    switch(CO_STATUS(CO_THIS)) {                                \
    case COGO_STATUS_STARTED:                                   \
        goto cogo_enter;                                        \
        COGO_PC = &&cogo_enter;  /* remove compiler error */    \
    case COGO_STATUS_STOPPED:                                   \
        goto cogo_exit;                                         \
    default:                                                    \
        goto *COGO_PC;                                          \
    }                                                           \
    cogo_enter

#define CO_YIELD                                                \
    do {                                                        \
        COGO_PC = &&COGO_LABEL; /* 1. save restore point */     \
        goto cogo_exit;         /* 2. return */                 \
    COGO_LABEL:;                /* 3. restore point */          \
    } while (0)

#define CO_RETURN                                               \
    goto cogo_return            /* end coroutine */

#define CO_END                                                  \
    cogo_return:                                                \
        COGO_PC = (const void *)COGO_STATUS_STOPPED;            \
    cogo_exit

// Make goto label.
// e.g. COGO_LABEL(13)       -> cogo_yield_13
//      COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

#endif  // MOXITREL_COGO_YIELD_IMPL_H_
