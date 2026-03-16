#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXSIZE 1024

enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE};

typedef struct RE RESULT;

void ignore(char *str) {
    fprintf(stderr, "%s", str);
}

typedef struct {
    char *var;
    RESULT *val;
} Pair;

// [var val][var val][][]

typedef struct {
    Pair p[MAXSIZE];
    size_t length;
    size_t capacity;
} Env;

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
    p.var = strdup(var);
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

typedef struct EXP Exp;
typedef struct AE APPLY_EXP;
typedef struct LE LAMBDA_EXP;

typedef struct {
    char *opt; // + - * /
    Exp *b1;
    Exp *b2;
} THREE_EXP;


typedef struct AE{
    Exp *fun;
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

Exp *apply2exp(APPLY_EXP *ae) {
    Exp *e = malloc (sizeof (Exp));
    e->type = APPLY;
    e->as.apply = ae;
    return e;
}

typedef struct CE {
    char *x;
    Exp *body;
    Env env;
} Closure;

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
 

typedef struct RE {
    int type; // INT, CLOSURE
    union u2 {
	int num;
	Closure closure;
    } as;
} RESULT;

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
	/*
    case [fun, e]:
        fun = interpreter(fun, env)
        arg = closure_x (fun)
        body = closure_body (fun)
        env = closure_env (fun)
        new_env = extend_env (arg, interpreter (e, env), env)
	return interpreter (body, new_env)
	*/

	// 先把 apply 的函数变成闭包

	// [fun body]
	// fun -> closure
	
	// 这里，exp.as.apply->fun 如果不是 lambda 呢，如果是 lambda ，就直接变成闭包就可以了，如果本身是一个 apply 呢，所以不应该进行转换，同时类型也不应该局限在 lambda，应该是 Exp
	
	RESULT *close_fun = interpreter(*(exp.as.apply->fun), env);
	Closure result_ce = result2closure(*close_fun);
	
	char *x = closure_x (result_ce);
	Exp *body = closure_body (result_ce);
	Env oenv = closure_env (result_ce);

	// TODO 是这里的原因？导致无法嵌套 lambda?
	RESULT *ebody = interpreter(*(exp.as.apply->body), env); // ebody 可能是 INT，也可能是 ClOSURE
	// int ebody_val = result2int(ebody);
	// new_env = extend_env (arg, interpreter (e, env), env) 对不上
	Env new_env = extend_env(x, ebody, oenv); // 这里不对
	return interpreter(*body, new_env);
    default:
	assert(1 == 2);
    }
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

// lambda :: char * -> Exp -> Exp*
Exp* lambda(char *x, Exp *body) {
    LAMBDA_EXP *le = malloc (sizeof (LAMBDA_EXP));
    le->arg = strdup(x);
    le->body = body;
    Exp *e = malloc (sizeof (Exp));
    e->type = LAMBDA;
    e->as.lambda = le;
    return e;
}

// apply :: Exp* -> Exp* -> Exp*
// apply 的 lamb 可能接收的是另一个 apply
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
	// ignore("IDONT KNOW");
	APPLY_EXP *ae = malloc (sizeof (APPLY_EXP));
	// 充当 apply 的 fun 的是 apply 这个整体，但类型又不统一 Exp <-> APPLY_EXP
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
    s->as.str = strdup(x);
    return s;
}

int main() {
    // 写一个友好的转换方式
    // Exp c = calc("+", 1, 2);
    // Exp p = lambda("x", calc("+", 1, 2));
    // Exp a = apply(lambda("x", str("x")), 1);
    // Exp i = num(1);
    // Exp s = str("x")

    Exp *e = calc("+", num(1), num(2));
    Exp *e1 = apply(lambda("x", calc("+", str("x"), num(1))), num(1));

    // 原因好像是 apply 接收的默认是 lambda, 而这个是 apply
    Exp *e2 = apply(apply(lambda("x", lambda("y", calc("+", str("x"), str("y")))), num(1)), num(2)); // 这个现在好像还不支持嵌套 lambda

    Env ne;
    ne = init_env();
    RESULT *re = interpreter(*e2, ne);
    int result = result2int(*re);
    printf("%d", result);
    return 0;
}

