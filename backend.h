#ifndef BACKEND
#define BACKEND
#include <stdint.h>
#include "rpc.h"
#include "message.h"
int addInts(int a, int b);

int multiplyInts(int a, int b);

float divideFloats(float a, float b);

int Sleep (int x);

uint64_t factorial(int x);

functionNode *findIndex(rpc_t *r, char *name);

void registerFunctions(rpc_t *r);

void validateArgs(char **args);

void handleMessage(rpc_t *r, message_t *message);

#endif