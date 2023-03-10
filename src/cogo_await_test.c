#include <assert.h>
#include <cogo/cogo_await.h>
#include <stdlib.h>
#include <unity.h>

CO_DECLARE(/*NAME*/ await2) {
CO_BEGIN:
  CO_YIELD;
CO_END:;
}

CO_DECLARE(/*NAME*/ await1, await2_t* await2) {
  await1_t* const thiz = (await1_t*)co_this;
CO_BEGIN:
  CO_AWAIT(thiz->await2);
CO_END:;
}

static void test_resume(void) {
  await2_t await2 = CO_MAKE(/*NAME*/ await2);
  await1_t await1 = CO_MAKE(/*NAME*/ await1, /*await2*/ &await2);
  cogo_await_t* co = &await1.super;

  // init
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&await1));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, co_status(&await2));

  // await2 yield: stop when CO_YIELD, but not when CO_AWAIT or CO_RETURN (except root coroutine)
  co = CO_RESUME(co);
  TEST_ASSERT_EQUAL_PTR(&await2, co);
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, co_status(&await1));
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, co_status(&await2));

  // await1 fini: stop when root coroutine fini
  co = CO_RESUME(co);
  TEST_ASSERT_NULL(co);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&await1));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, co_status(&await2));
}

CO_DECLARE(/*NAME*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)co_this;
CO_BEGIN:
  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }
CO_END:;
}

void test_nat(void) {
  nat_t n = CO_MAKE(/*NAME*/ nat);  // "v" isn't explicitly inited

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
}

static int fibonacci(int n) {
  assert(n > 0);
  switch (n) {
    case 1:
    case 2:
      return 1;
    default:
      return fibonacci(n - 1) + fibonacci(n - 2);
  }
}

CO_DECLARE(/*NAME*/ fibonacci, /*parameter*/ int n, /*return*/ int v, /*local variable*/ fibonacci_t* fib_n1, /*local variable*/ fibonacci_t* fib_n2) {
  fibonacci_t* const thiz = (fibonacci_t*)co_this;
CO_BEGIN:
  assert(thiz->n > 0);
  switch (thiz->n) {
    case 1:  // f(1) = 1
    case 2:  // f(2) = 1
      thiz->v = 1;
      CO_RETURN;
    default:  // f(n) = f(n-1) + f(n-2)
      thiz->v = 0;

      thiz->fib_n1 = (fibonacci_t*)malloc(sizeof(*thiz->fib_n1));
      TEST_ASSERT_NOT_NULL(thiz->fib_n1);
      *thiz->fib_n1 = CO_MAKE(/*NAME*/ fibonacci, thiz->n - 1);
      CO_AWAIT(thiz->fib_n1);  // eval f(n-1)
      thiz->v += thiz->fib_n1->v;
      free(thiz->fib_n1);

      thiz->fib_n2 = (fibonacci_t*)malloc(sizeof(*thiz->fib_n2));
      TEST_ASSERT_NOT_NULL(thiz->fib_n2);
      *thiz->fib_n2 = CO_MAKE(/*NAME*/ fibonacci, thiz->n - 2);
      CO_AWAIT(thiz->fib_n2);  // eval f(n-2)
      thiz->v += thiz->fib_n2->v;
      free(thiz->fib_n2);

      CO_RETURN;
  }

CO_END:;
}

static void test_fibonacci(void) {
  struct {
    fibonacci_t fib;
    int v;
  } test_cases[] = {
      // "v", "fib_n1" and "fib_n2" aren't needed to explicitly init
      {CO_MAKE(fibonacci, 1), fibonacci(1)},
      {CO_MAKE(fibonacci, 2), fibonacci(2)},
      {CO_MAKE(fibonacci, 11), fibonacci(11)},
      {CO_MAKE(fibonacci, 23), fibonacci(23)},
      {CO_MAKE(fibonacci, 29), fibonacci(29)},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    CO_RUN(&test_cases[i].fib);
    TEST_ASSERT_EQUAL_INT(test_cases[i].fib.v, test_cases[i].v);
  }
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_resume);
  RUN_TEST(test_nat);
  RUN_TEST(test_fibonacci);

  return UNITY_END();
}
