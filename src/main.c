#include "lispy.h"
#include "mpc.h"
#include <editline/readline.h>
#include <stdio.h>

int main(int argc, char **argv) {
  /* Create Some Parsers */
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");
  const char *language = " \
    number: /-?[0-9]+(\\.[0-9]+)?/ ; \
    operator: '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\"; \
    expr: <number> | '(' <operator> <expr>+ ')' ; \
    lispy: /^/ <operator> <expr>+ /$/ ; \
  ";

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT, language, Number, Operator, Expr, Lispy);

  /* Print Version and Exit information */
  puts("Lispy Version 0.0.3");
  puts("Press Ctrl+c to Exit\n");

  /* In a never endind loop */
  while (1) {
    /* Output our prompt and get input */
    char *input = readline("lispy> ");

    /* Add input to history */
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t mpc_result;
    if (mpc_parse("<stdin>", input, Lispy, &mpc_result)) {
      /* On Success Print the result of the evaluation*/
      lval eval_result = eval(mpc_result.output);
      lval_println(eval_result);
      mpc_ast_delete(mpc_result.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(mpc_result.output);
      mpc_err_delete(mpc_result.output);
    }

    /* Free retrieved input */
    free(input);
  }

  /* Undefine and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
