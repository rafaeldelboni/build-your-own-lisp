#include "lispy.h"
#include <stdlib.h>
#include <string.h>

long eval_op(long x, char *op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  };
  if (strcmp(op, "-") == 0) {
    return x - y;
  };
  if (strcmp(op, "*") == 0) {
    return x * y;
  };
  if (strcmp(op, "/") == 0) {
    return x / y;
  };
  return 0;
}

long eval(mpc_ast_t *tree) {
  /* If tagged as number return it directly. */
  if (strstr(tree->tag, "number")) {
    return atoi(tree->contents);
  }

  /* The operator is aways second child. */
  char *op = tree->children[1]->contents;

  /* We store the third child in `x` */
  long x = eval(tree->children[2]);

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (i < tree->children_num && strstr(tree->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(tree->children[i]));
    i++;
  }

  return x;
}
