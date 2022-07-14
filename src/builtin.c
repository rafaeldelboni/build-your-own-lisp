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

#define LASSERT_NUMERIC(func, args, index)                                     \
  LASSERT(args,                                                                \
          args->cell[index]->type == LVAL_LONG ||                              \
              args->cell[index]->type == LVAL_DOUBLE,                          \
          "Function '%s' passed incorrect type for argument %i. Got %s, "      \
          "Expected numeric value (Integer or Float).",                        \
          func, index, lval_ltype_name(args->cell[index]->type));

#define LASSERT_NUM(func, args, num)                                           \
  LASSERT(args, args->count == num,                                            \
          "Function '%s' passed incorrect number of arguments. Got %i, "       \
          "Expected %i .",                                                     \
          func, args->count, num);

#define LASSERT_NOT_EMPTY(func, args, index)                                   \
  LASSERT(args, args->cell[index]->count != 0,                                 \
          "Function '%s' passed {} for argument %i.", func, index);

// TODO: unit test
lval *builtin_ord(lenv *env, lval *arg, char *op) {
  LASSERT_NUM(op, arg, 2);
  LASSERT_NUMERIC(op, arg, 0);
  LASSERT_NUMERIC(op, arg, 1);

  lval *x = lval_pop(arg, 0);
  lval *y = lval_pop(arg, 0);
  lval_del(arg);

  int double_result = x->type == LVAL_DOUBLE || y->type == LVAL_DOUBLE;
  long x_double = x->type == LVAL_LONG ? (double)x->val_long : x->val_double;
  long y_double = y->type == LVAL_LONG ? (double)y->val_long : y->val_double;

  double result;

  if (strcmp(op, ">") == 0) {
    result = (x_double > y_double);
  }

  if (strcmp(op, "<") == 0) {
    result = (x_double < y_double);
  }

  if (strcmp(op, ">=") == 0) {
    result = (x_double >= y_double);
  }

  if (strcmp(op, "<=") == 0) {
    result = (x_double <= y_double);
  }

  lval_del(x);
  lval_del(y);

  return double_result ? lval_double(result) : lval_long((long)result);
}

// TODO: unit test
lval *builtin_gt(lenv *env, lval *arg) { return builtin_ord(env, arg, ">"); }

// TODO: unit test
lval *builtin_lt(lenv *env, lval *arg) { return builtin_ord(env, arg, "<"); }

// TODO: unit test
lval *builtin_ge(lenv *env, lval *arg) { return builtin_ord(env, arg, ">="); }

// TODO: unit test
lval *builtin_le(lenv *env, lval *arg) { return builtin_ord(env, arg, "<="); }

// TODO: unit test
lval *builtin_cmp(lenv *env, lval *arg, char *op) {
  LASSERT_NUM(op, arg, 2);

  lval *x = lval_pop(arg, 0);
  lval *y = lval_pop(arg, 0);
  lval_del(arg);

  int result;

  if (strcmp(op, "==") == 0) {
    result = lval_eq(x, y);
  }

  if (strcmp(op, "!=") == 0) {
    result = !lval_eq(x, y);
  }

  lval_del(x);
  lval_del(y);

  return lval_long(result);
}

// TODO: unit test
lval *builtin_eq(lenv *env, lval *arg) { return builtin_cmp(env, arg, "=="); }

// TODO: unit test
lval *builtin_ne(lenv *env, lval *arg) { return builtin_cmp(env, arg, "!="); }

// TODO: unit test
lval *builtin_if(lenv *env, lval *arg) {
  LASSERT_NUM("if", arg, 3);
  LASSERT_TYPE("if", arg, 0, LVAL_LONG);
  LASSERT_TYPE("if", arg, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", arg, 2, LVAL_QEXPR);

  /* Mark Both Expressions as evaluable */
  lval *result;
  arg->cell[1]->type = LVAL_SEXPR;
  arg->cell[2]->type = LVAL_SEXPR;

  if (arg->cell[0]->val_long) {
    /* If condition is true evaluate first expression */
    result = builtin_lval_eval(env, lval_pop(arg, 1));
  } else {
    /* Otherwise evaluate second expression */
    result = builtin_lval_eval(env, lval_pop(arg, 2));
  }

  /* Delete argument list and return */
  lval_del(arg);
  return result;
}

// TODO: unit test
lval *builtin_or(lenv *env, lval *arg) {
  for (int i = 0; i < arg->count; i++) {
    LASSERT_TYPE("||", arg, i, LVAL_LONG);
  }

  int result = lval_pop(arg, 0)->val_long;
  for (int i = 0; i < arg->count; i++) {
    result = result || arg->cell[i]->val_long;
  }

  lval_del(arg);

  return lval_long(result);
}

// TODO: unit test
lval *builtin_and(lenv *env, lval *arg) {
  for (int i = 0; i < arg->count; i++) {
    LASSERT_TYPE("&&", arg, i, LVAL_LONG);
  }

  int result = lval_pop(arg, 0)->val_long;
  for (int i = 0; i < arg->count; i++) {
    result = result && arg->cell[i]->val_long;
  }

  lval_del(arg);

  return lval_long(result);
}

// TODO: unit test
lval *builtin_not(lenv *env, lval *arg) {
  LASSERT_NUM("!", arg, 1);
  LASSERT_TYPE("!", arg, 0, LVAL_LONG);

  int result = !lval_pop(arg, 0)->val_long;

  lval_del(arg);

  return lval_long(result);
}

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
  LASSERT_NUMERIC("const", arg, 0);
  LASSERT_TYPE("cons", arg, 1, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("cons", arg, 1);

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

lval *builtin_var(lenv *env, lval *arg, char *func) {
  LASSERT_TYPE(func, arg, 0, LVAL_QEXPR);

  /* Fist argument is symbol list */
  lval *syms = arg->cell[0];

  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(arg, syms->cell[i]->type == LVAL_SYM,
            "Function '%s' cannot define non-symbol!"
            "Got %s, Expected %s.",
            func, lval_ltype_name(syms->cell[i]->type),
            lval_ltype_name(LVAL_SYM));
  }

  /* Check correct number of symbols and values */
  LASSERT(arg, syms->count == arg->count - 1,
          "Fuction '%s' passed too many arguments for symbols. "
          "Got %i, Expected %i.",
          func, syms->count, arg->count - 1);

  for (int i = 0; i < syms->count; i++) {
    /* If 'def' define in globally. If 'put' define in locally */
    if (strcmp(func, "def") == 0) {
      lenv_def(env, syms->cell[i], arg->cell[i + 1]);
    }
    if (strcmp(func, "=") == 0) {
      lenv_put(env, syms->cell[i], arg->cell[i + 1]);
    }
  }

  lval_del(arg);
  return lval_sexpr();
}

lval *builtin_def(lenv *env, lval *arg) { return builtin_var(env, arg, "def"); }

lval *builtin_put(lenv *env, lval *arg) { return builtin_var(env, arg, "="); }

lval *builtin_lambda(lenv *env, lval *arg) {
  /* Check Two arguments, each of which are Q-Expressions */
  LASSERT_NUM("\\", arg, 2);
  LASSERT_TYPE("\\", arg, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", arg, 1, LVAL_QEXPR);

  /* Check first Q-Expression contains only Symbols */
  for (int i = 0; i < arg->cell[0]->count; i++) {
    LASSERT(arg, (arg->cell[0]->cell[i]->type == LVAL_SYM),
            "Cannot define non-symbol. Got %s, Expected %s.",
            lval_ltype_name(arg->cell[0]->cell[i]->type),
            lval_ltype_name(LVAL_SYM));
  }

  /* Pop first two arguments and pass them to lval_lambda */
  lval *formals = lval_pop(arg, 0);
  lval *body = lval_pop(arg, 0);
  lval_del(arg);

  return lval_lambda(formals, body);
}

lval *builtin_print(lenv *env, lval *arg) {
  /* Print each argument followed by a space */
  for (int i = 0; i < arg->count; i++) {
    lval_print(arg->cell[i]);
    putchar(' ');
  }
  /* Print a newline and delete arguments*/
  putchar('\n');
  lval_del(arg);

  return lval_sexpr();
}

lval *builtin_error(lenv *env, lval *arg) {
  LASSERT_NUM("error", arg, 1);
  LASSERT_TYPE("error", arg, 0, LVAL_STR);

  /* Construct Error from first argument */
  lval *err = lval_err(arg->cell[0]->val_string);

  /* Delete arguments and return */
  lval_del(arg);
  return err;
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
  lenv_add_builtin(env, "\\", builtin_lambda);

  /* Comparison Functions */
  lenv_add_builtin(env, "if", builtin_if);
  lenv_add_builtin(env, "||", builtin_or);
  lenv_add_builtin(env, "&&", builtin_and);
  lenv_add_builtin(env, "!", builtin_not);
  lenv_add_builtin(env, "==", builtin_eq);
  lenv_add_builtin(env, "!=", builtin_ne);
  lenv_add_builtin(env, ">", builtin_gt);
  lenv_add_builtin(env, "<", builtin_lt);
  lenv_add_builtin(env, ">=", builtin_ge);
  lenv_add_builtin(env, "<=", builtin_le);

  /* String Functions */
  // TODO: make a way to pass the parser not using a global var
  /*lenv_add_builtin(env, "load", builtin_load);*/
  lenv_add_builtin(env, "error", builtin_error);
  lenv_add_builtin(env, "print", builtin_print);
}

lval *lval_call(lenv *env, lval *func, lval *args) {
  /* If Builtin then simply apply that */
  if (func->val_fun.builtin) {
    return func->val_fun.builtin(env, args);
  }

  /* Record Argument Counts */
  int given = args->count;
  int total = func->val_fun.formals->count;

  /* While arguments still remain to be processed */
  while (args->count) {
    /* If we've ran out of formal arguments ot bind */
    if (func->val_fun.formals->count == 0) {
      lval_del(args);
      return lval_err("Function passed too many arguments. "
                      "Got %i, Expected %i.",
                      given, total);
    }
    /* Pop the first symbol from the formals */
    lval *symbol = lval_pop(func->val_fun.formals, 0);

    /* Special Case to deal with '&' */
    if (strcmp(symbol->val_symbol, "&") == 0) {
      /* Ensure '&' is followed by another symbol */
      if (func->val_fun.formals->count != 1) {
        lval_del(args);
        return lval_err("Function format invalid. "
                        "Symbol '&' not followed by single symbol.");
      }
      /* Next formal should be bound to remaining arguments */
      lval *new_symbol = lval_pop(func->val_fun.formals, 0);
      lenv_put(func->val_fun.env, new_symbol, builtin_list(env, args));
      lval_del(symbol);
      lval_del(new_symbol);
      break;
    }

    /* Pop the next argument from the list */
    lval *value = lval_pop(args, 0);

    /* Bind a copy into the function's environment */
    lenv_put(func->val_fun.env, symbol, value);

    /* Delete symbol and value */
    lval_del(symbol);
    lval_del(value);
  }

  /* Argument list is now bound so can be cleaned up */
  lval_del(args);

  /* If '&' remains in formal list bind to empty list */
  if (func->val_fun.formals->count > 0 &&
      strcmp(func->val_fun.formals->cell[0]->val_symbol, "&") == 0) {
    /* Check to ensure that & is not passed invalidly. */
    if (func->val_fun.formals->count != 2) {
      return lval_err("Function format invalid. "
                      "Symbol '&' not followed by single symbol.");
    }

    /* Pop and delete '&' symbol */
    lval_del(lval_pop(func->val_fun.formals, 0));

    /* Pop next symbol and create empty list */
    lval *symbol = lval_pop(func->val_fun.formals, 0);
    lval *value = lval_qexpr();

    /* Bind to environment and delete */
    lenv_put(func->val_fun.env, symbol, value);
    lval_del(symbol);
    lval_del(value);
  }

  /* If all formals have been bound evaluate */
  if (func->val_fun.formals->count == 0) {
    /* Set environment parent to evaluation environment */
    func->val_fun.env->parent = env;
    /* Evaluate and return */
    return builtin_eval(func->val_fun.env,
                        lval_add(lval_sexpr(), lval_copy(func->val_fun.body)));
  } else {
    /* Otherwise return partially evaluated function */
    return lval_copy(func);
  }
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
    lval *err =
        lval_err("S-Expression starts with incorrect type. "
                 "Got %s, Expected %s.",
                 lval_ltype_name(function->type), lval_ltype_name(LVAL_FUN));
    lval_del(function);
    lval_del(value);
    return err;
  }

  /* If so call function to get result */
  lval *result = lval_call(env, function, value);
  lval_del(function);
  return result;
}
