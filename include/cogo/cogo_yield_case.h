// clang-format off
/* Use Duff's Device (Protothreads)

* API
CO_BEGIN      : coroutine begin label.
CO_END        : coroutine end label.
CO_YIELD      : yield from coroutine.
CO_RETURN     : return from coroutine.
co_this       : point to coroutine object.
co_status()   : get the current running status.
    >0: running
     0: inited
    -1: finished

* Example
void nat_func(nat_t* co_this)
{
CO_BEGIN:

    for (co_this->i = 0; ;co_this->i++) {
        CO_YIELD;
    }

CO_END:;
}

* Internal
void nat_func(nat_t* co_this)
{
    switch (co_this->pc) {          // CO_BEGIN:
    case  0:                        //

        for (co_this->i = 0; ;co_this->i++) {

            co_this->pc = 11;       //
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
#ifndef COGO_COGO_YIELD_IMPL_H_
#define COGO_COGO_YIELD_IMPL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
    #define COGO_ASSERT(...)    assert(__VA_ARGS__)
#else
    #define COGO_ASSERT(...)    /*noop*/
#endif

#define COGO_STATUS_INITED      0
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
#define COGO_PC                                         \
_Pragma("GCC diagnostic push")                          \
_Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")  \
    (((cogo_yield_t*)(co_this))->cogo_pc)               \
_Pragma("GCC diagnostic pop")

// get the current running state
static inline int co_status(void* co)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    return ((cogo_yield_t*)co)->cogo_pc;
#pragma GCC diagnostic pop
}

#define CO_BEGIN                                        \
    switch (co_status(co_this)) {                       \
    default:                    /* invalid  pc */       \
        COGO_ASSERT(((void)"cogo_pc isn't valid", 0));  \
        goto cogo_exit;                                 \
    case COGO_STATUS_STOPPED:   /* coroutine end */     \
        goto cogo_exit;                                 \
    case COGO_STATUS_INITED     /* coroutine begin */

#define CO_YIELD                                                                    \
    do {                                                                            \
        COGO_PC = __LINE__;     /* 1. save the restore point, at case __LINE__: */  \
        goto cogo_exit;         /* 2. return */                                     \
    case __LINE__:;             /* 3. restore point */                              \
    } while (0)

#define CO_RETURN                                       \
    goto cogo_return            /* end coroutine */

#define CO_END                                          \
_Pragma("GCC diagnostic push")                          \
_Pragma("GCC diagnostic ignored \"-Wunused-label\"")    \
    cogo_return:                                        \
_Pragma("GCC diagnostic pop")                           \
        COGO_PC = COGO_STATUS_STOPPED;                  \
    }                                                   \
    cogo_exit

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_YIELD_IMPL_H_ */
