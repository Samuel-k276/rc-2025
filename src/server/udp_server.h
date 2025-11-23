#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

void* udp_server_thread(void* arg);

void login(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr, socklen_t &addr_len);
void logout(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr, socklen_t &addr_len);

#endif