#ifndef LENV_HEADER_H
#define LENV_HEADER_H

#include "lval.h"

struct lenv {
  lenv *parent;
  int count;
  char **syms;
  lval **vals;
};

lenv *lenv_new(void);

void lenv_del(lenv *env);

lval *lenv_get(lenv *env, lval *value);

void lenv_put(lenv *env, lval *symbol, lval *value);

lenv *lenv_copy(lenv *env);

void lenv_def(lenv *env, lval *symbol, lval *value);

void lenv_add_builtin(lenv *env, char *name, lbuiltin func);

#endif // LENV_HEADER_H
