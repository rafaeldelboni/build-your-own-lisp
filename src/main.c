#include "lval.h"
#include "lispy.h"
#include "builtin.h"
#include "mpc.h"
#include <editline/readline.h>
#include <stdio.h>

mpc_parser_t *Float;
mpc_parser_t *Number;
mpc_parser_t *Symbol;
mpc_parser_t *String;
mpc_parser_t *Comment;
mpc_parser_t *Sexpr;
mpc_parser_t *Qexpr;
mpc_parser_t *Expr;
mpc_parser_t *Lispy;

int main(int argc, char **argv) {
  /* Create Some Parsers */
  Float = mpc_new("float");
  Number = mpc_new("number");
  Symbol = mpc_new("symbol");
  String = mpc_new("string");
  Comment = mpc_new("comment");
  Sexpr = mpc_new("sexpr");
  Qexpr = mpc_new("qexpr");
  Expr = mpc_new("expr");
  Lispy = mpc_new("lispy");

  const char *language = " \
    float: /-?[0-9]+\\.[0-9]+/ ; \
    number: /-?[0-9]+/ ; \
    symbol: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&|]+/ ; \
    string: /\"(\\\\.|[^\"])*\"/ ; \
    comment: /;[^\\r\\n]*/ ; \
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}' ;\
    expr: <float> | <number> | <symbol> | <string> \
        | <comment> | <sexpr> | <qexpr> ; \
    lispy: /^/ <expr>* /$/ ; \
  ";

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT, language, Float, Number, Symbol, String, Comment,
            Sexpr, Qexpr, Expr, Lispy);

  /* Create the state of the actual env */
  lenv *env = lenv_new();
  lispy_builtin_default_functions(env);

  /* Interactive prompt */
  if (argc == 1) {
    /* Print Version and Exit information */
    puts("Lispy Version 0.0.14");
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
        /*mpc_ast_print(mpc_result.output);*/
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
  }

  /* Supplied with list of files */
  if (argc >= 2) {
    /* loop over each supplied filename (starting from 1) */
    for (int i = 1; i < argc; i++) {
      /* Argument list with a single argument, the filename */
      lval *args = lval_add(lval_sexpr(), lval_str(argv[i]));
      /* Pass to builtin load and get the result */
      lval *result = lispy_load(env, args);

      /* If the result is an error be sure to print it */
      if (result->type == LVAL_ERR) {
        lval_println(result);
      }
      lval_del(result);
    }
  }

  /* Free state environment */
  lenv_del(env);

  /* Undefine and Delete our Parsers */
  mpc_cleanup(6, Float, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr,
              Lispy);

  return 0;
}
