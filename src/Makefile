shell: arit.l arit.y shell/shell.c shell/shell.h
	flex arit.l
	byacc -d arit.y
	gcc -o arit y.tab.c lex.yy.c shell/shell.c -o main
	./main.exe

test: shell/shell.c shell/shell.h shell/main.c
	gcc shell/main.c shell/shell.c -o test