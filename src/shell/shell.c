#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "shell.h"

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 10

void test(){
    printf("OLA");
}
void cd(char *args, int arg_count){
    printf("%s", args);
    if (arg_count != 2) {
        fprintf(stderr, "Usage: cd <directory>\n");
    } else {
        if (chdir(args) != 0) {
            perror("cd");
        }
    }
    return;
}


