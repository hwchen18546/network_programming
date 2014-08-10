#include<unistd.h>
#include<cstdio>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>//setenv()
#include<sys/wait.h>
#include<ctype.h>//isdigit

#include<iostream>
usingnamespacestd;

inttotal_cmd=0;
classcmd{
public:
    char*name;
    intpi[2];
    intno;
    cmd(){no=0;}
};

classverpipe{
public:
    intpi[2];
    intopen;
    verpipe(){open=0;}
};

intreadcmd(char*haha);
intgetcmd(char*cut);
intwritedata(char*cut);
intreaddata(char*cut);
intholddata(char*last);
intshell();

char*fn;//output file name
char*fn2;//input file name
intnowline=0;
intsavepipe=0;
interrpipe=0;
intfuture=0;
intwriteagain=0;

cmdCMD[3100];
verpipeVPI[3100];

FILE*fp;
FILE*fp2;

intshell()//newsockfd下的childpid(輸入輸出都倒到newsockfd)
{
    constchar*constpath="ras";
    chdir(path);//change dir in program

    setenv("PATH","bin:.",1);//1 overwrite , 0 add , :分隔 , .目前資料夾

    charinputline[20000];

    write(1,"****************************************",strlen("****************************************"));
    write(1,"\n",1);
    write(1,"** Welcome to the information server. **",strlen("** Welcome to the information server. **"));
    write(1,"\n",1);
    write(1,"****************************************",strlen("****************************************"));
    write(1,"\n",1);

    while(1)
    {
        write(1,"% ",strlen("% "));
        cin.getline(inputline,20000);//scanf(%s)只能讀到空白

        while(inputline[strlen(inputline)-1]=='\n'||inputline[strlen(inputline)-1]=='\r'){//手動輸入會多一個，自動輸入會多兩個
            inputline[strlen(inputline)-1]='\0';
        }
        //inputline[strlen(inputline)-2]='\0';		專門砍測資的\r\n

        if(strncmp(inputline,"exit",4)==0)break;
        nowline++;
        writedata(inputline);//	 是否有>需要寫出檔案
        readdata(inputline);//	 是否有<需要讀入檔案
        getcmd(inputline);//   分割成一個一個cmd
        //printf("指令數 %d\n",total_cmd);
        holddata(CMD[total_cmd-1].name);//   是否有|n和!n需要hold住檔案
        //printf("total_cmd = %d\n",total_cmd);


        for(inti=0;i<total_cmd;i++){

            //只有一個指令
            if(total_cmd==1){
                if(fn!=NULL){//寫檔案，要fork不然parent stdout會壞掉
                    if(fork()==0){//child
                        if(writeagain==1){
                            fp=fopen(fn,"a");
                        }
                        else
                        fp=fopen(fn,"w");
                        dup2(fileno(fp),STDOUT_FILENO);
                        fclose(fp);
                        readcmd(CMD[i].name);
                        exit(NULL);
                    }
                    else{//parent
                        wait(NULL);
                    }
                }
                elseif(fn2!=NULL){//讀檔案，要fork不然parent stdout會壞掉
                    if(fork()==0){//child
                        fp2=fopen(fn2,"r");
                        dup2(fileno(fp2),STDIN_FILENO);
                        fclose(fp2);
                        readcmd(CMD[i].name);
                        exit(NULL);
                    }
                    else{//parent
                        wait(NULL);
                    }
                }
                elseif(savepipe==1&&VPI[nowline].open!=1){//這個指令要存到pipe
                    if(fork()==0){
                        dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                        if(errpipe==1)
                        dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                        close(VPI[nowline+future].pi[0]);
                        close(VPI[nowline+future].pi[1]);
                        readcmd(CMD[i].name);
                        exit(NULL);
                    }
                    else{//parent
                        //close(VPI[nowline+future].pi[0]); 這行不能加，後面還有可能要丟parent要留著
                        wait(NULL);
                    }
                }
                elseif(VPI[nowline].open==1&&savepipe!=1){//前面有管子要進來
                    if(fork()==0){
                        dup2(VPI[nowline].pi[1],STDIN_FILENO);
                        close(VPI[nowline].pi[1]);
                        close(VPI[nowline].pi[0]);
                        readcmd(CMD[i].name);
                        exit(NULL);
                    }
                    else{//parent
                        close(VPI[nowline].pi[1]);
                        close(VPI[nowline].pi[0]);
                        wait(NULL);
                    }
                }
                elseif(VPI[nowline].open==1&&savepipe==1){//前面進來，管子出去
                    if(fork()==0){
                        dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                        if(errpipe==1)//error一起導
                        dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                        close(VPI[nowline+future].pi[0]);
                        close(VPI[nowline+future].pi[1]);
                        dup2(VPI[nowline].pi[1],STDIN_FILENO);
                        close(VPI[nowline].pi[1]);
                        close(VPI[nowline].pi[0]);
                        readcmd(CMD[i].name);
                        exit(NULL);
                    }
                    else{//parent
                        close(VPI[nowline].pi[1]);
                        close(VPI[nowline].pi[0]);
                        wait(NULL);
                    }

                }
                else//有可能有setenv，不要fork
                readcmd(CMD[i].name);

            }
            //total_cmd!=1 超過一個指令
            else{
                if(CMD[i].no!=total_cmd-1)//最後一個指令不用管子往後
                pipe(CMD[i].pi);
                intcmdchild=fork();
                if(cmdchild==0){
                    if(CMD[i].no==0){//first
                        if(VPI[nowline].open==1){
                            dup2(VPI[nowline].pi[1],STDIN_FILENO);
                            close(VPI[nowline].pi[1]);
                            close(VPI[nowline].pi[0]);//不關還是會cu住
                        }
                        dup2(CMD[i].pi[0],STDOUT_FILENO);//關掉OUT 改成CMD[0].pi進去
                        close(CMD[i].pi[0]);
                        close(CMD[i].pi[1]);
                    }
                    elseif(CMD[i].no==total_cmd-1){//last
                        dup2(CMD[i-1].pi[1],STDIN_FILENO);
                        if(fn!=NULL){//寫檔案
                            if(writeagain==1){
                                fp=fopen(fn,"a");
                            }
                            else
                            fp=fopen(fn,"w");
                            dup2(fileno(fp),STDOUT_FILENO);
                            fclose(fp);
                        }
                        elseif(savepipe==1){
                            dup2(VPI[nowline+future].pi[0],STDOUT_FILENO);
                            if(errpipe==1)//error一起導
                            dup2(VPI[nowline+future].pi[0],STDERR_FILENO);
                            close(VPI[nowline+future].pi[0]);
                            close(VPI[nowline+future].pi[1]);
                        }
                        close(CMD[i-1].pi[0]);
                        close(CMD[i-1].pi[1]);
                    }
                    else{//中間的
                        dup2(CMD[i-1].pi[1],STDIN_FILENO);
                        dup2(CMD[i].pi[0],STDOUT_FILENO);//關掉OUT 改成CMD[0].pi進去
                        close(CMD[i-1].pi[0]);
                        close(CMD[i-1].pi[1]);
                        close(CMD[i].pi[0]);
                        close(CMD[i].pi[1]);
                    }
                    readcmd(CMD[i].name);
                    exit(0);
                }
                else{//parent
                    if(CMD[i].no!=total_cmd-1)//不是最後一個，parent要把丟進去的管子關掉，沒寫好就hang住了!!
                    close(CMD[i].pi[0]);
                    if(CMD[i].no!=0)//不是第一個，parent要把前面丟進來的管子關掉
                    close(CMD[i-1].pi[1]);
                    if(savepipe==1){
                        //close(VPI[nowline+future].pi[0]);	//現在關掉parent fork就沒有這根了
                    }
                    if(VPI[nowline].open==1){
                        close(VPI[nowline].pi[1]);
                        close(VPI[nowline].pi[0]);
                    }

                    wait(NULL);
                }
            }
        }
        total_cmd=0;
        savepipe=0;
        future=0;
        errpipe=0;
        writeagain=0;
    }
}

//////////////////////////////////////////////////////////
intholddata(char*last){

    if(isdigit(last[0])){//是否為|n
        future=atoi(last);
        total_cmd--;
        if(VPI[nowline+future].open!=1){//還沒開過
            if(pipe(VPI[nowline+future].pi)<0){//開管
                fprintf(stderr,"Unknown Command:");
                fprintf(stderr,"\n");
            }
            VPI[nowline+future].open=1;//告知已開
        }
        savepipe=1;
    }
    else{//是否含有驚嘆號 ls !n
        char*temp;
        temp=strtok(last,"!");//ls
        strcpy(CMD[total_cmd-1].name,temp);
        temp=strtok(NULL,"!");//數字
        if(temp!=NULL){
            future=atoi(temp);
            if(VPI[nowline+future].open!=1){//還沒開過
                if(pipe(VPI[nowline+future].pi)<0){//開管
                    fprintf(stderr,"Unknown Command:");
                    fprintf(stderr,"\n");
                }
                VPI[nowline+future].open=1;//告知已開
            }
            savepipe=1;
            errpipe=1;
        }
    }
}
//////////////////////////////////////////////////////////

intwritedata(char*cut){
    fn=strtok(cut,">");//切第一個>回來
    if(fn[strlen(fn)+1]=='>')
    writeagain=1;
    fn=strtok(NULL,">");//切第二個指向空白
    if(fn==NULL)
        ;
    else{
        fn=fn+1;//去頭
    }
    return0;
}

//////////////////////////////////////////////////////////

intreaddata(char*cut){
    fn2=strtok(cut,"<");
    fn2=strtok(NULL,"<");
    if(fn2==NULL)
        ;
    else{
        fn2=fn2+1;//去頭
    }
    return0;
}

//////////////////////////////////////////////////////////

intgetcmd(char*cut){
    if(total_cmd==0){
        CMD[total_cmd].name=strtok(cut,"|");
        CMD[total_cmd].no=0;
    }
    else{
        CMD[total_cmd].name=strtok(NULL,"|");
    }
    if(CMD[total_cmd].name!=NULL){
        total_cmd++;
        CMD[total_cmd].no=total_cmd;
        getcmd(cut);
    }
    /*
        CMD[total_cmd].name = strtok (cut,"|");
        CMD[total_cmd].no = 0;
        total_cmd++;
        CMD[total_cmd].name = strtok (NULL,"|");
        while(CMD[total_cmd].name!=NULL){
            CMD[total_cmd].no = total_cmd;
            total_cmd++;
            CMD[total_cmd].name = strtok (NULL,"|");
        }
    */
    returntotal_cmd;
}

char*pch;
//////////////////////////////////////////////////////////
intreadcmd(char*haha){
    if(strlen(haha)==0)return0;
    pch=strtok(haha," ");
    //printenv
    if(strcmp(pch,"printenv")==0){
        pch=strtok(NULL," ");
        if(pch!=NULL){
            write(1,getenv(pch),strlen(getenv(pch)));
            write(1,"\n",1);
        }
        else{
            write(1,getenv("PATH"),strlen(getenv("PATH")));
            write(1,"\n",1);
        }
    }

    //setenv
    elseif(strcmp(pch,"setenv")==0){
        pch=strtok(NULL," ");//PATH
        if(pch!=NULL){
            if(setenv(pch,strtok(NULL," "),1)<0){
                fprintf(stderr,"setenv fail");
                fprintf(stderr,"\n");
            }
        }
    }
    //bin
    else{
        intcount=0;
        char*cmd_list[10];//單個指令最長的參數
        cmd_list[0]=pch;//"ls"
        while(pch!=NULL){//把指令拆開存進cmd_list
            count++;
            pch=strtok(NULL," ");
            cmd_list[count]=pch;
        }
        if(fork()==0){
            if(execvp(cmd_list[0],cmd_list)<0){
                fprintf(stderr,"Unknown Command: ");
                fprintf(stderr,"[");
                fprintf(stderr,cmd_list[0]);
                fprintf(stderr,"].");
                fprintf(stderr,"\n");
            }
            exit(NULL);
        }
        else
        wait(NULL);//parent等待child執行完再繼續
    }
}