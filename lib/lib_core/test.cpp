#include "core.h"

int main(int argc, char **argv){
    int core = 1;
    Executor *e = new Executor(core);
    e->initialize();
    return 0;
}