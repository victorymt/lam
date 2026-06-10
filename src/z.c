#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "constructor.h"
#include "y.tab.h"

extern void *result;

int main(void) {
    int r = yyparse();
    if (r == 0) {
        printf("Success\n");
        Env ne = init_env();

        printf("AST: ");
        pretty_print((Exp *)result);

        RESULT re = interpreter(*(Exp *)result, ne);
        if (re.type == INT) {
            printf("Result: %d\n", result2int(re));
        } else if (re.type == CLOSURE) {
            printf("Result: <closure>\n");
        } else {
            printf("Result: <error>\n");
        }
    }
    return 0;
}
