#include "unp.h"

void sig_pipe(int signo){
    printf("calling SIGPIPE handler function.\n");
    return;
}

/*
void str_cli(FILE *fp, int sockfd){
    char  sendline[MAXLINE], recvline[MAXLINE];
    while(Fgets(sendline, MAXLINE, fp) != NULL){
        Writen(sockfd, sendline, strlen(sendline));
        if(Readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely");
        Fputs(recvline, stdout);
    }
}
*/

int main(int argc, char** argv){
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2)
        err_quit("usage: tcpcli <IPaddress>");
    
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Signal(SIGPIPE, sig_pipe);

    Connect(sockfd, (SA*) &servaddr, sizeof(servaddr));
    sleep(2);

    char buf[10] = "11aabbccdd";


    for(; ; ){
        write(sockfd, buf, sizeof(buf));
        sleep(2);
    }
    /*
    write(sockfd, buf, sizeof(buf));
    sleep(2);
    */
    exit(0);
}