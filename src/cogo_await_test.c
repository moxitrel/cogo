#include <assert.h>
#include <cogo/cogo_await.h>
#include <stdlib.h>
#include <unity.h>

CO_DECLARE(co3) {
CO_BEGIN:

  CO_YIELD;
  CO_RETURN;
  CO_YIELD;  // never run

CO_END:;
}

CO_DECLARE(co2, co3_t co3) {
  co2_t* const thiz = (co2_t*)co_this;
CO_BEGIN:

  CO_YIELD;
  CO_AWAIT(&thiz->co3);

CO_END:;
}

CO_DECLARE(co1, co2_t co2) {
  co1_t* const thiz = (co1_t*)co_this;
CO_BEGIN:

  CO_AWAIT(&thiz->co2);

CO_END:;
}

static void test_step(void) {
  co1_t main = CO_MAKE(co1, CO_MAKE(co2, CO_MAKE(co3)));
  cogo_await_sched_t sched = {
      .stack_top = &main.super,
  };

  co1_t* const co1 = &main;
  co2_t* const co2 = &co1->co2;
  co3_t* const co3 = &co2->co3;
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, cogo_status(co1));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, cogo_status(co2));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, cogo_status(co3));

  // fc2 yield
  cogo_await_t* co = CO_SCHED_STEP(&sched);
  TEST_ASSERT_EQUAL_PTR(co2, co);
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, cogo_status(co1));
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, cogo_status(co2));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_INIT, cogo_status(co3));

  // fc3 first yield
  co = CO_SCHED_STEP(&sched);
  TEST_ASSERT_EQUAL_PTR(co3, co);
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, cogo_status(co1));
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, cogo_status(co2));
  TEST_ASSERT_GREATER_THAN_UINT(CO_STATUS_INIT, cogo_status(co3));

  // fc3 co_return
  co = CO_SCHED_STEP(&sched);
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, cogo_status(co1));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, cogo_status(co2));
  TEST_ASSERT_EQUAL_UINT(CO_STATUS_FINI, cogo_status(co3));
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

CO_DECLARE(fibonacci, int n, int v, fibonacci_t* fib_n1, fibonacci_t* fib_n2) {
  fibonacci_t* const thiz = (fibonacci_t*)co_this;
CO_BEGIN:
  assert(thiz->n > 0);

  switch (thiz->n) {
    case 1:  // f(1) = 1
    case 2:  // f(2) = 1
      thiz->v = 1;
      CO_RETURN;
    default:  // f(n) = f(n-1) + f(n-2)
      thiz->fib_n1 = (fibonacci_t*)malloc(sizeof(*thiz->fib_n1));
      thiz->fib_n2 = (fibonacci_t*)malloc(sizeof(*thiz->fib_n2));
      assert(thiz->fib_n1);
      assert(thiz->fib_n2);
      *thiz->fib_n1 = CO_MAKE(fibonacci, thiz->n - 1);
      *thiz->fib_n2 = CO_MAKE(fibonacci, thiz->n - 2);

      CO_AWAIT(thiz->fib_n1);  // eval f(n-1)
      CO_AWAIT(thiz->fib_n2);  // eval f(n-2)
      thiz->v = thiz->fib_n1->v + thiz->fib_n2->v;

      free(thiz->fib_n1);
      free(thiz->fib_n2);
      CO_RETURN;
  }

CO_END:;
}

static void test_run(void) {
  struct {
    fibonacci_t fib;
    int value;
  } example[] = {
      {CO_MAKE(fibonacci, 1), fibonacci(1)},
      {CO_MAKE(fibonacci, 2), fibonacci(2)},
      {CO_MAKE(fibonacci, 11), fibonacci(11)},
      {CO_MAKE(fibonacci, 23), fibonacci(23)},
      {CO_MAKE(fibonacci, 29), fibonacci(29)},
  };

  for (size_t i = 0; i < sizeof(example) / sizeof(example[0]); i++) {
    CO_RUN(&example[i].fib);
    TEST_ASSERT_EQUAL_INT(example[i].fib.v, example[i].value);
  }
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_step);
  RUN_TEST(test_run);

  return UNITY_END();
}
