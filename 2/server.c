#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CUR 100
#define MAX_ACC 10
#define MAX_TEXT 1000


struct MessageStore{
    char name[MAX_CUR];
    char data[MAX_TEXT];
};

struct MessageStore message[MAX_ACC];

int checkName(char name[]){
    for (int i = 0; i < MAX_ACC; ++i) {
        if (strcmp(message[i].name,name)==0){
            return i;
        }
    }
    return -1;
}

int nextPointer(){
    for (int i = 0; i < MAX_ACC; ++i) {
        if (strcmp(message[i].data,"\0")==0 && strcmp(message[i].name,"\0")==0){
            return i;
        }
    }
    return -1;
}


char *sendMsg(int index){
    char buffer[MAX_TEXT];
    if (strlen(message[index].data)!=0){
        strcpy(buffer,message[index].data);
        strcpy(message[index].data, "\0");
        return strdup(&buffer[0]);
    }
    return "\n";
}

void receive(int sindex, int dindex, char buff[]){
    char buffer[MAX_TEXT];
    strcpy(buffer,message[sindex].name);
    strcat(buffer," -> ");
    strcat(buffer,buff);
    strcat(buffer,"\n");
    strcat(message[dindex].data,buffer);
}

int addPerson(int client){
    char name[MAX_CUR];
    char buff[MAX_TEXT];
    int current;
    bzero(buff,MAX_TEXT);
    strcpy(buff,"Enter name: ");
    write(client, buff, sizeof(buff));
    bzero(name,MAX_CUR);
    read(client, name, sizeof(name));
    if (checkName(name)==-1) {
        current = nextPointer();
        if (current == -1) {
            bzero(buff,MAX_TEXT);
            strcpy(buff,"Full Buffer!");
            write(client, buff, sizeof(buff));
            return -1;
        } else {
            strcpy(message[current].name, name);
        }
        return current;
    } else{
        current = checkName(name);
    }
    return current;
}


int addFriend(char name[]){
    int current;
    if (checkName(name)==-1) {
        current = nextPointer();
        if (current == -1) {
            return -1;
        } else {
            strcpy(message[current].name, name);
        }
        return current;
    } else{
        current = checkName(name);
    }
    strcpy(message[current].name, name);
    return current;
}

void error(const char *msg){
    perror(msg);
    exit(1);
}


void transmit(int index,int sockfeed){
    int n, indexf;
    char buffer[MAX_TEXT];
    char temp[MAX_CUR];
    bzero(temp,MAX_CUR);
    strcpy(temp,"\nWho do you want to text:");
    int i=0;
    while(i<2){
        if(i<1){
            bzero(buffer,MAX_TEXT);
            strcpy(buffer,sendMsg(index));
            strcat(buffer,temp);
            write(sockfeed,buffer,sizeof(buffer));
        }
        bzero(buffer,MAX_TEXT);
        memset(buffer ,0 , MAX_TEXT);
        int n = read(sockfeed, buffer, sizeof(buffer));
        if(n<0){
            error("Error in reading from the socket");
        }
        if(strcmp(buffer,"\0")!=0)
            indexf = addFriend(buffer);
        i++;
    }
    while(1 && indexf!= -1){
        bzero(buffer,MAX_TEXT);
        strcpy(buffer,"Input > ");
        write(sockfeed,buffer,sizeof(buffer));

        bzero(buffer,MAX_TEXT);
        int n = read(sockfeed, buffer, sizeof(buffer));
        if(n<0){
            error("Error in reading from the socket");
        }
        if(strcmp(buffer,"exit")==0){
            break;
        }
        receive(index,indexf,buffer);
        //printf("<%d %d> <%s>\n",index,indexf,message[indexf].data);
    }
}

int main(int argc, char **argv){
    // Create the socket
    int server_sock, newsocket_conn;
    char *ip = "127.0.0.1";
    int port, index=-1;
    if(argc<2){
        port = 9200;
    }else{
        port = atoi(argv[1]);
    }
    socklen_t client;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock<0){
        error("Server failed connection!");
    }

    // Define the address
    struct sockaddr_in server_address, client_addr;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    // Bind the server
    if(bind(server_sock, (struct sockaddr *) &server_address, sizeof(server_address))<0){
        error("Error binding the socket");
    }

    // Listen
    listen(server_sock, 5);
    printf("Listening for connection\n");
    int check = 1;
    int client_socket;
    // Accepting connection
    while(1){
        if(check>1){
            printf("Listening for connection...\n");
        }
        newsocket_conn = accept(server_sock, (struct sockaddr *) &client_addr, &client);
        if(newsocket_conn<0){
            error("Error during accept");
        }
        printf("Connection was created successfully\n");
        index = addPerson(newsocket_conn);
        if(strcmp(message[index].data,"\0")==0)
            strcat(message[index].data,"Good afternoon");
        transmit(index, newsocket_conn);
        close(newsocket_conn); // Closing accepted connection
        printf("Connection terminated\n");
        check++;
    }
    
    close(server_sock); // Closing socket connection
    return 0;
}