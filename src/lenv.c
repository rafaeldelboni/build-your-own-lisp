#include "lenv.h"
#include "lval.h"
#include <stdlib.h>
#include <string.h>

// TODO: unit test
lenv *lenv_new(void) {
  lenv *env = malloc(sizeof(lenv));
  env->parent = NULL;
  env->count = 0;
  env->syms = NULL;
  env->vals = NULL;
  return env;
}

// TODO: unit test
void lenv_del(lenv *env) {
  for (int i = 0; i < env->count; i++) {
    free(env->syms[i]);
    lval_del(env->vals[i]);
  }
  free(env->syms);
  free(env->vals);
  free(env);
}

// TODO: unit test
lval *lenv_get(lenv *env, lval *value) {
  /* Iterate over all items in environment */
  for (int i = 0; i < env->count; i++) {
    /* Check of the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(env->syms[i], value->val_symbol) == 0) {
      return lval_copy(env->vals[i]);
    }
  }
  /* If no symbol found return error */
  if (env->parent) {
    return lenv_get(env->parent, value);
  } else {
    return lval_err("Unbound symbol! '%s'", value->val_symbol);
  }
}

// TODO: unit test
void lenv_put(lenv *env, lval *symbol, lval *value) {
  /* Iterate over all items in environment */
  /* This is to see if variable already exits */
  for (int i = 0; i < env->count; i++) {
    /* If variable is found  delete iteem at that position */
    /* And replace with variable supplied by user */
    if (strcmp(env->syms[i], symbol->val_symbol) == 0) {
      lval_del(env->vals[i]);
      env->vals[i] = lval_copy(value);
      return;
    }
  }

  /* If no existing entry found allocate space for new entry */
  env->count++;
  env->vals = realloc(env->vals, sizeof(lval *) * env->count);
  env->syms = realloc(env->syms, sizeof(char *) * env->count);

  /* Copy contens of lval and symbol string into new location */
  env->vals[env->count - 1] = lval_copy(value);
  env->syms[env->count - 1] = malloc(strlen(symbol->val_symbol) + 1);
  strcpy(env->syms[env->count - 1], symbol->val_symbol);
}

// TODO: unit test
lenv *lenv_copy(lenv *env) {
  lenv *copy = malloc(sizeof(lenv));
  copy->parent = env->parent;
  copy->count = env->count;
  copy->syms = malloc(sizeof(char *) * copy->count);
  copy->vals = malloc(sizeof(lval *) * copy->count);

  for (int i = 0; i < env->count; i++) {
    copy->syms[i] = malloc(strlen(env->syms[i]) + 1);
    strcpy(copy->syms[i], env->syms[i]);
    copy->vals[i] = lval_copy(env->vals[i]);
  }

  return copy;
}

void lenv_def(lenv *env, lval *symbol, lval *value) {
  /* Iterate till `env` has no parent */
  while (env->parent) {
    env = env->parent;
  }
  /* Put value in `env` */
  lenv_put(env, symbol, value);
}

void lenv_add_builtin(lenv *env, char *name, lbuiltin func) {
  lval *symbol = lval_sym(name);
  lval *value = lval_fun(func);
  lenv_put(env, symbol, value);
  lval_del(symbol);
  lval_del(value);
}
