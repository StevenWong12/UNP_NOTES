#include "unp.h"

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock, ret;
    socklen_t len;
    char buf[5];
    struct sockaddr_in serv_addr, clnt_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);

    serv_sock = Socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(serv_sock);
    Bind(serv_sock, (SA *)&serv_addr, sizeof(serv_addr));

    Listen(serv_sock, 5);
    printf("Start to listen...\n");

    for(; ;){
        clnt_sock = Accept(serv_sock, (SA*)&clnt_addr, &len);
        printf("successfully accept a new connect.\n");
        sleep(5);
        ret = recv(clnt_sock, buf, sizeof(buf), MSG_PEEK);

        printf("readable length: %d\n", ret);
        printf("buffer: %s\n", buf);

        ret = recv(clnt_sock, buf, sizeof(buf), 0);
        
        printf("readable length: %d\n", ret);
        printf("buffer: %s\n", buf);
    }

    exit(0);
    
}