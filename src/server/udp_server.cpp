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
#include "user.h"
#include "events.h"

/**
 * Login a user, responds with "RLI <status>"
 * If the user is not registered, it will be registered and respond with status=REG.
 * If the user is registered and the password is correct, it will respond with status=OK.
 * If the user is registered and the password is incorrect, it will respond with status=NOK.
 * @param uid: user ID
 * @param password: user password
 * @param socket_fd: socket file descriptor
 * @param client_addr: client address
 * @param addr_len: client address length
 */
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

/**
 * Logout a user, responds with "RLO <status>"
 * If the user is not registered, it will respond with status=UNR.
 * If the user is not logged in, it will respond with status=NOK.
 * If the password is incorrect, it will respond with status=WRP.
 * If the user is registered and logged in and the password is correct, it will respond with status=OK.
 * @param uid: user ID
 * @param socket_fd: socket file descriptor
 * @param client_addr: client address
 * @param addr_len: client address length
 */
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
/**
 * Unregister a user, responds with "RUR <status>"
 * If the user is not registered, it will respond with status=UNR.
 * If the user is not logged in, it will respond with status=NOK.
 * If the password is incorrect, it will respond with status=WRP.
 * If the user is registered and logged in and the password is correct, it will respond with status=OK.
 * @param uid: user ID
 * @param socket_fd: socket file descriptor
 * @param client_addr: client address
 * @param addr_len: client address length
 */
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

void list_my_events(std::string uid_str, int &socket_fd, struct sockaddr_in &client_addr,
                socklen_t &addr_len) {

    if (!is_user_logged_in(uid_str)) {
        sendto(socket_fd, "RME NLG\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    int uid = stoi(uid_str);
    if (!uid_has_events(uid)) {
        sendto(socket_fd, "RME NOK\n", 8, 0, (struct sockaddr *)&client_addr, addr_len);
        return;
    }

    //TODO:
    //get_user_events();
                }
/**
 * Init UDP server
 * @param port: port
 * @param socket_fd: socket file descriptor
 * @param hints: hints
 * @param res: result
 */

void init_udp_server(char *port, int &socket_fd, struct addrinfo &hints, struct addrinfo *&res) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
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
void *udp_server_thread(void *arg) {
    char *port = (char *)arg;
    int udp_socket_fd;
    struct addrinfo udp_hints, *udp_res;
    struct sockaddr_in client_addr;

    std::cout << "[UDP] Initializing UDP server on port " << port << std::endl;
    init_udp_server(port, udp_socket_fd, udp_hints, udp_res);

    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_LENGTH];
    int n;

    while (true) {
        n = recvfrom(udp_socket_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n == -1) {
            std::cerr << "[UDP] Failed to receive message" << std::endl;
            continue;
        }
        buffer[n] = '\0';
        std::string message = std::string(buffer);

        const CommandType command_type = get_command_type(message.substr(0, CMD_LENGTH));
        switch (command_type) {
            case LOGIN:
                if (!is_valid_login_command(message)) {
                    std::cerr << "[UDP] Invalid login command: " << message << std::endl;
                    n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                    break;
                }
                {
                    std::stringstream ss(message);
                    std::string command, uid, password;
                    ss >> command >> uid >> password;
                    login(uid, password, udp_socket_fd, client_addr, addr_len);
                }
                break;
            case LOGOUT:
                if (!is_valid_logout_command(message)) {
                    std::cerr << "[UDP] Invalid logout command: " << message << std::endl;
                    n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                    break;
                }
                {
                    std::stringstream ss(message);
                    std::string command, uid, password;
                    ss >> command >> uid >> password;
                    logout(uid, password, udp_socket_fd, client_addr, addr_len);
                }
                break;
            case UNREGISTER:
                if (!is_valid_unregister_command(message)) {
                    std::cerr << "[UDP] Invalid unregister command: " << message << std::endl;
                    n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                    break;
                }
                {
                    std::stringstream ss(message);
                    std::string command, uid, password;
                    ss >> command >> uid >> password;
                    unregister(uid,password, udp_socket_fd, client_addr, addr_len);
                }
                if (n == -1) {
                    perror("sendto");
                    exit(1);
                }
                break;
            case LIST_MY_EVENTS:
                if (!is_valid_list_my_events_command(message)) {
                    std::cerr << "[UDP] Invalid list my events command: " << message << std::endl;
                    n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                    break;
                }
                {
                    std::stringstream ss(message);
                    std::string command, uid, password;
                    ss >> command >> uid >> password;
                    list_my_events(uid, udp_socket_fd, client_addr, addr_len);
                }
                if (n == -1) {
                    perror("sendto");
                    exit(1);
                }
                break;
            case LIST_MY_RESERVATIONS:
                if (!is_valid_list_my_reservations_command(message)) {
                    std::cerr << "[UDP] Invalid list my reservations command: " << message << std::endl;
                    n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                    break;
                }
                n = sendto(udp_socket_fd, "LMR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                if (n == -1) {
                    perror("sendto");
                    exit(1);
                }
                break;
            case INVALID_COMMAND:
            default:
                std::cerr << "[UDP] Invalid command: " << message << std::endl;
                n = sendto(udp_socket_fd, "ERR\n", 4, 0, (struct sockaddr *)&client_addr, addr_len);
                if (n == -1) {
                    perror("sendto");
                    exit(1);
                }
                break;
        }
        message.pop_back();
        std::cout << "[UDP] Received message: " << message << std::endl;
    }

    freeaddrinfo(udp_res);
    close(udp_socket_fd);
    return nullptr;
}
