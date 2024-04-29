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
#define PORT 60584
#define MAX_MESSAGE_SIZE 50
#define PASSWORD_SIZE 5
#define MAX_USERNAME_SIZE 20

typedef struct {
    char username[MAX_USERNAME_SIZE];
    char message[MAX_MESSAGE_SIZE];
    char password[PASSWORD_SIZE];
} userData;

typedef struct {
    int fd;
    int isExit;
} ThreadArgs;

  

void *SendMessage(void *arg){
    char buffer[MAX_MESSAGE_SIZE];
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_fd = args->fd;
    while(1){
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 
        if(strcmp(buffer,"/EXIT") == 0){
            args->isExit = 1;
            break;
        }else if(strlen(buffer) > 0) {
            size_t test = send(client_fd, buffer, strlen(buffer), 0);
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
    args.isExit = 0;
    int status, valread;
    struct sockaddr_in serv_addr;
    userData data;
    strcat(data.username,CreateUser());
     if (data.username == NULL) {
        return;
    }
    printf("%s\n", data.username);

    char *ip = malloc(20*sizeof(char)); //! FREE IP
    ip = "127.0.0.1";
    char buffer[1024] = { 0 };
    if ((args.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
    if ((status= connect(args.fd, (struct sockaddr*)&serv_addr,
             sizeof(serv_addr)))< 0) {
        printf("\nConnection Failed \n");
        return;
    }
    size_t test = send(args.fd, &data, sizeof(data), 0);

    //Creating a thread to recieve user inputs and send them
    pthread_t sendMessage;
    pthread_create(&sendMessage, NULL,SendMessage,(void *)&args);
    while(1){
        valread = read(args.fd, buffer, 1024 - 1); // subtract 1 for the null
        printf("Recieved: %s\n", buffer);
        if(args.isExit == 1){
            break;
            args.isExit = 0;
        }
    }
    pthread_join(sendMessage, NULL);
    // closing the connected socket
    close(args.fd);
    return ;
}
void createChat(){
    srand(time(NULL));
    printf("Choose type:\n1->private\n2->public\n");
    int option;
    scanf("%d", &option);
    int password;
    if(option == 1){
        
        for(int i = 0; i<= PASSWORD_SIZE; i++ ){
            int num = rand() % 10;
            printf("%d - ",num);
            password = concat(num, password);
        }
    }
    printf("\n%d\n",password);
    return;
    ThreadArgs args;
    args.isExit = 0;
    int new_socket, fd;
    ssize_t bytes_received;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    // Creating socket file descriptor
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(fd, SOL_SOCKET,
                   SO_REUSEADDR , &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((args.fd
         = accept(fd, (struct sockaddr*)&address,
                  &addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    userData data;
    printf("ACCEPTED a client\n");
    bytes_received = recv(args.fd, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Recv failed");
    }
    memcpy(&data, buffer, sizeof(userData));



    pthread_t sendMessage;
    pthread_create(&sendMessage, NULL,SendMessage,(void *)&args);
    while(1){
        bytes_received = read(args.fd, buffer, 1024 - 1); // subtract 1 for the null
        printf("Recieved: %s\n", buffer);
        if(args.isExit == 1){
            break;
            args.isExit = 0;
        }
    }
    pthread_join(sendMessage, NULL);
    
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    close(args.fd);
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



