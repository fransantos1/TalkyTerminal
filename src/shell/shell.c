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
#include <signal.h>
#include <curl/curl.h>
#include <jansson.h>
#include "shell.h"
#include "ChatRoom.h"

//#include <utime.h>

#define MAX_COMMAND_LEN 1024
#define MAX_ARGS 10


typedef struct {
    int isOccupied;
} square;

void life(char *args) {
    char *ptr = strtok(args, " ");
    while (ptr != NULL) {
        if (strcmp(ptr, "--help") == 0) {
            printf("\033[33mConway's Game of Life\n");
            printf("\033[31mRules:\033[0m\n-Any live cell with fewer than two live neighbors dies, as if by underpopulation.\n");
            printf("-Any live cell with two or three live neighbors lives on to the next generation.\n");
            printf("-Any live cell with more than three live neighbors dies, as if by overpopulation.\n");
            printf("-Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.\n");
            printf("\033[31mHow to play:\033[0m\n-mouse to place a cell\n-double click to remove\n-start with enter\n-press 'q' whenever to leave the game \033[0m\n");
            return;
        } else {
            printf("Invalid argument");
            return;
        }

    }
    
    refresh();
    MEVENT event;
    int height, width;
    initscr();                  // Initialize the window
    cbreak();                   // Line buffering disabled
    noecho();                   // Don't echo input
    keypad(stdscr, TRUE);       // Enable keypad mode
    mousemask(ALL_MOUSE_EVENTS, NULL); // Enable mouse events
    curs_set(0);                // Hide the cursor
    nodelay(stdscr, FALSE); 
     
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
                        int counter = 0; // neighbours count
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
		

       
    }
    refresh();
    signal(SIGWINCH, SIG_IGN);
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

void shell_mv(char *args){
    if (args == NULL) {
        printf("Usage: mv <file/directory> <file/directory>, mv <directory>/<new name>, mv <file/directory> <new name> \n");
        return;
    }

     if(strlen(args)> MAX_COMMAND_LEN){
        printf("Arguments exceed maximum length!");
        return;
    }

    char path[MAX_COMMAND_LEN];
    strcpy(path, "mv ");
    strcat(path, args);
    int result = system(path);
    if(result == -1){
        perror("mv");
    }
}

void shell_rm(char *args){
    if (args == NULL) {
        printf("Usage: rm <file/directory> \n");
        return;
    }

     if(strlen(args)> MAX_COMMAND_LEN){
         printf("Arguments exceed maximum length!");
        return;
    }

    char path[MAX_COMMAND_LEN];
    strcpy(path, "rm ");
    strcat(path, args);
    int result = system(path);
    if(result == -1){
        perror("rm");
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
            printf("n\033[31mtalky  [arg]\ntalky  -s      host\ntalky  -c      connect\033[0m\n");
            return;
        } else if (ptr[0] == '-') {
            if (strcmp(ptr, "-s") == 0) {
                createChat();
                return;
            } else if (strcmp(ptr, "-c") == 0) {
                joinChat();
                return;
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
        char *ptr = strtok(args, " ");
        while (ptr != NULL) {
        if (strcmp(ptr, "--help") == 0) {
            printf("Help requested:\n\033[0mcompile  [filename]  -l      List\ncompile  [filename]  -c      Compile\ncompile  [filename]  -e      Execute\033[0m\n");
            return;
        }}
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
    if(filename_found == 0&& extension_found == 0){printf("Please try typing compile [filename] --help\n");} 
}

struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

void chat(char *args){
    CURL *curl;
    CURLcode result;
    int maxTokens = 2048;
    //const char *token = getenv("access_token");
    const char *token = "";
    char buffer[MAX_COMMAND_LEN];
    char post_fields[MAX_COMMAND_LEN];
    char api_url[MAX_COMMAND_LEN];

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl == NULL){
        perror("HTTP request failed");
    }
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(buffer, "Authorization: Bearer %s",token);
    headers = curl_slist_append(headers, buffer);

    sprintf(post_fields, "{\"contents\": [{\"role\": \"user\",\"parts\": [{\"text\": \"%s\"},]},],\"generationConfig\": {\"maxOutputTokens\": %d,\"temperature\": 0.9,\"topP\": 1,}}", args, maxTokens);

    curl_easy_setopt(curl, CURLOPT_URL, "https://europe-southwest1-aiplatform.googleapis.com/v1/projects/talkyterminal/locations/europe-southwest1/publishers/google/models/gemini-1.0-pro-001:streamGenerateContent");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    result = curl_easy_perform(curl);

    if(result != CURLE_OK){
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
        perror("Curl:");
        return;
    }
    //https://forkful.ai/pt/c/data-formats-and-serialization/working-with-json/
    const char* json_string = chunk.memory;
    json_error_t error;
    json_t *root;
    size_t index;
    char *res = malloc(maxTokens*8+1); //+1 for null terminator
    if (res == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    res[0] = '\0';
    root = json_loads(json_string, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return;
    }

    if (!json_is_array(root)) {
        json_decref(root);
        return;
    }

    // Iterate through the array
    for (size_t i = 0; i < json_array_size(root); i++) {
        json_t *item = json_array_get(root, i);
        json_t *candidates = json_object_get(item, "candidates");
        if (!json_is_array(candidates)) continue;

        for (size_t j = 0; j < json_array_size(candidates); j++) {
            json_t *candidate = json_array_get(candidates, j);
            json_t *content = json_object_get(candidate, "content");

            if (json_is_object(content)) {
                json_t *parts = json_object_get(content, "parts");

                if (json_is_array(parts)) {
                    for (size_t k = 0; k < json_array_size(parts); k++) {
                        json_t *part = json_array_get(parts, k);
                        json_t *text = json_object_get(part, "text");
                
                        if (json_is_string(text)) {
                            strcat(res, json_string_value(text));
                        }
                        json_decref(part);
                        json_decref(text);
                    }
                }
                json_decref(parts);
            }
            json_decref(candidate);
            json_decref(content);

        }
        json_decref(item);
        json_decref(candidates);
    }

    json_decref(root);
    printf("%s\n",res);
    free(res);
    free(chunk.memory);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    post_fields[0] = '\0';
    buffer[0] = '\0';
}