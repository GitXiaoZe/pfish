#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <sys/sysinfo.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <vector>
#include <map>
#include <memory>
#include <utility>

#define PATH_BUF 1024
#define NODE_PATH "/sys/devices/system/node"
#define FILE_BUF 4096


class Socket{
    public:
        Socket(unsigned int sockets_id_) 
            : socket_id(sockets_id_){
                cpu_ids = std::make_shared< std::vector<int> >();
            }
        unsigned int socket_id;
        std::shared_ptr< std::vector<int> > cpu_ids;
};

class Core{
    public :
        pthread_t thread_id;
        unsigned core_id;
        unsigned socket_id;
        //This variable indicate that the core's role : runing task, or sending data(RPC message, shuffle);
        unsigned role; 
};

class Executor{
    public :
        Executor(unsigned int cores_num_);
        void initialize();

    private :
        unsigned int sockets_num;
        unsigned int cores_num;
        unsigned int getSocketNum();
        std::shared_ptr< std::map<int, std::shared_ptr<Socket> > > cpu_infos;
};


#endif // !CORE_H