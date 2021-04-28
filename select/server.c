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

int clients[10]; // hold sockets to all clients

int main(int argc,char *argv[]) {
	struct sockaddr_in my_addr,their_addr;
	int my_sock, their_sock;
	socklen_t their_addr_size;

	int portno;

    //max message length
	char msg[500];
	int len;
	char ip[INET_ADDRSTRLEN];
	if(argc > 2) {// two arguments maximum
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[1]);

    //create socket
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portno);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);

    ///bind the socket to the address and port number
	if(bind(my_sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) != 0) {
		perror("binding unsuccessful");
		exit(1);
	}

    //listen for connections from clients
	if(listen(my_sock,10) != 0) {
		perror("listening unsuccessful");
		exit(1);
	}

    fd_set current_sockets; // file descriptor sets (bitset)
    for (int i = 0; i < 10; i++) 
		clients[i] = 0;

	while (true) {
        FD_ZERO(&current_sockets); // clear fd set
        FD_SET(my_sock, &current_sockets);
        int maxfd = my_sock;

        for (int i = 0; i < 10; i++) {
            if (clients[i] > 0)
                FD_SET(clients[i], &current_sockets);
            if (clients[i] > maxfd)
                maxfd = clients[i];
        }

        if (select(maxfd + 1, &current_sockets, NULL, NULL, NULL) < 1) {
            perror("select error");
            exit(EXIT_FAILURE); 
        }

        if (FD_ISSET(my_sock, &current_sockets)) { // new connection
            if ((their_sock = accept(my_sock,(struct sockaddr *)&their_addr,&their_addr_size)) < 0) {
                perror("accept unsuccessful");
                exit(1);
            }
            inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
            printf("%s connected\n",ip);

            for (int j = 0; j < 10; j++) {
                if (clients[j] == 0) {
                    clients[j] = their_sock;
                    break;
                }
            }
        } else {
            for (int i = 0; i < 10; i++) {
                int sfd = clients[i];
                if (FD_ISSET(sfd, &current_sockets)) {
                        char msg[500];
                        int len = recv(sfd, msg, 500, 0);
                        if (len == 0) {
                            close(sfd);
                            clients[i] = 0;
                        } else {
                            msg[len] = '\0';
                            for (int j = 0; j < 10; j++) {
                                if (clients[j] != 0 && clients[j] != sfd) {
                                    if (send(clients[j], msg, strlen(msg),0) < 0) {
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