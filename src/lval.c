#include "lval.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval *lval_long(long l) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_LONG;
  value->val_long = l;
  return value;
}

lval *lval_double(double d) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_DOUBLE;
  value->val_double = d;
  return value;
}

lval *lval_sym(char *s) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_SYM;
  value->val_symbol = malloc(strlen(s) + 1);
  strcpy(value->val_symbol, s);
  return value;
}

lval *lval_sexpr(void) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_SEXPR;
  value->count = 0;
  value->cell = NULL;
  return value;
}

lval *lval_err(char *e) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_ERR;
  value->val_err = malloc(strlen(e) + 1);
  strcpy(value->val_err, e);
  return value;
}

lval *lval_add(lval *parent, lval *child) {
  parent->count++;
  parent->cell = realloc(parent->cell, sizeof(lval *) * parent->count);
  parent->cell[parent->count - 1] = child;
  return parent;
}

void lval_del(lval *value) {
  switch (value->type) {
  /* Do nothin special for number type */
  case LVAL_LONG:
    break;
  case LVAL_DOUBLE:
    break;

  /* For Err or Sym free the string data */
  case LVAL_ERR:
    free(value->val_err);
    break;
  case LVAL_SYM:
    free(value->val_symbol);
    break;

  /* If Sexpr then delete all elements inside */
  case LVAL_SEXPR:
    for (int i = 0; i < value->count; i++) {
      lval_del(value->cell[i]);
    }

    /* Also free the memory alocated to contain the pointers */
    free(value->cell);
    break;
  }

  /* Free the memory allocated for the "lval" struct itself */
  free(value);
}

void lval_print(lval *value) {
  switch (value->type) {
  case LVAL_LONG:
    printf("%li", value->val_long);
    break;
  case LVAL_DOUBLE:
    printf("%f", value->val_double);
    break;
  case LVAL_ERR:
    printf("Error: %s", value->val_err);
    break;
  case LVAL_SYM:
    printf("%s", value->val_symbol);
    break;
  case LVAL_SEXPR:
    lval_expr_print(value, '(', ')');
    break;
  }
}

void lval_expr_print(lval *value, char open, char close) {
  putchar(open);
  for (int i = 0; i < value->count; i++) {
    /* Print value contained within */
    lval_print(value->cell[i]);
    /* Dont print trailing space if last element */
    if (i != (value->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_println(lval *value) {
  lval_print(value);
  putchar('\n');
}

// TODO: unit test
lval *lval_pop(lval *value, int i) {
  /* Find the item at "i" */
  lval *poped = value->cell[i];

  /* Shift memory after the intem at "i" over the top */
  memmove(&value->cell[i], &value->cell[i + 1],
          sizeof(lval *) * (value->count - i - 1));

  /* Decreate the coutn of items in the list */
  value->count--;

  /* Realocate the memore used */
  value->cell = realloc(value->cell, sizeof(lval *) * value->count);

  return poped;
}

// TODO: unit test
lval *lval_take(lval *value, int i) {
  lval *taken = lval_pop(value, i);
  lval_del(value);
  return taken;
}

lval *lval_eval_op(lval *x, char *op, lval *y) {
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

// TODO: unit test
lval *lval_builtin_op(lval *value, char *op) {
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

    x = lval_eval_op(x, op, y);

    lval_del(y);
  }

  lval_del(value);
  return x;
}

// TODO: unit test
lval *lval_eval(lval *value) {
  /* Evaluate Sexpressions */
  if (value->type == LVAL_SEXPR) {
    return lval_eval_sexpr(value);
  }
  /* All other lval types remain the same */
  return value;
}

// TODO: unit test
lval *lval_eval_sexpr(lval *value) {
  /* Evaluate Children */
  for (int i = 0; i < value->count; i++) {
    value->cell[i] = lval_eval(value->cell[i]);
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
  lval *result = lval_builtin_op(value, symbol_element->val_symbol);
  lval_del(symbol_element);
  return result;
}
