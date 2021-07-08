#include "unp.h"

char buf[48];

void inet_pnet_loose(const char* strptr, struct in6_addr *addptr){
    if(inet_pton(AF_INET6, strptr, (char*)buf) == 1){
        printf("inet_pton success!\n");
        printf("IPv6 address is: %s\n", buf);
    }
    else
        printf("inet_pton failed!\n");
    
    if(inet_aton(strptr, addptr) == 1){
        printf("inet_aton success!\n");
        printf("IPv6 address is %d\n", addptr->s6_addr);
    }
    
};

int main(int argc, char** argv){
    if(argc != 2){
        printf("./loose <addr_str>\n");
        exit(1);
    }
    struct in6_addr *addptr = malloc(sizeof(struct in6_addr)); 
    inet_pnet_loose(argv[1], addptr);

    exit(0);
}