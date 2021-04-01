#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){
    //socket
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd == -1){
        perror("Socket error from receiver side");
        exit(0);
    }

    struct sockaddr_in server, client;
    client.sin_family = AF_INET;
    client.sin_port = htons(6000); //server port
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    //call bind  function
    int b = bind(fd, (struct sockaddr*) &client, sizeof(struct sockaddr));
    if (b==-1){
        perror("Bind error from client side");
        exit(0);
    }

    char strt[200]= "gb7ig87oenhrf8oeywhirf";
    int size = sizeof(struct sockaddr);

    //rreceive message from sender
    int k = recvfrom(fd,strt, strlen(strt), 0, (struct sockaddr*)&server, &size);
    strt[k] = '\0';

    printf("Message received from sender is: %s\n", strt);
    

    char str[200];
    printf("Enter message to send to receiver: ");
    fgets(str, 200, stdin);

    server.sin_family = AF_INET;
    server.sin_port = htons(5000); //sender's port address
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // sender's ip address

    //send receiver's response
    sendto(fd,str, strlen(str), 0, (struct sockaddr*)&server, sizeof(server));

    //closing the socket
    close(fd);
}