#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define SIZE 1024

//send file function
void send_file(FILE *fp, int sockfd){
	int n;
	char data[SIZE] = {0};
	while(fgets(data, SIZE, fp) != NULL) {
		// send the data to the server
		if (send(sockfd, data, sizeof(data), 0) == -1) {
			perror("Error in sending file.");
			exit(1);
		}
		bzero(data, SIZE);// empty the data buffer
	}
}

int main(){
	//ip address
	char *ip = "127.0.0.1"; 

	//port number
	int port = 8080;

	int connection;

	int sockfd;
	struct sockaddr_in server_addr;
	FILE *fp;

	//filename 
	char *filename = "file.txt";

	//create the socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("Error  creating the socket");
		exit(1);
	}
	printf("Server socket created successfully.\n");

	//set the port and ip address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);

	//execute the connect function
	connection = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(connection == -1) {
		perror("Error in socket");
		exit(1);
	}
		printf("Connected to Server.\n");

	//open the filename specifed in read mode, the data read from the file is stored in a the variable
	//fp
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error in reading file.");
		exit(1);
	}

	// use the send_file function to sen the contents of the file saved in the fp variable
	send_file(fp, sockfd);
	printf("File data sent successfully.\n");
	printf("Closing the connection.\n");
	// close the connection
	close(sockfd);
	return 0;
}
