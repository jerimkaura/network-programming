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
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    // use IPv4 addresses
    server_address.sin_family = AF_INET;
    // use port 9302
    server_address.sin_port = htons(9302);
    // bind socket to all available network interfaces
    // including the localhost 127.0.0.1
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // make the socket enter into listening state
    listen(server_socket, 5);

    // accept an incoming connection from the remote client
    // and create an associated socket
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    // send data to the client
    send(client_socket, server_message, sizeof(server_message), 6);
    
    // close the connection
    close(client_socket);
    
    return 0;
}