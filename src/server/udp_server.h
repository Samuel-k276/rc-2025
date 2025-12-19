#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

void init_udp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res);
void handle_udp_message(int socket_fd, bool verbose);

void login(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr, socklen_t &addr_len);
void logout(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
            socklen_t &addr_len);
void unregister(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
                socklen_t &addr_len);
void list_my_events(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
                    socklen_t &addr_len);

#endif