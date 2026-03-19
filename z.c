#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ast.h"
#include "constructor.h"

int main() {
    Exp *e = calc("+", num(1), num(2));
    Exp *e1 = apply(lambda("x", calc("+", str("x"), num(1))), num(1));
    Exp *e2 = apply(apply(lambda("x", lambda("y", calc("+", str("x"), str("y")))), num(1)), num(2)); 
    Exp *e3 = lambda("x", str("x"));

    Env ne;
    ne = init_env();
    RESULT *re = interpreter(*e, ne);
    int result = result2int(*re);
    // printf("%d", result);
    pretty_print(e2);
    return 0;
}

