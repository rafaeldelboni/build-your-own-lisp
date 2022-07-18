#include "lenv.h"
#include "lval.h"
#include "mpc.h"

lval *builtin_ord(lenv *env, lval *arg, char *op);

lval *builtin_gt(lenv *env, lval *arg);

lval *builtin_lt(lenv *env, lval *arg);

lval *builtin_ge(lenv *env, lval *arg);

lval *builtin_le(lenv *env, lval *arg);

lval *builtin_cmp(lenv *env, lval *arg, char *op);

lval *builtin_if(lenv *env, lval *arg);

lval *builtin_or(lenv *env, lval *arg);

lval *builtin_and(lenv *env, lval *arg);

lval *builtin_head(lenv *env, lval *arg);

lval *builtin_tail(lenv *env, lval *arg);

lval *builtin_list(lenv *env, lval *arg);

lval *builtin_join(lenv *env, lval *arg);

lval *builtin_cons(lenv *env, lval *arg);

lval *builtin_len(lenv *env, lval *arg);

lval *builtin_init(lenv *env, lval *arg);

lval *builtin_var(lenv *env, lval *arg, char *func);

lval *builtin_def(lenv *env, lval *arg);

lval *builtin_put(lenv *env, lval *arg);

lval *builtin_eval(lenv *env, lval *arg);

lval *builtin_lambda(lenv *env, lval *arg);

lval *builtin_print(lenv *env, lval *arg);

lval *builtin_error(lenv *env, lval *arg);

lval *builtin_op(lenv *env, lval *value, char *op);

lval *builtin_add(lenv *env, lval *value);

lval *builtin_sub(lenv *env, lval *value);

lval *builtin_mul(lenv *env, lval *value);

lval *builtin_div(lenv *env, lval *value);

lval *builtin_rest(lenv *env, lval *value);

lval *builtin_pow(lenv *env, lval *value);

lval *builtin_min(lenv *env, lval *value);

lval *builtin_max(lenv *env, lval *value);

lval *builtin_not(lenv *env, lval *arg);

lval *builtin_eq(lenv *env, lval *arg);

lval *builtin_ne(lenv *env, lval *arg);

lval *builtin_eval_op(lval *x, char *op, lval *y);

lval *builtin_lval_call(lenv *env, lval *func, lval *arg);

lval *builtin_lval_eval(lenv *env, lval *value);

lval *builtin_lval_eval_sexpr(lenv *env, lval *value);
