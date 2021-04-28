#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>

#define PORT 9200
#define BUFFMAX 1024
#define MAXCLIENTS 20
#define SERVER_IP "127.0.0.1"

typedef struct client_list *List;
typedef struct args *Args;

struct args{
	List list;
	int fd;
};

typedef struct client_node *Node;

struct client_list{
	Node head, tail;
	cnd_t t_lock;
	mtx_t mutex;
};

struct client_info{
	char host[INET_ADDRSTRLEN];
	int port;
};

struct client_node{
	struct client_info client;
	Node next, prev;
};

int client_equals(struct client_info a, struct client_info b){
	return (a.port == b.port && !strcmp(a.host, b.host));
}

Node list_find(List list, struct client_info client){
	assert(list != NULL);
	Node found = NULL;
	for(Node curr=list->head;curr!=NULL;curr=curr->next){
		if(client_equals(curr->client, client))
			found = curr;
	}
	return found;
}

void node_destroy(Node node){
	free(node);
}

void list_remove(List list, Node to_remove){
	if(to_remove == NULL){
		fprintf(stderr, "Tried to remove a node that wasn't in the list!");
		return;
	}
	if(list->head == to_remove){
		assert(to_remove->prev == NULL);
		list->head = to_remove->next;
	}
	if(list->tail == to_remove){
		assert(to_remove->next == NULL);
		list->tail = to_remove->prev;
	}
	if(to_remove->next) 
		to_remove->next = to_remove->next->next;
	if(to_remove->prev) 
		to_remove->prev = to_remove->prev->prev;
	node_destroy(to_remove);
}

void list_remove_client(List list, struct client_info client){
	Node to_remove = list_find(list, client);
	list_remove(list, to_remove);
}

void list_add_node(List list, Node node){
	assert(list != NULL);
	if(list->tail == NULL){
		assert(list->head == NULL);
		list->head = list->tail = node;
	} else {
		assert(list->head != NULL);
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	}
}


Node node_new(struct client_info client){
	Node node = calloc(1, sizeof(*node));
	node->client = client;
	return node;
}

void list_add(List list, struct client_info client){
	Node new = node_new(client);
	list_add_node(list, new);
}

char *get_time(void){
	time_t t = time(0);
	return ctime(&t);
}


struct client_info get_client_info(struct sockaddr_in *sa){
	struct client_info info = {};
	info.port = ntohs(sa->sin_port);
	inet_ntop(sa->sin_family,&(sa->sin_addr),info.host,INET_ADDRSTRLEN);
	return info;
}

char *print_client_buf(struct client_info client, char *buffer){
	sprintf(buffer,"%s:%d", client.host,client.port);
	return buffer;
}

int send_wrapper(int feed, char *buffer,struct sockaddr_in *sockaddr_to, socklen_t *to_len, char *sender){
	char name[BUFFMAX] = {0};
	int numbytes = sendto(feed, buffer, strlen(buffer), 0, (struct sockaddr *) sockaddr_to, *to_len);
	struct client_info client = get_client_info(sockaddr_to);
	printf("%s -> Send %d bytes to %s: %s\n", sender, numbytes, print_client_buf(client, name), buffer);
	return numbytes;
}

int recv_wrapper(int feed, char *buffer, struct sockaddr_in *sockaddr_from, socklen_t *from_len, char *sender){
	char name[BUFFMAX] = {0};
	printf("%s -> receiving\n", sender);
	int numbytes = recvfrom(feed,buffer,BUFFMAX,0,(struct sockaddr *) sockaddr_from, from_len);
	buffer[numbytes] = '\0';
	buffer[strcspn(buffer,"\n")] = '\0';
	struct client_info client = get_client_info(sockaddr_from);
	printf("%s -> reveiced %d bytes from %s: %s\n",sender,numbytes,print_client_buf(client, name),buffer);
	return numbytes;
}


int list_contains(List list, struct client_info client){
	assert(list != NULL);
	return (list_find(list,client)!=NULL);
}


void fill_sockaddr(struct sockaddr_in *sa, char *ip, int port){
	memset(sa, 0, sizeof(struct sockaddr_in));
	sa->sin_family = AF_INET;
	sa->sin_port = htons(port);
	inet_pton(AF_INET, ip, &(sa->sin_addr));
}

int recv_msg_handler(void *args_){
	Args args = (Args) args_;
	List list = args->list;
	int serverfd = args->fd;
	int clientfd = args->fd;
	char buffer[BUFFMAX+1] = {0};
	char buffer1[BUFFMAX+1] = {0};

	struct sockaddr_in sockaddr_from;
	socklen_t from_len = sizeof(sockaddr_from);
	while(1){
		recv_wrapper(serverfd,buffer,&sockaddr_from,&from_len,"recv");
		struct client_info client = get_client_info(&sockaddr_from);
		mtx_lock(&(list->mutex));
		printf("Received request from %s listening at %d: %s at time %s",
			client.host,client.port,buffer,get_time());
		if(!strcmp(buffer,"join")){
			list_add(list,client);
			strcpy(buffer,"Joined successful");
		}else if(!strcmp(buffer,"exit")){
			if(list_contains(list,client)){
				list_remove_client(list,client);
				strcpy(buffer,"Client removed");
			} else {
				strcpy(buffer,"You are not currently connected");
			}
		} else {
			for(Node curr = list->head; curr != NULL; curr=curr->next){
				struct sockaddr_in sockaddr_to = {0};
				socklen_t to_len = sizeof(sockaddr_to);
				fill_sockaddr(&sockaddr_to,curr->client.host, curr->client.port);
				send_wrapper(clientfd, buffer, &sockaddr_to, &to_len, "send");
			}
			cnd_signal(&(list->t_lock));
			printf("recv signaled\n");
			mtx_unlock(&(list->mutex));
			continue;
		}
		send_wrapper(serverfd,buffer,&sockaddr_from,&from_len,"recv");
		cnd_signal(&(list->t_lock));
		printf("recv signaled\n");
		mtx_unlock(&(list->mutex));
	}
	return EXIT_SUCCESS;
}




void print_list(List list){
	printf("Clients are:\n");
	char name[BUFFMAX] = {0};
	for(Node curr = list->head; curr != NULL; curr = curr->next){
		printf("%s -> ", print_client_buf(curr->client, name));
	}
	printf("---\n");
}


int send_msg_handler(void *arg){
	Args args = (Args) arg;
	List list = args->list;
	int clientfd = args->fd;
	char buf[BUFFMAX+1] = {0};
	char name[BUFFMAX] = {0};
	while(1){
		mtx_lock(&(list->mutex));
		print_list(list);
		/*for(Node curr = list->head; curr != NULL; curr=curr->next){
			//char *curr_time = get_time();
			//snprintf(buf,BUFFMAX, curr_time);
			//printf("Current Client: %s", print_client_buf(curr->client,name));
			struct sockaddr_in sockaddr_to = {0};
			socklen_t to_len = sizeof(sockaddr_to);
			fill_sockaddr(&sockaddr_to,curr->client.host, curr->client.port);
			send_wrapper(clientfd, buff, &sockaddr_to, &to_len, "send");
		}*/
		cnd_signal(&(list->t_lock));
		mtx_unlock(&(list->mutex));
		sleep(1);
	}
	return EXIT_SUCCESS;
}



Args new_args(List list, int feed){
	Args args = calloc(1, sizeof(*args));
	args->list = list;
	args->fd = feed;
	return args;
}

void get_input(char *buffer, char *msg){
	printf("%s",msg);
	fgets(buffer,BUFFMAX,stdin);
	buffer[strcspn(buffer,"\n")] = '\0';
}


char *get_name(struct sockaddr_in *sa, char *name){
	inet_ntop(sa->sin_family,&(sa->sin_addr), name, BUFFMAX);
	return name;
}


void print_client(struct client_info client){
	printf("%s:%d\n",client.host,client.port);
}



List list_new(void){
	List list = calloc(1, sizeof(*list));
	cnd_init(&list->t_lock);
	mtx_init(&list->mutex, mtx_plain);
	return list;
}



void list_destroy(List list){
	for(Node curr=list->head;curr!=NULL;){
		Node temp = curr;
		curr = curr->next;
		node_destroy(temp);
	}
	mtx_destroy(&list->mutex);
	cnd_destroy(&list->t_lock);
	free(list);
}





int main(){
	int clientfd = socket(AF_INET,SOCK_DGRAM,0);
    int listenfd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in sockaddr_to;
	fill_sockaddr(&sockaddr_to,SERVER_IP,PORT);
	const int so_reuseaddr = 1;
	setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),&so_reuseaddr,sizeof(int));
    
	bind(listenfd,(struct sockaddr *) &sockaddr_to,sizeof(sockaddr_to));

	List list = list_new();
	Args server_info = new_args(list, listenfd);
	Args client_info = new_args(list, clientfd);

	thrd_t recv_msg_thread;
	thrd_t send_msg_thread;

	thrd_create(&recv_msg_thread, recv_msg_handler, (void *) server_info);
	thrd_create(&send_msg_thread, send_msg_handler, (void *) client_info);
	while(1){
		usleep(100000);
	}
    
    close(listenfd);
	close(clientfd);
	int retval;
	thrd_join(recv_msg_thread, &retval);
	thrd_join(send_msg_thread, &retval);
	list_destroy(list);
    return 0;
}