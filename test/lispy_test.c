#include "lispy.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_eval_long(void) {
  mpc_ast_t *trunk;
  trunk = mpc_ast_build(
      4, ">", mpc_ast_new("regex", ""), mpc_ast_new("operator", "+"),
      mpc_ast_new("expr|number", "3"), mpc_ast_new("expr|number", "2"));

  TEST_ASSERT_EQUAL_INT(5, eval(trunk).val_long);
  TEST_ASSERT_EQUAL_INT(LVAL_LONG, eval(trunk).type);

  mpc_ast_delete(trunk);
}

void test_eval_double(void) {
  mpc_ast_t *trunk;
  trunk = mpc_ast_build(
      4, ">", mpc_ast_new("regex", ""), mpc_ast_new("operator", "+"),
      mpc_ast_new("expr|number", "3.6"), mpc_ast_new("expr|number", "2.4"));

  TEST_ASSERT_EQUAL_FLOAT(6, eval(trunk).val_double);
  TEST_ASSERT_EQUAL_INT(LVAL_DOUBLE, eval(trunk).type);

  mpc_ast_delete(trunk);
}

void test_eval_op_long(void) {
  TEST_ASSERT_EQUAL_INT(2, eval_op(lval_long(1), "+", lval_long(1)).val_long);
  TEST_ASSERT_EQUAL_INT(0, eval_op(lval_long(1), "-", lval_long(1)).val_long);
  TEST_ASSERT_EQUAL_INT(4, eval_op(lval_long(2), "*", lval_long(2)).val_long);
  TEST_ASSERT_EQUAL_INT(2, eval_op(lval_long(4), "/", lval_long(2)).val_long);
  TEST_ASSERT_EQUAL_INT(4, eval_op(lval_long(10), "%", lval_long(6)).val_long);
  TEST_ASSERT_EQUAL_INT(16, eval_op(lval_long(4), "^", lval_long(2)).val_long);
  TEST_ASSERT_EQUAL_INT(1, eval_op(lval_long(1), "min", lval_long(5)).val_long);
  TEST_ASSERT_EQUAL_INT(1, eval_op(lval_long(1), "min", lval_long(1)).val_long);
  TEST_ASSERT_EQUAL_INT(5, eval_op(lval_long(1), "max", lval_long(5)).val_long);
  TEST_ASSERT_EQUAL_INT(5, eval_op(lval_long(5), "max", lval_long(5)).val_long);
}

void test_eval_op_double(void) {
  TEST_ASSERT_EQUAL_FLOAT(2.2, eval_op(lval_double(1.1), "+", lval_double(1.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(0.0, eval_op(lval_double(1.1), "-", lval_double(1.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(4.84, eval_op(lval_double(2.2), "*", lval_double(2.2)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(1.76, eval_op(lval_double(4.4), "/", lval_double(2.5)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(4.0, eval_op(lval_double(10.0), "%", lval_double(6.0)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(70.9425383673, eval_op(lval_double(5.5), "^", lval_double(2.5)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(1.1, eval_op(lval_double(1.1), "min", lval_double(5.5)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(1.0, eval_op(lval_double(1.0), "min", lval_double(1.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(1.1, eval_op(lval_double(1.1), "min", lval_double(1.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(5.1, eval_op(lval_double(1.0), "max", lval_double(5.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(5.1, eval_op(lval_double(5.0), "max", lval_double(5.1)).val_double);
  TEST_ASSERT_EQUAL_FLOAT(5.1, eval_op(lval_double(5.1), "max", lval_double(5.1)).val_double);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_eval_op_long);
  RUN_TEST(test_eval_op_double);
  RUN_TEST(test_eval_long);
  RUN_TEST(test_eval_double);

  return UNITY_END();
}
