/* Create Enumeration of Possible lval Types */
enum { LVAL_ERR, LVAL_LONG, LVAL_DOUBLE, LVAL_SYM, LVAL_SEXPR };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Declare New lval Struct */
typedef struct lval{
  int type;
  /* Union allows to store different data types in the same memory location */
  union {
    long val_long;
    double val_double;
    char *val_symbol;
    char *val_err;
  };
  /* Count and Pointer to a list of "lval*" */
  int count;
  struct lval** cell;
} lval;

/* Create a new number type lval */
lval *lval_long(long l);

/* Create a new double type lval */
lval *lval_double(double d);

/* Create a new error type lval */
lval *lval_err(char* error_str);

/* Construct a pointer to a new Symbol lval */
lval *lval_sym(char* s);

/* A pointer to a new empty Sexpr lval*/
lval *lval_sexpr(void);

lval *lval_add(lval *parent, lval *child);

void lval_del(lval *value);

void lval_print(lval *value);

void lval_expr_print(lval *value, char open, char close);

void lval_println(lval *value);


lval *lval_pop(lval *value, int i);

lval *lval_take(lval *value, int i);

lval *lval_eval_op(lval *x, char *op, lval *y);

lval *lval_builtin_op(lval *value, char *op);

lval *lval_eval(lval *value);

lval *lval_eval_sexpr(lval *value);
