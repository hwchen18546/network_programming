#include "shell.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, clilen, childpid;
    struct sockaddr_in cli_addr, serv_addr;
    int SERV_TCP_PORT;

    if(argc == 2)
    SERV_TCP_PORT = (u_short)atoi(argv[1]);
    else
    SERV_TCP_PORT = 12345;
    /*
    * Open a TCP socket (an Internet stream socket).
    */
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    perror("server: can't open stream socket");	//fprintf(stderr, "Error: Unable to fork.\n");
    /*
    * Bind our local address so that the client can send to us.
    */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_TCP_PORT);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    perror("server: can't bind local address");
    listen(sockfd, 5);

    for ( ; ; ) {
        clilen = sizeof(cli_addr);
        //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        newsockfd = accept(sockfd, (sockaddr *)&cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0) perror("server: accept error");
        if ( (childpid = fork()) < 0) perror("server: fork error");
        else if (childpid == 0) {
            /* child process */
            /* close original socket */
            close(sockfd);
            /* process start*/
            dup2(newsockfd,0);
            dup2(newsockfd,1);
            dup2(newsockfd,2);
            shell();
            exit(0);
        }
        close(newsockfd); /* parent process */
    }
}

