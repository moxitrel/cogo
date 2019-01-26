/* See gen_label.h

* Internal
** Overview
    switch (pc) {
    case  0:        // begin
        ...

        pc = 11;    //
        return;     // yield and restore
    case 11:        //
        ...

        pc = N;     //
        return;     // yield and restore
    case  N:        //
        ...
    }

** Source
void f(gen_t *co)
{
    co_begin(co);       // coroutine begin

    for (co->i = 0; co->i < 9; co->i++) {
        co_yield(co);   // yield
    }

    co_end(co);         // coroutine end
}

** Expand Macro
void f(gen_t *co)
{
 //
 // co_begin(co);
 //
    switch (co->pc) {
    case  0:                // coroutine begin
        for (co->i = 0; co->i < 9; co->i++) {
         //
         // co_yield(co);
         //
            co->pc = 11;    // 1. save restore point, next call will be "case 11:"
            return;         // 2. yield
    case 11:;               // 3. put a case after each return as restore point
        }

 //
 // co_end(co);
 //
    }
}

*/
#ifndef COGOTO_GEN_H
#define COGOTO_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// generator context
typedef struct {
    int pc;
} gen_t;

#define GEN_PC(CO)      (((gen_t *)(CO))->pc)

inline static int co_state(const gen_t *const co)
{
    assert(co);
    return co->pc;
}

// co_begin(gen_t *);
#define co_begin(CO, ...)                               \
    switch (GEN_PC(CO)) {                               \
    default:                /* invalid _pc      */      \
        assert(((void)"pc isn't valid.", 0));           \
        goto CO_END;                                    \
    case -1:                /* coroutine end    */      \
        goto CO_END;                                    \
    case  0:                /* coroutine begin  */      \

// co_yield(gen_t *);
#define co_yield(CO, ...)                                                               \
do {                                                                                    \
        __VA_ARGS__;            /* run before return, intent for handle return value */ \
        GEN_PC(CO) = __LINE__;  /* 1. save the restore point, at case __LINE__: */      \
        goto CO_END;            /* 2. return */                                         \
    case __LINE__:;             /* 3. put case after each *return* as restore point */  \
} while (0)

// co_return(gen_t *);
#define co_return(CO, ...)                                                              \
do {                                                                                    \
        __VA_ARGS__;            /* run before return, intent for handle return value */ \
        goto CO_RETURN;         /* return */                                            \
} while (0)

// co_end(gen_t *)
#define co_end(CO)                          \
    CO_RETURN:                              \
        GEN_PC(CO) = -1;   /* finish */     \
    CO_END:;                                \
    }

#endif // COGOTO_GEN_H
