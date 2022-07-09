#include "lval.h"

lval *builtin_head(lval *arg);

lval *builtin_tail(lval *arg);

lval *builtin_list(lval *arg);

lval *builtin_join(lval *arg);

lval *builtin_eval(lval *value);

lval *builtin_op(lval *value, char *op);

lval *builtin_functions(lval *arg, char *func);

lval *builtin_eval_op(lval *x, char *op, lval *y);

lval *builtin_lval_eval(lval *value);

lval *builtin_lval_eval_sexpr(lval *value);
