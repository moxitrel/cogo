/* Use Duff's Device (Protothreads)

* API
co_this     : point to coroutine object.
CO_BEGIN    : coroutine begin label.
CO_YIELD    : yield from coroutine.
CO_RETURN   : return from coroutine.
CO_END      : coroutine end label.

co_status() : get the current running status.
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
#ifndef COGO_YIELD_CASE_H_
#define COGO_YIELD_CASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

typedef int cogo_pc_t;
#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)

// implement yield
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  >0: running
  //   0: inited
  //  -1: finished
  cogo_pc_t pc;
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_PC(COGO_YIELD) (((cogo_yield_t*)(COGO_YIELD))->pc)

#define CO_STATUS_BEGIN     COGO_PC_BEGIN
#define CO_STATUS_END       COGO_PC_END
// get the current running state
static inline cogo_pc_t co_status(cogo_yield_t const* const co) {
  COGO_ASSERT(co);
  return co->pc;
}

#define COGO_BEGIN(COGO_YIELD)                           \
  switch (COGO_PC(COGO_YIELD)) {                         \
    default: /* invalid  pc */                           \
      COGO_ASSERT(((void)"pc isn't valid", 0));          \
      goto cogo_end;                                     \
      goto cogo_return; /* HACK: no warn unused label */ \
    case COGO_PC_END:   /* coroutine end */              \
      goto cogo_end;                                     \
    case COGO_PC_BEGIN /* coroutine begin */

#define COGO_YIELD(COGO_YIELD)                                                            \
  do {                                                                                    \
    /**/ COGO_PC(COGO_YIELD) = __LINE__; /* 1. save the restore point (case __LINE__:) */ \
    /**/ goto cogo_end;                  /* 2. return */                                  \
    case __LINE__:;                      /* 3. restore point */                           \
  } while (0)

#define COGO_RETURN(COGO_YIELD) goto cogo_return /* end coroutine */

#define COGO_END(COGO_YIELD)              \
  cogo_return:                            \
  /**/ COGO_PC(COGO_YIELD) = COGO_PC_END; \
  }                                       \
  cogo_end

#define CO_BEGIN  COGO_BEGIN(co_this)
#define CO_END    COGO_END(co_this)
#define CO_YIELD  COGO_YIELD(co_this)
#define CO_RETURN COGO_RETURN(co_this)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_CASE_H_