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

#include "../common/constants.h"
#include "tcp_server.h"

int socket_fd;
struct addrinfo hints, *res;
struct sockaddr_in client_addr;\

// Server settings
char *port = (char *)"58011"; // Group 11 + 50000
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

    init_tcp_server(port, socket_fd, hints, res);

    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_LENGTH];
    int bytes_read;
    while (true) {
        int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            exit(EXIT_FAILURE);
        }

        bytes_read = read(client_fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            std::cerr << "Failed to read from client" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Received message: " << buffer << std::endl;
        write(client_fd, "Message received", 16);
        close(client_fd);
    }

    freeaddrinfo(res);
    close(socket_fd);

    return 0;
}
