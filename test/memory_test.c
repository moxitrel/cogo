//
// Created by M2 on 2018/12/6.
//
#include "../src/memory.h"
#include <stdio.h>
#include <stdarg.h>

void test_ASSERT(void)
{
    CALLOC((uint64_t)(1) << 63, 90);
}

void test_FATAL(int x, ...)
{
    va_list ap;
    va_start(ap, x);

    int y = va_arg(ap, int);

    va_end(ap);
}
int main(void)
{
    test_FATAL(1);
}