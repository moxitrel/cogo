#ifndef CO_GOTO_H
#define CO_GOTO_H

#include "_m_list.h"
#include "../stack.h"

// CO_BEGIN(), YIELD(), CO_END() 三者不是表达式, 必须作为独立的语句使用

typedef struct ctx_t {
    // coroutine function
    void (*fun)(struct ctx_t *);

    // save the restore point
    // >=0: running
    // < 0: finish
    int yield;

    // child coroutine context
    struct ctx_t *ctx;
} ctx_t;


// 定义协程函数
#define CO_FUN(F, ...)    \
typedef struct {          \
    ctx_t ctx_t;          \
    MAP(ID, __VA_ARGS__); \
} F##_ctx_t;              \
void F(ctx_t *ctx)

// 返回 实参X 的指针
#define CO_ARG(F, X)    (& ((F##_ctx_t *)ctx)->X)

#define CO_BEGIN(...)                   \
    switch (ctx->yield) {               \
    case 0: break;          /* begin */ \
 /* case N: goto yield_N;   */          \
 /* ...                     */          \
    MAP(CASE_GOTO, __VA_ARGS__);        \
    default: goto finally;  /* end */   \
    }                                   \

#define YIELD(...)                                                                 \
    __VA_ARGS__;           /* run before return, intent for handle return value */ \
    ctx->yield = __LINE__; /* 1. set the restore point, at lable yield_N */        \
    goto finally;          /* 2. return */                                         \
LABEL(__LINE__):           /* 3. put label after each *return* as restore point */ \

#define CO_END()                         \
    ctx->yield = -1;                     \
finally:                                 \


/*_Thread_local*/ Stack coStack;
/*_Thread_local*/ void *tmp;

#define CO_SCHED(F, ...)                                        \
(   tmp = Stack_Emplace(&coStack, sizeof(F##_ctx_t)) -> ptr     \
,   *(F##_ctx_t *)tmp = (F##_ctx_t){{.fun = (F)}, __VA_ARGS__}  \
,   ctx->ctx = (ctx_t *)tmp                                     \
)
#define AWAIT(F, ...)           \
{                               \
    CO_SCHED(F, __VA_ARGS__);   \
    YIELD();                    \
}
#define CO_VAL(F, X)    (& ((const F##_ctx_t *)ctx->ctx)->X )

#define CO_RUN(F, ...)                                          \
ctx_t *ctx = (ctx_t *)&(F##_ctx_t){};                           \
{                                                               \
    CO_SCHED(F, __VA_ARGS__);                                   \
    while (Stack_Size(&coStack) > 0) {                          \
        ctx_t *ctx =  Stack_Top(&coStack) -> ptr;               \
        ctx->fun(ctx);                                          \
        if (ctx->yield < 0) {                                   \
            Stack_Pop(&coStack);                                \
        }                                                       \
    }                                                           \
    Stack_Gc(&coStack, Object_Gc);                              \
}

#define ID(X) X;
#define CASE_GOTO(N)    case N: goto yield_##N;

#define LABEL(N)        LABEL_(N)
#define LABEL_(N)       yield_##N

#endif // CO_GOTO_H
