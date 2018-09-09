/*

CO_BEGIN(...)   : 协程开始
CO_END()        : 协程结束
YIELD()         : 返回
ctx             : 协程函数的参数名，用于存储局部变量

CO_BEGIN(), YIELD(), CO_END() 三者不是表达式, 必须作为独立的语句使用


**** 原理: 将 *栈变量* 保存到 *堆* 上

// 展开前
void coroutine(coroutine_ctx_t *ctx)
{
  CO_BEGIN(19,23,26,27);    // 协程开始
                            // 19,23,...: YIELD()所在的行号, 即 __LINE__ 的值

  YIELD();          // 第1处返回，下次被调用，从此处开始执行

  for (; ctx->i < 9; ctx->i ++) {
    YIELD();        // 第2处返回，下次被调用，从此处开始执行
  }

  YIELD();          // 第3处返回，下次被调用，从此处开始执行
  YIELD();          // 第4处返回，下次被调用，从此处开始执行

  CO_END();         // 协程结束
}


// 展开后
void coroutine(coroutine_ctx_t *ctx)    // coroutine_ctx_t 类型由自己定义, 可添加任意字段
{
  // CO_BEGIN(19,23,26,27);
  switch (ctx->yield) {         // ctx->yield 存储 从哪里开始继续运行
  case 0:    break;             // 协程开始
  case 19:   goto yield_19;     // YIELD()
  case 23:   goto yield_23;     // YIELD()
  case 26:   goto yield_26;     // YIELD()
  case 27:   goto yield_27;     // YIELD()
  default:   return;            // 协程结束
  }


  // YIELD()
  ctx->yield = 10;  // 1. save restore point, next call will be case 19: goto yield_19
  return;           // 2. return
yield_19:;          // 3. put a label after each *return* as the restore point

  for (; ctx->i < 9; ctx->i ++) {
    // YIELD()
    ctx->yield = 23;// 1. save restore point, next call will be case 23: goto yield_23
    return;         // 2. return
yield_23:;          // 3. label the restore point
  }

  // YIELD()
  ctx->yield = 26;  // 1. save restore point, next call will be case 26: goto yield_26
  return;           // 2. return
yield_26:;          // 3. label the restore point

  // YIELD()
  ctx->yield = 27;  // 1. save restore point, next call will be case 4: goto yield_4
  return;           // 2. return
yield_27:;          // 3. label the restore point

  // CO_END()
  ctx->yield = -1;      //协程运行结束
}

**** 用法1:

// 1. 包含头文件
#include "co_goto.h"


// 2. 定义 协程 参数类型
typedef struct {

  // 2.1. ***必须继承 CoroutineContext
  CoroutineContext;

  // 其他可选, 定义 局部变量, 返回值, ...
  T x;
  T v;

} coroutine_ctx_t;

// 3. 定义协程
// ***函数类型必须为 void(coroutine_ctx_t *ctx);
// ***参数名  必须为 ctx
void coroutine(coroutine_ctx_t *ctx)
{

    //
    // CO_BEGIN(...) 之前的代码，每次调用都会被执行
    // Run every time when the function called, intent for var alias,
    // e.g. T *x = &ctx->x;
    // ...
    //


    //
    // 协程开始
    //
    CO_BEGIN(N, ...);   // N, ...: YIELD()所在的行号, 即 __LINE__


    //
    // 用户代码
    //
    // YIELD();
    // ...
    // YIELD();
    // ...
    //


    //
    // 协程结束
    //
    CO_END();


    //
    // CO_END() 之后的代码，每次YIELD()返回前 都会被执行
    //
}

**** 用法2:   另见 co_goto_test.c

// 1. 包含头文件
#include "co_goto.h"

// 2. 定义协程 coroutine, 包含2个变量 x, v
CO_DEFINE(coroutine, T x, T v)
{
    //
    // 函数体含义同上
    //
    // 引用 x 需要使用: ctx->x
    // 引用 v 需要使用: ctx->v
    //
}

*/
#ifndef CO_GOTO_H
#define CO_GOTO_H

#include "_list.h"


#define CoroutineContext                        \
  struct {                                      \
    /* save the restore point */                \
    /*  0: ready */                             \
    /* >0: running */                           \
    /* <0: finish */                            \
    int   yield;                                \
  }


#define CASE_GOTO(N)    case N: goto yield_##N;
#define CO_BEGIN(...)                                                               \
do {                                                                                \
    switch (ctx->yield) {                                                           \
    case 0:   break;        /* begin */                                             \
    /* case N: goto yield_N; */                                                     \
    /* ...                   */                                                     \
    MAP(CASE_GOTO, __VA_ARGS__);                                                    \
    default:  goto finally; /* end */                                               \
    }                                                                               \
} while (0)


#define LABEL_(N)   yield_##N
#define LABEL(N)    LABEL_(N)
#define YIELD(...)                                                                      \
do {                                                                                    \
    ctx->yield = __LINE__;      /* 1. set the restore point, at lable yield_N */        \
    __VA_ARGS__;                /* run before return, intent for handle return value */ \
    goto finally;               /* 2. return */                                         \
LABEL(__LINE__):;               /* 3. put label after each *return* as restore point */ \
} while (0)


#define CO_END(...)                                             \
do {                                                            \
    ctx->yield = -1;                                            \
finally:                                                        \
    __VA_ARGS__;            /* run before return */             \
} while(0)


#define ID(X)   X;
#define CO_DEFINE(F, ...)           \
    typedef struct {                \
        CoroutineContext;           \
                                    \
        MAP(ID, __VA_ARGS__);       \
    } F##_ctx_t;                    \
                                    \
    void F(F##_ctx_t *ctx)

#endif //CO_GOTO_H
