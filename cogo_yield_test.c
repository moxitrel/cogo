#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

void setUp(void) {
}

void tearDown(void) {
}

CO_DECLARE(coyield, int v) {
CO_BEGIN:

  ((coyield_t *)co_this)->v++;
  CO_YIELD;
  ((coyield_t *)co_this)->v++;

CO_END:;
}

static void test_yield(void) {
  coyield_t co = CO_MAKE(coyield, 0);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  coyield_func(&co);
  TEST_ASSERT_NOT_EQUAL_UINT(COGO_STATUS_INITED, co_status(&co));   // running
  TEST_ASSERT_NOT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(1, co.v);

  coyield_func(&co);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, co.v);

  // noop when coroutine end
  coyield_func(&co);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(2, co.v);
}

CO_DECLARE(coreturn, int v) {
CO_BEGIN:

  ((coreturn_t *)co_this)->v++;
  CO_RETURN;
  ((coreturn_t *)co_this)->v++;

CO_END:;
}

static void test_return(void) {
  coreturn_t co = CO_MAKE(coreturn, 0);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(&co));  // init
  TEST_ASSERT_EQUAL_INT(0, co.v);

  coreturn_func(&co);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(&co));  // running
  TEST_ASSERT_EQUAL_INT(1, co.v);
}

CO_DECLARE(prologue, int enter, int exit) {
  prologue_t *thiz = (prologue_t *)co_this;
  thiz->enter++;
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:
  thiz->exit++;
}

static void test_prologue(void) {
  prologue_t co = CO_MAKE(prologue, 0, 0);

  while (co_status(&co) != COGO_STATUS_STOPPED) {
    prologue_func(&co);
  }
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);

  prologue_func(&co);
  TEST_ASSERT_EQUAL_INT(4, co.enter);
  TEST_ASSERT_EQUAL_INT(4, co.exit);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_yield);
  RUN_TEST(test_return);
  RUN_TEST(test_prologue);

  return UNITY_END();
}
