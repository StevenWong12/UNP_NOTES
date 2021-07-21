#include "unp.h"

void handler(int sockfd){
    ssize_t n;
    char temp_buf[100];
    
again:
    while((n = read(sockfd, temp_buf, 100)) > 0)
        Writen(sockfd, temp_buf, n);

    if(n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        perror("read error.\n");
}

int main(int argc, char **argv){
    int serv_sock, on;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t len, clnt_len;
    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&clnt_addr, sizeof(clnt_addr));

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    len = sizeof(serv_addr);
    
    /*
    on = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    */

    bind(serv_sock, (SA*) &serv_addr, len);

    listen(serv_sock, LISTENQ);

    for(; ; ){
        int clnt_sock = accept(serv_sock, (SA*)&clnt_addr, &clnt_len);

        handler(clnt_sock);
    }

    exit(0);
}