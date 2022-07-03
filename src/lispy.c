#include "lispy.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

lval eval_op(lval x, char *op, lval y) {
  /* If either value is an error return it */
  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }

  /* Other wise do maths on the number values */
  if (strcmp(op, "+") == 0) {
    return lval_num(x.num + y.num);
  };
  if (strcmp(op, "-") == 0) {
    return lval_num(x.num - y.num);
  };
  if (strcmp(op, "*") == 0) {
    return lval_num(x.num * y.num);
  };
  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error */
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  };
  if (strcmp(op, "%") == 0) {
    return lval_num(x.num % y.num);
  };
  if (strcmp(op, "^") == 0) {
    long power = pow(x.num, y.num);
    return lval_num(power);
  };
  if (strcmp(op, "min") == 0) {
    long min = x.num < y.num ? x.num : y.num;
    return lval_num(min);
  };
  if (strcmp(op, "max") == 0) {
    long max = x.num > y.num ? x.num : y.num;
    return lval_num(max);
  };

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *tree) {
  /* If tagged as number return it directly. */
  if (strstr(tree->tag, "number")) {
    /* Checks if there is some error in conversion */
    errno = 0;
    long x = strtol(tree->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* The operator is aways second child. */
  char *op = tree->children[1]->contents;

  /* We store the third child in `x` */
  lval x = eval(tree->children[2]);

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (i < tree->children_num && strstr(tree->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(tree->children[i]));
    i++;
  }

  return x;
}
