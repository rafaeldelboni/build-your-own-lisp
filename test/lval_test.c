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

void test_lval_builtin_op(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval *test_calc = lval_builtin_op(test_value, "+");
  TEST_ASSERT_EQUAL_INT(6, test_calc->val_long);
  lval_del(test_calc);
}

void test_lval_builtin_op_err(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("fun"));
  lval *test_calc = lval_builtin_op(test_value, "+");
  TEST_ASSERT_EQUAL_STRING("Cannot operate on non-number!", test_calc->val_err);
  lval_del(test_calc);
}

void test_lval_builtin_op_negation(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_long(1));
  lval *test_calc = lval_builtin_op(test_value, "-");
  TEST_ASSERT_EQUAL_INT(-1, test_calc->val_long);
  lval_del(test_calc);
}

void test_lval_eval(void) {
  /* Evaluate Children */
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval *test_eval = lval_eval(test_value);
  TEST_ASSERT_EQUAL_INT(6, test_eval->val_long);
  lval_del(test_eval);

  /* Empty Expression */
  TEST_ASSERT_EQUAL_INT(LVAL_SEXPR, lval_eval(lval_sexpr())->type);

  /* Single Expression */
  TEST_ASSERT_EQUAL_INT(2, lval_eval(lval_long(2))->val_long);
}

void test_lval_eval_err(void) {
  /* Error Checking */
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_sym("-"));
  lval_add(test_value, lval_long(3));
  lval *test_eval = lval_eval(test_value);
  TEST_ASSERT_EQUAL_STRING("Cannot operate on non-number!", test_eval->val_err);
  lval_del(test_eval);

  /* Ensure First Element is Symbol */
  lval *test_value_2 = lval_sexpr();
  lval_add(test_value_2, lval_long(1));
  lval_add(test_value_2, lval_long(2));
  lval *test_eval_2 = lval_eval(test_value_2);
  TEST_ASSERT_EQUAL_STRING("S-expression Does not start with symbol!", test_eval_2->val_err);
  lval_del(test_eval_2);
}

void test_lval_eval_op_long(void) {
  TEST_ASSERT_EQUAL_INT(
      2, lval_eval_op(lval_long(1), "+", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      0, lval_eval_op(lval_long(1), "-", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      4, lval_eval_op(lval_long(2), "*", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      2, lval_eval_op(lval_long(4), "/", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      4, lval_eval_op(lval_long(10), "%", lval_long(6))->val_long);
  TEST_ASSERT_EQUAL_INT(
      16, lval_eval_op(lval_long(4), "^", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      1, lval_eval_op(lval_long(1), "min", lval_long(5))->val_long);
  TEST_ASSERT_EQUAL_INT(
      1, lval_eval_op(lval_long(1), "min", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      5, lval_eval_op(lval_long(1), "max", lval_long(5))->val_long);
  TEST_ASSERT_EQUAL_INT(
      5, lval_eval_op(lval_long(5), "max", lval_long(5))->val_long);
}

void test_lval_eval_op_double(void) {
  TEST_ASSERT_EQUAL_FLOAT(
      2.2, lval_eval_op(lval_double(1.1), "+", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      0.0, lval_eval_op(lval_double(1.1), "-", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      4.84, lval_eval_op(lval_double(2.2), "*", lval_double(2.2))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.76, lval_eval_op(lval_double(4.4), "/", lval_double(2.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      4.0, lval_eval_op(lval_double(10.0), "%", lval_double(6.0))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      70.9425383673,
      lval_eval_op(lval_double(5.5), "^", lval_double(2.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.1, lval_eval_op(lval_double(1.1), "min", lval_double(5.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.0, lval_eval_op(lval_double(1.0), "min", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.1, lval_eval_op(lval_double(1.1), "min", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1, lval_eval_op(lval_double(1.0), "max", lval_double(5.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1, lval_eval_op(lval_double(5.0), "max", lval_double(5.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1, lval_eval_op(lval_double(5.1), "max", lval_double(5.1))->val_double);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_lval_long);
  RUN_TEST(test_lval_double);
  RUN_TEST(test_lval_sym);
  RUN_TEST(test_lval_sexpr);
  RUN_TEST(test_lval_err);
  RUN_TEST(test_lval_add);
  RUN_TEST(test_lval_pop);
  RUN_TEST(test_lval_take);
  RUN_TEST(test_lval_builtin_op);
  RUN_TEST(test_lval_builtin_op_err);
  RUN_TEST(test_lval_builtin_op_negation);
  RUN_TEST(test_lval_eval);
  RUN_TEST(test_lval_eval_err);
  RUN_TEST(test_lval_eval_op_long);
  RUN_TEST(test_lval_eval_op_double);

  return UNITY_END();
}
