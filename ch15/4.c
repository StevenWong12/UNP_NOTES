#include "unp.h"
#include <setjmp.h>

static jmp_buf env;

static void alarm_handler(int signo){
    longjmp(env, 1);
}

void clear(int *fd, int n){
    for(int i = 0; i < n; i++)
        close(fd[i]);
}

int main(int argc, char *argv[]){
    int p1[2], listenfd, listen_q, connfd[100], backlog = 0;
    struct sockaddr_in servsock, connsock[100];
    Sigfunc *sigfunc;
    pid_t childpid;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    Pipe(p1);

    if((childpid = Fork()) == 0){
        close(p1[1]);
        listenfd = Socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servsock, sizeof(servsock));
        servsock.sin_family = AF_INET;
        servsock.sin_port = htons(SERV_PORT);
        Inet_pton(AF_INET, "127.0.0.1", &servsock.sin_addr);


        Bind(listenfd, (SA *)&servsock, sizeof(servsock));
        Read(p1[0], &listen_q, sizeof(int));

        Listen(listenfd, listen_q);
        pthread_cond_signal(&cond);
        for(; ;);
    }   
    else{
        sigfunc = Signal(SIGALRM, alarm_handler);
        close(p1[0]);
        bzero(&connsock, sizeof(connsock));
        for(int i = 0; i < 100; i++){
            connsock[i].sin_family = AF_INET;
            connsock[i].sin_port = htons(SERV_PORT);
            Inet_pton(AF_INET, "127.0.0.1", &connsock[i].sin_addr);
            connfd[i] = Socket(AF_INET, SOCK_STREAM, 0);
        }

        while(backlog < 14){
            backlog++;
            write(p1[1], &backlog, sizeof(int));
            pthread_cond_wait(&cond, &mutex);
            for( int i = 0 ; i < 100 ; i++ ){
                if(setjmp(env) != 0){
                    printf("current backlog is: %d, maximum backlog is: %d\n", backlog, i);
                    clear(connfd, i);
                    break;
                }
                alarm(2);
                Connect(connfd[i], (SA *)&connsock[i], sizeof(connsock[i]));
            }
        }
    }

    exit(0);
}