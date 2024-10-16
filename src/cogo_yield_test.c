#include <assert.h>
#include <cogo/cogo_yield.h>
#include <string.h>
#include <unity.h>

static void func_yield(cogo_yield_t* co, int* v) {
  COGO_BEGIN(co) :;

  (*v)++;
  COGO_YIELD(co);
  (*v)++;

  COGO_END(co) :;
}

static void test_yield(void) {
  cogo_yield_t co;
  memset(&co, 0, sizeof(co));

  int v = 0;
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, v);

  func_yield(&co, &v);
  TEST_ASSERT_NOT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // running
  TEST_ASSERT_NOT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // running
  TEST_ASSERT_EQUAL_INT(1, v);

  func_yield(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // end
  TEST_ASSERT_EQUAL_INT(2, v);

  // noop when coroutine end
  func_yield(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(2, v);
}

static void func_return(cogo_yield_t* co, int* v) {
  COGO_BEGIN(co) :;

  (*v)++;
  COGO_RETURN(co);
  (*v)++;

  COGO_END(co) :;
}

static void test_return(void) {
  cogo_yield_t co;
  memset(&co, 0, sizeof(co));

  int v = 0;
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_BEGIN, CO_STATUS(&co));  // begin
  TEST_ASSERT_EQUAL_INT(0, v);

  func_return(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));  // end
  TEST_ASSERT_EQUAL_INT(1, v);

  // noop when coroutine end
  func_return(&co, &v);
  TEST_ASSERT_EQUAL_INT64(CO_STATUS_END, CO_STATUS(&co));
  TEST_ASSERT_EQUAL_INT(1, v);
}

typedef struct prologue {
  int enter;
  int exit;
  cogo_yield_t co;
} prologue_t;

static void func_prologue(prologue_t* prologue) {
  prologue->enter++;
  COGO_BEGIN(&prologue->co) :;

  COGO_YIELD(&prologue->co);
  COGO_YIELD(&prologue->co);

  COGO_END(&prologue->co) :;
  prologue->exit++;
}

static void test_prologue(void) {
  prologue_t prologue = {
      .enter = 0,
      .exit = 0,
  };

  while (CO_STATUS(&prologue.co) != CO_STATUS_END) {
    func_prologue(&prologue);
  }
  TEST_ASSERT_EQUAL_INT(3, prologue.enter);
  TEST_ASSERT_EQUAL_INT(3, prologue.exit);

  func_prologue(&prologue);
  TEST_ASSERT_EQUAL_INT(4, prologue.enter);
  TEST_ASSERT_EQUAL_INT(4, prologue.exit);
}

CO_DECLARE(/*func*/ nat, /*param*/ int v) {
  nat_t* const thiz = (nat_t*)co_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n;
  memset(&n, 0, sizeof(n));

  nat_resume(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  nat_resume(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  nat_resume(&n);
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
