#include "cogo_co.h"
#include <cassert>
#include <cstdlib>
#include <memory>
#include "gtest/gtest.h"

inline int cogo_sch_push(cogo_sch_t* sch, cogo_co_t* co) {
    assert(sch);
    assert(sch->stack_top);
    assert(co);
    if (co != sch->stack_top) {
        co->caller = sch->stack_top;
        sch->stack_top = co;
    }
    return 1;
}

inline cogo_co_t* cogo_sch_pop(cogo_sch_t* sch) {
    assert(sch);
    return sch->stack_top;
}

static inline void cogo_co_run(void* co) {
    cogo_sch_t sch = {
            .stack_top = static_cast<cogo_co_t*>(co),
    };
    while (cogo_sch_step(&sch)) {
        // noop
    }
}

CO_DECLARE(static f3) {
CO_BEGIN:
    CO_YIELD;
    CO_RETURN;
    CO_YIELD;  // never run
CO_END:;
}

CO_DECLARE(static f2, f3_t f3) {
CO_BEGIN:
    CO_YIELD;
    CO_AWAIT(&static_cast<f2_t*>(CO_THIS)->f3);
CO_END:;
}

CO_DECLARE(static f1, f2_t f2) {
CO_BEGIN:
    CO_AWAIT(&static_cast<f1_t*>(CO_THIS)->f2);
CO_END:;
}

TEST(CogoCo, Step) {
    auto&& f1 = CO_MAKE(f1, CO_MAKE(f2, CO_MAKE(f3)));
    auto&& f2 = f1.f2;
    auto&& f3 = f2.f3;

    cogo_sch_t sch = {
            .stack_top = reinterpret_cast<cogo_co_t*>(&f1),
    };
    ASSERT_EQ(CO_STATUS(&f1), COGO_STATUS_STARTED);
    ASSERT_EQ(CO_STATUS(&f2), COGO_STATUS_STARTED);
    ASSERT_EQ(CO_STATUS(&f3), COGO_STATUS_STARTED);

    // fc2 yield
    auto co = cogo_sch_step(&sch);
    EXPECT_EQ(co, reinterpret_cast<cogo_co_t*>(&f2));
    EXPECT_GT(CO_STATUS(&f1), COGO_STATUS_STARTED);
    EXPECT_GT(CO_STATUS(&f2), COGO_STATUS_STARTED);
    EXPECT_EQ(CO_STATUS(&f3), COGO_STATUS_STARTED);

    // fc3 first yield
    co = cogo_sch_step(&sch);
    EXPECT_EQ(co, reinterpret_cast<cogo_co_t*>(&f3));
    EXPECT_GT(CO_STATUS(&f1), COGO_STATUS_STARTED);
    EXPECT_GT(CO_STATUS(&f2), COGO_STATUS_STARTED);
    EXPECT_GT(CO_STATUS(&f3), COGO_STATUS_STARTED);

    // fc3 co_return
    co = cogo_sch_step(&sch);
    EXPECT_EQ(CO_STATUS(&f1), COGO_STATUS_STOPPED);
    EXPECT_EQ(CO_STATUS(&f2), COGO_STATUS_STOPPED);
    EXPECT_EQ(CO_STATUS(&f3), COGO_STATUS_STOPPED);
}

static int fibonacci(int n) {
    assert(n >= 0);

    switch (n) {
    case 0:
        return 1;
    case 1:
        return 1;
    default:
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

CO_DECLARE(static fibonacci, int n, int v, fibonacci_t* fib_n1, fibonacci_t* fib_n2) {
    auto* thiz = static_cast<fibonacci_t*>(CO_THIS);
    auto& n = thiz->n;
    auto& v = thiz->v;
    auto& fib_n1 = thiz->fib_n1;
    auto& fib_n2 = thiz->fib_n2;
    auto alloc = std::allocator<fibonacci_t>{};
CO_BEGIN:
    assert(n >= 0);

    switch (n) {
    case 0:  // f(0) = 1
        v = 1;
        CO_RETURN;
    case 1:  // f(1) = 1
        v = 1;
        CO_RETURN;
    default:  // f(n) = f(n-1) + f(n-2)
        fib_n1 = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
        fib_n2 = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
        assert(fib_n1);
        assert(fib_n2);
        *fib_n1 = CO_MAKE(fibonacci, n - 1);
        *fib_n2 = CO_MAKE(fibonacci, n - 2);

        CO_AWAIT(fib_n1);  // eval f(n-1)
        CO_AWAIT(fib_n2);  // eval f(n-2)
        v = fib_n1->v + fib_n2->v;

        std::allocator_traits<decltype(alloc)>::deallocate(alloc, fib_n1, 1);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, fib_n2, 1);
        CO_RETURN;
    }

CO_END:;
}

TEST(cogo_co_t, Run) {
    struct {
        fibonacci_t fib;
        int value;
    } example[] = {
            {CO_MAKE(fibonacci, 0), fibonacci(0)},
            {CO_MAKE(fibonacci, 1), fibonacci(1)},
            {CO_MAKE(fibonacci, 11), fibonacci(11)},
            {CO_MAKE(fibonacci, 23), fibonacci(23)},
            {CO_MAKE(fibonacci, 29), fibonacci(29)},
    };

    for (size_t i = 0; i < sizeof(example) / sizeof(example[0]); i++) {
        cogo_co_run(&example[i].fib);
        ASSERT_EQ(example[i].fib.v, example[i].value);
    }
}
