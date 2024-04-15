%union{
	char* str;
}
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "shell/shell.h"
extern int yylex();
%}

%start programa
%token FIM
%token <str> COMMAND
%%

programa: 
		|	programa linha
		;

linha: '\n'
		{printf("NEW LINE\n");}
	|	FIM '\n'
			{exit(0);}
	| 	COMMAND
			{	
				char* token = strdup($1);
				if(strcmp(strtok(token, " "), "cd") == 0){
					while (*$1 != 0 && *($1++) != ' ') {}
					cd($1, 2);
				}
			}
	;

%%
int yyerror(char* s){
	printf("Erro de sintaxe\n");
}

int main(){
	yyparse();
}
