//
// Created by m2 on 18/1/6.
//

#ifndef NSTD_LOOP_H
#define NSTD_LOOP_H
#include "memory.h"

typedef struct Loop *Loop;
typedef int (*LoopCallback)(void *);

inline static Loop    Loop_New      (LoopCallback fun, void *arg);
inline static void    Loop_Destroy  (Loop *);

// Loop run fun() if fun() return 0
inline static void    Loop_Do       (Loop);


struct Loop {
    LoopCallback fun;
    void *arg;
};

static Loop Loop_New(LoopCallback fun, void *arg)
{
    assert(fun);

    Loop v = T_NEW(v);
    v->fun = fun;
    v->arg = arg;

    return v;
}

static void Loop_Destroy(Loop *o)
{
    T_DESTROY(*o);
}

static void Loop_Do(Loop o)
{
    assert(o);
    while (o->fun(o->arg) == 0) {
        ;
    }
}

#endif //NSTD_LOOP_H
