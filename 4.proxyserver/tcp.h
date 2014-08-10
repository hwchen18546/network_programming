#ifndef _tcp_h
#define _tcp_h

extern int client_fd[5];
extern fd_set afds;
extern int NowRun;
void conncet_server(int id);
void conncet_sock_server(int id);

extern FILE *fp[5];
void create_files(int id);

extern char readtxt_buf[10000];
void read_files_send(int id);

int readline(int fd,int id,char *ptr,int maxlen);


#endif
