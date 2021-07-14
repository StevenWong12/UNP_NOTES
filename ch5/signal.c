#include "unp.h"

Sigfunc * signal(int signo, Sigfunc* func){
    struct sigaction act, oact;
}