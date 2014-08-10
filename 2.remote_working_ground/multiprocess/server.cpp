#include "slave.h"

int main(int argc, char *argv[])
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int shmid1;
        must *M;
        if ((shmid1 = shmget((key_t)125, 1024, IPC_CREAT | 0666)) < 0) {
            perror("shmget1");
            exit(1);
        }
        if ((M = (must *)shmat(shmid1, NULL, 0)) == (must *) -1) {
            perror("shmat1");
            exit(1);
        }
        for(int i=0; i<MAXCLI ;i++){
            M->unknown[i] = 0 ;
            for(int j=0; j<MAXCLI ;j++)
                M->fifo_open[i][j] = 0;
        }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int shmid2;
        client *C;

        if ((shmid2 = shmget((key_t)124, 1024, IPC_CREAT | 0666)) < 0) {
            perror("shmget2");
            exit(1);
        }
        if ((C = (client *)shmat(shmid2, NULL, 0)) == (client *) -1) {
            perror("shmat2");
            exit(1);
        }
        C->total_cli=0;
        for(int i=0; i<MAXCLI ;i++)
            C->online[i] = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int sockfd;
        struct sockaddr_in serv_addr;
        int SERV_TCP_PORT;

             if(argc == 2)
                 SERV_TCP_PORT = (u_short)atoi(argv[1]);
             else
                SERV_TCP_PORT = 1324;

        /*
        * Open a TCP socket (an Internet stream socket).
        */
        if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("server: can't open stream socket"); //fprintf(stderr, "Error: Unable to fork.\n");
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int i =0 ;
        int temp_newsockfd ,temp_clilen ;
        struct sockaddr_in temp_cli_addr;
        for (; ;) {
                temp_clilen = sizeof(temp_cli_addr);
                temp_newsockfd = accept(sockfd, (sockaddr *)&temp_cli_addr, (socklen_t*)&temp_clilen);
                C->total_cli++;

                i = findID(C);
                killpeople(C,100);

                if(i == -1)     continue;
                C->clilen[i] = temp_clilen;
                C->cli_addr[i] = temp_cli_addr;
                C->newsockfd[i] = temp_newsockfd;

                sprintf (M->welcom,"*** User '%s' entered from %s/%u. ***" , C->name[i] ,inet_ntoa(C->cli_addr[i].sin_addr) ,C->cli_addr[i].sin_port );

                if (C->newsockfd[i] < 0)                perror("server: accept error");
                C->childpid[i] = fork();
                if ( C->childpid[i] < 0)    perror("server: fork error");
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                else if (C->childpid[i] == 0) { /* child process */
                        C->childpid[i] = getpid();
                        close(sockfd);
                        dup2(C->newsockfd[i],0);
                        dup2(C->newsockfd[i],1);
                        dup2(C->newsockfd[i],2);
                        shell(C->id[i]);            //!!!!!
                        exit(0);
                }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                else{
                        //sprintf (shm,"*** User '%s' entered from %s/%u. ***" , C->name ,inet_ntoa(C->cli_addr.sin_addr) ,C->cli_addr.sin_port );
                        cout<<"total_cli = "<<C->total_cli<<" Get i="<<i<<endl;
                        cout<<M->welcom<<endl;

                }
                close(C->newsockfd[i]); /* parent process */
        }
}

