/*
COGO_QUEUE_ITEM_T       : the element type of queue
COGO_QUEUE_ITEM_NEXT()  : return the next element
*/
#include <stdbool.h>
#include <stddef.h>

#ifndef COGO_CO_ST_Q_INC_
#define COGO_CO_ST_Q_INC_

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

#endif  // COGO_CO_ST_Q_INC_

#define COGO_Q_T            COGO_QUEUE_T(COGO_QUEUE_ITEM_T)
#define COGO_Q_IS_EMPTY     COGO_QUEUE_IS_EMPTY(COGO_QUEUE_ITEM_T)
#define COGO_Q_POP          COGO_QUEUE_POP(COGO_QUEUE_ITEM_T)
#define COGO_Q_PUSH         COGO_QUEUE_PUSH(COGO_QUEUE_ITEM_T)
#define COGO_Q_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(COGO_QUEUE_ITEM_T)

typedef struct {
  COGO_QUEUE_ITEM_T* head;
  COGO_QUEUE_ITEM_T* tail;
} COGO_Q_T;

static inline bool COGO_Q_IS_EMPTY(const COGO_Q_T* thiz) {
  return thiz->head == NULL;
}

// dequeue, return NULL if empty
static inline COGO_QUEUE_ITEM_T* COGO_Q_POP(COGO_Q_T* thiz) {
  COGO_QUEUE_ITEM_T* node = thiz->head;
  if (!COGO_Q_IS_EMPTY(thiz)) {
    thiz->head = COGO_QUEUE_ITEM_NEXT(thiz->head);
  }
  return node;
}

// enqueue
static inline void COGO_Q_PUSH(COGO_Q_T* thiz, COGO_QUEUE_ITEM_T* node) {
  if (COGO_Q_IS_EMPTY(thiz)) {
    thiz->head = node;
  } else {
    COGO_QUEUE_ITEM_NEXT(thiz->tail) = node;
  }
  thiz->tail = node;
  COGO_QUEUE_ITEM_NEXT(node) = NULL;
}

static inline COGO_QUEUE_ITEM_T* COGO_Q_POP_NONEMPTY(COGO_Q_T* thiz) {
  COGO_QUEUE_ITEM_T* node = thiz->head;
  thiz->head = COGO_QUEUE_ITEM_NEXT(thiz->head);
  return node;
}

#undef COGO_Q_T
#undef COGO_Q_IS_EMPTY
#undef COGO_Q_POP
#undef COGO_Q_PUSH
#undef COGO_Q_POP_NONEMPTY