#include "unp.h"

int main(int argc, char *argv[]){
    int sockfd, n;
    char recvline[MAXLINE+1];
    socklen_t len;
    struct sockaddr_un servsock;
    
    servsock.sun_family = AF_LOCAL;
    strcpy(servsock.sun_path, UNIXSTR_PATH);

    sockfd = Socket(AF_LOCAL, SOCK_STREAM, 0);
    len = sizeof(servsock);
    connect(sockfd, (SA *)&servsock, len);
    Getpeername(sockfd, (SA *)&servsock, &len);
    printf("connected to %s\n", Sock_ntop_host((SA *)&servsock, len));
    sleep(5);

    while( (n = Read(sockfd, recvline, MAXLINE)) > 0){
        printf("number of bytes: %d\n", n);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }

    exit(0);
}