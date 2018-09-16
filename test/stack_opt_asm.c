//
// Created by M2 on 2018/9/13.
//

#include "../src/stack.h"
#define NDEBUG
inline static void Stack_Dec(Stack *o)
{
    assert(o);
    o->size--;
}

void test_Pop(Stack *o)
{
    Stack_Dec(o);
}