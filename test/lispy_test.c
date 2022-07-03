#include "lispy.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_eval(void) {
  mpc_ast_t *trunk;
  trunk = mpc_ast_build(
      4, ">", mpc_ast_new("regex", ""), mpc_ast_new("operator", "+"),
      mpc_ast_new("expr|number", "3"), mpc_ast_new("expr|number", "2"));

  TEST_ASSERT_EQUAL_INT(5, eval(trunk).num);
  TEST_ASSERT_EQUAL_INT(LVAL_NUM, eval(trunk).type);

  mpc_ast_delete(trunk);
}

void test_eval_op(void) {
  TEST_ASSERT_EQUAL_INT(2, eval_op(lval_num(1), "+", lval_num(1)).num);
  TEST_ASSERT_EQUAL_INT(0, eval_op(lval_num(1), "-", lval_num(1)).num);
  TEST_ASSERT_EQUAL_INT(4, eval_op(lval_num(2), "*", lval_num(2)).num);
  TEST_ASSERT_EQUAL_INT(2, eval_op(lval_num(4), "/", lval_num(2)).num);
  TEST_ASSERT_EQUAL_INT(4, eval_op(lval_num(10), "%", lval_num(6)).num);
  TEST_ASSERT_EQUAL_INT(16, eval_op(lval_num(4), "^", lval_num(2)).num);
  TEST_ASSERT_EQUAL_INT(1, eval_op(lval_num(1), "min", lval_num(5)).num);
  TEST_ASSERT_EQUAL_INT(1, eval_op(lval_num(1), "min", lval_num(1)).num);
  TEST_ASSERT_EQUAL_INT(5, eval_op(lval_num(1), "max", lval_num(5)).num);
  TEST_ASSERT_EQUAL_INT(5, eval_op(lval_num(5), "max", lval_num(5)).num);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_eval_op);
  RUN_TEST(test_eval);

  return UNITY_END();
}
