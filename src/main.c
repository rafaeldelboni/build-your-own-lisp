#include "builtin.h"
#include "lispy.h"
#include "lval.h"
#include "mpc.h"
#include <editline/readline.h>
#include <stdio.h>

int main(int argc, char **argv) {
  /* Create Some Parsers */
  mpc_parser_t *Float = mpc_new("float");
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Sexpr = mpc_new("sexpr");
  mpc_parser_t *Qexpr = mpc_new("qexpr");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  const char *language = " \
    float: /-?[0-9]+\\.[0-9]+/ ; \
    number: /-?[0-9]+/ ; \
    symbol: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}' ;\
    expr: <float> | <number> | <symbol> | <sexpr> | <qexpr> ; \
    lispy: /^/ <expr>* /$/ ; \
  ";

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT, language, Float, Number, Symbol, Sexpr, Qexpr,
            Expr, Lispy);

  /* Print Version and Exit information */
  puts("Lispy Version 0.0.12");
  puts("Press Ctrl+c to Exit\n");

  /* Create the state of the actual env */
  lenv *env = lenv_new();
  builtin_default_functions(env);

  /* In a never endind loop */
  while (1) {
    /* Output our prompt and get input */
    char *input = readline("lispy> ");

    /* Add input to history */
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t mpc_result;
    if (mpc_parse("<stdin>", input, Lispy, &mpc_result)) {
      /*mpc_ast_print(mpc_result.output);*/
      /* On Success Print the result of the evaluation*/
      lval *eval_read = builtin_lval_eval(env, lispy_read(mpc_result.output));
      lval_println(eval_read);
      mpc_ast_delete(mpc_result.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(mpc_result.output);
      mpc_err_delete(mpc_result.output);
    }

    /* Free retrieved input */
    free(input);
  }

  /* Free state environment */
  lenv_del(env);

  /* Undefine and Delete our Parsers */
  mpc_cleanup(6, Float, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

  return 0;
}
