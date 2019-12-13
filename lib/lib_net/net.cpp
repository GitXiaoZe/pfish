#include "net.h"

NetServer::NetServer(unsigned short port_)
     : port(port_){
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port_);
}
void NetServer::initialize(){
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

    //--------------------------------------------------
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
}

void NetServer::start(){
    int active_fds;
    struct epoll_event waiting_events[MAX_EVENTS];
    struct epoll_event accept_event;
    assert(listen_socket_fd > 0);

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
            } else { // need to process data 

            }
        }
    }

}
