#include "memory.h"

typedef struct Stack Stack;

inline static Stack * Stack_New(void);
inline static void    Stack_Destroy(Stack **, void(Object *));
inline static Object *Stack_Emplace(Stack *, size_t);
inline static Object *Stack_Pop(Stack *);
inline static Object *Stack_Top(const Stack *);
inline static size_t  Stack_Size(const Stack *);
inline static void    Stack_Gc(Stack *, void(Object *));

const static size_t L1_SIZE_BITS = 8;
const static size_t L2_SIZE_BITS = 8;
const static size_t L3_SIZE_BITS = 8;
const static size_t L1_SIZE      = 1 << L1_SIZE_BITS;
const static size_t L2_SIZE      = 1 << L2_SIZE_BITS;
const static size_t L3_SIZE      = 1 << L3_SIZE_BITS;
#define L1(N) ((N) >> (L3_SIZE_BITS + L2_SIZE_BITS) & (L1_SIZE - 1))
#define L2(N) ((N) >> L3_SIZE_BITS & (L2_SIZE - 1))
#define L3(N) ((N) & (L3_SIZE - 1))

struct Stack {
    Object ***    elements; // items[L1][L2][L3] = Object
    uint_fast32_t size;     // how many items used
    uint_fast32_t cap;      // how many items allocated, N * L3_SIZE
};

#define STACK() (&(Stack){})

Stack *Stack_New(void)
{
    return CALLOC(1, sizeof(Stack));
}

void Stack_Destroy(Stack **o, void objectGc(Object *))
{
    assert(o);

    (*o)->size = 0;
    Stack_Gc(*o, objectGc);
    FREE(*o);
}

Object *Stack_Emplace(Stack *o, size_t elementSize)
{
    assert(o);

    size_t l1 = L1(o->size);
    size_t l2 = L2(o->size);
    size_t l3 = L3(o->size);

    if (o->size == o->cap) {
        if (l2 == 0) {
            if (l1 == 0) {
                if (o->elements == NULL) { // stack han't been inited
                    o->elements = MALLOC(sizeof *o->elements * L1_SIZE);
                } else {
                    ASSERT(false, "Stack is full!");
                }
            }
            o->elements[l1] = MALLOC(sizeof **o->elements * L2_SIZE);
        }
        o->elements[l1][l2] = CALLOC(L3_SIZE, sizeof ***o->elements);
        o->cap += L3_SIZE;
    }

    o->size++;
    return Object_Resize(&o->elements[l1][l2][l3], elementSize);
}

void Stack_Gc(Stack *o, void objectGc(Object *))
{
    assert(o);

    while (o->cap - o->size >= L3_SIZE) {
        o->cap -= L3_SIZE;

        size_t l1 = L1(o->cap);
        size_t l2 = L2(o->cap);

        if (objectGc) {
            for (int i = 0; i < L3_SIZE; i++) {
                objectGc(&o->elements[l1][l2][i]);
            }
        }
        FREE(o->elements[l1][l2]);
        if (l2 == 0) {
            FREE(o->elements[l1]);
            if (l1 == 0) {
                FREE(o->elements);
            }
        }
    }
}

Object *Stack_Top(const Stack *o)
{
    assert(o);
    assert(o->size > 0);

    size_t top = o->size - 1;
    return &o->elements[L1(top)][L2(top)][L3(top)];
}

Object *Stack_Pop(Stack *o)
{
    assert(o);
    assert(o->size > 0);

    o->size--;
    return &o->elements[L1(o->size)][L2(o->size)][L3(o->size)];
}

size_t Stack_Size(const Stack *o)
{
    assert(o);
    return o->size;
}
