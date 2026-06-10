#include "constructor.h"
#include <stdlib.h>
#include <assert.h>

// lambda :: char * -> Exp -> Exp*
Exp* lambda(char *x, Exp *body) {
    LAMBDA_EXP *le = malloc (sizeof (LAMBDA_EXP));
    le->arg = x;
    le->body = body;
    Exp *e = malloc (sizeof (Exp));
    e->type = LAMBDA;
    e->as.lambda = le;
    return e;
}

// apply :: Exp* -> Exp* -> Exp*
Exp* apply(Exp *lamb, Exp *body) {
    if (lamb->type == LAMBDA) {
	APPLY_EXP *ae = malloc(sizeof (APPLY_EXP));
	ae->fun = lambda2exp(lamb->as.lambda);
	ae->body = body;
	Exp *a = malloc (sizeof (Exp));
	a->type = APPLY;
	a->as.apply = ae;
	return a;
    } else {
	assert(lamb->type == APPLY);
	APPLY_EXP *ae = malloc (sizeof (APPLY_EXP));
	Exp *apply_exp = apply2exp(lamb->as.apply);
	ae->fun = apply_exp;
	ae->body = body;
	Exp *a = malloc (sizeof (Exp));
	a->type = APPLY;
	a->as.apply = ae;
	return a;
    }
}


// num :: int -> Exp*
Exp *num(int n) {
    Exp *ne = malloc (sizeof (Exp));
    ne->type = INT;
    ne->as.num = n;
    return ne;
}

// str :: char* -> Exp*
Exp *str(char *x) {
    Exp *s = malloc(sizeof (Exp));
    s->type = STR;
    s->as.str = x;
    return s;
}

// calc :: char * -> Exp* -> Exp* -> Exp
Exp *calc(char *opt, Exp *n1, Exp *n2) {
    THREE_EXP *nthree = malloc (sizeof (THREE_EXP));
    nthree->opt = opt;
    nthree->b1 = n1;
    nthree->b2 = n2;
    Exp *c = malloc (sizeof (Exp));
    c->type = THREE;
    c->as.three = nthree;
    return c;
}
