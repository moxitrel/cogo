#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

static void yield_resume(cogo_yield_t* const co, int* const v) {
  COGO_BEGIN(co) :;

  (*v)++;
  COGO_YIELD(co);
  (*v)++;

  COGO_END(co) :;
}

static void test_yield(void) {
  cogo_yield_t co = {};
  int v = 0;
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, v);

  yield_resume(&co, &v);
  TEST_ASSERT_NOT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // running
  TEST_ASSERT_NOT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(1, v);

  yield_resume(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, v);

  // noop when coroutine end
  yield_resume(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(2, v);
}

CO_DECLARE(/*NAME*/ return1, int v) {
CO_BEGIN:

  ((return1_t*)co_this)->v++;
  CO_RETURN;
  ((return1_t*)co_this)->v++;

CO_END:;
}

static void test_return(void) {
  return1_t co = CO_MAKE(/*NAME*/ return1, /*v*/ 0);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, co.v);

  CO_RESUME(&co);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // end
  TEST_ASSERT_EQUAL_INT(1, co.v);

  // noop when coroutine end
  CO_RESUME(&co);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(1, co.v);
}

CO_DECLARE(static /*NAME*/ prologue, int enter, int exit) {
  prologue_t* const thiz = (prologue_t*)co_this;
  thiz->enter++;
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:
  thiz->exit++;
}

static void test_prologue(void) {
  prologue_t co = CO_MAKE(/*NAME*/ prologue, /*enter*/ 0, /*exit*/ 0);

  CO_RUN(&co);
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);

  CO_RESUME(&co);
  TEST_ASSERT_EQUAL_INT(3, co.enter);
  TEST_ASSERT_EQUAL_INT(3, co.exit);
}

CO_DECLARE(/*NAME*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)co_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = CO_MAKE(nat);  // "v" is implicitly initialized to ZERO

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  CO_RESUME(&n);
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
