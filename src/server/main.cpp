#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include "../common/constants.h"
#include "tcp_server.h"
#include "udp_server.h"

// Server settings
char *port = (char *)"58011"; // Group 11 + 50000
bool verbose = false;

// Thread function for TCP server
void* tcp_server_thread(void* arg) {
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

// Thread function for UDP server
void* udp_server_thread(void* arg) {
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

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:v")) != -1) {
        switch (opt) {
            case 'p':
                port = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-p <ESport>] [-v]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Server is running on port " << port << std::endl;
    std::cout << "Verbose mode: " << (verbose ? "on" : "off") << std::endl;

    // Create threads for TCP and UDP servers
    pthread_t tcp_thread, udp_thread;

    if (pthread_create(&tcp_thread, nullptr, tcp_server_thread, nullptr) != 0) {
        std::cerr << "Failed to create TCP thread" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&udp_thread, nullptr, udp_server_thread, nullptr) != 0) {
        std::cerr << "Failed to create UDP thread" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Wait for threads to complete (they run indefinitely)
    pthread_join(tcp_thread, nullptr);
    pthread_join(udp_thread, nullptr);

    return 0;
}
