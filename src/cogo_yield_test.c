#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

CO_DECLARE(yield_tester, int v) {
CO_BEGIN:

  ((yield_tester_t *)co_this)->v++;
  CO_YIELD;
  ((yield_tester_t *)co_this)->v++;

CO_END:;
}

static void test_yield(void) {
  yield_tester_t co = CO_MAKE(yield_tester, 0);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  yield_tester_func(&co);
  TEST_ASSERT_NOT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // running
  TEST_ASSERT_NOT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(1, co.v);

  yield_tester_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, co.v);

  // noop when coroutine end
  yield_tester_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(2, co.v);
}

CO_DECLARE(return_tester, int v) {
CO_BEGIN:

  ((return_tester_t *)co_this)->v++;
  CO_RETURN;
  ((return_tester_t *)co_this)->v++;

CO_END:;
}

static void test_return(void) {
  return_tester_t co = CO_MAKE(return_tester, 0);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  return_tester_func(&co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(1, co.v);
}

CO_DECLARE(prologue_tester, int enter, int exit) {
  prologue_tester_t *thiz = (prologue_tester_t *)co_this;
  thiz->enter++;
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:
  thiz->exit++;
}

static void test_prologue(void) {
  prologue_tester_t co = CO_MAKE(prologue_tester, 0, 0);

  while (co_status(&co) != CO_STATUS_FINI) {
    prologue_tester_func(&co);
  }
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);

  prologue_tester_func(&co);
  TEST_ASSERT_EQUAL_INT(4, co.enter);
  TEST_ASSERT_EQUAL_INT(4, co.exit);
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

  return UNITY_END();
}
