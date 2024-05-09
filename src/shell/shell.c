#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ncurses.h>


#include "shell.h"
#include "ChatRoom.h"

//#include <utime.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 10


void life(char *in_token) {
    MEVENT event;

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);

    int ch;
    while ((ch = getch()) != 'q') {  // Exit loop on 'q' key
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_CLICKED) {
                    printf("Mouse clicked at (%d, %d)\n", event.x, event.y);
                }
            }
        }
    }
    // Cleanup
    endwin();
}




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

//Clear the terminal screen.

//! Ncurses has a function for clear
void shell_clear(char *args){
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

void shell_cp(const char *args){
    if(strlen(args)> MAX_COMMAND_LEN - 8){
        printf("Input excedes character limit.\n");
        return;
    }

    char command[MAX_COMMAND_LEN];
    strcpy(command, "cp ");
    strcat(command, args);

    int result = system(command);
    if(result == -1){
        perror("cp");
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

/*
    talky [arg]
    talky  -h      host
    talky  -c      connect
*/


 


void talky(char *args){
    if(args[0] == '\0'){
        printf("Not recognized:\n\n\033[31mtalky  [arg]\ntalky  -s      host\ntalky  -c      connect\033[0m\n");
        return;
    }
    char *ptr = strtok(args, " ");
    while (ptr != NULL) {
        if (strchr(ptr, '-') != NULL) {
            if(strchr(ptr, 's') != NULL){
                createChat();
            }else if(strchr(ptr, 'c') != NULL){
                joinChat();

            }else{
                printf("parameter not recognized\n");
                return;
            }
        }
        ptr = strtok(NULL, " ");
    }
    return;
    //    memmove(buffer, buffer + 1, strlen(buffer));

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
    char extension_list[256][256]; // Array to store extensions
    int extension_count = 0; // Counter for extensions
    int filename_found = 0;
    int extension_found = 0;
    size_t suffix_found_length = 0;
    char suffix_found[256] = "";
    char command[MAX_COMMAND_LEN];

    // Initialize extension_list
    for (int i = 0; i < 256; ++i) {
        extension_list[i][0] = '\0';
    }

    if (!strlen(extension)) {
        dir = opendir(".");

        if (!dir) {
            perror("opendir");
            return;
        }
        const char *filename_dot = strrchr(filename, '.');
        if (filename_dot != NULL){   
            suffix_found_length = strlen(filename_dot + 1);
            strncpy(suffix_found, filename_dot + 1, suffix_found_length); // Copy the suffix to suffix_found
            suffix_found[suffix_found_length] = '\0'; // Null-terminate the string
        }
        while ((entry = readdir(dir)) != NULL) {
            strcpy(filename_copy, entry->d_name); // Make a copy of the filename;
            const char *dot_position = strrchr(filename_copy, '.');
            if (filename_dot != NULL && strcmp(filename, filename_copy) == 0){
                size_t suffix_length = strlen(filename_dot + 1);
                // Create a buffer to store the substring after the dot
                char suffix[suffix_length + 1]; // +1 for null-terminator
                strcpy(suffix, filename_dot + 1);
                // Print the part after the dot
                strcpy(extension, suffix);
                // Add extension to the extension_list
                strcpy(extension_list[extension_count++], suffix);
                filename_found = 1;
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
                    // Add extension to the extension_list
                    strcpy(extension_list[extension_count++], suffix);
                    filename_found = 1;
                }
            }
        }
        
        closedir(dir);
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

        if(strcmp(suffix_found, fileTypes[i].extension) == 0){extension_found = 1;}
        for (size_t j = 0; j < extension_count; j++){
            if (strcmp(extension_list[j], fileTypes[i].extension) == 0) {
                 printf("This is a %s\n", fileTypes[i].type);
                char full_filename[1024]; // Assuming maximum filename length of 1023 characters
                const char *dot_position = strrchr(filename, '.');
                if (dot_position != NULL) {
                    size_t length = dot_position - filename;
                    strncpy(full_filename, filename, length);
                    full_filename[length] = '\0';
                } else {
                    strcpy(full_filename, filename);
                }
                char part[1024]; // Assuming maximum filename length of 1023 characters
                strcpy(part, full_filename);
                strcat(full_filename, ".");
                strcat(full_filename, fileTypes[i].extension);
                printf("This file is named: %s\n", full_filename);
                int result = 0;
                if(strcmp(fileTypes[i].extension, "c") == 0){
                    strcpy(command, "gcc -o ");
                    strcat(command, part);
                    strcat(command, " ");
                    strcat(command, part);
                    strcat(command, ".c");
                    result = system(command);
                }else if(strcmp(fileTypes[i].extension, "cs") == 0){
                    strcpy(command, "mcs -out:");
                    strcat(command, part);
                    strcat(command, ".exe ");
                    strcat(command, part);
                    strcat(command, ".cs");
                    result = system(command);
                }else if(strcmp(fileTypes[i].extension, "java") == 0){
                    strcpy(command, "javac ");
                    strcat(command, part);
                    strcat(command, ".java");
                    result = system(command);
                }else if(strcmp(fileTypes[i].extension, "py") == 0){
                    strcpy(command, "python ");
                    strcat(command, part);
                    strcat(command, ".py");
                    result = system(command);
                    if(result == -1){
                        strcpy(command, "python ");
                        strcat(command, part);
                        strcat(command, ".py");
                        int result = system(command);
                    }
                }
                if(result == -1){
                    printf("Compilation failed\n");
                } else {
                    printf("Compilation successful\n");
                }
                
                extension_found = 1;
            }
        }        
    }
    if(filename_found == 0){printf("No matching file found\n");} 
    if (extension_found == 0){printf("Unknown extension\n");}
}

