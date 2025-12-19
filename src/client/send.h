#ifndef SEND_H
#define SEND_H

#include "input_handler.h"
#include <netdb.h>
#include <string>

// Send command over TCP (opens new connection each time)
bool send_tcp_command(std::string &buffer, struct addrinfo *res, std::string &response);

// Send command over UDP using existing socket
bool send_udp_command(int socket_fd, std::string &message, struct addrinfo *res, std::string &response);

#endif