#ifndef _htm_h
#define _htm_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>      // 'fcntl'
#include <cerrno>       // 'errno'
#include <netdb.h>  //  'gethostbyname'

#include <iostream>
using namespace std;

extern struct sockaddr_in serv_addr[5];
extern int STARDREAD[5];
extern class _URL URL;

class _URL{
    public:
        int port[5];
        int on[5];
        //int stillrun;
        char ip_string[5][20];
        char port_string[5][20];
        char fname_string[5][20];
        char original[50];
        void    parser();
        void getenvdata();
        _URL(){
            for(int i = 0; i<5; i++){
                port[i] = -1 ;
                on[i] = 0 ;
            }
        };
};

void h_begin();
void h_end();
void h_table();
#endif
