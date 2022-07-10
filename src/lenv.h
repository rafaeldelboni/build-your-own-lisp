#include "lval.h"

struct lenv {
  int count;
  char **syms;
  lval **vals;
};

lenv *lenv_new(void);

void lenv_del(lenv *env);

lval *lenv_get(lenv *env, lval *value);

void lenv_put(lenv *env, lval *symbol, lval *value);

void lenv_add_builtin(lenv *env, char *name, lbuiltin func);
