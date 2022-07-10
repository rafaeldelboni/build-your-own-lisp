#include "builtin.h"
#include "lval.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LASSERT(args, cond, fmt, ...)                                          \
  if (!(cond)) {                                                               \
    lval *err = lval_err(fmt, ##__VA_ARGS__);                                  \
    lval_del(args);                                                            \
    return err;                                                                \
  }

#define LASSERT_TYPE(func, args, index, expect)                                \
  LASSERT(args, args->cell[index]->type == expect,                             \
          "Function '%s' passed incorrect type for argument %i. Got %s, "      \
          "Expected %s.",                                                      \
          func, index, lval_ltype_name(args->cell[index]->type),               \
          lval_ltype_name(expect));

#define LASSERT_NUM(func, args, num)                                           \
  LASSERT(args, args->count == num,                                            \
          "Function '%s' passed incorrect number of arguments. Got %i, "       \
          "Expected %i .",                                                     \
          func, args->count, num);

#define LASSERT_NOT_EMPTY(func, args, index)                                   \
  LASSERT(args, args->cell[index]->count != 0,                                 \
          "Function '%s' passed {} for argument %i.", func, index);

lval *builtin_head(lenv *env, lval *arg) {
  LASSERT_NUM("head", arg, 1);
  LASSERT_TYPE("head", arg, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("head", arg, 0);

  lval *value = lval_take(arg, 0);
  while (value->count > 1) {
    lval_del(lval_pop(value, 1));
  }
  return value;
}

lval *builtin_tail(lenv *env, lval *arg) {
  LASSERT_NUM("tail", arg, 1);
  LASSERT_TYPE("tail", arg, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("tail", arg, 0);

  lval *value = lval_take(arg, 0);
  lval_del(lval_pop(value, 0));
  return value;
}

lval *builtin_list(lenv *env, lval *arg) {
  arg->type = LVAL_QEXPR;
  return arg;
}

lval *builtin_eval(lenv *env, lval *arg) {
  LASSERT_NUM("eval", arg, 1);
  LASSERT_TYPE("eval", arg, 0, LVAL_QEXPR);

  lval *value = lval_take(arg, 0);
  value->type = LVAL_SEXPR;
  return builtin_lval_eval(env, value);
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

lval *builtin_join(lenv *env, lval *arg) {
  for (int i = 0; i < arg->count; i++) {
    LASSERT_TYPE("join", arg, i, LVAL_QEXPR);
  }

  lval *value = lval_pop(arg, 0);

  while (arg->count) {
    value = builtin_join_xy(value, lval_pop(arg, 0));
  }

  lval_del(arg);
  return value;
}

lval *builtin_cons(lenv *env, lval *arg) {
  LASSERT_NUM("cons", arg, 2);
  LASSERT(arg,
          arg->cell[0]->type == LVAL_LONG || arg->cell[0]->type == LVAL_DOUBLE,
          "Function 'cons' passed incorrect type for argument 0. Got %s, "
          "Expected %s or %s.",
          lval_ltype_name(arg->cell[0]->type), lval_ltype_name(LVAL_LONG),
          lval_ltype_name(LVAL_DOUBLE));
  LASSERT_TYPE("cons", arg, 1, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("cons", arg, 0);

  lval *input = lval_pop(arg, 0);
  lval *value = lval_qexpr();
  value = lval_add(value, input);

  while (arg->count) {
    value = builtin_join_xy(value, lval_pop(arg, 0));
  }

  lval_del(arg);
  return value;
}

lval *builtin_len(lenv *env, lval *arg) {
  LASSERT_NUM("len", arg, 1);
  LASSERT_TYPE("len", arg, 0, LVAL_QEXPR);

  lval *value = lval_long(arg->cell[0]->count);
  lval_del(arg);
  return value;
}

lval *builtin_init(lenv *env, lval *arg) {
  LASSERT_NUM("init", arg, 1);
  LASSERT_TYPE("init", arg, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("init", arg, 0);

  lval *value = lval_take(arg, 0);
  lval_del(lval_pop(value, (value->count - 1)));
  return value;
}

lval *builtin_def(lenv *env, lval *arg) {
  LASSERT_TYPE("def", arg, 0, LVAL_QEXPR);

  /* Fist argument is symbol list */
  lval *syms = arg->cell[0];

  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(arg, syms->cell[i]->type == LVAL_SYM,
            "Function 'def cannot define non-symbol!");
  }

  /* Check correct number of symbols and values */
  LASSERT(arg, syms->count == arg->count - 1,
          "Fuction 'def' cannot define incorrect \
           number of values to symbols!");

  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(env, syms->cell[i], arg->cell[i + 1]);
  }

  lval_del(arg);
  return lval_sexpr();
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

lval *builtin_op(lenv *env, lval *value, char *op) {
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

lval *builtin_add(lenv *env, lval *value) {
  return builtin_op(env, value, "+");
}

lval *builtin_sub(lenv *env, lval *value) {
  return builtin_op(env, value, "-");
}

lval *builtin_mul(lenv *env, lval *value) {
  return builtin_op(env, value, "*");
}

lval *builtin_div(lenv *env, lval *value) {
  return builtin_op(env, value, "/");
}

lval *builtin_rest(lenv *env, lval *value) {
  return builtin_op(env, value, "%");
}

lval *builtin_pow(lenv *env, lval *value) {
  return builtin_op(env, value, "^");
}

lval *builtin_min(lenv *env, lval *value) {
  return builtin_op(env, value, "min");
}

lval *builtin_max(lenv *env, lval *value) {
  return builtin_op(env, value, "max");
}

void builtin_default_functions(lenv *env) {
  /* List Functions */
  lenv_add_builtin(env, "head", builtin_head);
  lenv_add_builtin(env, "tail", builtin_tail);
  lenv_add_builtin(env, "list", builtin_list);
  lenv_add_builtin(env, "join", builtin_join);
  lenv_add_builtin(env, "cons", builtin_cons);
  lenv_add_builtin(env, "len", builtin_len);
  lenv_add_builtin(env, "init", builtin_init);
  lenv_add_builtin(env, "eval", builtin_eval);

  /* Math Functions */
  lenv_add_builtin(env, "+", builtin_add);
  lenv_add_builtin(env, "-", builtin_sub);
  lenv_add_builtin(env, "*", builtin_mul);
  lenv_add_builtin(env, "/", builtin_div);
  lenv_add_builtin(env, "%", builtin_rest);
  lenv_add_builtin(env, "^", builtin_pow);
  lenv_add_builtin(env, "min", builtin_min);
  lenv_add_builtin(env, "max", builtin_max);

  /* Variable Functions */
  lenv_add_builtin(env, "def", builtin_def);
}

lval *builtin_lval_eval(lenv *env, lval *value) {
  /* Evaluate Symbols */
  if (value->type == LVAL_SYM) {
    lval *env_value = lenv_get(env, value);
    lval_del(value);
    return env_value;
  }

  /* Evaluate Sexpressions */
  if (value->type == LVAL_SEXPR) {
    return builtin_lval_eval_sexpr(env, value);
  }

  /* All other lval types remain the same */
  return value;
}

lval *builtin_lval_eval_sexpr(lenv *env, lval *value) {
  /* Evaluate Children */
  for (int i = 0; i < value->count; i++) {
    value->cell[i] = builtin_lval_eval(env, value->cell[i]);
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
    return builtin_lval_eval(env, lval_take(value, 0));
  }

  /* Ensure First Element is a function */
  lval *function = lval_pop(value, 0);
  if (function->type != LVAL_FUN) {
    lval_del(function);
    lval_del(value);
    return lval_err("First element is not a function");
  }

  /* If so call function to get result */
  lval *result = function->val_fun(env, value);
  lval_del(function);
  return result;
}
