#include "cogo/cogo_pt.h"

#include <assert.h>
#include <unity.h>

static void func_yield(COGO_T* cogo, int* v) {
COGO_BEGIN(cogo):

    (*v)++;
    COGO_YIELD(cogo);
    (*v)++;

COGO_END(cogo):;
}

static void test_yield(void) {
    COGO_T cogo = {0};
    int v = 0;
    TEST_ASSERT_EQUAL_INT(0, v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&cogo));

    func_yield(&cogo, &v);
    TEST_ASSERT_EQUAL_INT(1, v);
    TEST_ASSERT_NOT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&cogo));
    TEST_ASSERT_NOT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));

    func_yield(&cogo, &v);
    TEST_ASSERT_EQUAL_INT(2, v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));

    // Noop if coroutine end.
    func_yield(&cogo, &v);
    TEST_ASSERT_EQUAL_INT(2, v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));
}

typedef struct func_return {
    COGO_T cogo;
    int v;
} func_return_t;

static void func_return(func_return_t* param) {
COGO_BEGIN(&param->cogo):

    (param->v)++;
    COGO_RETURN(&param->cogo);
    (param->v)++;

COGO_END(&param->cogo):;
}

static void test_return(void) {
    func_return_t args = {
            .v = 0,
    };
    TEST_ASSERT_EQUAL_INT(0, args.v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&args.cogo));

    func_return(&args);
    TEST_ASSERT_EQUAL_INT(1, args.v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&args.cogo));

    // Noop if coroutine end.
    func_return(&args);
    TEST_ASSERT_EQUAL_INT(1, args.v);
    TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&args.cogo));
}

typedef struct prologue {
    int enter;
    int exit;
} prologue_t;

static void func_prologue(prologue_t* prologue, COGO_T* COGO_THIS) {
    prologue->enter++;
CO_BEGIN:  // COGO_BEGIN(COGO_THIS):

    CO_YIELD;  // COGO_YIELD(COGO_THIS);
    CO_YIELD;

CO_END:  // COGO_END(COGO_THIS):
    prologue->exit++;
}

static void test_prologue(void) {
    prologue_t prologue = {
            .enter = 0,
            .exit = 0,
    };
    COGO_T cogo = {0};

    while (COGO_PC(&cogo) != COGO_PC_END) {
        func_prologue(&prologue, &cogo);
    }
    TEST_ASSERT_EQUAL_INT(3, prologue.enter);
    TEST_ASSERT_EQUAL_INT(3, prologue.exit);

    func_prologue(&prologue, &cogo);
    TEST_ASSERT_EQUAL_INT(4, prologue.enter);
    TEST_ASSERT_EQUAL_INT(4, prologue.exit);
}

typedef struct ng {
    int v;
    COGO_T cogo;
} ng_t;

static int func_ng(ng_t* ng) {
    COGO_T* COGO_THIS = &ng->cogo;
CO_BEGIN:  // COGO_BEGIN(COGO_THIS):

    for (;; ng->v++) {
        CO_YIELD;  // COGO_YIELD(COGO_THIS);
    }

CO_END:  // COGO_END(COGO_THIS):
    return ng->v;
}

static void test_ng(void) {
    ng_t ng = {
            .v = 0,
    };
    TEST_ASSERT_EQUAL_INT(0, func_ng(&ng));
    TEST_ASSERT_EQUAL_INT(1, func_ng(&ng));
    TEST_ASSERT_EQUAL_INT(2, func_ng(&ng));
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_yield);
    RUN_TEST(test_return);
    RUN_TEST(test_prologue);
    RUN_TEST(test_ng);

    return UNITY_END();
}
