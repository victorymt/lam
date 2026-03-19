#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "constructor.h"
#include "y.tab.h"

extern void *result;
int main() {
    Exp *e = calc("+", num(1), num(2));
    Exp *e1 = apply(lambda("x", calc("+", str("x"), num(1))), num(1));
    Exp *e2 = apply(apply(lambda("x", lambda("y", calc("+", str("x"), str("y")))), num(1)), num(2)); 
    Exp *e3 = lambda("x", str("x"));

    // pretty_print(e2);
        int r = yyparse();
    if (r == 0) {
	printf("Success\n");
	Env ne;
	ne = init_env();
	pretty_print(result);
	RESULT *re = interpreter(*(Exp*)result, ne);
	assert(re->type == INT);
	int result = result2int(*re);
	printf("%d\n", result);
    }
    return 0;
}

