#include "lispy.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval *lispy_read_long(mpc_ast_t *tree) {
  errno = 0;
  long value = strtol(tree->contents, NULL, 10);
  return errno != ERANGE ? lval_long(value) : lval_err("Invalid integer");
}

lval *lispy_read_double(mpc_ast_t *tree) {
  errno = 0;
  double value = strtod(tree->contents, NULL);
  return errno != ERANGE ? lval_double(value) : lval_err("Invalid float");
}

lval *lispy_read(mpc_ast_t *tree) {
  /* If Symbol, Number or Float return conversion to that type */
  if (strstr(tree->tag, "number")) {
    return lispy_read_long(tree);
  }
  if (strstr(tree->tag, "float")) {
    return lispy_read_double(tree);
  }
  if (strstr(tree->tag, "symbol")) {
    return lval_sym(tree->contents);
  }

  /* If root (>) or sexpr then create empty list */
  lval *value = NULL;
  if (strcmp(tree->tag, ">") == 0) {
    value = lval_sexpr();
  }
  if (strstr(tree->tag, "sexpr")) {
    value = lval_sexpr();
  }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < tree->children_num; i++) {
    if (strcmp(tree->children[i]->contents, "(") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->contents, ")") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->tag, "regex") == 0) {
      continue;
    }
    value = lval_add(value, lispy_read(tree->children[i]));
  }

  return value;
}
