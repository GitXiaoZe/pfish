#ifndef MASTER_H
#define MASTER_H

#include <map>
#include <memory>
#include <utility>

#include "../lib_net/net.h"

#define NETSERVER_PORT 50000

class WorkerNode{
    public :
        WorkerNode(unsigned int cores_, unsigned int sockets_, unsigned long memory_, 
                    unsigned int ip_, unsigned short port_) 
                    : cores(cores_), sockets(sockets_), memory(memory_), ip(ip_), port(port_){}

        WorkerNode(unsigned int ip_, unsigned short port_) : WorkerNode(0, 0, 0, ip_, port_){}
        void setResourceInfo(unsigned int cores_, unsigned int sockets_, unsigned int memory_){
            cores = cores_;
            sockets = sockets_;
            memory = memory_;
        }
        void setConnectionFd(int connection_socket_fd_){
            connection_socket_fd = connection_socket_fd_;
        }
    unsigned int cores;
    unsigned int sockets;
    unsigned long memory; //In bytes;
    unsigned int ip;
    unsigned short port;
    int connection_socket_fd;
};

class Master : public std::enable_shared_from_this<Master>{
    public : 
        Master();
        void initialize();
        void registerWorkerNode(unsigned int ip_, unsigned short port_, int sock_fd_, 
                unsigned int cores_, unsigned int sockets_, unsigned long memory_);

        std::shared_ptr<NetServer> net_server;
        std::shared_ptr< std::map<unsigned int, std::shared_ptr<WorkerNode> > > worker_nodes;

    
};

#endif // !MASTER_H
