%union{
	char* str;
}
%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "shell/shell.h"
extern int yylex();

void printcwd(){
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("\033[0;32m%s: \033[0;0m", cwd);
	} else {
		perror("getcwd() error");
	}
}
%}

%start programa
%token FIM 
%token <str> COMMAND
%%

programa: 
		|	programa linha
		;

linha: '\n'
		{ printcwd();}
	|	FIM '\n'
			{exit(0);}
	| 	COMMAND
			{	
				char* token = strdup($1);
				strtok(token, " ");
				if(strcmp(token, "cd") == 0){
					cd(token);
				}else if(strcmp(token, "clear") == 0){
					clear();
				}else if(strcmp(token,"ls") == 0){
					ls(token);
				}else if(strcmp(token,"pwd") == 0){
					pwd();
				}else if(strcmp(token,"mkdir") == 0){
					newmkdir(token);
				}else{
					//test();
					printf(" \033[0;31mCommand not found (p≧w≦q)\n");
				}
			}
	;

%%
int yyerror(char* s){
	printf("Erro de sintaxe\n");
}

int main(){
	printf("\033[0;35m////////////////////////////////////////////////////////////////////////////////////////////\n");
	printf("////////////////////////////////////////////////////////////////////////////////////////////\n");
	printf("///////////////////////////////// \033[0;33mWELCOME TO TalkyTerminal\033[0;35m /////////////////////////////////\n");
	printf("////////////////////////////////////////////////////////////////////////////////////////////\n");
	printf("////////////////////////////////////////////////////////////////////////////////////////////\n\n\n");
	printf("\033[0;32mFor help just type: help\n\n\033[0;0m");
	printcwd();
	yyparse();
	
}

