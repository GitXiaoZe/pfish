#include "Master.h"


Master::Master(){
    net_server = std::make_shared<NetServer>(NETSERVER_PORT);
    worker_nodes = std::make_shared< std::map<unsigned int, std::shared_ptr<WorkerNode> > >();
}

void Master::initialize(){
    net_server->setMaster(shared_from_this());
    net_server->initialize();
}

void Master::registerWorkerNode(unsigned int ip_, unsigned short port_, int sock_fd_, 
        unsigned int cores_, unsigned int sockets_, unsigned long memory_){
        std::shared_ptr<WorkerNode> new_workernode 
            = std::make_shared<WorkerNode>(cores_, sockets_, memory_, ip_, port_);
        new_workernode->setConnectionFd(sock_fd_);
        worker_nodes->insert(std::pair<unsigned int, std::shared_ptr<WorkerNode>>(ip_, new_workernode));        
}

