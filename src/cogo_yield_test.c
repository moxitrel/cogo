#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

CO_DECLARE(/*NAME*/ yield, int v) {
  yield_t *const thiz = (yield_t *)co_this;
CO_BEGIN:
  thiz->v++;
  CO_YIELD;
  thiz->v++;
CO_END:;
}

static void test_yield(void) {
  yield_t co = CO_MAKE(/*NAME*/ yield, /*v*/ 0);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  yield_func(&co);
  TEST_ASSERT_NOT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // running
  TEST_ASSERT_NOT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));
  TEST_ASSERT_EQUAL_INT(1, co.v);

  yield_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // fini
  TEST_ASSERT_EQUAL_INT(2, co.v);

  // noop when coroutine end
  yield_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // fini
  TEST_ASSERT_EQUAL_INT(2, co.v);
}

CO_DECLARE(/*NAME*/ return0, int v) {
  return0_t *const thiz = (return0_t *)co_this;
CO_BEGIN:
  thiz->v++;
  CO_RETURN;
  thiz->v++;
CO_END:;
}

static void test_return(void) {
  return0_t co = CO_MAKE(/*NAME*/ return0, /*v*/ 0);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  return0_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // fini
  TEST_ASSERT_EQUAL_INT(1, co.v);

  // noop when coroutine end
  return0_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // fini
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
  prologue_t co = CO_MAKE(/*NAME*/ prologue, /*enter*/ 0, /*exit*/ 0);

  while (co_status(&co) != CO_STATUS_FINI) {
    prologue_func(&co);
  }

  // prologue and epilogue are always run even fini
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);

  // prologue and epilogue are always run even fini
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

void test_nat(void) {
  nat_t n = CO_MAKE(/*NAME*/ nat);  // "v" isn't explicitly inited

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
