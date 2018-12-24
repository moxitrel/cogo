#include "../../src/coroutine/gen_label_value.h"
#include <stdio.h>

typedef struct {
    gen_t co;   // must inherent gen_t
    int i;
} gen_example_t;

void gen_example(gen_example_t *co)
{
    co_begin(co);

    for (; co->i < 5; co->i++) {
        printf("%d\n", co->i);
        co_return(co);
    }

    co_end(co);
}

int main(void)
{
    gen_example_t co = {
        .i = 0,
    };
    while (co_state(&co.co) >= 0) {
        gen_example(&co);
    }

    return 0;
}