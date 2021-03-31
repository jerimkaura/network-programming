#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char* msg){
    perror(msg);
    exit(0);
}

void chat(int clientfeed){
    int buffsize = 512, n;
    char buffer[buffsize];
    while (1)
    {
        bzero(buffer, sizeof(buffer));
        printf("Enter msg: ");
        n=0;
        while ((buffer[n++]=getchar())!='\n');
        write(clientfeed, buffer, sizeof(buffer));
        bzero(buffer,sizeof(buffer));
        read(clientfeed,buffer,sizeof(buffer));
        printf("Server msg: %s", buffer);
        if(strncmp("exit",buffer, 4) == 0){
            printf("Client Closed\n");
            break;
        }
        
    }
    
}

int main(int argc, char *argv[]){
    int sockfeed, n;
    struct sockaddr_in client_addr;
    struct hostent *server;
    char buffer[512];
    
    sockfeed = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfeed<0){
        error("Error opening the socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no server\n");
        exit(0);
    }
    client_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &client_addr.sin_addr.s_addr, server->h_length);
    client_addr.sin_port = htons(9050);
    if(connect(sockfeed, (struct sockaddr *) &client_addr, sizeof(client_addr))<0){
        error("Error connecting");
    }
    chat(sockfeed);
    close(sockfeed);
    return 0;
    

    return 0;
}