#include "lispy.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval eval_op(lval x, char *op, lval y) {
  int double_result = x.type == LVAL_DOUBLE || y.type == LVAL_DOUBLE;

  long x_long, y_long;
  double x_double, y_double;

  if (double_result) {
    x_double = x.type == LVAL_LONG ? (double)x.val_long : x.val_double;
    y_double = y.type == LVAL_LONG ? (double)y.val_long : y.val_double;
  } else {
    x_long = x.val_long;
    y_long = y.val_long;
  }

  /* If either value is an error return it */
  if (x.type == LVAL_ERR) {
    return x;
  }
  if (y.type == LVAL_ERR) {
    return y;
  }

  /* Other wise do maths on the number values */
  if (strcmp(op, "+") == 0) {
    return double_result ? lval_double(x_double + y_double)
                         : lval_long(x_long + y_long);
  };
  if (strcmp(op, "-") == 0) {
    return double_result ? lval_double(x_double - y_double)
                         : lval_long(x_long - y_long);
  };
  if (strcmp(op, "*") == 0) {
    return double_result ? lval_double(x_double * y_double)
                         : lval_long(x_long * y_long);
  };
  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error */
    if (double_result) {
      return y_double == 0 ? lval_err(LERR_DIV_ZERO)
                           : lval_double(x_double / y_double);
    } else {
      return y_long == 0 ? lval_err(LERR_DIV_ZERO) : lval_long(x_long / y_long);
    }
  };
  if (strcmp(op, "%") == 0) {
    return double_result ? lval_double((long)x_double % (long)y_double)
                         : lval_long(x_long % y_long);
  };
  if (strcmp(op, "^") == 0) {
    return double_result ? lval_double(pow(x_double, y_double))
                         : lval_long(pow(x_long, y_long));
  };
  if (strcmp(op, "min") == 0) {
    return double_result
               ? lval_double(x_double < y_double ? x_double : y_double)
               : lval_long(x_long < y_long ? x_long : y_long);
  };
  if (strcmp(op, "max") == 0) {
    return double_result
               ? lval_double(x_double > y_double ? x_double : y_double)
               : lval_long(x_long > y_long ? x_long : y_long);
  };

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *tree) {
  /* If tagged as number return it directly. */

  if (strstr(tree->tag, "number")) {
    errno = 0;
    long x = strtol(tree->contents, NULL, 10);
    return errno != ERANGE ? lval_long(x) : lval_err(LERR_BAD_NUM);
  }

  if (strstr(tree->tag, "float")) {
    errno = 0;
    double x = strtod(tree->contents, NULL);
    return errno != ERANGE ? lval_double(x) : lval_err(LERR_BAD_NUM);
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
