#include "unp.h"
#include <netinet/tcp.h>

int main(int argc, char **argv){
    int sockfd, n;
    int recvbuf, mss;
    socklen_t len = sizeof(int);
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if( argc != 2)
        err_quit("usage: a.out <IPaddress>");

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_quit("inet_pton error for %s", argv[1]);

    // get mss and buffer
    
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, &len);
    getsockopt(mss, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);
    printf("before connect\nrecvbuf: %d\nmss: %d\n", recvbuf, mss);

    if(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");
    
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, &len);
    getsockopt(mss, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);
    printf("after connect\nrecvbuf: %d\nmss: %d\n", recvbuf, mss);
    
    while( (n = read(sockfd, recvline, MAXLINE)) > 0){
        recvline[n] = 0;
        if(fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    if(n < 0)
        err_sys("read error");
    
    exit(0);
}