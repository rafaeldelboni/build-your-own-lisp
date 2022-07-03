#include "lval.h"
#include <stdio.h>

lval lval_num(long x) {
  lval value;
  value.type = LVAL_NUM;
  value.num = x;
  return value;
}

lval lval_err(int x) {
  lval value;
  value.type = LVAL_ERR;
  value.err = x;
  return value;
}

const char *lval_string(lval value) {
  char *buffer;

  switch (value.type) {
  /* In the case the type is a number print it */
  /* Then 'break' out of the switch*/
  case LVAL_NUM:
    snprintf(buffer, 11, "%ld", value.num);
    break;

  /* In the case the typeis an error */
  case LVAL_ERR:
    /* Check what type of error it is and print it */
    if (value.err == LERR_DIV_ZERO) {
      buffer = "Error: Division By Zero!";
    }
    if (value.err == LERR_BAD_OP) {
      buffer = "Error: Invalid Operator!";
    }
    if (value.err == LERR_BAD_NUM) {
      buffer = "Error: Invalid Number!";
    }
    break;
  }

  return buffer;
}
