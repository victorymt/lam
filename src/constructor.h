#ifndef CONS_
#define CONS_

#include "ast.h"

Exp *calc(char *opt, Exp *n1, Exp *n2);
Exp *lambda(char *x, Exp *body);
Exp *apply(Exp *lamb, Exp *body);
Exp *num(int n);
Exp *str(char *x);

#endif
