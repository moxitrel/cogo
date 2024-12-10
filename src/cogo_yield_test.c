#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

COGO_DECLARE(/*NAME*/ ng, /*params*/ int v) {
  ng_t* const thiz = (ng_t*)cogo_this;
CO_BEGIN:

  for (;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_ng(void) {
  ng_t ng = {
      .cogo = COGO_INIT(ng, &ng),
      .v = 0,
  };

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(0, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(1, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(2, ng.v);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_ng);

  return UNITY_END();
}
