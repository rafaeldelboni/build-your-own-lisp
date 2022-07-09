#include "lval.h"
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

lval *lval_qexpr(void) {
  lval *value = malloc(sizeof(lval));
  value->type = LVAL_QEXPR;
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
