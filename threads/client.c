#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// receiving a message
void *recvmg(void *sock)
{
	int socket_fd = *((int *)sock);
	char msg[500];
	int len;
    // receive until end of message
	while((len = recv(socket_fd,msg,500,0)) > 0) {
		msg[len] = '\0';
        // print received message
		fputs(msg, stdout);
        // clear the message buffer
		memset(msg,'\0',sizeof(msg)); 
	}
}

int main(int argc, char *argv[]) {
	struct sockaddr_in socket_addr;
	int my_socket;
	int socket_fd;
	int socket_addr_size;
	int portno;
	pthread_t send_thread,receive_thread;
	char msg[500];
	char username[100];
	char res[600];
	char ip[INET_ADDRSTRLEN];
	int len;

    // check argument count.
	if(argc > 3) {
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[2]); // get the port number from argv[2]
	strcpy(username,argv[1]); // copy name from argv[1] into the username variable
	my_socket = socket(AF_INET,SOCK_STREAM,0); // create the socket
	memset(socket_addr.sin_zero,'\0',sizeof(socket_addr.sin_zero));

	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(portno);
	socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(my_socket,(struct sockaddr *)&socket_addr,sizeof(socket_addr)) < 0) {
		perror("connection not esatablished");
		exit(1);
	}
	inet_ntop(AF_INET, (struct sockaddr *)&socket_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n",ip);

    // craeting the thread to receive the message
	pthread_create(&receive_thread,NULL,recvmg,&my_socket);

	while(fgets(msg,500,stdin) > 0) { // get input from the client
		strcpy(res,username);
		strcat(res,": ");
		strcat(res,msg);
		len = write(my_socket,res,strlen(res));
		if(len < 0) { // if the write function return 0, the nothing was written
			perror("message not sent");
			exit(1);
		}
		memset(msg,'\0',sizeof(msg)); // clear msg
		memset(res,'\0',sizeof(res)); // clear res
	}

	pthread_join(receive_thread, NULL);
	close(my_socket);

	return 0;
}