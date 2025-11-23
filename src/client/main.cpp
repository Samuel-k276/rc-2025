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

int socket_fd;
struct addrinfo hints, *res;

int main(int argc, char *argv[]) {
    char *ESIP = (char *)"127.0.0.1";
    char *ESport = (char *)"58011"; // Group 11 + 50000
    int opt;

    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                ESIP = optarg;
                break;
            case 'p':
                ESport = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-n <ESIP>] [-p <ESport>]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Connecting to server at " << ESIP << ":" << ESport << std::endl;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    int error = getaddrinfo(ESIP, ESport, &hints, &res);
    if (error != 0) {
        std::cerr << "Failed to get address info: " << gai_strerror(error) << std::endl;
        exit(EXIT_FAILURE);
    }

    int n = connect(socket_fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        exit(EXIT_FAILURE);
    }

    write(socket_fd, "Hello, server!", 13);

    freeaddrinfo(res);
    close(socket_fd);
    return 0;
}