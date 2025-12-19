#include "tcp_server.h"
#include "../common/commands.h"
#include "../common/constants.h"
#include "../common/input.h"
#include "../common/utils.h"
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

    std::istringstream iss(message);
    std::string status, ok, eid_str;
    iss >> status >> ok >> eid_str;
    int eid = std::stoi(eid_str);

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
    std::vector<int> eids = get_all_event_ids();
    bool has_valid_events = false;
    for (int eid : eids) {
        Event e = load_event_from_disk(eid);
        if (!e.owner_uid.empty()) {
            has_valid_events = true;
            break;
        }
    }
    
    if (!has_valid_events) {
        send(client_fd, "RLS NOK\n", 8, 0);
        return;
    }
    
    std::string message = list_events();
    send(client_fd, message.c_str(), message.length(), 0);
    return;
}

void show_event_details(int eid, int client_fd) {
    if (!event_exist(eid)) {
        send(client_fd, "RSE NOK\n", 8, 0);
        return;
    }

    Event e = load_event_from_disk(eid);
    if (e.owner_uid.empty()) {
        send(client_fd, "RSE NOK\n", 8, 0);
        return;
    }

    std::string fdata = load_event_description_file(eid, e.file_name);
    if (fdata.empty()) {
        send(client_fd, "RSE NOK\n", 8, 0);
        return;
    }

    std::string fsize = std::to_string(fdata.length());
    std::string message = "RSE OK " + e.owner_uid + " " + e.name + " " + e.date_time + " " +
                          std::to_string(e.total_seats) + " " + std::to_string(e.reserved_seats) + " " +
                          e.file_name + " " + fsize + " " + fdata + "\n";
    
    send(client_fd, message.c_str(), message.length(), 0);
    return;
}

void reserve(std::string uid, std::string password, std::string eid, std::string number_of_people, int client_fd) {
    if (!event_exist(stoi(eid))) {
        send(client_fd, "RRI NOK\n", 8, 0);
        return;
    }
    if (!is_user_logged_in(uid)) {
        send(client_fd, "RRI NLG\n", 8, 0);
        return;
    }
    if (get_user(uid).password != password) {
        send(client_fd, "RRI WRP\n", 8, 0);
        return;
    }
    int state = get_event_status(stoi(eid));
    if (state == 2) {
        send(client_fd, "RRI SLD\n", 8, 0);
        return;
    }
    if (state == 0) {
        send(client_fd, "RRI PST\n", 8, 0);
        return;
    }
    if (state == 3) {
        send(client_fd, "RRI CLO\n", 8, 0);
        return;
    }
    if (!enough_seats(stoi(eid), stoi(number_of_people))) {
        Event e = load_event_from_disk(stoi(eid));
        int reserved_seats = load_event_reserved_seats(stoi(eid));
        int remaining_seats = e.total_seats - reserved_seats;
        std::string message = "RRI REJ " + std::to_string(remaining_seats) + "\n";
        send(client_fd, message.c_str(), message.length(), 0);
        return;
    }

    add_reservation(uid, stoi(eid), stoi(number_of_people));
    send(client_fd, "RRI ACC\n", 8, 0);
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
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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

void handle_tcp_client(int client_fd, bool verbose, struct sockaddr_in &client_addr) {
    // Read complete message - may need multiple reads for large messages (e.g., CREATE with file data)
    std::string message;
    char buffer[4096];
    int total_bytes = 0;
    bool found_newline = false;

    while (!found_newline && total_bytes < MAX_TCP_MESSAGE_LENGTH) {
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            std::cerr << "[TCP] Failed to read from client" << std::endl;
            close(client_fd);
            return;
        }

        if (bytes_read == 0) {
            // Client closed connection
            if (message.empty()) {
                close(client_fd);
                return;
            }
            break; // Use what we have
        }

        buffer[bytes_read] = '\0';
        message += std::string(buffer, bytes_read);
        total_bytes += bytes_read;

        // Check if we received the newline terminator
        if (message.find('\n') != std::string::npos) {
            found_newline = true;
        }
    }

    if (total_bytes >= MAX_TCP_MESSAGE_LENGTH && !found_newline) {
        std::cerr << "[TCP] Message too large or incomplete" << std::endl;
        send(client_fd, "ERR\n", 4, 0);
        close(client_fd);
        return;
    }
    
    // Trim trailing whitespace and newlines
    while (!message.empty() && (message.back() == '\n' || message.back() == '\r' || message.back() == ' ')) {
        message.pop_back();
    }

    const std::string command_code = message.substr(0, CMD_LENGTH);
    const CommandType command_type = get_command_type(command_code);
    std::string uid = "";
    
    switch (command_type) {
        case CREATE_EVENT: {
            std::string password, name, event_date, attendance_size, fname, fsize, fdata;
            if (parse_create_command(message, uid, password, name, event_date, attendance_size, fname, fsize, fdata)) {
                print_verbose_message(uid, command_code, client_addr, verbose);
                create(uid, password, name, event_date, attendance_size, fname, fsize, fdata, client_fd);
            } else {
                std::cerr << "[TCP] Invalid create command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        }
        case CLOSE_EVENT: {
            std::string password, eid;
            if (parse_close_command(message, uid, password, eid)) {
                print_verbose_message(uid, command_code, client_addr, verbose);
                close(uid, password, eid, client_fd);
            } else {
                std::cerr << "[TCP] Invalid close command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        }
        case LIST_EVENTS:
            if (parse_list_command(message)) {
                print_verbose_message("-", command_code, client_addr, verbose);
                list(client_fd);
            } else {
                std::cerr << "[TCP] Invalid list command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        case SHOW_EVENT_DETAILS: {
            std::string eid;
            if (parse_show_event_details_command(message, eid)) {
                print_verbose_message("-", command_code, client_addr, verbose);
                show_event_details(stoi(eid), client_fd);
            } else {
                std::cerr << "[TCP] Invalid show event details command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        }
        case RESERVE: {
            std::string password, eid, number_of_people;
            if (parse_reserve_command(message, uid, password, eid, number_of_people)) {
                print_verbose_message(uid, command_code, client_addr, verbose);
                reserve(uid, password, eid, number_of_people, client_fd);
            } else {
                std::cerr << "[TCP] Invalid reserve command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        }
        case CHANGE_PASS: {
            std::string old_password, new_password;
            if (parse_change_pass_command(message, uid, old_password, new_password)) {
                print_verbose_message(uid, command_code, client_addr, verbose);
                change_pass(uid, old_password, new_password, client_fd);
            } else {
                std::cerr << "[TCP] Invalid change pass command: " << message << std::endl;
                send(client_fd, "ERR\n", 4, 0);
            }
            break;
        }
        default:
            std::cerr << "[TCP] Invalid command: " << message << std::endl;
            send(client_fd, "ERR\n", 4, 0);
            break;
    }

    shutdown(client_fd, SHUT_WR);
    close(client_fd);
}
