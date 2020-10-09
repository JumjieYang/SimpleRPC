#ifndef NETWORK
#define NETWORK

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int create_server(const char *host, uint16_t port, int *sockfd);
int accept_connection(int sockfd, int *clientfd);
int connect_to_server(const char *host, uint16_t port, int *sockfd);
ssize_t send_message(int sockfd, const char *buf, size_t len);
ssize_t recv_message(int sockfd, char *buf, size_t len);

#endif

