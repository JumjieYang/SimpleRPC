#include "rpc.h"
#include "string.h"
#include "network.h"
#include "message.h"
#define BUFFSIZE 1024

rpc_t *RPC_Connect(char *name, int port){
    int sockfd;
    connect_to_server(name,port, &sockfd);
    rpc_t *rpc = (rpc_t *) malloc(sizeof(rpc_t));
    rpc -> sockfd = sockfd;
    return rpc;
}


void RPC_Call(rpc_t *r, message_t *msg){
    char ret[1024];
    send_message(r->sockfd,(char*)msg,sizeof(*msg));
    ssize_t byte_count = recv_message(r->sockfd, ret, sizeof(ret));
    if (byte_count <=0){
        free(r);
        free(msg);
        exit(1);
    }
    printf("%s\n",ret);
    if (!strcmp(ret,"Bye!")){
        free(r);
        free(msg);
        exit(0);
    }
    free(msg);
}

void RPC_close(rpc_t *r){
    free(r);
}

int main(int argc, char **argv){
    rpc_t *backend = RPC_Connect(argv[1],atoi(argv[2]));
    char statement[1024];
    char *find;
    while(1){
        memset(statement,NULL,1024);
        printf(">> ");
        fgets(statement,50, stdin);
        RPC_Call(backend,parsingInput(statement));
    }
    RPC_Close(backend);
    return 0;
}
