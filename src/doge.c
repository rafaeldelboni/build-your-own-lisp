#include "mpc.h"

int main(int argc, char **argv) {
  mpc_result_t result;

  mpc_parser_t *Adjective = mpc_new("adjective");
  mpc_parser_t *Noun = mpc_new("noun");
  mpc_parser_t *Phrase = mpc_new("phrase");
  mpc_parser_t *Doge = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT, " \
            adjective  : \"wow\" | \"many\" \
                       | \"so\" | \"such\"; \
            noun       : \"lisp\" | \"language\" \
                       | \"book\" | \"build\" | \"c\"; \
            phrase     : <adjective> <noun>; \
            doge       : /^/ <phrase>* /$/; \
            ",
            Adjective, Noun, Phrase, Doge);

  const char *inputs = " \
    wow book such language many lisp \n \
    so c such build such language \n \
    many build wow c \n \
    wow lisp wow c many language \n \
    so c";

  if (mpc_parse("inputs", inputs, Doge, &result)) {
    mpc_ast_print(result.output);
    mpc_ast_delete(result.output);
  } else {
    mpc_err_print(result.output);
    mpc_err_delete(result.output);
  }

  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);

  return 0;
}
