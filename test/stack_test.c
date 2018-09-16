#include "../lib/Unity/src/unity.h"
#include "../src/stack.h"

void test_Stack_Example(void)
{
#define SIZE_T_OF(P) (*(size_t *)(P))

    Stack *o = Stack_New();
    TEST_ASSERT(o);
    TEST_ASSERT(o->elements == NULL);
    TEST_ASSERT(Stack_Size(o) == 0);
    TEST_ASSERT(o->cap == 0);

    SIZE_T_OF(Stack_Emplace(o, 0)) = Stack_Size(o);
    TEST_ASSERT(o->elements);
    TEST_ASSERT(o->cap == 1 * L3_SIZE);
    TEST_ASSERT(Stack_Size(o) == 1);

    while (Stack_Size(o) < o->cap) {
        size_t preSize = Stack_Size(o);

        SIZE_T_OF(Stack_Emplace(o, 0)) = preSize;

        TEST_ASSERT(SIZE_T_OF(Stack_Top(o)) == preSize)
        TEST_ASSERT(Stack_Size(o) == preSize + 1);
        TEST_ASSERT(o->cap == 1 * L3_SIZE);
    }

    SIZE_T_OF(Stack_Emplace(o, 0)) = Stack_Size(o);
    TEST_ASSERT(o->cap == 2 * L3_SIZE);
    TEST_ASSERT(Stack_Size(o) == 1 + L3_SIZE);

    while (Stack_Size(o) > 0) {
        TEST_ASSERT(SIZE_T_OF(Stack_Pop(o)) == Stack_Size(o));
    }
    TEST_ASSERT(o->cap == 2 * L3_SIZE);
    TEST_ASSERT(Stack_Size(o) == 0);

    Stack_Gc(o, NULL);
    TEST_ASSERT(o->cap == 0);

    Stack_Destroy(&o, NULL);
    TEST_ASSERT(o == NULL);

#undef SIZE_T_OF
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_Stack_Example);

    return UNITY_END();
}
