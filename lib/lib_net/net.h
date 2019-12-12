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



#define OUTSTANDING_LISTENQ 5
#define MAX_EVENTS 100
#define BUF_SIZE 1<<30

class netServer{
    public :
        netServer(unsigned short port_);
        void initialize();
        void start();
        
    private :
        unsigned short port; //the port to listen
        int listen_socket_fd;
        struct sockaddr_in serveraddr;

        int listen_epoll_fd;
        struct epoll_event listen_connection_event;
};

#endif // !NET_H