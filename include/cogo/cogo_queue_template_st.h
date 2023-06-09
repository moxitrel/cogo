/*
COGO_QUEUE_ELEMENT_T: the element type of queue
COGO_QUEUE_NEXT()   : return the next element
*/
#include <stdbool.h>
#include <stddef.h>

#ifndef COGO_QUEUE_TEMPLATE_ST_H_
#define COGO_QUEUE_TEMPLATE_ST_H_

#define COGO_QUEUE_T(T)             COGO_QUEUE_T1(T)
#define COGO_QUEUE_T1(T)            cogo_##T##_queue_t

#define COGO_QUEUE_IS_EMPTY(T)      COGO_QUEUE_IS_EMPTY1(T)
#define COGO_QUEUE_IS_EMPTY1(T)     cogo_##T##_queue_is_empty

#define COGO_QUEUE_POP(T)           COGO_QUEUE_POP1(T)
#define COGO_QUEUE_POP1(T)          cogo_##T##_queue_pop

#define COGO_QUEUE_PUSH(T)          COGO_QUEUE_PUSH1(T)
#define COGO_QUEUE_PUSH1(T)         cogo_##T##_queue_push

#define COGO_QUEUE_POP_NONEMPTY(T)  COGO_QUEUE_POP_NONEMPTY1(T)
#define COGO_QUEUE_POP_NONEMPTY1(T) cogo_##T##_queue_pop_nonempty

#endif  // COGO_QUEUE_TEMPLATE_ST_H_

#define COGO_Q_T            COGO_QUEUE_T(COGO_QUEUE_ELEMENT_T)
#define COGO_Q_IS_EMPTY     COGO_QUEUE_IS_EMPTY(COGO_QUEUE_ELEMENT_T)
#define COGO_Q_POP          COGO_QUEUE_POP(COGO_QUEUE_ELEMENT_T)
#define COGO_Q_PUSH         COGO_QUEUE_PUSH(COGO_QUEUE_ELEMENT_T)
#define COGO_Q_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(COGO_QUEUE_ELEMENT_T)

typedef struct {
  COGO_QUEUE_ELEMENT_T* head;
  COGO_QUEUE_ELEMENT_T* tail;
} COGO_Q_T;

static inline bool COGO_Q_IS_EMPTY(COGO_Q_T const* const q) {
  return q->head == NULL;
}

// dequeue, return NULL if empty
static inline COGO_QUEUE_ELEMENT_T* COGO_Q_POP(COGO_Q_T* const q) {
  COGO_QUEUE_ELEMENT_T* node = q->head;
  if (!COGO_Q_IS_EMPTY(q)) {
    q->head = COGO_QUEUE_NEXT(q->head);
  }
  return node;
}

// enqueue
static inline void COGO_Q_PUSH(COGO_Q_T* const q, COGO_QUEUE_ELEMENT_T* const node) {
  if (COGO_Q_IS_EMPTY(q)) {
    q->head = node;
  } else {
    COGO_QUEUE_NEXT(q->tail) = node;
  }
  q->tail = node;
  COGO_QUEUE_NEXT(node) = NULL;
}

static inline COGO_QUEUE_ELEMENT_T* COGO_Q_POP_NONEMPTY(COGO_Q_T* const q) {
  COGO_QUEUE_ELEMENT_T* node = q->head;
  q->head = COGO_QUEUE_NEXT(q->head);
  return node;
}

#undef COGO_Q_T
#undef COGO_Q_IS_EMPTY
#undef COGO_Q_POP
#undef COGO_Q_PUSH
#undef COGO_Q_POP_NONEMPTY
