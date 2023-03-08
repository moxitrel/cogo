/* Use Duff's Device (Protothreads)

* API
CO_BEGIN      : coroutine begin label.
CO_END        : coroutine end label.
CO_YIELD      : yield from coroutine.
CO_RETURN     : return from coroutine.
co_this       : point to coroutine object.
cogo_status() : get the current running status.
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
#ifndef COGO_COGO_YIELD_CASE_H_
#define COGO_COGO_YIELD_CASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

typedef unsigned cogo_pc_t;
#define CO_STATUS_INIT ((cogo_pc_t)0)
#define CO_STATUS_FINI ((cogo_pc_t)-1)

// yield context
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  >0: running
  //   0: inited
  //  -1: finished
  cogo_pc_t pc;
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_PC (((cogo_yield_t*)co_this)->pc)

// get the current running state
static inline cogo_pc_t cogo_status(void const* const co) {
  COGO_ASSERT(co);
  return ((cogo_yield_t const*)co)->pc;
}

#define CO_BEGIN                                          \
  switch (cogo_status(co_this)) {                         \
    default: /* invalid  pc */                            \
      COGO_ASSERT(((void)"pc isn't valid", 0));           \
      goto cogo_exit;                                     \
      goto cogo_return;  /* HACK: no warn unused label */ \
    case CO_STATUS_FINI: /* coroutine end */              \
      goto cogo_exit;                                     \
    case CO_STATUS_INIT /* coroutine begin */

#define CO_YIELD                                                                \
  do {                                                                          \
    /**/ COGO_PC = __LINE__; /* 1. save the restore point, at case __LINE__: */ \
    /**/ goto cogo_exit;     /* 2. return */                                    \
    case __LINE__:;          /* 3. restore point */                             \
  } while (0)

#define CO_RETURN goto cogo_return /* fini coroutine */

#define CO_END                   \
  cogo_return:                   \
  /**/ COGO_PC = CO_STATUS_FINI; \
  }                              \
  cogo_exit

#ifdef __cplusplus
}
#endif
#endif  // COGO_COGO_YIELD_CASE_H_
