#include "lval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval lval_long(long x) {
  lval value;
  value.type = LVAL_LONG;
  value.val_long = x;
  return value;
}

lval lval_double(double x) {
  lval value;
  value.type = LVAL_DOUBLE;
  value.val_double = x;
  return value;
}

lval lval_err(int x) {
  lval value;
  value.type = LVAL_ERR;
  value.err = x;
  return value;
}

void lval_string(lval value, char *output) {
  switch (value.type) {
  /* MAX Long (9223372036854775807) has ~20 digits */
  case LVAL_LONG:
    snprintf(output, 20, "%ld", value.val_long);
    break;

  /* MAX Double (1.8 x 10^308) has ~310 decimal digits */
  case LVAL_DOUBLE:
    snprintf(output, 310, "%lf", value.val_double);
    break;

  /* In the case the typeis an error */
  case LVAL_ERR:
    /* Check what type of error it is and return it */
    if (value.err == LERR_DIV_ZERO) {
      strncpy(output, "Error: Division By Zero!", 25);
    }
    if (value.err == LERR_BAD_OP) {
      strncpy(output, "Error: Invalid Operator!", 25);
    }
    if (value.err == LERR_BAD_NUM) {
      strncpy(output, "Error: Invalid Number!", 25);
    }
    break;
  }
}

void lval_println(lval value) {
  char *output = (char *)malloc(sizeof(char) * 4);
  lval_string(value, output);
  printf("%s\n", output);
  free(output);
}
