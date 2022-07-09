#include "builtin.h"
#include "unity.h"
#include <stdlib.h>

void setUp(void) {}

void tearDown(void) {}

void test_builtin_op(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval *test_calc = builtin_op(test_value, "+");
  TEST_ASSERT_EQUAL_INT(6, test_calc->val_long);
  lval_del(test_calc);
}

void test_builtin_op_err(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("fun"));
  lval *test_calc = builtin_op(test_value, "+");
  TEST_ASSERT_EQUAL_STRING("Cannot operate on non-number!", test_calc->val_err);
  lval_del(test_calc);
}

void test_builtin_op_negation(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_long(1));
  lval *test_calc = builtin_op(test_value, "-");
  TEST_ASSERT_EQUAL_INT(-1, test_calc->val_long);
  lval_del(test_calc);
}

void test_do_eval(void) {
  /* Evaluate Children */
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));
  lval *test_eval = builtin_lval_eval(test_value);
  TEST_ASSERT_EQUAL_INT(6, test_eval->val_long);
  lval_del(test_eval);

  /* Empty Expression */
  TEST_ASSERT_EQUAL_INT(LVAL_SEXPR, builtin_lval_eval(lval_sexpr())->type);

  /* Single Expression */
  TEST_ASSERT_EQUAL_INT(2, builtin_lval_eval(lval_long(2))->val_long);
}

void test_eval_err(void) {
  /* Error Checking */
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_sym("-"));
  lval_add(test_value, lval_long(3));
  lval *test_eval = builtin_lval_eval(test_value);
  TEST_ASSERT_EQUAL_STRING("Cannot operate on non-number!", test_eval->val_err);
  lval_del(test_eval);

  /* Ensure First Element is Symbol */
  lval *test_value_2 = lval_sexpr();
  lval_add(test_value_2, lval_long(1));
  lval_add(test_value_2, lval_long(2));
  lval *test_eval_2 = builtin_lval_eval(test_value_2);
  TEST_ASSERT_EQUAL_STRING("S-expression Does not start with symbol!",
                           test_eval_2->val_err);
  lval_del(test_eval_2);
}

void test_builtin_eval_op_long(void) {
  TEST_ASSERT_EQUAL_INT(
      2, builtin_eval_op(lval_long(1), "+", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      0, builtin_eval_op(lval_long(1), "-", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      4, builtin_eval_op(lval_long(2), "*", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      2, builtin_eval_op(lval_long(4), "/", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      4, builtin_eval_op(lval_long(10), "%", lval_long(6))->val_long);
  TEST_ASSERT_EQUAL_INT(
      16, builtin_eval_op(lval_long(4), "^", lval_long(2))->val_long);
  TEST_ASSERT_EQUAL_INT(
      1, builtin_eval_op(lval_long(1), "min", lval_long(5))->val_long);
  TEST_ASSERT_EQUAL_INT(
      1, builtin_eval_op(lval_long(1), "min", lval_long(1))->val_long);
  TEST_ASSERT_EQUAL_INT(
      5, builtin_eval_op(lval_long(1), "max", lval_long(5))->val_long);
  TEST_ASSERT_EQUAL_INT(
      5, builtin_eval_op(lval_long(5), "max", lval_long(5))->val_long);
}

void test_builtin_eval_op_double(void) {
  TEST_ASSERT_EQUAL_FLOAT(
      2.2,
      builtin_eval_op(lval_double(1.1), "+", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      0.0,
      builtin_eval_op(lval_double(1.1), "-", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      4.84,
      builtin_eval_op(lval_double(2.2), "*", lval_double(2.2))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.76,
      builtin_eval_op(lval_double(4.4), "/", lval_double(2.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      4.0,
      builtin_eval_op(lval_double(10.0), "%", lval_double(6.0))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      70.9425383673,
      builtin_eval_op(lval_double(5.5), "^", lval_double(2.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.1,
      builtin_eval_op(lval_double(1.1), "min", lval_double(5.5))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.0,
      builtin_eval_op(lval_double(1.0), "min", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      1.1,
      builtin_eval_op(lval_double(1.1), "min", lval_double(1.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1,
      builtin_eval_op(lval_double(1.0), "max", lval_double(5.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1,
      builtin_eval_op(lval_double(5.0), "max", lval_double(5.1))->val_double);
  TEST_ASSERT_EQUAL_FLOAT(
      5.1,
      builtin_eval_op(lval_double(5.1), "max", lval_double(5.1))->val_double);
}

void test_builtin_head(void) {
  lval *test_value = lval_qexpr();
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));

  lval *test_value_parent = lval_sexpr();
  lval_add(test_value_parent, test_value);

  lval *test_calc = builtin_head(test_value_parent);
  TEST_ASSERT_EQUAL_INT(1, test_calc->cell[0]->val_long);

  lval_del(test_calc);
}

void test_builtin_tail(void) {
  lval *test_value = lval_qexpr();
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));

  lval *test_value_parent = lval_sexpr();
  lval_add(test_value_parent, test_value);

  lval *test_calc = builtin_tail(test_value_parent);
  TEST_ASSERT_EQUAL_INT(2, test_calc->cell[0]->val_long);
  TEST_ASSERT_EQUAL_INT(3, test_calc->cell[1]->val_long);

  lval_del(test_calc);
}

void test_builtin_list(void) {
  lval *test_value = lval_sexpr();
  lval_add(test_value, lval_long(1));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));

  lval *test_calc = builtin_list(test_value);
  TEST_ASSERT_EQUAL_INT(LVAL_QEXPR, test_calc->type);

  lval_del(test_calc);
}

void test_builtin_eval(void) {
  lval *test_value = lval_qexpr();
  lval_add(test_value, lval_sym("+"));
  lval_add(test_value, lval_long(2));
  lval_add(test_value, lval_long(3));

  lval *test_value_parent = lval_sexpr();
  test_value_parent = lval_add(test_value_parent, test_value);

  lval *test_calc = builtin_eval(test_value_parent);
  TEST_ASSERT_EQUAL_INT(5, test_calc->val_long);

  lval_del(test_calc);
}

void test_builtin_join(void) {
  lval *test_value_1 = lval_qexpr();
  lval_add(test_value_1, lval_long(1));
  lval_add(test_value_1, lval_long(2));
  lval_add(test_value_1, lval_long(3));

  lval *test_value_2 = lval_qexpr();
  lval_add(test_value_2, lval_long(4));
  lval_add(test_value_2, lval_long(5));
  lval_add(test_value_2, lval_long(6));

  lval *test_value_parent = lval_sexpr();
  lval_add(test_value_parent, test_value_1);
  lval_add(test_value_parent, test_value_2);

  lval *test_calc = builtin_join(test_value_parent);
  TEST_ASSERT_EQUAL_INT(LVAL_QEXPR, test_calc->type);
  TEST_ASSERT_EQUAL_INT(6, test_calc->count);
  TEST_ASSERT_EQUAL_INT(4, test_calc->cell[3]->val_long);

  lval_del(test_calc);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_builtin_op);
  RUN_TEST(test_builtin_op_err);
  RUN_TEST(test_builtin_op_negation);
  RUN_TEST(test_do_eval);
  RUN_TEST(test_eval_err);
  RUN_TEST(test_builtin_eval_op_long);
  RUN_TEST(test_builtin_eval_op_double);
  RUN_TEST(test_builtin_head);
  RUN_TEST(test_builtin_tail);
  RUN_TEST(test_builtin_list);
  RUN_TEST(test_builtin_eval);
  RUN_TEST(test_builtin_join);

  return UNITY_END();
}
