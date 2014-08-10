#include "slave.h"

    int glo_clientid = 0;

//計算水平cmd
    int total_cmd = 0;

//計算垂直cmd
    int nowline = 0;

//Function類
    int writeagain = 0;
    int savepipe = 0;                   // 有|n的話savepipe=1
    int errpipe = 0;                        // 有!n的話errpipe=1
    int future = 0;                     // n的值

    int fifo_to =-100;
    int fifo_from = -100;

        char *fn;   //output file name
        char *fn2;  //input file name

        char deletefn[100];

        cmd CMD[3100];
        verpipe VPI[3100];

        FILE *fp;
        FILE *fp2;

        client *C;
        must *M;
        char inputline[20000];
        char inputline_copy[20000];

int shell(int clientid)         //newsockfd下的childpid(輸入輸出都倒到newsockfd)
{
    glo_clientid = clientid;
    const char * const path = "ras";
    chdir(path);        //change dir in program

    setenv("PATH", "bin:.",1); //1 overwrite , 0 add , :分隔 , .目前資料夾

    write(1,"****************************************", strlen("****************************************"));
    write(1,"\n",1);
    write(1,"** Welcome to the information server. **", strlen("** Welcome to the information server. **"));
    write(1,"\n",1);
    write(1,"****************************************", strlen("****************************************"));
    write(1,"\n",1);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int shmid1;
        if ((shmid1 = shmget((key_t)125, 1024, 0666)) < 0) {
            perror("shmget1");
            exit(1);
        }
        if ((M = (must *)shmat(shmid1, NULL, 0)) == (must *) -1) {
            perror("shmat1");
            exit(1);
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int shmid2;
        if ((shmid2 = shmget((key_t)124, 1024, 0666)) < 0) {
            perror("shmget2");
            exit(1);
        }
        if ((C = (client *)shmat(shmid2, NULL, 0)) == (client *) -1) {
            perror("shmat2");
            exit(1);
        }
        write(1,M->welcom, strlen(M->welcom));
        write(1,"\n", 1);
        //cout<<welcom<<endl;
///////////////////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, signalhandler);
///////////////////////////////////////////////////////////////////////////////////////

    while(1)
    {
        write(1, "% ", strlen("% "));
        //read(C->newsockfd[clientid-1] ,inputline,20000);
        cin.getline(inputline,20000);           //scanf(%s)只能讀到空白

        //砍掉尾巴渣渣手動輸入會多一個，自動輸入會多兩個
        while(inputline[strlen(inputline)-1] == '\n' || inputline[strlen(inputline)-1] == '\r'){
                inputline[strlen(inputline)-1] = '\0' ;
        }
        //沒有輸入東西pass掉
        if (strlen(inputline) == 0)
            continue;

        //離開也先pass掉
        if (strncmp(inputline,"exit",4)==0){
            sprintf (C->exit,"*** User '%s' left. ***" , C->name[clientid-1]);
            killpeople(C,101);
            C->online[clientid-1] = 0 ;
            C->total_cli--;
            for(int i=0; i<MAXCLI ;i++){
                    if(M->fifo_open[clientid-1][i] == 1){       //pipe給別人的不算
                            sprintf(deletefn,"../temp%d-%d.txt",clientid ,i+1);
                            unlink(deletefn);
                            M->fifo_open[clientid-1][i] = 0;
                    }

                    if(M->fifo_open[i][clientid-1] == 1){       //別人pipe給我沒收也不算
                            sprintf(deletefn,"../temp%d-%d.txt", i+1 ,clientid);
                            unlink(deletefn);
                            M->fifo_open[i][clientid-1] = 0;
                    }

            }
            break;

        }
        //行數+1
        nowline++;

        strcpy(inputline_copy,inputline);

        //聊天室tell who yell會在這邊執行
        if(chatword(inputline, clientid,C))
                continue;

        if(writedata(inputline)){                               //  是否有>需要寫出檔案
                fifo_to = -100;                                             //  如果 ls >100 fifo_to = 100會到下一圈
                continue;                                           //  pass掉>n的error test
        }

        getcmd(inputline);                              //   分割成一個一個cmd，用|切開
        holddata(CMD[total_cmd-1].name);        //   是否有|n和!n需要hold住檔案

        if(fifo_from == -100){                                  //   如果前面已經讀到<n 這邊就不用再一次
                if(readdata(CMD[0].name)){                      //   是否有<需要讀入檔案
                        fifo_from = -100;                                               //如果 ls <1000 fifo_from = 1000會到下一圈 ls <2，就不會進來了eo4!!
                        continue;                                           //  pass掉>n的error test
                }
        }
        //cout<<"fifo_to "<<fifo_to<<endl;
        //cout<<"fifo_from "<<fifo_from<<endl;
        //開始執行指令
        for(int i=0 ; i<total_cmd ;i++){
//只有一個指令
            if(total_cmd==1){
                    if(fn != NULL){     //寫檔案，要fork不然parent stdout會壞掉
                            if(fork()==0){      //child
                                    if(fifo_to == -100){            //一般沒有>數字
                                            if(writeagain == 1){
                                                fp=fopen(fn,"a");
                                            }
                                            else
                                                fp=fopen(fn,"w");
                                        }
                                        else{                           //有>數字
                                                sprintf(fn,"../temp%d-%d.txt",glo_clientid ,fifo_to);
                                                fp=fopen(fn,"w");
                                        }

                                        if(VPI[nowline].open ==1 ){                 //進來是管子  出去寫檔案
                                                dup2(VPI[nowline].pi[1],STDIN_FILENO);
                                                close(VPI[nowline].pi[1]);
                                                close(VPI[nowline].pi[0]);
                                        }
                                        else if(fn2 != NULL){
                                                fn2[strlen(fn2)-1]='\0';
                                                if(fifo_from !=-100){
                                                        sprintf(fn2,"../temp%d-%d.txt",fifo_from ,glo_clientid);
                                                }
                                                fp2=fopen(fn2,"r");                     //進來是讀檔 出去是寫黨
                                                if(fp2){
                                                        dup2(fileno(fp2),STDIN_FILENO);
                                                        fclose(fp2);
                                                        if(fifo_from!=-100){
                                                                unlink(fn2);        //砍掉檔案
                                                                M->fifo_open[fifo_from-1][glo_clientid-1] = 0 ;
                                                        }
                                                }
                                                else
                                                    cout<<"not open"<<endl;
                                        }

                                    dup2(fileno(fp),STDOUT_FILENO);
                                    fclose(fp);
                                    readcmd(CMD[i].name);
                                    exit(0);
                            }
                            else{           //parent
                                    if(VPI[nowline].open ==1 ){
                                            close(VPI[nowline].pi[1]);
                                            close(VPI[nowline].pi[0]);
                                    }
                                    wait(NULL);
                            }
                    }
                    else if(fn2 != NULL){       //讀檔案，要fork不然parent stdout會壞掉
                            if(fork()==0){      //child
                                    if(fifo_from !=-100)
                                            sprintf(fn2,"../temp%d-%d.txt",fifo_from ,glo_clientid);
                                    fp2=fopen(fn2,"r");
                                    dup2(fileno(fp2),STDIN_FILENO);
                                    fclose(fp2);
                                    readcmd(CMD[i].name);
                                    if(fifo_from!=-100){
                                        unlink(fn2);        //砍掉檔案
                                        M->fifo_open[fifo_from-1][glo_clientid-1] = 0 ;
                                    }
                                    exit(0);
                            }
                            else{           //parent
                                    wait(NULL);
                            }
                    }
                    else if (savepipe == 1 && VPI[nowline].open !=1){           //這個指令要存到pipe
                        if(fork()==0){
                                    dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                                    if( errpipe ==1)
                                        dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                                    close(VPI[nowline+future].pi[0]);
                                    close(VPI[nowline+future].pi[1]);
                                    readcmd(CMD[i].name);
                                    exit(0);
                            }
                            else{           //parent
                                    //close(VPI[nowline+future].pi[0]); 這行不能加，後面還有可能要丟parent要留著
                                    wait(NULL);
                            }
                    }
                    else if(VPI[nowline].open ==1 && savepipe != 1){            //前面有管子要進來
                            if(fork()==0){
                                    dup2(VPI[nowline].pi[1],STDIN_FILENO);
                                    close(VPI[nowline].pi[1]);
                                    close(VPI[nowline].pi[0]);
                                    readcmd(CMD[i].name);
                                    exit(0);
                            }
                            else{           //parent
                                    close(VPI[nowline].pi[1]);
                                    close(VPI[nowline].pi[0]);
                                    wait(NULL);
                            }
                    }
                    else if(VPI[nowline].open ==1 && savepipe == 1){            //前面進來，管子出去
                            if(fork()==0){
                                    dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                                    if( errpipe ==1)                        //error一起導
                                            dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                                    close(VPI[nowline+future].pi[0]);
                                    close(VPI[nowline+future].pi[1]);
                                    dup2(VPI[nowline].pi[1],STDIN_FILENO);
                                    close(VPI[nowline].pi[1]);
                                    close(VPI[nowline].pi[0]);
                                    readcmd(CMD[i].name);
                                    exit(0);
                            }
                            else{           //parent
                                    close(VPI[nowline].pi[1]);
                                    close(VPI[nowline].pi[0]);
                                    wait(NULL);
                            }

                    }

                    else        //有可能有setenv，不要fork
                                    readcmd(CMD[i].name);

            }
//total_cmd!=1 超過一個指令
            else{
                if(CMD[i].no!=total_cmd-1)  //最後一個指令不用宣告管子往後
                    pipe(CMD[i].pi);
                int cmdchild = fork();
                if(cmdchild==0){
                        if(CMD[i].no==0){   //first

                            if(VPI[nowline].open ==1){
                                dup2(VPI[nowline].pi[1],STDIN_FILENO);
                                close(VPI[nowline].pi[1]);
                                close(VPI[nowline].pi[0]);          //不關還是會cu住
                            }

                            if(fn2 != NULL){                        //進來是讀檔 出去是寫黨
                                fn2[strlen(fn2)-1]='\0';
                                if(fifo_from !=-100)
                                        sprintf(fn2,"../temp%d-%d.txt",fifo_from ,glo_clientid);
                                fp2=fopen(fn2,"r");
                                dup2(fileno(fp2),STDIN_FILENO);
                                fclose(fp2);
                                if(fifo_from!=-100){
                                    unlink(fn2);        //砍掉檔案
                                    M->fifo_open[fifo_from-1][glo_clientid-1] = 0 ;
                                }
                            }

                            dup2(CMD[i].pi[0],STDOUT_FILENO);   //關掉OUT 改成CMD[0].pi進去
                            close(CMD[i].pi[0]);
                            close(CMD[i].pi[1]);
                        }
                        else if(CMD[i].no==total_cmd-1){ //last
                            dup2(CMD[i-1].pi[1],STDIN_FILENO);
                                if(fn != NULL){         //寫檔案
                                    if(fifo_to == -100){            //一般沒有>數字
                                            if(writeagain == 1){
                                                fp=fopen(fn,"a");
                                            }
                                            else
                                                fp=fopen(fn,"w");
                                        }
                                        else{                           //有>數字
                                                sprintf(fn,"../temp%d-%d.txt",glo_clientid ,fifo_to);
                                                fp=fopen(fn,"w");
                                        }
                                        dup2(fileno(fp),STDOUT_FILENO);
                                        fclose(fp);
                                }
                                else if (savepipe == 1){
                                    dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                                    if( errpipe ==1)                        //error一起導
                                            dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                                    close(VPI[nowline+future].pi[0]);
                                    close(VPI[nowline+future].pi[1]);
                                }
                            close(CMD[i-1].pi[0]);
                            close(CMD[i-1].pi[1]);
                        }
                        else{   //中間的
                            dup2(CMD[i-1].pi[1],STDIN_FILENO);
                            dup2(CMD[i].pi[0],STDOUT_FILENO);   //關掉OUT 改成CMD[0].pi進去
                            close(CMD[i-1].pi[0]);
                            close(CMD[i-1].pi[1]);
                            close(CMD[i].pi[0]);
                            close(CMD[i].pi[1]);
                        }
                            readcmd(CMD[i].name);
                            exit(0);
                }
                else{                           //parent
                            if(CMD[i].no!=total_cmd-1)  //不是最後一個，parent要把丟進去的管子關掉，沒寫好就hang住了!!
                                    close(CMD[i].pi[0]);
                            if(CMD[i].no!=0)                        //不是第一個，parent要把前面丟進來的管子關掉
                                    close(CMD[i-1].pi[1]);
                            if (savepipe == 1){
                                    //close(VPI[nowline+future].pi[0]); //現在關掉parent fork就沒有這根了
                            }
                            if(VPI[nowline].open ==1){
                                    close(VPI[nowline].pi[1]);
                                    close(VPI[nowline].pi[0]);
                            }
                            wait(NULL);
                            if( M->unknown[glo_clientid-1] == 1)
                                break;
                }
            }
        }
        if(fifo_from != -100)
                killpeople(C,104);
//      sleep(1);
        if(fifo_to != -100)
                killpeople(C,103);
        M->unknown[glo_clientid-1] = 0;
        total_cmd = 0;
        savepipe = 0;
        future = 0;
        errpipe = 0;
        writeagain = 0;
        fifo_to =-100;
        fifo_from = -100;
    }
}

///////////////////////////////////////////////////////////
void signalhandler(int signo){
            char temp[200];
            if(C->SIGUSR_NO == 100){
                write(1,M->welcom,strlen(M->welcom));
            }
            else if(C->SIGUSR_NO == 101){
                write(1,C->exit,strlen(C->exit));
            }
            else if(C->SIGUSR_NO == 102){
                write(1,M->yell,strlen(M->yell));
            }
            else if(C->SIGUSR_NO == 103){
                write(1,M->fifo_sendmsg,strlen(M->fifo_sendmsg));
            }
            else if(C->SIGUSR_NO == 104){
                write(1,M->fifo_recvmsg,strlen(M->fifo_recvmsg));
            }
            else if(C->SIGUSR_NO == 105){
                write(1,M->tellname,strlen(M->tellname));
            }
            else{
                //cout<<"From "<<C->recvid[glo_clientid-1]<<" to "<<glo_clientid<<endl;
                sprintf (temp,"*** %s told you ***: %s" , C->name[C->recvid[glo_clientid-1]-1], C->message[glo_clientid-1]);
                write(1,temp ,strlen(temp));
            }
            write(1,"\n",1);
}
//////////////////////////////////////////////////////////
void killpeople(class client *C, int cast){
            C->SIGUSR_NO = cast ;
            if(cast <50){                                                                                           //tell cast message
                    if(C->online[cast] == 1 && C->message[cast] != NULL)
                                kill(C->childpid[cast],SIGUSR1);
            }
            else{
                    for (int i=0 ;i<MAXCLI ; i++){                                                  //welcom exit 都沒有自己
                                    if(C->online[i] == 1 && i != glo_clientid-1)
                                            kill(C->childpid[i],SIGUSR1);
                    }
                    if(cast == 101 ||cast == 102 || cast == 103 || cast == 104 || cast == 105)                                  //yell send recv要在加自己
                            kill(C->childpid[glo_clientid-1],SIGUSR1);
            }
}
//////////////////////////////////////////////////////////
int findID(class client *C){                //在server.cpp才會用到
    if(C->total_cli<=MAXCLI){
            for (int i=0 ;i<MAXCLI ; i++){
                    if(C->online[i] == 0){
                            C->online[i] = 1;
                            C->id[i] = i+1;
                            strcpy(C->name[i],"(no name)");
                            return i ;
                    }
            }
    }
    else
        return -1;
}
//////////////////////////////////////////////////////////
int chatword(char *cut, int clientid, class client *C){
        char temp[100];
        if(strcmp(cut,"who")==0){
                write(1,"<ID>   <nickname>  <IP/port>   <indicate me>",strlen("<ID> <nickname>  <IP/port>   <indicate me>"));
                write(1,"\n",1);
                for(int i=0; i<MAXCLI; i++){
                        if(C->online[i] ==1){
                                if(i == clientid-1){
                                                sprintf (temp,"%d   %s  %s/%u   <-me" ,C->id[i], C->name[i] ,inet_ntoa(C->cli_addr[i].sin_addr),C->cli_addr[i].sin_port);
                                }
                                else{
                                                sprintf (temp,"%d   %s  %s/%u" ,C->id[i], C->name[i] ,inet_ntoa(C->cli_addr[i].sin_addr),C->cli_addr[i].sin_port);
                                }
                                write(1,temp ,strlen(temp));
                                write(1,"\n",1);
                        }
                }
                return 1;
        }
        else if(strncmp(cut,"name ",5)==0){
            cut =  strtok (cut," ");
            cut =  strtok (NULL," ");
            for (int i=0 ;i<MAXCLI ; i++){                                                  //welcom exit 都沒有自己
                    if(C->online[i] == 1 && strcmp( C->name[i],cut) == 0){
                            sprintf (temp,"*** User '%s' already exists. ***" ,cut);
                            write(C->newsockfd[glo_clientid-1],temp,strlen(temp));
                            write(C->newsockfd[glo_clientid-1],"\n",1);
                            return 1;
                    }
            }
            strcpy(C->name[clientid-1],cut);
            sprintf (M->tellname,"*** User from %s/%u is named '%s'. ***" ,inet_ntoa(C->cli_addr[clientid-1].sin_addr),C->cli_addr[clientid-1].sin_port, C->name[clientid-1]);
            killpeople(C,105);
            return 1;
        }
        else if(strncmp(cut,"tell ",5)==0){
                cut =  strtok (cut," ");
                cut =  strtok (NULL," ");
                C->sendid[glo_clientid-1] = atoi(cut);                          //把我的sendid存入tell的對象
                C->recvid[C->sendid[glo_clientid-1]-1] = glo_clientid;  //把tell的對象的recvid指向自己
                if(C->online[C->sendid[glo_clientid-1]-1] == 0){
                        sprintf (temp,"*** Error: user #%d does not exist yet. ***" ,C->sendid[glo_clientid-1]);
                        write(1,temp ,strlen(temp));
                        write(1,"\n",1);
                }
                else{
                        cut =  strtok (NULL,"");    //指向空格後面的東西 包含空格 所以用"" 而不是" "
                        if(cut != NULL){
                            //cout<<"From "<<glo_clientid<<" to "<<C->sendid[glo_clientid-1]<<endl;
                            strcpy(C->message[C->sendid[glo_clientid-1]-1], cut);
                            killpeople(C,C->sendid[glo_clientid-1]-1);
                        }
                }
                return 1;
        }
        else if(strncmp(cut,"yell ",5)==0){
            cut =  strtok (cut," ");
            cut =  strtok (NULL,"");
            sprintf (M->yell,"*** %s yelled ***: %s",C->name[glo_clientid-1] ,cut);
            killpeople(C,102);
            return 1;
        }

        return 0;
}
//////////////////////////////////////////////////////////
int holddata(char *last){

          if (isdigit(last[0])){                                                    //是否為|n
                future = atoi (last);
                total_cmd--;
                if(VPI[nowline+future].open != 1){              //還沒開過
                        if(pipe(VPI[nowline+future].pi)<0){  //開管
                                fprintf(stderr, "Pipe fail1");
                                fprintf(stderr, "\n");
                        }
                        VPI[nowline+future].open = 1 ; //告知已開
                }
                savepipe = 1;
          }
          else{                                                                         //是否含有驚嘆號 ls !n
                char* temp;
                temp = strtok (last,"!" );  //ls
                strcpy(CMD[total_cmd-1].name,temp);
                temp = strtok (NULL,"!" );  //數字
                if(temp != NULL){
                    future = atoi (temp);
                    if(VPI[nowline+future].open != 1){              //還沒開過
                            if(pipe(VPI[nowline+future].pi)<0){  //開管
                                    fprintf(stderr, "Pipe fail1");
                                    fprintf(stderr, "\n");
                            }
                            VPI[nowline+future].open = 1 ; //告知已開
                    }
                    savepipe = 1;
                    errpipe = 1;
                }
          }


}

//////////////////////////////////////////////////////////
int writedata(char *cut){           // cat > abc.txt
    fn =  strtok (cut,">" );            // cat \0 abc.txt ，fn指向c
    if (fn[strlen(fn)+1]=='>')          // 如果是\0接的是>，就改成複寫
            writeagain = 1;
    fn =  strtok (NULL,">" );       //fn指向"空白"abc.txt\0
    if(fn == NULL)
        ;//printf("cut nothing\n");
    else{
        readdata(fn);                           //處理先有>n才有<n的case
        //printf("cut filename\n");
        if (isdigit(fn[0])){                // >數字  聊天室PIPE
            fifo_to = atoi(fn);
            if(C->online[fifo_to-1] == 0){
                cout<<"*** Error: user #"<<fifo_to<<" does not exist yet. ***"<<endl;
                return 1;
            }
            else if (M->fifo_open[glo_clientid-1][fifo_to-1] == 1){
                cout<<"*** Error: the pipe #"<<glo_clientid<<"->#"<<fifo_to<<" already exists. ***"<<endl;
                return 1;
            }
            else{
                M->fifo_open[glo_clientid-1][fifo_to-1] = 1;
                sprintf (M->fifo_sendmsg,"*** %s (#%d) just piped '%s' to %s (#%d) ***" ,C->name[glo_clientid-1],glo_clientid, inputline_copy,C->name[fifo_to-1],fifo_to);
            }
        }
        else
            fn = fn + 1;                    //把空白的頭殺幹掉
        return 0;
    }
    return 0 ;
}

//////////////////////////////////////////////////////////

int readdata(char *cut){
    fn2 = strtok (cut,"<" );
    fn2 = strtok (NULL,"<" );
    if(fn2 == NULL)
        ;//printf("cut nothing\n");
    else{
        if (isdigit(fn2[0])){               // >數字  聊天室PIPE
            fifo_from = atoi(fn2);
            if(M->fifo_open[fifo_from-1][glo_clientid-1] == 0){
                cout<<"*** Error: the pipe #"<<fifo_from<<"->#"<<glo_clientid<<" does not exist yet. ***"<<endl;
                return 1;
            }
            else{
                sprintf (M->fifo_recvmsg,"*** %s (#%d) just received from %s (#%d) by '%s' ***" ,C->name[glo_clientid-1],glo_clientid,C->name[fifo_from-1],fifo_from,inputline_copy);
            }
        }
        else{
                fn2 = fn2 + 1;                  //去頭
        }
    }
    return 0 ;
}

//////////////////////////////////////////////////////////

int getcmd(char *cut){
        if(total_cmd==0){
            CMD[total_cmd].name = strtok (cut,"|");
            CMD[total_cmd].no = 0;
        }
        else{
            CMD[total_cmd].name = strtok (NULL,"|");
        }
        if(CMD[total_cmd].name!=NULL){
            total_cmd++;
            CMD[total_cmd].no = total_cmd;
            getcmd(cut);
        }

            return total_cmd;
}

    char *pch;
//////////////////////////////////////////////////////////
int readcmd(char *haha){
        //printf("strlen(%s)%d\n",haha,strlen(haha));
        if(strlen(haha)==0) return 0;
        pch = strtok (haha," ");
//printenv
        if(strcmp(pch,"printenv")==0){
            pch = strtok (NULL," ");
            if(pch != NULL){
                write(1,"PATH=",strlen("PATH="));
                write(1,getenv(pch),strlen(getenv(pch)));
                write(1,"\n",1);
            }
            else{
                write(1,"PATH=",strlen("PATH="));
                write(1,getenv("PATH"),strlen(getenv("PATH")));
                write(1,"\n",1);
            }
        }

//setenv
        else if(strcmp(pch,"setenv")==0){
            pch = strtok (NULL," ");        //PATH
            if(pch != NULL){
                if(setenv(pch, strtok (NULL," "),1)<0){
                        fprintf(stderr, "setenv fail");
                        fprintf(stderr, "\n");
                }
            }
        }
//bin
        else{
                int count = 0;
                char *cmd_list[10];         //單個指令最長的參數
                cmd_list[0] = pch;          //"ls"
                while (pch != NULL){        //把指令拆開存進cmd_list
                    count++;
                    pch = strtok (NULL," ");
                    cmd_list[count] = pch;
                }
            if(fork()==0){
                        if(execvp(cmd_list[0],cmd_list)<0){
                            fprintf(stderr, "Unknown command: ");
                            fprintf(stderr, "[");
                            fprintf(stderr, cmd_list[0]);
                            fprintf(stderr, "].");
                            fprintf(stderr, "\n");
                            M->unknown[glo_clientid-1] = 1;
                        }
                exit(0);
            }
            else
                wait(NULL); //parent等待child執行完再繼續
        }
}