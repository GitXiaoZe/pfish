#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <errno.h>
#include <assert.h>

#include <map>
#include <memory>
#include <utility>

#include "rpc_msg.h"

#define OUTSTANDING_LISTENQ 5
#define MAX_EVENTS 100
#define MSG_BUF_SIZE 4096

#define PORT_LEN 16
#define PORT_MASK 0xFFFF
#define IP_LENGTH 32
#define IP_MASK 0xFFFFFFFF

class Master;

class NetServer{
    public :
        NetServer(unsigned short port_);
        void initialize();
        void start();
        void setMaster(std::shared_ptr<Master> master_){
            master = master_;
        }
        
        unsigned short port; //the port to listen
        int listen_socket_fd;
        struct sockaddr_in serveraddr;

        int listen_epoll_fd;
        struct epoll_event listen_connection_event;

        std::shared_ptr<Master> master;
        std::shared_ptr< std::map<int, unsigned long> > fd2ip_port; // convert a network fd to the ip & port of a worker
};

#endif // !NET_H