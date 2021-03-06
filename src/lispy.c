#include "lval.h"
#include "lispy.h"
#include "mpc.h"
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

// TODO:unit test
lval *lispy_read_string(mpc_ast_t *tree) {
  /* Cut off the final quote character */
  tree->contents[strlen(tree->contents) - 1] = '\0';
  /* Copy the string missing out the first quote character */
  char *unescaped = malloc(strlen(tree->contents + 1) + 1);
  strcpy(unescaped, tree->contents + 1);
  /* Pass through the unescape function */
  unescaped = mpcf_unescape(unescaped);
  /* Construct a new lval using the string */
  lval *string = lval_str(unescaped);
  /* Free the string and return */
  free(unescaped);
  return string;
}

lval *lispy_read(mpc_ast_t *tree) {
  /* If Symbol, String, Number or Float return conversion to that type */
  if (strstr(tree->tag, "number")) {
    return lispy_read_long(tree);
  }
  if (strstr(tree->tag, "float")) {
    return lispy_read_double(tree);
  }
  if (strstr(tree->tag, "symbol")) {
    return lval_sym(tree->contents);
  }
  if (strstr(tree->tag, "string")) {
    return lispy_read_string(tree);
  }

  /* If root (>) or sexpr then create empty list */
  lval *value = NULL;
  if (strcmp(tree->tag, ">") == 0) {
    value = lval_sexpr();
  }
  if (strstr(tree->tag, "sexpr")) {
    value = lval_sexpr();
  }
  if (strstr(tree->tag, "qexpr")) {
    value = lval_qexpr();
  }

  /* Fill this list with any valid expression contained within */
  for (int i = 0; i < tree->children_num; i++) {
    if (strcmp(tree->children[i]->contents, "(") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->contents, ")") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->contents, "{") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->contents, "}") == 0) {
      continue;
    }
    if (strcmp(tree->children[i]->tag, "regex") == 0) {
      continue;
    }
    if (strstr(tree->children[i]->tag, "comment")) {
      continue;
    }
    value = lval_add(value, lispy_read(tree->children[i]));
  }

  return value;
}

lval *lispy_load(lenv *env, lval *arg) {
  /* Parse file given by string name */
  mpc_result_t result;
  if (mpc_parse_contents(arg->cell[0]->val_string, Lispy, &result)) {
    /* Read contents */
    lval *expressions = lispy_read(result.output);
    mpc_ast_delete(result.output);
    /* Evaluate each Expression */
    while (expressions->count) {
      lval *expr = builtin_lval_eval(env, lval_pop(expressions, 0));
      /* If Evaluation leads to error print it */
      if (expr->type == LVAL_ERR) {
        lval_println(expr);
      }
      lval_del(expr);
    }
    /* Delete expressions and arguments */
    lval_del(expressions);
    lval_del(arg);
    /* Return empty list */
    return lval_sexpr();
  } else {
    /* Get Parse Error as String */
    char *err_msg = mpc_err_string(result.error);
    mpc_err_delete(result.error);
    /* Create new error message using it */
    lval *err = lval_err("Could not load libraty %s", err_msg);
    free(err_msg);
    lval_del(arg);
    /* Cleanup and return error */
    return err;
  }
}

void lispy_builtin_default_functions(lenv *env) {
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
  lenv_add_builtin(env, "load", lispy_load);
  lenv_add_builtin(env, "error", builtin_error);
  lenv_add_builtin(env, "print", builtin_print);
}
