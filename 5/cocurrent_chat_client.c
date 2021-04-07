    
    #include <stdio.h>

    #include <stdlib.h>

    #include <unistd.h>

    #include <errno.h>

    #include <string.h>

    #include <sys/types.h>

    #include <sys/socket.h>

    #include <netinet/in.h>

    #include <arpa/inet.h>

    #include <netdb.h>

    /* the port users will be connecting to */

    #define MYPORT 4950

     

    int main(int argc, char *argv[ ])

    {

    int sockfd;

    /* connector’s address information */

    struct sockaddr_in their_addr;

    struct hostent *he;

    int numbytes;

     

    if (argc != 3)

    {

    fprintf(stderr, "Client-Usage: %s <hostname> <message>\n", argv[0]);

    exit(1);

    }

    /* get the host info */

    if ((he = gethostbyname(argv[1])) == NULL)

    {

    perror("Client-gethostbyname() error lol!");

    exit(1);

    }

    else

    printf("Client-gethostname() is OK...\n");

     

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)

    {

    perror("Client-socket() error lol!");

    exit(1);

    }

    else

    printf("Client-socket() sockfd is OK...\n");

     

    /* host byte order */

    their_addr.sin_family = AF_INET;

    /* short, network byte order */

    printf("Using port: 4950\n");

    their_addr.sin_port = htons(MYPORT);

    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    /* zero the rest of the struct */

    memset(&(their_addr.sin_zero), '\0', 8);

     

    if((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)

    {

    perror("Client-sendto() error lol!");

    exit(1);

    }

    else

    printf("Client-sendto() is OK...\n");

     

    printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));

     

    if (close(sockfd) != 0)

    printf("Client-sockfd closing is failed!\n");

    else

    printf("Client-sockfd successfully closed!\n");

    return 0;

    }