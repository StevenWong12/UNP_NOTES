#include "unp.h"
#include <sys/epoll.h>

void str_cli(FILE* fp, int sockfd){
    int kq, i, n, nev, stdineof = 0, isfile;
    char buf[MAXLINE];
    
}