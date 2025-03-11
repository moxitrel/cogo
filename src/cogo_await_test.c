#include <assert.h>
#include <cogo/cogo_await.h>
#include <stdlib.h>
#include <unity.h>

COGO_DECLARE(await2, ) {
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:;
}

COGO_DECLARE(await1, await2_t* a2) {
  await1_t* const thiz = (await1_t*)COGO_THIS;
CO_BEGIN:

  CO_AWAIT(thiz->a2);

CO_END:;
}

static void test_resume(void) {
  await2_t a2 = {
      .COGO_SELF = COGO_INIT(await2, &a2),
  };
  await1_t a1 = {
      .COGO_SELF = COGO_INIT(await1, &a1),
      .a2 = &a2,
  };

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

COGO_DECLARE(static await0, await2_t a2) {
  await0_t* const thiz = (await0_t*)COGO_THIS;
CO_BEGIN:

  COGO_RESUME(&thiz->a2);
  TEST_ASSERT_NOT_NULL(thiz->a2.COGO_SELF.anon.sched->top);

  CO_AWAIT(&thiz->a2);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&thiz->a2));

CO_END:;
}

static void test_await_resumed(void) {
  await0_t a0 = {
      .COGO_SELF = COGO_INIT(await0, &a0),
      .a2 = {
          .COGO_SELF = COGO_INIT(await2, &a0.a2),
      },
  };
  COGO_RUN(&a0);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_STATUS(&a0));
}

COGO_DECLARE(ng, int v) {
  ng_t* const thiz = (ng_t*)COGO_THIS;
CO_BEGIN:

  for (;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_ng(void) {
  ng_t ng = {
      .COGO_SELF = COGO_INIT(ng, &ng),
  };

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(0, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(1, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(2, ng.v);
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

COGO_DECLARE(fib, /*arg*/ int n, /*retval*/ int v, /*local*/ fib_t* fib1, /*local*/ fib_t* fib2) {
  fib_t* const thiz = (fib_t*)COGO_THIS;
CO_BEGIN:
  assert(thiz->n > 0);

  if (thiz->n == 1 || thiz->n == 2) {  // f(1) = 1, f(2) = 1
    thiz->v = 1;
  } else {  // f(n) = f(n-1) + f(n-2)
    thiz->v = 0;

    thiz->fib1 = (fib_t*)malloc(sizeof(*thiz->fib1));
    assert(thiz->fib1);
    *thiz->fib1 = (fib_t){
        .COGO_SELF = COGO_INIT(fib, thiz->fib1),
        .n = thiz->n - 1,
    };
    CO_AWAIT(thiz->fib1);  // eval f(n-1)
    thiz->v += thiz->fib1->v;
    free(thiz->fib1);

    thiz->fib2 = (fib_t*)malloc(sizeof(*thiz->fib2));
    assert(thiz->fib2);
    *thiz->fib2 = (fib_t){
        .COGO_SELF = COGO_INIT(fib, thiz->fib2),
        .n = thiz->n - 2,
    };
    CO_AWAIT(thiz->fib2);  // eval f(n-2)
    thiz->v += thiz->fib2->v;
    free(thiz->fib2);
  }

CO_END:;
}

static void test_fib(void) {
  fib_t f03 = {
      .COGO_SELF = COGO_INIT(fib, &f03),
      .n = 3,
  };
  fib_t f11 = {
      .COGO_SELF = COGO_INIT(fib, &f11),
      .n = 11,
  };
  fib_t f23 = {
      .COGO_SELF = COGO_INIT(fib, &f23),
      .n = 23,
  };

  struct {
    fib_t* fib;
    int v;
  } test_cases[] = {
      {&f03, fib(3)},
      {&f11, fib(11)},
      {&f23, fib(23)},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    COGO_RUN(test_cases[i].fib);
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
  RUN_TEST(test_ng);
  RUN_TEST(test_fib);

  return UNITY_END();
}
