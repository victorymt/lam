#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

// extend_env :: char * -> int -> Env -> Env
Env extend_env(char *var, int val, Env env) {
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

// lookup :: char * -> Env -> int
int lookup(char *var, Env env) {
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

// lambda2exp :: LAMBDA_EXP* -> Exp
Exp lambda2exp(LAMBDA_EXP *le) {
    Exp e;
    e.type = LAMBDA;
    e.as.lambda = le;
    return e;
}

// closure_x :: Closure -> char *
char *closure_x(Closure closure) {
    return NULL;
}
// closure_body :: Closure -> Exp *
Exp *closure_body(Closure closure) {
    return NULL;
}

// closure_env :: Closure -> Env *
Env *closure_env (Closure closure) {
    return NULL;
}

// Close :: char * -> Exp * -> Env -> Closure
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
RESULT interpreter(Exp exp, Env env) {
    switch (exp.type) {
    case INT:
	return (RESULT){INT, exp.as.num};
    case STR:
	int val = lookup(exp.as.str, env);
	return (RESULT){INT, val};
    case LAMBDA:
	RESULT rc;
	rc.type = CLOSURE;
	// Close :: char * -> Exp * -> Env -> Closure
	rc.as.closure = Close(exp.as.lambda->arg, exp.as.lambda->body, env);
	return rc;
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
	// interpreter :: Exp -> Env -> RESULT
	//
	// typedef struct EXP {
	    //     int type; // INT STR LAMBDA THREE APPLY
	    //     union u {
		// 	int num;
		//         char *str;
		//         LAMBDA_EXP *lambda;
		//         THREE_EXP *three;
		//         APPLY_EXP *apply;
		//     } as;
		// } Exp;

		// 先把 apply 的函数变成闭包
		// 这里 exp.as.apply->fun 的类型是 LAMBDA_EXP* 不是 Exp，需要先转换
	Exp apply_fun = lambda2exp(exp.as.apply->fun);
	RESULT close_fun = interpreter(apply_fun, env); 
	RESULT ebody = interpreter(*(exp.as.apply->body), env);

	// 从 RESULT 转换成 Clouse
	Closure result_ce = result2closure(close_fun);
	char *x = closure_x (result_ce);
	Exp *body = closure_body (result_ce);
	Env *oenv = closure_env (result_ce);
	// extend_env :: char * -> int -> Env -> Env

	// 从 RESULT 转换成 int
	int ebody_val = result2int(ebody);
	Env new_env = extend_env(x, ebody_val, *oenv);
	return interpreter(*body, new_env);
    default:
	break;
    }
}

int main() {

    return 0;
}
