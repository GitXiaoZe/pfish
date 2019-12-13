#include "net.h"
#include "../master/Master.h"

NetServer::NetServer(unsigned short port_)
     : port(port_){
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port_);
}
void NetServer::initialize(){
    //-----------------initialize listening socket-------------------
    if( (listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        printf("netServer initialization ERROR : listen_socket_fd initialization;\n");
        exit(-1);
    }

    int optval = 1;
    if( setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, 
            (const void*)&optval, sizeof(optval)) < 0 ){
        printf("netServer initialization ERROR : set SO_REUSEADDR;\n");
        exit(-1);
    }
    
    if( bind(listen_socket_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0 ){
        printf("netServer initialization ERROR : bind socket;\n");
        exit(-1);
    }

    if( listen(listen_socket_fd, OUTSTANDING_LISTENQ) < 0){
        printf("netServer initalization ERROR : mark a connection-mode socket;\n");
        exit(-1);
    }

    //-------------------initialize an epoll instance,  and add the listening socket to it------------------------ 
    if( (listen_epoll_fd = epoll_create(MAX_EVENTS)) < 0 ){
        printf("netServer initalization ERROR : create epoll fd;\n");
        exit(-1);
    }

    listen_connection_event.events = EPOLLIN;
    listen_connection_event.data.fd = listen_socket_fd;
    if( epoll_ctl(listen_epoll_fd, EPOLL_CTL_ADD, listen_socket_fd, &listen_connection_event) < 0 ){
        printf("netServer initalization ERROR : epoll ctl;\n");
        exit(-1);
    }

    //-------------------initialize a map to store workers' ip and port---------------
    fd2ip_port = std::make_shared< std::map<int, unsigned long> >();

}

void NetServer::start(){
    assert(listen_socket_fd > 0);
    
    int active_fds;
    struct epoll_event waiting_events[MAX_EVENTS];
    struct epoll_event accept_event;

    char msg_buf[MSG_BUF_SIZE];

    for(;;){
        active_fds = epoll_wait(listen_epoll_fd, waiting_events, MAX_EVENTS, -1);
        if( active_fds < 0 ){
            printf("netServer running ERROR : epoll wait;\n");
            exit(-1);
        }
        for(int i=0; i < active_fds; i++){
            if( waiting_events[i].data.fd == listen_socket_fd ){
                struct sockaddr client_addr;
                int addr_len;
                int conn_sock = accept(listen_socket_fd, &client_addr, (socklen_t*)&addr_len);
                if( conn_sock < 0){
                    printf("netServer running ERROR : accepting connection;\n");
                    exit(-1);
                }
                //may need to set edge-trigger 
                accept_event.events = EPOLLIN;
                accept_event.data.fd = conn_sock;
                if( epoll_ctl(listen_epoll_fd, EPOLL_CTL_ADD, conn_sock, &accept_event) < 0 ){
                    printf("netServer running ERROR : adding new connection;\n");
                    exit(-1);
                }else{
                    printf("Accept a new connection;\n");
                }
                
                struct sockaddr_in* client_addr_in = (struct sockaddr_in*)&client_addr; 
                unsigned long ip_port = (ntohl(client_addr_in->sin_addr.s_addr) << PORT_LEN) | (ntohs(client_addr_in->sin_port)); 
                fd2ip_port->insert( std::pair<int, unsigned long>(conn_sock, ip_port));
            } else { // need to process data 
                std::map<int, unsigned long>::iterator iter = fd2ip_port->find(waiting_events[i].data.fd);
                assert(iter != fd2ip_port->end());
                int sock_fd = waiting_events[i].data.fd;
                unsigned long ip_port = iter->second;
                unsigned short port = (unsigned short)ip_port;
                unsigned int ip = (unsigned int)(ip_port >> PORT_LEN);
                int size = -1;
                if( (size = read(sock_fd, msg_buf, MSG_BUF_SIZE)) < 0){// we assume that each RPC message size <= MSG_BUF_SIZE
                    printf("netServer running ERROR when reading data from %d.%d.%d.%d:%d\n", 
                        ((ip & 0xFF000000)>>24), ((ip & 0x00FF0000)>>16), ((ip & 0x0000FF00)>>8), ((ip & 0x000000FF)), port);
                    exit(-1);
                }
                switch(msg_buf[0]){
                    case REGISTER_WORKER :
                        master->registerWorkerNode(ip, port, sock_fd,
                            *(unsigned int*)(msg_buf + 1),
                            *(unsigned int*)(msg_buf + 5),
                            *(unsigned long*)(msg_buf + 9));

                        break;
                    default : 
                        printf("Unknown Msg : %s\n", msg_buf); 
                }
                
            }
        }
    }

}
