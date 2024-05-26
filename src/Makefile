shell: arit.l arit.y shell/shell.c shell/shell.h shell/ChatRoom.c shell/ChatRoom.h
	byacc -d arit.y; flex arit.l
	gcc -o Talky shell/shell.c shell/ChatRoom.c y.tab.c lex.yy.c -lncurses -lcurl -ljansson -lm


test:  shell/ChatRoom.c
	gcc shell/ChatRoom.c -o test