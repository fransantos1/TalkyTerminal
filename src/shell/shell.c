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


typedef struct {
    int isOccupied;
} square;

void life(char *in_token) {
    refresh();
    MEVENT event;
    int height, width;
    initscr();                  // Initialize the window
    cbreak();                   // Line buffering disabled
    noecho();                   // Don't echo input
    keypad(stdscr, TRUE);       // Enable keypad mode
    mousemask(ALL_MOUSE_EVENTS, NULL); // Enable mouse events
    curs_set(0);                // Hide the cursor
     
    getmaxyx(stdscr, height, width);
    square map[height][width];

    // Initialize the map
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            map[i][j].isOccupied = 0;  // Assuming 0 is the default state
        }
    }

    clear();  // Clear the screen at the beginning
    refresh();
    int ch;
    while ((ch = getch()) != 'q') {  // Exit loop on 'q' key
        if (ch == '\n' || ch == EOF) {
            nodelay(stdscr, TRUE); 
                while (1) { 
                    ch = getch();  
                    if (ch == 'q') { 
                        break;
                    }
                square tempMap[height][width];
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        tempMap[i][j].isOccupied = map[i][j].isOccupied;
                    }
                }

                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        int counter = 0; // SEE neighbours
                        for (int n = i - 1; n <= i + 1; n++) {
                            for (int m = j - 1; m <= j + 1; m++) {
                                if (n == i && m == j) continue; 
                                if (n >= 0 && n < height && m >= 0 && m < width) {
                                    if (map[n][m].isOccupied == 1) {
                                        counter++;
                                    }
                                }
                            }
                        }
                        if (map[i][j].isOccupied == 0 && counter == 3) {
                            tempMap[i][j].isOccupied = 1;
                        } else if (map[i][j].isOccupied == 1 && (counter < 2 || counter > 3)) {
                            tempMap[i][j].isOccupied = 0;
                        }
                    }
                }
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        if (map[i][j].isOccupied != tempMap[i][j].isOccupied) {
                            map[i][j].isOccupied = tempMap[i][j].isOccupied;
                            mvprintw(i, j, map[i][j].isOccupied ? "X" : " ");
                        }
                    }
                }
                refresh();
                usleep(250000);
            }
            break;
        }
		
        if (ch == KEY_MOUSE) {
            if (getmouse(&event) == OK) {
                if (event.bstate & BUTTON1_CLICKED) {
                    if (event.y >= 0 && event.y < height && event.x >= 0 && event.x < width) {
                        map[event.y][event.x].isOccupied = 1;
                        mvprintw(event.y, event.x, "X");
                    }
                }
                if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                    if (event.y >= 0 && event.y < height && event.x >= 0 && event.x < width) {
                        map[event.y][event.x].isOccupied = 0;
                        mvprintw(event.y, event.x, " ");
                    }
                }
            }
        }
       
    }
    refresh();
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

void talky(char *args) {
    char *ptr = strtok(args, " ");
    while (ptr != NULL) {
        if (strcmp(ptr, "--help") == 0) {
            printf("Help requested:\n\033[31mtalky  [arg]\ntalky  -s      host\ntalky  -c      connect\033[0m\n");
            return;
        } else if (ptr[0] == '-') {
            if (strcmp(ptr, "-s") == 0) {
                createChat();
            } else if (strcmp(ptr, "-c") == 0) {
                joinChat();
            } else {
                printf("Parameter '%s' not recognized\n", ptr);
                return;
            }
        } else {
            printf("Invalid argument: %s\n", ptr);
            return;
        }
        ptr = strtok(NULL, " ");
    }
}

struct FileType {
    const char *extension;
    const char *type;
    const char *compile;
    const char *execute;
};
char* replaceWord(const char* s, const char* oldW, 
                const char* newW){ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
            i += oldWlen - 1; 
        } 
    } 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
    i = 0; 
    while (*s) { 
        if (strstr(s, oldW) == s) { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
    result[i] = '\0'; 
    return result; 
} 
void compile(char *filename) {
    DIR *dir;
    struct dirent *entry;
    char extension[256] = "";
    char filename_copy[256];
    char extension_list[256][256];
    int extension_count = 0; 
    int filename_found = 0;
    int extension_found = 0;
    size_t suffix_found_length = 0;
    char suffix_found[256] = "";
    char command[MAX_COMMAND_LEN];
    int result = 0;
    char full_filename[1024];
    char args[200];     
    char *space_position = strchr(filename, ' ');
    if (space_position != NULL) {
        strcpy(args, space_position + 1);
        char filename_before_space[256];
        strncpy(filename_before_space, filename, space_position - filename);
        filename_before_space[space_position - filename] = '\0';
        strcpy(filename, filename_before_space);
    }
    for (int i = 0; i < 256; ++i) {
        extension_list[i][0] = '\0';
    }
    if (!strlen(extension)) {
        dir = opendir(".");
        if (!dir) {
            perror("opendir");
            return;
        }
        const char *dot_position = strrchr(filename, '.');
        if (dot_position != NULL) {
            size_t length = dot_position - filename;
            strncpy(full_filename, filename, length);
            full_filename[length] = '\0';
            size_t suffix_length = strlen(dot_position + 1);
            strncpy(suffix_found, dot_position + 1, suffix_length);
            suffix_found[suffix_length] = '\0';
        } else {
            strcpy(full_filename, filename);
        }
        while ((entry = readdir(dir)) != NULL) {
            strcpy(filename_copy, entry->d_name);
            const char *dot_position = strrchr(filename_copy, '.');
            if (dot_position != NULL && strcmp(filename, filename_copy) == 0){
                size_t suffix_length = strlen(dot_position + 1);
                char suffix[suffix_length + 1];
                strcpy(suffix, dot_position + 1);
                strcpy(extension, suffix);
                strcpy(extension_list[extension_count++], suffix);
                filename_found = 1;
                break;
            }
            else if (dot_position != NULL) {
                size_t length = dot_position - filename_copy;
                char prefix[length + 1];
                strncpy(prefix, filename_copy, length);
                prefix[length] = '\0';
                if (strcmp(filename, prefix) == 0) {
                    size_t suffix_length = strlen(dot_position + 1);
                    char suffix[suffix_length + 1];
                    strcpy(suffix, dot_position + 1);
                    strcpy(extension, suffix);
                    strcpy(extension_list[extension_count++], suffix);
                    filename_found = 1;
                }
            }
        }
        closedir(dir);
    }
    struct FileType fileTypes[] = {
        {"c", "C file", "gcc -o file file.c", "./file"}, 
        {"cs", "Visual C# file", "mcs -out: file.exe file.cs", "mono file.exe"},
        {"java", "Java file", "javac file.java", "java Main"},
        {"py", "Python file", "", "python file.py"},
        {"js", "Javascript file", "", "node file.js" },
        {"h", "H file", "", ""},
        {"php", "Hypertext Preprocessor script file", "", "php file.php"},
        {"swift", "Swift file", "", ""},
        {"vb", "Visual Basic file", "", ""},
        {"doc", "Word document file", "", ""},
        {"docx", "Word document file", "", ""},
        {"html", "HTML file", "", ""},
        {"html", "HTML file", "", ""},
        {"pdf", "PDF file", "", ""},
        {"txt", "Text file", "", ""},
        {"rtf", "Rich Text Format file", "", ""}
    };
    const char *dot_position = strrchr(filename, '.');
    if (dot_position != NULL) {
        size_t length = dot_position - filename;
        strncpy(full_filename, filename, length);
        full_filename[length] = '\0';
    } else {
        strcpy(full_filename, filename);
    }
    if (extension_count > 1){
        extension_found = 1;
        printf("%d", extension_count);
        printf(" files found \n");
        for (size_t i = 0; i < extension_count; i++){
            printf(full_filename);
            printf(".");
            printf(extension_list[i]);
            printf("\n");
        }
        printf("please try again with the specific file\n");
    }else{
        for (size_t j = 0; j < sizeof(fileTypes) / sizeof(fileTypes[0]); ++j) {
            if (strcmp(extension_list[0], fileTypes[j].extension) == 0) {
                extension_found = 1;
                char part[1024];
                strcpy(part, full_filename);
                if(args == NULL){ printf("args is null");}
                if(strcmp(args, "-l") == 0){
                    strcat(full_filename, ".");
                    strcat(full_filename, fileTypes[j].extension);
                    strcat(full_filename, " this is a ");
                    strcat(full_filename, fileTypes[j].extension);
                    strcat(full_filename, "\n");
                    printf(full_filename);
                }else if(fileTypes[j].compile == NULL && fileTypes[j].execute == NULL &&
                        (strcmp(args, "") == 0 || strcmp(args, "-c") == 0 || strcmp(args, "-e") == 0)){
                    strcpy(part, full_filename);
                    strcat(full_filename, ".");
                    strcat(full_filename, fileTypes[j].extension);
                    strcat(full_filename, " this is a ");
                    strcat(full_filename, fileTypes[j].extension);
                    strcat(full_filename, " file at the momment we are unable to compile this type of file \n");
                    printf(full_filename);
                }else if(strcmp(args, "") == 0 || strcmp(args, "-c") == 0 || strcmp(args, "-e") == 0){
                    char command[1024];
                    strcpy(command, fileTypes[j].compile);
                    if (strcmp(fileTypes[j].compile, "") != 0) {
                        strcpy(command, fileTypes[j].compile);
                        char* resulte  = replaceWord(command,"file", part);
                        int result = system(resulte);
                        if (result == 0) {
                            printf("Compilation successful\n");
                        } 
                    }else if( strcmp(args, "-e") == 0){
                        strcpy(command, fileTypes[j].execute);
                        char* resulte  = replaceWord(command,"file", part);
                        int result = system(resulte);
                        if (result == 0) {
                            printf("Execution successful\n");
                        }
                    }else{printf("File can only be executed not compiled try adding -e at the end\n");}
                }else{
                    printf("unknown option please try again\n");
                }                 
            }        
        }
    }
    if(filename_found == 0){printf("No matching file found\n");} 
    if (extension_found == 0){printf("Unknown extension\n");}
}