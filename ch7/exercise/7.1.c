#include "unp.h"
#include <netinet/tcp.h>

int main(int argc, char** argv){
    int tcpfd, udpfd;
    int tcprcv, tcpsnd, udprcv, udpsnd;
    int mss;
    socklen_t len = sizeof(int);

    tcpfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    udpfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(getsockopt(tcpfd, IPPROTO_TCP,TCP_MAXSEG, &mss, &len) < 0)
        err_sys("fail to get tcp rcv.");
    if(getsockopt(tcpfd, SOL_SOCKET, SO_RCVBUF, &tcprcv, &len) < 0)
        err_sys("fail to get tcp rcv.");
    if(getsockopt(tcpfd, SOL_SOCKET, SO_SNDBUF, &tcpsnd, &len) < 0)
        err_sys("fail to get tcp snd.");
    if(getsockopt(udpfd, SOL_SOCKET, SO_RCVBUF, &udprcv, &len) < 0)
        err_sys("fail to get udp rcv.");
    if(getsockopt(tcpfd, SOL_SOCKET, SO_SNDBUF, &udprcv, &len) < 0)
        err_sys("fail to get udp snd.");

    printf("mss: %d\ntcp rcv: %d\ntcp snd: %d\nudp rcv: %d\nudp snd: %d\n", mss,tcprcv, tcpsnd, udprcv, udpsnd);

    exit(0);
}