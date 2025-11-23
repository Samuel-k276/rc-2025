#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void init_udp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res);

#endif