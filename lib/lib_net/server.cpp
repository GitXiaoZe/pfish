#include "net.h"

int main(int argc, char ** argv){
    netServer *netserver = new netServer(50000);
    netserver->initialize();
    netserver->start();
    return 0;
}