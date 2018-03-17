#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>  /* netdb is necessary for struct hostent */

#define PORT 8002  /* server port */

#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
    int sockfd, num;    /* files descriptors */
    char buf[MAXDATASIZE];    /* buf will store received text */
    struct hostent *he;    /* structure that will get information about remote host */
    struct sockaddr_in server;
    
    if (argc != 2)
    {
        printf("Usage: %s <IP Address>\n",argv[0]);
        exit(1);
    }
    
    if((he=gethostbyname(argv[1]))==NULL)
    {
        printf("gethostbyname() error\n");
        exit(1);
    }
    
    if((sockfd=socket(AF_INET,SOCK_STREAM, 0))==-1)
    {
        printf("socket() error\n");
        exit(1);
    }
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    if(connect(sockfd, (struct sockaddr *)&server, sizeof(server))==-1){
        printf("connect error is %u",errno);
        exit(1);
    }

    close(sockfd);
    return 0;
}