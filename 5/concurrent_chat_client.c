#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//contains a structure of storing addresses
#include <netinet/in.h>
int main(){
	//create a client socket
  int network_socket;
	//domain of socket, type of socket(sock_stream is for tcp. others include datagram sockets for udp), protocol is 0 for tcp
  	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// specify server address
	struct sockaddr_in server_address;
	// address family, 
	server_address.sin_family = AF_INET;
	// port, using htons to convert an integer port number to appropriate format
	server_address.sin_port = htons(6000);
	// bind the socket to a particular address
	// it is a struct within a struct with only one field
	server_address.sin_addr.s_addr = INADDR_ANY;	//this is the loopback address
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	if(connection_status == -1){
		printf("An error occured while establishing connection \n\n");
		exit(4);
	}

	char sendline[4096];
	printf("Enter message to send: ");
    	fgets(sendline, 4096, stdin);


	send(network_socket, sendline, strlen(sendline), 0);
	// receive data from the server
	char server_response[4096];
	
		if(recv(network_socket, &server_response, sizeof(server_response), 0)){
		
		// print the data you get back
		printf("The server sent the data: %s\n", server_response);
		memset(sendline, 0, sizeof(sendline));
					
		}
	
		close(network_socket);
  return 0;
}