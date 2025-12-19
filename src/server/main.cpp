#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "storage.h"
#include "tcp_server.h"
#include "udp_server.h"

char *port = (char *)"58011"; // default port for group 11
bool verbose = false; // when enabled, prints request details (UID, command, IP, port)

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

    init_storage();

    int tcp_socket_fd;
    struct addrinfo tcp_hints, *tcp_res;
    init_tcp_server(port, tcp_socket_fd, tcp_hints, tcp_res);

    int udp_socket_fd;
    struct addrinfo udp_hints, *udp_res;
    init_udp_server(port, udp_socket_fd, udp_hints, udp_res);

    fd_set read_fds;
    int max_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(tcp_socket_fd, &read_fds);
        FD_SET(udp_socket_fd, &read_fds);
        max_fd = (tcp_socket_fd > udp_socket_fd) ? tcp_socket_fd : udp_socket_fd;

        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "select error" << std::endl;
            continue;
        }

        if (FD_ISSET(tcp_socket_fd, &read_fds)) {
            int new_client_fd = accept(tcp_socket_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_client_fd == -1) {
                std::cerr << "[TCP] Failed to accept connection" << std::endl;
                continue;
            }
            handle_tcp_client(new_client_fd, verbose, client_addr);
        }

        if (FD_ISSET(udp_socket_fd, &read_fds)) {
            handle_udp_message(udp_socket_fd, verbose);
        }
    }

    freeaddrinfo(tcp_res);
    freeaddrinfo(udp_res);
    close(tcp_socket_fd);
    close(udp_socket_fd);

    return 0;
}
