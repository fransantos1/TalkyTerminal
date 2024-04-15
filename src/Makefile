shell: arit.l arit.y shell/shell.c shell/shell.h
	flex arit.l
	byacc -d arit.y
	gcc -o arit shell/shell.c y.tab.c lex.yy.c  -o shell
	./shell.exe

test: shell/shell.c shell/shell.h shell/main.c
	gcc shell/main.c shell/shell.c -o test