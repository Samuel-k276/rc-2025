#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tcp_server.h"
#include "udp_server.h"

/**
 * Server port
 * Group 11 + 50000 is the default port for the server.
 */
char *port = (char *)"58011";

/**
 * Verbose mode
 * It outputs to the screen a short description of the received requests
 * (UID, type of request), and the IP and port originating those requests.
 */
bool verbose = false;

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

    // Initialize TCP server
    int tcp_socket_fd;
    struct addrinfo tcp_hints, *tcp_res;
    std::cout << "[TCP] Initializing TCP server on port " << port << std::endl;
    init_tcp_server(port, tcp_socket_fd, tcp_hints, tcp_res);

    // Initialize UDP server
    int udp_socket_fd;
    struct addrinfo udp_hints, *udp_res;
    std::cout << "[UDP] Initializing UDP server on port " << port << std::endl;
    init_udp_server(port, udp_socket_fd, udp_hints, udp_res);

    fd_set read_fds;
    int max_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    std::cout << "Server ready. Waiting for connections..." << std::endl;

    while (true) {
        // Clear and set file descriptors
        FD_ZERO(&read_fds);
        FD_SET(tcp_socket_fd, &read_fds);
        FD_SET(udp_socket_fd, &read_fds);
        max_fd = (tcp_socket_fd > udp_socket_fd) ? tcp_socket_fd : udp_socket_fd;

        // Wait for activity on any socket
        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "select error" << std::endl;
            continue;
        }

        // Check for new TCP connection
        if (FD_ISSET(tcp_socket_fd, &read_fds)) {
            int new_client_fd = accept(tcp_socket_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_client_fd == -1) {
                std::cerr << "[TCP] Failed to accept connection" << std::endl;
                continue;
            }

            std::cout << "[TCP] New client connected (fd: " << new_client_fd << ")" << std::endl;
            // Handle client immediately: read, respond, and close
            handle_tcp_client(new_client_fd);
        }

        // Check for UDP message
        if (FD_ISSET(udp_socket_fd, &read_fds)) {
            handle_udp_message(udp_socket_fd);
        }
    }

    // Cleanup (unreachable in normal operation)
    freeaddrinfo(tcp_res);
    freeaddrinfo(udp_res);
    close(tcp_socket_fd);
    close(udp_socket_fd);

    return 0;
}
