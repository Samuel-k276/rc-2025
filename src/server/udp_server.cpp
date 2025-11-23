#include "udp_server.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../common/constants.h"

void init_udp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;

    int error = getaddrinfo(NULL, port, &hints, &res);
    if (error != 0) {
        std::cerr << "Failed to get address info: " << gai_strerror(error) << std::endl;
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    
    error = bind(socket_fd, res->ai_addr, res->ai_addrlen);
    if (error == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        freeaddrinfo(res);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * UDP server thread
 * UDP will be used for managing users and listing users' activities.
 * @param arg: port
 * @return nullptr
 */
void* udp_server_thread(void* arg) {
    char* port = (char*)arg;
    int udp_socket_fd;
    struct addrinfo udp_hints, *udp_res;
    struct sockaddr_in client_addr;

    std::cout << "[UDP] Initializing UDP server on port " << port << std::endl;
    init_udp_server(port, udp_socket_fd, udp_hints, udp_res);

    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_LENGTH];
    int bytes_read;

    while (true) {
        bytes_read = recvfrom(udp_socket_fd, buffer, sizeof(buffer) - 1, 0,
                              (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_read == -1) {
            std::cerr << "[UDP] Failed to receive message" << std::endl;
            continue;
        }

        buffer[bytes_read] = '\0';
        std::cout << "[UDP] Received message: " << buffer << std::endl;
        
        const char* response = "Message received";
        sendto(udp_socket_fd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_len);
    }

    freeaddrinfo(udp_res);
    close(udp_socket_fd);
    return nullptr;
}

