/* Use Duff's Device (Protothreads)

* API
co_this     : point to coroutine object.
CO_BEGIN    : coroutine begin label.
CO_END      : coroutine end label.
CO_YIELD    : yield from coroutine.
CO_RETURN   : return from coroutine.

co_status_t : type of CO_STATUS().
CO_STATUS() : get the current running status.
  CO_STATUS_BEGIN: inited
  CO_STATUS_END  : finished
  >0             : running

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
#ifndef COGO_YIELD_SWITCH_H_
#define COGO_YIELD_SWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

typedef int co_status_t;
#define CO_STATUS_BEGIN 0
#define CO_STATUS_END   (-1)

// implement yield
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  >0: running
  //   0: inited
  //  -1: finished
  co_status_t pc;

  // the coroutine function
  void (*resume)(void* co_this);
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_PC(CO)   (((cogo_yield_t*)(CO))->pc)

// get the current running state (rvalue)
#define CO_STATUS(CO) ((co_status_t)COGO_PC(CO))

#define COGO_BEGIN(CO)                                                \
  switch (CO_STATUS(CO)) {                                            \
    default: /* invalid pc */                                         \
      COGO_ASSERT(((void)"pc isn't valid", 0));                       \
      goto cogo_end;                                                  \
      goto cogo_return; /* HACK: eliminate warning of unused label */ \
    case CO_STATUS_END: /* coroutine end */                           \
      goto cogo_end;                                                  \
    case CO_STATUS_BEGIN /* coroutine begin */

#define COGO_YIELD(CO)                                                            \
  do {                                                                            \
    /**/ COGO_PC(CO) = __LINE__; /* 1. save the restore point (case __LINE__:) */ \
    /**/ goto cogo_end;          /* 2. return */                                  \
    case __LINE__:;              /* 3. restore point */                           \
  } while (0)

#define COGO_RETURN(CO) \
  goto cogo_return /* end coroutine */

#define COGO_END(CO)                \
  cogo_return:                      \
  /**/ COGO_PC(CO) = CO_STATUS_END; \
  }                                 \
  cogo_end

#define CO_BEGIN  COGO_BEGIN(co_this)
#define CO_END    COGO_END(co_this)
#define CO_YIELD  COGO_YIELD(co_this)
#define CO_RETURN COGO_RETURN(co_this)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_SWITCH_H_
