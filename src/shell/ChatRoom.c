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
#define BUFFER_SIZE 8192 

#define MAX_MESSAGE_SIZE 100
#define MESSAGE_HISTORY_SIZE 10


// fix bug when enter with empty string, either the cmd goes doen or it prints a line
// instead of showing the number of users, show the name
// when leaving chat, it sends 2 /n
// send chat messages history

// todo more pleasent terminal interface(ncursors)
// todo send files
// todo kick/ban users
// todo encryption keys, public/private keys
// todo Hash Password

int client_epoll;

typedef struct{
    int r;
    int g;
    int b;
} color;

typedef struct {
    char username[MAX_USERNAME_SIZE];
    color color;
    int isHost;//0 yes, 1 no
    int status;//0-needs auth/ 1-needs username/2-connected
    int client_socket;
    int isConnected;//0 not connected, 1 is connected
} ThreadArgs;

typedef struct{
    int type;// 0-auth/ 1-username/ 2-history/ 3-message/ 4-file
    char msg[BUFFER_SIZE];
} Servermsg;

int command = 0; //for own 0- none/ 1-leave




char* CreateUser() {
    char* username = malloc(MAX_USERNAME_SIZE * sizeof(char)); // Allocate memory for the username
    if (username == NULL) {
        return NULL;
    }
    printf("Insert a username: ");
    
    if (fgets(username, MAX_USERNAME_SIZE, stdin) != NULL) {
        if (strchr(username, '\n') == NULL) {
            while ((getchar()) != '\n');
        } else {
            username[strcspn(username, "\n")] = '\0';
        }
    }
    return username;
}
void joinChat(){
    ThreadArgs args;
    int status, valread;
    struct sockaddr_in serv_addr;
    char* username = CreateUser();
    if(username == NULL){
        printf("ERROR allocating memory\n");
        free(username);
        return;  
    }
    char *ip = malloc(20*sizeof(char));
    printf("ip:");
    if (fgets(ip, 20, stdin) != NULL) {

        if (strchr(ip, '\n') == NULL) {
            while ((getchar()) != '\n');
        } else {
            ip[strcspn(ip, "\n")] = '\0';
        }
    }
    //strcpy(ip, "127.0.0.1");
    char buffer[BUFFER_SIZE] = { 0 };


    if ((args.client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        free(ip);
        free(username);
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf( "Invalid address/ Address not supported\n");
        free(ip);
        free(username);
        return;
    }
    int num_fds;
    struct epoll_event events[2];

    client_epoll = epoll_create1(0);
    if (client_epoll == -1) {
        free(username);
        free(ip);
        perror("epoll_create1");
        return;
    }


    if ((status= connect(args.client_socket, (struct sockaddr*)&serv_addr,
        sizeof(serv_addr)))< 0) {
        printf("Connection Failed\n");
        free(username);
        free(ip);
        return;
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = 0;
    if (epoll_ctl(client_epoll, EPOLL_CTL_ADD, 0, &event) == -1) {
        perror("epoll_ctl");
        free(username);
        free(ip);
        return;
    }
    struct epoll_event event1;
    event1.events = EPOLLIN;
    event1.data.fd = args.client_socket;
    if (epoll_ctl(client_epoll, EPOLL_CTL_ADD, args.client_socket, &event1) == -1) {
        perror("epoll_ctl");
        free(username);
        free(ip);
        return;
    }
    while(command != 1){
        memset(buffer, 0, sizeof(buffer));
        int num_fds = epoll_wait(client_epoll, events, 2, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            free(username);
            free(ip);
            return;
        }
        Servermsg message;
        for (int n = 0; n < num_fds; ++n) {
            if (!(events[n].events & EPOLLIN))  {
                continue;
            }
            if(events[n].data.fd == 0){
                valread = read(events[n].data.fd, buffer, BUFFER_SIZE - 1);
                if(buffer[0] == '\n'){
                    printf("\033[1A");
                    printf("\033[K");
                    continue;
                }
                buffer[strcspn(buffer, "\n")] = '\0'; 
                if(strlen(buffer) < 1) {
                    continue;
                }
                if(strcmp("/leave", buffer) == 0){
                    command = 1;
                    continue;
                }
                printf("\033[1A"); // Move cursor up one line
                printf("\033[K");  // Clear line
                size_t test = send(args.client_socket, buffer, strlen(buffer), 0);
                buffer[0] = '\0';
                continue;
            }


            valread = read(events[n].data.fd, &message, sizeof(message));
            if(valread == 0){
                printf("Disconnected from Server\n");
                command =1;
                break;
            }
            if(message.msg[0] != '\0'){
                printf("%s\n",message.msg);
            }
            switch(message.type){
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
        }
    }
    command = 0;
    close(args.client_socket);
    free(ip);
    free(username);
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
int epoll_fd;
int epoll_fd1;




void *AcceptConn(void *args){
    epoll_fd1 = epoll_create1(0);
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    socklen_t addrlen = sizeof(address);
    char errorMessage[14] = "Server is full";


    struct epoll_event event1;
    event1.events = EPOLLIN | EPOLLET; // Monitor for read events in edge-triggered mode
    event1.data.fd = server_socket;
    if (epoll_ctl(epoll_fd1, EPOLL_CTL_ADD, server_socket, &event1) == -1) {
        perror("epoll_ctl");
        close(server_socket);
        pthread_exit(NULL);
    }
    while(command != 1){
            int tempsocket;
            struct epoll_event events[maxConns];
            int num_events = epoll_wait(epoll_fd1, events, maxConns, 500); // Wait for events with a timeout of 1 second
            if (num_events == -1) {
                perror("epoll_wait");
                pthread_exit(NULL); ;
            }
            if (num_events == 0) {
                continue;
            }
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
            msg.type = 1;
            threadArgs[availableIndex].status = 1;
            if(isPrivate == 1){
                threadArgs[availableIndex].status = 0;
                msg.type = 0;
            }
            n_connected ++;
            threadArgs[availableIndex].client_socket = tempsocket;
            threadArgs[availableIndex].isConnected = 1;
            threadArgs[availableIndex].color.r = rand() % 255 + 1;
            threadArgs[availableIndex].color.g = rand() % 255 + 1;
            threadArgs[availableIndex].color.b = rand() % 255 + 1;
            struct epoll_event event;
            event.events = EPOLLIN; // Monitor for read events
            event.data.fd = tempsocket;
            event.data.ptr = (void*)&threadArgs[availableIndex];
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tempsocket, &event) == -1) {
                perror("epoll_ctl");
                close(event.data.fd);
                continue;
            }

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
void sendMessage(int type,char *in_rec_message, int socket){
    Servermsg msg;
    msg.type = type;
    strcpy(msg.msg, in_rec_message);
    send(socket,&msg, sizeof(msg), 0);
}


void createChat(){
    srand(time(NULL));
    int option = 0;
    char *username = CreateUser();
    printf("Choose type:\n1->private\n2->public\n");
    int tempInput;
    while (option == 0) {
        if (scanf("%d", &tempInput) != 1) {
            printf("Invalid input\n");
            while (getchar() != '\n');
            continue;
        }
        if (tempInput != 1 && tempInput != 2) {
            printf("Not recognized\n");
            continue;
        }
        while (getchar() != '\n');
        option = tempInput;
    }
    printf("Max Conns:\n");
    tempInput = 0;
    while (1) {
        if (scanf("%d", &tempInput) != 1) {
            printf("Invalid input\n");
            while (getchar() != '\n');
            continue;
        }
        if (tempInput == 0) {
            printf("Not valid\n");
            continue;
            while (getchar() != '\n');
        }
        while (getchar() != '\n');
        maxConns = tempInput;
        break;
    }
    isPrivate = 0;
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
    

    ThreadArgs args[maxConns+1]; // struct of incomming connections +1 for own 

    //---------------------------------epol---------------------
    int num_fds;
    struct epoll_event events[maxConns+1];
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        free(username);
        return;
    }

   
    args[maxConns].isConnected = 1;
    args[maxConns].client_socket = 0;
    strcpy(args[maxConns].username, username);
    args[maxConns].status = 2;
    args[maxConns].isHost = 1;
    args[maxConns].color.r = rand() % 255 + 1;
    args[maxConns].color.g = rand() % 255 + 1;
    args[maxConns].color.b = rand() % 255 + 1;
    struct epoll_event event;
    event.events = EPOLLIN; // Monitor for read events
    event.data.fd = 0;
    event.data.ptr = (void*)&args[maxConns];
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event) == -1) {
        perror("epoll_ctl");
        free(username);
        return;
    }

    for(int i = 0;i<maxConns;i++){
        args[i].isConnected = 0;
        args[i].client_socket = -1;
        args[i].status = -1;
        args[i].isHost = 0;
    }
    ssize_t bytes_received;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[MAX_MESSAGE_SIZE] = { 0 };
    // Creating socket file descriptor


    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        free(username);
        return;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_socket, SOL_SOCKET,
                   SO_REUSEADDR , &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        free(username);
        return;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Forcefully attaching socket to the port 8080
    if (bind(server_socket, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        free(username);
        return;
    }
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        free(username);
        return;
    }

    int realMessageSize = MAX_MESSAGE_SIZE+MAX_USERNAME_SIZE+30; // 30 for color and formatting
    char messageHistory[MESSAGE_HISTORY_SIZE][realMessageSize+1];
    for(int i = 0; i < MESSAGE_HISTORY_SIZE; i++){
        messageHistory[i][0] = '\0';
    }
    int history_index = 0;

    system("clear");
    pthread_t acceptClient;
    pthread_create(&acceptClient, NULL, AcceptConn, (void*) &args);
    char rec_message[MAX_MESSAGE_SIZE];
    
    printf("Server Port: %d\n", PORT);
    if(isPrivate == 1)
        printf("Password: %s\n",password);

    char *tempMessage = malloc(150);
    snprintf(tempMessage, 150, "User \033[38;2;%d;%d;%dm%s\033[0m joined", args[maxConns].color.r, args[maxConns].color.g, args[maxConns].color.b, args[maxConns].username);
    printf("%s\n",tempMessage);
    strcpy(messageHistory[history_index],tempMessage);
    free(tempMessage);
    history_index++;
    
    while(command != 1){
        num_fds = epoll_wait(epoll_fd, events, maxConns+1, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            free(username);
            return;
        }

        for (int n = 0; n < num_fds; ++n) {
            if (events[n].events & EPOLLIN) {
                ThreadArgs* arg = (ThreadArgs*)events[n].data.ptr;
                memset(buffer, 0, sizeof(buffer));
                memset(rec_message, 0, sizeof(rec_message));
                bytes_received = read(arg->client_socket, buffer, MAX_MESSAGE_SIZE - 1);
                if(bytes_received == -1){continue;}
                if(bytes_received == 0){ // if disconnected
                    snprintf(rec_message, sizeof(rec_message), "User \033[38;2;%d;%d;%dm%s\033[0m left", arg->color.r, arg->color.g, arg->color.b, arg->username);
                    memset(arg->username, 0, sizeof(arg->username));
                    n_connected --;
                    arg->isConnected = 0;
                    close(arg->client_socket);

                    // Remove file descriptor from epoll
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, arg->client_socket, NULL);
                    broadcast(rec_message, (void*) &args);
                    continue;
                }
                if(buffer[0] == '\n'){
                    if(arg->isHost == 1){
                        printf("\033[1A");
                        printf("\033[K");
                    }
                    continue;
                }
               

                if (buffer[bytes_received - 1] == '\n')
                    buffer[bytes_received - 1] = '\0';


                if(bytes_received > sizeof(char)*MAX_MESSAGE_SIZE){
                    send(arg->client_socket, "Message Too Big", strlen("Message Too Big "), 0);
                    continue;
                }
                if(arg->isHost == 1){
                    
                    printf("\033[1A");
                    printf("\033[K");
                    
                    if(buffer[0] == '/')
                    { //------------------------------------------------ analise function here
                        memmove(buffer, buffer + 1, strlen(buffer));
                        if(strcmp("leave", buffer) == 0){
                            command = 1;
                            break;
                        } else if(strcmp("users", buffer) == 0){
                            printf("Connected users:\n");
                            for(int i = 0;i<=maxConns; i++){
                                if(args[i].isConnected == 0)
                                    continue;
                                printf("\033[38;2;%d;%d;%dm%s\033[0m\n", args[i].color.r, args[i].color.g, args[i].color.b, args[i].username);
                            }
                            continue; 
                            break;
                        }
                          
                    }
                    
 
                }
                switch(arg->status){
                    case 0:
                        Servermsg msg;
                        if( strcmp(password, buffer) != 0)
                        {
                            msg.type = -1;
                            strcpy(msg.msg, "wrong Password");
                            send(arg->client_socket, &msg, sizeof(msg), 0);
                            close(arg->client_socket);
                            memset(arg->username, 0, sizeof(arg->username));
                            n_connected --;
                            arg->isConnected = 0;
                            continue;
                            break;
                        }
                        msg.msg[0] = '\0';
                        arg->status = 1;
                        
                        msg.type = 1;
                        send(arg->client_socket, &msg, sizeof(msg), 0);
                        continue;
                        break;
                    case 1:
                        if(bytes_received > MAX_USERNAME_SIZE* sizeof(char)-1){ 
                            send(arg->client_socket, "Username too big choose another one", strlen("Username too big choose another one"), 0);
                            continue;
                            break;
                        }
                        //read username
                        memcpy(arg->username, buffer, bytes_received*sizeof(char));
                        snprintf(rec_message, sizeof(rec_message), "User \033[38;2;%d;%d;%dm%s\033[0m joined", arg->color.r, arg->color.g, arg->color.b, arg->username);
                        arg->status = 2;


                        //send history
                        int tempIndex = history_index;
                        do{
                            if(tempIndex == MESSAGE_HISTORY_SIZE){
                                tempIndex = 0;
                            }
                            sendMessage(2, messageHistory[tempIndex], arg->client_socket);
                            tempIndex ++;
                        }
                        while(tempIndex != history_index);
                        break;
                    case 2:
                        //read message
                        if(buffer[0] == '/')
                        { //------------------------------------------------ analise function here
                            memmove(buffer, buffer + 1, strlen(buffer));
                            if(strcmp("users", buffer) == 0){
                                char message[50]; // Allocate enough space for your message
                                sprintf(message, "Connected users:", n_connected+1);
                                sendMessage(3,message,arg->client_socket);
                                message[0] = '\0';
                                for(int i = 0;i<=maxConns; i++){
                                    if(args[i].isConnected == 0)
                                        continue;
                                    sprintf(message, "\033[38;2;%d;%d;%dm%s\033[0m", args[i].color.r, args[i].color.g, args[i].color.b, args[i].username);
                                    sendMessage(3,message,arg->client_socket);
                                    message[0] = '\0';
                                }
                                continue;
                            }
                            break;
                        }
                        snprintf(rec_message, sizeof(rec_message), "\033[38;2;%d;%d;%dm%s\033[0m : %s", arg->color.r, arg->color.g, arg->color.b, arg->username, buffer);
                        break;
                }
            if(rec_message[0] == '\0'){continue;} // if there are no message
            broadcast(rec_message, (void*) &args);

            if(history_index == MESSAGE_HISTORY_SIZE){
                history_index = 0;
            }
            strcpy(messageHistory[history_index],rec_message);
            history_index ++;
            }
        }

    }
    command = 0;
    close(epoll_fd);
    close(epoll_fd1);
    
    pthread_join(acceptClient, NULL);
    for(int i = 0;i<maxConns; i++){
        if(args[i].isConnected == 0){continue;}
        close(args[i].client_socket);
    }
    close(server_socket);
    free(username);
    return;
}





