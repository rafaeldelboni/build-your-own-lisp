#include "lval.h"
#include "unity.h"
#include <stdlib.h>

void setUp(void) {}

void tearDown(void) {}

void test_lval_long(void) {
  lval test_num = lval_long(123);
  TEST_ASSERT_EQUAL_INT32(123, test_num.val_long);
  TEST_ASSERT_EQUAL_INT(LVAL_LONG, test_num.type);
}

void test_lval_double(void) {
  lval test_num = lval_double(123.12);
  TEST_ASSERT_EQUAL_FLOAT(123.12, test_num.val_double);
  TEST_ASSERT_EQUAL_INT(LVAL_DOUBLE, test_num.type);
}

void test_lval_err(void) {
  lval test_num = lval_err(123);
  TEST_ASSERT_EQUAL_INT(123, test_num.err);
  TEST_ASSERT_EQUAL_INT(LVAL_ERR, test_num.type);
}

void test_lval_string(void) {
  char *output = (char *)malloc(sizeof(char) * 4);
  lval_string(lval_long(123), output);
  TEST_ASSERT_EQUAL_STRING("123", output);

  lval_string(lval_err(LERR_DIV_ZERO), output);
  TEST_ASSERT_EQUAL_STRING("Error: Division By Zero!", output);

  lval_string(lval_err(LERR_BAD_OP), output);
  TEST_ASSERT_EQUAL_STRING("Error: Invalid Operator!", output);

  lval_string(lval_err(LERR_BAD_NUM), output);
  TEST_ASSERT_EQUAL_STRING("Error: Invalid Number!", output);

  free(output);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_lval_long);
  RUN_TEST(test_lval_double);
  RUN_TEST(test_lval_err);
  RUN_TEST(test_lval_string);

  return UNITY_END();
}
