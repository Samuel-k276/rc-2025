#include "tcp_server.h"
#include "user.h"
#include "events.h"
#include "../common/input.h"
#include "../common/commands.h"
#include "../common/constants.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>



void create(std::string uid,std::string password, std::string name,std::string event_date,
            std::string attendance_size,std::string fname,std::string fsize,std::string fdata,
            int client_fd) {
    if (!is_user_logged_in(uid)) {
        send(client_fd, "RCE NLG\n", 8, 0);
        return;
    }

    if (get_user(uid).password != password) {
        send(client_fd, "RCE WRP\n", 8, 0);
        return;
    }

    if (!is_name_valid(name) || !is_date_time_valid(event_date) ||
    !is_attendance_size_valid(attendance_size) || !is_fname_valid(fname) ||
    !is_fsize_valid(fsize) || !space_for_new_event()) {
        send(client_fd, "RCE NOK\n", 8, 0);
        return;
    }

    std::string message = add_event(uid,name,fname, event_date, stoi(attendance_size));
    send(client_fd, message.c_str(), message.length(), 0);
    return;
}

void change_pass(std::string uid, std::string old_password, std::string new_password, int client_fd) {
    if (!is_user_logged_in(uid)) {
        send(client_fd, "RCP NLG\n", 8, 0);
        return;
    }

    if (!is_user_registered(uid)) {
        send(client_fd, "RCP NID\n", 8, 0);
        return;
    }

    if (get_user(uid).password != old_password) {
        send(client_fd, "RCP NOK\n", 8, 0);
        return;
    }

    change_user_password(uid, new_password);
    send(client_fd, "RCP OK\n", 7, 0);
    return;
}

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
 * Handle TCP client connection
 * TCP will be used to transfer files with more information to the user application,
 * and managing events and reservations.
 * Reads message, sends response, and closes connection.
 * @param client_fd: client file descriptor
 */
void handle_tcp_client(int client_fd) {
    char buffer[MAX_MESSAGE_LENGTH];
    int bytes_read;

    bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        std::cerr << "[TCP] Failed to read from client" << std::endl;
        close(client_fd);
        return;
    }

    if (bytes_read == 0) {
        // Client closed connection before sending data
        std::cout << "[TCP] Client closed connection (fd: " << client_fd << ")" << std::endl;
        close(client_fd);
        return;
    }

    buffer[bytes_read] = '\0';
    std::cout << "[TCP] Received message: " << buffer << std::endl;
    
    std::string message = std::string(buffer);

    const CommandType command_type = get_command_type(message.substr(0, CMD_LENGTH));
    switch (command_type) {
        case CREATE_EVENT:
            if (!is_valid_create_command(message)) {
                std::cerr << "[TCP] Invalid create command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            {
                std::stringstream ss(message);
                std::string command;
                std::string uid;
                std::string password;
                std::string name;
                std::string event_date;
                std::string attendance_size;
                std::string fname;
                std::string fsize;
                std::string fdata;

                ss >> command >> uid >> password >> name >> event_date >> attendance_size >> fname >> fsize >> fdata;
                create(uid, password, name, event_date, attendance_size,
                        fname, fsize, fdata, client_fd);
            }
            break;
        case CLOSE_EVENT:
            break;
        case LIST_EVENTS:
            break;
        case SHOW_EVENT_DETAILS:
            break;
        case RESERVE:
            break;
        case CHANGE_PASS:
            if (!is_valid_change_pass_command(message)) {
                std::cerr << "[TCP] Invalid change pass command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            {
                std::stringstream ss(message);
                std::string command;
                std::string uid;
                std::string old_password;
                std::string new_password;
                
                ss >> command >> uid >> old_password >> new_password;
                change_pass(uid, old_password, new_password, client_fd);
            }
            break;
        default:
            std::cerr << "[TCP] Invalid command: " << message << std::endl;
            send(client_fd, "ERR\n", 4, 0);
            break;
    }
    message.pop_back();
    std::cout << "[TCP] Received message: " << message << std::endl;
    
    // Close connection after sending response
    close(client_fd);
    std::cout << "[TCP] Connection closed (fd: " << client_fd << ")" << std::endl;
}
