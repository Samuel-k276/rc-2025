#include "send.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool send_tcp_command(int socket_fd, std::string &buffer, struct addrinfo *res) {
    ssize_t n = connect(socket_fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to connect TCP command to server" << std::endl;
        close(socket_fd);
        return false;
    }
    n = write(socket_fd, buffer.c_str(), buffer.length());
    if (n == -1) {
        std::cerr << "Failed to send TCP command to server" << std::endl;
        close(socket_fd);
        return false;
    }
    char response[4096];
    n = read(socket_fd, response, sizeof(response) - 1);
    if (n == -1) {
        std::cerr << "Failed to receive UDP command to server" << std::endl;
        close(socket_fd);
        return false;
    }
    response[n] = '\0';
    close(socket_fd);
    return true;
}

bool send_udp_command(int socket_fd, std::string &message, struct addrinfo *res) {
    struct sockaddr_in addr;

    int n = sendto(socket_fd, message.c_str(), message.length(), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to send UDP command to server" << std::endl;
        return false;
    }
    message.pop_back();
    std::cout << "Sent UDP command to server: " << message << std::endl;

    socklen_t addrlen = sizeof(addr);
    char response[4096];
    n = recvfrom(socket_fd, response, sizeof(response), 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1) {
        perror("recvfrom");
        exit(1);
    }

    n = write(1, "Echo from server: ", 18);
    if (n == -1) {
        perror("stdin");
        exit(1);
    }
    n = write(1, response, n);
    if (n == -1) {
        perror("buffer");
        exit(1);
    }
    return true;
}