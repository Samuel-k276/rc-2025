#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

void init_tcp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res);
void* tcp_server_thread(void* arg);

#endif

