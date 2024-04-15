%option noyywrap
%{
#include "y.tab.h"
//extern int yylval;
%}

%%
[a-zA-Z0-9 .]+    { yylval.str =malloc(yyleng+1); 
                            strcpy(yylval.str, yytext); 
                            return COMMAND;}
FIM             return FIM;
\n              return yytext[0];
