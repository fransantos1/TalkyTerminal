#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>


#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 10


//Change directory.
void cd(char *in_token){
    if (in_token == NULL) {
        printf("Usage: cd <directory>\n");
        return;
    }
    if (chdir(in_token) != 0) {
        perror("");
    }
    return;
}

//List directory contents.
void shell_ls(char *args){
     if(strlen(args)> MAX_COMMAND_LEN){
        printf("invalid input");
        return;
    }

    char path[MAX_COMMAND_LEN];
    strcpy(path, "ls ");
    strcat(path, args);
    int result = system(path);
    if(result == -1){
        perror("ls");
    }
}


/*
    ls [arg] [file]
    ls              display corrent files without including ". .." 
    ls -a,          display every files including ". .." 
    ls       /.     display files on that path(if the folder starts on the root, follow root if not display files in the next folders)    
*/
// void shell_ls(char *args){
//     DIR *dir;
//     struct dirent *dp;
//     args = strtok(NULL, " ");
//     char path[1024];
//     path[0] = '\0';
//     int isALL= 0;
//     while (args != NULL) {
//         if (strchr(args, '-') != NULL) {
//             if(strchr(args, 'a') != NULL){
//                 isALL = 1;
//             }else{
//                 printf("parameter not recognized\n");
//                 return;
//             }
//         } else {
//             strcat(path, args);
//         }
//         args = strtok(NULL, " ");
//     }
//     if(strlen(path) == 0){
//         dir = opendir(".");
//     }else{
//         dir = opendir(path);
//     }
//     if (dir == NULL) {
//         perror("opendir");
//         return;
//     }
//     int counter = 0;
//     while ((dp = readdir(dir)) != NULL) {
//         if(isALL != 1 && counter <=1){
//             counter ++; 
//           continue;     
//         }
//         printf("%s ", dp->d_name);
//         counter ++;
//     }
//     printf("\n");
//     closedir(dir);
// }


//Clear the terminal screen.
void clear(char *args){
    if(strlen(args)> MAX_COMMAND_LEN){
        printf("invalid input");
        return;
    }
    char path[MAX_COMMAND_LEN];
    strcpy(path, "clear ");
    strcat(path, args);
    int result = system(path);
    if(result == -1){
        perror("clear");
    }
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
//mkdir: Make directory not finished
void newmkdir(char *name){
    int result = mkdir(name, 0777);
    if(result != 0){
        perror("mkdir");
    }
}


void shell_touch(char *args) {
    if(strlen(args) > MAX_COMMAND_LEN - 8) {
        printf("Input excedes character limit.\n");
        return;
    }
    char command[MAX_COMMAND_LEN];
    strcpy(command, "touch ");
    strcat(command, args);
    int result = system(command);
    if(result == -1) {
        perror("touch");
    }
}

void shell_echo(char *args){
    if(strlen(args)> MAX_COMMAND_LEN - 6){
        printf("Input excedes character limit.");
        return;
    }
    char command[MAX_COMMAND_LEN];
    strcpy(command, "echo ");
    strcat(command, args);
    //snprintf(command, sizeof(command), "echo \"%s\"", args);
    int result = system(command);
    if(result == -1){
        perror("echo");
    }
}

struct FileType {
    const char *extension;
    const char *type;
};

void compile(char *filename) {
    DIR *dir;
    struct dirent *entry;
    char extension[256] = ""; // Assuming maximum extension length of 255 characters
    char filename_copy[256]; // Assuming maximum filename length of 255 characters

    if (!strlen(extension)) {
        dir = opendir(".");

        if (!dir) {
            perror("opendir");
            return;
        }

        while ((entry = readdir(dir)) != NULL) {
            strcpy(filename_copy, entry->d_name); // Make a copy of the filename;
            const char *dot_position = strrchr(filename_copy, '.');
            const char *filename_dot = strrchr(filename, '.');
            if (filename_dot != NULL && strcmp(filename, filename_copy) == 0){
                size_t suffix_length = strlen(filename_dot + 1);
                // Create a buffer to store the substring after the dot
                char suffix[suffix_length + 1]; // +1 for null-terminator
                strcpy(suffix, filename_dot + 1);
                // Print the part after the dot
                strcpy(extension, suffix);
                break;
            }
            else if (dot_position != NULL) {
                // Calculate the length of the substring before the dot
                size_t length = dot_position - filename_copy;
                // Create a buffer to store the substring before the dot
                char prefix[length + 1]; // +1 for null-terminator
                strncpy(prefix, filename_copy, length);
                prefix[length] = '\0'; // Null-terminate the string
                // Compare the prefix with the original filename
                if (strcmp(filename, prefix) == 0) {
                    // Calculate the length of the substring after the dot
                    size_t suffix_length = strlen(dot_position + 1);
                    // Create a buffer to store the substring after the dot
                    char suffix[suffix_length + 1]; // +1 for null-terminator
                    strcpy(suffix, dot_position + 1);
                    // Print the part after the dot
                    strcpy(extension, suffix);
                    break;
                }
            }
        }
        
        closedir(dir);
    }

    if (!*extension) {
        printf("Unknown extension\n");
        return;
    }

    struct FileType fileTypes[] = {
        {"c", "C file"},
        {"h", "H file"},
        {"java", "Java file"},
        {"py", "Python file"},
        {"cs", "Visual C# file"},
        {"php", "Hypertext Preprocessor script file"},
        {"swift", "Swift file"},
        {"vb", "Visual Basic file"},
        {"doc", "Word document file"},
        {"docx", "Word document file"},
        {"html", "HTML file"},
        {"a", "i see the problem file"},
        {"htm", "HTML file"},
        {"pdf", "PDF file"},
        {"txt", "Text file"},
        {"rtf", "Rich Text Format file"}
    };

    for (size_t i = 0; i < sizeof(fileTypes) / sizeof(fileTypes[0]); ++i) {
        if (strcmp(extension, fileTypes[i].extension) == 0) {
            printf("This is a %s\n", fileTypes[i].type);
            printf("This file is named: %s\n", filename_copy);
            return;
        }
    }

    printf("No matching file found\n");
}




/*
void echo2(char *args){
    if(strlen(args)> MAX_COMMAND_LEN - 6){
        printf("IMPOSSIBLE ISN'T HE JUST A QI GATHERING JUNIOR HOW COULD HE DO THIS!?!");
        return;
    }

     if (strpbrk(input, ";|><&$`") != NULL) {
        printf("JUNIOR YOU DARE!?!\n");
        return;
    }
    char command[MAX_COMMAND_LEN];
    strcpy(command, "echo ");
    strcat(command, args);
    int result = system(command);
    if(result == -1){
        perror("echo");
    }
}
*/

//rm: Remove files or directories.
//mv: Move (rename) files or directories.
//cp: Copy files or directories.
//touch: Create an empty file.
//cat: Concatenate and display files.
//echo: Print text or variables to the terminal.
//history: Display command history.
//man : display manual (might not be possible)
//chatroom 
//autocorrect
//auto detect
//AI api
//visualização de graficos
/*
    printf("\033[A");
    printf("\r");
    printf("\033[1;1H");//move cursor to x=1,y=1; so we need to create a "virtual canvas, from the y we begin or it glitches"
*/