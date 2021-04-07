#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <fcntl.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//assumming the server is an organization chat bot that sends simple gretings while dealing with multiple clients
//then prompts a user agent to respond to that query
int main(){
	// create the master server socket
	int server_socket;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	// specify the server address
	struct sockaddr_in server_address;
		// address family, 
	server_address.sin_family = AF_INET;
		// port, using htons to convert an integer port number to appropriate format
	server_address.sin_port = htons(6000);
		// bind the socket to a particular address
	server_address.sin_addr.s_addr = INADDR_ANY;	//this is the loopback address
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	
	// listen for a max of 5 clients
	listen(server_socket, 5);
	printf("%s\n","Listening for connections....");
	
	// create a client socket
	int client_socket;
	int n;
	char buffer[4096];
	pid_t childpid;
	struct sockaddr_in cliaddr; //store client address information
	socklen_t clilen;
	
	
	while(1){
		//
		clilen = sizeof(cliaddr);
			// accept a connection (allow a client connection from the queue of the master server socket)
		client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, &clilen);
		printf("%s\n","Received request from client....");
		
		if( (childpid = fork()) == 0 ){
			// create new process using fork
			printf("%s\n", "Child created for dealing with the request");
			close(server_socket);//not sure this is what is to be closed
			
			while ( (n = recv(client_socket, buffer, 4096, 0)) > 0){
				printf("%s", "String received from the client: ");
				puts(buffer);
				char response[4096] = "Hae, we have received your query. Please wait as we process it and contact you.";
				send(client_socket, response, sizeof(response), 0);
				memset(&response, 0, sizeof(response));
		
			}
			if ( n < 0){printf("%s\n", "Read error");}
			exit(0);
			
		}
		close(client_socket);
	}
	
	return 0;
}