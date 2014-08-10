#include "htm.h"

void h_begin() {
    printf("Content-type:text/html\n\n");
    char title[50];
    sprintf(title,"Network Programming Homework 3");
    printf("<html>\n<head>\n");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\"/>\n");
    printf("<title>%s</title>\n</head>\n", title);
    printf("<body bgcolor=#EEDDDD>\n");
    printf("<font face=\"Courier New\" size=4 color=#000000>\n");
}

void h_end() {
    printf("</font></body></html>\n");
}

void h_table(){
        printf("<table width=\"800\" border=\"1\">\n");
        printf("<tr>\n");
        for(int i =0; i<5; i++){
            if(URL.on[i] == 1)
                printf("<td>%s</td>",inet_ntoa(serv_addr[i].sin_addr));
        }
        printf("<tr>\n");
        for(int i =0; i<5; i++){
            if(URL.on[i] == 1)
                printf("<td valign=\"top\" id=\"m%d\">\n",i);
        }
        printf("</table>\n");
}

void _URL :: getenvdata(){
    char *data;
    //data的指標指向環境變數"QUERY_STRING"
    data=getenv("QUERY_STRING");
    //把data的內容讀到m內，URL.original和data指向不同記憶體位置
    sscanf(data,"%s", original);
    //printf("\n<p>%s</p>", original);

}

void _URL :: parser(){
        char* cut_per;
        char* pch;
        struct hostent *h;
        /*
        struct hostent {
              char  *h_name;
              char  **h_aliases;
              int   h_addrtype;
              int   h_length;
              char  **h_addr_list;
        };
        */
        for(int i=0 ; i<5 ; i++){

            bzero(&serv_addr[i], sizeof(serv_addr[i]));
            serv_addr[i].sin_family = AF_INET;

            bzero(&sock_serv_addr[i], sizeof(sock_serv_addr[i]));
            sock_serv_addr[i].sin_family = AF_INET;

            if(i ==0)
                cut_per =  strtok (original,"&");
            else
                cut_per =  strtok (NULL,"&");

                strcpy(ip_string[i] , cut_per);
                cut_per =  strtok (NULL,"&");
                strcpy(port_string[i] , cut_per);
                cut_per =  strtok (NULL,"&");
                strcpy(fname_string[i] , cut_per);
                cut_per =  strtok (NULL,"&");
                strcpy(sock_ip_string[i] , cut_per);
                cut_per =  strtok (NULL,"&");
                strcpy(sock_port_string[i] , cut_per);

                pch = strchr(ip_string[i],'=');         //回傳=那個指標
                if(pch!=NULL){
                    strcpy(ip_string[i],pch+1);     //複製=後面的值

                            if((h=gethostbyname(ip_string[i])) == NULL) {
                                    //printf("gethostbyname(ip_string[%d] = %s ): error",i,ip_string[i]);
                                    serv_addr[i].sin_addr.s_addr = inet_addr(ip_string[i]);
                                    //exit(0);
                            }
                            else{
                                    memcpy(&(serv_addr[i].sin_addr.s_addr), h->h_addr, h->h_length);
                                    //printf("Hostname: %s\n", h->h_name);
                                    //printf("IP Address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
                            }



                }
                pch = strchr(port_string[i],'=');
                if(pch!=NULL){
                    strcpy(port_string[i],pch+1);
                    port[i] =(unsigned short)atoi(port_string[i]);
                    serv_addr[i].sin_port = htons(port[i]);
                }

                pch = strchr(fname_string[i],'=');
                if(pch!=NULL)
                    strcpy(fname_string[i],pch+1);

                pch = strchr(sock_ip_string[i],'=');
                if(pch!=NULL){
                    strcpy(sock_ip_string[i],pch+1);
                            if((h=gethostbyname(sock_ip_string[i])) == NULL) {
                                    sock_serv_addr[i].sin_addr.s_addr = inet_addr(sock_ip_string[i]);
                            }
                            else{
                                    memcpy(&(sock_serv_addr[i].sin_addr.s_addr), h->h_addr, h->h_length);
                                    //printf("Hostname: %s\n", h->h_name);
                                    //printf("IP Address: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));
                            }
                }

                pch = strchr(sock_port_string[i],'=');
                if(pch!=NULL){
                    strcpy(sock_port_string[i],pch+1);
                    sock_port[i] =(unsigned short)atoi(sock_port_string[i]);
                    sock_serv_addr[i].sin_port = htons(sock_port[i]);
                }

                if(ntohs(serv_addr[i].sin_port) != 0 && fname_string[i] != NULL)
                    on[i] = 1;
                if(ntohs(sock_serv_addr[i].sin_port) != 0 && strcmp(inet_ntoa(sock_serv_addr[i].sin_addr),"255.255.255.255"))
                    sock_on[i] = 1;

                /*
                printf("<pre>%15s\t",inet_ntoa(serv_addr[i].sin_addr));
                printf("%u\t",ntohs(serv_addr[i].sin_port));
                printf("%10s\t",fname_string[i]);
                printf("%8s\t",(on[i] == 1)?"on":"close");
                printf("%15s\t",inet_ntoa(sock_serv_addr[i].sin_addr));
                printf("%u\t",ntohs(sock_serv_addr[i].sin_port));
                printf("%10s\t</br></pre>",(sock_on[i] == 1)?"sock_on":"close");
                */
        }
}
