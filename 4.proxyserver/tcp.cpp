#include "htm.h"
#include "tcp.h"

int NowRun =0;
int client_fd[5];
fd_set afds;


void conncet_server(int id){

    client_fd[id]  = socket(AF_INET,SOCK_STREAM,0);

    int flag = fcntl(client_fd[id], F_GETFL, 0);
    fcntl(client_fd[id], F_SETFL, flag | O_NONBLOCK);
            int n;
            if(n = connect(client_fd[id],(struct sockaddr *)&serv_addr[id],sizeof(serv_addr[id])) == -1)
            {
                    if (errno != EINPROGRESS){
                        printf("<script>document.all['m%d'].innerHTML += \"<xmp>Connect fail</xmp>\";</script>\n",id);
                        URL.on[id] = 881;
                    }
                    if (errno == EINPROGRESS)
                        ;//printf("non-block<br>");
            }
            //else{
                    FD_SET(client_fd[id],&afds);
                    NowRun++;
                    sleep(0.5);     //waiting for welcome messages for non-block
            //}

}

void conncet_sock_server(int id){

    client_fd[id]  = socket(AF_INET,SOCK_STREAM,0);
    int flag = fcntl(client_fd[id], F_GETFL, 0);
    fcntl(client_fd[id], F_SETFL, flag | O_NONBLOCK);
            int n;
            if(n = connect(client_fd[id],(struct sockaddr *)&sock_serv_addr[id],sizeof(sock_serv_addr[id])) == -1)
            {
                    if (errno != EINPROGRESS){
                        printf("<script>document.all['m%d'].innerHTML += \"<xmp>Connect sock fail</xmp>\";</script>\n",id);
                        URL.on[id] = 881;
                    }
                    if (errno == EINPROGRESS)
                        ;//printf("non-block<br>");
            }

            FD_SET(client_fd[id],&afds);
            NowRun++;
            sleep(0.5);     //waiting for welcome messages for non-block

        if(URL.on[id] != 881){
            char* pch;
            /* 送Sock_reply */
            unsigned char request[20];
            memset(request,'\0',20);
            unsigned char reply[8];
            memset(reply,'\0',8);
            request[0] = 0x04;
            request[1] = 0x01;  //connect mode;

            request[2] = (unsigned char)(ntohs(serv_addr[id].sin_port) / 256);
            request[3] = (unsigned char)(ntohs(serv_addr[id].sin_port) % 256);

                pch = strtok(inet_ntoa(serv_addr[id].sin_addr),".");
            request[4] = (unsigned char)atoi(pch);
                pch = strtok(NULL,".");
            request[5] = (unsigned char)atoi(pch);
                pch = strtok(NULL,".");
            request[6] = (unsigned char)atoi(pch);
                pch = strtok(NULL,".");
            request[7] = (unsigned char)atoi(pch);
            //printf("<script>document.all['m%d'].innerHTML += \"<xmp>%u.%u.%u.%u.%u.%u.%u.%u</xmp>\";</script>\n",id,request[0],request[1],request[2],request[3],request[4],request[5],request[6],request[7],request[8]);
            sleep(1);   //non-block要睡一下
            write(client_fd[id],request,10);
            sleep(1);
            read(client_fd[id],reply,8);
            //printf("<script>document.all['m%d'].innerHTML += \"<xmp>%u.%u.%u.%u.%u.%u.%u.%u</xmp>\";</script>\n",id,reply[0],reply[1],reply[2],reply[3],reply[4],reply[5],reply[6],reply[7],reply[8]);
            if(reply[1] != 0x5A){
                printf("<script>document.all['m%d'].innerHTML += \"<xmp>Firewall Block!</xmp>\";</script>\n",id);
                URL.on[id] = 881;
            }
        }


}

FILE *fp[5];

void create_files(int id){

        char filedir[50];
        strcpy(filedir,"test/");
        strcat(filedir , URL.fname_string[id]);
        fp[id] = fopen(filedir, "r");
}

char readtxt_buf[10000];

void read_files_send(int id){

            //讀測資的資訊
            int len;
            for(int j = 0 ;j<10000;j++)
                readtxt_buf[j] = '\0';

            len = readline(fileno(fp[id]),id,readtxt_buf,sizeof(readtxt_buf));
            if(len < 0)
                exit(1);
            else if (len==0)
                return;
            readtxt_buf[len+1] = '\0';
            write(client_fd[id], readtxt_buf, strlen(readtxt_buf)); //送了    //server會幫砍尾巴渣渣

            while(readtxt_buf[strlen(readtxt_buf)-1] == '\n' || readtxt_buf[strlen(readtxt_buf)-1] == '\r')
                    readtxt_buf[strlen(readtxt_buf)-1] = '\0' ;     //砍尾巴渣渣，不然strcmp會比不了

            printf("<script>document.all['m%d'].innerHTML += \"<b>%s</b><br>\";</script>\n",id,readtxt_buf);
            fflush(stdout);

            STARDREAD[id] = 0;
}

int readline(int fd,int id,char *ptr,int maxlen)
{
    int n,rc;
    char c;
    *ptr = 0;
    for(n=1;n<maxlen;n++)
    {
        if((rc=read(fd,&c,1)) == 1)
        {
            *ptr++ = c;
            if(c==' '&& *(ptr-2) =='%'){
                    STARDREAD[id] = 1;
                    fflush(stdout);
                    break;
            }
            if(c=='\n')  break;
        }
        else if(rc==0)
        {
            if(n==1)     return(0);
            else         break;
        }
        else
            return(-1);
    }
    return(n);
}
