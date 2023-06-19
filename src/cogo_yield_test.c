#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

typedef struct yield {
  cogo_yield_t super;
  int v;
} yield_t;

void yield_func(yield_t *thiz) {
  COGO_BEGIN(thiz) :;

  thiz->v++;
  COGO_YIELD(thiz);
  thiz->v++;

  COGO_END(thiz) :;
}

static void test_yield(void) {
  yield_t co = {
      .v = 0,
  };
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_BEGIN, COGO_PC(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, co.v);

  yield_func(&co);
  TEST_ASSERT_NOT_EQUAL_UINT64(COGO_PC_BEGIN, COGO_PC(&co));  // running
  TEST_ASSERT_NOT_EQUAL_UINT64(COGO_PC_END, COGO_PC(&co));
  TEST_ASSERT_EQUAL_INT(1, co.v);

  yield_func(&co);
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_END, COGO_PC(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, co.v);

  // noop when coroutine end
  yield_func(&co);
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_END, COGO_PC(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, co.v);
}

typedef struct return1 {
  cogo_yield_t super;
  int v;
} return1_t;

void return1_func(return1_t *co_this) {
CO_BEGIN:

  co_this->v++;
  CO_RETURN;
  co_this->v++;

CO_END:;
}

static void test_return(void) {
  return1_t co = {
      .v = 0,
  };
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_BEGIN, COGO_PC(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, co.v);

  return1_func(&co);
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_END, COGO_PC(&co));  // end
  TEST_ASSERT_EQUAL_INT(1, co.v);

  // noop when coroutine end
  return1_func(&co);
  TEST_ASSERT_EQUAL_UINT64(COGO_PC_END, COGO_PC(&co));  // end
  TEST_ASSERT_EQUAL_INT(1, co.v);
}

CO_DECLARE(/*NAME*/ prologue, int enter, int exit) {
  prologue_t *const thiz = (prologue_t *)co_this;
  thiz->enter++;
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:
  thiz->exit++;
}

static void test_prologue(void) {
  prologue_t co = CO_MAKE(prologue, 0, 0);

  while (co_status((cogo_yield_t *)&co) != CO_STATUS_END) {
    prologue_func(&co);
  }

  // prologue and epilogue are always run even ended
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);

  // prologue and epilogue are always run even ended
  prologue_func(&co);
  TEST_ASSERT_EQUAL_INT(4, co.enter);
  TEST_ASSERT_EQUAL_INT(4, co.exit);
}

CO_DECLARE(/*NAME*/ nat, /*return*/ int v) {
  nat_t *const thiz = (nat_t *)co_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = CO_MAKE(nat);  // "v" isn't explicitly initialized

  nat_func(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  nat_func(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  nat_func(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
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
  RUN_TEST(test_nat);

  return UNITY_END();
}
