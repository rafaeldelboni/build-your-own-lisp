#include "lispy.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_eval(void) {
  mpc_ast_t *trunk;
  trunk = mpc_ast_build(4, ">",
                        mpc_ast_new("regex", ""),
                        mpc_ast_new("operator", "+"),
                        mpc_ast_new("expr|number", "3"),
                        mpc_ast_new("expr|number", "2"));

  TEST_ASSERT_EQUAL_INT(5, eval(trunk));

  mpc_ast_delete(trunk);
}

void test_eval_op(void) {
  TEST_ASSERT_EQUAL_INT(2, eval_op(1, "+", 1));
  TEST_ASSERT_EQUAL_INT(0, eval_op(1, "-", 1));
  TEST_ASSERT_EQUAL_INT(4, eval_op(2, "*", 2));
  TEST_ASSERT_EQUAL_INT(2, eval_op(4, "/", 2));
  TEST_ASSERT_EQUAL_INT(4, eval_op(10, "%", 6));
  TEST_ASSERT_EQUAL_INT(16, eval_op(4, "^", 2));
  TEST_ASSERT_EQUAL_INT(1, eval_op(1, "min", 5));
  TEST_ASSERT_EQUAL_INT(1, eval_op(1, "min", 1));
  TEST_ASSERT_EQUAL_INT(5, eval_op(1, "max", 5));
  TEST_ASSERT_EQUAL_INT(5, eval_op(5, "max", 5));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_eval_op);
  RUN_TEST(test_eval);

  return UNITY_END();
}
