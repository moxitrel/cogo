#include <assert.h>
#include <unity.h>

#if defined(COGO_NO_LABELS_AS_VALUES)
#include <cogo/private/cogo_pt_case.h>
#elif defined(__GNUC__)
#include <cogo/private/cogo_pt_goto.h>
#else
#include <cogo/private/cogo_pt_case.h>
#endif

static void func_yield(cogo_pt_t* cogo, int* v) {
  COGO_BEGIN(cogo) :;

  (*v)++;
  COGO_YIELD(cogo);
  (*v)++;

  COGO_END(cogo) :;
}

static void test_yield(void) {
  cogo_pt_t cogo = {0};

  int v = 0;
  TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&cogo));  // begin
  TEST_ASSERT_EQUAL_INT(0, v);

  func_yield(&cogo, &v);
  TEST_ASSERT_NOT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&cogo));  // running
  TEST_ASSERT_NOT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));    // running
  TEST_ASSERT_EQUAL_INT(1, v);

  func_yield(&cogo, &v);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));  // end
  TEST_ASSERT_EQUAL_INT(2, v);

  // noop when coroutine end
  func_yield(&cogo, &v);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&cogo));
  TEST_ASSERT_EQUAL_INT(2, v);
}

typedef struct func_return {
  cogo_pt_t cogo;
  int v;
} func_return_t;

static void func_return(func_return_t* param) {
  COGO_BEGIN(&param->cogo) :;

  (param->v)++;
  COGO_RETURN(&param->cogo);
  (param->v)++;

  COGO_END(&param->cogo) :;
}

static void test_return(void) {
  func_return_t args = {.v = 0};

  TEST_ASSERT_EQUAL_INT64(COGO_PC_BEGIN, COGO_PC(&args.cogo));  // begin
  TEST_ASSERT_EQUAL_INT(0, args.v);

  func_return(&args);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&args.cogo));  // end
  TEST_ASSERT_EQUAL_INT(1, args.v);

  // noop when coroutine end
  func_return(&args);
  TEST_ASSERT_EQUAL_INT64(COGO_PC_END, COGO_PC(&args.cogo));
  TEST_ASSERT_EQUAL_INT(1, args.v);
}

typedef struct prologue {
  int enter;
  int exit;
} prologue_t;

static void func_prologue(cogo_pt_t* cogo_this, prologue_t* prologue) {
  prologue->enter++;
CO_BEGIN:

  CO_YIELD;
  CO_YIELD;

CO_END:
  prologue->exit++;
}

static void test_prologue(void) {
  cogo_pt_t cogo = {0};
  prologue_t prologue = {
      .enter = 0,
      .exit = 0,
  };

  while (COGO_PC(&cogo) != COGO_PC_END) {
    func_prologue(&cogo, &prologue);
  }
  TEST_ASSERT_EQUAL_INT(3, prologue.enter);
  TEST_ASSERT_EQUAL_INT(3, prologue.exit);

  func_prologue(&cogo, &prologue);
  TEST_ASSERT_EQUAL_INT(4, prologue.enter);
  TEST_ASSERT_EQUAL_INT(4, prologue.exit);
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

  return UNITY_END();
}
