/*

co_begin()  : 协程开始
co_end()    : 协程结束
co_return() : 协程返回, yield

obj.state() : 获取运行状态
           0: 准备就绪
          >0: 正在运行
          <0: 结束

*** 用法

//
// 1. 包含头文件
//
#include "gen.hpp"

//
// 2. 自定义协程结构，必须继承 gen_t
//
struct T : public gen_t {
    //
    // 3.1. 定义 局部变量, 返回值, ...
    //
    int i;


    //
    // 3.2. 定义协程函数, 返回类型必须为 void
    //
    void f(...)
    {
        //
        // 协程开始
        //
        co_begin(45, 49, ...);      // 45, 49, ...: 列出所有 co_return() 所在的行号, 即 __LINE__ 的值


        //
        // 用户代码 (*** 无法使用局部变量 ***)
        //
        for (i = 0; i < 9; i++) {
            printf("%d\n", i);
            co_return();    // 返回，下次被调用，从此处开始执行
        }

        printf("%d\n", i);
        co_return();


        //
        // 协程结束
        //
        co_end();
    }
};

//
// 4. 使用
//
void example()
{
    T gen;

    // gen.state(): 获取运行状态
    //    0: 准备就绪
    //   >0: 正在运行
    //   <0: 结束
    while (gen.state() >= 0) {
        gen.f(...);
    }
}

*** 原理: 将 *栈变量* 保存到堆上,

// 展开后
void coroutine(coroutine_t *co) // coroutine_t 由自己定义, 可添加任意字段
{
  // co_begin();
  switch (co->_pc) {             // co->_pc 存储 从哪里开始继续运行
  case 0:    break;             // 协程开始
  case 19:   goto CO_RETURN_19; // 还原点
  case 23:   goto CO_RETURN_23; // 还原点
  case 26:   goto CO_RETURN_26; // 还原点
  case -1:   return;            // 协程结束
  }


  // co_return()
  co->_pc = 19;      // 1. save restore point, next call will be case 19: goto CO_RETURN_19
  return;           // 2. return
CO_RETURN_19:;          // 3. put a label after each *return* as the restore point

  for (; co->i < 9; co->i ++) {
    co->v = co->x;

    // co_return()
    co->_pc = 23;    // 1. save restore point, next call will be case 23: goto CO_RETURN_23
    return;         // 2. return
CO_RETURN_23:;          // 3. label the restore point
  }

  co->v += 1;

  // co_return()
  co->_pc = 26;      // 1. save restore point, next call will be case 26: goto CO_RETURN_26
  return;           // 2. return
CO_RETURN_26:;          // 3. label the restore point

  // co_end()
  co->_pc = -1;      //协程运行结束
}

*/
#ifndef COROUTINE_GEN_H
#define COROUTINE_GEN_H

#ifndef assert
#   define assert(...)  /* nop */
#endif

// gen_t: coroutine context.
//  .state(): return running state.
class gen_t {
protected:
    // Start point where coroutine continue to run after yield.
    //   0: inited
    //  >0: running
    //  <0: finished (-1: ok, -2: invalid _pc)
    int _pc = 0;
public:
    // Get the running state.
    int state() const
    {
        return _pc;
    }
};

//
// co_begin(), co_end(), co_return() 不是函数表达式, 必须作为独立的语句使用
//

// Make goto label.
// e.g. CO_LABEL(13)       -> CO_RETURN_13
//      CO_LABEL(__LINE__) -> CO_RETURN_118
#define CO_LABEL(N)     CO_LABEL_(N)
#define CO_LABEL_(N)    CO_RETURN_##N

#define CASE_GOTO(N)    case N: goto CO_LABEL(N)

// gen_t::co_begin(...);
#define co_begin(...)                                   \
    switch (gen_t::_pc) {                               \
    case  0:                /* coroutine begin */       \
        break;                                          \
 /* case -1:              *//* coroutine end   */       \
 /*     goto CO_END;      */                            \
 /* case  N:              */                            \
 /*     goto CO_RETURN_N; */                            \
    MAP(CASE_GOTO, __VA_ARGS__);                        \
    default:                                            \
        assert(((void)"pc isn't valid.", false));       \
        gen_t::_pc = -2;   /* invalid _pc, kill */      \
        goto CO_END;                                    \
    }


// Yield from the coroutine.
// gen_t::co_return();
#define co_return(...)                                                                  \
    __VA_ARGS__;                /* run before return, intent for handle return value */ \
    gen_t::_pc = __LINE__;      /* 1. save the restore point, at label CO_RETURN_N */   \
    goto CO_END;                /* 2. return */                                         \
CO_LABEL(__LINE__):             /* 3. put label after each *return* as restore point */ \


// gen_t::co_end()
#define co_end()                            \
    gen_t::_pc = -1;   /* finish */         \
CO_END:                                     \



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
// SEE: https://stackoverflow.com/questions/11317474/macro-to-count-number-of-arguments
//      http://p99.gforge.inria.fr/p99-html/p99__args_8h_source.html
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
