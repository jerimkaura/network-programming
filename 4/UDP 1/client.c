#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // message that the client will send to the server
    char client_message[256] = "Hello from some remote!";

    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // specify an address for the socket
    struct sockaddr_in server_address;
    // use IPv4 addresses
    server_address.sin_family = AF_INET;
    // use port 9302
    server_address.sin_port = htons(9302);
    // bind socket to all available network interfaces
    // including the localhost 127.0.0.1
    server_address.sin_addr.s_addr = INADDR_ANY;

    // send data to server
    sendto(network_socket, client_message, sizeof(client_message), 0, (struct sockaddr*) &server_address, sizeof(server_address));
    printf("Client sent: %s\n", client_message);

    // receive data from the server
    char server_response[256];
    int size = sizeof(server_address);
    recvfrom(network_socket, server_response, sizeof(server_response), 0, (struct sockaddr *) &server_address, &size);

    // print out the server's response
    printf("Client received: %s\n", server_response);

    // close the socket
    close(network_socket);

    return 0;
}