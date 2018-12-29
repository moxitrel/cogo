/*

CO(FUN)     : 新建协程
co_begin()  : 协程开始
co_end()    : 协程结束
co_yield() : 返回
co_await()   : 调用其他协程
co_state()  : 获取协程运行状态
              >=0: 正在运行
               -1: 运行结束

*** 用法

// 1. 包含头文件
#include "co_switch_goto.h"

// 2. 自定义协程结构 (局部变量, 返回值), 必须继承 gen_t
typedef struct {
    // must inherit gen_t (put gen_t first).
    gen_t co;

    // user definitions
    ...
} fun_t;

// 3. 定义协程函数, 类型必须为 void(gen_t *)
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
    co_begin(co,35,39,...); // 35,39, ...: 列出所有 co_yield(), co_await() 所在的行号, 即 __LINE__ 的值


    //
    // 用户代码:  !!! 禁用局部变量 !!!
    //
    co_yield(co);          // 返回，下次被调用，从此处开始执行，***局部变量无法被恢复***

    for (; co->i < 9; co->i ++) {
        v = x;
        co_yield(co);
    }

    v += 1;
    co_yield(co);

    //
    // 协程结束
    //
    co_end(co);


    //
    // co_end()之后的代码，每次co_yield()返回前 都会被执行
    //
}

// 4. define initializer
void fun_init(fun_t *o, co_fun_t fun,...)
{
    assert(o);
    assert(fun);

    *o = (fun_t){
        // init gen_t
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
  case 19:   goto YIELD_19;     // 还原点
  case 23:   goto YIELD_23;     // 还原点
  case 26:   goto YIELD_26;     // 还原点
  case -1:   return;            // 协程结束
  }


  // co_yield()
  co->pc = 19;      // 1. save restore point, next call will be case 19: goto YIELD_19
  return;           // 2. return
YIELD_19:;          // 3. put a label after each *return* as the restore point

  for (; co->i < 9; co->i ++) {
    co->v = co->x;

    // co_yield()
    co->pc = 23;    // 1. save restore point, next call will be case 23: goto YIELD_23
    return;         // 2. return
YIELD_23:;          // 3. label the restore point
  }

  co->v += 1;

  // co_yield()
  co->pc = 26;      // 1. save restore point, next call will be case 26: goto YIELD_26
  return;           // 2. return
YIELD_26:;          // 3. label the restore point

  // co_end()
  co->pc = -1;      //协程运行结束
}

*/
#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: coroutine context, must be inherited (as first field) by user-defined type.
// e.g.
//   typedef struct {
//     gen_t co;
//     ...
//   } user_defined_t;
typedef struct {
    // Start point where coroutine continue to run after yield.
    //   0: inited
    //  >0: running
    //  <0: finished (-1: ok, -2: invalid pc)
    int pc;
} gen_t;

// return gen_t.pc
#define GEN_PC(CO)      (((gen_t *)(CO))->pc)

inline static int co_state(const gen_t *const co)
{
    assert(co);
    return co->pc;
}


// API is *** not type safe ***
//
// co_begin(), co_end(), co_yield() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_YIELD_13
//      CO_LABEL(__LINE__) -> CO_YIELD_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_YIELD_##N

#define CASE_GOTO(N)    case N: goto CO_LABEL(N)

// co_begin(co_t *, ...);
#define co_begin(CO, ...)                               \
do {                                                    \
    switch (GEN_PC(CO)) {                               \
    case  0:                /* coroutine begin */       \
        break;                                          \
 /* case -1:              *//* coroutine end   */       \
 /*     goto CO_END;      */                            \
 /* case  N:              */                            \
 /*     goto CO_YIELD_N; */                             \
    MAP(CASE_GOTO, __VA_ARGS__);                        \
    default:                                            \
        assert(((void)"pc isn't valid.", false));       \
        GEN_PC(CO) = -2;   /* invalid _pc, kill */      \
        goto CO_END;                                    \
    }                                                   \
} while (0)


// Yield from the coroutine. (yield)
// co_yield(co_t *);
#define co_yield(CO, ...)                                                               \
do {                                                                                    \
    __VA_ARGS__;                /* run before return, intent for handle return value */ \
    GEN_PC(CO) = __LINE__;      /* 1. save the restore point, at label YIELD_N */       \
    goto CO_END;                /* 2. return */                                         \
CO_LABEL(__LINE__):;            /* 3. put label after each *return* as restore point */ \
} while (0)


// co_end(co_t *)
#define co_end(CO)                          \
do {                                        \
    GEN_PC(CO) = -1;   /* finish */         \
CO_END:;                                    \
} while (0)


// Count the number of arguments.
// e.g. LEN(1)      -> 1
//      LEN(1,2)    -> 2
//      LEN(1,2,3)  -> 3
// BUG: LEN1()      -> 1, expect 0
#define LEN1(...)       ARG_PAT(__VA_ARGS__, LEN_PADDING)

#define ARG_PAT(...)    ARG_PAT_(__VA_ARGS__)
#define ARG_PAT_(                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,       \
    _11, _12, _13, _14, _15, _16, _17, _18, _19,   N, ...)  N
#define LEN_PADDING                                         \
     19,  18,  17,  16,  15,  14,  13,  12,  11,  10,       \
      9,   8,   7,   6,   5,   4,   3,   2,   1,   0

// Count the number of arguments. (Solve the LEN1(...)'s BUG)
//
// e.g. LEN()       -> LEN_(0,1,1) -> LEN_01(1) -> 0
//      LEN(1)      -> LEN_(0,0,1) -> LEN_00(1) -> 1
//      LEN(1,2)    -> LEN_(1,1,2) -> LEN_11(2) -> 2
//      LEN(1,2,3)  -> LEN_(1,1,3) -> LEN_11(3) -> 3
//      LEN(1,2,...)-> LEN_(1,1,N) -> LEN_11(N) -> N
//
// SEE: http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
//      P99, advanced macro tricks (http://p99.gforge.inria.fr/p99-html/index.html)
//
#define LEN(...)                            \
LEN_(                                       \
    HAS_COMMA(__VA_ARGS__),                 \
    HAS_COMMA(GET_COMMA __VA_ARGS__ ()),    \
    LEN1(__VA_ARGS__)                       \
)
#define LEN_(D1, D2, N)     LEN_01N(D1, D2, N)
#define LEN_01N(D1, D2, N)  LEN_##D1##D2(N)
#define LEN_01(N)           0
#define LEN_00(N)           1
#define LEN_11(N)           N

#define HAS_COMMA_PADDING                                   \
      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,       \
      1,   1,   1,   1,   1,   1,   1,   1,   0,   0
#define HAS_COMMA(...)      ARG_PAT(__VA_ARGS__, HAS_COMMA_PADDING)
#define GET_COMMA(...)      ,


#define MAP(F, ...)       MAP_N_(LEN(__VA_ARGS__), F, __VA_ARGS__)
#define MAP_N_(...)       MAP_N(__VA_ARGS__)
#define MAP_N(N, F, ...)  MAP_##N(F, __VA_ARGS__)
#define MAP_0( F, ...)
#define MAP_1( F, X, ...) F(X)
#define MAP_2( F, X, ...) F(X); MAP_1( F, __VA_ARGS__)
#define MAP_3( F, X, ...) F(X); MAP_2( F, __VA_ARGS__)
#define MAP_4( F, X, ...) F(X); MAP_3( F, __VA_ARGS__)
#define MAP_5( F, X, ...) F(X); MAP_4( F, __VA_ARGS__)
#define MAP_6( F, X, ...) F(X); MAP_5( F, __VA_ARGS__)
#define MAP_7( F, X, ...) F(X); MAP_6( F, __VA_ARGS__)
#define MAP_8( F, X, ...) F(X); MAP_7( F, __VA_ARGS__)
#define MAP_9( F, X, ...) F(X); MAP_8( F, __VA_ARGS__)
#define MAP_10(F, X, ...) F(X); MAP_9( F, __VA_ARGS__)
#define MAP_11(F, X, ...) F(X); MAP_10(F, __VA_ARGS__)
#define MAP_12(F, X, ...) F(X); MAP_11(F, __VA_ARGS__)
#define MAP_13(F, X, ...) F(X); MAP_12(F, __VA_ARGS__)
#define MAP_14(F, X, ...) F(X); MAP_13(F, __VA_ARGS__)
#define MAP_15(F, X, ...) F(X); MAP_14(F, __VA_ARGS__)
#define MAP_16(F, X, ...) F(X); MAP_15(F, __VA_ARGS__)
#define MAP_17(F, X, ...) F(X); MAP_16(F, __VA_ARGS__)
#define MAP_18(F, X, ...) F(X); MAP_17(F, __VA_ARGS__)
#define MAP_19(F, X, ...) F(X); MAP_18(F, __VA_ARGS__)

#endif // COROUTINE_GEN_H
