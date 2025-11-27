#include <arpa/inet.h>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/commands.h"
#include "../common/constants.h"

int socket_fd;
struct addrinfo hints, *res;

char *ESIP = (char *)"127.0.0.1";
char *ESport = (char *)"58011"; // Group 11 + 50000

int main(int argc, char *argv[]) {
    int opt;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[MAX_MESSAGE_LENGTH];
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

    // socket_fd = socket(AF_INET, SOCK_STREAM, 0); TCP
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    // hints.ai_socktype = SOCK_STREAM; // TCP socket

    int error = getaddrinfo(ESIP, ESport, &hints, &res);
    if (error != 0) {
        std::cerr << "Failed to get address info: " << gai_strerror(error) << std::endl;
        exit(EXIT_FAILURE);
    }
    /*
        int n = connect(socket_fd, res->ai_addr, res->ai_addrlen);
        if (n == -1) {
            std::cerr << "Failed to connect to server" << std::endl;
            exit(EXIT_FAILURE);
        }*/ //TCP

    while (true) {
        fgets(buffer, sizeof(buffer), stdin);
        std::string message = std::string(buffer);
        const CommandType command_type = get_command_type(message.substr(0, CMD_LENGTH));
        switch (command_type) {
            case LOGIN:
                if (!is_valid_login_command(message)) {
                    std::cerr << "Invalid login command: " << message << std::endl;
                    continue;
                }
                break;
            case LOGOUT:
                if (!is_valid_logout_command(message)) {
                    std::cerr << "Invalid logout command: " << message << std::endl;
                    continue;
                }
                break;
            case INVALID_COMMAND:
                std::cerr << "Invalid command: " << message << std::endl;
                continue;
            default:
                std::cerr << "Invalid command: " << message << std::endl;
                continue;
        }

        n = sendto(socket_fd, buffer, strlen(buffer) - 1, 0, res->ai_addr, res->ai_addrlen);
        if (n == -1) {
            perror("sendto");
            exit(1);
        }
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }

        addrlen = sizeof(addr);
        n = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen);
        if (n == -1) {
            perror("recvfrom");
            exit(1);
        }
        write(1, "Echo from server: ", 18);
        write(1, buffer, n);
    }

    freeaddrinfo(res);
    close(socket_fd);
    return 0;
}