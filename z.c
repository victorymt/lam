#include <stdio.h>

enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE};


typedef struct {
    char *opt; // + - * /
    BODY_EXP *b1;
    BODY_EXP *b2;
} THREE_EXP;

typedef struct {
    LAMBDA_EXP *fun;
    BODY_EXP *body;
} APPLY_EXP;

typedef struct {
    int type;
    union as {
	int num;
	char *str;
	THREE *three;
	APPLY_EXP *apply;
    }
} BODY_EXP;

typedef struct {
    int *arg;
    BODY_EXP *body;
} LAMBDA_EXP;

typedef struct {
    int type; // INT STR LAMBDA THREE APPLY
    union as {
	int num;
        char *str;
        LAMBDA_EXP *lambda;
        THREE_EXP *three;
        APPLY_EXP *apply;
    }
} Exp;

typedef struct {
    int type; // INT, CLOSURE
    union as {
	int num;
	CLOSURE *closure;
    }
} RESULT;

RESULT interpreter(Exp exp, Env env) {
    switch (exp.type) {
    case INT:
	return (RESULT){INT, exp.as.num};
	break;
    case STR:
	int r = lookup(exp.as.str, env);
	return (RESULT){INT, r};
	break;
    case LAMBDA:
	return (RESULT){CLOSURE, Close(exp.as.lambda->arg, exp.as.lambda->body, env)};
	break;
    case THREE:
	char *opt = exp.as.three->opt;
	BODY_EXP *b1 = exp.as.three->b1;
	BODY_EXP *b2 = exp.as.three->b2;
	
	Exp *e1 = Body2Exp(b1);
	Exp *e2 = Body2Exp(b2);

	RESULT v1 = interpreter(*e1, env);
	RESULT v2 = interpreter(*e2, env);

	char op = str2char(opt);
	switch (op) {
	case '+':
	    return (RESULT){INT, v1.as.num + v2.as.num};

	case '-':
	    return (RETURN){INT, v1.as.num - v2.as.num};
	case '*':
	    return (RETURN){INT, v1.as.num * v2.as.num};
	case '/':
	    return (RETURN){INT, (int)(v1.as.num / v2.as.num)}
	}
	return;
    case APPLY:
	// RESULT fun = interpreter(exp.as.apply->fun)    过一会再看
	
    }

	
}
int main() {

    return 0;
}
