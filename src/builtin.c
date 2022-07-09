#include "builtin.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LASSERT(args, cond, err)                                               \
  if (!(cond)) {                                                               \
    lval_del(args);                                                            \
    return lval_err(err);                                                      \
  }

lval *builtin_head(lval *arg) {
  LASSERT(arg, arg->count == 1, "Function 'head' passed too many arguments!");
  LASSERT(arg, arg->cell[0]->type == LVAL_QEXPR,
          "Function 'head' passed incorrect type!");
  LASSERT(arg, arg->cell[0]->count != 0, "Function 'head' passed {}!");

  lval *value = lval_take(arg, 0);
  while (value->count > 1) {
    lval_del(lval_pop(value, 1));
  }
  return value;
}

lval *builtin_tail(lval *arg) {
  LASSERT(arg, arg->count == 1, "Function 'tail' passed too many arguments!");
  LASSERT(arg, arg->cell[0]->type == LVAL_QEXPR,
          "Function 'tail' passed incorrect type!");
  LASSERT(arg, arg->cell[0]->count != 0, "Function 'tail' passed {}!");

  lval *value = lval_take(arg, 0);
  lval_del(lval_pop(value, 0));
  return value;
}

lval *builtin_list(lval *arg) {
  arg->type = LVAL_QEXPR;
  return arg;
}

lval *builtin_eval(lval *arg) {
  LASSERT(arg, arg->count == 1, "Function 'eval' passed too many arguments!");
  LASSERT(arg, arg->cell[0]->type == LVAL_QEXPR,
          "Function 'eval' passed incorrect type!");

  printf("val-symbol:          %s\n", arg->cell[0]->val_symbol);
  lval *value = lval_take(arg, 0);
  value->type = LVAL_SEXPR;
  return builtin_lval_eval(value);
}

lval *builtin_join_xy(lval *x, lval *y) {
  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }
  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}

lval *builtin_join(lval *arg) {
  for (int i = 0; i < arg->count; i++) {
    LASSERT(arg, arg->cell[i]->type == LVAL_QEXPR,
            "Function 'join' passed incorrect type.");
  }

  lval *value = lval_pop(arg, 0);

  while (arg->count) {
    value = builtin_join_xy(value, lval_pop(arg, 0));
  }

  lval_del(arg);
  return value;
}

lval *builtin_eval_op(lval *x, char *op, lval *y) {
  int double_result = x->type == LVAL_DOUBLE || y->type == LVAL_DOUBLE;

  long x_long, y_long;
  double x_double, y_double;

  if (double_result) {
    x_double = x->type == LVAL_LONG ? (double)x->val_long : x->val_double;
    y_double = y->type == LVAL_LONG ? (double)y->val_long : y->val_double;
  } else {
    x_long = x->val_long;
    y_long = y->val_long;
  }

  /* If either value is an error return it */
  if (x->type == LVAL_ERR) {
    return x;
  }
  if (y->type == LVAL_ERR) {
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
      return y_double == 0 ? lval_err("Division By Zero!")
                           : lval_double(x_double / y_double);
    } else {
      return y_long == 0 ? lval_err("Division By Zero!")
                         : lval_long(x_long / y_long);
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

  return lval_err("Invalid operand!");
}

lval *builtin_op(lval *value, char *op) {
  /* Ensure all argument are number */
  for (int i = 0; i < value->count; i++) {
    if (value->cell[i]->type != LVAL_LONG &&
        value->cell[i]->type != LVAL_DOUBLE) {
      lval_del(value);
      return lval_err("Cannot operate on non-number!");
    }
  }

  /* Pop the first element */
  lval *x = lval_pop(value, 0);

  /* If no arguments and sub then perform unary negation */
  if ((strcmp(op, "-") == 0) && value->count == 0) {
    if (x->type == LVAL_LONG) {
      x->val_long = -x->val_long;
    } else {
      x->val_double = -x->val_double;
    }
  }

  /* While there are still elements remaining */
  while (value->count > 0) {
    /* Pop the next element */
    lval *y = lval_pop(value, 0);

    x = builtin_eval_op(x, op, y);

    lval_del(y);
  }

  lval_del(value);
  return x;
}

lval *builtin_functions(lval *arg, char *func) {
  if ((strcmp("list", func) == 0)) {
    return builtin_list(arg);
  }
  if ((strcmp("head", func) == 0)) {
    return builtin_head(arg);
  }
  if ((strcmp("tail", func) == 0)) {
    return builtin_tail(arg);
  }
  if ((strcmp("join", func) == 0)) {
    return builtin_join(arg);
  }
  if ((strcmp("eval", func) == 0)) {
    return builtin_eval(arg);
  }
  if ((strcmp("min", func) == 0) || (strcmp("max", func) == 0) ||
      (strstr("+-*/%^", func))) {
    return builtin_op(arg, func);
  }
  lval_del(arg);
  return lval_err("Unknown Function!");
}

lval *builtin_lval_eval(lval *value) {
  /* Evaluate Sexpressions */
  if (value->type == LVAL_SEXPR) {
    return builtin_lval_eval_sexpr(value);
  }
  /* All other lval types remain the same */
  return value;
}

lval *builtin_lval_eval_sexpr(lval *value) {
  /* Evaluate Children */
  for (int i = 0; i < value->count; i++) {
    value->cell[i] = builtin_lval_eval(value->cell[i]);
  }

  /* Error Checking */
  for (int i = 0; i < value->count; i++) {
    if (value->cell[i]->type == LVAL_ERR) {
      return lval_take(value, i);
    }
  }

  /* Empty Expression */
  if (value->count == 0) {
    return value;
  }

  /* Single Expression */
  if (value->count == 1) {
    return lval_take(value, 0);
  }

  /* Ensure First Element is Symbol */
  lval *symbol_element = lval_pop(value, 0);
  if (symbol_element->type != LVAL_SYM) {
    lval_del(symbol_element);
    lval_del(value);
    return lval_err("S-expression Does not start with symbol!");
  }

  /* Call builtin with operator */
  lval *result = builtin_functions(value, symbol_element->val_symbol);
  lval_del(symbol_element);
  return result;
}
