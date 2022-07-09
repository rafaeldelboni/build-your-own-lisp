#include "lval.h"
#include "unity.h"
#include <stdlib.h>

void setUp(void) {}

void tearDown(void) {}

void test_lval_long(void) {
  lval *test_value = lval_long(123);
  TEST_ASSERT_EQUAL_INT32(123, test_value->val_long);
  TEST_ASSERT_EQUAL_INT(LVAL_LONG, test_value->type);
  lval_del(test_value);
}

void test_lval_double(void) {
  lval *test_value = lval_double(123.12);
  TEST_ASSERT_EQUAL_FLOAT(123.12, test_value->val_double);
  TEST_ASSERT_EQUAL_INT(LVAL_DOUBLE, test_value->type);
  lval_del(test_value);
}

void test_lval_sym(void) {
  lval *test_value = lval_sym("fun");
  TEST_ASSERT_EQUAL_STRING("fun", test_value->val_symbol);
  TEST_ASSERT_EQUAL_INT(LVAL_SYM, test_value->type);
  lval_del(test_value);
}

void test_lval_sexpr(void) {
  lval *test_value = lval_sexpr();
  TEST_ASSERT_EQUAL_INT(0, test_value->count);
  TEST_ASSERT_EQUAL_INT(LVAL_SEXPR, test_value->type);
  TEST_ASSERT_EQUAL(NULL, test_value->cell);
  lval_del(test_value);
}

void test_lval_qexpr(void) {
  lval *test_value = lval_qexpr();
  TEST_ASSERT_EQUAL_INT(0, test_value->count);
  TEST_ASSERT_EQUAL_INT(LVAL_QEXPR, test_value->type);
  TEST_ASSERT_EQUAL(NULL, test_value->cell);
  lval_del(test_value);
}

void test_lval_err(void) {
  lval *test_value = lval_err("123");
  TEST_ASSERT_EQUAL_STRING("123", test_value->val_err);
  TEST_ASSERT_EQUAL_INT(LVAL_ERR, test_value->type);
  lval_del(test_value);
}

void test_lval_add(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  TEST_ASSERT_EQUAL_INT(4, test_value->count);
  TEST_ASSERT_EQUAL_STRING("+", test_value->cell[0]->val_symbol);
  TEST_ASSERT_EQUAL_INT(1, test_value->cell[1]->val_long);
  TEST_ASSERT_EQUAL_INT(2, test_value->cell[2]->val_long);
  TEST_ASSERT_EQUAL_INT(3, test_value->cell[3]->val_long);
  lval_del(test_value);
}

void test_lval_pop(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval_pop(test_value, 1);
  TEST_ASSERT_EQUAL_INT(3, test_value->count);
  TEST_ASSERT_EQUAL_STRING("+", test_value->cell[0]->val_symbol);
  TEST_ASSERT_EQUAL_INT(2, test_value->cell[1]->val_long);
  TEST_ASSERT_EQUAL_INT(3, test_value->cell[2]->val_long);
  lval_del(test_value);
}

void test_lval_take(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval *test_take = lval_take(test_value, 2);
  TEST_ASSERT_EQUAL_INT(0, test_take->count);
  TEST_ASSERT_EQUAL_INT(2, test_take->val_long);
  lval_del(test_take);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_lval_long);
  RUN_TEST(test_lval_double);
  RUN_TEST(test_lval_sym);
  RUN_TEST(test_lval_sexpr);
  RUN_TEST(test_lval_qexpr);
  RUN_TEST(test_lval_err);
  RUN_TEST(test_lval_add);
  RUN_TEST(test_lval_pop);
  RUN_TEST(test_lval_take);

  return UNITY_END();
}
