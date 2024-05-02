#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#define PORT 60584
#define MAX_MESSAGE_SIZE 50
#define PASSWORD_SIZE 5
#define MAX_USERNAME_SIZE 20


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
    char buffer[1024] = { 0 };
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
        printf("Recieved: %s\n", buffer);
        buffer[0] = '\0';
    }
    pthread_join(sendMessage, NULL);
    close(args.client_socket);
    exit(1);
    return ;
}





struct sockaddr_in address;
int maxConns = 10;
int server_socket;// server socket 
int availableIndex = -1;
int n_connected = 0;

void AcceptConn(ThreadArgs *args){
    socklen_t addrlen = sizeof(address);
    char errorMessage[14] = "Server is full";
    char promptMessage[21] = "Enter your username: ";
    while(1){
        int tempsocket;
        if ((tempsocket = accept(server_socket, (struct sockaddr*)&address,
            &addrlen))< 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        for(int i= 0; i<maxConns; i++){
            if(args[i].isConnected == 0){
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
        args[availableIndex].client_socket = tempsocket;
        args[availableIndex].isConnected = 1;
        // Send prompt to client
        send(tempsocket, promptMessage, strlen(promptMessage), 0);
    
    }
}
void createChat(){
    //insert here the rest of the code to choose if the server is private, max people connected, etc(already made but in discord)
    ThreadArgs args[maxConns]; // struct of incomming connections
    for(int i = 0;i<maxConns;i++){
        args[i].isConnected = 0;
        args[i].client_socket = -1;
    }
    ssize_t bytes_received;
    
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
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
    pthread_create(&acceptClient, NULL, AcceptConn, (ThreadArgs *) &args);
    pthread_create(&sendMessage, NULL,SendMessage,(void *)&args);
    while(1){
        if(n_connected > 0){
            //use select to see if there are any incoming messages on the args[] array
            //display the args.name before
            bytes_received = read(args[0].client_socket, buffer, 1024 - 1); // subtract 1 for the null
            if(bytes_received > 0){
                printf("Recieved: %s\n", buffer);
            }
            //prolly a for loop with a send to every args[]? dk if there is a broadcast function or smth
            //after send the message to everyone WITH the name
        }
    }
    pthread_join(acceptClient, NULL);
    pthread_join(sendMessage, NULL);
    close(server_socket);
    close(args[0].client_socket);
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



