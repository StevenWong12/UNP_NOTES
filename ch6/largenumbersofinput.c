#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

int main(int argc, char** argv){
    fd_set set;
    char buffer[3];
    char r;
    FD_ZERO(&set);
    /*
    while(1){
        FD_SET(fileno(stdin), &set);
        select(fileno(stdin)+1, &set, NULL, NULL, NULL);

        printf("read from stdin.\n");
        read(fileno(stdin), buffer, sizeof(buffer));
    }

    */
    while(1){
        FD_SET(fileno(stdin), &set);
        select(fileno(stdin) +1, &set, NULL, NULL, NULL);

        printf("get from stdin.\n");
        r = fgetc(stdin); // stdio stream

    }
    
    exit(0);
}