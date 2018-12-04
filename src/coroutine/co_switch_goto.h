/*

co_begin()      : 协程开始
co_end()        : 协程结束
co_return()     : 返回
co_state()      : 获取协程运行状态
                    >=0: 正在运行
                    < 0: 运行结束
*** 用法

// 1. 包含头文件
#include "co_switch_goto.h"

// 2. 自定义协程结构 (局部变量, 返回值), 必须继承 co_t
typedef struct {
    // must inherit co_t (put co_t first).
    co_t co_t;

    // user definitions
    ...
} fun_t;

// 3. 定义协程函数, 类型必须为 void(co_t *)
void fun(co_t *co)
{
    //
    // co_begin()之前的代码，每次调用都会被执行
    //

    // 将参数 co 转换为自定义类型 fun_t
    fun_t *arg = (fun_t *)co;

    // 别名引用
    T *x = &arg->x;
    T *v = &arg->v;

    //
    // 协程开始
    //
    co_begin(co, 35,39,...); // 35,39, ...: 列出所有 co_return(), co_call() 所在的行号, 即 __LINE__ 的值


    //
    // 用户代码:  !!! 禁用局部变量 !!!
    //
    co_return(co);          // 返回，下次被调用，从此处开始执行，***局部变量无法被恢复***


    //
    // 协程结束
    //
    co_end(co);


    //
    // co_end()之后的代码，每次co_return()返回前 都会被执行
    //
}

*/
#ifndef CO_SWITCH_GOTO_H
#define CO_SWITCH_GOTO_H

#include "../stack.h"

// The base structure of coroutine context.
//
// co_t must be inherited (as first field) by user-defined type.
// e.g.
//   typedef struct {
//     co_t co_t;
//     ...
//   } user_defined_t;
//
typedef struct co_t {
    // coroutine function
    void (*_fun)(struct co_t *);

    // save the restore point
    // >=0: running
    // < 0: finish
    int _state;

    // when self finished, run _caller next
    struct co_t *_caller;
    // when co_call(), pause self, run _callee until finished
    struct co_t *_callee;
    // the coroutines run concurrently with me
    struct co_t *_concurrent;
} co_t;

// get co_t._state
#define co_state(CO)    ((int)                      {((co_t *)(CO)) -> _state })
// get co_t._fun
#define co__fun(CO)     ((void (*)(struct co_t *))  {((co_t *)(CO)) -> _fun   })
// get co_t._caller
#define co__caller(CO)  ((co_t *)                   {((co_t *)(CO)) -> _caller})
// get co_t._callee
#define co__callee(CO)  ((co_t *)                   {((co_t *)(CO)) -> _callee})


//
// co_begin(), co_end(), co_return(), co_call() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. RETURN_LINE(13)       -> return_13
//      RETURN_LINE(__LINE__) -> return_108
#define RETURN_LINE(N)      RETURN_LINE_(N)
#define RETURN_LINE_(N)     return_##N

#define CASE_GOTO(N)        case N: goto RETURN_LINE(N);


#define co_begin(CO, ...)               \
    switch (co_state(CO)) {             \
    default:    /* invalid state */     \
        ASSERT(0, "co_state():%d isn't valid.", co_state(CO)); \
        break;                          \
    case -1: goto finally;  /* end   */ \
    case  0: break;         /* begin */ \
 /* case  N: goto return_N;  */         \
 /* ...                      */         \
    MAP(CASE_GOTO, __VA_ARGS__);        \
    }                                   \


#define co_return(CO, ...)                                                                          \
    __VA_ARGS__;                            /* run before return, intent for handle return value */ \
    (((co_t *)(CO))->_state) = __LINE__;    /* 1. set the restore point, at lable return_N */       \
    goto finally;                           /* 2. return */                                         \
RETURN_LINE(__LINE__):                      /* 3. put label after each *return* as restore point */ \


#define co_end(CO)                      \
    (((co_t *)(CO))->_state) = -1;      \
finally:                                \


#define co_call(CO, CALLEE)                             \
    ((co_t *)(CO))     -> _callee = (co_t *)(CALLEE);   \
    ((co_t *)(CALLEE)) -> _caller = (co_t *)(CO);       \
    co_return(CO)                                       \



// Count the number of arguments.
// e.g. LEN(A)       -> 1
//      LEN(A,B)     -> 2
//      LEN(A,B,C,D) -> 4
//
// BUG: LEN() -> 1, excepted 0
#define LEN(...)        LEN_PATT_(__VA_ARGS__, LEN_PATT_PADDING)
#define LEN_PATT_(...)  LEN_PATT(__VA_ARGS__)
#define LEN_PATT_PADDING                                    \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0
#define LEN_PATT(                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N


#define MAP(F, ...)       MAP_N_(LEN(__VA_ARGS__), F, __VA_ARGS__)
#define MAP_N_(...)       MAP_N(__VA_ARGS__)
#define MAP_N(N, F, ...)  MAP_##N(F, __VA_ARGS__)
#define MAP_0( F, ...)
#define MAP_1( F, X, ...) F(X) MAP_0( F, __VA_ARGS__)
#define MAP_2( F, X, ...) F(X) MAP_1( F, __VA_ARGS__)
#define MAP_3( F, X, ...) F(X) MAP_2( F, __VA_ARGS__)
#define MAP_4( F, X, ...) F(X) MAP_3( F, __VA_ARGS__)
#define MAP_5( F, X, ...) F(X) MAP_4( F, __VA_ARGS__)
#define MAP_6( F, X, ...) F(X) MAP_5( F, __VA_ARGS__)
#define MAP_7( F, X, ...) F(X) MAP_6( F, __VA_ARGS__)
#define MAP_8( F, X, ...) F(X) MAP_7( F, __VA_ARGS__)
#define MAP_9( F, X, ...) F(X) MAP_8( F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X) MAP_9( F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X) MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X) MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X) MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X) MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X) MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X) MAP_15(F, __VA_ARGS__)
#define MAP_17(F, X, ...) F(X) MAP_16(F, __VA_ARGS__)
#define MAP_18(F, X, ...) F(X) MAP_17(F, __VA_ARGS__)
#define MAP_19(F, X, ...) F(X) MAP_18(F, __VA_ARGS__)

////////////////////////////////////////////////////////////////


/*_Thread_local*/ Stack coStack;
/*_Thread_local*/ void *tmp;

#define CO_SCHED(F, ...)                                        \
(   tmp = Stack_Emplace(&coStack, sizeof(F##_ctx_t)) -> ptr     \
,   *(F##_ctx_t *)tmp = (F##_ctx_t){{.fun = (F)}, __VA_ARGS__}  \
,   ctx->ctx = (co_t *)tmp                                     \
)
#define AWAIT(F, ...)           \
{                               \
    CO_SCHED(F, __VA_ARGS__);   \
    co_return();                    \
}
#define CO_VAL(F, X)    (& ((const F##_ctx_t *)ctx->ctx)->X )

#define CO_RUN(F, ...)                                          \
co_t *ctx = (co_t *)&(F##_ctx_t){};                           \
{                                                               \
    CO_SCHED(F, __VA_ARGS__);                                   \
    while (Stack_Size(&coStack) > 0) {                          \
        co_t *ctx =  Stack_Top(&coStack) -> ptr;               \
        ctx->fun(ctx);                                          \
        if (ctx->_state < 0) {                                   \
            Stack_Pop(&coStack);                                \
        }                                                       \
    }                                                           \
    Stack_Gc(&coStack, Object_Gc);                              \
}

#endif // CO_SWITCH_GOTO_H
