#include <assert.h>
#include <cogo/cogo_co.h>
#include <stdlib.h>
#include <unity.h>

void setUp(void) {
}

void tearDown(void) {
}

int cogo_sch_push(cogo_sch_t* sch, cogo_co_t* co) {
  assert(sch);
  assert(sch->stack_top);
  assert(co);
  if (co != sch->stack_top) {
    co->caller = sch->stack_top;
    sch->stack_top = co;
  }
  return 1;
}

cogo_co_t* cogo_sch_pop(cogo_sch_t* sch) {
  assert(sch);
  return sch->stack_top;
}

static inline void cogo_co_run(void* co) {
  cogo_sch_t sch = {
      .stack_top = (cogo_co_t*)co,
  };
  while (cogo_sch_step(&sch)) {
    // noop
  }
}

CO_DECLARE(static f3) {
CO_BEGIN:

  CO_YIELD;
  CO_RETURN;
  CO_YIELD;  // never run

CO_END:;
}

CO_DECLARE(static f2, f3_t f3) {
  f2_t* thiz = (f2_t*)co_this;
CO_BEGIN:

  CO_YIELD;
  CO_AWAIT(&thiz->f3);

CO_END:;
}

CO_DECLARE(static f1, f2_t f2) {
  f1_t* thiz = (f1_t*)co_this;
CO_BEGIN:

  CO_AWAIT(&thiz->f2);

CO_END:;
}

static void test_step(void) {
  f1_t comain = CO_MAKE(f1, CO_MAKE(f2, CO_MAKE(f3)));
  cogo_sch_t sch = {
      .stack_top = (cogo_co_t*)&comain,
  };

  f1_t* f1 = &comain;
  f2_t* f2 = &f1->f2;
  f3_t* f3 = &f2->f3;
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(f1));
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(f2));
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(f3));

  // fc2 yield
  cogo_co_t* co = cogo_sch_step(&sch);
  TEST_ASSERT_EQUAL_PTR(f2, co);
  TEST_ASSERT_GREATER_THAN_UINT(COGO_STATUS_INITED, co_status(f1));
  TEST_ASSERT_GREATER_THAN_UINT(COGO_STATUS_INITED, co_status(f2));
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_INITED, co_status(f3));

  // fc3 first yield
  co = cogo_sch_step(&sch);
  TEST_ASSERT_EQUAL_PTR(f3, co);
  TEST_ASSERT_GREATER_THAN_UINT(COGO_STATUS_INITED, co_status(f1));
  TEST_ASSERT_GREATER_THAN_UINT(COGO_STATUS_INITED, co_status(f2));
  TEST_ASSERT_GREATER_THAN_UINT(COGO_STATUS_INITED, co_status(f3));

  // fc3 co_return
  co = cogo_sch_step(&sch);
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(f1));
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(f2));
  TEST_ASSERT_EQUAL_UINT(COGO_STATUS_STOPPED, co_status(f3));
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

CO_DECLARE(static fibonacci, int n, int v, fibonacci_t* fib_n1, fibonacci_t* fib_n2) {
  fibonacci_t* thiz = (fibonacci_t*)co_this;
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
    cogo_co_run(&example[i].fib);
    TEST_ASSERT_EQUAL_INT(example[i].fib.v, example[i].value);
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_step);
  RUN_TEST(test_run);

  return UNITY_END();
}
