// outfile=$(mktemp); clang co_goto_test.c ../../lib/Unity/src/unity.c -o
// $outfile && $outfile

#include "../../lib/Unity/src/unity.h"
#include "co_goto.h"

// gen_0_13: 生成器, 返回 0, 1, 2, ... 13
//
// DEFINE_CO(gen_0_13, int value, int i) 等价于:
//
//    typedef struct {
//        CoroutineContext;
//
//        int value;    // 保存返回值
//        int i;        // 局部变量
//    } gen_0_13_ctx_t;
//
// void gen_0_13(ctx_t *ctx)
//
CO_FUNC(gen_0_13, int value, int i)
{
    // BEGIN() 之前的代码，每次调用都会被执行
    int *value = CO_ARG(gen_0_13, value); // & ((gen_0_13 *)ctx) -> value
    int *i     = CO_ARG(gen_0_13, i);     // & ((gen_0_13 *)ctx) -> i

    CO_BEGIN(34,
             38,
             41,
             44,
             47); //协程开始
                  // 33,37, ...: YIELD() 所在的行号; 若不正确, 则会编译失败
                  // (e.g. use of undeclared label 'yield_32')

    // 返回 0, 1, ... 9
    for (; *i < 10; (*i)++) {
        *value = *i; //存储返回值
        YIELD();     //返回
    }

    *value = (*i)++; // value=10, i=11
    YIELD();

    *value = (*i)++; // value=11, i=12
    YIELD();

    *value = (*i)++; // value=12, i=13
    YIELD();

    *value = (*i)++; // value=13, i=14
    YIELD();

    CO_END(); //协程结束
}

void test_gen_0_13(void)
{
    gen_0_13_ctx_t ctx = {};

    for (int i = 0; i < 14; i++) {
        gen_0_13((ctx_t *)&ctx);
        TEST_ASSERT(ctx.value == i);
    }

    TEST_ASSERT(ctx.yield > 0); // 此时，协程未运行完毕，即END()未执行
    gen_0_13((ctx_t *)&ctx);    // 运行END()
    TEST_ASSERT(ctx.yield < 0); // 协程已运行结束
}

// void test_run(void)
//{
//    #include "../memory.h"
//
//    typedef struct {
//        void (*f)(ctx_t *);
//        ctx_t *ctx;
//    } co_t;
//
//    ctx_t **stack = NEW(sizeof *stack * 1024);
//
//    gen_0_13_ctx_t *x = NEW(sizeof *x)
//    *x = (gen_0_13_ctx_t){0, 0};
//    *stack++ = x;
//
//    while (!stack_empty()) {
//        T elt = stack_top();
//        elt.f(elt.x);
//        if (elt.x->yield < 0) {
//            stack_pop();  // free(elt.x)
//        }
//    }
//
//    stack--;
//    DELETE(stack);
//}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_gen_0_13);

    return UNITY_END();
}
