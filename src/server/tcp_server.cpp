#include "tcp_server.h"

#include "../common/constants.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

void init_tcp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
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

    error = listen(socket_fd, 10);
    if (error == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        freeaddrinfo(res);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * TCP server thread
 * TCP will be used to transfer files with more information to the user application,
 * and managing events and reservations.
 * @param arg: port
 * @return nullptr
 */
void *tcp_server_thread(void *arg) {
    char *port = (char *)arg;
    int tcp_socket_fd;
    struct addrinfo tcp_hints, *tcp_res;
    struct sockaddr_in client_addr;

    std::cout << "[TCP] Initializing TCP server on port " << port << std::endl;
    init_tcp_server(port, tcp_socket_fd, tcp_hints, tcp_res);

    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_LENGTH];
    int bytes_read;

    while (true) {
        int client_fd = accept(tcp_socket_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            std::cerr << "[TCP] Failed to accept connection" << std::endl;
            continue;
        }

        bytes_read = read(client_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            std::cerr << "[TCP] Failed to read from client" << std::endl;
            close(client_fd);
            continue;
        }

        buffer[bytes_read] = '\0';
        std::cout << "[TCP] Received message: " << buffer << std::endl;
        write(client_fd, "Message received", 16);
        close(client_fd);
    }

    freeaddrinfo(tcp_res);
    close(tcp_socket_fd);
    return nullptr;
}
