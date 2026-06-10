#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "constructor.h"

static int passed = 0, failed = 0;

#define TEST(name, body) do {                       \
    printf("  %-40s ", name);                       \
    int ok = 1;                                     \
    do { body; } while (0);                         \
    if (ok) { passed++; printf("✓\n"); }            \
    else     { failed++; printf("✗\n"); }           \
} while (0)

#define CHECK(cond) do {                            \
    if (!(cond)) { ok = 0; printf("\n    FAIL: %s", #cond); } \
} while (0)

/* ── 辅助：从字符串构建表达式并求值 ── */
#include "y.tab.h"
extern void *result;
extern int yyparse(void);
extern FILE *yyin;

static RESULT eval_str(const char *input) {
    yyin = fmemopen((void *)input, strlen(input), "r");
    if (!yyin) { perror("fmemopen"); exit(1); }
    int r = yyparse();
    if (r != 0) {
        RESULT err; err.type = RESULT_NONE_TYPE; return err;
    }
    Env env = init_env();
    return interpreter(*(Exp *)result, env);
}

int main(void) {
    printf("── Lambda 解释器测试 ──\n\n");

    /* 1. 数值 */
    TEST("数值字面量", {
        RESULT r = eval_str("42");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 42);
    });

    /* 2. 二元运算 */
    TEST("加法 (+ 1 2)", {
        RESULT r = eval_str("(+ 1 2)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 3);
    });

    TEST("减法 (- 5 3)", {
        RESULT r = eval_str("(- 5 3)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 2);
    });

    TEST("乘法 (* 3 4)", {
        RESULT r = eval_str("(* 3 4)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 12);
    });

    TEST("除法 (/ 10 3)", {
        RESULT r = eval_str("(/ 10 3)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 3);
    });

    /* 3. Lambda 应用 */
    TEST("简单应用 ((lambda (x) (+ x 1)) 3)", {
        RESULT r = eval_str("((lambda (x) (+ x 1)) 3)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 4);
    });

    TEST("恒等函数 ((lambda (x) x) 99)", {
        RESULT r = eval_str("((lambda (x) x) 99)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 99);
    });

    /* 4. 嵌套闭包（闭包捕获） */
    TEST("嵌套闭包 (((lambda (x) (lambda (y) (+ x y))) 1) 2)", {
        RESULT r = eval_str("(((lambda (x) (lambda (y) (+ x y))) 1) 2)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 3);
    });

    /* 5. 多层应用 */
    TEST("多层应用 ((lambda (x) (lambda (y) (+ x y))) 5 6)", {
        RESULT r = eval_str("(((lambda (x) (lambda (y) (+ x y))) 5) 6)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 11);
    });

    /* 6. 变量查找 */
    // 先构造抽象语法树：((lambda (x) x) 7)
    TEST("变量查找 ((lambda (x) x) 7)", {
        RESULT r = eval_str("((lambda (x) x) 7)");
        CHECK(r.type == INT);
        CHECK(result2int(r) == 7);
    });

    /* 7. Closure 类型检查 */
    TEST("lambda 求值得到 closure", {
        RESULT r = eval_str("(lambda (x) x)");
        CHECK(r.type == CLOSURE);
    });

    printf("\n── 合计: %d 通过, %d 失败 ──\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
