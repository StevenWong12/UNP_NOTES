#include "unp.h"


// use wait to recycle the zombie process
void sig_chld(int signo){
    pid_t pid;
    int stat;

    pid = wait(&stat);
    printf("child &d terminated\n", pid);
    return ;
}

// use waitpid to recycle the zombie process (the correct version)

void sig_chld(int signo){
    pid_t pid;
    int stat;

    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0 )
        printf("child &d terminated\n", pid);

    return ;
    
}