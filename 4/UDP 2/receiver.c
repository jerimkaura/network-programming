#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int  main (){
    system("clear");
    //Create a socket that returns a socket descriptor
    int receiver_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiver_descriptor < 0){
        perror("Socket creation error");
        exit(0);
    }

    struct sockaddr_in server_address, client_address;

    // Set port and IP:
    server_address.sin_family = AF_INET;

    //port 
    server_address.sin_port = htons(5000);

    //ip address
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if(bind(receiver_descriptor, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        printf("Couldn't bind to the port\n");
        exit(0);
    }

    //messages placeholder arrays
    char sender_message[2000], receiver_message[2000];
    while(1){
        int size = sizeof(server_address);

        // receive message from sender
        int len = recvfrom(receiver_descriptor, receiver_message, sizeof(receiver_message), 0, (struct sockaddr*)&client_address, &size);
        receiver_message[len] = '\0';

        //terminate chat session upon receiving "end"
        if(strcmp(receiver_message, "end")==0){
            break;
        }

        //output the message
        printf("Message received : %s\n", receiver_message);

        // Get input from the user:
        printf("Enter message: ");
        fgets(sender_message, 2000, stdin);
        //scanf("%s", sender_message);

        //send respose to sender
        sendto(receiver_descriptor, sender_message, strlen(sender_message), 0, (struct sockaddr*)&client_address, sizeof(client_address));
        
        //terminate chat session upon sending "end"
        if(strcmp(receiver_message[0], "end")==0){
            break;
        }
    }
    
    //close the secket on receiver side
    close(receiver_descriptor);
}
