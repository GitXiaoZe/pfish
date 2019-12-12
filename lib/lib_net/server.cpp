#include "net.h"

int main(int argc, char ** argv){
    netServer *netserver = new netServer((unsigned short)50000);
    netserver->initialize();
    netserver->start();
    return 0;
}