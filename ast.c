#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"

// ignore :: char* -> IO
void ignore(char *str) {
    fprintf(stderr, "%s", str);
}

// empty_envp :: Env -> int
int empty_envp(Env env) {
    return env.length == 0;
}

// init_env :: () -> Env
Env init_env() {
    Env env;
    memset(env.p, 0, sizeof (Pair) * MAXSIZE);
    env.length = 0;
    env.capacity = MAXSIZE;
    return env;
}

// extend_env :: char * -> RESULT -> Env -> Env
Env extend_env(char *var, RESULT *val, Env env) {
    Pair p;
    p.var = var;
    p.val = val;
    Env new_env = env;
    new_env.p[new_env.length] = p;
    new_env.length++;
    return new_env;
}

// Error :: char * -> IO
void Error(char *p) {
    fprintf(stderr, p);
}

// lookup :: char * -> Env -> RESULT
RESULT* lookup(char *var, Env env) {
    int i;
    for (i = 0; i < env.length; i++) {
	if (strcmp(env.p[i].var, var) == 0) {
	    return env.p[i].val;
	}
    }
    Error("unknown symbol!");
}

// apply2exp :: APPLY_EXP* -> Exp
Exp *apply2exp(APPLY_EXP *ae) {
    Exp *e = malloc (sizeof (Exp));
    e->type = APPLY;
    e->as.apply = ae;
    return e;
}

// lambda2exp :: LAMBDA_EXP* -> Exp
Exp* lambda2exp(LAMBDA_EXP *le) {
    Exp *e = malloc (sizeof (Exp));
    e->type = LAMBDA;
    e->as.lambda = le;
    return e;
}

// closure_x :: Closure -> char *
char *closure_x(Closure closure) {
    return closure.x;
}
// closure_body :: Closure -> Exp *
Exp *closure_body(Closure closure) {
    return closure.body;
}

// closure_env :: Closure -> Env
Env closure_env (Closure closure) {
    return closure.env;
}

// Close :: char * -> Exp * -> Env -> Closure
Closure Close(char *arg, Exp *body, Env env) {
    Closure ce;
    ce.x = strdup(arg);
    ce.body = body;
    ce.env = env;
    return ce;
}

// result2closure :: RESULT -> Closure
Closure result2closure(RESULT re) {
    assert(re.type == CLOSURE);
    Closure ce = re.as.closure;
    return ce;
}

// result2int :: RESULT -> int
int result2int(RESULT re) {
    assert(re.type == INT);
    return re.as.num;
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

// 写一个友好的转换方式
// Exp* c = calc("+", 1, 2);
// Exp* p = lambda("x", calc("+", 1, 2));
// Exp* a = apply(lambda("x", str("x")), 1);
// Exp* i = num(1);
// Exp* s = str("x")

void pretty_print(Exp *e);
void pretty_print_apply(APPLY_EXP *apply) {
    pretty_print(apply->fun);
    pretty_print(apply->body);
}

void pretty_print_three(THREE_EXP *e) {
    printf("Opt: %s\n", e->opt);
    pretty_print(e->b1);
    pretty_print(e->b2);
}

void pretty_print_lambda(LAMBDA_EXP *le) {
    printf("Arg: %s\n", le->arg);
    pretty_print(le->body);
}

void pretty_print(Exp *e) {
    switch (e->type) {
    case INT:
	printf("INT: %d\n", e->as.num);
	break;
    case STR:
	printf("STR: %s\n", e->as.str);
	break;
    case LAMBDA:
	pretty_print_lambda(e->as.lambda);
	break;
    case THREE:
	pretty_print_three(e->as.three);
	break;
    case APPLY:
	pretty_print_apply(e->as.apply);
	break;
    default:
	assert(1 == 2);
	break;
    }
}

void print_closure(Closure closure) {
    char *x = closure_x(closure);
    Exp *body = closure_body(closure);
    Env env = closure_env(closure);
    printf("%s\n", x);
    pretty_print(body);
}

// interpreter :: Exp -> Env -> RESULT
RESULT *interpreter(Exp exp, Env env) {
    RESULT *result = malloc (sizeof (RESULT));
    switch (exp.type) {
    case INT:
	result->type = INT;
	result->as.num = exp.as.num;
	return result;
    case STR:
	RESULT *val = lookup(exp.as.str, env);
	return val;
    case LAMBDA:
	result->type = CLOSURE;
	// Close :: char * -> Exp * -> Env -> Closure
	result->as.closure = Close(exp.as.lambda->arg, exp.as.lambda->body, env);
	return result;
    case THREE:
	char *opt = exp.as.three->opt;
	Exp *e1 = exp.as.three->b1;
	Exp *e2 = exp.as.three->b2;
	
	RESULT *v1 = interpreter(*e1, env);
	RESULT *v2 = interpreter(*e2, env);

	char op = opt[0];
	result->type = INT;
	switch (op) {
	case '+':
	    result->as.num = v1->as.num + v2->as.num;
	    return result;

	case '-':
	    result->as.num = v1->as.num - v2->as.num;
	    return result;
	case '*':
	    result->as.num = v1->as.num * v2->as.num;
	    return result;
	case '/':
	    result->as.num = (int)(v1->as.num / v2->as.num);
	    return result;
	}
	assert(1 == 2);
	// return (RESULT){INT, 0};
    case APPLY:

	RESULT *close_fun = interpreter(*(exp.as.apply->fun), env);
	Closure result_ce = result2closure(*close_fun);
	
	char *x = closure_x (result_ce);
	Exp *body = closure_body (result_ce);
	Env oenv = closure_env (result_ce);
	RESULT *ebody = interpreter(*(exp.as.apply->body), env); 
	Env new_env = extend_env(x, ebody, oenv); 
	return interpreter(*body, new_env);
    default:
	assert(1 == 2);
    }
}


