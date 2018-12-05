/*

co_begin()  : 协程开始
co_end()    : 协程结束
co_return() : 返回
co_state()  : 获取协程运行状态
                >=0: 正在运行
                < 0: 运行结束
*** 用法

// 1. 包含头文件
#include "co_switch_goto.h"

// 2. 自定义协程结构 (局部变量, 返回值), 必须继承 co_t
typedef struct {
    // must inherit co_t (put co_t first).
    co_t co;

    // user definitions
    ...
} fun_t;

// 3. 定义协程函数, 类型必须为 void(co_t *)
void fun(fun_t *co)
{
    //
    // co_begin()之前的代码，每次调用都会被执行
    //

    // 设置别名
    T &x = co->x;
    T &v = co->v;

    //
    // 协程开始
    //
    co_begin(co,35,39,...); // 35,39, ...: 列出所有 co_return(), co_call() 所在的行号, 即 __LINE__ 的值


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

// 4. define initializer
void *fun_init(fun_t *o, ...)
{
    *o = (fun_t){
        // init co_t
        .co = CO(fun),

        // init user definitions
        ...
    };
}

*/
#ifndef CO_SWITCH_GOTO_H
#define CO_SWITCH_GOTO_H

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
    void *label;

    // when fun finished, run caller next
    co_t *caller;
    // coroutine called by co_call()
    co_t *callee;
    // the coroutines run concurrently with me
    //co_t *concurrent;
};

// make a new co_t in stack
#define CO(FUN)         ((co_t){.fun = (co_fun_t)(FUN),})
// get co_t.fun
#define CO_FUN(CO)      (((co_t *)(CO))->fun)
// get co_t.state
#define CO_STATE(CO)    (((co_t *)(CO))->state)
// get co_t.label
#define CO_LABEL(CO)    (((co_t *)(CO))->label)
// get co_t.caller
#define CO_CALLER(CO)   (((co_t *)(CO))->caller)
// get co_t.callee
#define CO_CALLEE(CO)   (((co_t *)(CO))->callee)

// get co_t.state
inline static int co_state(const void *o)
{
    assert(o);
    return CO_STATE(o);
}

//
// co_begin(), co_end(), co_return(), co_call() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. RETURN_LABEL(13)       -> return_13
//      RETURN_LABEL(__LINE__) -> return_118
#define RETURN_LABEL(N)     RETURN_LABEL_(N)
#define RETURN_LABEL_(N)    return_##N

#define CASE_GOTO(N)        case N: goto RETURN_LABEL(N);


#define co_begin(CO, ...)                           \
do {                                                \
    const co_t *_co = (co_t *)(CO);                 \
    switch (CO_STATE(_co)) {                        \
    case -1:                                        \
        goto finally;  /* coroutine end   */        \
    case  0:                /* coroutine begin */   \
        CO_STATE(CO) = 1;                           \
        break;                                      \
    default:                                        \
        goto *CO_LABEL(_co);                        \
    }                                               \
} while (0)

#define co_return(CO, ...)                                                                              \
    __VA_ARGS__;                                /* run before return, intent for handle return value */ \
    CO_LABEL(CO) = &&RETURN_LABEL(__LINE__);    /* 1. set the restore point, at lable return_N */       \
    goto finally;                               /* 2. return */                                         \
RETURN_LABEL(__LINE__):                         /* 3. put label after each *return* as restore point */ \

#define co_end(CO)                          \
    CO_STATE(CO) = -1;                      \
    finally:                                \

#define co_call(CO, CALLEE)                 \
do {                                        \
    co_t *_co           = (co_t *)(CO);     \
    co_t *_callee       = (co_t *)(CALLEE); \
    CO_CALLEE(_co)      = _callee;          \
    CO_CALLER(_callee)  = _co;              \
    co_return(_co);                         \
} while (0)


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

#endif // CO_SWITCH_GOTO_H
