#include <stdio.h>

void h_begin(char*);
void h_end(void);

main() {
    h_begin("A Test CGI Program by C");
    printf("<P>\nHello, world.\n");
    h_end();
}

void h_begin(char *title) {
    printf("Content-type:text/html\n\n");

    printf("<HTML><HEAD>\n<TITLE>%s</TITLE>\n", title);
    printf("<META HTTP-EQUIV=\"content-type\""
	   "CONTENT=\"text/html; charset=big5\">\n</HEAD>\n"
	   "<BODY BGCOLOR=#f8f4e7 LINK=#0000ff VLINK=#436eee ALINK=#b22222>\n");
}

void h_end(void) {
    printf("</BODY></HTML>\n");
}