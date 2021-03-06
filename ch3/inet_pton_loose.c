#include "unp.h"

char buf[48];

void inet_pnet_loose(const char* strptr, struct in_addr *addptr){
    if(inet_pton(AF_INET, strptr, (char*)buf) == 1){
        printf("inet_pton success!\n");
        printf("IPv4 address is: %s\n", buf);
    }
    else
        printf("inet_pton failed!\n");
    
    if(inet_aton(strptr, addptr) == 1){
        printf("inet_aton success!\n");
        printf("IPv4 address is %x\n%x\n", inet_lnaof(*addptr), inet_netof(*addptr));

    }
    
};

int main(int argc, char** argv){
    if(argc != 2){
        printf("./loose <addr_str>\n");
        exit(1);
    }
    struct in_addr *addptr = malloc(sizeof(struct in_addr)); 
    inet_pnet_loose(argv[1], addptr);

    exit(0);
}