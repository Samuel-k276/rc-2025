#include "commands.h"
#include "input.h"
#include <cstdbool>
#include <iostream>
#include <sstream>
#include <string>

#include "../common/constants.h"

const int space_and_newline_length = 3;

bool is_valid_command_udp(std::string command) { return CommandUDPMap.find(command) != CommandUDPMap.end(); }

bool is_valid_command_tcp(std::string command) { return CommandTCPMap.find(command) != CommandTCPMap.end(); }

bool is_valid_command(std::string command) { return is_valid_command_udp(command) || is_valid_command_tcp(command); }

CommandType get_command_type(std::string  command) {
    if (command.empty() || command.length() != CMD_LENGTH) {
        return INVALID_COMMAND;
    }

    if (is_valid_command_udp(command)) {
        return CommandUDPMap.at(command);
    } else if (is_valid_command_tcp(command)) {
        return CommandTCPMap.at(command);
    }
    return INVALID_COMMAND;
}

bool is_valid_login_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    if (!is_uid_valid(uid)) {
        std::cerr << "Invalid UID: " << uid << std::endl;
        return false;
    }

    if (!is_password_valid(password)) {
        std::cerr << "Invalid password: " << password << std::endl;
        return false;
    }

    return true;
}

bool is_valid_logout_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LOGOUT && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_unregister_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == UNREGISTER && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_list_my_events_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LIST_MY_EVENTS && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_list_my_reservations_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LIST_MY_RESERVATIONS && is_uid_valid(uid) && is_password_valid(password);
}