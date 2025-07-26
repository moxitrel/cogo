#include <assert.h>
#include <cogo/cogo_call.h>
#include <stdlib.h>
#include <unity.h>

typedef struct yield {
    COGO_T cogo;
} yield_t;

static void yield_func(COGO_T* COGO_THIS) {
CO_BEGIN:

    CO_YIELD;
    CO_YIELD;

CO_END:;
}

typedef struct await {
    COGO_T cogo;
    yield_t* y;
} await_t;

static void await_func(COGO_T* cogo) {
    await_t* thiz = (await_t*)cogo;
COGO_BEGIN(cogo):

    COGO_AWAIT(cogo, &thiz->y->cogo);

COGO_END(cogo):;
}

static void test_resume(void) {
    yield_t y = {
            .cogo = COGO_INIT(&y.cogo, yield_func),
    };
    await_t a = {
            .cogo = COGO_INIT(&a.cogo, await_func),
            .y = &y,
    };

    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&a.cogo));
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&y.cogo));

    // yield_func yield (1): stop when CO_YIELD, but not CO_AWAIT or CO_RETURN
    COGO_RESUME(&a.cogo);
    TEST_ASSERT_GREATER_THAN_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&y.cogo));
    TEST_ASSERT_GREATER_THAN_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&a.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_STATUS_END, COGO_STATUS(&y.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_STATUS_END, COGO_STATUS(&a.cogo));

    // yield_func yield (2): stop when CO_YIELD, but not CO_AWAIT or CO_RETURN
    COGO_RESUME(&a.cogo);
    TEST_ASSERT_GREATER_THAN_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&y.cogo));
    TEST_ASSERT_GREATER_THAN_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&a.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_STATUS_END, COGO_STATUS(&y.cogo));
    TEST_ASSERT_LESS_THAN_UINT64(COGO_STATUS_END, COGO_STATUS(&a.cogo));

    // await_func end: stop when root coroutine finished
    COGO_RESUME(&a.cogo);
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&y.cogo));
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&a.cogo));

    // noop when coroutine end
    COGO_RESUME(&a.cogo);
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&y.cogo));
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&a.cogo));
}

typedef struct resume {
    COGO_T cogo;
    yield_t y;
} resume_t;

static void resume_func(COGO_T* COGO_THIS) {
    resume_t* thiz = (resume_t*)COGO_THIS;
CO_BEGIN:;

    // yield_func yield (1)
    COGO_RESUME(&thiz->y.cogo);
    TEST_ASSERT_NOT_NULL(COGO_THIS->a.sch->top);

    CO_AWAIT(&thiz->y.cogo);
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&thiz->y.cogo));

CO_END:;
}

static void test_call_resume(void) {
    resume_t r = {
            .cogo = COGO_INIT(&r.cogo, resume_func),
            .y = {
                    .cogo = COGO_INIT(&r.y.cogo, yield_func),
            },
    };

    // yield_func yield (2)
    COGO_RESUME(&r.cogo);
    TEST_ASSERT_GREATER_THAN_UINT64(COGO_STATUS_BEGIN, COGO_STATUS(&r.cogo));

    COGO_RESUME(&r.cogo);
    TEST_ASSERT_EQUAL_UINT64(COGO_STATUS_END, COGO_STATUS(&r.cogo));
}

typedef struct ng {
    COGO_T cogo;
    int v;
} ng_t;

static void ng_func(COGO_T* COGO_THIS) {
    ng_t* thiz = (ng_t*)COGO_THIS;
CO_BEGIN:

    for (;; thiz->v++) {
        CO_YIELD;
    }

CO_END:;
}

static void test_ng(void) {
    ng_t ng = {
            .cogo = COGO_INIT(&ng.cogo, ng_func),
            .v = 0,
    };

    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(0, ng.v);

    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(1, ng.v);

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

static void fib_func(COGO_T* COGO_THIS) {
    fib_t* fib = (fib_t*)COGO_THIS;
    assert(fib->n > 0);
CO_BEGIN:

    if (fib->n == 1 || fib->n == 2) {  // f(1) = 1, f(2) = 1
        fib->v = 1;
    } else {  // f(n) = f(n-1) + f(n-2)
        fib->v = 0;

        fib->fib1 = (fib_t*)malloc(sizeof(*fib->fib1));
        assert(fib->fib1);
        *fib->fib1 = (fib_t){
                .cogo = COGO_INIT(&fib->fib1->cogo, fib_func),
                .n = fib->n - 1,
        };
        CO_AWAIT(&fib->fib1->cogo);  // eval f(n-1)
        fib->v += fib->fib1->v;
        free(fib->fib1);

        fib->fib2 = (fib_t*)malloc(sizeof(*fib->fib2));
        assert(fib->fib2);
        *fib->fib2 = (fib_t){
                .cogo = COGO_INIT(&fib->fib2->cogo, fib_func),
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
            .cogo = COGO_INIT(&f03.cogo, fib_func),
            .n = 3,
    };
    fib_t f11 = {
            .cogo = COGO_INIT(&f11.cogo, fib_func),
            .n = 11,
    };
    fib_t f23 = {
            .cogo = COGO_INIT(&f23.cogo, fib_func),
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
        while (COGO_RESUME(&test_cases[i].fib->cogo)) {
        }
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
    RUN_TEST(test_call_resume);
    RUN_TEST(test_ng);
    RUN_TEST(test_fib);

    return UNITY_END();
}
