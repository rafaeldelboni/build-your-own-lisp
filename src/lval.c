#include "lval.h"
#include "lenv.h"
#include "mpc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *lval_ltype_name(int type) {
  switch (type) {
  case LVAL_ERR:
    return "Error";
  case LVAL_LONG:
    return "Integer";
  case LVAL_DOUBLE:
    return "Float";
  case LVAL_SYM:
    return "Symbol";
  case LVAL_STR:
    return "String";
  case LVAL_FUN:
    return "Function";
  case LVAL_SEXPR:
    return "S-Expression";
  case LVAL_QEXPR:
    return "Q-Expression";
  default:
    return "Unknown";
  };
}

// TODO: unit tests
int lval_eq(lval *x, lval *y) {
  /* Different Types are always unequal */
  if (x->type != y->type) {
    return 0;
  }

  /* Compare Based upon type */
  switch (x->type) {
  /* Compare Numeric Value */
  case LVAL_LONG:
    return (x->val_long == y->val_long);
  case LVAL_DOUBLE:
    return (x->val_long == y->val_long);

  /* Compare String Value */
  case LVAL_ERR:
    return (strcmp(x->val_err, y->val_err) == 0);
  case LVAL_SYM:
    return (strcmp(x->val_symbol, y->val_symbol) == 0);
  case LVAL_STR:
    return (strcmp(x->val_string, y->val_string) == 0);

  /* If builtin compare, otherwise compare formals and body */
  case LVAL_FUN:
    if (x->val_fun.builtin || y->val_fun.builtin) {
      return x->val_fun.builtin == y->val_fun.builtin;
    } else {
      return lval_eq(x->val_fun.formals, y->val_fun.formals) &&
             lval_eq(x->val_fun.body, y->val_fun.body);
    }

  /* If list compare every individual element */
  case LVAL_QEXPR:
  case LVAL_SEXPR:
    if (x->count != y->count) {
      return 0;
    }
    for (int i = 0; i < x->count; i++) {
      /* If any element not equal then whole list not equal */
      if (!lval_eq(x->cell[i], y->cell[i])) {
        return 0;
      }
    }
    /* Otherwise lists must be equal */
    return 1;
    break;
  }
  return 0;
}

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

// TODO: unit test
lval *lval_str(char *s) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_STR;
  value->val_string = malloc(strlen(s) + 1);
  strcpy(value->val_string, s);
  return value;
}

// TODO: unit test
lval *lval_fun(lbuiltin func) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_FUN;
  value->val_fun.builtin = func;
  return value;
}

// TODO: unit test
lval *lval_lambda(lval *formals, lval *body) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_FUN;

  /* Set Builtin to Null */
  value->val_fun.builtin = NULL;

  /* Build new environment */
  value->val_fun.env = lenv_new();

  /* Set Formals and Body */
  value->val_fun.formals = formals;
  value->val_fun.body = body;

  return value;
}

lval *lval_sexpr(void) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_SEXPR;
  value->count = 0;
  value->cell = NULL;
  return value;
}

lval *lval_qexpr(void) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_QEXPR;
  value->count = 0;
  value->cell = NULL;
  return value;
}

lval *lval_err(char *fmt, ...) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_ERR;

  /* Create a variable argument list and initialize it */
  va_list var_args;
  va_start(var_args, fmt);

  /* Allocate 512 bytes of space */
  value->val_err = malloc(512);

  /* printf the error string with a maximum of 511 characters) */
  vsnprintf(value->val_err, 511, fmt, var_args);

  /* Reallocate to number of bytes actually used */
  value->val_err = realloc(value->val_err, strlen(value->val_err) + 1);

  /* Cleanup our variable argument list */
  va_end(var_args);

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
  /* Do nothin special for number, float and function type */
  case LVAL_LONG:
    break;
  case LVAL_DOUBLE:
    break;
  case LVAL_FUN:
    if (!value->val_fun.builtin) {
      lenv_del(value->val_fun.env);
      lval_del(value->val_fun.formals);
      lval_del(value->val_fun.body);
    }
    break;

  /* For Err or Sym free the string data */
  case LVAL_ERR:
    free(value->val_err);
    break;
  case LVAL_SYM:
    free(value->val_symbol);
    break;
  case LVAL_STR:
    free(value->val_string);
    break;

  /* If Sexpr or Qexpr then delete all elements inside */
  case LVAL_SEXPR:
  case LVAL_QEXPR:
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

lval *lval_copy(lval *value) {
  lval *copy = malloc(sizeof(lval));
  copy->type = value->type;

  switch (value->type) {

  /* Copy Functions, Numbers and Floats Directly */
  case LVAL_FUN:
    if (value->val_fun.builtin) {
      copy->val_fun = value->val_fun;
    } else {
      copy->val_fun.builtin = NULL;
      copy->val_fun.env = lenv_copy(value->val_fun.env);
      copy->val_fun.formals = lval_copy(value->val_fun.formals);
      copy->val_fun.body = lval_copy(value->val_fun.body);
    }
    break;
  case LVAL_LONG:
    copy->val_long = value->val_long;
    break;
  case LVAL_DOUBLE:
    copy->val_double = value->val_double;
    break;

  /* Copy String using malloc and strcpy */
  case LVAL_ERR:
    copy->val_err = malloc(strlen(value->val_err) + 1);
    strcpy(copy->val_err, value->val_err);
    break;

  case LVAL_SYM:
    copy->val_symbol = malloc(strlen(value->val_symbol) + 1);
    strcpy(copy->val_symbol, value->val_symbol);
    break;

  case LVAL_STR:
    copy->val_string = malloc(strlen(value->val_string) + 1);
    strcpy(copy->val_string, value->val_string);
    break;

  /* Copy Lists by copying each sub-expression */
  case LVAL_SEXPR:
  case LVAL_QEXPR:
    copy->count = value->count;
    copy->cell = malloc(sizeof(lval *) * copy->count);
    for (int i = 0; i < copy->count; i++) {
      copy->cell[i] = lval_copy(value->cell[i]);
    }
    break;
  }

  return copy;
}

// TODO: unit test
void lval_print_str(lval *value) {
  /* Make a Copy of the string */
  char *escaped = malloc(strlen(value->val_string) + 1);
  strcpy(escaped, value->val_string);
  /* Pass it through the escape function */
  escaped = mpcf_escape(escaped);
  /* Print it between " characters */
  printf("\"%s\"", escaped);
  /* free the copied string */
  free(escaped);
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
  case LVAL_STR:
    lval_print_str(value);
    break;
  case LVAL_FUN:
    if (value->val_fun.builtin) {
      printf("<builtin>");
    } else {
      printf("(\\ ");
      lval_print(value->val_fun.formals);
      putchar(' ');
      lval_print(value->val_fun.body);
      putchar(')');
    }
    break;
  case LVAL_SEXPR:
    lval_expr_print(value, '(', ')');
    break;
  case LVAL_QEXPR:
    lval_expr_print(value, '{', '}');
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

lval *lval_take(lval *value, int i) {
  lval *taken = lval_pop(value, i);
  lval_del(value);
  return taken;
}
