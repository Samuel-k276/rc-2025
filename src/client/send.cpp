#include "send.h"
#include "../common/constants.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool send_tcp_command(std::string &buffer, struct addrinfo *res, std::string &response) {
    // Create a new TCP socket for each command (since we close it after use)
    int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd == -1) {
        std::cerr << "Failed to create TCP socket" << std::endl;
        return false;
    }

    ssize_t n = connect(tcp_fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to connect TCP command to server" << std::endl;
        close(tcp_fd);
        return false;
    }
    n = write(tcp_fd, buffer.c_str(), buffer.length());
    if (n == -1) {
        std::cerr << "Failed to send TCP command to server" << std::endl;
        close(tcp_fd);
        return false;
    }
    
    // Read complete response - may need multiple reads for large messages (e.g., SHOW with file data)
    response.clear();
    char response_buffer[4096];
    int total_bytes = 0;
    bool found_newline = false;

    while (!found_newline && total_bytes < MAX_TCP_MESSAGE_LENGTH) {
        n = read(tcp_fd, response_buffer, sizeof(response_buffer) - 1);
        if (n == -1) {
            std::cerr << "Failed to receive TCP command from server" << std::endl;
            close(tcp_fd);
            return false;
        }

        if (n == 0) {
            // Server closed connection
            break;
        }

        response_buffer[n] = '\0';
        response += std::string(response_buffer, n);
        total_bytes += n;

        // Check if we received the newline terminator
        if (response.find('\n') != std::string::npos) {
            found_newline = true;
        }
    }

    if (total_bytes >= MAX_TCP_MESSAGE_LENGTH && !found_newline) {
        std::cerr << "Response too large or incomplete" << std::endl;
        close(tcp_fd);
        return false;
    }

    close(tcp_fd);
    return true;
}

bool send_udp_command(int socket_fd, std::string &message, struct addrinfo *res, std::string &response) {
    struct sockaddr_in addr;

    int n = sendto(socket_fd, message.c_str(), message.length(), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        std::cerr << "Failed to send UDP command to server" << std::endl;
        return false;
    }

    socklen_t addrlen = sizeof(addr);
    char response_buffer[4096];
    n = recvfrom(socket_fd, response_buffer, sizeof(response_buffer), 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1) {
        std::cerr << "Failed to receive UDP command from server" << std::endl;
        return false;
    }
    response_buffer[n] = '\0';
    response = std::string(response_buffer);
    return true;
}
