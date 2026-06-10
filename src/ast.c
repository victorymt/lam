#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"

/* ═══════════════════ 环境操作 ═══════════════════ */

// init_env :: () -> Env
Env init_env(void) {
    return NULL;
}

// extend_env :: char * -> RESULT -> Env -> Env
// 创建一个新节点，strdup 持有 var，val 值拷贝，prev 指向旧 env
Env extend_env(char *var, RESULT val, Env env) {
    EnvNode *node = malloc(sizeof(EnvNode));
    if (!node) { Error("out of memory\n"); exit(1); }
    node->var = strdup(var);
    node->val = val;
    node->prev = env;
    return node;
}

// lookup :: char * -> Env -> RESULT
// 沿链表查找，找不到返回 RESULT_NONE
RESULT lookup(char *var, Env env) {
    for (Env cur = env; cur != NULL; cur = cur->prev) {
        if (strcmp(cur->var, var) == 0) {
            return cur->val;
        }
    }
    Error("unknown symbol: ");
    Error(var);
    Error("\n");
    return RESULT_NONE;
}

// Error :: char * -> IO
void Error(char *p) {
    fprintf(stderr, "%s", p);
}

/* ═══════════════════ AST 包装 ═══════════════════ */

// apply2exp :: APPLY_EXP* -> Exp*
Exp *apply2exp(APPLY_EXP *ae) {
    Exp *e = malloc(sizeof(Exp));
    e->type = APPLY;
    e->as.apply = ae;
    return e;
}

// lambda2exp :: LAMBDA_EXP* -> Exp*
Exp *lambda2exp(LAMBDA_EXP *le) {
    Exp *e = malloc(sizeof(Exp));
    e->type = LAMBDA;
    e->as.lambda = le;
    return e;
}

/* ═══════════════════ Closure 工具 ═══════════════════ */

// closure_x :: Closure -> char *
char *closure_x(Closure closure) { return closure.x; }

// closure_body :: Closure -> Exp *
Exp *closure_body(Closure closure) { return closure.body; }

// closure_env :: Closure -> Env
Env closure_env(Closure closure) { return closure.env; }

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
    return re.as.closure;
}

// result2int :: RESULT -> int
int result2int(RESULT re) {
    assert(re.type == INT);
    return re.as.num;
}

/* ═══════════════════ 解释器 ═══════════════════ */

// interpreter :: Exp -> Env -> RESULT
RESULT interpreter(Exp exp, Env env) {
    switch (exp.type) {
    case INT: {
        RESULT r;
        r.type = INT;
        r.as.num = exp.as.num;
        return r;
    }
    case STR: {
        return lookup(exp.as.str, env);
    }
    case LAMBDA: {
        RESULT r;
        r.type = CLOSURE;
        r.as.closure = Close(exp.as.lambda->arg,
                             exp.as.lambda->body,
                             env);
        return r;
    }
    case THREE: {
        char *opt = exp.as.three->opt;
        Exp *e1 = exp.as.three->b1;
        Exp *e2 = exp.as.three->b2;

        RESULT v1 = interpreter(*e1, env);
        RESULT v2 = interpreter(*e2, env);

        char op = opt[0];
        RESULT r;
        r.type = INT;
        switch (op) {
        case '+': r.as.num = v1.as.num + v2.as.num; return r;
        case '-': r.as.num = v1.as.num - v2.as.num; return r;
        case '*': r.as.num = v1.as.num * v2.as.num; return r;
        case '/': r.as.num = v1.as.num / v2.as.num; return r;
        default:
            assert(0 && "unknown operator");
        }
        /* unreachable — suppress -Wimplicit-fallthrough */
        return r;
    }
    case APPLY: {
        RESULT close_fun = interpreter(*(exp.as.apply->fun), env);
        Closure result_ce = result2closure(close_fun);

        char *x    = closure_x(result_ce);
        Exp *body  = closure_body(result_ce);
        Env oenv   = closure_env(result_ce);
        RESULT ebody = interpreter(*(exp.as.apply->body), env);

        Env new_env = extend_env(x, ebody, oenv);
        return interpreter(*body, new_env);
    }
    default:
        assert(0 && "unknown exp type");
    }
}

/* ═══════════════════ Pretty-print (S-表达式) ═══════════════════ */

static void pretty_print_rec(Exp *e);

static void pretty_print_apply(APPLY_EXP *apply) {
    printf("(");
    pretty_print_rec(apply->fun);
    printf(" ");
    pretty_print_rec(apply->body);
    printf(")");
}

static void pretty_print_three(THREE_EXP *e) {
    printf("(%s ", e->opt);
    pretty_print_rec(e->b1);
    printf(" ");
    pretty_print_rec(e->b2);
    printf(")");
}

static void pretty_print_lambda(LAMBDA_EXP *le) {
    printf("(lambda (%s) ", le->arg);
    pretty_print_rec(le->body);
    printf(")");
}

static void pretty_print_rec(Exp *e) {
    switch (e->type) {
    case INT:
        printf("%d", e->as.num);
        break;
    case STR:
        printf("%s", e->as.str);
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
        assert(0);
    }
}

void pretty_print(Exp *e) {
    pretty_print_rec(e);
    printf("\n");
}
