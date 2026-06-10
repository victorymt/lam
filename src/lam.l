%{
#include "y.tab.h"
%}

%%
[ \t\n]    {}
[0-9]+     { yylval.num = atoi(yytext); return T_NUM; }
[+*/-]     { yylval.opt = strdup(yytext); return T_OPT; }
[()\n]     { return yytext[0]; }
"lambda"   { return T_LAMBDA; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.id = strdup(yytext); return T_ID;}
.          { return 0;}
%%

int yywrap(void) {
    return 1;
}
