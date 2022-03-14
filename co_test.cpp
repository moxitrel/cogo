#include "co.h"
#include <assert.h>
#include <stdlib.h>
#include "gtest/gtest.h"

// put a coroutine into queue
inline int cogo_sch_add(cogo_sch_t* sch, cogo_co_t* co) {
    assert(sch);
    assert(sch->stack_top);
    assert(co);
    if (co != sch->stack_top) {
        co->caller = sch->stack_top;
        sch->stack_top = co;
    }
    return 1;
}

// fetch the next coroutine to be run
inline cogo_co_t* cogo_sch_rm(cogo_sch_t* sch) {
    assert(sch);
    return sch->stack_top;
}

static inline void cogo_co_run(void* co) {
    cogo_sch_t sch = {
            .stack_top = (cogo_co_t*)co,
    };
    while (cogo_sch_step(&sch)) {
    }
}

CO_DECLARE(static fc3) {
CO_BEGIN:
    CO_YIELD;
    CO_RETURN;
    CO_YIELD;  // never run
CO_END:;
}

CO_DECLARE(static fc2, fc3_t f3) {
CO_BEGIN:
    CO_YIELD;
    CO_AWAIT(&((fc2_t*)CO_THIS)->f3);
CO_END:;
}

CO_DECLARE(static fc1, fc2_t f2) {
CO_BEGIN:
    CO_AWAIT(&((fc1_t*)CO_THIS)->f2);
CO_END:;
}

TEST(cogo_co_t, Step) {
    fc1_t f1 = CO_MAKE(fc1, CO_MAKE(fc2, CO_MAKE(fc3)));
    auto& f2 = f1.f2;
    auto& f3 = f2.f3;

    cogo_sch_t sch = {
            .stack_top = (cogo_co_t*)&f1,
    };
    ASSERT_EQ(CO_STATUS(&f1), 0);
    ASSERT_EQ(CO_STATUS(&f2), 0);
    ASSERT_EQ(CO_STATUS(&f3), 0);

    // fc2 yield
    auto co = cogo_sch_step(&sch);
    EXPECT_EQ(co, (cogo_co_t*)&f2);
    EXPECT_GT(CO_STATUS(&f1), 0);
    EXPECT_GT(CO_STATUS(&f2), 0);
    EXPECT_EQ(CO_STATUS(&f3), 0);

    // fc3 first yield
    co = cogo_sch_step(&sch);
    EXPECT_EQ(co, (cogo_co_t*)&f3);
    EXPECT_GT(CO_STATUS(&f1), 0);
    EXPECT_GT(CO_STATUS(&f2), 0);
    EXPECT_GT(CO_STATUS(&f3), 0);

    // fc3 co_return
    co = cogo_sch_step(&sch);
    EXPECT_EQ(CO_STATUS(&f1), -1);
    EXPECT_EQ(CO_STATUS(&f2), -1);
    EXPECT_EQ(CO_STATUS(&f3), -1);
}

static unsigned fibonacci(unsigned n) {
    switch (n) {
    case 0:
        return 1;
    case 1:
        return 1;
    default:
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

CO_DECLARE(static fibonacci, unsigned n, unsigned v, fibonacci_t* fib_n1, fibonacci_t* fib_n2) {
    auto* thiz = (fibonacci_t*)CO_THIS;
    auto& n = thiz->n;
    auto& v = thiz->v;
    auto& fib_n1 = thiz->fib_n1;
    auto& fib_n2 = thiz->fib_n2;
CO_BEGIN:

    switch (n) {
    case 0:  // f(0) = 1
        v = 1;
        CO_RETURN;
    case 1:  // f(1) = 1
        v = 1;
        CO_RETURN;
    default:  // f(n) = f(n-1) + f(n-2)
        fib_n1 = (fibonacci_t*)malloc(sizeof(*fib_n1));
        fib_n2 = (fibonacci_t*)malloc(sizeof(*fib_n2));
        assert(fib_n1 != nullptr);
        assert(fib_n2 != nullptr);

        *fib_n1 = CO_MAKE(fibonacci, .n = n - 1);
        *fib_n2 = CO_MAKE(fibonacci, .n = n - 2);
        CO_AWAIT(fib_n1);  // eval f(n-1)
        CO_AWAIT(fib_n2);  // eval f(n-2)
        v = fib_n1->v + fib_n2->v;

        free(fib_n1);
        free(fib_n2);
        CO_RETURN;
    }

CO_END:;
}

TEST(cogo_co_t, Run) {
    struct {
        fibonacci_t fib;
        unsigned value;
    } example[] = {
            {CO_MAKE(fibonacci, .n = 0), fibonacci(0)},
            {CO_MAKE(fibonacci, .n = 1), fibonacci(1)},
            {CO_MAKE(fibonacci, .n = 11), fibonacci(11)},
            {CO_MAKE(fibonacci, .n = 23), fibonacci(23)},
            {CO_MAKE(fibonacci, .n = 29), fibonacci(29)},
    };

    for (size_t i = 0; i < sizeof(example) / sizeof(example[0]); i++) {
        cogo_co_run(&example[i].fib);
        ASSERT_EQ(example[i].fib.v, example[i].value);
    }
}
