#include <assert.h>
#include <cogo/cogo_await.h>
#include <stdlib.h>
#include <unity.h>

COGO_DECLARE(/*NAME*/ await2) {
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:;
}

COGO_DECLARE(/*NAME*/ await1, /*param*/ await2_t* a2);
COGO_DEFINE(/*NAME*/ await1) {
  await1_t* const thiz = (await1_t*)cogo_this;
CO_BEGIN:

  CO_AWAIT(thiz->a2);

CO_END:;
}

static void test_resume(void) {
  await2_t a2 = COGO_INIT(/*NAME*/ await2, /*this*/ &a2);
  await1_t a1 = COGO_INIT(/*NAME*/ await1, /*this*/ &a1, /*param*/ &a2);

  // begin
  TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_STATUS(&a1));
  TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_STATUS(&a2));

  // await2 yield: stop when CO_YIELD, but not CO_AWAIT or CO_RETURN
  COGO_RESUME(&a1);
  TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_STATUS(&a2));
  TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_STATUS(&a2));
  TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_STATUS(&a1));
  TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_STATUS(&a1));

  // await2 yield
  COGO_RESUME(&a1);
  TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_STATUS(&a2));
  TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_STATUS(&a2));
  TEST_ASSERT_GREATER_THAN_INT64(COGO_PC_BEGIN, COGO_STATUS(&a1));
  TEST_ASSERT_LESS_THAN_UINT64(COGO_PC_END, COGO_STATUS(&a1));

  // await1 end: stop when root coroutine finished
  COGO_RESUME(&a1);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a2));
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a1));

  // noop when coroutine end
  COGO_RESUME(&a1);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a2));
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a1));
}

COGO_DECLARE(/*NAME*/ static await0, /*param*/ await2_t a2) {
  await0_t* const thiz = (await0_t*)cogo_this;
CO_BEGIN:

  COGO_RESUME(&thiz->a2);
  TEST_ASSERT_NOT_NULL(thiz->a2.cogo.sched->top);

  CO_AWAIT(&thiz->a2);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&thiz->a2));

CO_END:;
}

static void test_await_resumed(void) {
  await0_t a0 = COGO_INIT(
      /*NAME*/ await0,
      /*this*/ &a0,
      /*param*/ COGO_INIT(
          /*NAME*/ await2,
          /*this*/ &a0.a2));
  while (COGO_RESUME(&a0) != COGO_PC_END) {
  }
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a0));
}

COGO_DECLARE(/*NAME*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)cogo_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = COGO_INIT(/*NAME*/ nat, /*this*/ &n);  // "v" is implicitly initialized to 0

  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
}

static int fib(int n) {
  assert(n > 0);
  switch (n) {
    case 1:
    case 2:
      return 1;
    default:
      return fib(n - 1) + fib(n - 2);
  }
}

COGO_DECLARE(/*NAME*/ fib2, /*param*/ int n, /*return*/ int v, /*local*/ fib2_t* fib_n1, /*local*/ fib2_t* fib_n2) {
  fib2_t* const thiz = (fib2_t*)cogo_this;
CO_BEGIN:
  assert(thiz->n > 0);

  if (thiz->n == 1 || thiz->n == 2) {  // f(1) = 1, f(2) = 1
    thiz->v = 1;
  } else {  // f(n) = f(n-1) + f(n-2)
    thiz->v = 0;

    thiz->fib_n1 = (fib2_t*)malloc(sizeof(*thiz->fib_n1));
    assert(thiz->fib_n1);
    // "v", "fib_n1" and "fib_n2" are implicitly initialized to 0.
    *thiz->fib_n1 = COGO_INIT(/*NAME*/ fib2, /*this*/ thiz->fib_n1, /*param*/ thiz->n - 1);
    CO_AWAIT(thiz->fib_n1);  // eval f(n-1)
    thiz->v += thiz->fib_n1->v;
    free(thiz->fib_n1);

    thiz->fib_n2 = (fib2_t*)malloc(sizeof(*thiz->fib_n2));
    assert(thiz->fib_n2);
    *thiz->fib_n2 = COGO_INIT(/*NAME*/ fib2, /*this*/ thiz->fib_n2, /*param*/ thiz->n - 2);
    CO_AWAIT(thiz->fib_n2);  // eval f(n-2)
    thiz->v += thiz->fib_n2->v;
    free(thiz->fib_n2);
  }

CO_END:;
}

static void test_fib2(void) {
  fib2_t f03 = COGO_INIT(/*NAME*/ fib2, /*this*/ &f03, /*param*/ 3);
  fib2_t f11 = COGO_INIT(/*NAME*/ fib2, /*this*/ &f11, /*param*/ 11);
  fib2_t f23 = COGO_INIT(/*NAME*/ fib2, /*this*/ &f23, /*param*/ 23);

  struct {
    fib2_t* fib;
    int v;
  } test_cases[] = {
      {&f03, fib(3)},
      {&f11, fib(11)},
      {&f23, fib(23)},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    // run
    while (COGO_RESUME(test_cases[i].fib) != COGO_PC_END) {
    }
    TEST_ASSERT_EQUAL_INT(test_cases[i].fib->v, test_cases[i].v);
  }
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_resume);
  RUN_TEST(test_await_resumed);
  RUN_TEST(test_nat);
  RUN_TEST(test_fib2);

  return UNITY_END();
}
