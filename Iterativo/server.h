#pragma once 

#include <netinet/in.h>

extern int serverSocket;

int serverInit(const char *ip, int port);
int serverAccept(int listen_fd, struct sockaddr_in *client_addr);
void serverLoop(int slaveSocket);

#define BUFSIZE 512