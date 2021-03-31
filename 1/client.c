#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    // use IPv4 addresses
    server_address.sin_family = AF_INET;
    // use port 9302
    server_address.sin_port = htons(9302);
    // bind socket to all available network interfaces
    // including the localhost 127.0.0.1
    server_address.sin_addr.s_addr = INADDR_ANY;

    // connect to the server
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // check for error with the connection
    if (connection_status == -1) {
        printf("There was an error making a connection to the remote socket \n\n");
        return 1;
    }

    // receive data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("The server sent the data: %s\n", server_response);

    // close the network connection
    close(network_socket);
    return 0;
}