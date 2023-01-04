/* Use Duff's Device (Protothreads)

* API
CO_BEGIN      : coroutine begin label.
CO_END        : coroutine end label.
CO_YIELD      : yield from coroutine.
CO_RETURN     : return from coroutine.
CO_THIS       : point to coroutine object.
CO_STATUS(CO) : get the current running status.
    >0: running
     0: inited
    -1: finished

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
    switch (CO_THIS->pc) {          // CO_BEGIN:
    case  0:                        //

        for (CO_THIS->i = 0; ;CO_THIS->i++) {

            CO_THIS->pc = 11;       //
            return;                 // CO_YIELD;
    case 11:;                       //

        }

    }                               // CO_END:
}

* Drawbacks
- No CO_YIELD allowed in *case* statement.

* See Also
- Coroutines in C   (https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html)
- Protothreads      (http://dunkels.com/adam/pt/expansion.html)

*/
// clang-format off
#ifndef COGO_COGO_YIELD_IMPL_H_
#define COGO_COGO_YIELD_IMPL_H_

#ifdef assert
    #define COGO_ASSERT(...)    assert(__VA_ARGS__)
#else
    #define COGO_ASSERT(...)    /* nop */
#endif

#define COGO_STATUS_STARTED     0
#define COGO_STATUS_STOPPED     -1

// yield context
typedef struct cogo_yield {
    // start point where coroutine function continue to run after yield.
    //  >0: running
    //   0: inited
    //  -1: finished
    int cogo_pc;
} cogo_yield_t;

// cogo_yield_t.cogo_pc
#define COGO_PC         (((cogo_yield_t*)(CO_THIS))->cogo_pc)

// get the current running state
#define CO_STATUS(CO)   (((cogo_yield_t*)(CO))->cogo_pc)

#define CO_BEGIN                                        \
    switch (CO_STATUS(CO_THIS)) {                       \
    default:                    /* invalid  pc */       \
        COGO_ASSERT(((void)"cogo_pc isn't valid", 0));  \
        goto cogo_exit;                                 \
    case COGO_STATUS_STOPPED:   /* coroutine end */     \
        goto cogo_exit;                                 \
    case COGO_STATUS_STARTED    /* coroutine begin */

#define CO_YIELD                                                                    \
    do {                                                                            \
        COGO_PC = __LINE__;     /* 1. save the restore point, at case __LINE__: */  \
        goto cogo_exit;         /* 2. return */                                     \
    case __LINE__:;             /* 3. restore point */                              \
    } while (0)

#define CO_RETURN                                       \
    goto cogo_return            /* end coroutine */

#define CO_END                                          \
    cogo_return:                                        \
        COGO_PC = COGO_STATUS_STOPPED;                  \
    }                                                   \
    cogo_exit

#endif /* COGO_COGO_YIELD_IMPL_H_ */