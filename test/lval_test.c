#include "lval.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_lval_num(void) {
  lval test_num = lval_num(123);
  TEST_ASSERT_EQUAL_INT(123, test_num.num);
  TEST_ASSERT_EQUAL_INT(LVAL_NUM, test_num.type);
}

void test_lval_err(void) {
  lval test_num = lval_err(123);
  TEST_ASSERT_EQUAL_INT(123, test_num.err);
  TEST_ASSERT_EQUAL_INT(LVAL_ERR, test_num.type);
}

void test_lval_string(void) {
  TEST_ASSERT_EQUAL_STRING("123", lval_string(lval_num(123)));
  TEST_ASSERT_EQUAL_STRING("Error: Division By Zero!",
                           lval_string(lval_err(LERR_DIV_ZERO)));
  TEST_ASSERT_EQUAL_STRING("Error: Invalid Operator!",
                           lval_string(lval_err(LERR_BAD_OP)));
  TEST_ASSERT_EQUAL_STRING("Error: Invalid Number!",
                           lval_string(lval_err(LERR_BAD_NUM)));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_lval_num);
  RUN_TEST(test_lval_err);
  RUN_TEST(test_lval_string);

  return UNITY_END();
}
