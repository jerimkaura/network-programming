#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>


#define PORT 9200
#define BUFFMAX 1024
#define MAXCLIENTS 20
#define SERVER_IP "127.0.0.1"


struct sockaddr_in sockaddr_to;
char buffer[BUFFMAX] = {0};
char name[BUFFMAX] = {0};
int sockfd;
struct sockaddr_in sockaddr_from;
socklen_t from_len;
volatile sig_atomic_t flag = 0;

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void get_input(char *buffer, char *msg){
  printf("%s",msg);
  fgets(buffer,BUFFMAX,stdin);
  buffer[strcspn(buffer,"\n")] = '\0';
}

void fill_sockaddr(struct sockaddr_in *sa, char *ip, int port){
  memset(sa, 0, sizeof(struct sockaddr_in));
  sa->sin_family = AF_INET;
  sa->sin_port = htons(port);
  inet_pton(AF_INET, ip, &(sa->sin_addr));
}

char *get_name(struct sockaddr_in *sa, char *name){
  inet_ntop(sa->sin_family, &(sa->sin_addr),name,BUFFMAX);
  return name;
}

void recv_msg(){
  while(1){
    int reci = recvfrom(sockfd,buffer,BUFFMAX,MSG_WAITALL,(struct sockaddr *)
        &sockaddr_from, &from_len);
    buffer[reci] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    if(reci>0){
      printf("Received %d bytes from %s: %s\n",reci,get_name(&sockaddr_from,name),buffer);
    } else if(reci == 0){
      break;
    }
    memset(buffer,0,sizeof(buffer));
  }
}

void send_msg(){
  while(1){
    get_input(buffer,"> ");
    if(strcmp(buffer,"exit")==0)
      break;
    else{
      sendto(sockfd, buffer,strlen(buffer),MSG_CONFIRM,(struct sockaddr *) &sockaddr_to, sizeof(sockaddr_to));
    }
    bzero(buffer,sizeof(buffer));
  }
}

int main(){
  signal(SIGINT, catch_ctrl_c_and_exit);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  get_input(buffer,"Please type 'join'\n");
 
  fill_sockaddr(&sockaddr_to, SERVER_IP, PORT);
  int numbytes = sendto(sockfd,buffer,strlen(buffer),0,
    (struct sockaddr *)&sockaddr_to, sizeof(sockaddr_to));
  printf("Send %d bytes to %s:%d\n", numbytes, get_name(&sockaddr_to, name),
    ntohs(sockaddr_to.sin_port));
  

  from_len = sizeof(sockaddr_from);
  numbytes = recvfrom(sockfd, buffer, BUFFMAX, 0, (struct sockaddr *) &sockaddr_from,
    &from_len);
  buffer[numbytes] = '\0';
  buffer[strcspn(buffer, "\n")] = '\0';
  printf("Received %d bytes from %s:%d: %s\n",numbytes,get_name(&sockaddr_from,name),
    ntohs(sockaddr_from.sin_port),buffer);
  if(!strcmp(buffer,"Joined successful")){
    pthread_t send_thread;
    if(pthread_create(&send_thread,NULL,(void *)send_msg,NULL)!=0){
      printf("ERROR: pthread\n");
      return EXIT_FAILURE;
    }
    pthread_t recv_thread;
    if(pthread_create(&recv_thread, NULL, (void *) recv_msg, NULL) != 0){
      printf("ERROR: pthread\n");
      return EXIT_FAILURE;
    }
    while (1){
      if(flag){
        printf("\nBye\n");
        break;
        }
    }

  }
  strcpy(buffer,"exit");
  numbytes = sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *) &sockaddr_to,
    sizeof(sockaddr_to));
  printf("Send %d bytes to %s:%d\n",numbytes,get_name(&sockaddr_to,name),
    ntohs(sockaddr_to.sin_port));
  close(sockfd);
  return 0;
}