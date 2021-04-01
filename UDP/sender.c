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
    //socket creation
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd == -1){
        perror("Socket error from sender side");
        exit(0);
    }

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port = htons(5000); //senders port address
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // sender ip address

    int b = bind(fd, (struct sockaddr*) &server, sizeof(struct sockaddr));
    if (b==-1){
        perror("Bind error from server side");
        exit(0);
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(6000); //receiver por address
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    char str[200];
    printf("Enter message to send to receiver: ");
    fgets(str, 200, stdin);

    //send the message to receiver
    sendto(fd,str, strlen(str), 0, (struct sockaddr*)&client, sizeof(client));

    char strs[200]= "gb7ig87oenhrf8oeywhirf";
    int size = sizeof(struct sockaddr);

    //receive response from receiver
    int k = recvfrom(fd,strs, strlen(strs), 0, (struct sockaddr*)&client, &size);
    strs[k] = '\0';

    printf("Message received from receiver is: %s\n", strs);

    //closing the socket
    close(fd);

}