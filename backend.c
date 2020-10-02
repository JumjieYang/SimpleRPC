#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rpc.h"
#include "a1_lib.h"
char ret[1024];
int addInts(int a, int b) {
    return a + b;
}

int multiplyInts(int a, int b){
    return a * b;
}

float divideFloats(float a, float b){
    if (b == 0){
        sprintf(ret,"Cannot divide by 0");
        return 0;
    }
    return a / b;
}

int sleep(int x){
    long start_time = clock();
    while (1)
    {
        if ((int)(clock() - start_time)/ CLOCKS_PER_SEC >= x)
            return 0;
    }
    return 1;
}

uint64_t factorial(int x){
    if (x == 0 || x == 1)
        return 1;
    return x * factorial(x-1);
}


rpc_t *RPC_Init(char *host, int port){
    int sockfd;
    functionNode *fn = (functionNode*) malloc(sizeof(functionNode));
    if (create_server(host,port, &sockfd)<0){
        printf("error happened\n");
        exit(1);
    };
    rpc_t *rpc = (rpc_t *) malloc(sizeof(rpc_t));
    rpc ->sockfd = sockfd;
    return rpc;
}

void RPC_Register(rpc_t *r, char *name, void *fn){
    functionData *fd = (functionData*) malloc (sizeof(functionData));
    functionNode *function_node = (functionNode *)malloc (sizeof(functionNode));
    fd ->functionName = name;
    fd ->callback = fn;
    function_node ->data = fd;
    function_node -> next = NULL;
    if (r -> fn == NULL)
        r -> fn = function_node;
    else{
        functionNode *current = r -> fn;
        while ( current ->next != NULL)
            current = current -> next;
        current-> next = function_node;
    }
}


void RPC_Close(rpc_t *r){

}


functionNode *findIndex(rpc_t *r, char *command){
    functionNode *node = r ->fn;
    while (node != NULL){
        if (strcmp(node ->data ->functionName, command)==0)
            return node;
        node = node ->next;
    }

    // remove "\n" at end
    int length = strlen(command) - 1;
    if (*command && command[length] == '\n') 
        command[length] = '\0';

    sprintf(ret,"Error: Command \"%s\" not found",command);
    return NULL;
}

void registerFunctions(rpc_t *rpc){
    RPC_Register(rpc, "add",addInts);
    RPC_Register(rpc, "multiply",multiplyInts);
    RPC_Register(rpc, "divide",divideFloats);
    RPC_Register(rpc, "sleep",sleep);
    RPC_Register(rpc, "factorial",factorial);

    // RPC_Register(rpc, "quit",RPC_Close);
}

void validateArgs(char **argv){
    if (argv[1] == NULL || argv[2] == NULL){
    printf("Input parameter invalid\n");
    exit(1);
    }
    int size = sizeof(argv[2])/sizeof(argv[2][0]);
    char buf[size];
    snprintf(buf,sizeof(buf),"%d",atoi(argv[2]));
    if (strcmp(argv[2],buf)){
        printf("port must be a number\n");
        exit(1);
    }
}

void handleMessage(rpc_t *r, char*msg){
    memset(ret,NULL,sizeof(ret));
    char *token;
    token = strtok(msg," ");
    functionNode *node = findIndex(r,token);
    if (node != NULL){
        if (!strcmp(token, "add") || !strcmp(token, "multiply") || !strcmp(token, "divide")){
                if (!strcmp(token, "divide")){
                    float (*p)(float,float) = node->data->callback;
                    float f1, f2,result;
                    token = strtok(NULL," ");
                    if (token != NULL)
                        f1 = atof(token);
                    else
                        sprintf(ret,"Invalid Arguments");
                    token = strtok(NULL," ");
                    if (token != NULL)
                        f2 = atof(token);
                    else
                        sprintf(ret,"Invalid Arguments");
                    result = p(f1,f2);
                    if (!strlen(ret))          
                    sprintf(ret,"%.6f",result);

                    
                }
                else {
                    int (*p)(int,int) = node -> data -> callback;
                    int n1,n2;
                    token = strtok(NULL," ");
                    if (token == NULL){
                        sprintf(ret,"Invalid Arguments");
                        return;
                    }
                    else{
                        n1 = atoi(token);
                        token = strtok(NULL," ");
                        if (token == NULL){
                            sprintf(ret,"Invalid Arguments");
                            return;
                        }
                        else{
                            n2 = atoi(token);
                            sprintf(ret,"%d",p(n1,n2));
                        }
                    }
                }
            }
        else if(!strcmp(token, "sleep") || !strcmp(token, "factorial")){
            int (*p) (int) = node -> data->callback; 
            int n1;
            token =strtok(NULL, " ");
            if (token == NULL){
                sprintf(ret,"Invalid Arguments");
                return;
            }
            else{
                n1 = atoi(token);
                sprintf(ret,"%d",p(n1));
            }
        }
        else if (!strcmp(token, "quit")){
            node-> data ->callback;
        }
    }
}

int main(int argc, char **argv) { 
    int clientfd;
    validateArgs(argv);
    rpc_t *rpc = RPC_Init(argv[1],atoi(argv[2]));
    registerFunctions(rpc);
    accept_connection(rpc->sockfd,&clientfd);
    char message[1024];
    while(1){
    recv_message(clientfd,message,1024);
    printf("Client: %s",message);
    handleMessage(rpc,message);
    printf("%s\n",ret);
    send_message(clientfd,ret,1024);
    memset(message,'\0',sizeof(message));
    }
    return 0;
    }