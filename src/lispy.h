#include "lval.h"
#include "mpc.h"

lval eval_op(lval x, char *op, lval y);
lval eval(mpc_ast_t *tree);
