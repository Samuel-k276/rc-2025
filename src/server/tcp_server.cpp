#include "tcp_server.h"
#include "../common/commands.h"
#include "../common/constants.h"
#include "../common/input.h"
#include "events.h"
#include "input_handler.h"
#include "storage.h"
#include "user.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <unistd.h>

void create(std::string uid, std::string password, std::string name, std::string event_date,
            std::string attendance_size, std::string fname, std::string fsize, std::string fdata, int client_fd) {
    if (!is_user_logged_in(uid)) {
        send(client_fd, "RCE NLG\n", 8, 0);
        return;
    }

    if (get_user(uid).password != password) {
        send(client_fd, "RCE WRP\n", 8, 0);
        return;
    }

    if (!is_name_valid(name) || !is_date_time_valid(event_date) || !is_attendance_size_valid(attendance_size) ||
        !is_fname_valid(fname) || !is_fsize_valid(fsize) || !space_for_new_event()) {
        send(client_fd, "RCE NOK\n", 8, 0);
        return;
    }

    std::string message = add_event(uid, name, fname, event_date, stoi(attendance_size));

    // Extract EID from response message (format: "RCE OK 001")
    std::istringstream iss(message);
    std::string status, ok, eid_str;
    iss >> status >> ok >> eid_str;
    int eid = std::stoi(eid_str);

    // Save event description file
    // fdata is passed as parameter and may contain binary data
    save_event_description_file(eid, fname, fdata);

    send(client_fd, message.c_str(), message.length(), 0);
    return;
}

void close(std::string uid, std::string password, std::string eid, int client_fd) {
    if (!is_user_logged_in(uid)) {
        send(client_fd, "RCL NLG\n", 8, 0);
        return;
    }
    if (get_user(uid).password != password || !is_user_registered(uid)) {
        send(client_fd, "RCL NOK\n", 8, 0);
        return;
    }
    if (!event_exist(stoi(eid))) {
        send(client_fd, "RCL NOE\n", 8, 0);
        return;
    }
    if (!owner_of_event(uid, stoi(eid))) {
        send(client_fd, "RCL EOW\n", 8, 0);
        return;
    }
    int state = get_event_status(stoi(eid));
    if (state == 2) {
        send(client_fd, "RCL SLD\n", 8, 0);
        return;
    }
    if (state == 0) {
        send(client_fd, "RCL PST\n", 8, 0);
        return;
    }
    if (state == 3) {
        send(client_fd, "RCL CLO\n", 8, 0);
        return;
    }

    close_event(stoi(eid));
    send(client_fd, "RCL OK\n", 7, 0);
    return;
}

void list(int client_fd) {
    if (!event_exist(1)) {
        send(client_fd, "RLS NOK\n", 8, 0);
        return;
    }
    std::string message = list_events();
    send(client_fd, message.c_str(), message.length(), 0);
    return;
}

void reserve(std::string uid, std::string password, std::string eid, std::string number_of_people, int client_fd) {
    if (!event_exist(stoi(eid))) {
        send(client_fd, "PRI NOK\n", 8, 0);
        return;
    }
    if (!is_user_logged_in(uid)) {
        send(client_fd, "PRI NLG\n", 8, 0);
        return;
    }
    if (get_user(uid).password != password) {
        send(client_fd, "PRI WRP\n", 8, 0);
        return;
    }
    int state = get_event_status(stoi(eid));
    if (state == 2) {
        send(client_fd, "PRI SLD\n", 8, 0);
        return;
    }
    if (state == 0) {
        send(client_fd, "PRI PST\n", 8, 0);
        return;
    }
    if (state == 3) {
        send(client_fd, "PRI CLO\n", 8, 0);
        return;
    }
    if (!enough_seats(stoi(eid), stoi(number_of_people))) {
        send(client_fd, "PRI REJ\n", 8, 0);
        return;
    }

    add_reservation(uid, stoi(eid), stoi(number_of_people));
    send(client_fd, "PRI ACC\n", 8, 0);
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
    std::string message = std::string(buffer);
    
    // Remove trailing newline and whitespace for logging
    while (!message.empty() && (message.back() == '\n' || message.back() == '\r' || message.back() == ' ')) {
        message.pop_back();
    }
    std::cout << "[TCP] Received message: " << message << std::endl;

    const CommandType command_type = get_command_type(message.substr(0, CMD_LENGTH));
    switch (command_type) {
        case CREATE_EVENT: {
            std::string uid, password, name, event_date, attendance_size, fname, fsize, fdata;
            if (!parse_create_command(message, uid, password, name, event_date, attendance_size, fname, fsize, fdata)) {
                std::cerr << "[TCP] Invalid create command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            create(uid, password, name, event_date, attendance_size, fname, fsize, fdata, client_fd);
            break;
        }
        case CLOSE_EVENT: {
            std::string uid, password, eid;
            if (!parse_close_command(message, uid, password, eid)) {
                std::cerr << "[TCP] Invalid close command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            close(uid, password, eid, client_fd);
            break;
        }
        case LIST_EVENTS:
            if (!parse_list_command(message)) {
                std::cerr << "[TCP] Invalid list command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            list(client_fd);
            break;
        case SHOW_EVENT_DETAILS:
            break;
        case RESERVE: {
            std::string uid, password, eid, number_of_people;
            if (!parse_reserve_command(message, uid, password, eid, number_of_people)) {
                std::cerr << "[TCP] Invalid reserve command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            reserve(uid, password, eid, number_of_people, client_fd);
            break;
        }
        case CHANGE_PASS: {
            std::string uid, old_password, new_password;
            if (!parse_change_pass_command(message, uid, old_password, new_password)) {
                std::cerr << "[TCP] Invalid change pass command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
                break;
            }
            change_pass(uid, old_password, new_password, client_fd);
            break;
        }
        default:
            std::cerr << "[TCP] Invalid command: " << message << std::endl;
            send(client_fd, "ERR\n", 4, 0);
            break;
    }

    // Shutdown write side to signal that we're done sending
    // This allows the client to detect EOF and close gracefully
    shutdown(client_fd, SHUT_WR);
    
    // Close connection after sending response
    close(client_fd);
    std::cout << "[TCP] Connection closed (fd: " << client_fd << ")" << std::endl;
}
