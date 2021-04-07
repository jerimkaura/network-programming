#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

void chat(int sockfeed){
    int n;
    char buffer[512];
    while(1){
        bzero(buffer,512);
        
        int n = read(sockfeed, buffer, sizeof(buffer));
        if(n<0){
            error("Error in reading from the socket");
        }
        printf("Client Output: %s\n", buffer);
        bzero(buffer,512);
        printf("Enter msg: ");
        n = 0;
        while ((buffer[n++] = getchar()) != '\n');
        write(sockfeed,buffer,sizeof(buffer));
        if(strncmp("exit",buffer, 4) == 0){
            printf("Server Closed\n");
            break;
        }
        
    }
}

int main(int argc, char **argv){
    // Create the socket
    int server_sock, newsocket_conn;
    char *ip = "127.0.0.1";
    int port;
    if(argc<2){
        port = 9200;
    }else{
        port = atoi(argv[1]);
    }
    socklen_t client;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock<0){
        error("Server failed connection!");
    }

    // Define the address
    struct sockaddr_in server_address, client_addr;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    // Bind the server
    if(bind(server_sock, (struct sockaddr *) &server_address, sizeof(server_address))<0){
        error("Error binding the socket");
    }

    // Listen
    listen(server_sock, 5);
    int client_socket;
    // Accepting connection
    
    newsocket_conn = accept(server_sock, (struct sockaddr *) &client_addr, &client);
    if(newsocket_conn<0){
        error("Error during accept");
    }
    printf("Connection was created successfully\n");
    chat(newsocket_conn);
    close(newsocket_conn); // Closing accepted connection
    close(server_sock); // Closing socket connection
    return 0;
}