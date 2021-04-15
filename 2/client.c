#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#define MAX_CUR 100
#define MAX_ACC 10
#define MAX_TEXT 1000

void error(const char* msg){
    perror(msg);
    exit(0);
}

void chat(int clientfeed){
    int n;
    char buffer[MAX_TEXT];
    while (1)
    {
        bzero(buffer,sizeof(buffer));
        read(clientfeed,buffer,sizeof(buffer));
        printf("Server: %s", buffer);
        
        bzero(buffer, sizeof(buffer));
        printf(">> ");
        n=0;
        char ch;
        while ((ch=getchar())!='\n'){
            buffer[n++] = ch;
        }
        write(clientfeed, buffer, sizeof(buffer));
        if(strncmp("exit",buffer, 4) == 0){
            printf("Client Closed\n");
            break;
        }
        
        
    }
    
}

int main(int argc, char **argv){
    int sockfeed, n;
    char *ip = "127.0.0.1";
    int port;
    if(argc<2){
        port = 9200;
    }else{
        port = atoi(argv[1]);
    }
    struct sockaddr_in client_addr;
    struct hostent *server;
    char buffer[MAX_TEXT];
    
    sockfeed = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfeed<0){
        error("Error opening the socket");
    }
    server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr, "Error, no server\n");
        exit(0);
    }
    client_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &client_addr.sin_addr.s_addr, server->h_length);
    client_addr.sin_port = htons(port);
    if(connect(sockfeed, (struct sockaddr *) &client_addr, sizeof(client_addr))<0){
        error("Error connecting");
    }
    

    chat(sockfeed);
    close(sockfeed);
    return 0;
    

    return 0;
}