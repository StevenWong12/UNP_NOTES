#include "unp.h"

int main(int argc, char *argv[]){
    int sockfd, n, ret;
    char recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr_storage ss;

    if(argc != 3)
        err_quit("usage: daytimetcpcli <hostname/IPaddress> <service/port #>");
    
    sockfd = tcp_connect(argv[1], argv[2]);

    len = sizeof(ss);
    Getpeername(sockfd, (SA *)&ss, &len);
    printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

    ret = recv(sockfd, recvline, MAXLINE, MSG_PEEK);
    printf("bytes in the buffer(recv): %d\n", ret);
    ioctl(sockfd, FIONREAD, &ret);
    printf("bytes in the buffer(ioctl): %d\n", ret);
    while ( (n = Read(sockfd, recvline, MAXLINE)) > 0 ){
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }

    exit(0);
    
}