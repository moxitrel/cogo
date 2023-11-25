#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

static void yield_resume(cogo_yield_t* const co, int* const v) {
  CO_BEGIN_F(co);

  (*v)++;
  CO_YIELD_F(co);
  (*v)++;

  CO_END_F(co);
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

static void return_resume(cogo_yield_t* const co, int* const v) {
  CO_BEGIN_F(co);

  (*v)++;
  CO_RETURN_F(co);
  (*v)++;

  CO_END_F(co);
}

static void test_return(void) {
  cogo_yield_t co = {};
  int v = 0;
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, v);

  return_resume(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // end
  TEST_ASSERT_EQUAL_INT(1, v);

  // noop when coroutine end
  return_resume(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(1, v);
}

typedef struct prologue {
  cogo_yield_t co;
  int enter;
  int exit;
} prologue_t;

static void prologue_resume(prologue_t* const prologue) {
  prologue->enter++;
  CO_BEGIN_F(&prologue->co);

  CO_YIELD_F(&prologue->co);
  CO_YIELD_F(&prologue->co);

  CO_END_F(&prologue->co);
  prologue->exit++;
}

static void test_prologue(void) {
  prologue_t prologue = {
      .enter = 0,
      .exit = 0,
  };

  while (CO_STATUS(&prologue.co) != CO_STATUS_END) {
    prologue_resume(&prologue);
  }
  TEST_ASSERT_EQUAL_INT(3, prologue.enter);
  TEST_ASSERT_EQUAL_INT(3, prologue.exit);

  prologue_resume(&prologue);
  TEST_ASSERT_EQUAL_INT(4, prologue.enter);
  TEST_ASSERT_EQUAL_INT(4, prologue.exit);
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
