#include "lenv.h"
#include "lval.h"

lval *builtin_head(lenv *env, lval *arg);

lval *builtin_tail(lenv *env, lval *arg);

lval *builtin_list(lenv *env, lval *arg);

lval *builtin_join(lenv *env, lval *arg);

lval *builtin_cons(lenv *env, lval *arg);

lval *builtin_len(lenv *env, lval *arg);

lval *builtin_init(lenv *env, lval *arg);

lval *builtin_def(lenv *env, lval *arg);

lval *builtin_eval(lenv *env, lval *arg);

lval *builtin_op(lenv *env, lval *value, char *op);

lval *builtin_eval_op(lval *x, char *op, lval *y);

void builtin_default_functions(lenv *env);

lval *builtin_lval_eval(lenv *env, lval *value);

lval *builtin_lval_eval_sexpr(lenv *env, lval *value);
