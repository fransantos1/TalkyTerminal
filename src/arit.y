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

// char[] commandlist[] = {
// 	"cd", "clear", "ls", "pwd", "mkdir", "touch", "echo", "compile", "talky", "life", "cp", "chat","mv", "rm"
// }

%}

%start programa
%token <comval> FIM CD CLEAR LS PWD MKDIR TOUCH ECHO COMPILE TALKY CP LIFE CHAT MV RM CALC
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
				system($1);
			}
	;

command: CD argument
			{cd($2);}
		| CLEAR argument
			{shell_clear($2);}
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
			{
				talky($2);
			}
		
		| LIFE argument
			{
				life($2);
			}
		| CP argument
			{shell_cp($2); }
		| RM argument
			{shell_rm($2); }
		| MV argument
			{shell_mv($2); }
		| CHAT argument
			{chat($2); }
		| CALC
			{calc($1);}
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

