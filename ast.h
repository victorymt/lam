#ifndef Z_INTERPRETER_
#define Z_INTERPRETER_

#define MAXSIZE 1024
enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE};

typedef struct RE RESULT;
typedef struct EXP Exp;
typedef struct AE APPLY_EXP;
typedef struct LE LAMBDA_EXP;

typedef struct {
    char *var;
    RESULT *val;
} Pair;

typedef struct {
    Pair p[1024];
    size_t length;
    size_t capacity;
} Env;


typedef struct {
    char *opt;
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
    int type;
    union u {
 int num;
        char *str;
        LAMBDA_EXP *lambda;
        THREE_EXP *three;
        APPLY_EXP *apply;
    } as;
} Exp;

Exp *apply2exp(APPLY_EXP *ae);

typedef struct CE {
    char *x;
    Exp *body;
    Env env;
} Closure;


typedef struct RE {
    int type;
    union u2 {
 int num;
 Closure closure;
    } as;
} RESULT;

Closure result2closure(RESULT re);
int result2int(RESULT re);
RESULT *interpreter(Exp exp, Env env);
Exp *calc(char *opt, Exp *n1, Exp *n2);
Exp *lambda(char *x, Exp *body);
Exp *apply(Exp *lamb, Exp *body);
Exp *num(int n);
Exp *str(char *x);
void pretty_print_apply(APPLY_EXP *apply);
void pretty_print_three(THREE_EXP *e);
void pretty_print_lambda(LAMBDA_EXP *le);
void pretty_print(Exp *e);
void print_closure(Closure closure);

void ignore(char *str);
int empty_envp(Env env);
Env init_env(void);
Env extend_env(char *var, RESULT *val, Env env);
void Error(char *p);
RESULT *lookup(char *var, Env env);

Exp *lambda2exp(LAMBDA_EXP *le);
char *closure_x(Closure closure);
Exp *closure_body(Closure closure);
Env closure_env(Closure closure);
Closure Close(char *arg, Exp *body, Env env);


#endif
