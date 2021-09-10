#include "unp.h"

int main(int argc, char *argv[]){
    int sock;
    char buf[5] = "abcde";
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(sock, (SA*) &servaddr, len) == -1){
        printf("connect error, errno = %d\n", errno);
        exit(1);
    }

    send(sock, buf, sizeof(buf), 0);

    return 0;
}