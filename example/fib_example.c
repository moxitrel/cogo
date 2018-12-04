#include "/Users/m2/Dropbox/projects/lib/clib/src/coroutine/co_goto.h"

CO_FUN(g, uintmax_t x, uintmax_t v, uintmax_t v_1, uintmax_t v_2)
{
    uintmax_t *x   = CO_ARG(g, x);
    uintmax_t *v   = CO_ARG(g, v);
    uintmax_t *v_1 = CO_ARG(g, v_1);
    uintmax_t *v_2 = CO_ARG(g, v_2);
    CO_BEGIN(12, 14, 16, 18);

    if (*x < 3) {
        YIELD(*v = 1); // return 1
    } else {
        AWAIT(g, *x - 1, 0, 0, 0); // 调用 g(*x-1, 0, 0, 0)
        *v_1 = *CO_VAL(g, v);      // 获取 AWAIT(g, *x-1, 0, 0, 0) 的返回值
        AWAIT(g, *x - 2, 0, 0, 0); // 调用 g(*x-2, 0, 0, 0)
        *v_2 = *CO_VAL(g, v);      // 获取 AWAIT(g, *x-2, 0, 0, 0) 的返回值
        YIELD(*v = *v_1 + *v_2);   // return f(x-1) + f(x-2)
    }

    CO_END();
}

uintmax_t f(uintmax_t x)
{
    if (x < 3) {
        return 1;
    } else {
        return f(x - 1) + f(x - 2);
    }
}

int main(void)
{
    uintmax_t v;
    uintmax_t x = 39;
    CO_RUN(g, x, 0, 0, 0);
    v = *CO_VAL(g, v); // 获取 CO_RUN(g, x, 0, 0, 0) 的返回值
    printf("f, g: %ju, %ju\n", f(x), v);
    return 0;
}
