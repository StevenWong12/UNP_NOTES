#include "unp.h"

char buf[48];

int inet_pnet_loose(const char* strptr){
    if(inet_pton(AF_INET, strptr, buf) == 1)
        printf()
};

int main(int argc, char** argv){
    if(argc != 2){
        printf("./loose <addr_str>\n");
        exit(1);
    }

}