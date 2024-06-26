%option noyywrap
%{
#include "y.tab.h"
//extern int yylval;

%}
%%

quit             return FIM;
help        { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return HELP;}
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
touch          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return TOUCH;}
echo          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return ECHO;}
compile          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return COMPILE;}
talky          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return TALKY;}
life          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return LIFE;}
cp          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return CP;}
chat          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return CHAT;}   
mv          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return MV;}   
rm          { yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return RM;}   
"="[ a-zA-Z0-9./()+-./*]+    {  yylval.comval =malloc(yyleng+1); 
                            strcpy(yylval.comval, yytext); 
                            return CALC; }

[a-zA-Z0-9./-]+  { yylval.str = malloc(yyleng + 1); 
                    strcpy(yylval.str, yytext); 
                    return ARGS; }
\n               { return yytext[0]; }
.                { }
%%

