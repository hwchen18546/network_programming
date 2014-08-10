//slave.h

#ifndef _slave_h
#define _slave_h

        #include <unistd.h>     //read write
        #include <cstdio>
        #include <string.h>
        #include <stdio.h>
        #include <stdlib.h> //setenv()
        #include <sys/wait.h>
        #include <ctype.h> //isdigit

        #include <iostream>
        using namespace std;

        #include <sys/ipc.h>        //shm
        #include <sys/shm.h>
        #include <sys/types.h>

        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>  //inet_ntoa

        #include <signal.h>
        #include <sys/select.h>

        //聊天室client的class
        #define MAXCLI 30
        class client{
            public:
                char exit[100];                                         //和yell共用
                char path[MAXCLI][100];
                int SIGUSR_NO;
                int sendid[MAXCLI];
                int recvid[MAXCLI];
                int total_cli;
                int newsockfd[MAXCLI];                          //主要server存有各別服務client的server的FD，從3起跳
                struct sockaddr_in cli_addr[MAXCLI];        //每個client連進來的資料
                int clilen[MAXCLI];                                 //sizeof(C[i].cli_addr)
                int id[MAXCLI];
                int childpid[MAXCLI];
                int online[MAXCLI];
                char name[MAXCLI][15];                          //client名稱
                char message[MAXCLI][100];
                int nowline[MAXCLI];                                //計算垂直cmd
                int future[MAXCLI];                     // n的值
                client(){
                    total_cli = 0;
                    for(int i=0; i<MAXCLI ;i++){
                        online[i] = 0;
                        nowline[i] = 0;
                        future[i] = 0;
                    }
                }
        };
        class must{
            public:
                int unknown[MAXCLI];
                int fifo_open[MAXCLI][MAXCLI];
                    //fifo_open[sender][reader]
                char broadmsg[100];
                char fifo_sendmsg[100];
                char fifo_recvmsg[100];
                char tellname[100];
        };

        extern class client C;
        extern class must *M;

        extern char inputline[20000];
        extern char inputline_copy[20000];
        extern int glo_clientid;
        extern int fifo_to;
        extern int fifo_from;
        //
        void signalhandler(int signo);
        void killpeople(int cast);
        //聊天室保留字
            int chatword(char *cut, int clientid);

        //找最小的clientid給新的人
        int findID();

        //計算水平cmd的Class和參數
        extern int total_cmd ;
        class cmd {
          public:
            char *name;
            int pi[2];
            int no;
            cmd(){no = 0 ;}
        };
        //計算垂直cmd的Class和參數
        class verpipe {
          public:
            int pi[2];
            int open;
            verpipe(){open = 0;}
        };

        extern class verpipe VPI[MAXCLI][3100];

        //Function類
        int shell(int clientid);                            //最外層
        int getcmd(char *cut);      // 把inputline切成一個個cmd的物件
        int writedata(char *cut);   // > stdout寫到檔案 、>>複寫檔案
            extern int writeagain ;
        int readdata(char *cut);        // < stdin讀到檔案
        int holddata(char *last);   // 有無pipe N
            extern int savepipe ;                   // 有|n的話savepipe=1
            extern int errpipe ;                        // 有!n的話errpipe=1
            extern int future ;                     // n的值
        int readcmd(char *haha);    // 執行指令

#endif
