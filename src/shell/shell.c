#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
//#include <utime.h>

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
    const char *compile;
    const char *execute;
};

void replaceString(char *command, const char *part) {
    char *pos = command;
    char *file_pos = strstr(pos, "file");
    while (file_pos != NULL) {
        size_t prefix_length = file_pos - pos;
        char updated_command[1024]; // Adjust the size as needed
        strncpy(updated_command, pos, prefix_length);
        updated_command[prefix_length] = '\0'; // Null-terminate the string
        strcat(updated_command, part);
        pos = file_pos + strlen("file"); // Update pos to point after "file"
        strcat(updated_command, pos); // Concatenate the rest of the original string
        strcpy(command, updated_command); // Update the original string
        file_pos = strstr(pos, "file"); // Find the next occurrence of "file"
    }
}

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
    int result = 0;
    char full_filename[1024];
    char args[200];     
    char *space_position = strchr(filename, ' ');
    if (space_position != NULL) {
        // Copy the part after the space into args
        strcpy(args, space_position + 1);
        // Allocate memory for a new string to store the part before the space
        char filename_before_space[256];
        strncpy(filename_before_space, filename, space_position - filename);
        filename_before_space[space_position - filename] = '\0'; // Null-terminate the string
        // Copy the part before the space back into filename
        strcpy(filename, filename_before_space);
    }
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
        const char *dot_position = strrchr(filename, '.');
        if (dot_position != NULL) {
            size_t length = dot_position - filename;
            strncpy(full_filename, filename, length);
            full_filename[length] = '\0';

            // Extract and store the part after the dot in suffix_found
            size_t suffix_length = strlen(dot_position + 1);
            strncpy(suffix_found, dot_position + 1, suffix_length);
            suffix_found[suffix_length] = '\0';
        } else {
            strcpy(full_filename, filename);
        }
        while ((entry = readdir(dir)) != NULL) {
            strcpy(filename_copy, entry->d_name); // Make a copy of the filename;
            const char *dot_position = strrchr(filename_copy, '.');
            if (dot_position != NULL && strcmp(filename, filename_copy) == 0){
                size_t suffix_length = strlen(dot_position + 1);
                // Create a buffer to store the substring after the dot
                char suffix[suffix_length + 1]; // +1 for null-terminator
                strcpy(suffix, dot_position + 1);
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
                char part[1024]; // Assuming maximum filename length of 1023 character
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
                        replaceString(command, part);
                        int result = system(command);
                        if (result == 0) {
                            printf("Compilation successful\n");
                        } 
                    }else if( strcmp(args, "-e") == 0){
                        strcpy(command, fileTypes[j].execute);
                        replaceString(command, part);
                        int result = system(command);
                        if (result == 0) {
                            printf("Compilation successful\n");
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




/*
void shell_touch(char *args) {
    // Check if the length of the arguments exceeds the maximum command length
    if(strlen(args) > MAX_COMMAND_LEN - 8) {
        printf("Input exceeds character limit.\n");
        return;
    }
    // Open or create the file with write-only mode
    int fd = open(args, O_WRONLY | O_CREAT, 0644);
    if(fd == -1) {
        perror("open");
        return;
    }
    // Close the file descriptor
    if(close(fd) == -1) {
        perror("close");
        return;
    }
    printf("File '%s' created or modified.\n", args);
}*/
/*
void shell_touch(char *args) {
    if(strlen(args) > MAX_COMMAND_LEN - 8) {
        printf("Input exceeds character limit.\n");
        return;
    }

    // Extract options from args
    char options[4] = ""; // Assuming maximum of 3 options plus null terminator
    char filename[MAX_COMMAND_LEN] = "";
    int i = 0;
    int j = 0;
    while(args[i] != '\0') {
        if(args[i] == '-') {
            // Found an option
            options[j++] = args[++i];
        } else {
            // Found part of the filename
            filename[strlen(filename)] = args[i];
        }
        i++;
    }

    // Check if -c option is present
    if(strchr(options, 'c') != NULL) {
        printf("No files will be created.\n");
        return;
    }

    // Open or create the file with write-only mode
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if(fd == -1) {
        perror("open");
        return;
    }

    // Close the file descriptor
    if(close(fd) == -1) {
        perror("close");
        return;
    }

    printf("File '%s' created or modified.\n", filename);

    // Check for options to change access and modification times
    if(strchr(options, 'd') != NULL) {
        struct utimbuf ut;
        ut.actime = ut.modtime = time(NULL);
        if(utime(filename, &ut) == -1) {
            perror("utime");
            return;
        }
        printf("Access time of file '%s' changed.\n", filename);
    }

    if(strchr(options, 'f') != NULL) {
        printf("Option -f ignored.\n");
    }
}*/
