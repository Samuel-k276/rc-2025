#include "udp_server.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>

#include "../common/commands.h"
#include "../common/constants.h"
#include "events.h"
#include "input_handler.h"
#include "user.h"

void login(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
           socklen_t &addr_len) {
    if (!is_user_registered(uid)) {
        add_user(uid, password);
        sendto(socket_fd, "RLI REG\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (get_user(uid).password != password) {
        sendto(socket_fd, "RLI NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    login_user(uid);
    sendto(socket_fd, "RLI OK\n", 7, 0, (struct sockaddr *)&client_addr, addr_len);
    return;
}

void logout(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
            socklen_t &addr_len) {
    if (!is_user_registered(uid)) {
        sendto(socket_fd, "RLO UNR\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (!is_user_logged_in(uid)) {
        sendto(socket_fd, "RLO NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (get_user(uid).password != password) {
        sendto(socket_fd, "RLO WRP\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    logout_user(uid);
    sendto(socket_fd, "RLO OK\n", 7, 0, (struct sockaddr *)&client_addr, addr_len);
    return;
}
void unregister(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
                socklen_t &addr_len) {
    if (!is_user_registered(uid)) {
        sendto(socket_fd, "UNR UNR\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (!is_user_logged_in(uid)) {
        sendto(socket_fd, "UNR NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (get_user(uid).password != password) {
        sendto(socket_fd, "UNR WRP\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    logout_user(uid);
    remove_user(uid);
    sendto(socket_fd, "UNR OK\n", 7, 0, (struct sockaddr *)&client_addr, addr_len);
    return;
}

void list_my_events(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
                    socklen_t &addr_len) {

    if (!is_user_logged_in(uid)) {
        sendto(socket_fd, "RME NLG\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (get_user(uid).password != password) {
        sendto(socket_fd, "RME WRP\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (!uid_has_events(uid)) {
        sendto(socket_fd, "RME NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    std::string message = get_user_events(uid);
    sendto(socket_fd, message.c_str(), message.length(), 0, (struct sockaddr *)&client_addr, addr_len);
    return;
}

void list_my_reservations(std::string uid, std::string password, int &socket_fd, struct sockaddr_in &client_addr,
                          socklen_t &addr_len) {

    if (!is_user_logged_in(uid)) {
        sendto(socket_fd, "RMR NLG\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (get_user(uid).password != password) {
        sendto(socket_fd, "RMR WRP\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    if (!uid_has_reservations(uid)) {
        sendto(socket_fd, "RMR NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    std::string message = get_user_reservations(uid);
    sendto(socket_fd, message.c_str(), message.length(), 0, (struct sockaddr *)&client_addr, addr_len);
    return;
}

void init_udp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
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

void handle_udp_message(int socket_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_LENGTH];
    int n;

    n = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (n == -1) {
        std::cerr << "[UDP] Failed to receive message" << std::endl;
        return;
    }
    buffer[n] = '\0';
    std::string message = std::string(buffer);

    const CommandType command_type = get_command_type(message.substr(0, CMD_LENGTH));
    switch (command_type) {
        case LOGIN: {
            std::string uid, password;
            if (!parse_login_command(message, uid, password)) {
                std::cerr << "[UDP] Invalid login command: " << message << std::endl;
                sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                break;
            }
            login(uid, password, socket_fd, client_addr, addr_len);
            break;
        }
        case LOGOUT: {
            std::string uid, password;
            if (!parse_logout_command(message, uid, password)) {
                std::cerr << "[UDP] Invalid logout command: " << message << std::endl;
                sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                break;
            }
            logout(uid, password, socket_fd, client_addr, addr_len);
            break;
        }
        case UNREGISTER: {
            std::string uid, password;
            if (!parse_unregister_command(message, uid, password)) {
                std::cerr << "[UDP] Invalid unregister command: " << message << std::endl;
                sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                break;
            }
            unregister(uid, password, socket_fd, client_addr, addr_len);
            break;
        }
        case LIST_MY_EVENTS: {
            std::string uid, password;
            if (!parse_list_my_events_command(message, uid, password)) {
                std::cerr << "[UDP] Invalid list my events command: " << message << std::endl;
                sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                break;
            }
            list_my_events(uid, password, socket_fd, client_addr, addr_len);
            break;
        }
        case LIST_MY_RESERVATIONS: {
            std::string uid, password;
            if (!parse_list_my_reservations_command(message, uid, password)) {
                std::cerr << "[UDP] Invalid list my reservations command: " << message << std::endl;
                sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                break;
            }
            list_my_reservations(uid, password, socket_fd, client_addr, addr_len);
            break;
        }
        case INVALID_COMMAND:
        default:
            std::cerr << "[UDP] Invalid command: " << message << std::endl;
            sendto(socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
            break;
    }
    message.pop_back();
    std::cout << "[UDP] Received message: " << message << std::endl;
}
