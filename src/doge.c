#include "mpc.h"
#include <stdio.h>

int number_of_nodes(mpc_ast_t* tree) {
  if (tree->children_num == 0) { return 1; }
  if (tree->children_num >= 0) {
    int total = 1;
    for (int i = 0; i < tree->children_num; i++) {
      total = total + number_of_nodes(tree->children[i]);
    }
    return total;
  }
  return 0;
}

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
    /* Load AST from output */
    mpc_ast_t *a = result.output;
    printf("Tag: %s\n", a->tag);
    printf("Contents: %s\n", a->contents);
    printf("Number of children: %i\n", a->children_num);
    printf("Number of nodes: %i\n", number_of_nodes(a));
    printf("\n");

    /* Get First Phrase */
    mpc_ast_t *c1 = a->children[1];
    printf("First Phrase Tag: %s\n", c1->tag);
    printf("First Phrase Contents: %s\n", c1->contents);
    printf("First Phrase Number of children: %i\n", c1->children_num);
    printf("First Phrase Number of nodes: %i\n", number_of_nodes(c1));
    printf("\n");

    /* Get First Phrase Child */
    mpc_ast_t *c10 = c1->children[0];
    printf("First Phrase Child Tag: %s\n", c10->tag);
    printf("First Phrase Child Contents: %s\n", c10->contents);
    printf("First Phrase Child Number of children: %i\n", c10->children_num);
    printf("First Phrase Child Number of nodes: %i\n", number_of_nodes(c10));
    printf("\n");

    mpc_ast_print(result.output);
    mpc_ast_delete(result.output);
  } else {
    mpc_err_print(result.output);
    mpc_err_delete(result.output);
  }

  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);

  return 0;
}
