#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#define BUF_SIZE 1<<30

//"172.16.100.1"
#define SERVER_IP 2886755329
#define SERVER_PORT 50000

int main(int argc, char **argv){

    int client_fd;
    //char *host = SERVER_IP;

    if( (client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
        printf("client create socket ERROR");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    memser(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = SERVER_IP;
    server_addr.sin_port = SERVER_PORT;

    if( connect(client_fd, (struct sockaddr*)server_addr, sizeof(server_addr)) < 0 ){
        printf("connect to server FAIL");
        exit(-1);
    }

    char *buf = (char *)malloc(BUF_SIZE);
    memset(buf, 0, BUF_SIZE);
    int cnt = 10;
    for(int i = 0; i < cnt; i++){
        write(client_fd, buf, BUF_SIZE);
    }
    free(buf);

    return 0;
}