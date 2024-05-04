#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <unistd.h>
#include <time.h>
#include <fcntl.h>


#define PORT 60584
#define MAX_MESSAGE_SIZE 100
#define PASSWORD_SIZE 5
#define MAX_USERNAME_SIZE 10
#define MAX_SERVER_SIZE 100
#define BUFFER_SIZE 1024
#define MESSAGE_HISTORY_SIZE 80

char MESSAGE_HISTORY[MESSAGE_HISTORY_SIZE][MAX_MESSAGE_SIZE];

typedef struct {
    char username[MAX_USERNAME_SIZE];
    int client_socket;
    int isConnected; //0 not connected, 1 is connected
} ThreadArgs;

void *SendMessage(void *arg){
    char buffer[MAX_MESSAGE_SIZE];
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_socket = args->client_socket;
    while(1){
        fgets(buffer, sizeof(buffer), stdin);
        // printf("\033[1A"); // Move cursor up one line
        // printf("\033[K");  // Clear line
        buffer[strcspn(buffer, "\n")] = '\0'; 
        if(strlen(buffer) > 0) {
            size_t test = send(client_socket, buffer, strlen(buffer), 0);
            buffer[0] = '\0';
        }   
    }
    return NULL;
}
char* CreateUser(){
    char* username = malloc(MAX_USERNAME_SIZE*sizeof(char)); //! FREE USERNAME
    if(username == NULL){
        return NULL;
    }
    printf("Insert a username:");
    scanf("%19s", username);
    return username;
    
}
void joinChat(){
    ThreadArgs args;
    int status, valread;
    struct sockaddr_in serv_addr;
    // strcat(data.username,CreateUser());
    //  if (data.username == NULL) {
    //     return;
    // }
    // printf("%s\n", data.username);

    char *ip = malloc(20*sizeof(char)); //! FREE IP
    ip = "127.0.0.1";
    char buffer[BUFFER_SIZE] = { 0 };
    if ((args.client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return;
    }
    if ((status= connect(args.client_socket, (struct sockaddr*)&serv_addr,
             sizeof(serv_addr)))< 0) {
        printf("\nConnection Failed \n");
        return;
    }
    //try to get it
    //if password necessary send password
    //if not send name


    // size_t test = send(args.fd, &data, sizeof(data), 0);
    
    //Creating a thread to recieve user inputs and send them
    pthread_t sendMessage;
    pthread_create(&sendMessage, NULL,SendMessage,(void *)&args);
    while(1){
        valread = read(args.client_socket, buffer, 1024 - 1); // subtract 1 for the null
        if(valread == 0){
            printf("Disconnected from Server\n");
            break;
        }
        printf("%s\n", buffer);
        buffer[0] = '\0';
    }
    pthread_join(sendMessage, NULL);
    close(args.client_socket);
    exit(1);
    return ;
}





struct sockaddr_in address;
int maxConns = 5; //cannot exceed 1023
int server_socket;// server socket 
int availableIndex = -1;
int n_connected = 0;

void *AcceptConn(void *args){
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    socklen_t addrlen = sizeof(address);
    char errorMessage[14] = "Server is full";
    char promptMessage[21] = "Enter your username: ";
    while(1){
        int tempsocket;
        if ((tempsocket = accept(server_socket, (struct sockaddr*)&address,&addrlen))< 0){
            perror("Accept");
            exit(1);
        }
        for(int i= 0; i<maxConns; i++){
            if(threadArgs[i].isConnected == 0){
                availableIndex = i;
                break;
            }
            availableIndex = -1;
        }
        if(availableIndex == -1){
            send(tempsocket, errorMessage, strlen(errorMessage), 0);
            close(tempsocket);  
            continue;
        }
        int status = fcntl(tempsocket, F_SETFL, fcntl(tempsocket, F_GETFL, 0) | O_NONBLOCK);
        if (status == -1){
            perror("calling fcntl");
            close(tempsocket);
            continue;
        }
        n_connected ++;
        threadArgs[availableIndex].client_socket = tempsocket;
        threadArgs[availableIndex].isConnected = 1;
        memset(threadArgs[availableIndex].username, 0, sizeof(threadArgs[availableIndex].username));
        // Send prompt to client
        send(tempsocket, promptMessage, strlen(promptMessage), 0);
    }
            return NULL;

}



void createChat(){
    //insert here the rest of the code to choose if the server is private, max people connected, etc(already made but in discord)
    ThreadArgs args[maxConns]; // struct of incomming connections
    //use epool()?
    char *username = CreateUser();
    printf("%s\n",username);

    int flags = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);// NOT THE BEST WAY is my guess

    for(int i = 0; i<MESSAGE_HISTORY_SIZE;i++){
            MESSAGE_HISTORY[i][0] = '\0';
    }
    for(int i = 0;i<maxConns;i++){
        args[i].isConnected = 0;
        args[i].client_socket = -1;
    }
    ssize_t bytes_received;
    
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[MAX_MESSAGE_SIZE] = { 0 };

    // Creating socket file descriptor

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_socket, SOL_SOCKET,
                   SO_REUSEADDR , &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Forcefully attaching socket to the port 8080
    if (bind(server_socket, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    pthread_t sendMessage;
    pthread_t acceptClient;
    pthread_create(&acceptClient, NULL, AcceptConn, (void*) &args);
    //pthread_create(&sendMessage, NULL,SendMessage,(void *)&args);
    char rec_message[MAX_MESSAGE_SIZE];
    int history_index = 0;
    while(1){
        memset(buffer, 0, sizeof(buffer));
        memset(rec_message, 0, sizeof(rec_message));
        bytes_received = read(0, buffer, sizeof(buffer));
        if (bytes_received > 0) {
            printf("%s\n",buffer);
            //snprintf(rec_message, sizeof(rec_message), "\033[32m%s\033[0m : %s", username, buffer);
        } 

        for(int i = 0; i<maxConns; i++){
                if(args[i].isConnected == 0){continue;}// if the index doesnt have a user
                bytes_received = read(args[i].client_socket, buffer, MAX_MESSAGE_SIZE - 1); // subtract 1 for the null
                if(bytes_received == -1){continue;}// if the index sayed nothing, continue

                if(bytes_received > sizeof(char)*MAX_MESSAGE_SIZE){
                    send(args[i].client_socket, "Message Too Big", strlen("Message Too Big"), 0);
                    continue;
                }
                if(bytes_received == 0){ // if disconnected
                    printf("%s\n",args[i].username);
                    snprintf(rec_message, sizeof(rec_message), "User \033[32m%s\033[0m left", args[i].username);
                    memset(args[i].username, 0, sizeof(args[i].username));
                    n_connected --;
                    args[i].isConnected = 0;
                }else if (args[i].username[0] == '\0') { // if it did not have a username
                    if(bytes_received > MAX_USERNAME_SIZE* sizeof(char)-1){ 
                        send(args[i].client_socket, "Username too big", strlen("Username too big"), 0);
                        continue;
                    }
                    memcpy(args[i].username, buffer, bytes_received*sizeof(char));
                    snprintf(rec_message, sizeof(rec_message), "User \033[32m%s\033[0m Joined", args[i].username);
                } else {
                    snprintf(rec_message, sizeof(rec_message), "\033[32m%s\033[0m : %s", args[i].username, buffer);
                }
                strcpy(MESSAGE_HISTORY[history_index], rec_message);

                history_index ++;
            printf("%s\n", rec_message);
            if(rec_message[0] == '\0'){continue;} // if there are no messages

            for(int n = 0; n<maxConns;n++){
                if(args[n].isConnected == 0){continue;}

                send(args[n].client_socket, rec_message, strlen(rec_message), 0);
            }

            if(history_index == 10){
                for(int i = 0; i<MESSAGE_HISTORY_SIZE;i++){
                    if(MESSAGE_HISTORY[i][0] == '\0'){continue;}
                }
            }
        }

    }
    pthread_join(acceptClient, NULL);
    pthread_join(sendMessage, NULL);
    for(int i = 0;i<maxConns; i++){
        if(args[i].isConnected == 0){continue;}
        close(args[i].client_socket);
    }
    close(server_socket);
    return;
}

int main(int argc, char *argv[]) {
    char input[10]; // Assuming the input won't exceed 100 characters
    printf("\033[32m------------WELCOME TO CHATROOMS :D--------------\033[0m\n");
    printf("Select Mode JOIN/CREATE: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; 
    if(strcmp(input, "JOIN") == 0 ){
        joinChat();
    }else if(strcmp(input, "CREATE") == 0){
        createChat();
    }else{
        printf("Not a valid input");
    }

    return 0;
}



