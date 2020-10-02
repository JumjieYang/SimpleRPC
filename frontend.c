#include "rpc.h"
#include "string.h"
#include"a1_lib.h"

#define BUFFSIZE 1024

rpc_t *RPC_Connect(char *name, int port){
    int sockfd;
    connect_to_server(name,port, &sockfd);
    rpc_t *rpc = (rpc_t *) malloc(sizeof(rpc_t));
    rpc -> sockfd = sockfd;
    return rpc;
}


void RPC_Call(rpc_t *r, char *statement){
    char server_msg[BUFFSIZE] = {0};
    send_message(r->sockfd,statement,strlen(statement));
    ssize_t byte_count = recv_message(r->sockfd, server_msg, sizeof(server_msg));
    if (byte_count <=0) {
        exit(1);
    }
    printf("%s\n",server_msg);
}

// void RPC_close(rpc_t *r){
//     free(r);
// }

int main(int argc, char **argv){
    rpc_t *backend = RPC_Connect(argv[1],atoi(argv[2]));
    char statement[1024] = {0};
    while(1){
        printf(">> ");
        fgets(statement,50, stdin);
        // RPC_Call(backend,statement);
        RPC_Call(backend,statement);
    }
    // RPC_Close(backend);
    return 0;
}
