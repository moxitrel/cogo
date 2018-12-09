/*

CO(FUN)     : 新建协程
co_begin()  : 协程开始
co_end()    : 协程结束
co_return() : 返回
co_call()   : 调用协程
co_state()  : 获取协程运行状态
              >=0: 正在运行
               -1: 运行结束

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
    assert(co);
    
    // 别名关联
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

    for (; co->i < 9; co->i ++) {
        v = x;
        co_return(co);
    }

    v += 1;
    co_return(co);

    //
    // 协程结束
    //
    co_end(co);


    //
    // co_end()之后的代码，每次co_return()返回前 都会被执行
    //
}

// 4. define initializer
void fun_init(fun_t *o, co_fun_t fun,...)
{
    assert(o);
    assert(fun);

    *o = (fun_t){
        // init co_t
        .co = CO(fun),

        // init user definitions
        ...
    };
}

 *** 原理: 将 *栈变量* 保存到堆上,

// 展开后
void coroutine(coroutine_t *co) // coroutine_t 由自己定义, 可添加任意字段
{
  // co_begin();
  switch (co->pc) {             // co->pc 存储 从哪里开始继续运行
  case 0:    break;             // 协程开始
  case 19:   goto return_19;    // 还原点
  case 23:   goto return_23;    // 还原点
  case 26:   goto return_26;    // 还原点
  case -1:   return;            // 协程结束
  }


  // co_return()
  co->pc = 19;      // 1. save restore point, next call will be case 19: goto return_19
  return;           // 2. return
return_19:;         // 3. put a label after each *return* as the restore point

  for (; co->i < 9; co->i ++) {
    co->v = co->x;

    // co_return()
    co->pc = 23;    // 1. save restore point, next call will be case 23: goto return_23
    return;         // 2. return
return_23:;         // 3. label the restore point
  }

  co->v += 1;

  // co_return()
  co->pc = 26;      // 1. save restore point, next call will be case 26: goto return_26
  return;           // 2. return
return_26:;         // 3. label the restore point

  // co_end()
  co->pc = -1;      //协程运行结束
}

*/
#ifndef CO_SWITCH_GOTO_H
#define CO_SWITCH_GOTO_H

#include "../memory.h"
#include <stdint.h>

//
// API is *** not type safe ***
//

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

    // save the start point where coroutine continue to run when yield
    // >=0: running
    //  -1: finished
    intptr_t pc;

    // when fun finished, run caller next
    co_t *caller;
    // coroutine called by co_call()
    co_t *callee;
};

// make a co_t in stack
#define CO(FUN)         ((co_t){.fun = (co_fun_t)(FUN),})
// return co_t.fun
#define CO_FUN(CO)      (((co_t *)(CO))->fun)
// return co_t.pc
#define CO_PC(CO)       (((co_t *)(CO))->pc)
// return co_t.caller
#define CO_CALLER(CO)   (((co_t *)(CO))->caller)
// return co_t.callee
#define CO_CALLEE(CO)   (((co_t *)(CO))->callee)

// return co_t.pc
inline static int co_state(const void *co)
{
    assert(co);
    return CO_PC(co);
}

inline static co_t *co_add_callee(co_t *co, co_t *callee)
{
    assert(co);
    assert(callee);
    CO_CALLEE(co)     = callee;
    CO_CALLER(callee) = co;
    return co;
}

//
// co_begin(), co_end(), co_return(), co_call() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. RETURN_LABEL(13)       -> return_13
//      RETURN_LABEL(__LINE__) -> return_118
#define RETURN_LABEL(N)     RETURN_LABEL_(N)
#define RETURN_LABEL_(N)    RETURN_##N

#define CASE_GOTO(N)        case N: goto RETURN_LABEL(N);

// co_begin(co_t *, ...);
#define co_begin(CO, ...)                           \
do {                                                \
    const intptr_t pc = CO_PC(CO);                  \
    switch (pc) {                                   \
    case  0: break;         /* coroutine begin */   \
 /* case  N: goto return_N;  */                     \
 /* ...                      */                     \
    MAP(CASE_GOTO, __VA_ARGS__);                    \
    case -1: goto finally;  /* coroutine end   */   \
    default: FATAL("pc:%td isn't valid.", pc);      \
    }                                               \
} while (0)

// Yield from the coroutine.
// co_return(co_t *);
#define co_return(CO, ...)                                                              \
    __VA_ARGS__;                /* run before return, intent for handle return value */ \
    CO_PC(CO) = __LINE__;       /* 1. save the restore point, at label return_N */      \
    goto finally;               /* 2. return */                                         \
RETURN_LABEL(__LINE__):;        /* 3. put label after each *return* as restore point */ \

// co_end(co_t *)
#define co_end(CO)                          \
    CO_PC(CO) = -1;   /* finish */          \
finally:;                                   \

// Call another coroutine.
// co_call(co_t *co, co_t *callee);
#define co_call(CO, CALLEE)                 \
    co_return(co_add_callee((co_t *)(CO), (co_t *)(CALLEE)))

// Loop running the coroutine until finished.
inline static void co_run(void *co)
{
    for (;;) {
        if (CO_PC(co) < 0) {    // finished
            // stop or return to caller
            if (CO_CALLER(co) == NULL) {
                return;
            } else {
                co = CO_CALLER(co);
                CO_CALLEE(co) = NULL;
            }
        } else if (CO_CALLEE(co) != NULL) { // call another coroutine
            co = CO_CALLEE(co);
        } else {    // continue run
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
