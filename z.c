// 我觉得可以不要 BODY_EXP，直接用 Exp 代替就可以了，就是使用其中的一部分
#include <stdio.h>

enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE};


typedef struct {
    char *opt; // + - * /
    Exp *b1;
    Exp *b2;
} THREE_EXP;

typedef struct {
    LAMBDA_EXP *fun;
    Exp *body;
} APPLY_EXP;

typedef struct {
    char *arg;
    Exp *body;
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
	Exp *e1 = exp.as.three->b1;
	Exp *e2 = exp.as.three->b2;
	
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
