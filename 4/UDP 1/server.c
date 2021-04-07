#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // message that the server will send to the client
    char server_message[256] = "You have reached the server!";

    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == -1) {
        perror("Server socket creation failed!");
        exit(0);
    }

    // define the server address
    struct sockaddr_in server_address, client_address;
    // use IPv4 addresses
    server_address.sin_family = AF_INET;
    // use port 9302
    server_address.sin_port = htons(9302);
    // bind socket to all available network interfaces
    // including the localhost 127.0.0.1
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    int b_code = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (b_code < 0) {
        perror("Failed to bind socket to IP & port");
        exit(0);
    }

    // receive data from client
    char buf[256];
    int size = sizeof(client_address);
    recvfrom(server_socket, buf, 256, 0, (struct sockaddr *) &client_address, &size);
    printf("Server received: %s\n", buf);

    // send data to the client
    sendto(server_socket, server_message, sizeof(server_message), 0, (struct sockaddr*) &client_address, sizeof(client_address));
    printf("Server sent: %s\n", server_message);
    
    // close the socket
    close(server_socket);
    
    return 0;
}