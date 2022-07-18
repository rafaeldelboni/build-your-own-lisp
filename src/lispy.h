#include "lval.h"
#include "builtin.h"
#include "mpc.h"

lval *lispy_read_long(mpc_ast_t *tree);
lval *lispy_read_double(mpc_ast_t *tree);
lval *lispy_read_string(mpc_ast_t *tree);
lval *lispy_read(mpc_ast_t *tree);
lval *lispy_load(lenv *env, lval *arg);
void lispy_builtin_default_functions(lenv *env);
