#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN];
};

int clients[100];
int n = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// function to forward a message to all clients
// apart from the sender of the message
void send_threadoall(char *msg,int curr) {
	int i;
	pthread_mutex_lock(&mutex);
	for(i = 0; i < n; i++) {
		if(clients[i] != curr) {
			if(send(clients[i],msg,strlen(msg),0) < 0) {
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}

// function to allow the server to receive a message a client
void *recvmg(void *sock) {
	struct client_info cl = *((struct client_info *)sock);
	char msg[500];
	int len, i, j;
	while((len = recv(cl.sockno,msg,500,0)) > 0) { // check if a message exists
		msg[len] = '\0';
		send_threadoall(msg, cl.sockno); // send it to all clients
		memset(msg,'\0',sizeof(msg));
	}

	pthread_mutex_lock(&mutex);
	// if someone disconnects...
	printf("%s disconnected\n", cl.ip);
	for (i = 0; i < n; i++) {
		if (clients[i] == cl.sockno) {
			j = i;
			while (j < n-1) {
				clients[j] = clients[j+1];
				j++;
			}
		}
	}
	n--;
	pthread_mutex_unlock(&mutex);
}

int main(int argc,char *argv[]) {
	struct sockaddr_in my_addr,their_addr;
	int my_sock;
	int their_sock;
	socklen_t their_addr_size;

	int portno;

    // send & receive threads
	pthread_t send_thread,receive_thread;

    // max message length
	char msg[500];
	int len;
	struct client_info cl;
	char ip[INET_ADDRSTRLEN];
	if(argc > 2) {// two arguments maximum
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[1]);

    // create socket
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(portno);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);

    // bind the socket to the address and port number
	if(bind(my_sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) != 0) {
		perror("binding unsuccessful");
		exit(1);
	}

    // listen for connections from clients
	if(listen(my_sock,5) != 0) {
		perror("listening unsuccessful");
		exit(1);
	}

	while(1) {
        // accept connections
		if((their_sock = accept(my_sock,(struct sockaddr *)&their_addr,&their_addr_size)) < 0) {
			perror("accept unsuccessful");
			exit(1);
		}

		pthread_mutex_lock(&mutex); // no thread can access the locked region of the code
		inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
		printf("%s connected\n",ip);
		cl.sockno = their_sock;
		strcpy(cl.ip,ip);
		clients[n] = their_sock;
		n++;
		// create a receive thread to get messages from the client
		pthread_create(&receive_thread, NULL,recvmg, &cl);
		pthread_mutex_unlock(&mutex); 
	}
	return 0;
}