// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/*
COGO_QUEUE_VALUE_T: the element type of queue
COGO_QUEUE_NEXT()   : return the next element
*/
#ifndef COGO_QUEUE_TEMPLATE_H_
    #define COGO_QUEUE_TEMPLATE_H_

    #define COGO_QUEUE_T(T)                   COGO_QUEUE_DO1_T(T)
    #define COGO_QUEUE_DO1_T(T)               cogo_queue_t_##T

    #define COGO_QUEUE_MAKE(T)                COGO_QUEUE_DO1_MAKE(T)
    #define COGO_QUEUE_DO1_MAKE(T)            cogo_queue_make_##T

    #define COGO_QUEUE_IS_EMPTY(T)            COGO_QUEUE_DO1_IS_EMPTY(T)
    #define COGO_QUEUE_DO1_IS_EMPTY(T)        cogo_queue_is_empty_##T

    #define COGO_QUEUE_REMOVE(T)              COGO_QUEUE_DO1_REMOVE(T)
    #define COGO_QUEUE_DO1_REMOVE(T)          cogo_queue_remove_##T

    #define COGO_QUEUE_ADD(T)                 COGO_QUEUE_DO1_ADD(T)
    #define COGO_QUEUE_DO1_ADD(T)             cogo_queue_add_##T

    #define COGO_QUEUE_REMOVE_NONEMPTY(T)     COGO_QUEUE_DO1_REMOVE_NONEMPTY(T)
    #define COGO_QUEUE_DO1_REMOVE_NONEMPTY(T) cogo_queue_remove_nonempty_##T

#endif  // COGO_QUEUE_TEMPLATE_H_

#define COGO_Q_T               COGO_QUEUE_T(COGO_QUEUE_VALUE_T)
#define COGO_Q_MAKE            COGO_QUEUE_MAKE(COGO_QUEUE_VALUE_T)
#define COGO_Q_IS_EMPTY        COGO_QUEUE_IS_EMPTY(COGO_QUEUE_VALUE_T)
#define COGO_Q_REMOVE          COGO_QUEUE_REMOVE(COGO_QUEUE_VALUE_T)
#define COGO_Q_ADD             COGO_QUEUE_ADD(COGO_QUEUE_VALUE_T)
#define COGO_Q_REMOVE_NONEMPTY COGO_QUEUE_REMOVE_NONEMPTY(COGO_QUEUE_VALUE_T)

typedef struct COGO_Q_T {
    COGO_QUEUE_VALUE_T* head;
    COGO_QUEUE_VALUE_T* tail;
} COGO_Q_T;

static inline COGO_Q_T COGO_Q_MAKE(void) {
    COGO_Q_T q = {
            /*head=*/0,
            /*tail=*/0,
    };
    return q;
}

static inline int COGO_Q_IS_EMPTY(COGO_Q_T const* const q) {
    return !q->head;
}

// dequeue, return NULL if empty
static inline COGO_QUEUE_VALUE_T* COGO_Q_REMOVE(COGO_Q_T* const q) {
    COGO_QUEUE_VALUE_T* node = q->head;
    if (node) {
        q->head = COGO_QUEUE_NEXT(q->head);
    }
    return node;
}

// enqueue
static inline void COGO_Q_ADD(COGO_Q_T* const q, COGO_QUEUE_VALUE_T* const node) {
    if (COGO_Q_IS_EMPTY(q)) {
        q->head = node;
    } else {
        COGO_QUEUE_NEXT(q->tail) = node;
    }
    q->tail = node;
    COGO_QUEUE_NEXT(node) = 0;
}

static inline COGO_QUEUE_VALUE_T* COGO_Q_REMOVE_NONEMPTY(COGO_Q_T* const q) {
    COGO_QUEUE_VALUE_T* node = q->head;
    q->head = COGO_QUEUE_NEXT(q->head);
    return node;
}

#undef COGO_Q_REMOVE_NONEMPTY
#undef COGO_Q_ADD
#undef COGO_Q_REMOVE
#undef COGO_Q_IS_EMPTY
#undef COGO_Q_T
