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

#define BUFSIZE 8096

struct {
    char *ext;
    char *filetype;
} extensions [] = { //初始化
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"cgi", "cgi" },
    {"exe","text/plain" },
    {0,0} };

void handle_socket(int fd)
{
    int j, file_fd, buflen, len;
    long i, ret;
    char * fstr;
    static char buffer[BUFSIZE+1];

    ret = read(fd,buffer,BUFSIZE);   /* 讀取瀏覽器要求 */
    if (ret==0||ret==-1) {
     /* 網路連線有問題，所以結束行程 */
        exit(3);
    }

    //看完整Http request
    //printf("%s\n",buffer);

    /* 在讀取到的字串結尾補空字元，方便後續程式判斷結尾 */
    if (ret>0&&ret<BUFSIZE)
        buffer[ret] = 0;
    else
        buffer[0] = 0;

    /* 移除換行字元，這樣printf buffer只剩第一行 */
    for (i=0;i<ret;i++)
        if (buffer[i]=='\r'||buffer[i]=='\n')
            buffer[i] = 0;

    printf("--------------------------------------\n");
    printf("HTTP REQUEST\n");
    //只看第一行Http request
    printf("    %s\n",buffer);

    /* 只接受 GET 命令要求 */
    if (strncmp(buffer,"GET ",4)&&strncmp(buffer,"get ",4))
        exit(3);

    /* 我們要把 GET /index.html HTTP/1.0 後面的 HTTP/1.0 用空字元隔開，
    這樣printf buffer只剩下要get的內容了 */
    for(i=4;i<BUFSIZE;i++) {
        if(buffer[i] == ' ') {
            buffer[i] = 0;
            break;
        }
    }

    /* 檔掉回上層目錄的路徑『..』 */
    for (j=0;j<i-1;j++)
        if (buffer[j]=='.'&&buffer[j+1]=='.')
            exit(3);


    /* 當客戶端要求根目錄時讀取 index.html */
    if (!strncmp(&buffer[0],"GET /\0",6)||!strncmp(&buffer[0],"get /\0",6) )
        strcpy(buffer,"GET /index.html\0");

    //取出?後面的參數
    char *parm;
    parm = strchr(buffer,'?');
    if(parm!=NULL){
        parm[0] = '\0';
        printf("    Parameter = %s\n",&parm[1]);
        setenv("QUERY_STRING",&parm[1],1);      //1 overwrite 0 add
    }
    else
        printf("    No Parameter\n");

    /* 檢查客戶端所要求的檔案格式 */
    buflen = strlen(buffer);
    fstr = (char *)0;

    for(i=0;extensions[i].ext!=0;i++) {
        len = strlen(extensions[i].ext);
        //檢查附檔名是否有對應
        if(!strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            fstr = extensions[i].filetype;
            break;
        }
    }

    /* 檔案格式不支援 */
    if(fstr == 0) {
        fstr = extensions[i-1].filetype;    //當text/plain
    }

    printf("HTTP REQUEST\n");

    /* 開啟檔案  從第五個字元開始讀取目錄*/
        //printf("&buffer[5] = %s\n",&buffer[5]);
    if((file_fd=open(&buffer[5],O_RDONLY))==-1)
        write(fd, "Failed to open file\n", 20);

    char filename[50];
    strcpy(filename,&buffer[5]);

    /* 傳回瀏覽器成功碼 200 和內容的格式 */           //http response
    sprintf(buffer,"HTTP/1.0 200 OK\r\n", fstr);
    write(fd,buffer,strlen(buffer));

    printf("    HTTP/1.0 200 OK\r\n");
    printf("Check\n");
    printf("    File name = %s \n",filename);

    if(!strcmp(fstr,"cgi")){

                    //Get firstline
                    read(file_fd, buffer, BUFSIZE);
                    int z;
                    for(z=0; z<BUFSIZE; z++){
                        if(buffer[z]=='\n'){
                            buffer[z]='\0';
                            break;
                        }
                    }
                    printf("    CGI first line = %s\n",buffer);
                    dup2(fd,STDOUT_FILENO);
                    dup2(fd,STDERR_FILENO);

                    //if( buffer[0] == '#' || buffer[1] == '!')     //取#!後面的執行路徑，但後來直接./就好
                    //      buffer = &buffer[2];
                    //printf("  CGI line = %s\n",buffer);
                    //execl("/usr/bin/perl","perl",filename,NULL);

                    strcpy(buffer , "./");
                    strcat(buffer , filename);
                    execl(buffer, filename, NULL);
                    //system("/usr/bin/perl hello.cgi");
                    exit(0);

    }
    else{
            sprintf(buffer,"Content-Type: %s\r\n\r\n", fstr);
            write(fd,buffer,strlen(buffer));
            /* 讀取檔案內容輸出到客戶端瀏覽器 */
            while ((ret=read(file_fd, buffer, BUFSIZE))>0) {
                write(fd,buffer,ret);
            }
    }
    exit(1);
}

main(int argc, char **argv)
{
    int i, pid, listenfd, socketfd;
    size_t length;
    static struct sockaddr_in cli_addr;
    static struct sockaddr_in serv_addr;

/*
    printf("argv = %d\n",argc);
    printf("argv[0] = %s\n",argv[0]);
    printf("argv[1] = %s\n",argv[1]);
    printf("argv[2] = %s\n",argv[2]);
*/

    if(argc==1)
        printf("web server default port 14785\n");

    int port;
    if(argc==2)
        port=(u_short)atoi(argv[1]);
    else
        port=14785;

    /* 使用 www 當網站根目錄 */
    if(chdir("www") == -1){
        printf("ERROR: Can't Change to directory %s\n",argv[2]);
        exit(4);
    }

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
    serv_addr.sin_family = AF_INET;
    /* 使用任何在本機的對外 IP */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* 使用 default or argv[1] Port */
    serv_addr.sin_port = htons(port);

    /* 開啟網路監聽器 */
    if (bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
        exit(3);

    /* 開始監聽網路 */
    if (listen(listenfd,64)<0)
        exit(3);

    while(1) {
        length = sizeof(cli_addr);
        /* 等待客戶端連線 */
        if ((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, (socklen_t*)&length))<0)
            exit(3);

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
