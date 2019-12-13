#include "core.h"

Executor::Executor(unsigned int cores_num_) : cores_num(cores_num_), sockets_num(0){
    cpu_infos = std::make_shared< std::map<int, std::shared_ptr<Socket> > >();
    getSocketNum();

}

void Executor::initialize(){
    unsigned int available_cores = sysconf(_SC_NPROCESSORS_ONLN);

    if(cores_num > available_cores){
        printf("The number of cores(%d) to initialize must less or equal than the number of available cores(%d);",
            cores_num, available_cores);
    }

    unsigned int *available_cores_per_socket = new unsigned int[sockets_num];
    char path[PATH_BUF];
    unsigned int dir_length = strlen(NODE_PATH);
    memcpy(path, NODE_PATH, dir_length);

    //we assume that 0 < sockets_num < 10
    for(int i=0; i < sockets_num; i++){
        int path_length = dir_length;
        path[path_length++] = '/';
        memcpy(path + path_length, "node", 4);
        path_length += 4;
        path[path_length++] = (char)('0' + i); 
        path[path_length++] = '/';
        memcpy(path + path_length, "cpulist", 7);
        path_length += 7; 
        path[path_length] = '\0'; // path = "/sys/devices/system/node/nodeX/cpulist"

 
        assert(cpu_infos->find(i) == cpu_infos->end());

        std::shared_ptr<Socket> sock = std::make_shared<Socket>(i);
        cpu_infos->insert( std::pair<int, std::shared_ptr<Socket> >(i, sock) );

        FILE *fd = fopen(path, "r");
        if(fd == NULL){
            printf("Open file %s ERROR\n", path);
            exit(-1);
        }
        char buffer[FILE_BUF];        
        fgets(buffer, FILE_BUF, fd); //generally, we only read once
        int pre_ptr, post_ptr;
        bool tail = false;
        for(pre_ptr = 0; buffer[pre_ptr] != '\0'; pre_ptr = post_ptr + 1){
            for(post_ptr = pre_ptr + 1; buffer[post_ptr] != '\0' && buffer[post_ptr] != ','; post_ptr++);
            if( buffer[post_ptr] == '\0' ) tail = true;
            else buffer[post_ptr] = '\0';
            sock->cpu_ids->push_back(atoi(buffer + pre_ptr));
            if(tail) break;
        }
        available_cores_per_socket[i] = (unsigned int)(sock->cpu_ids->size());
    }

    //need to create Core
    


}

// private
unsigned int Executor::getSocketNum(){
    if(sockets_num > 0)
        return sockets_num;
    
    DIR *dir = opendir(NODE_PATH);
    if(dir == NULL){
        printf("Open file %s FAIL\n", NODE_PATH);
        exit(-1);
    }
    struct dirent *entry = NULL;
    while( (entry = readdir(dir)) != NULL){
        if( strstr(entry->d_name, "node") != NULL) sockets_num++;
    }
    closedir(dir);
}