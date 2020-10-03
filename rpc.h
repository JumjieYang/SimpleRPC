#ifndef RPC
#define RPC
#define BUFSIZE 20

typedef struct functionData{
    char *functionName;
    void *callback;
} functionData;

typedef struct functionNode{
    functionData *data;
    struct functionNode *next;
} functionNode;

typedef struct rpc_t {
        int sockfd;
        int clientfd;
        functionNode *fn;
} rpc_t;


rpc_t *RPC_Init(char *host, int port);
void RPC_Register(rpc_t *r, char *name, void *fn);
rpc_t *RPC_Connect(char *name, int port);
void RPC_Close(rpc_t *r);
void RPC_Call(rpc_t *r, char *args);

#endif