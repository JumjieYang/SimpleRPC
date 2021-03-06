#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

int sleepServer(int x){
    return sleep(x);
}

uint64_t factorial(int x){
    if (x == 0 || x == 1)
        return 1;
    return x * factorial(x-1);
}


rpc_t *RPC_Init(char *host, int port){
    int sockfd,clientfd;
    functionNode *fn = (functionNode*) malloc(sizeof(functionNode));
    if (create_server(host,port, &sockfd)<0){
        printf("error happened\n");
        exit(1);
    };
    rpc_t *rpc = (rpc_t *) malloc(sizeof(rpc_t));
    rpc ->sockfd = sockfd;
    rpc ->clientfd = clientfd;
    printf("Server listening on port %s:%d\n",host,port);
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
    functionNode *temp;

   while (r ->fn != NULL)
    {
       temp = r -> fn;
       r->fn = r->fn->next;
       free(temp);
    }
    free (r);
}


functionNode *findIndex(rpc_t *r, char *command){
    functionNode *node = r ->fn;
    while (node != NULL){
        if (strcmp(node ->data ->functionName, command)==0)
            return node;
        node = node ->next;
    }
    sprintf(ret,"Error: Command \"%s\" not found",command);
    return NULL;
}

void registerFunctions(rpc_t *rpc){
    RPC_Register(rpc, "add",addInts);
    RPC_Register(rpc, "multiply",multiplyInts);
    RPC_Register(rpc, "divide",divideFloats);
    RPC_Register(rpc, "sleep",sleepServer);
    RPC_Register(rpc, "factorial",factorial);
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

void handleMessage(rpc_t *r, message_t*msg){
    memset(ret,NULL,sizeof(ret));
    functionNode *node = findIndex(r,msg->command);
    if (node != NULL){
        if (!strcmp(msg->command, "add") || !strcmp(msg->command, "multiply") || !strcmp(msg->command, "divide")){
                if (!strcmp(msg->command, "divide")){
                    float (*p)(float,float) = node->data->callback;
                    float f1, f2,result;
                    if (strlen(msg->arg1) != 0&& strlen(msg->arg2) !=0){
                        f1 = atof(msg->arg1);
                        f2 = atof(msg->arg2);
                    }
                    else{
                        sprintf(ret,"Invalid Arguments");
                        return;
                    }
                    result = p(f1,f2);
                    if (!strlen(ret))          
                    sprintf(ret,"%.6f",result);
                }
                else {
                    int (*p)(int,int) = node -> data -> callback;
                    int n1,n2;
                    if (strlen(msg->arg1) ==0 || strlen(msg->arg2) == 0){
                        sprintf(ret,"Invalid Arguments");
                        return;
                    }
                    else{
                        n1 = atoi(msg->arg1);
                        n2 = atoi(msg->arg2);
                            sprintf(ret,"%d",p(n1,n2));
                    }
                }
            }
        else if(!strcmp(msg->command, "sleep") || !strcmp(msg->command, "factorial")){
            int (*p) (int) = node -> data->callback; 
            int n1;
            if (!strlen(msg->arg1)){
                sprintf(ret,"Invalid Arguments");
                return;
            }
            else{
                n1 = atoi(msg->arg1);
                sprintf(ret,"%d",p(n1));
            }
        }
    }
}

int serve(rpc_t *rpc) {
    message_t *msg = (message_t*)malloc(sizeof(message_t));
    const int size = sizeof(message_t);
    while(1){
        ssize_t byte_count = recv_message(rpc->clientfd,(char*)msg,size);
        if (byte_count <=0)
            return 1;
        if (!strcmp(msg->command,"exit")){
            sprintf(ret, "Bye!");
            send_message(rpc->clientfd,ret,size);
            return 1;
        }
        if (!strcmp(msg->command,"quit") ){
            sprintf(ret, "Bye!");
            send_message(rpc->clientfd,ret,size);
            return 10;
        }
        if (!strcmp(msg->command,"shutdown")){
            sprintf(ret, "Bye!");
            send_message(rpc->clientfd,ret,size);
            return 10;
        }
            handleMessage(rpc,msg);
            printf("%d\n",send_message(rpc->clientfd,ret,1024));
    }
    memset(msg,NULL,sizeof(message_t));

}
int main(int argc, char **argv) { 
    int pid;
    int pids[10] = {0};
    int alive_client=0;
    int rval;
    int status;
    int stop =0;
    int count =0;
    validateArgs(argv);
    rpc_t *rpc = RPC_Init(argv[1],atoi(argv[2]));
    registerFunctions(rpc);

A:  accept_connection(rpc->sockfd, &rpc->clientfd);
   alive_client +=1;
   pid = fork();

   if (pid == 0){
       return serve(rpc);
   }else {
       pids[count] = pid;
       count +=1;
   }

   while (1) {
       sleep(1);
       if (stop ==0){
           for (int i=0; i< 10; i++){
               printf("pid: %d\n",pids[i]);
               if(pids[i] != 0){
                   waitpid(pids[i], &rval, WNOHANG);
                   status = WEXITSTATUS(rval);
                   printf("status %d\n", status);
                   if (status ==1){
                       pids[i] =0;
                       alive_client -=1;
                       if (alive_client <5){
                           goto A;
                       }
                   }
                   else if (status == 10){
                       stop = 1;
                       break;
                   }
               }
               else if (alive_client <5 && count == alive_client){
                   goto A;
               }
           }
       }
       else
           for (int j = 0; j< 10; j++){
               waitpid(pids[j],NULL,WNOHANG);
               return 0;
       }
   }
}