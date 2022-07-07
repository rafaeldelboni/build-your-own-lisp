#include "lval.h"
#include "mpc.h"

lval *lispy_read_long(mpc_ast_t *tree);
lval *lispy_read_double(mpc_ast_t *tree);
lval *lispy_read(mpc_ast_t *tree);
lval *lispy_eval_op(lval *x, char *op, lval *y);
