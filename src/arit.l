%option noyywrap
%{
#include "y.tab.h"
//extern int yylval;

%}
%%
FIM             return FIM;
[a-zA-Z0-9 ./]+    { yylval.str =malloc(yyleng+1); 
                            strcpy(yylval.str, yytext); 
                            return COMMAND;}
\n              return yytext[0];
