#include "memory.h"

typedef struct Stack Stack;

inline static Stack *Stack_New(void);
inline static void   Stack_Destroy(Stack **);
inline static void   Stack_Push(Stack *, void *data);
inline static void * Stack_Pop(Stack *);
inline static void * Stack_Top(const Stack *);
inline static size_t Stack_Size(const Stack *);
inline static void   Stack_Gc(Stack *);

const static size_t STACK_PAGE_SIZE = 0xff + 1; // memory size allocated each time

#define D1(N) ((N) >> 0 & 0xff)
#define D2(N) ((N) >> 8 & 0xff)
#define D3(N) ((N) >> 16 & 0xff)

struct Stack {
    void ****items; // items[D3][D2][D1] = void*
    size_t   size;  // how many items used
    size_t   cap;   // how many items allocated, N * STACK_PAGE_SIZE
};

#define STACK() (&(Stack){})

Stack *Stack_New(void)
{
    return CALLOC(1, sizeof(Stack));
}

void Stack_Destroy(Stack **o)
{
    assert(o);

    if (*o) {
        while ((*o)->size > 0) {
            Stack_Pop(*o);
        }
        Stack_Gc(*o);
        DELETE(*o);
    }

    return;
}

size_t Stack_Size(const Stack *o)
{
    assert(o);
    return o->size;
}

void Stack_Push(Stack *o, void *data)
{
    assert(o);

    uint8_t a = D1(o->size);
    uint8_t b = D2(o->size);
    uint8_t c = D3(o->size);

    if (a == 0) {
        if (b == 0) {
            if (c == 0) {
                if (o->items == NULL) { // stack han't been inited
                    o->items = NEW(sizeof *o->items * STACK_PAGE_SIZE);
                } else { // stack is full
                    // TODO: error cap > max
                    assert(("stack is full", 0));
                }
            }
            o->items[c] = NEW(sizeof **o->items * STACK_PAGE_SIZE);
        }
        o->items[c][b] = NEW(sizeof ***o->items * STACK_PAGE_SIZE);
        o->cap += STACK_PAGE_SIZE;
    }

    o->items[c][b][a] = data;
    o->size++;
    return;
}

void *Stack_Pop(Stack *o)
{
    void *v = Stack_Top(o);
    o->size--;
    return v;
}

void *Stack_Top(const Stack *o)
{
    assert(o);
    assert(o->size > 0);

    size_t top = o->size - 1;
    return o->items[D3(top)][D2(top)][D1(top)];
}

void Stack_Gc(Stack *o)
{
    assert(o);

    while (o->cap >= o->size + STACK_PAGE_SIZE) {
        o->cap -= STACK_PAGE_SIZE;

        uint8_t a = D1(o->cap);
        uint8_t b = D2(o->cap);
        uint8_t c = D3(o->cap);

        DELETE(o->items[c][b]);
        if (b == 0) {
            DELETE(o->items[c]);
            if (c == 0) {
                if (o->items != NULL) {
                    DELETE(o->items);
                }
            }
        }
    }

    return;
}
