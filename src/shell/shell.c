#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#


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
    DIR *dir;
    struct dirent *dp;
    args = strtok(NULL, " ");
    char path[1024];
    path[0] = '\0';
    int isALL= 0;
    while (args != NULL) {
        if (strchr(args, '-') != NULL) {
            if(strchr(args, 'a') != NULL){
                isALL = 1;
            }else{
                printf("parameter not recognized\n");
                return;
            }
        } else {
            strcat(path, args);
        }
        args = strtok(NULL, " ");
    }
    if(strlen(path) == 0){
        dir = opendir(".");
    }else{
        dir = opendir(path);
    }
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    int counter = 0;
    while ((dp = readdir(dir)) != NULL) {
        if(isALL != 1 && counter <=1){
            counter ++; 
            continue;
             
        }
        printf("%s ", dp->d_name);
        counter ++;
    }
    printf("\n");
    closedir(dir);

}

//Clear the terminal screen.
void clear(){
    system("clear");
}


float sleepTime = 500; //in microseconds 1ms = 1000us
void test(){
    int x = 10;
    int y = 10;
    int animation[x][y];
    int counter=0;
    for(int i = 1; i<=11;i++){    
        for (int l = 0; l<10;l++ ){
            for(int j = 0;j<10;j++){
                animation[l][j]=counter;
                printf("[%d]",animation[l][j]);
                fflush(stdout);
                usleep(sleepTime);
            }
            printf("\n");
        }
        if(i != 11){
            for(int m = 0; m<10;m++){
            printf("\033[A");
            }
        }
        
        counter = i;
    }
    printf("\n");
    return;
}
//pwd: Print working directory.
void pwd(){
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("%s\n", cwd);
	} else {
		perror("getcwd() error");
	}
}
struct stat st = {0};
//mkdir: Make directory.
void newmkdir(char *name){
    name = strtok(NULL, " ");
    int result = mkdir(name, 0777);
    if(result != 0){
        perror("mkdir");
    }
}

//rm: Remove files or directories.
//mv: Move (rename) files or directories.
//cp: Copy files or directories.
//touch: Create an empty file.
//cat: Concatenate and display files.
//echo: Print text or variables to the terminal.
//history: Display command history.

/*
    printf("\033[A");
    printf("\r");
    printf("\033[1;1H");//move cursor to x=1,y=1; so we need to create a "virtual canvas, from the y we begin or it glitches"
*/