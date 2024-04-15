#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"
#include <dirent.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 10


//Change directory.
void cd(char *in_token){
    char *token = in_token;
    int counter = 0;
    char*  arg;
    while( token != NULL ) {
        counter ++;
        if(counter == 2){
            arg = token;
            break;
        }
        token = strtok(NULL, " ");
    }
    if (counter != 2) {
        printf("Usage: cd <directory>\n");
        return;
    }
    if (chdir(arg) != 0) {
            perror("cd");
        }
    return;
}

//List directory contents.
/*
    ls [arg] [file]
    ls              display corrent files without including ". .." 
    ls -a,          display every files including ". .." 
    ls       /.     display files on that path(if the folder starts on the root, follow root if not display files in the next folders)    

*/
void ls(char *args){
    //if there are no args, run only ls
    //seperate this even more
    //if there are 2
    //see if its -a, if not error
    //if there is only one, see if it has /
    //if it has / is a path
    //if not is -a and this directory
    //if its a path, verify if it starts from the root or not
    




    DIR *dir;
    struct dirent *dp;
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((dp = readdir(dir)) != NULL) {
        printf("%s ", entry->d_name);
    }
    printf("\n");
    closedir(dir);


}

//Clear the terminal screen.
void clear(){
    system("clear");
}

/*
pwd: Print working directory.
mkdir: Make directory.
rm: Remove files or directories.
cp: Copy files or directories.
mv: Move (rename) files or directories.
touch: Create an empty file.
cat: Concatenate and display files.
echo: Print text or variables to the terminal.
history: Display command history.

*/