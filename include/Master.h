#ifndef MASTER_H
#define MASTER_H
#define WORKERS 100
extern "C" {
    struct worker_metainfo{
        unsigned int cores;
        unsigned int memory;
    };
}

class Master{
    public :
        Master();
        void initialize();
    private :
        worker_metainfo workers[WORKERS];
};


#endif // !MASTER_H
