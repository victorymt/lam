%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "constructor.h"

void *result;

extern int yylex(void);
void yyerror(const char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}

%}

%union {
    int num;
    char *opt;
    char *id;
    struct EXP *node;			/* ? */
}
%define parse.error verbose
%token	<num> T_NUM
%token	<id> T_ID
%token	<opt> T_OPT
%token T_LAMBDA
%type	<node>		exp lambda_exp apply_exp three_exp num_exp id_exp

%%
/* (lambda (x) body)
   (fun apply)
   (+ 1 2)
   1 */
program : exp { result = $1; }
        ;
exp     :       lambda_exp
       	|	apply_exp
	|	three_exp
	|	num_exp { $$ = $1; }
	|	id_exp
        ;
lambda_exp : '(' T_LAMBDA '(' T_ID ')' exp ')' { $$ = lambda($4, $6); }
           ;
apply_exp  : '(' exp exp ')' { $$ = apply($2, $3); }
           ;
three_exp  : '(' T_OPT  exp exp ')' { $$ = calc($2, $3, $4); }
           ;
num_exp    : T_NUM { $$ = num($1); }
           ;
id_exp     : T_ID { $$ = str($1); }
           ;
%%

int main() {
    int r = yyparse();
    if (r == 0) {
	printf("Success\n");
	Env ne;
	ne = init_env();
	RESULT *re = interpreter(*(Exp*)result, ne);
	int result = result2int(*re);
	printf("%d\n", result);
	// printf("%d", result);
	/* pretty_print(); */
    }
}
