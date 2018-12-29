//
// Created by M2 on 2018-12-24.
//#undef __GNUC__
#include <stdio.h>
#include "../../src/coroutine/fun.h"

typedef struct {
    fun_t fun;
    char c;
} print_t;

void print(print_t *co)
{
    co_begin(co, 18);

    for (co->c = 'a'; co->c < 'f'; co->c ++) {
        printf("%c\n", co->c);
        co_yield(co);
    }

    co_end(co);
}

#define PRINT() ((print_t){.fun = FUN(print)})

typedef struct loop3_t {
    fun_t fun;
    print_t print;
    int i;
} loop3_t;

void loop3(loop3_t *co)
{
    co_begin(co, 38);
    
    for (co->i = 0; co->i < 3; co->i ++) {
        co->print = PRINT();
        co_await(co, &co->print);
        printf("\n");
    }
    
    co_end(co);
}

#define LOOP3() ((loop3_t){.fun = FUN(loop3)})

int main(void)
{
    fun_run(&LOOP3());

    return 0;
}