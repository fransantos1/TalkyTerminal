%option noyywrap
%{
#include "y.tab.h"
//extern int yylval;

%}
%%

FIM             return FIM;
cd          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return CD;}
clear      { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return CLEAR;} 
ls          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return LS;}
pwd         { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return PWD;}
mkdir       { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return MKDIR;}
[a-zA-Z0-9./-]+    { yylval.str =malloc(yyleng+1); 
                            strcpy(yylval.str, yytext); 
                            return ARGS;}
\n              return yytext[0];
.   { }
