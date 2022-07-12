#ifndef LVAL_HEADER_H
#define LVAL_HEADER_H

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Create Enumeration of Possible lval Types */
enum {
  LVAL_ERR,
  LVAL_LONG,
  LVAL_DOUBLE,
  LVAL_SYM,
  LVAL_FUN,
  LVAL_SEXPR,
  LVAL_QEXPR
};

typedef lval *(*lbuiltin)(lenv *, lval *);

/* Declare New lval Struct */
typedef struct lval {
  int type;
  /* Union allows to store different data types in the same memory location */
  union {
    long val_long;
    double val_double;
    char *val_symbol;
    // lbuiltin val_fun;
    char *val_err;
    struct {
      lbuiltin builtin;
      lenv *env;
      lval *formals;
      lval *body;
    } val_fun;
  };
  /* Count and Pointer to a list of "lval*" */
  int count;
  struct lval **cell;
} lval;

char *lval_ltype_name(int type);

/* Create a new number type lval */
lval *lval_long(long l);

/* Create a new double type lval */
lval *lval_double(double d);

/* Create a new error type lval */
lval *lval_err(char *fmt, ...);

/* Construct a pointer to a new Symbol lval */
lval *lval_sym(char *s);

/* Construct a pointer to a new Function lval */
lval *lval_fun(lbuiltin func);
lval *lval_lambda(lval *formals, lval *body);

/* A pointer to a new empty Sexpr lval*/
lval *lval_sexpr(void);

/* A pointer to a new empty Qexpr lval */
lval *lval_qexpr(void);

lval *lval_add(lval *parent, lval *child);

void lval_del(lval *value);

lval *lval_copy(lval *value);

void lval_print(lval *value);

void lval_expr_print(lval *value, char open, char close);

void lval_println(lval *value);

lval *lval_pop(lval *value, int i);

lval *lval_take(lval *value, int i);

#endif // LVAL_HEADER_H
