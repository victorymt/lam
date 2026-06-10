#ifndef Z_INTERPRETER_
#define Z_INTERPRETER_

#include <stddef.h>

enum type { INT, STR, LAMBDA, THREE, APPLY, CLOSURE };
enum { RESULT_NONE_TYPE = -1 };

/* ── 前向声明 ── */
typedef struct EXP Exp;
typedef struct AE APPLY_EXP;
typedef struct LE LAMBDA_EXP;
typedef struct EnvNode *Env;   /* Env 是指针 —— 不完整类型就够用 */

/* ── 子表达式节点 ── */
typedef struct {
    char *opt;
    Exp *b1;
    Exp *b2;
} THREE_EXP;

typedef struct AE {
    Exp *fun;
    Exp *body;
} APPLY_EXP;

typedef struct LE {
    char *arg;
    Exp *body;
} LAMBDA_EXP;

/* ── 表达式 ── */
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

/* ── Closure：捕获一个函数和它的定义环境 ── */
typedef struct CE {
    char *x;        /* strdup'd */
    Exp *body;      /* AST 指针 */
    Env env;        /* 环境链表头指针 */
} Closure;

/* ── 运行时值：INT 或 CLOSURE ── */
typedef struct RE {
    int type;
    union u2 {
        int num;
        Closure closure;
    } as;
} RESULT;

/* 查找失败的哨兵值 */
#define RESULT_NONE ((RESULT){ .type = RESULT_NONE_TYPE })

/* ── 环境节点：函数式链表（不可变语义） ── */
typedef struct EnvNode {
    char *var;        /* strdup'd，节点持有所有权 */
    RESULT val;       /* 值类型 —— 不存在所有权问题 */
    Env prev;         /* 外层环境 */
} EnvNode;

/* ── 环境操作 ── */
Env init_env(void);
Env extend_env(char *var, RESULT val, Env env);
RESULT lookup(char *var, Env env);
void Error(char *p);

/* ── 解释器 ── */
RESULT interpreter(Exp exp, Env env);
int result2int(RESULT re);
Closure result2closure(RESULT re);

/* ── Closure 工具 ── */
Closure Close(char *arg, Exp *body, Env env);
char *closure_x(Closure closure);
Exp *closure_body(Closure closure);
Env closure_env(Closure closure);

/* ── AST 构造器（在 constructor.h） ── */
Exp *apply2exp(APPLY_EXP *ae);
Exp *lambda2exp(LAMBDA_EXP *le);

/* ── 打印 ── */
void pretty_print(Exp *e);

#endif
