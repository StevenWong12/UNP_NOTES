#include "unp.h"
#include <time.h>

int main(int argc, char *argv[]){
    int listenfd, connfd;
    socklen_t len;
    char buff[MAXLINE];
    time_t ticks;
    struct sockaddr_un servsock, clisock; 


    unlink(UNIXSTR_PATH);
    servsock.sun_family = AF_LOCAL;
    strcpy(servsock.sun_path, UNIXSTR_PATH);

    listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    
    Bind(listenfd, (SA *)&servsock, sizeof(servsock));
    Listen(listenfd, LISTENQ);

    for( ; ; ){
        len = sizeof(clisock);
        connfd = Accept(listenfd, (SA *)&clisock, &len);
        printf("connection from %s\n", Sock_ntop( (SA *)&clisock, len));

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        for(int i = 0; i < sizeof(buff); i++){
            send(connfd, &buff[i], 1, MSG_EOR);
        }

        Close(connfd);
    }
}