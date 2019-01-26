#include "../src/gen.h"
#include <stdio.h>

typedef struct {
    gen_t gen;
    int i;
} natgen_t;

// natural number generator
void natgen(natgen_t *co)
{
    int *i = &co->i;
    co_begin(co,16);

    for (*i = 0;;(*i)++) {
        co_yield(co);
    }

    co_end(co);
}

int main(void)
{
    natgen_t co = {};
    for (int i = 0; i < 9; i++) {
        natgen(&co);
        printf("%d\n", co.i);
    }
}

void test_label_value(void)
{
    void *xs[] = {&&L1, &&L2};
    void *pc = xs[(uintptr_t)pc%2];
    goto *pc;

    for (int i = 0; i < 9; i++) {
        printf("%d\n", i);
    }
L1:
    printf("hello");
L2:
    printf("world");
}