%union{
	char* comval;
	char* str;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "shell/shell.h"
#include <string.h>
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
%token <comval> FIM CD CLEAR LS PWD MKDIR TOUCH ECHO COMPILE TALKY
%token <str> ARGS 
%type <str> argument
%type <comval> command
%%

programa: 
		|	programa linha
		;

linha: 	'\n'
			{printcwd();}
		| FIM '\n'
				{exit(0);}
		| command
		| argument
			{
				test();
				printf("Command not recognized type /help for commands x3c \n");
			}
	;

command: CD argument
			{cd($2);}
		| CLEAR argument
			{clear($2);}
		| LS argument
			{shell_ls($2);}
		| PWD argument
			{pwd($2);}
		| MKDIR argument
			{newmkdir($2); }
		| TOUCH argument
			{ shell_touch($2); }
		| ECHO argument
			{ shell_echo($2); }
		| COMPILE argument
			{ compile($2); }
		| TALKY argument
			{}
		
        ;

argument: ARGS 	
			{ $$ = $1; } 
         | argument ARGS 
		 	{ $$ = strcat(strcat($1, " "), $2); } 
         | 
		 	{$$ = "";}
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

