#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

void *tcp_server_thread(void *arg);

#endif
