#include <assert.h>
#include <cogo/cogo_await.h>
#include <stdlib.h>
#include <unity.h>

#include "cogo/cogo_pt.h"

typedef struct await2 {
    COGO_T cogo;
} await2_t;

static void await2_run(COGO_T* cogo) {
COGO_BEGIN(cogo):

    COGO_YIELD(cogo);
    COGO_YIELD(cogo);

COGO_END(cogo):;
}

typedef struct await1 {
    COGO_T cogo;
    await2_t* a2;
} await1_t;

static void await1_run(COGO_T* cogo) {
    await1_t* a1 = (await1_t*)cogo;
COGO_BEGIN(cogo):

    COGO_AWAIT(cogo, &a1->a2->cogo);

COGO_END(cogo):;
}

static void test_resume(void) {
    await2_t a2 = {
            .cogo = COGO_INIT(await2_run, &a2.cogo),
    };
    await1_t a1 = {
            .cogo = COGO_INIT(await1_run, &a1.cogo),
            .a2 = &a2,
    };

    // begin
    TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&a1.cogo));
    TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&a2.cogo));

    // await2 yield: stop when CO_YIELD, but not CO_AWAIT or CO_RETURN
    // await1_run(&a1.cogo);
    COGO_RESUME(&a1.cogo);
    TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_PC(&a2.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_PC(&a2.cogo));
    TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_PC(&a1.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_PC(&a1.cogo));

    // await2 yield
    // await1_run(&a1.cogo);
    COGO_RESUME(&a1.cogo);
    TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_PC(&a2.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_PC(&a2.cogo));
    TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_PC(&a1.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_PC(&a1.cogo));

    // await1 end: stop when root coroutine finished
    // await1_run(&a1.cogo);
    COGO_RESUME(&a1.cogo);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a2.cogo));
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a1.cogo));

    // noop when coroutine end
    // await1_run(&a1.cogo);
    COGO_RESUME(&a1.cogo);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a2.cogo));
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a1.cogo));
}

typedef struct await0 {
    COGO_T cogo;
    await2_t a2;
} await0_t;

static void await0_run(COGO_T* COGO_THIS) {
    await0_t* a0 = (await0_t*)COGO_THIS;
CO_BEGIN:  // COGO_BEGIN(COGO_THIS):

    // await2_run(&a0->a2.cogo);
    COGO_RESUME(&a0->a2.cogo);
    TEST_ASSERT_NOT_NULL(COGO_THIS->anon.sched->top);

    CO_AWAIT(&a0->a2.cogo);  // COGO_AWAIT(COGO_THIS, &a0->a2);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a0->a2.cogo));

CO_END:;  // COGO_END(COGO_THIS):;
}

static void test_await_resumed(void) {
    await0_t a0 = {
            .cogo = COGO_INIT(await0_run, &a0.cogo),
            .a2 = {
                    .cogo = COGO_INIT(await2_run, &a0.a2.cogo),
            },
    };
    COGO_RUN(&a0.cogo);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&a0.cogo));
}

typedef struct ng {
    COGO_T cogo;
    int v;
} ng_t;

static void ng_run(COGO_T* COGO_THIS) {
    ng_t* ng = (ng_t*)COGO_THIS;
CO_BEGIN:  // COGO_BEGIN(COGO_THIS):

    for (;; ng->v++) {
        CO_YIELD;  // COGO_YIELD(COGO_THIS);
    }

CO_END:;  // COGO_END(COGO_THIS):;
}

static void test_ng(void) {
    ng_t ng = {
            .cogo = COGO_INIT(ng_run, &ng.cogo),
            .v = 0,
    };

    // ng_run(&ng.cogo);
    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(0, ng.v);

    // ng_run(&ng.cogo);
    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(1, ng.v);

    // ng_run(&ng.cogo);
    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(2, ng.v);
}

static int fib(int n) {
    assert(n > 0);
    switch (n) {
        case 1:
        case 2:
            return 1;
        default:
            return fib(n - 1) + fib(n - 2);
    }
}

typedef struct fib {
    COGO_T cogo;
    int n;
    int v;
    struct fib* fib1;
    struct fib* fib2;
} fib_t;

static void fib_run(COGO_T* COGO_THIS) {
    fib_t* fib = (fib_t*)COGO_THIS;
CO_BEGIN:

    if (fib->n == 1 || fib->n == 2) {  // f(1) = 1, f(2) = 1
        fib->v = 1;
    } else {  // f(n) = f(n-1) + f(n-2)
        fib->v = 0;

        fib->fib1 = (fib_t*)malloc(sizeof(*fib->fib1));
        assert(fib->fib1);
        *fib->fib1 = (fib_t){
                .cogo = COGO_INIT(fib_run, &fib->fib1->cogo),
                .n = fib->n - 1,
        };
        CO_AWAIT(&fib->fib1->cogo);  // eval f(n-1)
        fib->v += fib->fib1->v;
        free(fib->fib1);

        fib->fib2 = (fib_t*)malloc(sizeof(*fib->fib2));
        assert(fib->fib2);
        *fib->fib2 = (fib_t){
                .cogo = COGO_INIT(fib_run, &fib->fib2->cogo),
                .n = fib->n - 2,
        };
        CO_AWAIT(&fib->fib2->cogo);  // eval f(n-2)
        fib->v += fib->fib2->v;
        free(fib->fib2);
    }

CO_END:;
}

static void test_fib(void) {
    fib_t f03 = {
            .cogo = COGO_INIT(fib_run, &f03.cogo),
            .n = 3,
    };
    fib_t f11 = {
            .cogo = COGO_INIT(fib_run, &f11.cogo),
            .n = 11,
    };
    fib_t f23 = {
            .cogo = COGO_INIT(fib_run, &f23.cogo),
            .n = 23,
    };

    struct {
        fib_t* fib;
        int v;
    } test_cases[] = {
            {&f03, fib(3)},
            {&f11, fib(11)},
            {&f23, fib(23)},
    };

    for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        COGO_RUN(&test_cases[i].fib->cogo);
        TEST_ASSERT_EQUAL_INT(test_cases[i].fib->v, test_cases[i].v);
    }
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_resume);
    RUN_TEST(test_await_resumed);
    RUN_TEST(test_ng);
    RUN_TEST(test_fib);

    return UNITY_END();
}
