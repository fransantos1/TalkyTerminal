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
#include <math.h>

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
            printf("\n");
            printf("    \033[33mConway's Game of Life\033[0m\n");
            printf("      is a zero-player game where cells on a grid evolve based on simple rules involving their neighbors' states.\n\n");
            printf("        \033[31mRules:\033[0m\n");
            printf("            -Any live cell with fewer than two live neighbors dies, as if by underpopulation.\n");
            printf("            -Any live cell with two or three live neighbors lives on to the next generation.\n");
            printf("            -Any live cell with more than three live neighbors dies, as if by overpopulation.\n");
            printf("            -Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.\n\n");
            printf("        \033[33mHow to play:\033[0m\n");
            printf("            -mouse to place a cell\n");
            printf("            -double click to remove\n");
            printf("            -start with enter\n");
            printf("            -press 'q' whenever to leave the game \033[0m\n\n");
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
            printf("\n");
            printf("    \033[33mTalky\033[0m\n");
            printf("      is a peer to peer chat room, where you either connect or create your chat, and other people join, with your ip.\n\n");
            printf("        \033[31mtalky  [arg]\033[0m\n");
            printf("        talky  -s   \033[33mServer\033[0m\n");
            printf("        talky  -c   \033[33mClient\033[0m\n");
            printf("\n");
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
char* replaceWord(const char* s, const char* oldW,const char* newW){ 
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
void compile(char *args) {
    char *ptr = strtok(args, " ");
    int option = -1; //0 List, 1 Compile, 2 Execute
    while (ptr != NULL) {
        if (strcmp(ptr, "--help") == 0) {
            printf("\n");
            printf("    \033[33mCompile\033[0m\n");
            printf("      is a tool that lets you compile/run or just identify a file.\n\n");
            printf("        \033[31mcompile [arg] [filename]\033[0m\n");
            printf("        compile -l filename  \033[33mList\033[0m\n");
            printf("        compile -c filename  \033[33mCompile\033[0m\n");
            printf("        compile -e filename  \033[33mCompile and Execute\033[0m\n");
            printf("\n");
            return;
        } else if (ptr[0] == '-') {
            if (strcmp(ptr, "-l") == 0) {
                option = 0;
            } else if (strcmp(ptr, "-c") == 0) {
                option = 1;
            } else if (strcmp(ptr, "-e") == 0) {
                option = 2;
            } else if (strcmp(ptr, "-ai") == 0) {
                option = 3;
            } else {
                printf("Parameter '%s' not recognized\n", ptr);
                return;
            }
            ptr = strtok(NULL, " ");
            if(ptr == NULL){
                printf("Wrong Usage: File not specified\n", ptr);
                return;
            }
            break;
        } else {
            printf("Wrong Usage: Mode not specified\n", ptr);
            return;
        }
        ptr = strtok(NULL, " ");
    }

    char *filename = ptr;
    DIR *dir;
    struct dirent *entry;
    char extension[256] = "";
    char filename_copy[256];

    char extension_list[256][256];
    for (int i = 0; i < 256; ++i) {
        extension_list[i][0] = '\0';
    }
    int extension_count = 0; 

    int filename_found = 0;
    int extension_found = 0;
    size_t suffix_found_length = 0;
    char suffix_found[256] = "";
    char command[MAX_COMMAND_LEN];
    int result = 0;
    char full_filename[1024]; 
    char text [4080];

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

    if(filename_found == 0){
        printf("No matching file found\n");
        return;
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
        printf("%d files found\n", extension_count);
        for (size_t i = 0; i < extension_count; i++){
            printf(full_filename);
            printf(".");
            printf(extension_list[i]);
            printf(" ");
        }
        printf("\nplease try again with the specific file\n");
        return;
    }

    for (size_t j = 0; j < sizeof(fileTypes) / sizeof(fileTypes[0]); ++j) {
        if (strcmp(extension_list[0], fileTypes[j].extension) != 0) 
            continue;

        extension_found = 1;
        char part[1024];
        strcpy(part, full_filename);
        if(option == 0){
            strcat(full_filename, ".");
            strcat(full_filename, fileTypes[j].extension);
            strcat(full_filename, " this is a ");
            strcat(full_filename, fileTypes[j].extension);
            strcat(full_filename, "\n");
            printf(full_filename);
            return;
        }
        if(option == 3){
            strcat(full_filename, ".");
            strcat(full_filename, extension_list[0]);
            strcpy(text, "Can you tell me the extension of the file ");
            strcat(text, full_filename);
            strcat(text, "and what type of file it is?");
            chat(text);
            return;
        }
        if(fileTypes[j].compile == ""){
            strcpy(part, full_filename);
            strcat(full_filename, ".");
            strcat(full_filename, fileTypes[j].extension);
            strcat(full_filename, " this is a ");
            strcat(full_filename, fileTypes[j].extension);
            strcat(full_filename, " file at the momment we are unable to compile this type of file \n");
            printf(full_filename);
            return;
        }
        char command[1024];
        strcpy(command, fileTypes[j].compile);
        char* resulte  = replaceWord(command,"file", part);
        int result = system(resulte);
        if (result == 0) {
            printf("Compilation successful\n");
        } 
            
        
        if(option == 2){
            if(fileTypes[j].execute == ""){
                strcpy(part, full_filename);
                strcat(full_filename, ".");
                strcat(full_filename, fileTypes[j].extension);
                strcat(full_filename, " this is a ");
                strcat(full_filename, fileTypes[j].extension);
                strcat(full_filename, " file at the momment we are unable to compile this type of file \n");
                printf(full_filename);
                return;
            }
            strcpy(command, fileTypes[j].execute);
            char* resulte  = replaceWord(command,"file", part);
            int result = system(resulte);
            if (result == 0) {
                printf("Execution successful\n");
            }
        }                
    }        
    if(extension_found == 0){printf("Unknown extension\n");} 
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

void chat(char *args) {
    CURL *curl;
    CURLcode result;
    //const char *token = getenv("access_token");
    const char *token="ya29.a0AXooCgu9sfDA2TNFZQwOvH_C-DNh43RtEhpZVuTDhsfC0dO0A13UdxzJa5uDuBeqcWmorub9tKB0i-1VLLTt2K5lANAH7U0wKmSzd2B5Hsej0trJ5z0uyef5fuymdrbuJbwBcjMwhykYaS5hi9agKsiDenjES4eDipsaCgYKAVgSARISFQHGX2MikxhBIF8gal2Ghu8tw3ZggA0170";
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

    sprintf(post_fields, "{\"contents\": [{\"role\": \"user\",\"parts\": [{\"text\": \"%s\"},]},],\"generationConfig\": {\"maxOutputTokens\": 2048,\"temperature\": 0.9,\"topP\": 1,}}", args);

    //para chatgpt
    //"{\"prompt\": \"%s\", \"max_tokens\": 60}"
    //curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/engines/gpt-3.5-turbo/completions");

    //sprintf(api_url, "https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateContent?key=%s", token);

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
    }else{

    //https://forkful.ai/pt/c/data-formats-and-serialization/working-with-json/

    const char* json_string = chunk.memory;
    json_error_t error;
    json_t *root;
    char *res = malloc(2048*8+1); //+1 for null terminator
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
    }
    
    free(chunk.memory);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    post_fields[0] = '\0';
    buffer[0] = '\0';
}




void func(char *args){
    args = args + 1; // Removing the "="
    int option = -0; // 1 -sin, 2-cos, 3-tan
    if(args[0] == ' '){
        char *ptr = strtok(args, " ");
        if(ptr == NULL || strcmp(ptr, "--help") != 0){
            printf("invalid expression\n");
            return;
        }
        printf("\n");
        printf("    \033[33m\"=\"\033[0m\n");
        printf("      is a graph visualization tool.\n\n");
        printf("        \033[31m=[number]\033[0m\n");
        printf("            1- \033[33msin(x)\033[0m\n");
        printf("            2- \033[33mcos(x)\033[0m\n");
        printf("            3- \033[33mtan(x)\033[0m\n");
        printf("            4- \033[33m2x\033[0m\n");
        printf("            5- \033[33mx^2 - 4x + 4\033[0m\n");
        printf("            6- \033[33mx^3 - 3x^2 + 2\033[0m\n");
        printf("            7- \033[33me^x\033[0m\n");
        printf("            8- \033[33m|x|\033[0m\n");
        printf("\n");


        return;
    }
    option = atoi(args);
    if(option ==0){
        printf("Invalid function send only \"= --help\". for equations\n");
        return;
    }
    MEVENT event;
    int height, width;
    initscr();                  // Initialize the window
    cbreak();                   // Line buffering disabled
    noecho();                   // Don't echo input
    curs_set(0);                // Hide the cursor
    clear();
    getmaxyx(stdscr, height, width);
    int counter =0;
    for(int i = 0; i < height; i++) {
        mvprintw(i, width / 2, "|");
    }
    for(int j = 0; j < width; j++) {
        mvprintw(height / 2, j, "-");
    }
    for (int x = 0; x < width; x++) {
        double newx = (double)((x - width / 2) * 4.0 * 3.14159265 / width);
        double y;
        switch (option) {
            case 1: 
                y =  sin(newx);
                break;
            case 2: 
                y = cos(newx);
                break;
            case 3: 
                y = tan(newx);
                break;
            case 4: 
                y = (2 * newx) ;
                break;
            case 5:
                y = pow(newx,2) ;
                break;
            case 6:
                y = pow(newx,3)- 3 * pow(newx,2) + 2;
                break;
            case 7:
                y = exp(newx) ;
                break;
            case 8: 
                y = fabs(newx);
                break;
        }
        int real_y = (int)(height / 2 - (y * height / 4.0));
        if (real_y >= 0 && real_y < height) {
            mvprintw(real_y, x, "*");
            usleep(1000);
            refresh();
        }
    }
    while(getch() != 'q'){}
    refresh();
    signal(SIGWINCH, SIG_IGN);
    endwin(); 
}