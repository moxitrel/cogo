#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

CO_DECLARE(/*name*/ nat, /*param*/ int v) {
  nat_t* const thiz = (nat_t*)cogo_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = {.v = 0};

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

  RUN_TEST(test_nat);

  return UNITY_END();
}
