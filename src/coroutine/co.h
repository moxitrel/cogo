#ifndef COROUTINE_CO_H
#define COROUTINE_CO_H

#include "fun.h"

typedef struct co_t {
    fun_t fun;
    struct co_t *q_next;
} co_t;

#define CO(F)   ((co_t){.fun = FUN(F),})

// Coroutine queue, who run concurrently with me.
_Thread_local static co_t *co_q_head;
_Thread_local static co_t *co_q_tail;

void co_run(co_t *co)
{
    for (co_q_tail = co_q_head = co; co_q_head != NULL;) {
        co_t **node_next;
        for (co_t **node = &co_q_head; *node != NULL; node = node_next) {
            node_next = &(*node)->q_next;

            *node = (co_t *)fun_step(&(*node)->fun);
            // delete node if finished.
            if (*node == NULL) {
                *node = *node_next;
            }
        }
    }
}

// Append co to the coroutine queue.
#define co_sched(CO, NEWCO)                 \
do {                                        \
    co_t *const _co = (co_t *)(CO);         \
    co_t *const _newco = (co_t *)(NEWCO);   \
    co_q_tail->q_next = _newco;             \
    co_q_tail = _newco;                     \
    co_return(_co);                         \
} while (0)

#endif //COROUTINE_CO_H
