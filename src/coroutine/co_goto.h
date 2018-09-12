/*

CO_BEGIN(...)   : 协程开始
CO_END()        : 协程结束
YIELD()         : 返回

CO_ARG(F, X)    : 返回协程函数 F 实参 X 的指针


*** 用法: 另见 co_goto_test.c

// 1. 包含头文件
#include "co_goto.h"

// 2. 定义协程: 函数名为 f, 包含2个参数 x,v
CO_FUNC(f, T x, T v)
{
    //
    // CO_BEGIN(...) 之前的代码，每次调用都会被执行
    //
    T *x = CO_ARG(f, x);    // 获取实参 x 的指针
    T *v = CO_ARG(f, v);    // 获取实参 v 的指针

    //
    // 协程开始
    //
    CO_BEGIN(35,39,42);   // 33,39, ...: 列出所有 YIELD() 所在的行号, 即
__LINE__ 的值


    //
    // 用户代码
    //
    for (*x = 0; *x < 9; (*x)++) {
        *v = *x;
        YIELD();    // 返回，下次被调用，从此处开始执行
    }

    *v += 1;
    YIELD();        // 返回，下次被调用，从此处开始执行

    *v += 1;
    YIELD();        // 返回，下次被调用，从此处开始执行


    //
    // 协程结束
    //
    CO_END();


    //
    // CO_END() 之后的代码，每次YIELD()返回前 都会被执行
    //
}


*** 原理: 将 *栈变量* 保存到 *堆* 上

// 展开后
void coroutine(coroutine_ctx_t *ctx)    // coroutine_ctx_t 类型由自己定义,
可添加任意字段
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
  ctx->yield = 10;  // 1. save restore point, next call will be case 19: goto
yield_19 return;           // 2. return yield_19:;          // 3. put a label
after each *return* as the restore point

  for (; ctx->i < 9; ctx->i ++) {
    // YIELD()
    ctx->yield = 23;// 1. save restore point, next call will be case 23: goto
yield_23 return;         // 2. return yield_23:;          // 3. label the
restore point
  }

  // YIELD()
  ctx->yield = 26;  // 1. save restore point, next call will be case 26: goto
yield_26 return;           // 2. return yield_26:;          // 3. label the
restore point

  // YIELD()
  ctx->yield = 27;  // 1. save restore point, next call will be case 4: goto
yield_4 return;           // 2. return yield_27:;          // 3. label the
restore point

  // CO_END()
  ctx->yield = -1;      //协程运行结束
}

*/
#ifndef CO_GOTO_H
#define CO_GOTO_H

#include "_m_list.h"

// CO_BEGIN(), YIELD(), CO_END() 三者不是表达式, 必须作为独立的语句使用

#define CoroutineContext             \
    struct {                         \
        /* save the restore point */ \
        /*  0: ready */              \
        /* >0: running */            \
        /* <0: finish */             \
        int yield;                   \
    }

#define CASE_GOTO(N) \
case N:              \
    goto yield_##N;
#define CO_BEGIN(...)                    \
    do {                                 \
        switch (ctx->yield) {            \
        case 0:                          \
            break; /* begin */           \
            /* case N: goto yield_N; */  \
            /* ...                   */  \
            MAP(CASE_GOTO, __VA_ARGS__); \
        default: goto finally; /* end */ \
        }                                \
    } while (0)

#define LABEL_(N) yield_##N
#define LABEL(N) LABEL_(N)
#define YIELD(...)                                                                     \
    do {                                                                               \
        __VA_ARGS__;           /* run before return, intent for handle return value */ \
        ctx->yield = __LINE__; /* 1. set the restore point, at lable yield_N */        \
        goto finally;          /* 2. return */                                         \
        LABEL(__LINE__) :;     /* 3. put label after each *return* as restore point */ \
    } while (0)

#define CO_END(...)                          \
    do {                                     \
        ctx->yield = -1;                     \
    finally:                                 \
        __VA_ARGS__; /* run before return */ \
    } while (0)

#define ID(X) X;
typedef CoroutineContext ctx_t;

#define CO_FUNC(F, ...)       \
    typedef struct {          \
        CoroutineContext;     \
                              \
        MAP(ID, __VA_ARGS__); \
    } F##_ctx_t;              \
                              \
    void F(ctx_t *ctx)

// 返回 实参X 的指针
#define CO_ARG(F, X) &((F##_ctx_t *)ctx)->X

#endif // CO_GOTO_H
