#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE 1024

enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE};

typedef struct {
    char *var;
    int val;
} Pair;

// [var val][var val][][]

typedef struct {
    Pair p[MAXSIZE];
    size_t length;
    size_t capacity;
} Env;

int empty_envp(Env env) {
    return env.length == 0;
}

Env init_env() {
    Env env;
    memset(env.p, 0, sizeof (Pair) * MAXSIZE);
    env.length = 0;
    env.capacity = MAXSIZE;
    return env;
}

Env extend_env(char *var, int val, Env env) {
    Pair p;
    p.var = strdup(var);
    p.val = val;
    Env new_env = env;
    new_env.p[new_env.length] = p;
    new_env.length++;
    return new_env;
}

int lookup(char *var, Env env) {
    int i;
    for (i = 0; i < env.length; i++) {
	if (strcmp(env.p[i].var, var) == 0) {
	    return env.p[i].val;
	}
    }
}

typedef struct EXP Exp;
typedef struct AE APPLY_EXP;
typedef struct LE LAMBDA_EXP;

typedef struct {
    char *opt; // + - * /
    Exp *b1;
    Exp *b2;
} THREE_EXP;

typedef struct AE{
    LAMBDA_EXP *fun;
    Exp *body;
} APPLY_EXP;

typedef struct LE {
    char *arg;
    Exp *body;
} LAMBDA_EXP;

typedef struct EXP {
    int type; // INT STR LAMBDA THREE APPLY
    union u {
	int num;
        char *str;
        LAMBDA_EXP *lambda;
        THREE_EXP *three;
        APPLY_EXP *apply;
    } as;
} Exp;

typedef struct CE {
    char *x;
    Exp *body;
    Env env;
} Closure;

Closure Close(char *arg, Exp *body, Env env) {
    Closure ce;
    ce.x = strdup(arg);
    ce.body = body;
    ce.env = env;
    return ce;
}

typedef struct {
    int type; // INT, CLOSURE
    union u2 {
	int num;
	Closure *closure;
    } as;
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

	char op = opt[0];
	RESULT r;
	switch (op) {
	case '+':
	    r.type = INT;
	    r.as.num = v1.as.num + v2.as.num;
	    return r;

	case '-':
	    r.type = INT;
	    r.as.num = v1.as.num - v2.as.num;
	    return r;
	case '*':
	    r.type = INT;
	    r.as.num = v1.as.num * v2.as.num;
	    return r;
	case '/':
	    r.type = INT;
	    r.as.num = (int)(v1.as.num / v2.as.num);
	    return r;
	}
	return (RESULT){INT, 0};
    case APPLY:
	RESULT close_fun = interpreter(exp.as.apply->fun, env);
	Exp ebody = interpreter(exp.as.apply->body, env);
	
	char *x = closure_x (close_fun);
	Exp *body = closure_body (close_fun);
	Env *env = closure_env (close_fun);
	
	Env new_env = extend_env(x, ebody, *env);
	return interpreter(body, new_env);
    default:
	break;
    }
}
int main() {

    return 0;
}
