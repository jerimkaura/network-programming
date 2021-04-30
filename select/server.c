#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdbool.h>

// hold sockets to all clients
int clients[10];

int main(int argc, char *argv[]) {
    struct sockaddr_in my_addr, their_addr;
    int my_sock, their_sock;
    socklen_t their_addr_size;

    int portno;

    // max message length
    char msg[500];
    int len;
    char ip[INET_ADDRSTRLEN];
    if (argc > 2) { // two arguments maximum
        printf("too many arguments");
        exit(1);
    }
    portno = atoi(argv[1]);

    // create socket
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portno);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    their_addr_size = sizeof(their_addr);

    // bind the socket to the address and port number
    if (bind(my_sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
        perror("binding unsuccessful");
        exit(1);
    }

    // listen for connections from clients
    if (listen(my_sock, 10) != 0) {
        perror("listening unsuccessful");
        exit(1);
    }

    fd_set current_sockets; // file descriptor set (bitset)
    for (int i = 0; i < 10; i++)
        clients[i] = 0;

    while (true) {
        FD_ZERO(&current_sockets); // clear fd set
        FD_SET(my_sock, &current_sockets); // add server's socket to fd set
        int maxfd = my_sock; // find the maximum socket fd for use in select function

        for (int i = 0; i < 10; i++) {
            if (clients[i] > 0) // has a valid socket
                FD_SET(clients[i], &current_sockets); // add socket to fd set
            if (clients[i] > maxfd) // finding maximum socket fd
                maxfd = clients[i];
        }

        // select returns the number of ready socket fds
        if (select(maxfd + 1, &current_sockets, NULL, NULL, NULL) < 1) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // check if my_sock (server's socket) has any activity
        // if it does, it means we have a new connection
        if (FD_ISSET(my_sock, &current_sockets)) {
            // accept new connection
            if ((their_sock = accept(my_sock, (struct sockaddr *)&their_addr, &their_addr_size)) < 0) {
                perror("accept unsuccessful");
                exit(1);
            }
            inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
            printf("%s connected\n", ip);

            // save the new connection's socket fd to clients
            for (int j = 0; j < 10; j++) {
                if (clients[j] == 0) {
                    clients[j] = their_sock;
                    break;
                }
            }
        } else {
            // process send messages (if any)
            for (int i = 0; i < 10; i++) {
                int sfd = clients[i];
                if (FD_ISSET(sfd, &current_sockets)) { // check if ready
                    char msg[500];
                    int len = recv(sfd, msg, 500, 0); // receive message
                    if (len == 0) {
                        // if ready and has no message to send, it means that
                        // the connection has been terminated by the client
                        close(sfd); // close the connection
                        clients[i] = 0;
                    } else {
                        msg[len] = '\0'; // null terminate message
                        for (int j = 0; j < 10; j++) {
                            // send the message to all clients apart from the sender
                            if (clients[j] != 0 && clients[j] != sfd) {
                                if (send(clients[j], msg, strlen(msg), 0) < 0) {
                                    perror("sending failure");
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}