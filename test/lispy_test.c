#include "lispy.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

/*void test_eval_long(void) {*/
  /*mpc_ast_t *trunk;*/
  /*trunk = mpc_ast_build(*/
      /*4, ">", mpc_ast_new("regex", ""), mpc_ast_new("symbol", "+"),*/
      /*mpc_ast_new("expr|number", "3"), mpc_ast_new("expr|number", "2"));*/

  /*TEST_ASSERT_EQUAL_INT(5, lispy_eval(trunk)->val_long);*/
  /*TEST_ASSERT_EQUAL_INT(LVAL_LONG, lispy_eval(trunk)->type);*/

  /*mpc_ast_delete(trunk);*/
/*}*/

/*void test_eval_double(void) {*/
  /*mpc_ast_t *trunk;*/
  /*trunk = mpc_ast_build(*/
      /*4, ">", mpc_ast_new("regex", ""), mpc_ast_new("symbol", "+"),*/
      /*mpc_ast_new("expr|float", "3.6"), mpc_ast_new("expr|float", "2.4"));*/

  /*TEST_ASSERT_EQUAL_FLOAT(6, lispy_eval(trunk)->val_double);*/
  /*TEST_ASSERT_EQUAL_INT(LVAL_DOUBLE, lispy_eval(trunk)->type);*/

  /*mpc_ast_delete(trunk);*/
/*}*/

int main(void) {
  UNITY_BEGIN();

  /*RUN_TEST(test_eval_long);*/
  /*RUN_TEST(test_eval_double);*/

  return UNITY_END();
}
