#include "htm.h"
#include "tcp.h"

struct sockaddr_in serv_addr[5];
class _URL URL;
int STARDREAD[5] = {0};

int main(int argc, char* argv[])
{
    h_begin();
    URL.getenvdata();
    URL.parser();
    h_table();
    NowRun = 0;
    FD_ZERO(&afds);

    for(int i = 0; i<5 ; i++){
            if(URL.on[i]){
                    //printf("%d onnnnnnnnnnnn <br>",i);
                    conncet_server(i);
                    create_files(i);
            }
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
//讀server的資訊

    fd_set rfds;
    char inputline[1000];

    while(1){
                for(int i = 0; i<5 ; i++){
                    if(URL.on[i]){
                            if(STARDREAD[i]==1){
                                    //sleep(1);     //加這行可以慢速trace XD
                                    read_files_send(i);
                                    if(!strcmp(readtxt_buf,"exit")){
                                            URL.on[i] = 881;
                                            sleep(1);
                                            //printf("<script>document.all['m%d'].innerHTML += \"<xmp>set 881 readtxt_buf = %s</xmp>\";</script>\n",i,readtxt_buf);
                                    }
                            }
                            if(serv_addr[i].sin_addr.s_addr == inet_addr("255.255.255.255")){   //host找不到都丟掉
                                    URL.on[i] = 881;
                                    //printf("<script>document.all['m%d'].innerHTML += \"<b>Can't find host</b><br>\";</script>\n",i);
                            }
                            if (fp[i] == NULL) {    //讀不到檔案踢掉
                                    URL.on[i] = 881;
                                    //printf("<script>document.all['m%d'].innerHTML += \"<b>File doesn't exist</b><br>\";</script>\n",i);
                            }
                    }
                }


                FD_ZERO(&rfds);
                memcpy(&rfds, &afds, sizeof(rfds));
                if(select(100,&rfds,NULL,NULL,NULL) < 0)
                    exit(1);

                for(int i = 0; i<5 ; i++){
                        if(URL.on[i]){
                                if(FD_ISSET(client_fd[i],&rfds)){
                                    for(int j = 0 ;j<1000;j++)
                                        inputline[j] = '\0';

                                    //如果用read會把socket一口氣讀進來不換行

                                    if(readline(client_fd[i], i, inputline,1000) == -1){
                                            sleep(1);
                                            continue;
                                    }
                                    if(inputline[strlen(inputline)-1] == '\n'){
                                        inputline[strlen(inputline)-1] ='\0';
                                        printf("<script>document.all['m%d'].innerHTML += \"<xmp>%s</xmp>\";</script>\n",i,inputline);
                                    }
                                    else{
                                        //ip對port錯，FD_ISSET會一直當有訊息會被%洗版，所以加上STARDREAD[i]==1
                                        if( URL.on[i] != 881 && STARDREAD[i]==1)
                                                printf("<script>document.all['m%d'].innerHTML += \"<b>%% </b>\";</script>\n",i);
                                    }
                                    fflush(stdout); //這行不加cgi會卡住

                                    //打exit後不會第一時間訊息到socket所以這個條件式要放在FD_ISSET內
                                    if(URL.on[i] == 881 ){
                                            fclose(fp[i]);
                                            close(client_fd[i]);
                                            NowRun--;
                                            FD_CLR(client_fd[i], &afds);
                                            URL.on[i] = 0;
                                    }
                                }
                        }
                }
                if(NowRun == 0)
                    break;

    }
    h_end();
    return 0;
}



