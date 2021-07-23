#ifndef MOXITREL_COGO_CO_IMPL_Q_H_
#define MOXITREL_COGO_CO_IMPL_Q_H_
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void* head;
    void* tail;
} co_queue_t;

#define CO_QUEUE_NEXT(Q,N)    (*(void**)((uintptr_t)(Q) + (N)))

static inline bool co_queue_empty(const co_queue_t* thiz)
{
    return thiz->head == NULL;
}

static inline void* co_queue_pop(co_queue_t* thiz, ptrdiff_t next)
{
    void* node = thiz->head;
    if (!co_queue_empty(thiz)) {
        thiz->head = CO_QUEUE_NEXT(thiz->head, next);
    }
    return node;
}

static inline void co_queue_push(co_queue_t* thiz, ptrdiff_t next, void* node)
{
    if (co_queue_empty(thiz)) {
        thiz->head = node;
    } else {
        CO_QUEUE_NEXT(thiz->tail, next) = node;
    }
    thiz->tail = node;
    CO_QUEUE_NEXT(node, next) = NULL;
}

static inline void* co_queue_pop_nonempty(co_queue_t* thiz, ptrdiff_t next)
{
    void* node = thiz->head;
    thiz->head = CO_QUEUE_NEXT(thiz->head, next);
    return node;
}

#endif  // MOXITREL_COGO_CO_IMPL_Q_H_
