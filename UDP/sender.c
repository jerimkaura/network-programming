#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<unistd.h>

int  main (){
    system("clear");
    char sender_message[2000], receiver_message[2000];
    //Create a socket that returns a socket descriptor
    int sender_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sender_descriptor < 0){
        perror("Socket creation error");
        exit(0);
    }

    struct sockaddr_in server_address;

    // Set port and IP:
    server_address.sin_family = AF_INET;

    //set port number
    server_address.sin_port = htons(5000);

    //set ip address
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1){
        // Get input from the user:
        printf("Enter message: ");
        fgets(sender_message, 2000, stdin);
        printf("%s", sender_message);
        // fgets(sender_message, strlen(sender_message), stdin);

        //send the message tothe receiver
        sendto(sender_descriptor, sender_message, strlen(sender_message), 0, (struct sockaddr*)&server_address, sizeof(server_address));

        //terminate chat session upon sending "end"
        if(strcmp(sender_message, "end")==0){
            break;
        }

        // Receive response from receiver
        int size = sizeof(server_address);
        int len = recvfrom(sender_descriptor, sender_message, sizeof(receiver_message), 0, (struct sockaddr*)&server_address, &size);
        sender_message[len] = '\0';

        // terminate chat session upon receiving "end"
        if(strcmp(receiver_message, "end")==0){
            break;
        }

        //output message received
        printf("Message received : %s\n", sender_message);
    }
    
    //close the socket
    close(sender_descriptor);
<<<<<<< HEAD
}
=======
}
>>>>>>> 507a2ca700e78292fbb180db47c9c24abe739b5f
