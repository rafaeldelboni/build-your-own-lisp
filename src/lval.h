/* Create Enumeration of Possible lval Types */
enum { LVAL_ERR, LVAL_LONG, LVAL_DOUBLE };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Declare New lval Struct */
typedef struct {
  int type;
  /* Union allows to store different data types in the same memory location */
  union {
    long val_long;
    double val_double;
  };
  int err;
} lval;

/* Create a new number type lval */
lval lval_long(long x);

/* Create a new double type lval */
lval lval_double(double x);

/* Create a new error type lval */
lval lval_err(int x);

/* Create a string on lval type */
void lval_string(lval value, char *output);

void lval_println(lval value);
