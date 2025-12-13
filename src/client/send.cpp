#include "send.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool send_tcp_command(int socket_fd, char *buffer) {
    // TODO: Implement TCP command sending
    return false;
}

bool send_udp_command(int socket_fd, std::string &message, struct addrinfo *res) {
    int n = sendto(socket_fd, message.c_str(), message.length(), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to send UDP command to server" << std::endl;
        return false;
    }
    message.pop_back();
    std::cout << "Sent UDP command to server: " << message << std::endl;
    return true;
}