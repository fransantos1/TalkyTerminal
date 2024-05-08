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
#include "ChatRoom.h"

#define PORT 60584
#define PASSWORD_SIZE 5
#define MAX_USERNAME_SIZE 10
#define MAX_SERVER_SIZE 100
#define BUFFER_SIZE 1024

#define MAX_MESSAGE_SIZE 100
#define MESSAGE_HISTORY_SIZE 80

char MESSAGE_HISTORY[MESSAGE_HISTORY_SIZE][MAX_MESSAGE_SIZE];

// todo choose max users
// todo user leave when disconnected from server

// todo final review

// todo kick/ban users

// todo insert port manually
// todo fix bug when enter with empty string, either the cmd goes doen or it prints a line
// todo more pleasent terminal interface
// todo instead of showing the number of users, show the name
// todo each user has its own color


// todo Hash Password




typedef struct {
    char username[MAX_USERNAME_SIZE];
    int status;//0-needs auth/ 1-needs username/2-connected
    int client_socket;
    int isConnected;//0 not connected, 1 is connected
} ThreadArgs;

typedef struct{
    int type;// 0-needs auth/ 1-needs username/ 2-sending history /3-message
    char msg[MAX_MESSAGE_SIZE];
} Servermsg;

int command = 0; //for own 0- none/ 1-leave

void *SendMessageThread(void *arg){
    char buffer[MAX_MESSAGE_SIZE];
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_socket = args->client_socket;
    while(1){
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 
        if(strlen(buffer) > 0) {
            if(strcmp("/leave", buffer) == 0){
                command = 1;
                return NULL;
            }
            printf("\033[1A"); // Move cursor up one line
            printf("\033[K");  // Clear line
            size_t test = send(client_socket, buffer, strlen(buffer), 0);
            buffer[0] = '\0';
        }   
    }
    return NULL;
}
Servermsg parse_server_message(char *buffer, size_t buffer_size) {
    Servermsg msg;
    msg.msg[0] = '\0';
    if (buffer_size < sizeof(int)) {
        msg.type = -1;
        return msg;
    }
    memcpy(&msg.type, buffer, sizeof(int));
    if (buffer_size > sizeof(int)) {
        strcpy(msg.msg, buffer + sizeof(int));
    }
    memset(buffer, 0, buffer_size);
    return msg;
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
    char* username = CreateUser();
    char *ip = malloc(20*sizeof(char));
    printf("ip:");
    scanf("%s", ip);
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

    if (fcntl(args.client_socket, F_SETFL, fcntl(args.client_socket, F_GETFL, 0) | O_NONBLOCK) == -1){
            perror("calling fcntl");
            close(args.client_socket);
    }
    pthread_t sendMessage;
    pthread_create(&sendMessage, NULL,SendMessageThread,(void *)&args);
    while(command != 1){
        sleep(0.3);
        valread = read(args.client_socket, buffer, BUFFER_SIZE - 1); // subtract 1 for the null
        if(valread < 0){continue;}
        if(valread == 0){
            printf("Disconnected from Server\n");
            command =1;
            break;
        }
        Servermsg msg = parse_server_message(buffer, valread);
        if(msg.msg[0] != '\0'){
            printf("%s\n",msg.msg);
        }
        switch(msg.type){
            case 0:
                printf("Whats the password?\n");
                break;
            case 1:
                size_t test = send(args.client_socket, username, strlen(username), 0);
                system("clear");
                //send username
                break;
            case 2:
                
                //recieve history
                break;
        }
        memset(buffer, 0, sizeof(buffer));

    }
    pthread_join(sendMessage, NULL);
    close(args.client_socket);
    free(ip);
    exit(1);
    return ;
}

char keys[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5',
'6', '7', '8', '9'};


struct sockaddr_in address;
int maxConns = 10; //cannot exceed 1023
int server_socket;// server socket 
int availableIndex = -1;
int n_connected = 0;
int isPrivate = 0; //0 no, 1 is

void *AcceptConn(void *args){
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    socklen_t addrlen = sizeof(address);
    char errorMessage[14] = "Server is full";
    if (fcntl(server_socket, F_SETFL, fcntl(server_socket, F_GETFL, 0) | O_NONBLOCK) == -1){
        perror("calling fcntl");
        close(server_socket);
        pthread_exit(NULL);
        
    }
    while(command != 1){
            sleep(0.5);
            int tempsocket;
            if ((tempsocket = accept(server_socket, (struct sockaddr*)&address,&addrlen))< 0){
                continue;
            }
            Servermsg msg;
            for(int i= 0; i<maxConns; i++){
                if(threadArgs[i].isConnected == 0){
                    availableIndex = i;
                    break;
                }
                availableIndex = -1;
            }
            if(availableIndex == -1){
                msg.type = 3;
                strcpy(msg.msg, errorMessage);
                send(tempsocket, &msg, sizeof(msg), 0);
                close(tempsocket);  
                continue;
            }
            
            if (fcntl(tempsocket, F_SETFL, fcntl(tempsocket, F_GETFL, 0) | O_NONBLOCK) == -1){//! REMOVE BLOCKING WHEN Epool
                perror("calling fcntl");
                close(tempsocket);
                continue;
            }
            
            msg.type = 1;
            threadArgs[availableIndex].status = 1;
            if(isPrivate == 1){
                threadArgs[availableIndex].status = 0;
                msg.type = 0;
            }
            n_connected ++;
            threadArgs[availableIndex].client_socket = tempsocket;
            threadArgs[availableIndex].isConnected = 1;
            send(tempsocket, &msg, sizeof(msg), 0);
            memset(threadArgs[availableIndex].username, 0, sizeof(threadArgs[availableIndex].username));
        }
        pthread_exit(NULL);
}

void broadcast(char *rec_message, void *in_args){
    ThreadArgs *args = (ThreadArgs *)in_args;
    Servermsg msg;
    msg.type = 3;
    strcpy(msg.msg, rec_message);
    for(int n = 0; n<=maxConns;n++){
        if(n == maxConns){//Print self
            printf("%s\n", msg.msg);
            continue;
        }
        if(args[n].isConnected == 0 && args[n].status == 2){continue;}
        send(args[n].client_socket,&msg, sizeof(msg), 0);
    }
}
void sendMessage(int type,char *rec_message, int socket){
    Servermsg msg;
    msg.type = type;
    strcpy(msg.msg, rec_message);
    send(socket,&msg, sizeof(msg), 0);
}


void createChat(){
    srand(time(NULL));
    int option = 0;
    printf("Choose type:\n1->private\n2->public\n");
    while(option == 0){
        int tempOption;
        scanf("%d", &tempOption);
        if(tempOption != 1 && tempOption != 2){
            printf("not recognized\n");
            continue;
        }
        option = tempOption;
        
    }
    int keys_len = sizeof(keys) / sizeof(keys[0]);
    char password[PASSWORD_SIZE];
    if (option == 1) {
        for (int i = 0; i < PASSWORD_SIZE; i++) {
            isPrivate=1;
            int random_index = rand() % keys_len;
            password[i] = keys[random_index];
        }
        password[PASSWORD_SIZE] = '\0';
    }
    //! max people connected, etc(already made but in discord)


    ThreadArgs args[maxConns+1]; // struct of incomming connections +1 for own 
    //use epool()?
    char *username = CreateUser();

    int flags = fcntl(0, F_GETFL, 0); //! REMOVE BLOCKING WHEN Epool
    fcntl(0, F_SETFL, flags | O_NONBLOCK);// NOT THE BEST WAY is my guess
    for(int i = 0; i<MESSAGE_HISTORY_SIZE;i++){
            MESSAGE_HISTORY[i][0] = '\0';
    }
    args[maxConns].isConnected = 1;
    args[maxConns].client_socket = 0;
    strcpy(args[maxConns].username, username);
    args[maxConns].status = 2;

    for(int i = 0;i<maxConns;i++){
        args[i].isConnected = 0;
        args[i].client_socket = -1;
        args[i].status = -1;
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

    system("clear");
    pthread_t acceptClient;
    pthread_create(&acceptClient, NULL, AcceptConn, (void*) &args);
    char rec_message[MAX_MESSAGE_SIZE];
    int history_index = 0;
    printf("Server Port: %d\n", PORT);
    if(isPrivate == 1)
        printf("Password: %s\n",password);
    while(command != 1){
        sleep(0.2);





        for(int i = 0; i<=maxConns; i++){
                if(args[i].isConnected == 0){continue;}// if the index doesnt have a user
                memset(buffer, 0, sizeof(buffer));
                memset(rec_message, 0, sizeof(rec_message));
                bytes_received = read(args[i].client_socket, buffer, MAX_MESSAGE_SIZE - 1);
                if(bytes_received == -1){continue;}
                if(bytes_received == 0){ // if disconnected
                    snprintf(rec_message, sizeof(rec_message), "User \033[32m%s\033[0m left", args[i].username);
                    memset(args[i].username, 0, sizeof(args[i].username));
                    n_connected --;
                    args[i].isConnected = 0;
                    broadcast(rec_message, (void*) &args);
                    continue;
                }

                if (buffer[bytes_received - 1] == '\n')
                    buffer[bytes_received - 1] = '\0';


                if(bytes_received > sizeof(char)*MAX_MESSAGE_SIZE){
                    send(args[i].client_socket, "Message Too Big", strlen("Message Too Big "), 0);
                    continue;
                }
                if(i == maxConns){
                    printf("\033[1A");
                    printf("\033[K");
                    if(buffer[0] == '/')
                    { //------------------------------------------------ analise function here
                        memmove(buffer, buffer + 1, strlen(buffer));
                        if(strcmp("leave", buffer) == 0){
                            command = 1;
                            break;
                        } else if(strcmp("users", buffer) == 0){
                            printf("Number of connected users: %d\n", n_connected+1);
                            continue; 
                            break;
                        }
                          
                    }
                    
 
                }
                switch(args[i].status){
                    case 0:
                        Servermsg msg;
                        if( strcmp(password, buffer) != 0)
                        {
                            msg.type = -1;
                            strcpy(msg.msg, "wrong Password");
                            send(args[i].client_socket, &msg, sizeof(msg), 0);
                            close(args[i].client_socket);
                            memset(args[i].username, 0, sizeof(args[i].username));
                            n_connected --;
                            args[i].isConnected = 0;
                            continue;
                            break;
                        }
                        msg.msg[0] = '\0';
                        args[i].status = 1;
                        
                        msg.type = 1;
                        send(args[i].client_socket, &msg, sizeof(msg), 0);
                        continue;
                        break;
                    case 1:
                        if(bytes_received > MAX_USERNAME_SIZE* sizeof(char)-1){ 
                            send(args[i].client_socket, "Username too big choose another one", strlen("Username too big choose another one"), 0);
                            continue;
                            break;
                        }
                        //read username
                        memcpy(args[i].username, buffer, bytes_received*sizeof(char));
                        snprintf(rec_message, sizeof(rec_message), "User \033[32m%s\033[0m Joined", args[i].username);
                        args[i].status = 2;

                        //send history


                        break;
                    case 2:
                        //read message
                        if(buffer[0] == '/')
                        { //------------------------------------------------ analise function here
                            memmove(buffer, buffer + 1, strlen(buffer));
                            if(strcmp("users", buffer) == 0){
                                char message[50]; // Allocate enough space for your message
                                sprintf(message, "Number of connected users: %d", n_connected+1);
                                sendMessage(3,message,args[i].client_socket);
                                continue;
                            }
                            break;
                        }
                        snprintf(rec_message, sizeof(rec_message), "\033[32m%s\033[0m : %s", args[i].username, buffer);

                        break;
                }
                strcpy(MESSAGE_HISTORY[history_index], rec_message);
                history_index ++;
            
            if(rec_message[0] == '\0'){continue;} // if there are no message
            broadcast(rec_message, (void*) &args);
            // if(history_index == 10){
            //     for(int i = 0; i<MESSAGE_HISTORY_SIZE;i++){
            //         if(MESSAGE_HISTORY[i][0] == '\0'){continue;}
            //     }
            // }
        }
    }
    pthread_join(acceptClient, NULL);
    for(int i = 0;i<maxConns; i++){
        if(args[i].isConnected == 0){continue;}
        close(args[i].client_socket);
    }
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



