#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

void init_tcp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res);
void handle_tcp_client(int client_fd, bool verbose, struct sockaddr_in &client_addr); // Handles request and closes connection

#endif
