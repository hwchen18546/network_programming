#include "slave.h"

fd_set rfds;    //read file descriptor set
fd_set afds;    //active file descriptor set
int nfds;
int main(int argc, char *argv[])
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int shmid1;
        if ((shmid1 = shmget((key_t)135, 1024, IPC_CREAT | 0666)) < 0) {
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
        int msockfd;
        struct sockaddr_in serv_addr;
        int SERV_TCP_PORT;

             if(argc == 2)
                 SERV_TCP_PORT = (u_short)atoi(argv[1]);
             else
                SERV_TCP_PORT = 1234;

        /*
        * Open a TCP socket (an Internet stream socket).
        */
        if ( (msockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("server: can't open stream socket"); //fprintf(stderr, "Error: Unable to fork.\n");
        /*
        * Bind our local address so that the client can send to us.
        */
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(SERV_TCP_PORT);
        if (bind(msockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        perror("server: can't bind local address");
        listen(msockfd, 5);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //nfds = getdtablesize();
        nfds = 100;
        FD_ZERO(&afds);
        FD_SET(msockfd,&afds);          //msockfd = master

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int i =0 ;
        int temp_newsockfd ,temp_clilen ;
        struct sockaddr_in temp_cli_addr;
        for (; ;) {
                memcpy(&rfds, &afds, sizeof(rfds));
                select(nfds, &rfds, (fd_set *)0 ,(fd_set *)0, NULL);

                if(FD_ISSET(msockfd, &rfds)){
                                temp_clilen = sizeof(temp_cli_addr);
                                temp_newsockfd = accept(msockfd, (sockaddr *)&temp_cli_addr, (socklen_t*)&temp_clilen);
                                C.total_cli++;

                                i = findID();

                                if(i == -1)     continue;
                                C.clilen[i] = temp_clilen;
                                C.cli_addr[i] = temp_cli_addr;
                                C.newsockfd[i] = temp_newsockfd;
                                FD_SET(C.newsockfd[i],&afds);

                                sprintf (M->broadmsg,"*** User '%s' entered from %s/%u. ***" , C.name[i] ,inet_ntoa(C.cli_addr[i].sin_addr) ,C.cli_addr[i].sin_port );

                                ///一進來初始化
                                strcpy(C.path[i], "bin:.");
                                C.future[i] = 0;
                                for(int j = 0; j<3100; j++)
                                    VPI[i][j].open = 0;

                                char temp[200];
                                sprintf(temp,"****************************************\n** Welcome to the information server. **\n****************************************\n");
                                write(C.newsockfd[i],temp, strlen(temp));

                                killpeople(100);
                                write(C.newsockfd[i], "% ", strlen("% "));
                                cout<<"  Total_cli = "<<C.total_cli<<"  New Client Get id="<<i+1<<endl<<endl;
                }

                for(int fd = 0; fd<nfds; fd++){
                        if(fd != msockfd && FD_ISSET(fd, &rfds)){
                                if(shell(fd-3) == 881){                             //id =1 fd = 4
                                    cout<<endl;                             //這行不加會有error@@ A離開B輸入會卡住...
                                    FD_CLR(fd, &afds);
                                }
                                else
                                    write(C.newsockfd[glo_clientid-1], "% ", strlen("% "));
                        }
                }
        }


}

