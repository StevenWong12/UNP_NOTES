#include "unp.h"

void handler(int sockfd){

}

int main(int argc, char **argv){
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, connfd;
    pid_t pid;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERV_PORT);

    listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    Bind(listenfd, (sockaddr*) servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    while(1){
        connfd = Accept(listenfd, (sockaddr*) cliaddr, sizeof(cliaddr));
        pid = Fork();
        if(pid == 0){
            Close(listenfd);
            handler(connfd);
            Close(connfd);
            exit(0);
        }
        Close(connfd);
    }

    exit(0);
}