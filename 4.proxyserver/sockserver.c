#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif

#define BUFSIZE 10000

struct sockaddr_in cli_addr;

void handle_socket(int browser_fd)
{
    int j, file_fd, buflen, len;
    long i, ret;
    char * fstr;
    char buffer[BUFSIZE];
    unsigned char request[1000];    //0~255
        memset(request, '\0', 1000);
    unsigned char reply[8];
        memset(reply, '\0', 8);

    struct sockaddr_in web_server_addr;

    //讀取瀏覽器的sock4_request
    ret = read(browser_fd,request,1000);

    //Parse Request的欄位
    unsigned char VN = request[0] ;
    unsigned char CD = request[1] ;
    unsigned int DST_PORT = request[2] << 8 | request[3] ;  //位元右移2^8，因為一個char = 8 bit
    unsigned int DST_IP = request[7] << 24 | request[6] << 16 | request[5] << 8 | request[4] ;  // 反過來
        // IP= request[4].request[5].request[6].request[7]
    char* USER_ID = request + 8 ;

    if(VN!=0x04)    //非sock4不收
        exit(0);

    //檢測Firewall
    FILE *firewall_fd;
    if((firewall_fd=fopen("socks.conf","r"))==NULL)
            printf("Fail to read socks.conf\n");

    char rule[10];
    char mode[10];
    char address_string[20];
    unsigned char address[4];   //Permit IP
    char * pch;

    fscanf(firewall_fd,"%s %s %s",rule,mode,address_string);
    while(!feof(firewall_fd)){
            pch = strtok(address_string,".");
            address[0] = (unsigned char)atoi(pch);
            pch = strtok(NULL,".");
            address[1] = (unsigned char)atoi(pch);
            pch = strtok(NULL,".");
            address[2] = (unsigned char)atoi(pch);
            pch = strtok(NULL,".");
            address[3] = (unsigned char)atoi(pch);
            //printf("%s    %s %u.%u.%u.%u \n",rule,mode,address[0],address[1],address[2],address[3]);  //print rule

            if( (!strcmp(mode,"c") && CD==0x01) || (!strcmp(mode,"b") && CD==0x02)){
                    //printf("match mode: %s    %s %u.%u.%u.%u \n",rule,mode,address[0],address[1],address[2],address[3]);  //print rule
                    if( ((address[0] == request[4]) || (address[0] == 0x00)) && ((address[1] == request[5]) || (address[1] == 0x00)) && ((address[2] == request[6]) || (address[2] == 0x00)) && ((address[3] == request[7]) || (address[3] == 0x00)) ){
                        //printf("Firewall Rule = Accept \n");
                        reply[1] = 0x5A; // 90
                        break;
                    }
                    else{
                        //printf("Firewall Rule = Reject \n");
                        reply[1] = 0x5B; // 91
                    }
            }
            else{
                reply[1] = 0x5B; // 91
            }
            memset(rule,'\0',sizeof(rule));
            memset(mode,'\0',sizeof(mode));
            memset(address_string,'\0',sizeof(address_string));
            fscanf(firewall_fd,"%s %s %s",rule,mode,address_string);
     }

            //秀出Sock Request 資訊
            printf("--------------------------------------------------------\n");
            printf("VN=%u CD=%u(%s) USER_ID=%s \n",VN,CD,(CD==0x01)?"CONNECT":"BIND",USER_ID);
            printf("Source Addr = %s(%u) \n",inet_ntoa(cli_addr.sin_addr),cli_addr.sin_port);
            printf("Destination Addr = %u.%u.%u.%u(%u) \n",request[4],request[5],request[6],request[7],DST_PORT);
            printf("Firewall Rule = %s\n", (reply[1] == 0x5A)?"Accept":"Reject");
            if(reply[1] == 0x5A)
                printf("Pass Rule: %s  %s  %u.%u.%u.%u \n",rule,mode,address[0],address[1],address[2],address[3]);
            else
                printf("Can't Pass Rule\n");
            printf("--------------------------------------------------------\n");

    fd_set rfds;
    fd_set afds;

    //connect mode
    if(CD==0x01){

            //回reply
            reply[0] = 0;
            //reply[1] = 0x5A ;
            reply[2] = request[2];
            reply[3] = request[3];
            reply[4] = request[4];
            reply[5] = request[5];
            reply[6] = request[6];
            reply[7] = request[7];
            write(browser_fd, reply, 8);

            //建立Dest Addr 的 Socket
            int web_fd;
            web_fd = socket(AF_INET,SOCK_STREAM,0);
            bzero((char *) &web_server_addr, sizeof(web_server_addr));
            web_server_addr.sin_family = AF_INET;
            web_server_addr.sin_addr.s_addr = DST_IP;
            web_server_addr.sin_port = htons(DST_PORT);
            //printf("%u %u %u\n",DST_PORT,web_server_addr.sin_port,ntohs(web_server_addr.sin_port));       // 21 5376 21

            if(connect(web_fd,(struct sockaddr *)&web_server_addr,sizeof(web_server_addr)) == -1){
                    printf("Connect web server fail\n");
                    exit(1);
            }
            else{
                    //printf("Connect web server success\n");
            }

            int nfds = ((browser_fd<web_fd)?web_fd:browser_fd)+1;
            int browser_read_end = 0;
            int websever_read_end = 0;
            FD_ZERO(&afds);
            FD_SET(web_fd,&afds);
            FD_SET(browser_fd,&afds);

            FILE *log_fd;
            if((log_fd=fopen("log.txt","w"))==NULL)
                printf("not open\n");

            while(1){

                    FD_ZERO(&rfds);
                    memcpy(&rfds, &afds, sizeof(rfds));
                    if(select(nfds,&rfds,NULL,NULL,NULL) < 0){
                            printf("Select fail\n");
                            exit(1);
                    }
                    if(FD_ISSET(browser_fd,&rfds)){
                            memset(buffer, '\0', BUFSIZE);
                            ret = read(browser_fd,buffer,BUFSIZE);   /* 讀取瀏覽器要求 */
                            //printf("1.Read from brow: %d\n",ret);
                            if (ret==0) {
                                //printf("Browser End!!\n");
                                FD_CLR(browser_fd, &afds);
                                browser_read_end = 1;
                            }
                            else if(ret==-1){
                                //printf("Browser Error!!\n");
                                //perror("Browser");
                                FD_CLR(browser_fd, &afds);
                                browser_read_end = 1;
                            }
                            else{
                                ret = write(web_fd,buffer,ret);   /* 讀取瀏覽器要求 */
                                //printf("2.Write to server: %d\n",ret);
                                //printf("%s",buffer);
                                //fprintf(log_fd,"%s", buffer);
                            }
                    }
                    if(FD_ISSET(web_fd,&rfds)){
                            memset(buffer, '\0', BUFSIZE);
                            ret = read(web_fd,buffer,BUFSIZE);
                            //printf("3.Read from server: %d\n",ret);
                            //printf("%s",buffer);
                            if (ret==0) {
                                //printf("Webserver End!!\n");
                                FD_CLR(web_fd, &afds);
                                websever_read_end = 1;
                                break;
                            }
                            else if (ret==-1) {
                                //printf("Webserver Error!!\n");
                                FD_CLR(web_fd, &afds);
                                //perror("Webserver");
                                websever_read_end = 1;
                                break;
                            }
                            else{
                                ret = write(browser_fd,buffer,ret);     //不能用strlen，因為不能保證資料中間沒有/0，如果有，buffer的內容不會全部送出去
                                //printf("4.Write to brows: %d\n",ret);
                                //printf("==========================\n");
                                //printf("%s",buffer);
                                //fprintf(log_fd,"%s", buffer);

                            }
                    }
            }
            close(browser_fd);
            close(web_fd);
            fclose(log_fd);
    }
    else if(CD==0x02){
        //printf("Bind Mode\n");
        int bindfd;
        static struct sockaddr_in bind_addr;

        if ((bindfd=socket(AF_INET, SOCK_STREAM,0))<0)
            exit(3);

        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind_addr.sin_port = htons(INADDR_ANY);

        /* 開啟網路監聽器 */
        if (bind(bindfd, (struct sockaddr *)&bind_addr,sizeof(bind_addr))<0){
            printf("Bind fail\n");
            exit(3);
        }

        int z, sa_len;
        struct sockaddr_in sa;
        sa_len = sizeof sa;
        z = getsockname(bindfd, (struct sockaddr *)&sa, &sa_len);
        if ( z == -1 )
            printf("getsockname fail\n");
        //printf("getsockname = %s(%u) %u \n",inet_ntoa(sa.sin_addr),sa.sin_port,ntohs(sa.sin_port));

        /* 開始監聽網路 */
        if (listen(bindfd,5)<0)
            exit(3);

        reply[0] = 0;
        //reply[1] = 0x5A ;     //上面firewall已經給定了
        reply[2] = (unsigned char)(ntohs(sa.sin_port) / 256);
        reply[3] = (unsigned char)(ntohs(sa.sin_port) % 256);
        reply[4] = 0;
        reply[5] = 0;
        reply[6] = 0;
        reply[7] = 0;
        write(browser_fd, reply, 8);
        //printf("reply[2] = %u reply[3] = %u\n",reply[2],reply[3]);

        int ftp_fd;
        struct sockaddr_in ftp_addr;
        size_t length = sizeof(ftp_addr);
        if ((ftp_fd = accept(bindfd, (struct sockaddr *)&ftp_addr, (socklen_t*)&length))<0){
            printf("accept fail!\n");
            exit(3);
        }
        //printf("accept success!\n");
        write(browser_fd, reply, 8);        //再寫一次

        fd_set rfds;
        fd_set afds;
        int nfds = ((browser_fd<ftp_fd)?ftp_fd:browser_fd)+1;
        FD_ZERO(&afds);
        FD_SET(ftp_fd,&afds);
        FD_SET(browser_fd,&afds);

            while(1){

                    FD_ZERO(&rfds);
                    memcpy(&rfds, &afds, sizeof(rfds));
                    if(select(nfds,&rfds,NULL,NULL,NULL) < 0){
                            printf("Select fail\n");
                            exit(1);
                    }
                    if(FD_ISSET(browser_fd,&rfds)){
                            memset(buffer, '\0', BUFSIZE);
                            ret = read(browser_fd,buffer,BUFSIZE);   /* 讀取瀏覽器要求 */
                            //printf("1.Read from brow: %d\n",ret);
                            if (ret==0) {
                                //printf("Browser End!!\n");
                                FD_CLR(browser_fd, &afds);
                            }
                            else if(ret==-1){
                                //printf("Browser Error!!\n");
                                //perror("Browser");
                                FD_CLR(browser_fd, &afds);
                            }
                            else{
                                ret = write(ftp_fd,buffer,ret);   /* 讀取瀏覽器要求 */
                                //printf("2.Write to server: %d\n",ret);
                                //printf("%s",buffer);
                                //fprintf(log_fd,"%s", buffer);
                            }
                    }
                    if(FD_ISSET(ftp_fd,&rfds)){
                            memset(buffer, '\0', BUFSIZE);
                            ret = read(ftp_fd,buffer,BUFSIZE);
                            //printf("3.Read from server: %d\n",ret);
                            //printf("%s",buffer);
                            if (ret==0) {
                                //printf("Webserver End!!\n");
                                FD_CLR(ftp_fd, &afds);
                                break;
                            }
                            else if (ret==-1) {
                                //printf("Webserver Error!!\n");
                                FD_CLR(ftp_fd, &afds);
                                //perror("Webserver");
                                break;
                            }
                            else{
                                ret = write(browser_fd,buffer,ret);     //不能用strlen，因為不能保證資料中間沒有/0，如果有，buffer的內容不會全部送出去
                                //printf("4.Write to brows: %d\n",ret);
                                //printf("==========================\n");
                                //printf("%s",buffer);
                                //fprintf(log_fd,"%s", buffer);

                            }
                    }
            }
            close(browser_fd);
            close(ftp_fd);

    }
}

main(int argc, char **argv)
{
    int i, pid, listenfd, socketfd;
    size_t length;
    static struct sockaddr_in proxy_addr;

    setenv("REQUEST_METHOD","GET",1);
    setenv("CONTENT_LENGTH","0",1);
    setenv("SCRIPT_NAME","/cgi/printenv.cgi",1);
    setenv("AUTH_TYPE","auth",1);
    setenv("REMOTE_USER","user",1);
    setenv("REMOTE_IDENT","ident",1);

    if(argc==1)
        printf("web server default port 11111\n");

    int port;
    if(argc==2)
        port=(u_short)atoi(argv[1]);
    else
        port=11111;

    /* //背景繼續執行 child變成放牛吃草 要用kill PID殺掉
    if(fork() != 0)
        return 0;
    */
    /* 讓父行程不必等待子行程結束 */
    signal(SIGCLD, SIG_IGN);

    /* 開啟網路 Socket */
    if ((listenfd=socket(AF_INET, SOCK_STREAM,0))<0)
        exit(3);

    /* 網路連線設定 */
    proxy_addr.sin_family = AF_INET;
    /* 使用任何在本機的對外 IP */
    proxy_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* 使用 default or argv[1] Port */
    proxy_addr.sin_port = htons(port);

    //讓結束之後不用等待
    int optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    /* 開啟網路監聽器 */
    if (bind(listenfd, (struct sockaddr *)&proxy_addr,sizeof(proxy_addr))<0){
        printf("Bind fail\n");
        exit(3);
    }

    /* 開始監聽網路 */
    if (listen(listenfd,64)<0)
        exit(3);

    char env_tem[100];
    while(1) {
        length = sizeof(cli_addr);
        /* 等待客戶端連線 */
        if ((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, (socklen_t*)&length))<0)
            exit(3);
        sprintf(env_tem,"%s",inet_ntoa(cli_addr.sin_addr));
        setenv("REMOTE_ADDR",env_tem,1);
        setenv("REMOTE_HOST",env_tem,1);

        /* 分出子行程處理要求 */
        if ((pid = fork()) < 0) {
            exit(3);
        } else {
            if (pid == 0) {  /* 子行程 */
                close(listenfd);
                handle_socket(socketfd);
            } else { /* 父行程 */
                close(socketfd);
            }
        }
    }
}
